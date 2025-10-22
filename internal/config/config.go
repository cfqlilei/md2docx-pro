package config

import (
	"encoding/json"
	"fmt"
	"os"
	"os/exec"
	"os/user"
	"path/filepath"
)

// Config 应用配置
type Config struct {
	PandocPath   string `json:"pandoc_path"`
	TemplateFile string `json:"template_file"`
	ServerPort   int    `json:"server_port"`
}

// DefaultConfig 默认配置
var DefaultConfig = &Config{
	PandocPath:   "",
	TemplateFile: "",
	ServerPort:   8080,
}

// getConfigFilePath 获取配置文件路径
func getConfigFilePath() string {
	// 首先尝试使用用户主目录下的应用程序配置目录
	if usr, err := user.Current(); err == nil {
		configDir := filepath.Join(usr.HomeDir, ".md2docx")
		// 确保配置目录存在
		if err := os.MkdirAll(configDir, 0755); err == nil {
			return filepath.Join(configDir, "config.json")
		}
	}

	// 如果无法获取用户目录，使用当前工作目录
	if wd, err := os.Getwd(); err == nil {
		return filepath.Join(wd, "config.json")
	}

	// 最后的备选方案
	return "config.json"
}

// Load 加载配置
func Load() (*Config, error) {
	config := &Config{
		PandocPath:   DefaultConfig.PandocPath,
		TemplateFile: DefaultConfig.TemplateFile,
		ServerPort:   DefaultConfig.ServerPort,
	}

	configFilePath := getConfigFilePath()

	// 如果配置文件存在，则加载
	if _, err := os.Stat(configFilePath); err == nil {
		data, err := os.ReadFile(configFilePath)
		if err != nil {
			return nil, fmt.Errorf("读取配置文件失败: %v", err)
		}

		// 创建临时配置用于解析
		var fileConfig Config
		if err := json.Unmarshal(data, &fileConfig); err != nil {
			return nil, fmt.Errorf("解析配置文件失败: %v", err)
		}

		// 合并配置，只覆盖非零值
		if fileConfig.PandocPath != "" {
			config.PandocPath = fileConfig.PandocPath
		}
		if fileConfig.TemplateFile != "" {
			config.TemplateFile = fileConfig.TemplateFile
		}
		if fileConfig.ServerPort != 0 {
			config.ServerPort = fileConfig.ServerPort
		}
	}

	// 如果没有配置Pandoc路径，尝试自动检测
	if config.PandocPath == "" {
		if pandocPath, err := findPandoc(); err == nil {
			config.PandocPath = pandocPath
			// 自动检测到路径后，保存配置
			if err := config.Save(); err != nil {
				fmt.Printf("警告: 无法保存自动检测的Pandoc路径: %v\n", err)
			}
		}
	}

	return config, nil
}

// Save 保存配置
func (c *Config) Save() error {
	configFilePath := getConfigFilePath()

	data, err := json.MarshalIndent(c, "", "  ")
	if err != nil {
		return fmt.Errorf("序列化配置失败: %v", err)
	}

	if err := os.WriteFile(configFilePath, data, 0644); err != nil {
		return fmt.Errorf("保存配置文件失败: %v", err)
	}

	return nil
}

// ValidatePandoc 验证Pandoc路径是否有效
func (c *Config) ValidatePandoc() error {
	// 如果路径为空，尝试自动检测
	if c.PandocPath == "" {
		if pandocPath, err := findPandoc(); err == nil {
			c.PandocPath = pandocPath
		} else {
			return fmt.Errorf("Pandoc路径未配置且无法自动检测: %v", err)
		}
	}

	// 如果路径不是绝对路径（如"pandoc"），直接尝试执行
	if c.PandocPath == "pandoc" || !filepath.IsAbs(c.PandocPath) {
		cmd := exec.Command(c.PandocPath, "--version")
		if err := cmd.Run(); err != nil {
			return fmt.Errorf("Pandoc执行失败: %v", err)
		}
		return nil
	}

	// 检查绝对路径的文件是否存在
	if _, err := os.Stat(c.PandocPath); os.IsNotExist(err) {
		return fmt.Errorf("Pandoc可执行文件不存在: %s", c.PandocPath)
	}

	// 尝试执行pandoc --version
	cmd := exec.Command(c.PandocPath, "--version")
	if err := cmd.Run(); err != nil {
		return fmt.Errorf("Pandoc执行失败: %v", err)
	}

	return nil
}

// ValidateTemplate 验证模板文件是否有效
func (c *Config) ValidateTemplate() error {
	if c.TemplateFile == "" {
		return nil // 模板文件是可选的
	}

	// 检查文件是否存在
	if _, err := os.Stat(c.TemplateFile); os.IsNotExist(err) {
		return fmt.Errorf("模板文件不存在: %s", c.TemplateFile)
	}

	// 检查文件扩展名
	ext := filepath.Ext(c.TemplateFile)
	if ext != ".docx" {
		return fmt.Errorf("模板文件必须是.docx格式: %s", c.TemplateFile)
	}

	return nil
}

// findPandoc 在系统PATH中查找Pandoc
func findPandoc() (string, error) {
	// 常见的Pandoc可执行文件名
	names := []string{"pandoc", "pandoc.exe"}

	for _, name := range names {
		if path, err := exec.LookPath(name); err == nil {
			return path, nil
		}
	}

	return "", fmt.Errorf("在系统PATH中未找到Pandoc")
}

// Update 更新配置
func (c *Config) Update(pandocPath, templateFile string) error {
	if pandocPath != "" {
		c.PandocPath = pandocPath
	}
	// 允许设置空模板文件（清空模板）
	c.TemplateFile = templateFile

	// 验证Pandoc配置（必须有效）
	if err := c.ValidatePandoc(); err != nil {
		return err
	}

	// 模板文件验证：只有在非空时才验证，允许用户设置不存在的路径
	// 实际使用时会在转换过程中再次验证

	// 保存配置
	return c.Save()
}
