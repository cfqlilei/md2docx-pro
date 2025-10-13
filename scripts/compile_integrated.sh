#!/bin/bash

# 整合版编译脚本
# 编译Go后端和Qt前端

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

log "开始编译整合版应用"
log "项目目录: $PROJECT_ROOT"

# 检查环境
check_environment() {
    log "检查编译环境..."
    
    # 检查Go
    if ! command -v go &> /dev/null; then
        error "Go未安装，请先安装Go 1.25+"
        exit 1
    fi
    success "Go版本: $(go version)"
    
    # 检查qmake
    if ! qmake --version &> /dev/null; then
        error "qmake未找到，请确保Qt 5.15已安装并设置PATH"
        log "请设置Qt路径，例如: export PATH=/opt/homebrew/opt/qt@5/bin:\$PATH"
        log "当前PATH: $PATH"
        exit 1
    fi
    success "qmake版本: $(qmake --version | head -1)"
    
    # 检查pandoc
    if ! command -v pandoc &> /dev/null; then
        warning "Pandoc未找到，运行时可能需要手动配置路径"
    else
        success "Pandoc版本: $(pandoc --version | head -1)"
    fi
}

# 编译Go后端
compile_backend() {
    log "编译Go后端..."
    
    # 创建构建目录
    mkdir -p build/release
    
    # 更新依赖
    log "更新Go依赖..."
    go mod tidy
    
    # 编译macOS版本
    log "编译macOS后端..."
    CGO_ENABLED=1 GOOS=darwin GOARCH=arm64 go build -buildvcs=false -o build/release/md2docx-server-macos ./cmd/server
    if [ $? -eq 0 ]; then
        success "macOS后端编译完成 ($(du -sh build/release/md2docx-server-macos | cut -f1))"
    else
        error "macOS后端编译失败"
        exit 1
    fi
    
    # 编译Windows版本（交叉编译）
    log "编译Windows后端..."
    CGO_ENABLED=1 GOOS=windows GOARCH=amd64 go build -buildvcs=false -o build/release/md2docx-server-windows.exe ./cmd/server
    if [ $? -eq 0 ]; then
        success "Windows后端编译完成 ($(du -sh build/release/md2docx-server-windows.exe | cut -f1))"
    else
        error "Windows后端编译失败"
        exit 1
    fi
}

# 编译Qt前端
compile_frontend() {
    log "编译Qt整合版前端..."

    cd qt-frontend

    # 创建构建目录
    mkdir -p build_simple_integrated
    cd build_simple_integrated

    # 生成Makefile
    log "生成Makefile..."
    qmake ../md2docx_simple_integrated.pro
    if [ $? -ne 0 ]; then
        error "qmake失败"
        cd "$PROJECT_ROOT"
        exit 1
    fi

    # 编译
    log "编译Qt应用..."
    make
    if [ $? -eq 0 ]; then
        success "Qt前端编译完成"

        # 将编译好的应用移动到build/release目录
        log "移动应用到build/release目录..."
        if [ -d "build_simple_integrated/release/md2docx_simple_integrated.app" ]; then
            cp -r build_simple_integrated/release/md2docx_simple_integrated.app "$PROJECT_ROOT/build/release/"
            success "应用已移动到 build/release/md2docx_simple_integrated.app"
        elif [ -f "build_simple_integrated/release/md2docx_simple_integrated" ]; then
            cp build_simple_integrated/release/md2docx_simple_integrated "$PROJECT_ROOT/build/release/"
            success "应用已移动到 build/release/md2docx_simple_integrated"
        else
            warning "未找到编译好的应用文件"
        fi
    else
        error "Qt前端编译失败"
        cd "$PROJECT_ROOT"
        exit 1
    fi

    cd "$PROJECT_ROOT"
}

# 验证编译结果
verify_build() {
    log "验证编译结果..."
    
    # 检查后端文件
    if [ -f "build/release/md2docx-server-macos" ]; then
        success "macOS后端: build/release/md2docx-server-macos ($(du -sh build/release/md2docx-server-macos | cut -f1))"
    else
        error "macOS后端文件不存在"
        exit 1
    fi

    if [ -f "build/release/md2docx-server-windows.exe" ]; then
        success "Windows后端: build/release/md2docx-server-windows.exe ($(du -sh build/release/md2docx-server-windows.exe | cut -f1))"
    else
        error "Windows后端文件不存在"
        exit 1
    fi
    
    # 检查前端应用包
    if [ -d "build/release/md2docx_simple_integrated.app" ]; then
        success "macOS前端: build/release/md2docx_simple_integrated.app ($(du -sh build/release/md2docx_simple_integrated.app | cut -f1))"

        # 检查内嵌的后端
        if [ -f "build/release/md2docx_simple_integrated.app/Contents/MacOS/md2docx-server-macos" ]; then
            success "内嵌后端已正确复制到应用包中"
        else
            warning "内嵌后端未找到，手动复制..."
            mkdir -p "build/release/md2docx_simple_integrated.app/Contents/MacOS/"
            cp build/release/md2docx-server-macos "build/release/md2docx_simple_integrated.app/Contents/MacOS/"
            success "手动复制内嵌后端完成"
        fi
    elif [ -f "build/release/md2docx_simple_integrated" ]; then
        success "前端可执行文件: build/release/md2docx_simple_integrated ($(du -sh build/release/md2docx_simple_integrated | cut -f1))"
    else
        error "前端文件不存在"
        exit 1
    fi
}

# 主函数
main() {
    log "=== 整合版编译开始 ==="
    
    check_environment
    compile_backend
    compile_frontend
    verify_build
    
    success "=== 编译完成 ==="
    log "编译结果:"
    log "  macOS后端: build/release/md2docx-server-macos"
    log "  Windows后端: build/release/md2docx-server-windows.exe"
    log "  macOS整合版应用: build/release/md2docx_simple_integrated.app"
    log ""
    log "下一步: 运行 ./scripts/build_integrated.sh 构建最终应用"
}

# 执行主函数
main "$@"
