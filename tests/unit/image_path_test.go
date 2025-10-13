package unit

import (
	"os"
	"path/filepath"
	"testing"

	"md2docx/internal/config"
	"md2docx/internal/converter"
	"md2docx/internal/models"
)

// TestImagePathResolution_相对路径图片嵌入 测试相对路径图片嵌入功能
func TestImagePathResolution_相对路径图片嵌入(t *testing.T) {
	// 创建临时测试目录
	tempDir, err := os.MkdirTemp("", "image_path_test")
	if err != nil {
		t.Fatalf("创建临时目录失败: %v", err)
	}
	defer os.RemoveAll(tempDir)

	// 创建images子目录
	imagesDir := filepath.Join(tempDir, "images")
	if err := os.MkdirAll(imagesDir, 0755); err != nil {
		t.Fatalf("创建images目录失败: %v", err)
	}

	// 创建一个测试图片文件（简单的PNG文件头）
	testImagePath := filepath.Join(imagesDir, "test-image.png")
	// PNG文件的最小有效头部
	pngHeader := []byte{0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A}
	if err := os.WriteFile(testImagePath, pngHeader, 0644); err != nil {
		t.Fatalf("创建测试图片失败: %v", err)
	}

	// 创建测试Markdown文件
	testMdContent := `# 图片路径测试

这是一个测试文档，包含相对路径的图片。

## 本地图片测试

![测试图片](images/test-image.png)

## 在线图片测试

![GitHub Logo](https://github.githubassets.com/images/modules/logos_page/GitHub-Mark.png)

## 其他内容

这是一些测试文本。
`

	testMdPath := filepath.Join(tempDir, "test.md")
	if err := os.WriteFile(testMdPath, []byte(testMdContent), 0644); err != nil {
		t.Fatalf("创建测试Markdown文件失败: %v", err)
	}

	// 创建转换器
	cfg, err := config.Load()
	if err != nil {
		t.Fatalf("加载配置失败: %v", err)
	}
	conv := converter.New(cfg)

	// 执行转换
	outputPath := filepath.Join(tempDir, "output.docx")
	req := &models.ConversionRequest{
		InputFile:    testMdPath,
		OutputDir:    tempDir,
		OutputName:   "output",
		TemplateFile: "",
	}

	resp, err := conv.ConvertSingle(req)
	if err != nil {
		t.Fatalf("转换失败: %v", err)
	}

	// 使用响应中的实际输出文件路径
	outputPath = resp.OutputFile

	// 验证输出文件存在
	if _, err := os.Stat(outputPath); os.IsNotExist(err) {
		t.Fatalf("输出文件不存在: %s", outputPath)
	}

	// 检查文件大小（包含图片的文档应该比纯文本大）
	fileInfo, err := os.Stat(outputPath)
	if err != nil {
		t.Fatalf("获取文件信息失败: %v", err)
	}

	fileSize := fileInfo.Size()
	t.Logf("生成的docx文件大小: %d bytes", fileSize)

	// 包含图片的文档应该大于10KB
	if fileSize < 10000 {
		t.Errorf("文件大小过小 (%d bytes)，可能图片未正确嵌入", fileSize)
	}

	t.Logf("✅ 相对路径图片嵌入测试通过，文件大小: %d bytes", fileSize)
}

// TestImagePathResolution_多种图片目录 测试多种图片目录结构
func TestImagePathResolution_多种图片目录(t *testing.T) {
	// 创建临时测试目录
	tempDir, err := os.MkdirTemp("", "multi_image_dir_test")
	if err != nil {
		t.Fatalf("创建临时目录失败: %v", err)
	}
	defer os.RemoveAll(tempDir)

	// 创建多个图片目录
	imageDirs := []string{"images", "figures", "pics", "assets"}
	for _, dirName := range imageDirs {
		dir := filepath.Join(tempDir, dirName)
		if err := os.MkdirAll(dir, 0755); err != nil {
			t.Fatalf("创建%s目录失败: %v", dirName, err)
		}

		// 在每个目录中创建一个测试图片
		imagePath := filepath.Join(dir, "test.png")
		pngHeader := []byte{0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A}
		if err := os.WriteFile(imagePath, pngHeader, 0644); err != nil {
			t.Fatalf("创建测试图片失败: %v", err)
		}

		t.Logf("创建测试图片: %s", imagePath)
	}

	// 创建包含多个图片的测试Markdown文件
	testMdContent := `# 多图片目录测试

## 不同目录的图片

![图片1](images/test.png)
![图片2](figures/test.png)
![图片3](pics/test.png)
![图片4](assets/test.png)

## 测试内容

这是一个包含多个图片目录的测试文档。
`

	testMdPath := filepath.Join(tempDir, "multi_test.md")
	if err := os.WriteFile(testMdPath, []byte(testMdContent), 0644); err != nil {
		t.Fatalf("创建测试Markdown文件失败: %v", err)
	}

	// 创建转换器
	cfg, err := config.Load()
	if err != nil {
		t.Fatalf("加载配置失败: %v", err)
	}
	conv := converter.New(cfg)

	// 执行转换
	req := &models.ConversionRequest{
		InputFile:    testMdPath,
		OutputDir:    tempDir,
		OutputName:   "multi_output",
		TemplateFile: "",
	}

	resp, err := conv.ConvertSingle(req)
	if err != nil {
		t.Fatalf("转换失败: %v", err)
	}

	// 使用响应中的实际输出文件路径
	outputPath := resp.OutputFile

	// 验证输出文件存在
	if _, err := os.Stat(outputPath); os.IsNotExist(err) {
		t.Fatalf("输出文件不存在: %s", outputPath)
	}

	// 检查文件大小
	fileInfo, err := os.Stat(outputPath)
	if err != nil {
		t.Fatalf("获取文件信息失败: %v", err)
	}

	fileSize := fileInfo.Size()
	t.Logf("生成的docx文件大小: %d bytes", fileSize)

	// 包含多个图片的文档应该更大
	if fileSize < 15000 {
		t.Errorf("文件大小过小 (%d bytes)，可能部分图片未正确嵌入", fileSize)
	}

	t.Logf("✅ 多图片目录测试通过，文件大小: %d bytes", fileSize)
}
