#!/bin/bash

# 完整的Markdown转Word工具启动脚本

echo "=== Markdown转Word工具 - 完整版启动 ==="
echo "启动时间: $(date '+%Y-%m-%d %H:%M:%S')"
echo "=========================================="

# 检查Go后端
echo "1. 检查Go后端..."
if ! command -v go >/dev/null 2>&1; then
    echo "❌ 错误: 未找到Go，请安装Go语言环境"
    exit 1
fi

echo "✅ Go版本: $(go version)"

# 检查Pandoc
echo
echo "2. 检查Pandoc..."
if ! command -v pandoc >/dev/null 2>&1; then
    echo "❌ 错误: 未找到Pandoc，请安装Pandoc"
    echo "可以使用: brew install pandoc"
    exit 1
fi

echo "✅ Pandoc版本: $(pandoc --version | head -n 1)"

# 检查Qt应用程序
echo
echo "3. 检查Qt前端..."
QT_APP="qt-frontend/build/md2docx.app/Contents/MacOS/md2docx"

if [ ! -f "$QT_APP" ]; then
    echo "❌ 错误: Qt前端未找到，请先构建Qt应用程序"
    echo "运行: ./build_simple.sh"
    exit 1
fi

echo "✅ Qt前端已就绪"

# 启动Go后端服务
echo
echo "4. 启动Go后端服务..."
echo "正在启动后端服务器..."

# 启动后端服务（后台运行）
go run cmd/server/main.go &
BACKEND_PID=$!

echo "后端服务PID: $BACKEND_PID"

# 等待后端启动
echo "等待后端服务启动..."
sleep 3

# 检查后端是否正常启动
echo "检查后端服务状态..."
if curl -s http://localhost:8080/api/health >/dev/null 2>&1; then
    echo "✅ 后端服务启动成功"
else
    echo "⚠️  后端服务可能还在启动中，继续等待..."
    sleep 2
    if curl -s http://localhost:8080/api/health >/dev/null 2>&1; then
        echo "✅ 后端服务启动成功"
    else
        echo "❌ 后端服务启动失败"
        kill $BACKEND_PID 2>/dev/null
        exit 1
    fi
fi

# 启动Qt前端
echo
echo "5. 启动Qt前端应用..."
echo "正在启动图形界面..."

# 启动Qt应用程序
open "$QT_APP" || {
    echo "❌ 无法启动Qt应用程序"
    kill $BACKEND_PID 2>/dev/null
    exit 1
}

echo "✅ Qt前端已启动"

echo
echo "=========================================="
echo "应用程序启动完成！"
echo
echo "使用说明:"
echo "1. Qt图形界面已打开，可以进行文件转换操作"
echo "2. 后端服务运行在: http://localhost:8080"
echo "3. 关闭Qt应用程序时，后端服务会自动停止"
echo
echo "如需手动停止后端服务:"
echo "  kill $BACKEND_PID"
echo
echo "日志信息:"
echo "  后端服务PID: $BACKEND_PID"
echo "  Qt应用程序: $QT_APP"
echo "=========================================="

# 等待Qt应用程序结束
echo "等待应用程序结束..."

# 监控Qt应用程序
while pgrep -f "md2docx.app" >/dev/null 2>&1; do
    sleep 2
done

echo
echo "Qt应用程序已关闭，正在清理后端服务..."

# 清理后端进程
if kill $BACKEND_PID 2>/dev/null; then
    echo "✅ 后端服务已停止"
else
    echo "⚠️  后端服务可能已经停止"
fi

echo
echo "=========================================="
echo "应用程序已完全退出"
echo "结束时间: $(date '+%Y-%m-%d %H:%M:%S')"
echo "感谢使用Markdown转Word工具！"
echo "=========================================="
