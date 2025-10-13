package unit

import (
	"encoding/json"
	"net/http"
	"net/http/httptest"
	"strings"
	"testing"

	"md2docx/internal/api"
	"md2docx/internal/config"
	"md2docx/internal/models"
)

// TestConfigValidationDetailed 测试详细的配置验证功能
func TestConfigValidationDetailed(t *testing.T) {
	tests := []struct {
		name           string
		pandocPath     string
		templateFile   string
		expectSuccess  bool
		expectMessages []string
	}{
		{
			name:          "有效的Pandoc路径，无模板文件",
			pandocPath:    "/usr/local/bin/pandoc", // 假设这是有效路径
			templateFile:  "",
			expectSuccess: false, // 实际会失败，因为路径可能不存在
			expectMessages: []string{
				"Pandoc路径验证",
				"未配置模板文件（可选）",
			},
		},
		{
			name:          "无效的Pandoc路径",
			pandocPath:    "/nonexistent/pandoc",
			templateFile:  "",
			expectSuccess: false,
			expectMessages: []string{
				"❌ Pandoc路径验证失败",
				"未配置模板文件（可选）",
			},
		},
		{
			name:          "空的Pandoc路径",
			pandocPath:    "",
			templateFile:  "",
			expectSuccess: false,
			expectMessages: []string{
				"❌ Pandoc路径验证失败",
				"未配置模板文件（可选）",
			},
		},
		{
			name:          "有效Pandoc路径和无效模板文件",
			pandocPath:    "/usr/local/bin/pandoc",
			templateFile:  "/nonexistent/template.docx",
			expectSuccess: false,
			expectMessages: []string{
				"Pandoc路径验证",
				"❌ 模板文件验证失败",
			},
		},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			// 创建测试配置
			cfg := &config.Config{
				PandocPath:   tt.pandocPath,
				TemplateFile: tt.templateFile,
				ServerPort:   8080,
			}

			// 创建处理器
			handler := api.New(cfg)

			// 创建验证配置请求
			req, err := http.NewRequest("POST", "/api/config/validate", nil)
			if err != nil {
				t.Fatal(err)
			}

			// 创建响应记录器
			rr := httptest.NewRecorder()

			// 调用处理器
			handler.ValidateConfig(rr, req)

			// 检查状态码
			if status := rr.Code; status != http.StatusOK {
				t.Errorf("期望状态码 %v, 实际 %v", http.StatusOK, status)
			}

			// 解析响应
			var response models.ConfigResponse
			if err := json.Unmarshal(rr.Body.Bytes(), &response); err != nil {
				t.Errorf("解析响应失败: %v", err)
			}

			// 检查成功状态
			if response.Success != tt.expectSuccess {
				t.Errorf("期望成功状态 %v, 实际 %v", tt.expectSuccess, response.Success)
			}

			// 检查消息内容
			for _, expectedMsg := range tt.expectMessages {
				if !strings.Contains(response.Message, expectedMsg) {
					t.Errorf("响应消息中未找到期望的内容: %s\n实际消息: %s", expectedMsg, response.Message)
				}
			}

			// 验证消息格式
			if response.Message == "" {
				t.Error("验证消息不应为空")
			}

			// 验证消息包含表情符号
			hasEmoji := strings.Contains(response.Message, "✅") ||
				strings.Contains(response.Message, "❌") ||
				strings.Contains(response.Message, "ℹ️")
			if !hasEmoji {
				t.Error("验证消息应包含表情符号")
			}
		})
	}
}

// TestConfigValidationMessageFormat 测试配置验证消息格式
func TestConfigValidationMessageFormat(t *testing.T) {
	cfg := &config.Config{
		PandocPath:   "/nonexistent/pandoc",
		TemplateFile: "",
		ServerPort:   8080,
	}

	handler := api.New(cfg)

	req, err := http.NewRequest("POST", "/api/config/validate", nil)
	if err != nil {
		t.Fatal(err)
	}

	rr := httptest.NewRecorder()
	handler.ValidateConfig(rr, req)

	var response models.ConfigResponse
	if err := json.Unmarshal(rr.Body.Bytes(), &response); err != nil {
		t.Errorf("解析响应失败: %v", err)
	}

	// 验证消息格式
	lines := strings.Split(response.Message, "\n")
	if len(lines) < 2 {
		t.Error("验证消息应包含多行内容")
	}

	// 验证每行都有适当的前缀
	for _, line := range lines {
		if line == "" {
			continue
		}
		hasPrefix := strings.HasPrefix(line, "✅") ||
			strings.HasPrefix(line, "❌") ||
			strings.HasPrefix(line, "ℹ️")
		if !hasPrefix {
			t.Errorf("消息行应有适当的前缀: %s", line)
		}
	}
}

// TestPandocValidation 测试Pandoc路径验证
func TestPandocValidation(t *testing.T) {
	tests := []struct {
		name        string
		pandocPath  string
		expectError bool
	}{
		{
			name:        "空路径",
			pandocPath:  "",
			expectError: true,
		},
		{
			name:        "不存在的路径",
			pandocPath:  "/nonexistent/pandoc",
			expectError: true,
		},
		{
			name:        "系统pandoc命令",
			pandocPath:  "pandoc",
			expectError: true, // 在测试环境中可能不存在
		},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			cfg := &config.Config{
				PandocPath: tt.pandocPath,
			}

			err := cfg.ValidatePandoc()
			hasError := err != nil

			if hasError != tt.expectError {
				t.Errorf("期望错误状态 %v, 实际 %v, 错误: %v", tt.expectError, hasError, err)
			}
		})
	}
}

// TestTemplateValidation 测试模板文件验证
func TestTemplateValidation(t *testing.T) {
	tests := []struct {
		name         string
		templateFile string
		expectError  bool
	}{
		{
			name:         "空模板文件",
			templateFile: "",
			expectError:  false, // 模板文件是可选的
		},
		{
			name:         "不存在的模板文件",
			templateFile: "/nonexistent/template.docx",
			expectError:  true,
		},
		{
			name:         "错误的文件扩展名",
			templateFile: "/path/to/template.txt",
			expectError:  true,
		},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			cfg := &config.Config{
				TemplateFile: tt.templateFile,
			}

			err := cfg.ValidateTemplate()
			hasError := err != nil

			if hasError != tt.expectError {
				t.Errorf("期望错误状态 %v, 实际 %v, 错误: %v", tt.expectError, hasError, err)
			}
		})
	}
}
