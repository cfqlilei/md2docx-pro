package utils

import (
	"fmt"
	"os"
	"path/filepath"
	"strings"
)

// ValidateInputFile 验证输入文件
func ValidateInputFile(filePath string) error {
	if filePath == "" {
		return fmt.Errorf("输入文件路径不能为空")
	}

	// 检查文件是否存在
	if _, err := os.Stat(filePath); os.IsNotExist(err) {
		return fmt.Errorf("输入文件不存在: %s", filePath)
	}

	// 检查文件扩展名
	ext := strings.ToLower(filepath.Ext(filePath))
	if ext != ".md" && ext != ".markdown" {
		return fmt.Errorf("输入文件必须是Markdown格式(.md或.markdown): %s", filePath)
	}

	// 检查文件是否可读
	file, err := os.Open(filePath)
	if err != nil {
		return fmt.Errorf("无法读取输入文件: %v", err)
	}
	file.Close()

	return nil
}

// ValidateOutputDir 验证输出目录
func ValidateOutputDir(dirPath string) error {
	if dirPath == "" {
		return nil // 输出目录可以为空，使用默认值
	}

	// 检查目录是否存在，如果不存在则创建
	if _, err := os.Stat(dirPath); os.IsNotExist(err) {
		if err := os.MkdirAll(dirPath, 0755); err != nil {
			return fmt.Errorf("无法创建输出目录: %v", err)
		}
	}

	// 检查目录是否可写
	testFile := filepath.Join(dirPath, ".write_test")
	file, err := os.Create(testFile)
	if err != nil {
		return fmt.Errorf("输出目录不可写: %v", err)
	}
	file.Close()
	os.Remove(testFile)

	return nil
}

// DetermineOutputPath 确定输出文件路径
func DetermineOutputPath(inputFile, outputDir, outputName string) (string, error) {
	// 确定输出目录
	var finalOutputDir string
	if outputDir != "" {
		finalOutputDir = outputDir
	} else {
		finalOutputDir = filepath.Dir(inputFile)
	}

	// 确定输出文件名
	var finalOutputName string
	if outputName != "" {
		finalOutputName = outputName
		// 如果用户指定的文件名已经有.docx扩展名，不要重复添加
		if !strings.HasSuffix(strings.ToLower(finalOutputName), ".docx") {
			finalOutputName += ".docx"
		}
	} else {
		baseName := filepath.Base(inputFile)
		finalOutputName = strings.TrimSuffix(baseName, filepath.Ext(baseName)) + ".docx"
	}

	// 构建完整的输出路径
	outputPath := filepath.Join(finalOutputDir, finalOutputName)

	return outputPath, nil
}

// EnsureDir 确保目录存在
func EnsureDir(dirPath string) error {
	if dirPath == "" {
		return nil
	}

	if _, err := os.Stat(dirPath); os.IsNotExist(err) {
		return os.MkdirAll(dirPath, 0755)
	}

	return nil
}

// FileExists 检查文件是否存在
func FileExists(filePath string) bool {
	_, err := os.Stat(filePath)
	return !os.IsNotExist(err)
}

// GetFileSize 获取文件大小
func GetFileSize(filePath string) (int64, error) {
	info, err := os.Stat(filePath)
	if err != nil {
		return 0, err
	}
	return info.Size(), nil
}

// SanitizeFileName 清理文件名，移除不安全字符
func SanitizeFileName(fileName string) string {
	// 移除或替换不安全的字符
	unsafe := []string{"/", "\\", ":", "*", "?", "\"", "<", ">", "|"}
	result := fileName
	for _, char := range unsafe {
		result = strings.ReplaceAll(result, char, "_")
	}
	return result
}

// GetAbsolutePath 获取绝对路径
func GetAbsolutePath(path string) (string, error) {
	if filepath.IsAbs(path) {
		return path, nil
	}
	return filepath.Abs(path)
}
