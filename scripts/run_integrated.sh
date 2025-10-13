#!/bin/bash

# 整合版运行脚本
# 启动整合版应用

set -e

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# 日志函数
log() {
    echo -e "${BLUE}[$(date '+%Y-%m-%d %H:%M:%S')]${NC} $1"
}

success() {
    echo -e "${GREEN}✅ $1${NC}"
}

error() {
    echo -e "${RED}❌ $1${NC}"
}

warning() {
    echo -e "${YELLOW}⚠️  $1${NC}"
}

# 项目根目录
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$PROJECT_ROOT"

log "启动整合版应用"
log "项目目录: $PROJECT_ROOT"

# 检查应用是否存在
check_app_exists() {
    VERSION=$(cat VERSION 2>/dev/null || echo "dev")
    APP_PATH="build/release/md2docx_simple_integrated-v${VERSION}.app"

    if [ ! -d "$APP_PATH" ]; then
        error "整合版应用不存在: $APP_PATH"
        log "请先运行构建流程:"
        log "  1. ./scripts/clean_integrated.sh    # 清理"
        log "  2. ./scripts/compile_integrated.sh  # 编译"
        log "  3. ./scripts/build_integrated.sh    # 构建"
        exit 1
    fi

    success "找到整合版应用: $APP_PATH"
    return 0
}

# 检查应用完整性
check_app_integrity() {
    VERSION=$(cat VERSION 2>/dev/null || echo "dev")
    APP_PATH="build/release/md2docx_simple_integrated-v${VERSION}.app"

    # 检查前端可执行文件
    if [ ! -f "$APP_PATH/Contents/MacOS/md2docx_simple_integrated" ]; then
        error "前端可执行文件不存在"
        exit 1
    fi
    success "前端可执行文件存在"

    # 检查内嵌后端
    if [ ! -f "$APP_PATH/Contents/MacOS/md2docx-server-macos" ]; then
        error "内嵌后端服务器不存在"
        exit 1
    fi
    success "内嵌后端服务器存在"

    # 检查执行权限
    if [ ! -x "$APP_PATH/Contents/MacOS/md2docx_simple_integrated" ]; then
        warning "前端可执行文件没有执行权限，正在修复..."
        chmod +x "$APP_PATH/Contents/MacOS/md2docx_simple_integrated"
    fi

    if [ ! -x "$APP_PATH/Contents/MacOS/md2docx-server-macos" ]; then
        warning "后端可执行文件没有执行权限，正在修复..."
        chmod +x "$APP_PATH/Contents/MacOS/md2docx-server-macos"
    fi

    success "应用完整性检查通过"
}

# 显示应用信息
show_app_info() {
    VERSION=$(cat VERSION 2>/dev/null || echo "dev")
    APP_PATH="build/release/md2docx_simple_integrated-v${VERSION}.app"

    log "应用信息:"
    log "  📦 应用包大小: $(du -sh "$APP_PATH" | cut -f1)"
    log "  🖥️  前端大小: $(du -sh "$APP_PATH/Contents/MacOS/md2docx_simple_integrated" | cut -f1)"
    log "  🔧 后端大小: $(du -sh "$APP_PATH/Contents/MacOS/md2docx-server-macos" | cut -f1)"
    log "  📍 应用路径: $APP_PATH"
}

# 启动应用
launch_app() {
    VERSION=$(cat VERSION 2>/dev/null || echo "dev")
    APP_PATH="build/release/md2docx_simple_integrated-v${VERSION}.app"

    log "🚀 启动整合版应用..."

    # 使用open命令启动应用
    open "$APP_PATH"

    if [ $? -eq 0 ]; then
        success "整合版应用已启动！"
        log ""
        log "应用特点:"
        log "  ✓ 单一程序，无需分别启动前后端"
        log "  ✓ 内嵌Go后端服务，自动启动"
        log "  ✓ 动态端口分配，避免冲突"
        log "  ✓ 完整的GUI界面"
        log "  ✓ 所有功能都已整合"
        log ""
        log "使用说明:"
        log "  • 应用启动后会自动启动内嵌的后端服务器"
        log "  • 后端会自动分配可用端口，避免冲突"
        log "  • 如果看到服务器启动中的提示，请等待几秒钟"
        log "  • 状态栏会显示服务器运行状态"
        log "  • 关闭应用时会自动停止后端服务器"
        log ""
        log "如果遇到问题:"
        log "  • 检查Pandoc是否已安装: pandoc --version"
        log "  • 查看应用日志或重启应用"
        log "  • 确保端口8080-8090范围内有可用端口"
    else
        error "应用启动失败"
        exit 1
    fi
}

# 主函数
main() {
    log "=== 启动整合版应用 ==="
    
    check_app_exists
    check_app_integrity
    show_app_info
    launch_app
    
    success "=== 启动完成 ==="
}

# 执行主函数
main "$@"
