package utils

import (
	"os"
	"path/filepath"
	"testing"
)

func TestValidateInputFile(t *testing.T) {
	// 测试空路径
	err := ValidateInputFile("")
	if err == nil {
		t.Error("期望验证失败，但成功了")
	}

	// 测试不存在的文件
	err = ValidateInputFile("/nonexistent/file.md")
	if err == nil {
		t.Error("期望验证失败，但成功了")
	}

	// 创建临时Markdown文件
	tmpFile, err := os.CreateTemp("", "test*.md")
	if err != nil {
		t.Fatalf("创建临时文件失败: %v", err)
	}
	defer os.Remove(tmpFile.Name())
	tmpFile.Close()

	// 测试有效的Markdown文件
	err = ValidateInputFile(tmpFile.Name())
	if err != nil {
		t.Errorf("验证有效Markdown文件失败: %v", err)
	}

	// 创建非Markdown文件
	tmpTxtFile, err := os.CreateTemp("", "test*.txt")
	if err != nil {
		t.Fatalf("创建临时txt文件失败: %v", err)
	}
	defer os.Remove(tmpTxtFile.Name())
	tmpTxtFile.Close()

	// 测试非Markdown文件
	err = ValidateInputFile(tmpTxtFile.Name())
	if err == nil {
		t.Error("期望验证失败（非Markdown文件），但成功了")
	}
}

func TestValidateOutputDir(t *testing.T) {
	// 测试空目录（应该成功）
	err := ValidateOutputDir("")
	if err != nil {
		t.Errorf("空目录验证失败: %v", err)
	}

	// 创建临时目录
	tmpDir, err := os.MkdirTemp("", "output_test")
	if err != nil {
		t.Fatalf("创建临时目录失败: %v", err)
	}
	defer os.RemoveAll(tmpDir)

	// 测试有效目录
	err = ValidateOutputDir(tmpDir)
	if err != nil {
		t.Errorf("验证有效目录失败: %v", err)
	}

	// 测试不存在的目录（应该自动创建）
	newDir := filepath.Join(tmpDir, "newdir")
	err = ValidateOutputDir(newDir)
	if err != nil {
		t.Errorf("验证不存在目录失败: %v", err)
	}

	// 验证目录是否被创建
	if _, err := os.Stat(newDir); os.IsNotExist(err) {
		t.Error("期望目录被创建，但不存在")
	}
}

func TestDetermineOutputPath(t *testing.T) {
	// 测试用例1：指定输出目录和文件名
	inputFile := "/path/to/input.md"
	outputDir := "/path/to/output"
	outputName := "custom"
	
	expected := "/path/to/output/custom.docx"
	result, err := DetermineOutputPath(inputFile, outputDir, outputName)
	if err != nil {
		t.Errorf("确定输出路径失败: %v", err)
	}
	if result != expected {
		t.Errorf("期望输出路径 %s, 实际 %s", expected, result)
	}

	// 测试用例2：只指定输出目录
	expected = "/path/to/output/input.docx"
	result, err = DetermineOutputPath(inputFile, outputDir, "")
	if err != nil {
		t.Errorf("确定输出路径失败: %v", err)
	}
	if result != expected {
		t.Errorf("期望输出路径 %s, 实际 %s", expected, result)
	}

	// 测试用例3：只指定文件名
	expected = "/path/to/custom.docx"
	result, err = DetermineOutputPath(inputFile, "", outputName)
	if err != nil {
		t.Errorf("确定输出路径失败: %v", err)
	}
	if result != expected {
		t.Errorf("期望输出路径 %s, 实际 %s", expected, result)
	}

	// 测试用例4：都不指定
	expected = "/path/to/input.docx"
	result, err = DetermineOutputPath(inputFile, "", "")
	if err != nil {
		t.Errorf("确定输出路径失败: %v", err)
	}
	if result != expected {
		t.Errorf("期望输出路径 %s, 实际 %s", expected, result)
	}
}

func TestFileExists(t *testing.T) {
	// 测试不存在的文件
	if FileExists("/nonexistent/file.txt") {
		t.Error("期望文件不存在，但返回存在")
	}

	// 创建临时文件
	tmpFile, err := os.CreateTemp("", "test*")
	if err != nil {
		t.Fatalf("创建临时文件失败: %v", err)
	}
	defer os.Remove(tmpFile.Name())
	tmpFile.Close()

	// 测试存在的文件
	if !FileExists(tmpFile.Name()) {
		t.Error("期望文件存在，但返回不存在")
	}
}

func TestGetFileSize(t *testing.T) {
	// 创建临时文件并写入内容
	tmpFile, err := os.CreateTemp("", "test*")
	if err != nil {
		t.Fatalf("创建临时文件失败: %v", err)
	}
	defer os.Remove(tmpFile.Name())

	content := "Hello, World!"
	if _, err := tmpFile.WriteString(content); err != nil {
		t.Fatalf("写入文件失败: %v", err)
	}
	tmpFile.Close()

	// 测试获取文件大小
	size, err := GetFileSize(tmpFile.Name())
	if err != nil {
		t.Errorf("获取文件大小失败: %v", err)
	}

	expectedSize := int64(len(content))
	if size != expectedSize {
		t.Errorf("期望文件大小 %d, 实际 %d", expectedSize, size)
	}

	// 测试不存在的文件
	_, err = GetFileSize("/nonexistent/file.txt")
	if err == nil {
		t.Error("期望获取文件大小失败，但成功了")
	}
}

func TestSanitizeFileName(t *testing.T) {
	testCases := []struct {
		input    string
		expected string
	}{
		{"normal_file.txt", "normal_file.txt"},
		{"file/with\\slash.txt", "file_with_slash.txt"},
		{"file:with*special?chars.txt", "file_with_special_chars.txt"},
		{"file\"with<quotes>and|pipes.txt", "file_with_quotes_and_pipes.txt"},
	}

	for _, tc := range testCases {
		result := SanitizeFileName(tc.input)
		if result != tc.expected {
			t.Errorf("输入 %s: 期望 %s, 实际 %s", tc.input, tc.expected, result)
		}
	}
}

func TestGetAbsolutePath(t *testing.T) {
	// 测试绝对路径
	absPath := "/absolute/path/file.txt"
	result, err := GetAbsolutePath(absPath)
	if err != nil {
		t.Errorf("获取绝对路径失败: %v", err)
	}
	if result != absPath {
		t.Errorf("期望绝对路径 %s, 实际 %s", absPath, result)
	}

	// 测试相对路径
	relPath := "relative/path/file.txt"
	result, err = GetAbsolutePath(relPath)
	if err != nil {
		t.Errorf("获取绝对路径失败: %v", err)
	}
	if !filepath.IsAbs(result) {
		t.Errorf("期望返回绝对路径，但得到相对路径: %s", result)
	}
}
