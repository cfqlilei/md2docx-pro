#!/bin/bash
# macOS启动脚本

echo "=== Markdown转Word工具 - macOS版本 ==="
echo "启动后端服务..."
./build/md2docx-server-macos &
BACKEND_PID=$!

echo "等待后端服务启动..."
sleep 3

echo "启动前端应用..."
./qt-frontend/build_md2docx_app/build/md2docx_app.app/Contents/MacOS/md2docx_app &
FRONTEND_PID=$!

echo "应用启动完成！"
echo "后端PID: $BACKEND_PID"
echo "前端PID: $FRONTEND_PID"
echo "按任意键退出..."
read -n 1

echo "关闭应用..."
kill $FRONTEND_PID 2>/dev/null || true
kill $BACKEND_PID 2>/dev/null || true
