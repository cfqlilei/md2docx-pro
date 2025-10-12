package main

import (
	"fmt"
	"log"
	"net/http"
	"os"
	"os/signal"
	"syscall"

	"md2docx/internal/api"
	"md2docx/internal/config"
)

func main() {
	// 加载配置
	cfg, err := config.Load()
	if err != nil {
		log.Fatalf("加载配置失败: %v", err)
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
