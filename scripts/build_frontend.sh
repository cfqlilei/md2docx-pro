#!/bin/bash

# Markdown转Word工具 - 前端构建脚本

set -e

echo "=== Markdown转Word工具前端构建 ==="
echo "开始时间: $(date)"
echo "================================"

# 项目根目录
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$PROJECT_ROOT"

echo "📁 项目目录: $PROJECT_ROOT"

# 检查Qt环境
if ! command -v qmake &> /dev/null; then
    echo "❌ Qt qmake未安装或不在PATH中"
    echo "💡 请安装Qt5并设置PATH环境变量"
    echo "   例如: export PATH=\"/opt/homebrew/opt/qt@5/bin:\$PATH\""
    exit 1
fi

echo "🔧 Qt版本: $(qmake -version)"

# 进入前端目录
cd qt-frontend

echo ""
echo "🏗️  构建前端应用..."
echo "-------------------"

# 构建主应用 (3页签版本)
echo "📱 构建主应用 (md2docx_app)..."
if [ ! -d "build_md2docx_app" ]; then
    mkdir build_md2docx_app
fi

cd build_md2docx_app
qmake ../md2docx_app.pro
make clean
make

if [ $? -eq 0 ]; then
    echo "✅ 主应用构建成功"
else
    echo "❌ 主应用构建失败"
    exit 1
fi

cd ..

# 构建简单完整版本
echo "📱 构建简单完整版本 (simple_complete)..."
if [ ! -d "build_simple_complete" ]; then
    mkdir build_simple_complete
fi

cd build_simple_complete
qmake ../simple_complete.pro
make clean
make

if [ $? -eq 0 ]; then
    echo "✅ 简单完整版本构建成功"
else
    echo "❌ 简单完整版本构建失败"
    exit 1
fi

cd ..

# 构建单文件测试版本
echo "📱 构建单文件测试版本 (single_test)..."
if [ ! -d "build_single_test" ]; then
    mkdir build_single_test
fi

cd build_single_test
qmake ../single_test.pro
make clean
make

if [ $? -eq 0 ]; then
    echo "✅ 单文件测试版本构建成功"
else
    echo "❌ 单文件测试版本构建失败"
    exit 1
fi

cd ..

echo ""
echo "📊 构建结果:"
echo "-------------------"
echo "主应用:"
ls -la build_md2docx_app/build/ 2>/dev/null || echo "  未找到构建文件"

echo ""
echo "简单完整版本:"
ls -la build_simple_complete/build/ 2>/dev/null || echo "  未找到构建文件"

echo ""
echo "单文件测试版本:"
ls -la build_single_test/build/ 2>/dev/null || echo "  未找到构建文件"

echo ""
echo "================================"
echo "✅ 前端构建完成时间: $(date)"
echo "📁 构建文件位置: qt-frontend/build_*/"
echo "================================"
