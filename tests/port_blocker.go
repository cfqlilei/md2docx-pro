package main

import (
	"fmt"
	"net"
	"os"
	"os/signal"
	"strconv"
	"syscall"
)

func main() {
	if len(os.Args) != 2 {
		fmt.Println("用法: go run port_blocker.go <端口号>")
		os.Exit(1)
	}

	port, err := strconv.Atoi(os.Args[1])
	if err != nil {
		fmt.Printf("无效的端口号: %s\n", os.Args[1])
		os.Exit(1)
	}

	// 监听指定端口
	address := fmt.Sprintf(":%d", port)
	listener, err := net.Listen("tcp", address)
	if err != nil {
		fmt.Printf("无法监听端口 %d: %v\n", port, err)
		os.Exit(1)
	}
	defer listener.Close()

	fmt.Printf("端口 %d 已被占用，按 Ctrl+C 停止\n", port)

	// 等待中断信号
	quit := make(chan os.Signal, 1)
	signal.Notify(quit, syscall.SIGINT, syscall.SIGTERM)

	// 在后台接受连接（但不处理）
	go func() {
		for {
			conn, err := listener.Accept()
			if err != nil {
				return
			}
			conn.Close()
		}
	}()

	// 等待信号
	<-quit
	fmt.Printf("\n端口 %d 释放\n", port)
}
