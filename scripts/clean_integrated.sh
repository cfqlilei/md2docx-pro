#!/bin/bash

# 整合版清理脚本
# 清理所有构建文件和临时文件，统一使用build目录

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

warning() {
    echo -e "${YELLOW}⚠️  $1${NC}"
}

# 项目根目录
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$PROJECT_ROOT"

log "开始清理整合版构建文件"
log "项目目录: $PROJECT_ROOT"

# 清理build目录（统一的构建输出目录）
log "清理build目录..."
if [ -d "build" ]; then
    rm -rf build
    success "删除 build/ 目录"
else
    warning "build/ 目录不存在"
fi

# 重新创建build目录结构
mkdir -p build/bin build/intermediate/go build/intermediate/qt
success "重新创建 build/ 目录结构"

# 清理Go模块缓存
log "清理Go模块缓存..."
go clean -cache 2>/dev/null || true
go clean -modcache 2>/dev/null || true
success "Go缓存清理完成"

# 清理Qt构建文件
log "清理Qt前端构建文件..."

# 清理所有Qt构建目录（这些目录现在由 qmake 在 build/intermediate/qt 中生成）
BUILD_DIRS=(
    "qt-frontend/build_simple_integrated"
    "qt-frontend/build_md2docx_app"
    "qt-frontend/build_integrated"
    "qt-frontend/build"
    "qt-frontend/debug"
    "qt-frontend/release"
)

for dir in "${BUILD_DIRS[@]}"; do
    if [ -d "$dir" ]; then
        rm -rf "$dir"
        success "删除 $dir/ 目录"
    fi
done

# 清理 Qt 生成的 Makefile 和临时文件
log "清理 Qt 生成的临时文件..."
find qt-frontend -maxdepth 1 -name "Makefile*" -delete 2>/dev/null || true
find qt-frontend -maxdepth 1 -name ".qmake.stash" -delete 2>/dev/null || true
success "Qt 临时文件清理完成"

# 清理临时文件
log "清理临时文件..."

# 清理macOS临时文件
find . -name ".DS_Store" -delete 2>/dev/null || true
find . -name "*.tmp" -delete 2>/dev/null || true
find . -name "*.log" -delete 2>/dev/null || true

# 清理Qt临时文件
find qt-frontend -name "Makefile*" -delete 2>/dev/null || true
find qt-frontend -name "*.o" -delete 2>/dev/null || true
find qt-frontend -name "moc_*" -delete 2>/dev/null || true
find qt-frontend -name "qrc_*" -delete 2>/dev/null || true
find qt-frontend -name "ui_*" -delete 2>/dev/null || true

success "临时文件清理完成"

# 清理启动脚本和多余文件
log "清理生成的启动脚本和多余文件..."
LAUNCH_SCRIPTS=(
    "launch_macos.sh"
    "launch_windows.bat"
    "deploy_integrated.sh"
    "BUILD_REPORT.md"
    "launch_integrated_simple.sh"
    "launch_integrated_windows.bat"
)

for script in "${LAUNCH_SCRIPTS[@]}"; do
    if [ -f "$script" ]; then
        rm -f "$script"
        success "删除 $script"
    fi
done

# 清理 build/bin 目录中的多余文件（保留最终的二进制文件）
log "清理 build/bin 目录中的多余文件..."
if [ -d "build/bin" ]; then
    # 删除 Windows 构建说明文件
    rm -f build/bin/BUILD_WINDOWS.md
    success "清理多余文件完成"
fi

# 清理测试结果
log "清理测试结果..."
if [ -d "tests/results" ]; then
    rm -rf tests/results
    mkdir -p tests/results
    success "清理测试结果目录"
fi

# 清理分发文件
log "清理分发文件..."
if [ -d "dist" ]; then
    rm -rf dist
    success "删除 dist/ 目录"
fi

# 清理配置文件
log "清理配置文件..."
rm -f config.json
success "删除配置文件"

success "=== 清理完成 ==="
log "已清理的内容:"
log "  ✓ build/ 目录（统一构建输出）"
log "    - build/bin/ 中的应用和二进制文件"
log "    - build/intermediate/ 中的中间文件"
log "  ✓ Go模块缓存"
log "  ✓ Qt前端构建文件"
log "  ✓ 临时文件和缓存"
log "  ✓ 生成的启动脚本"
log "  ✓ 测试结果文件"
log "  ✓ 分发文件"
log "  ✓ 配置文件"
log ""
log "现在可以进行全新构建了！"
