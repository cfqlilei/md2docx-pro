#!/bin/bash

# Markdown转Word工具 - 完整构建脚本

set -e

echo "=== Markdown转Word工具完整构建 ==="
echo "开始时间: $(date)"
echo "================================"

# 项目根目录
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$PROJECT_ROOT"

echo "📁 项目目录: $PROJECT_ROOT"

# 清理之前的构建
echo ""
echo "🧹 清理之前的构建..."
echo "-------------------"
rm -rf build/
rm -rf qt-frontend/build_*/

echo "✅ 清理完成"

# 构建后端
echo ""
echo "🚀 开始构建后端..."
echo "==================="
bash scripts/build_backend.sh

if [ $? -ne 0 ]; then
    echo "❌ 后端构建失败"
    exit 1
fi

# 构建前端
echo ""
echo "🖥️  开始构建前端..."
echo "==================="
bash scripts/build_frontend.sh

if [ $? -ne 0 ]; then
    echo "❌ 前端构建失败"
    exit 1
fi

# 运行测试
echo ""
echo "🧪 运行测试..."
echo "==================="
bash tests/run_all_tests.sh

if [ $? -ne 0 ]; then
    echo "⚠️  测试失败，但构建继续"
fi

echo ""
echo "================================"
echo "🎉 完整构建完成时间: $(date)"
echo ""
echo "📦 构建产物:"
echo "  后端: build/md2docx-server*"
echo "  前端: qt-frontend/build_*/"
echo ""
echo "🚀 启动应用:"
echo "  node scripts/launch_complete_app_macos.js"
echo "================================"
