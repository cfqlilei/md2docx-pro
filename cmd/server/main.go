package main

import (
	"fmt"
	"log"
	"net"
	"net/http"
	"os"
	"os/signal"
	"strconv"
	"syscall"

	"md2docx/internal/api"
	"md2docx/internal/config"
)

// 版本信息，在构建时通过ldflags注入
var (
	Version   = "dev"
	BuildTime = "unknown"
	GitCommit = "unknown"
)

func main() {
	// 输出版本信息
	fmt.Printf("=== Markdown转Word工具服务器 ===\n")
	fmt.Printf("版本: %s\n", Version)
	fmt.Printf("构建时间: %s\n", BuildTime)
	fmt.Printf("Git提交: %s\n", GitCommit)
	fmt.Printf("服务器端口: %d\n", 8080)

	// 加载配置
	cfg, err := config.Load()
	if err != nil {
		log.Fatalf("加载配置失败: %v", err)
	}

	// 检查是否通过环境变量指定端口
	if envPort := os.Getenv("SERVER_PORT"); envPort != "" {
		if port, err := strconv.Atoi(envPort); err == nil && port > 0 {
			cfg.ServerPort = port
		}
	}

	// 如果端口为0或被占用，动态分配端口
	if cfg.ServerPort == 0 || !isPortAvailable(cfg.ServerPort) {
		availablePort, err := findAvailablePort()
		if err != nil {
			log.Fatalf("无法找到可用端口: %v", err)
		}
		cfg.ServerPort = availablePort

		// 保存新端口到配置文件
		if err := cfg.Save(); err != nil {
			log.Printf("警告: 无法保存端口配置: %v", err)
		} else {
			log.Printf("端口配置已保存: %d", cfg.ServerPort)
		}
	} else {
		// 即使端口可用，也保存配置文件以确保前端能读取
		if err := cfg.Save(); err != nil {
			log.Printf("警告: 无法保存端口配置: %v", err)
		} else {
			log.Printf("端口配置已保存: %d", cfg.ServerPort)
		}
	}

	// 打印启动信息
	fmt.Printf("=== Markdown转Word工具服务器 ===\n")
	fmt.Printf("服务器端口: %d\n", cfg.ServerPort)
	fmt.Printf("Pandoc路径: %s\n", cfg.PandocPath)
	if cfg.TemplateFile != "" {
		fmt.Printf("模板文件: %s\n", cfg.TemplateFile)
	}
	fmt.Printf("================================\n")

	// 验证Pandoc配置
	if err := cfg.ValidatePandoc(); err != nil {
		fmt.Printf("警告: Pandoc配置无效: %v\n", err)
		fmt.Printf("请在设置中配置正确的Pandoc路径\n")
	} else {
		fmt.Printf("Pandoc配置验证成功\n")
	}

	// 设置路由
	mux := api.SetupRoutes(cfg)

	// 创建HTTP服务器
	server := &http.Server{
		Addr:    fmt.Sprintf(":%d", cfg.ServerPort),
		Handler: mux,
	}

	// 启动服务器
	go func() {
		fmt.Printf("服务器启动成功，监听端口 %d\n", cfg.ServerPort)
		fmt.Printf("API文档:\n")
		fmt.Printf("  健康检查: GET  http://localhost:%d/api/health\n", cfg.ServerPort)
		fmt.Printf("  获取配置: GET  http://localhost:%d/api/config\n", cfg.ServerPort)
		fmt.Printf("  更新配置: POST http://localhost:%d/api/config\n", cfg.ServerPort)
		fmt.Printf("  验证配置: POST http://localhost:%d/api/config/validate\n", cfg.ServerPort)
		fmt.Printf("  单文件转换: POST http://localhost:%d/api/convert/single\n", cfg.ServerPort)
		fmt.Printf("  批量转换: POST http://localhost:%d/api/convert/batch\n", cfg.ServerPort)
		fmt.Printf("按 Ctrl+C 停止服务器\n")

		if err := server.ListenAndServe(); err != nil && err != http.ErrServerClosed {
			log.Fatalf("服务器启动失败: %v", err)
		}
	}()

	// 等待中断信号
	quit := make(chan os.Signal, 1)
	signal.Notify(quit, syscall.SIGINT, syscall.SIGTERM)
	<-quit

	fmt.Println("\n正在关闭服务器...")
	if err := server.Close(); err != nil {
		log.Printf("服务器关闭失败: %v", err)
	} else {
		fmt.Println("服务器已关闭")
	}
}

// isPortAvailable 检查端口是否可用
func isPortAvailable(port int) bool {
	address := fmt.Sprintf(":%d", port)
	listener, err := net.Listen("tcp", address)
	if err != nil {
		return false
	}
	defer listener.Close()
	return true
}

// findAvailablePort 查找可用端口
func findAvailablePort() (int, error) {
	// 首先尝试8080-8090范围
	for port := 8080; port <= 8090; port++ {
		if isPortAvailable(port) {
			return port, nil
		}
	}

	// 如果都不可用，让系统分配一个随机端口
	listener, err := net.Listen("tcp", ":0")
	if err != nil {
		return 0, fmt.Errorf("无法获取可用端口: %v", err)
	}
	defer listener.Close()

	addr := listener.Addr().(*net.TCPAddr)
	return addr.Port, nil
}
