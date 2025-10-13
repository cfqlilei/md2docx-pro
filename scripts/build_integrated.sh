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

    # 检查是否已编译
    if [ -d "build/release/md2docx_simple_integrated.app" ]; then
        log "发现已存在的构建，继续构建流程..."
    else
        error "未找到编译结果，请先运行 ./scripts/compile_integrated.sh"
        exit 1
    fi

    # 验证应用包完整性
    if [ -f "build/release/md2docx_simple_integrated.app/Contents/MacOS/md2docx_simple_integrated" ]; then
        success "前端可执行文件存在"
    else
        error "前端可执行文件不存在"
        exit 1
    fi

    if [ -f "build/release/md2docx_simple_integrated.app/Contents/MacOS/md2docx-server-macos" ]; then
        success "内嵌后端服务器存在"
    else
        warning "内嵌后端服务器不存在，正在复制..."
        if [ -f "build/release/md2docx-server-macos" ]; then
            mkdir -p "build/release/md2docx_simple_integrated.app/Contents/MacOS/"
            cp build/release/md2docx-server-macos "build/release/md2docx_simple_integrated.app/Contents/MacOS/"
            success "内嵌后端服务器复制完成"
        else
            error "后端服务器文件不存在，请先运行 ./scripts/compile_integrated.sh"
            exit 1
        fi
    fi

    # 设置执行权限
    chmod +x "build/release/md2docx_simple_integrated.app/Contents/MacOS/md2docx_simple_integrated"
    chmod +x "build/release/md2docx_simple_integrated.app/Contents/MacOS/md2docx-server-macos"

    success "macOS整合版构建完成"
}

# 构建Windows版本（准备文件）
build_windows() {
    log "准备Windows整合版文件..."

    # 检查Windows后端
    if [ -f "build/release/md2docx-server-windows.exe" ]; then
        success "Windows后端已准备就绪: build/release/md2docx-server-windows.exe"
    else
        error "Windows后端不存在，请先运行 ./scripts/compile_integrated.sh"
        exit 1
    fi

    # 创建Windows构建说明
    cat > "build/release/BUILD_WINDOWS.md" << 'EOF'
# Windows整合版构建说明

## 前提条件
- Windows 10 或更高版本
- Qt 5.15.x (MSVC 2019)
- Visual Studio 2019 或更高版本

## 构建步骤

1. 打开Qt命令提示符
2. 进入项目目录
3. 运行Windows构建脚本：

```cmd
scripts\compile_integrated_windows.bat
scripts\build_integrated_windows.bat
```

## 注意事项
- 确保Qt路径正确设置
- 可能需要运行 windeployqt 部署依赖
- 最终应用将输出到 build\release\ 目录
EOF

    success "Windows构建说明已创建: build/release/BUILD_WINDOWS.md"
    warning "Windows版本需要在Windows环境下完成Qt前端构建"
}

# 创建启动脚本
create_launch_scripts() {
    log "创建启动脚本..."

    # 创建macOS启动脚本
    cat > launch_integrated.sh << 'EOF'
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

APP_PATH="build/md2docx_simple_integrated.app"

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
EOF

    chmod +x launch_integrated.sh
    success "创建macOS启动脚本: launch_integrated.sh"

    # 创建Windows启动脚本
    cat > launch_integrated.bat << 'EOF'
@echo off
echo === Markdown转Word工具 - 整合版 ===
echo 启动整合版应用...

set APP_PATH=build\release\md2docx_simple_integrated.exe

if exist "%APP_PATH%" (
    echo ✅ 找到整合版应用: %APP_PATH%
    echo 🚀 启动应用...
    start "" "%APP_PATH%"
    echo ✅ 整合版应用已启动！
    echo.
    echo 特点：
    echo   ✓ 单一程序，无需分别启动前后端
    echo   ✓ 内嵌后端服务，自动启动
    echo   ✓ 动态端口分配，避免冲突
    echo   ✓ 完整的GUI界面
    echo   ✓ 所有功能都已整合
) else (
    echo ❌ 错误: 整合版应用不存在
    echo 请先运行构建脚本:
    echo   scripts\compile_integrated_windows.bat
    echo   scripts\build_integrated_windows.bat
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
    
    APP_PATH="qt-frontend/build_simple_integrated/build_simple_integrated/release/md2docx_simple_integrated.app"
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
- ✅ 后端: build/md2docx-server-windows.exe ($(du -sh build/md2docx-server-windows.exe | cut -f1))
- ⚠️  前端: 需要在Windows环境下构建

## 启动方式

### macOS
\`\`\`bash
./launch_integrated_simple.sh
\`\`\`

### Windows
\`\`\`cmd
launch_integrated_windows.bat
\`\`\`

## 文件结构

\`\`\`
md2docx-src/
├── build/
│   ├── md2docx-server-macos          # macOS后端
│   ├── md2docx-server-windows.exe    # Windows后端
│   └── windows_integrated/           # Windows构建文件
├── qt-frontend/build_simple_integrated/build_simple_integrated/release/
│   └── md2docx_simple_integrated.app # macOS整合版应用
├── launch_integrated_simple.sh       # macOS启动脚本
└── launch_integrated_windows.bat     # Windows启动脚本
\`\`\`
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
    
    success "=== 构建完成 ==="
    log "构建结果:"
    log "  ✅ macOS整合版应用已就绪"
    log "  ✅ Windows构建文件已准备"
    log "  ✅ 启动脚本已创建"
    log "  ✅ 构建报告已生成"
    log ""
    log "启动方式:"
    log "  macOS: ./launch_integrated_simple.sh"
    log "  Windows: launch_integrated_windows.bat (需要先在Windows下构建)"
}

# 执行主函数
main "$@"
