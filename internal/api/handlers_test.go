package api

import (
	"bytes"
	"encoding/json"
	"net/http"
	"net/http/httptest"
	"testing"

	"md2docx/internal/config"
	"md2docx/internal/models"
)

func TestNew(t *testing.T) {
	cfg := &config.Config{
		PandocPath: "/usr/bin/pandoc",
	}

	handler := New(cfg)
	if handler == nil {
		t.Fatal("创建API处理器失败")
	}

	if handler.config != cfg {
		t.Error("API处理器配置不匹配")
	}
}

func TestHealth(t *testing.T) {
	cfg := &config.Config{
		PandocPath: "/usr/bin/pandoc",
	}
	handler := New(cfg)

	// 创建测试请求
	req, err := http.NewRequest("GET", "/api/health", nil)
	if err != nil {
		t.Fatal(err)
	}

	// 创建响应记录器
	rr := httptest.NewRecorder()

	// 调用处理器
	handler.Health(rr, req)

	// 检查状态码
	if status := rr.Code; status != http.StatusOK {
		t.Errorf("期望状态码 %v, 实际 %v", http.StatusOK, status)
	}

	// 检查响应内容
	var response map[string]interface{}
	if err := json.Unmarshal(rr.Body.Bytes(), &response); err != nil {
		t.Errorf("解析响应失败: %v", err)
	}

	if response["status"] != "ok" {
		t.Errorf("期望状态 'ok', 实际 '%v'", response["status"])
	}
}

func TestGetConfig(t *testing.T) {
	cfg := &config.Config{
		PandocPath:   "/usr/bin/pandoc",
		TemplateFile: "/path/to/template.docx",
		ServerPort:   8080,
	}
	handler := New(cfg)

	// 创建测试请求
	req, err := http.NewRequest("GET", "/api/config", nil)
	if err != nil {
		t.Fatal(err)
	}

	// 创建响应记录器
	rr := httptest.NewRecorder()

	// 调用处理器
	handler.GetConfig(rr, req)

	// 检查状态码
	if status := rr.Code; status != http.StatusOK {
		t.Errorf("期望状态码 %v, 实际 %v", http.StatusOK, status)
	}

	// 检查响应内容
	var response models.ConfigResponse
	if err := json.Unmarshal(rr.Body.Bytes(), &response); err != nil {
		t.Errorf("解析响应失败: %v", err)
	}

	if !response.Success {
		t.Error("期望获取配置成功")
	}

	if response.PandocPath != cfg.PandocPath {
		t.Errorf("期望Pandoc路径 %s, 实际 %s", cfg.PandocPath, response.PandocPath)
	}

	if response.TemplateFile != cfg.TemplateFile {
		t.Errorf("期望模板文件 %s, 实际 %s", cfg.TemplateFile, response.TemplateFile)
	}
}

func TestConvertSingle_InvalidMethod(t *testing.T) {
	cfg := &config.Config{
		PandocPath: "/usr/bin/pandoc",
	}
	handler := New(cfg)

	// 创建GET请求（应该只支持POST）
	req, err := http.NewRequest("GET", "/api/convert/single", nil)
	if err != nil {
		t.Fatal(err)
	}

	// 创建响应记录器
	rr := httptest.NewRecorder()

	// 调用处理器
	handler.ConvertSingle(rr, req)

	// 检查状态码
	if status := rr.Code; status != http.StatusMethodNotAllowed {
		t.Errorf("期望状态码 %v, 实际 %v", http.StatusMethodNotAllowed, status)
	}
}

func TestConvertSingle_InvalidJSON(t *testing.T) {
	cfg := &config.Config{
		PandocPath: "/usr/bin/pandoc",
	}
	handler := New(cfg)

	// 创建无效JSON的POST请求
	invalidJSON := `{"invalid": json}`
	req, err := http.NewRequest("POST", "/api/convert/single", bytes.NewBufferString(invalidJSON))
	if err != nil {
		t.Fatal(err)
	}
	req.Header.Set("Content-Type", "application/json")

	// 创建响应记录器
	rr := httptest.NewRecorder()

	// 调用处理器
	handler.ConvertSingle(rr, req)

	// 检查状态码
	if status := rr.Code; status != http.StatusBadRequest {
		t.Errorf("期望状态码 %v, 实际 %v", http.StatusBadRequest, status)
	}
}

func TestConvertSingle_ValidRequest(t *testing.T) {
	cfg := &config.Config{
		PandocPath: "/usr/bin/pandoc",
	}
	handler := New(cfg)

	// 创建有效的转换请求
	convReq := models.ConversionRequest{
		InputFile:  "/nonexistent/test.md", // 使用不存在的文件来测试错误处理
		OutputDir:  "/tmp",
		OutputName: "test_output",
	}

	jsonData, err := json.Marshal(convReq)
	if err != nil {
		t.Fatal(err)
	}

	req, err := http.NewRequest("POST", "/api/convert/single", bytes.NewBuffer(jsonData))
	if err != nil {
		t.Fatal(err)
	}
	req.Header.Set("Content-Type", "application/json")

	// 创建响应记录器
	rr := httptest.NewRecorder()

	// 调用处理器
	handler.ConvertSingle(rr, req)

	// 检查状态码
	if status := rr.Code; status != http.StatusOK {
		t.Errorf("期望状态码 %v, 实际 %v", http.StatusOK, status)
	}

	// 检查响应内容
	var response models.ConversionResponse
	if err := json.Unmarshal(rr.Body.Bytes(), &response); err != nil {
		t.Errorf("解析响应失败: %v", err)
	}

	// 由于使用了不存在的文件，期望转换失败
	if response.Success {
		t.Error("期望转换失败，但成功了")
	}
}

func TestConvertBatch_ValidRequest(t *testing.T) {
	cfg := &config.Config{
		PandocPath: "/usr/bin/pandoc",
	}
	handler := New(cfg)

	// 创建有效的批量转换请求
	batchReq := models.BatchConversionRequest{
		InputFiles: []string{"/nonexistent/test1.md", "/nonexistent/test2.md"},
		OutputDir:  "/tmp",
	}

	jsonData, err := json.Marshal(batchReq)
	if err != nil {
		t.Fatal(err)
	}

	req, err := http.NewRequest("POST", "/api/convert/batch", bytes.NewBuffer(jsonData))
	if err != nil {
		t.Fatal(err)
	}
	req.Header.Set("Content-Type", "application/json")

	// 创建响应记录器
	rr := httptest.NewRecorder()

	// 调用处理器
	handler.ConvertBatch(rr, req)

	// 检查状态码
	if status := rr.Code; status != http.StatusOK {
		t.Errorf("期望状态码 %v, 实际 %v", http.StatusOK, status)
	}

	// 检查响应内容
	var response models.ConversionResponse
	if err := json.Unmarshal(rr.Body.Bytes(), &response); err != nil {
		t.Errorf("解析响应失败: %v", err)
	}

	// 检查结果数量
	if len(response.Results) != 2 {
		t.Errorf("期望结果数量 2, 实际 %d", len(response.Results))
	}
}

func TestValidateConfig(t *testing.T) {
	cfg := &config.Config{
		PandocPath: "/nonexistent/pandoc", // 使用不存在的路径来测试验证失败
	}
	handler := New(cfg)

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

	// 检查响应内容
	var response models.ConfigResponse
	if err := json.Unmarshal(rr.Body.Bytes(), &response); err != nil {
		t.Errorf("解析响应失败: %v", err)
	}

	// 由于使用了不存在的Pandoc路径，期望验证失败
	if response.Success {
		t.Error("期望配置验证失败，但成功了")
	}
}
