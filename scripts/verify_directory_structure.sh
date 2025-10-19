#!/bin/bash

# 验证目录结构规范化脚本
# 检查所有文件是否按照新的规范存放

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

log "开始验证目录结构规范化"
log "项目目录: $PROJECT_ROOT"
echo ""

# 检查计数
CHECKS_PASSED=0
CHECKS_FAILED=0

# 检查函数
check_directory() {
    local dir=$1
    local description=$2
    
    if [ -d "$dir" ]; then
        success "✓ 目录存在: $dir ($description)"
        ((CHECKS_PASSED++))
    else
        error "✗ 目录不存在: $dir ($description)"
        ((CHECKS_FAILED++))
    fi
}

check_file() {
    local file=$1
    local description=$2
    
    if [ -f "$file" ]; then
        success "✓ 文件存在: $file ($description)"
        ((CHECKS_PASSED++))
    else
        warning "⚠ 文件不存在: $file ($description)"
        ((CHECKS_FAILED++))
    fi
}

# 验证源代码目录
log "=== 验证源代码目录 ==="
check_directory "cmd" "Go 命令行程序"
check_directory "internal" "Go 内部包"
check_directory "pkg" "Go 公共包"
check_directory "qt-frontend/src" "Qt 前端源代码"
check_directory "web" "Web 资源"
check_file "go.mod" "Go 模块文件"
check_file "go.sum" "Go 模块锁定文件"
echo ""

# 验证编译产物目录
log "=== 验证编译产物目录 ==="
check_directory "build" "编译产物根目录"
check_directory "build/bin" "最终的二进制文件目录"
check_directory "build/intermediate" "中间文件目录"
check_directory "build/intermediate/go" "Go 编译中间文件"
check_directory "build/intermediate/qt" "Qt 编译中间文件"
echo ""

# 验证测试目录
log "=== 验证测试目录 ==="
check_directory "tests" "测试根目录"
check_directory "tests/unit" "单元测试"
check_directory "tests/integration" "集成测试"
check_directory "tests/e2e" "端到端测试"
check_directory "tests/testdata" "测试数据"
check_directory "tests/results" "测试结果"
check_directory "tests/utils" "测试工具"
echo ""

# 验证脚本目录
log "=== 验证脚本目录 ==="
check_directory "scripts" "构建脚本目录"
check_file "scripts/compile_integrated.sh" "Mac 编译脚本"
check_file "scripts/compile_integrated_windows.bat" "Windows 编译脚本"
check_file "scripts/clean_integrated.sh" "Mac 清理脚本"
check_file "scripts/clean_integrated_windows.bat" "Windows 清理脚本"
echo ""

# 验证文档目录
log "=== 验证文档目录 ==="
check_directory "docs" "文档目录"
echo ""

# 验证 Qt 项目文件
log "=== 验证 Qt 项目文件 ==="
check_file "qt-frontend/md2docx_simple_integrated.pro" "Qt 简单整合版项目文件"
check_file "qt-frontend/md2docx_integrated.pro" "Qt 整合版项目文件"
check_file "qt-frontend/md2docx_app.pro" "Qt 应用项目文件"
echo ""

# 验证版本文件
log "=== 验证版本文件 ==="
check_file "VERSION" "版本号文件"
echo ""

# 显示验证结果
echo ""
log "=== 验证结果 ==="
log "通过检查: $CHECKS_PASSED"
log "失败检查: $CHECKS_FAILED"
echo ""

if [ $CHECKS_FAILED -eq 0 ]; then
    success "=== 所有检查都通过了！目录结构规范化成功 ==="
    exit 0
else
    warning "=== 有 $CHECKS_FAILED 个检查失败，请检查上述错误 ==="
    exit 1
fi

