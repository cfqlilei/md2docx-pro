#!/bin/bash

# 完整应用构建脚本
# 构建前后端完整版本

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

log "开始构建完整应用"
log "项目目录: $PROJECT_ROOT"

# 检查环境
check_environment() {
    log "检查构建环境..."
    
    # 检查Go
    if ! command -v go &> /dev/null; then
        error "Go未安装，请先安装Go 1.25+"
        exit 1
    fi
    success "Go版本: $(go version)"
    
    # 检查qmake
    if ! command -v qmake &> /dev/null; then
        error "qmake未找到，请确保Qt 5.15已安装并设置PATH"
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

# 构建Go后端
build_backend() {
    log "构建Go后端..."
    
    # 清理并创建构建目录
    rm -rf build
    mkdir -p build
    
    # 更新依赖
    go mod tidy
    
    # 构建macOS版本
    log "构建macOS后端..."
    CGO_ENABLED=1 GOOS=darwin GOARCH=arm64 go build -buildvcs=false -o build/md2docx-server-macos ./cmd/server
    success "macOS后端构建完成"
    
    # 构建Windows版本（交叉编译）
    log "构建Windows后端..."
    CGO_ENABLED=1 GOOS=windows GOARCH=amd64 go build -buildvcs=false -o build/md2docx-server-windows.exe ./cmd/server
    success "Windows后端构建完成"
    
    # 验证构建结果
    ls -la build/
}

# 构建Qt前端
build_frontend() {
    log "构建Qt前端..."
    
    cd qt-frontend
    
    # 清理之前的构建
    rm -rf build_md2docx_app
    mkdir -p build_md2docx_app
    cd build_md2docx_app
    
    # 构建macOS版本
    log "构建macOS前端..."
    qmake ../md2docx_app.pro
    make
    success "macOS前端构建完成"
    
    # 验证构建结果
    if [ -d "build/md2docx_app.app" ]; then
        success "macOS应用包创建成功: build/md2docx_app.app"
    else
        error "macOS应用包创建失败"
        exit 1
    fi
    
    cd "$PROJECT_ROOT"
}

# 测试构建结果
test_build() {
    log "测试构建结果..."
    
    # 测试后端
    if [ -f "build/md2docx-server-macos" ]; then
        success "macOS后端文件存在"
        # 简单测试启动
        timeout 5s ./build/md2docx-server-macos &
        sleep 2
        if curl -s http://localhost:8080/api/health > /dev/null; then
            success "macOS后端健康检查通过"
        else
            warning "macOS后端健康检查失败，可能需要手动测试"
        fi
        pkill -f md2docx-server-macos || true
    else
        error "macOS后端文件不存在"
    fi
    
    # 测试前端
    if [ -d "qt-frontend/build_md2docx_app/build/md2docx_app.app" ]; then
        success "macOS前端应用包存在"
    else
        error "macOS前端应用包不存在"
    fi
}

# 创建启动脚本
create_launch_scripts() {
    log "创建启动脚本..."
    
    # 创建简单的启动脚本
    cat > launch_macos.sh << 'EOF'
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
EOF

    chmod +x launch_macos.sh
    success "创建macOS启动脚本: launch_macos.sh"
}

# 主函数
main() {
    log "=== 完整应用构建开始 ==="
    
    check_environment
    build_backend
    build_frontend
    test_build
    create_launch_scripts
    
    success "=== 构建完成 ==="
    log "构建文件:"
    log "  后端 (macOS): build/md2docx-server-macos"
    log "  后端 (Windows): build/md2docx-server-windows.exe"
    log "  前端 (macOS): qt-frontend/build_md2docx_app/build/md2docx_app.app"
    log "  启动脚本: launch_macos.sh"
    log ""
    log "启动方式:"
    log "  Node.js脚本: node scripts/launch_complete_app_macos.js"
    log "  简单脚本: ./launch_macos.sh"
    log ""
    log "Windows版本需要在Windows环境下构建前端部分"
}

# 执行主函数
main "$@"
