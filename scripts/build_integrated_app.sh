#!/bin/bash

# 整合版应用构建脚本
# 构建包含内嵌后端的单一程序

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

log "开始构建整合版应用"
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

# 构建Qt整合版前端
build_integrated_frontend() {
    log "构建Qt整合版前端..."
    
    cd qt-frontend
    
    # 清理之前的构建
    rm -rf build_integrated
    mkdir -p build_integrated
    cd build_integrated
    
    # 构建整合版
    log "构建整合版应用..."
    qmake ../md2docx_integrated.pro
    make
    
    if [ $? -eq 0 ]; then
        success "整合版前端构建完成"
    else
        error "整合版前端构建失败"
        cd "$PROJECT_ROOT"
        exit 1
    fi
    
    # 验证构建结果
    if [ -d "release/md2docx_integrated.app" ]; then
        success "macOS整合版应用包创建成功: release/md2docx_integrated.app"
        
        # 检查后端文件是否正确复制
        if [ -f "release/md2docx_integrated.app/Contents/MacOS/md2docx-server-macos" ]; then
            success "后端服务器已嵌入到应用包中"
        else
            warning "后端服务器未正确嵌入，手动复制..."
            mkdir -p release/md2docx_integrated.app/Contents/MacOS/
            cp ../../build/md2docx-server-macos release/md2docx_integrated.app/Contents/MacOS/
        fi
    else
        error "macOS整合版应用包创建失败"
        cd "$PROJECT_ROOT"
        exit 1
    fi
    
    cd "$PROJECT_ROOT"
}

# 测试整合版应用
test_integrated_app() {
    log "测试整合版应用..."
    
    # 检查应用包结构
    APP_PATH="qt-frontend/build_integrated/release/md2docx_integrated.app"
    
    if [ -d "$APP_PATH" ]; then
        success "应用包存在: $APP_PATH"
        
        # 检查可执行文件
        if [ -f "$APP_PATH/Contents/MacOS/md2docx_integrated" ]; then
            success "前端可执行文件存在"
        else
            error "前端可执行文件不存在"
        fi
        
        # 检查嵌入的后端
        if [ -f "$APP_PATH/Contents/MacOS/md2docx-server-macos" ]; then
            success "嵌入的后端服务器存在"
        else
            error "嵌入的后端服务器不存在"
        fi
        
        # 检查应用包大小
        APP_SIZE=$(du -sh "$APP_PATH" | cut -f1)
        log "应用包大小: $APP_SIZE"
        
    else
        error "应用包不存在"
        return 1
    fi
}

# 创建启动脚本
create_launch_script() {
    log "创建启动脚本..."
    
    # 创建简单的启动脚本
    cat > launch_integrated.sh << 'EOF'
#!/bin/bash
# 整合版启动脚本

echo "=== Markdown转Word工具 - 整合版 ==="
echo "启动整合版应用..."

APP_PATH="qt-frontend/build_integrated/release/md2docx_integrated.app"

if [ -d "$APP_PATH" ]; then
    open "$APP_PATH"
    echo "应用已启动！"
else
    echo "错误: 应用包不存在，请先运行构建脚本"
    echo "运行: ./scripts/build_integrated_app.sh"
    exit 1
fi
EOF

    chmod +x launch_integrated.sh
    success "创建整合版启动脚本: launch_integrated.sh"
}

# 创建部署脚本
create_deploy_script() {
    log "创建部署脚本..."
    
    cat > deploy_integrated.sh << 'EOF'
#!/bin/bash
# 整合版部署脚本

echo "=== 部署整合版应用 ==="

APP_PATH="qt-frontend/build_integrated/release/md2docx_integrated.app"

if [ ! -d "$APP_PATH" ]; then
    echo "错误: 应用包不存在，请先构建"
    exit 1
fi

echo "使用macdeployqt部署依赖..."
macdeployqt "$APP_PATH"

echo "创建DMG安装包..."
mkdir -p dist
rm -f dist/md2docx_integrated.dmg

# 创建临时目录
TEMP_DIR=$(mktemp -d)
cp -R "$APP_PATH" "$TEMP_DIR/"

# 创建DMG
hdiutil create -volname "Markdown转Word工具" -srcfolder "$TEMP_DIR" -ov -format UDZO dist/md2docx_integrated.dmg

# 清理
rm -rf "$TEMP_DIR"

echo "✅ DMG安装包创建完成: dist/md2docx_integrated.dmg"
EOF

    chmod +x deploy_integrated.sh
    success "创建部署脚本: deploy_integrated.sh"
}

# 主函数
main() {
    log "=== 整合版应用构建开始 ==="
    
    check_environment
    build_backend
    build_integrated_frontend
    test_integrated_app
    create_launch_script
    create_deploy_script
    
    success "=== 整合版构建完成 ==="
    log "构建结果:"
    log "  整合版应用: qt-frontend/build_integrated/release/md2docx_integrated.app"
    log "  应用大小: $(du -sh qt-frontend/build_integrated/release/md2docx_integrated.app | cut -f1)"
    log "  启动脚本: launch_integrated.sh"
    log "  部署脚本: deploy_integrated.sh"
    log ""
    log "启动方式:"
    log "  直接启动: open qt-frontend/build_integrated/release/md2docx_integrated.app"
    log "  脚本启动: ./launch_integrated.sh"
    log ""
    log "部署方式:"
    log "  创建DMG: ./deploy_integrated.sh"
    log ""
    success "现在您有了一个包含前后端的单一程序！"
}

# 执行主函数
main "$@"
