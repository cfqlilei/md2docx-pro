#!/bin/bash

# 整合版应用启动脚本
# 启动包含内嵌后端的单一程序

echo "=== Markdown转Word工具 - 整合版 ==="
echo "启动整合版应用..."

APP_PATH="qt-frontend/build_simple_integrated/build_simple_integrated/release/md2docx_simple_integrated.app"

if [ -d "$APP_PATH" ]; then
    echo "✅ 找到整合版应用: $APP_PATH"
    echo "📦 应用大小: $(du -sh "$APP_PATH" | cut -f1)"
    echo "🚀 启动应用..."
    
    open "$APP_PATH"
    
    echo "✅ 整合版应用已启动！"
    echo ""
    echo "特点："
    echo "  ✓ 单一程序，无需分别启动前后端"
    echo "  ✓ 内嵌Go后端服务，自动启动"
    echo "  ✓ 完整的GUI界面"
    echo "  ✓ 所有功能都已整合"
    echo ""
    echo "如果应用启动后显示服务器错误，请等待几秒钟让内嵌服务器完全启动。"
else
    echo "❌ 错误: 整合版应用不存在"
    echo "请先构建整合版应用:"
    echo "  cd qt-frontend"
    echo "  mkdir -p build_simple_integrated && cd build_simple_integrated"
    echo "  qmake ../md2docx_simple_integrated.pro && make"
    exit 1
fi
