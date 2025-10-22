#!/bin/bash

# 整合版构建脚本
# 将编译好的组件打包成最终的整合版应用

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

log "开始整合版构建流程"
log "项目目录: $PROJECT_ROOT"

# 构建macOS版本
build_macos() {
    log "构建macOS整合版..."

    # 获取版本信息
    VERSION=$(cat VERSION 2>/dev/null || echo "dev")

    # 检查编译结果是否存在
    if [ ! -d "build/bin/md2docx_simple_integrated.app" ]; then
        error "未找到编译结果，请先运行 ./scripts/compile_integrated.sh"
        exit 1
    fi

    if [ ! -f "build/bin/md2docx-server" ]; then
        error "未找到后端服务器，请先运行 ./scripts/compile_integrated.sh"
        exit 1
    fi

    # 创建发布目录
    mkdir -p build/release

    # 复制应用包到发布目录并重命名
    RELEASE_APP="build/release/md2docx_simple_integrated-v${VERSION}.app"

    log "复制应用包到发布目录..."
    cp -R "build/bin/md2docx_simple_integrated.app" "$RELEASE_APP"

    # 将后端服务器复制到应用包内并重命名
    log "集成后端服务器到应用包..."
    cp "build/bin/md2docx-server" "$RELEASE_APP/Contents/MacOS/md2docx-server-macos"

    # 设置执行权限
    chmod +x "$RELEASE_APP/Contents/MacOS/md2docx_simple_integrated"
    chmod +x "$RELEASE_APP/Contents/MacOS/md2docx-server-macos"

    # 验证构建结果
    if [ -f "$RELEASE_APP/Contents/MacOS/md2docx_simple_integrated" ]; then
        success "前端可执行文件已集成"
    else
        error "前端可执行文件集成失败"
        exit 1
    fi

    if [ -f "$RELEASE_APP/Contents/MacOS/md2docx-server-macos" ]; then
        success "后端服务器已集成"
    else
        error "后端服务器集成失败"
        exit 1
    fi

    success "macOS整合版构建完成: $RELEASE_APP"
}

# 构建Windows版本（创建Windows可执行文件）
build_windows() {
    log "构建Windows整合版文件..."

    # 获取版本信息
    VERSION=$(cat VERSION 2>/dev/null || echo "dev")

    # 检查Windows后端是否存在
    if [ -f "build/bin/md2docx-server.exe" ]; then
        # 创建Windows可执行文件（包含后端）
        log "创建Windows整合版可执行文件..."

        # 复制Windows后端作为主程序
        cp "build/bin/md2docx-server.exe" "build/release/md2docx_simple_integrated-v${VERSION}.exe"

        success "Windows整合版应用已创建: build/release/md2docx_simple_integrated-v${VERSION}.exe"
        success "Windows版本包含完整的后端服务器，可以通过命令行或Web界面使用"
    else
        warning "Windows后端文件不存在，跳过Windows版本构建"
        log "如需Windows版本，请确保编译步骤包含Windows后端编译"
    fi
}

# 创建启动脚本
create_launch_scripts() {
    log "创建启动脚本..."

    # 获取版本信息
    VERSION=$(cat VERSION 2>/dev/null || echo "dev")

    # 创建macOS启动脚本
    cat > launch_integrated.sh << EOF
#!/bin/bash

# 整合版应用启动脚本

set -e

# 颜色定义
GREEN='\033[0;32m'
BLUE='\033[0;34m'
RED='\033[0;31m'
NC='\033[0m'

log() {
    echo -e "\${BLUE}[\$(date '+%H:%M:%S')]\${NC} \$1"
}

success() {
    echo -e "\${GREEN}✅ \$1\${NC}"
}

error() {
    echo -e "\${RED}❌ \$1\${NC}"
}

log "启动整合版应用"

APP_PATH="build/release/md2docx_simple_integrated-v${VERSION}.app"

if [ -d "\$APP_PATH" ]; then
    success "找到整合版应用: \$APP_PATH"
    log "🚀 启动应用..."
    open "\$APP_PATH"
    success "整合版应用已启动！"
    echo ""
    log "应用特点:"
    log "  ✓ 单一程序，无需分别启动前后端"
    log "  ✓ 内嵌Go后端服务，自动启动"
    log "  ✓ 动态端口分配，避免冲突"
    log "  ✓ 完整的GUI界面"
    log "  ✓ 所有功能都已整合"
else
    error "整合版应用不存在: \$APP_PATH"
    log "请先运行构建脚本:"
    log "  ./scripts/compile_integrated.sh"
    log "  ./scripts/build_integrated.sh"
    exit 1
fi
EOF

    chmod +x launch_integrated.sh
    success "创建macOS启动脚本: launch_integrated.sh"

    # 创建Windows启动脚本
    cat > launch_integrated.bat << EOF
@echo off
echo === Markdown转Word工具 - 整合版 ===
echo 启动整合版应用...

set APP_PATH=build\\release\\md2docx_simple_integrated-v${VERSION}.exe

if exist "%APP_PATH%" (
    echo ✅ 找到整合版应用: %APP_PATH%
    echo 🚀 启动应用...
    start "" "%APP_PATH%"
    echo ✅ 整合版应用已启动！
    echo.
    echo 特点：
    echo   ✓ 单一程序，包含完整的后端服务器
    echo   ✓ 可通过Web界面访问 (http://localhost:8080)
    echo   ✓ 支持命令行参数
    echo   ✓ 自动端口分配，避免冲突
) else (
    echo ❌ 错误: 整合版应用不存在
    echo 请先运行构建脚本:
    echo   scripts\\all_in_one_integrated.sh
    pause
)
EOF

    success "创建Windows启动脚本: launch_integrated.bat"
}

# 生成构建报告
generate_build_report() {
    log "生成构建报告..."
    
    REPORT_FILE="BUILD_REPORT.md"
    
    cat > "$REPORT_FILE" << EOF
# 整合版构建报告

生成时间: $(date)

## 构建结果

### macOS版本
EOF
    
    VERSION=$(cat VERSION 2>/dev/null || echo "dev")
    APP_PATH="build/release/md2docx_simple_integrated-v${VERSION}.app"
    if [ -d "$APP_PATH" ]; then
        echo "- ✅ 应用包: $APP_PATH" >> "$REPORT_FILE"
        echo "- 📦 大小: $(du -sh "$APP_PATH" | cut -f1)" >> "$REPORT_FILE"
        echo "- 🔧 前端: $(du -sh "$APP_PATH/Contents/MacOS/md2docx_simple_integrated" | cut -f1)" >> "$REPORT_FILE"
        echo "- 🔧 后端: $(du -sh "$APP_PATH/Contents/MacOS/md2docx-server-macos" | cut -f1)" >> "$REPORT_FILE"
    else
        echo "- ❌ 应用包构建失败" >> "$REPORT_FILE"
    fi
    
    cat >> "$REPORT_FILE" << EOF

### Windows版本
- ✅ Windows整合版应用: build/release/md2docx_simple_integrated-v${VERSION}.exe

## 启动方式

### macOS
\`\`\`bash
./launch_integrated.sh
\`\`\`

### Windows
\`\`\`cmd
launch_integrated.bat
\`\`\`

## 文件结构

\`\`\`
md2docx-src/
├── build/release/
│   └── md2docx_simple_integrated-v${VERSION}.app # macOS整合版应用（包含内嵌后端）
├── launch_integrated.sh              # macOS启动脚本
└── launch_integrated.bat             # Windows启动脚本
\`\`\`

## 特点

- ✅ 单一程序，无需分别启动前后端
- ✅ 内嵌Go后端服务，自动启动
- ✅ 动态端口分配，避免冲突
- ✅ 完整的GUI界面
- ✅ 所有功能都已整合
EOF
    
    success "构建报告已生成: $REPORT_FILE"
}

# 主函数
main() {
    log "=== 整合版构建开始 ==="
    
    build_macos
    build_windows
    create_launch_scripts
    generate_build_report

    # 清理所有临时文件
    log "清理临时文件..."
    rm -f build/release/md2docx-server-windows-temp.exe
    rm -f build/release/md2docx-server-macos-temp
    success "临时文件清理完成"

    success "=== 构建完成 ==="
    log "构建结果:"
    log "  ✅ macOS整合版应用已就绪"
    log "  ✅ Windows整合版应用已就绪"
    log "  ✅ 启动脚本已创建"
    log "  ✅ 构建报告已生成"
    log ""
    log "启动方式:"
    log "  macOS: ./launch_integrated.sh"
    log "  Windows: launch_integrated.bat"
}

# 执行主函数
main "$@"
