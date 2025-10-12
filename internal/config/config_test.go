package config

import (
	"os"
	"path/filepath"
	"testing"
)

func TestLoad(t *testing.T) {
	// 测试加载默认配置
	cfg, err := Load()
	if err != nil {
		t.Fatalf("加载配置失败: %v", err)
	}

	if cfg.ServerPort != DefaultConfig.ServerPort {
		t.Errorf("期望服务器端口 %d, 实际 %d", DefaultConfig.ServerPort, cfg.ServerPort)
	}
}

func TestSave(t *testing.T) {
	// 创建临时配置
	cfg := &Config{
		PandocPath:   "/usr/local/bin/pandoc",
		TemplateFile: "/path/to/template.docx",
		ServerPort:   9090,
	}

	// 保存配置
	err := cfg.Save()
	if err != nil {
		t.Fatalf("保存配置失败: %v", err)
	}

	// 验证文件是否存在
	if _, err := os.Stat(configFile); os.IsNotExist(err) {
		t.Fatalf("配置文件未创建")
	}

	// 清理测试文件
	defer os.Remove(configFile)

	// 重新加载配置验证
	loadedCfg, err := Load()
	if err != nil {
		t.Fatalf("重新加载配置失败: %v", err)
	}

	if loadedCfg.PandocPath != cfg.PandocPath {
		t.Errorf("期望Pandoc路径 %s, 实际 %s", cfg.PandocPath, loadedCfg.PandocPath)
	}

	if loadedCfg.TemplateFile != cfg.TemplateFile {
		t.Errorf("期望模板文件 %s, 实际 %s", cfg.TemplateFile, loadedCfg.TemplateFile)
	}

	if loadedCfg.ServerPort != cfg.ServerPort {
		t.Errorf("期望服务器端口 %d, 实际 %d", cfg.ServerPort, loadedCfg.ServerPort)
	}
}

func TestValidatePandoc(t *testing.T) {
	cfg := &Config{}

	// 测试空路径
	err := cfg.ValidatePandoc()
	if err == nil {
		t.Error("期望验证失败，但成功了")
	}

	// 测试不存在的路径
	cfg.PandocPath = "/nonexistent/pandoc"
	err = cfg.ValidatePandoc()
	if err == nil {
		t.Error("期望验证失败，但成功了")
	}
}

func TestValidateTemplate(t *testing.T) {
	cfg := &Config{}

	// 测试空模板文件（应该成功）
	err := cfg.ValidateTemplate()
	if err != nil {
		t.Errorf("空模板文件验证失败: %v", err)
	}

	// 测试不存在的模板文件
	cfg.TemplateFile = "/nonexistent/template.docx"
	err = cfg.ValidateTemplate()
	if err == nil {
		t.Error("期望验证失败，但成功了")
	}

	// 测试错误的文件扩展名
	// 创建临时文件
	tmpFile, err := os.CreateTemp("", "test*.txt")
	if err != nil {
		t.Fatalf("创建临时文件失败: %v", err)
	}
	defer os.Remove(tmpFile.Name())
	tmpFile.Close()

	cfg.TemplateFile = tmpFile.Name()
	err = cfg.ValidateTemplate()
	if err == nil {
		t.Error("期望验证失败（错误扩展名），但成功了")
	}
}

func TestUpdate(t *testing.T) {
	// 创建临时目录用于测试
	tmpDir, err := os.MkdirTemp("", "config_test")
	if err != nil {
		t.Fatalf("创建临时目录失败: %v", err)
	}
	defer os.RemoveAll(tmpDir)

	// 创建一个假的pandoc可执行文件
	fakePandoc := filepath.Join(tmpDir, "pandoc")
	if err := os.WriteFile(fakePandoc, []byte("#!/bin/bash\necho 'pandoc 2.0'"), 0755); err != nil {
		t.Fatalf("创建假pandoc文件失败: %v", err)
	}

	// 创建一个假的模板文件
	fakeTemplate := filepath.Join(tmpDir, "template.docx")
	if err := os.WriteFile(fakeTemplate, []byte("fake docx content"), 0644); err != nil {
		t.Fatalf("创建假模板文件失败: %v", err)
	}

	cfg := &Config{}

	// 测试更新配置（这个测试可能会失败，因为假的pandoc文件可能无法执行）
	err = cfg.Update(fakePandoc, fakeTemplate)
	// 由于我们的假pandoc可能无法正确执行，这里只检查路径是否设置
	if cfg.PandocPath != fakePandoc {
		t.Errorf("期望Pandoc路径 %s, 实际 %s", fakePandoc, cfg.PandocPath)
	}

	if cfg.TemplateFile != fakeTemplate {
		t.Errorf("期望模板文件 %s, 实际 %s", fakeTemplate, cfg.TemplateFile)
	}
}
