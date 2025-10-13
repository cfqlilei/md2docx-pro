package unit

import (
	"path/filepath"
	"strings"
	"testing"

	"md2docx/pkg/utils"
)

// TestFileNameHandling_双重后缀修复 测试文件名处理，确保没有双重后缀
func TestFileNameHandling_双重后缀修复(t *testing.T) {
	testCases := []struct {
		name           string
		inputFile      string
		outputDir      string
		outputName     string
		expectedSuffix string
		description    string
	}{
		{
			name:           "指定输出名称（无扩展名）",
			inputFile:      "/path/to/test.md",
			outputDir:      "/output",
			outputName:     "custom",
			expectedSuffix: "custom.docx",
			description:    "用户指定文件名不含扩展名，应自动添加.docx",
		},
		{
			name:           "指定输出名称（有扩展名）",
			inputFile:      "/path/to/test.md",
			outputDir:      "/output",
			outputName:     "custom.docx",
			expectedSuffix: "custom.docx",
			description:    "用户指定文件名已含.docx扩展名，不应重复添加",
		},
		{
			name:           "不指定输出名称",
			inputFile:      "/path/to/test.md",
			outputDir:      "/output",
			outputName:     "",
			expectedSuffix: "test.docx",
			description:    "不指定输出名称时，使用输入文件名并添加.docx",
		},
		{
			name:           "输入文件名包含多个点",
			inputFile:      "/path/to/test.backup.md",
			outputDir:      "/output",
			outputName:     "",
			expectedSuffix: "test.backup.docx",
			description:    "输入文件名包含多个点时，只移除最后的扩展名",
		},
		{
			name:           "中文文件名测试",
			inputFile:      "/path/to/表单管理模块详细设计方案.md",
			outputDir:      "/output",
			outputName:     "",
			expectedSuffix: "表单管理模块详细设计方案.docx",
			description:    "中文文件名应正确处理",
		},
		{
			name:           "用户指定中文文件名",
			inputFile:      "/path/to/test.md",
			outputDir:      "/output",
			outputName:     "6-表单管理模块详细设计方案",
			expectedSuffix: "6-表单管理模块详细设计方案.docx",
			description:    "用户指定中文文件名应正确处理",
		},
		{
			name:           "用户指定中文文件名含扩展名",
			inputFile:      "/path/to/test.md",
			outputDir:      "/output",
			outputName:     "6-表单管理模块详细设计方案.docx",
			expectedSuffix: "6-表单管理模块详细设计方案.docx",
			description:    "用户指定中文文件名已含扩展名，不应重复添加",
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

			// 验证没有双重.docx后缀
			filename := filepath.Base(result)
			docxCount := strings.Count(strings.ToLower(filename), ".docx")
			if docxCount > 1 {
				t.Errorf("文件名有双重.docx后缀: %s (出现%d次)", filename, docxCount)
			}

			// 验证文件名确实以.docx结尾
			if !strings.HasSuffix(strings.ToLower(filename), ".docx") {
				t.Errorf("文件名应以.docx结尾: %s", filename)
			}

			t.Logf("✅ %s: %s -> %s", tc.description, tc.outputName, filename)
		})
	}
}

// TestFileNameHandling_边界情况 测试边界情况
func TestFileNameHandling_边界情况(t *testing.T) {
	testCases := []struct {
		name        string
		inputFile   string
		outputDir   string
		outputName  string
		expectError bool
		description string
	}{
		{
			name:        "空输入文件",
			inputFile:   "",
			outputDir:   "/output",
			outputName:  "test",
			expectError: false,
			description: "空输入文件应该能处理",
		},
		{
			name:        "空输出目录",
			inputFile:   "/path/to/test.md",
			outputDir:   "",
			outputName:  "test",
			expectError: false,
			description: "空输出目录应使用输入文件目录",
		},
		{
			name:        "大小写混合的扩展名",
			inputFile:   "/path/to/test.md",
			outputDir:   "/output",
			outputName:  "test.DOCX",
			expectError: false,
			description: "大小写混合的扩展名应正确识别",
		},
		{
			name:        "多重扩展名",
			inputFile:   "/path/to/test.md",
			outputDir:   "/output",
			outputName:  "test.docx.docx",
			expectError: false,
			description: "多重扩展名应正确处理",
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			result, err := utils.DetermineOutputPath(tc.inputFile, tc.outputDir, tc.outputName)

			if tc.expectError && err == nil {
				t.Errorf("期望出错但成功了: %s", result)
			}

			if !tc.expectError && err != nil {
				t.Errorf("不期望出错但失败了: %v", err)
			}

			if err == nil {
				// 验证结果合理性
				filename := filepath.Base(result)
				if filename == "" {
					t.Errorf("生成的文件名为空")
				}

				t.Logf("✅ %s: 结果 %s", tc.description, result)
			}
		})
	}
}
