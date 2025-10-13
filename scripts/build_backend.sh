#!/bin/bash

# Markdown转Word工具 - 后端构建脚本

set -e

echo "=== Markdown转Word工具后端构建 ==="
echo "开始时间: $(date)"
echo "================================"

# 项目根目录
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$PROJECT_ROOT"

echo "📁 项目目录: $PROJECT_ROOT"

# 创建构建目录
mkdir -p build

# 检查Go环境
if ! command -v go &> /dev/null; then
    echo "❌ Go未安装或不在PATH中"
    exit 1
fi

echo "🔧 Go版本: $(go version)"

# 构建不同平台的后端
echo ""
echo "🏗️  构建后端服务器..."
echo "-------------------"

# macOS ARM64
echo "📱 构建 macOS ARM64..."
CGO_ENABLED=1 GOOS=darwin GOARCH=arm64 go build -buildvcs=false -o build/md2docx-server-macos-arm64 ./cmd/server
if [ $? -eq 0 ]; then
    echo "✅ macOS ARM64 构建成功"
else
    echo "❌ macOS ARM64 构建失败"
    exit 1
fi

# macOS AMD64
echo "💻 构建 macOS AMD64..."
CGO_ENABLED=1 GOOS=darwin GOARCH=amd64 go build -buildvcs=false -o build/md2docx-server-macos-amd64 ./cmd/server
if [ $? -eq 0 ]; then
    echo "✅ macOS AMD64 构建成功"
else
    echo "❌ macOS AMD64 构建失败"
    exit 1
fi

# Linux AMD64
echo "🐧 构建 Linux AMD64..."
CGO_ENABLED=1 GOOS=linux GOARCH=amd64 go build -buildvcs=false -o build/md2docx-server-linux-amd64 ./cmd/server
if [ $? -eq 0 ]; then
    echo "✅ Linux AMD64 构建成功"
else
    echo "❌ Linux AMD64 构建失败"
    exit 1
fi

# Windows AMD64
echo "🪟 构建 Windows AMD64..."
CGO_ENABLED=1 GOOS=windows GOARCH=amd64 go build -buildvcs=false -o build/md2docx-server-windows-amd64.exe ./cmd/server
if [ $? -eq 0 ]; then
    echo "✅ Windows AMD64 构建成功"
else
    echo "❌ Windows AMD64 构建失败"
    exit 1
fi

# 创建当前平台的符号链接
echo ""
echo "🔗 创建当前平台符号链接..."
CURRENT_OS=$(uname -s | tr '[:upper:]' '[:lower:]')
CURRENT_ARCH=$(uname -m)

case $CURRENT_ARCH in
    x86_64) CURRENT_ARCH="amd64" ;;
    arm64) CURRENT_ARCH="arm64" ;;
    aarch64) CURRENT_ARCH="arm64" ;;
esac

if [ "$CURRENT_OS" = "darwin" ]; then
    ln -sf "md2docx-server-macos-$CURRENT_ARCH" build/md2docx-server
    echo "✅ 创建符号链接: md2docx-server -> md2docx-server-macos-$CURRENT_ARCH"
elif [ "$CURRENT_OS" = "linux" ]; then
    ln -sf "md2docx-server-linux-$CURRENT_ARCH" build/md2docx-server
    echo "✅ 创建符号链接: md2docx-server -> md2docx-server-linux-$CURRENT_ARCH"
fi

echo ""
echo "📊 构建结果:"
echo "-------------------"
ls -la build/md2docx-server*

echo ""
echo "================================"
echo "✅ 后端构建完成时间: $(date)"
echo "📁 构建文件位置: build/"
echo "================================"
