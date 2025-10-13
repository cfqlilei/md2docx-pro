package tests

import (
	"bytes"
	"encoding/json"
	"fmt"
	"net/http"
	"net/http/httptest"
	"os"
	"path/filepath"
	"testing"
	"time"

	"md2docx/internal/api"
	"md2docx/internal/config"
	"md2docx/internal/models"
)

// TestIntegration_FullWorkflow 完整工作流程集成测试
func TestIntegration_FullWorkflow(t *testing.T) {
	// 跳过集成测试，如果没有设置环境变量
	if os.Getenv("RUN_INTEGRATION_TESTS") != "1" {
		t.Skip("跳过集成测试，设置 RUN_INTEGRATION_TESTS=1 来运行")
	}

	// 创建测试配置
	cfg := &config.Config{
		PandocPath:   "/usr/bin/pandoc", // 假设系统有pandoc
		TemplateFile: "",
		ServerPort:   8080,
	}

	// 设置路由
	mux := api.SetupRoutes(cfg)
	server := httptest.NewServer(mux)
	defer server.Close()

	// 测试健康检查
	t.Run("健康检查", func(t *testing.T) {
		resp, err := http.Get(server.URL + "/api/health")
		if err != nil {
			t.Fatalf("健康检查请求失败: %v", err)
		}
		defer resp.Body.Close()

		if resp.StatusCode != http.StatusOK {
			t.Errorf("期望状态码 %d, 实际 %d", http.StatusOK, resp.StatusCode)
		}

		var result map[string]interface{}
		if err := json.NewDecoder(resp.Body).Decode(&result); err != nil {
			t.Fatalf("解析响应失败: %v", err)
		}

		if result["status"] != "ok" {
			t.Errorf("期望状态 'ok', 实际 '%v'", result["status"])
		}
	})

	// 测试配置管理
	t.Run("配置管理", func(t *testing.T) {
		// 获取配置
		resp, err := http.Get(server.URL + "/api/config")
		if err != nil {
			t.Fatalf("获取配置请求失败: %v", err)
		}
		defer resp.Body.Close()

		var configResp models.ConfigResponse
		if err := json.NewDecoder(resp.Body).Decode(&configResp); err != nil {
			t.Fatalf("解析配置响应失败: %v", err)
		}

		if !configResp.Success {
			t.Error("获取配置失败")
		}

		// 验证配置
		validateReq, _ := http.NewRequest("POST", server.URL+"/api/config/validate", nil)
		resp, err = http.DefaultClient.Do(validateReq)
		if err != nil {
			t.Fatalf("验证配置请求失败: %v", err)
		}
		defer resp.Body.Close()

		if err := json.NewDecoder(resp.Body).Decode(&configResp); err != nil {
			t.Fatalf("解析验证响应失败: %v", err)
		}

		// 注意：如果系统没有pandoc，这里会失败
		if !configResp.Success {
			t.Logf("配置验证失败（可能是因为pandoc未安装）: %s", configResp.Error)
		}
	})

	// 测试文件转换（需要实际的测试文件）
	t.Run("文件转换", func(t *testing.T) {
		// 创建测试Markdown文件
		testContent := `# 集成测试文档

这是一个用于集成测试的Markdown文档。

## 功能测试

- 列表项1
- 列表项2
- 列表项3

**粗体文本** 和 *斜体文本*

` + "`代码文本`" + `

## 代码块

` + "```go" + `
package main

import "fmt"

func main() {
    fmt.Println("Hello, Integration Test!")
}
` + "```" + `

## 表格

| 列1 | 列2 | 列3 |
|-----|-----|-----|
| A   | B   | C   |
| 1   | 2   | 3   |
`

		// 创建临时测试文件
		tmpFile, err := os.CreateTemp("", "integration_test*.md")
		if err != nil {
			t.Fatalf("创建临时文件失败: %v", err)
		}
		defer os.Remove(tmpFile.Name())

		if _, err := tmpFile.WriteString(testContent); err != nil {
			t.Fatalf("写入测试文件失败: %v", err)
		}
		tmpFile.Close()

		// 创建输出目录
		outputDir, err := os.MkdirTemp("", "integration_output")
		if err != nil {
			t.Fatalf("创建输出目录失败: %v", err)
		}
		defer os.RemoveAll(outputDir)

		// 测试单文件转换
		convReq := models.ConversionRequest{
			InputFile:  tmpFile.Name(),
			OutputDir:  outputDir,
			OutputName: "integration_test_output",
		}

		jsonData, err := json.Marshal(convReq)
		if err != nil {
			t.Fatalf("序列化请求失败: %v", err)
		}

		resp, err := http.Post(server.URL+"/api/convert/single", "application/json", bytes.NewBuffer(jsonData))
		if err != nil {
			t.Fatalf("单文件转换请求失败: %v", err)
		}
		defer resp.Body.Close()

		var convResp models.ConversionResponse
		if err := json.NewDecoder(resp.Body).Decode(&convResp); err != nil {
			t.Fatalf("解析转换响应失败: %v", err)
		}

		// 注意：如果系统没有pandoc，这里会失败
		if !convResp.Success {
			t.Logf("单文件转换失败（可能是因为pandoc未安装）: %s", convResp.Error)
		} else {
			// 检查输出文件是否存在
			expectedOutput := filepath.Join(outputDir, "integration_test_output.docx")
			if _, err := os.Stat(expectedOutput); os.IsNotExist(err) {
				t.Errorf("输出文件不存在: %s", expectedOutput)
			} else {
				t.Logf("转换成功，输出文件: %s", expectedOutput)
			}
		}
	})
}

// TestIntegration_ErrorHandling 错误处理集成测试
func TestIntegration_ErrorHandling(t *testing.T) {
	cfg := &config.Config{
		PandocPath:   "/nonexistent/pandoc", // 故意使用不存在的路径
		TemplateFile: "",
		ServerPort:   8080,
	}

	mux := api.SetupRoutes(cfg)
	server := httptest.NewServer(mux)
	defer server.Close()

	// 测试无效的转换请求
	t.Run("无效转换请求", func(t *testing.T) {
		convReq := models.ConversionRequest{
			InputFile:  "/nonexistent/file.md",
			OutputDir:  "/tmp",
			OutputName: "test",
		}

		jsonData, err := json.Marshal(convReq)
		if err != nil {
			t.Fatalf("序列化请求失败: %v", err)
		}

		resp, err := http.Post(server.URL+"/api/convert/single", "application/json", bytes.NewBuffer(jsonData))
		if err != nil {
			t.Fatalf("转换请求失败: %v", err)
		}
		defer resp.Body.Close()

		var convResp models.ConversionResponse
		if err := json.NewDecoder(resp.Body).Decode(&convResp); err != nil {
			t.Fatalf("解析响应失败: %v", err)
		}

		if convResp.Success {
			t.Error("期望转换失败，但成功了")
		}

		if convResp.Error == "" {
			t.Error("期望有错误信息，但为空")
		}
	})

	// 测试批量转换错误处理
	t.Run("批量转换错误处理", func(t *testing.T) {
		batchReq := models.BatchConversionRequest{
			InputFiles: []string{"/nonexistent/file1.md", "/nonexistent/file2.md"},
			OutputDir:  "/tmp",
		}

		jsonData, err := json.Marshal(batchReq)
		if err != nil {
			t.Fatalf("序列化请求失败: %v", err)
		}

		resp, err := http.Post(server.URL+"/api/convert/batch", "application/json", bytes.NewBuffer(jsonData))
		if err != nil {
			t.Fatalf("批量转换请求失败: %v", err)
		}
		defer resp.Body.Close()

		var convResp models.ConversionResponse
		if err := json.NewDecoder(resp.Body).Decode(&convResp); err != nil {
			t.Fatalf("解析响应失败: %v", err)
		}

		if convResp.Success {
			t.Error("期望批量转换失败，但成功了")
		}

		if len(convResp.Results) != 2 {
			t.Errorf("期望结果数量 2, 实际 %d", len(convResp.Results))
		}

		// 检查每个结果都应该失败
		for i, result := range convResp.Results {
			if result.Success {
				t.Errorf("期望结果 %d 失败，但成功了", i)
			}
			if result.Error == "" {
				t.Errorf("期望结果 %d 有错误信息，但为空", i)
			}
		}
	})
}

// BenchmarkConversion 转换性能基准测试
func BenchmarkConversion(b *testing.B) {
	if os.Getenv("RUN_BENCHMARK_TESTS") != "1" {
		b.Skip("跳过基准测试，设置 RUN_BENCHMARK_TESTS=1 来运行")
	}

	// 创建测试文件
	testContent := "# 基准测试\n\n这是基准测试内容。\n\n" + 
		"重复内容 " + fmt.Sprintf("%d", time.Now().Unix())

	tmpFile, err := os.CreateTemp("", "benchmark*.md")
	if err != nil {
		b.Fatalf("创建临时文件失败: %v", err)
	}
	defer os.Remove(tmpFile.Name())

	if _, err := tmpFile.WriteString(testContent); err != nil {
		b.Fatalf("写入测试文件失败: %v", err)
	}
	tmpFile.Close()

	cfg := &config.Config{
		PandocPath: "/usr/bin/pandoc",
	}

	mux := api.SetupRoutes(cfg)
	server := httptest.NewServer(mux)
	defer server.Close()

	b.ResetTimer()

	for i := 0; i < b.N; i++ {
		convReq := models.ConversionRequest{
			InputFile:  tmpFile.Name(),
			OutputName: fmt.Sprintf("benchmark_%d", i),
		}

		jsonData, _ := json.Marshal(convReq)
		resp, err := http.Post(server.URL+"/api/convert/single", "application/json", bytes.NewBuffer(jsonData))
		if err != nil {
			b.Fatalf("转换请求失败: %v", err)
		}
		resp.Body.Close()
	}
}
