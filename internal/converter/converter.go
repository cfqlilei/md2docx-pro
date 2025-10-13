package converter

import (
	"fmt"
	"os"
	"os/exec"
	"path/filepath"
	"strings"
	"sync"

	"md2docx/internal/config"
	"md2docx/internal/models"
	"md2docx/pkg/utils"
)

// Converter 转换器
type Converter struct {
	config *config.Config
	mu     sync.RWMutex
}

// New 创建新的转换器
func New(cfg *config.Config) *Converter {
	return &Converter{
		config: cfg,
	}
}

// ConvertSingle 转换单个文件
func (c *Converter) ConvertSingle(req *models.ConversionRequest) (*models.ConversionResponse, error) {
	c.mu.RLock()
	defer c.mu.RUnlock()

	// 验证输入文件
	if err := utils.ValidateInputFile(req.InputFile); err != nil {
		return &models.ConversionResponse{
			Success: false,
			Error:   err.Error(),
		}, nil
	}

	// 验证输出目录
	if err := utils.ValidateOutputDir(req.OutputDir); err != nil {
		return &models.ConversionResponse{
			Success: false,
			Error:   err.Error(),
		}, nil
	}

	// 确定输出路径
	outputPath, err := utils.DetermineOutputPath(req.InputFile, req.OutputDir, req.OutputName)
	if err != nil {
		return &models.ConversionResponse{
			Success: false,
			Error:   fmt.Sprintf("确定输出路径失败: %v", err),
		}, nil
	}

	// 执行转换
	if err := c.convertFile(req.InputFile, outputPath, req.TemplateFile); err != nil {
		return &models.ConversionResponse{
			Success: false,
			Error:   fmt.Sprintf("转换失败: %v", err),
		}, nil
	}

	return &models.ConversionResponse{
		Success:    true,
		Message:    "转换成功",
		OutputFile: outputPath,
	}, nil
}

// ConvertBatch 批量转换文件
func (c *Converter) ConvertBatch(req *models.BatchConversionRequest) (*models.ConversionResponse, error) {
	c.mu.RLock()
	defer c.mu.RUnlock()

	if len(req.InputFiles) == 0 {
		return &models.ConversionResponse{
			Success: false,
			Error:   "输入文件列表不能为空",
		}, nil
	}

	// 验证输出目录
	if err := utils.ValidateOutputDir(req.OutputDir); err != nil {
		return &models.ConversionResponse{
			Success: false,
			Error:   err.Error(),
		}, nil
	}

	var results []models.ConversionResult
	var successCount int

	// 逐个处理文件
	for _, inputFile := range req.InputFiles {
		result := models.ConversionResult{
			InputFile: inputFile,
			Success:   false,
		}

		// 验证输入文件
		if err := utils.ValidateInputFile(inputFile); err != nil {
			result.Error = err.Error()
			results = append(results, result)
			continue
		}

		// 确定输出路径
		outputPath, err := utils.DetermineOutputPath(inputFile, req.OutputDir, "")
		if err != nil {
			result.Error = fmt.Sprintf("确定输出路径失败: %v", err)
			results = append(results, result)
			continue
		}

		// 执行转换
		if err := c.convertFile(inputFile, outputPath, req.TemplateFile); err != nil {
			result.Error = fmt.Sprintf("转换失败: %v", err)
			results = append(results, result)
			continue
		}

		result.Success = true
		result.OutputFile = outputPath
		results = append(results, result)
		successCount++
	}

	// 构建响应
	response := &models.ConversionResponse{
		Success: successCount > 0,
		Results: results,
	}

	if successCount == len(req.InputFiles) {
		response.Message = fmt.Sprintf("所有%d个文件转换成功", successCount)
	} else if successCount > 0 {
		response.Message = fmt.Sprintf("%d个文件转换成功，%d个文件转换失败", successCount, len(req.InputFiles)-successCount)
	} else {
		response.Message = "所有文件转换失败"
		response.Error = "批量转换失败"
	}

	return response, nil
}

// convertFile 执行单个文件的转换
func (c *Converter) convertFile(inputFile, outputFile, templateFile string) error {
	// 验证Pandoc配置
	if err := c.config.ValidatePandoc(); err != nil {
		return fmt.Errorf("Pandoc配置无效: %v", err)
	}

	// 构建Pandoc命令
	args := []string{
		inputFile,
		"-o", outputFile,
		"-f", "markdown",
		"-t", "docx",
		"--standalone",
		"--embed-resources", // 将图片等资源嵌入到输出文件中
	}

	// 添加资源路径参数，让pandoc能够找到相对路径的图片
	// 获取输入文件的目录作为资源根目录
	inputDir := filepath.Dir(inputFile)
	if inputDir != "." && inputDir != "" {
		// 添加输入文件目录和常见的图片目录到资源路径
		resourcePaths := []string{
			inputDir,                           // 输入文件所在目录
			filepath.Join(inputDir, "images"),  // images子目录
			filepath.Join(inputDir, "figures"), // figures子目录
			filepath.Join(inputDir, "pics"),    // pics子目录
			filepath.Join(inputDir, "assets"),  // assets子目录
		}

		// 检查哪些路径实际存在
		var existingPaths []string
		for _, path := range resourcePaths {
			if _, err := os.Stat(path); err == nil {
				existingPaths = append(existingPaths, path)
			}
		}

		// 如果有存在的路径，添加到pandoc参数中
		if len(existingPaths) > 0 {
			resourcePathArg := strings.Join(existingPaths, string(os.PathListSeparator))
			args = append(args, "--resource-path", resourcePathArg)
		}
	}

	// 如果指定了模板文件，添加模板参数
	if templateFile != "" {
		if err := utils.ValidateInputFile(templateFile); err == nil {
			args = append(args, "--reference-doc", templateFile)
		}
	} else if c.config.TemplateFile != "" {
		if err := c.config.ValidateTemplate(); err == nil {
			args = append(args, "--reference-doc", c.config.TemplateFile)
		}
	}

	// 执行Pandoc命令
	cmd := exec.Command(c.config.PandocPath, args...)
	output, err := cmd.CombinedOutput()
	if err != nil {
		return fmt.Errorf("Pandoc执行失败: %v, 输出: %s", err, string(output))
	}

	// 验证输出文件是否生成
	if !utils.FileExists(outputFile) {
		return fmt.Errorf("输出文件未生成: %s", outputFile)
	}

	return nil
}

// UpdateConfig 更新配置
func (c *Converter) UpdateConfig(cfg *config.Config) {
	c.mu.Lock()
	defer c.mu.Unlock()
	c.config = cfg
}
