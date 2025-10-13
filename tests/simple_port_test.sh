#!/bin/bash

# 简单的动态端口测试脚本

set -e

# 颜色定义
GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

log() {
    echo -e "${BLUE}[$(date '+%H:%M:%S')]${NC} $1"
}

success() {
    echo -e "${GREEN}✅ $1${NC}"
}

# 项目根目录
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$PROJECT_ROOT"

log "=== 简单动态端口测试 ==="

# 1. 清理旧配置
log "清理旧配置文件..."
rm -f config.json
rm -f qt-frontend/build_simple_integrated/build_simple_integrated/release/md2docx_simple_integrated.app/Contents/MacOS/config.json

# 2. 直接测试后端
log "测试后端动态端口分配..."
cd build
./md2docx-server-macos &
SERVER_PID=$!
cd ..

# 等待服务器启动
sleep 3

# 检查配置文件
if [ -f "config.json" ]; then
    success "配置文件已创建"
    cat config.json
    
    # 提取端口号
    PORT=$(grep -o '"server_port": *[0-9]*' config.json | grep -o '[0-9]*')
    log "检测到端口: $PORT"
    
    # 测试端口
    if curl -s "http://localhost:$PORT/api/health" > /dev/null; then
        success "端口 $PORT 访问正常"
    else
        log "端口 $PORT 访问失败"
    fi
else
    log "配置文件未创建"
fi

# 停止服务器
kill $SERVER_PID 2>/dev/null || true
sleep 2

log "=== 测试完成 ==="
