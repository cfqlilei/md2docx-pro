#!/bin/bash

# 整合版应用启动脚本

set -e

# 颜色定义
GREEN='\033[0;32m'
BLUE='\033[0;34m'
RED='\033[0;31m'
NC='\033[0m'

log() {
    echo -e "${BLUE}[$(date '+%H:%M:%S')]${NC} $1"
}

success() {
    echo -e "${GREEN}✅ $1${NC}"
}

error() {
    echo -e "${RED}❌ $1${NC}"
}

log "启动整合版应用"

APP_PATH="build/release/md2docx_simple_integrated-v1.0.0.app"

if [ -d "$APP_PATH" ]; then
    success "找到整合版应用: $APP_PATH"
    log "🚀 启动应用..."
    open "$APP_PATH"
    success "整合版应用已启动！"
    echo ""
    log "应用特点:"
    log "  ✓ 单一程序，无需分别启动前后端"
    log "  ✓ 内嵌Go后端服务，自动启动"
    log "  ✓ 动态端口分配，避免冲突"
    log "  ✓ 完整的GUI界面"
    log "  ✓ 所有功能都已整合"
else
    error "整合版应用不存在: $APP_PATH"
    log "请先运行构建脚本:"
    log "  ./scripts/compile_integrated.sh"
    log "  ./scripts/build_integrated.sh"
    exit 1
fi
