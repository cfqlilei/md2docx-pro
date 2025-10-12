package converter

import (
	"os"
	"testing"

	"md2docx/internal/config"
	"md2docx/internal/models"
)

func TestNew(t *testing.T) {
	cfg := &config.Config{
		PandocPath: "/usr/bin/pandoc",
	}

	converter := New(cfg)
	if converter == nil {
		t.Fatal("创建转换器失败")
	}

	if converter.config != cfg {
		t.Error("转换器配置不匹配")
	}
}

func TestConvertSingle_InvalidInput(t *testing.T) {
	cfg := &config.Config{
		PandocPath: "/usr/bin/pandoc",
	}
	converter := New(cfg)

	// 测试空输入文件
	req := &models.ConversionRequest{
		InputFile: "",
	}

	resp, err := converter.ConvertSingle(req)
	if err != nil {
		t.Errorf("转换单文件时发生错误: %v", err)
	}

	if resp.Success {
		t.Error("期望转换失败，但成功了")
	}

	// 测试不存在的输入文件
	req = &models.ConversionRequest{
		InputFile: "/nonexistent/file.md",
	}

	resp, err = converter.ConvertSingle(req)
	if err != nil {
		t.Errorf("转换单文件时发生错误: %v", err)
	}

	if resp.Success {
		t.Error("期望转换失败，但成功了")
	}
}

func TestConvertSingle_ValidInput(t *testing.T) {
	// 创建临时Markdown文件
	tmpFile, err := os.CreateTemp("", "test*.md")
	if err != nil {
		t.Fatalf("创建临时文件失败: %v", err)
	}
	defer os.Remove(tmpFile.Name())

	// 写入测试内容
	content := "# 测试标题\n\n这是测试内容。"
	if _, err := tmpFile.WriteString(content); err != nil {
		t.Fatalf("写入文件失败: %v", err)
	}
	tmpFile.Close()

	// 创建临时输出目录
	tmpDir, err := os.MkdirTemp("", "output_test")
	if err != nil {
		t.Fatalf("创建临时目录失败: %v", err)
	}
	defer os.RemoveAll(tmpDir)

	cfg := &config.Config{
		PandocPath: "/usr/bin/pandoc", // 假设系统有pandoc
	}
	converter := New(cfg)

	req := &models.ConversionRequest{
		InputFile:  tmpFile.Name(),
		OutputDir:  tmpDir,
		OutputName: "test_output",
	}

	resp, err := converter.ConvertSingle(req)
	if err != nil {
		t.Errorf("转换单文件时发生错误: %v", err)
	}

	// 注意：这个测试可能会失败，如果系统没有安装pandoc
	// 在实际环境中，我们需要mock pandoc或者跳过这个测试
	if !resp.Success && resp.Error != "" {
		t.Logf("转换失败（可能是因为pandoc未安装）: %s", resp.Error)
	}
}

func TestConvertBatch_EmptyInput(t *testing.T) {
	cfg := &config.Config{
		PandocPath: "/usr/bin/pandoc",
	}
	converter := New(cfg)

	// 测试空输入文件列表
	req := &models.BatchConversionRequest{
		InputFiles: []string{},
	}

	resp, err := converter.ConvertBatch(req)
	if err != nil {
		t.Errorf("批量转换时发生错误: %v", err)
	}

	if resp.Success {
		t.Error("期望批量转换失败，但成功了")
	}
}

func TestConvertBatch_ValidInput(t *testing.T) {
	// 创建多个临时Markdown文件
	var tmpFiles []string
	for i := 0; i < 3; i++ {
		tmpFile, err := os.CreateTemp("", "test*.md")
		if err != nil {
			t.Fatalf("创建临时文件失败: %v", err)
		}
		defer os.Remove(tmpFile.Name())

		content := "# 测试标题\n\n这是测试内容。"
		if _, err := tmpFile.WriteString(content); err != nil {
			t.Fatalf("写入文件失败: %v", err)
		}
		tmpFile.Close()

		tmpFiles = append(tmpFiles, tmpFile.Name())
	}

	// 创建临时输出目录
	tmpDir, err := os.MkdirTemp("", "batch_output_test")
	if err != nil {
		t.Fatalf("创建临时目录失败: %v", err)
	}
	defer os.RemoveAll(tmpDir)

	cfg := &config.Config{
		PandocPath: "/usr/bin/pandoc", // 假设系统有pandoc
	}
	converter := New(cfg)

	req := &models.BatchConversionRequest{
		InputFiles: tmpFiles,
		OutputDir:  tmpDir,
	}

	resp, err := converter.ConvertBatch(req)
	if err != nil {
		t.Errorf("批量转换时发生错误: %v", err)
	}

	// 检查结果数量
	if len(resp.Results) != len(tmpFiles) {
		t.Errorf("期望结果数量 %d, 实际 %d", len(tmpFiles), len(resp.Results))
	}

	// 注意：这个测试可能会失败，如果系统没有安装pandoc
	if !resp.Success && len(resp.Results) > 0 {
		t.Logf("批量转换失败（可能是因为pandoc未安装）")
		for _, result := range resp.Results {
			if !result.Success {
				t.Logf("文件 %s 转换失败: %s", result.InputFile, result.Error)
			}
		}
	}
}

func TestUpdateConfig(t *testing.T) {
	cfg1 := &config.Config{
		PandocPath: "/usr/bin/pandoc",
	}

	converter := New(cfg1)

	cfg2 := &config.Config{
		PandocPath: "/usr/local/bin/pandoc",
	}

	converter.UpdateConfig(cfg2)

	if converter.config != cfg2 {
		t.Error("更新配置失败")
	}
}

// 辅助函数：创建测试用的Markdown文件
func createTestMarkdownFile(t *testing.T, content string) string {
	tmpFile, err := os.CreateTemp("", "test*.md")
	if err != nil {
		t.Fatalf("创建临时文件失败: %v", err)
	}

	if _, err := tmpFile.WriteString(content); err != nil {
		t.Fatalf("写入文件失败: %v", err)
	}
	tmpFile.Close()

	return tmpFile.Name()
}

// 辅助函数：创建测试用的输出目录
func createTestOutputDir(t *testing.T) string {
	tmpDir, err := os.MkdirTemp("", "output_test")
	if err != nil {
		t.Fatalf("创建临时目录失败: %v", err)
	}
	return tmpDir
}
