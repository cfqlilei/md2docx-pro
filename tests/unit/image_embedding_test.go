package unit

import (
	"os"
	"path/filepath"
	"strings"
	"testing"

	"md2docx/internal/config"
	"md2docx/internal/converter"
	"md2docx/internal/models"
	"md2docx/pkg/utils"
)

// TestImageEmbedding 测试图片嵌入功能
func TestImageEmbedding(t *testing.T) {
	// 创建配置
	cfg := &config.Config{
		PandocPath: "/opt/homebrew/bin/pandoc", // 根据实际情况调整
	}

	// 验证Pandoc配置
	if err := cfg.ValidatePandoc(); err != nil {
		t.Skipf("跳过测试，Pandoc不可用: %v", err)
	}

	// 创建转换器
	conv := converter.New(cfg)

	// 创建临时目录
	tmpDir, err := os.MkdirTemp("", "image_test")
	if err != nil {
		t.Fatalf("创建临时目录失败: %v", err)
	}
	defer os.RemoveAll(tmpDir)

	// 创建包含图片的Markdown文件
	mdContent := `# 图片嵌入测试

这是一个包含图片的测试文档。

## 网络图片

![GitHub Logo](https://github.githubassets.com/images/modules/logos_page/GitHub-Mark.png)

## 测试内容

这里是一些测试内容，用于验证图片嵌入功能。
`

	mdFile := filepath.Join(tmpDir, "test_image.md")
	if err := os.WriteFile(mdFile, []byte(mdContent), 0644); err != nil {
		t.Fatalf("创建测试文件失败: %v", err)
	}

	// 执行转换
	req := &models.ConversionRequest{
		InputFile:  mdFile,
		OutputDir:  tmpDir,
		OutputName: "test_image_output",
	}

	resp, err := conv.ConvertSingle(req)
	if err != nil {
		t.Fatalf("转换失败: %v", err)
	}

	if !resp.Success {
		t.Fatalf("转换失败: %s", resp.Error)
	}

	// 验证输出文件存在
	if !utils.FileExists(resp.OutputFile) {
		t.Fatalf("输出文件不存在: %s", resp.OutputFile)
	}

	// 验证文件大小（包含图片的文件应该比较大）
	size, err := utils.GetFileSize(resp.OutputFile)
	if err != nil {
		t.Fatalf("获取文件大小失败: %v", err)
	}

	// 包含图片的DOCX文件应该至少有几KB
	if size < 5000 {
		t.Errorf("输出文件太小，可能图片没有嵌入: %d bytes", size)
	}

	t.Logf("转换成功，输出文件: %s, 大小: %d bytes", resp.OutputFile, size)
}

// TestFileNameHandling 测试文件名处理，确保没有双重后缀
func TestFileNameHandling(t *testing.T) {
	testCases := []struct {
		name           string
		inputFile      string
		outputDir      string
		outputName     string
		expectedSuffix string
	}{
		{
			name:           "指定输出名称（无扩展名）",
			inputFile:      "/path/to/test.md",
			outputDir:      "/output",
			outputName:     "custom",
			expectedSuffix: "custom.docx",
		},
		{
			name:           "指定输出名称（有扩展名）",
			inputFile:      "/path/to/test.md",
			outputDir:      "/output",
			outputName:     "custom.docx",
			expectedSuffix: "custom.docx",
		},
		{
			name:           "不指定输出名称",
			inputFile:      "/path/to/test.md",
			outputDir:      "/output",
			outputName:     "",
			expectedSuffix: "test.docx",
		},
		{
			name:           "输入文件已有docx扩展名",
			inputFile:      "/path/to/test.docx.md",
			outputDir:      "/output",
			outputName:     "",
			expectedSuffix: "test.docx.docx",
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			result, err := utils.DetermineOutputPath(tc.inputFile, tc.outputDir, tc.outputName)
			if err != nil {
				t.Fatalf("确定输出路径失败: %v", err)
			}

			expectedPath := filepath.Join(tc.outputDir, tc.expectedSuffix)
			if result != expectedPath {
				t.Errorf("期望输出路径 %s, 实际 %s", expectedPath, result)
			}

			// 验证没有双重.docx后缀（除非输入文件本身就有这种情况）
			if tc.outputName != "" && !strings.HasSuffix(tc.outputName, ".docx") {
				filename := filepath.Base(result)
				if strings.Count(filename, ".docx") > 1 {
					t.Errorf("文件名有双重.docx后缀: %s", filename)
				}
			}
		})
	}
}
