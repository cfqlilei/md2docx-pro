#!/bin/bash

# 整合版一次性执行脚本
# 清理 -> 编译 -> 构建 -> 运行

set -e

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
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

step() {
    echo -e "${PURPLE}🔄 $1${NC}"
}

# 项目根目录
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$PROJECT_ROOT"

# 显示欢迎信息
show_welcome() {
    echo -e "${PURPLE}"
    echo "╔══════════════════════════════════════════════════════════════╗"
    echo "║                Markdown转Word工具 - 整合版                    ║"
    echo "║                    一次性构建和运行脚本                        ║"
    echo "╚══════════════════════════════════════════════════════════════╝"
    echo -e "${NC}"
    log "项目目录: $PROJECT_ROOT"
    log "开始完整的构建流程..."
    echo ""
}

# 执行步骤并处理错误
execute_step() {
    local step_name="$1"
    local script_path="$2"
    local description="$3"
    
    step "$step_name: $description"
    
    if [ ! -f "$script_path" ]; then
        error "脚本不存在: $script_path"
        exit 1
    fi
    
    if ! chmod +x "$script_path"; then
        error "无法设置执行权限: $script_path"
        exit 1
    fi
    
    if "$script_path"; then
        success "$step_name 完成"
        echo ""
    else
        error "$step_name 失败"
        log "请检查错误信息并修复问题后重试"
        exit 1
    fi
}

# 显示进度
show_progress() {
    local current=$1
    local total=$2
    local step_name="$3"
    
    local percent=$((current * 100 / total))
    local filled=$((current * 50 / total))
    local empty=$((50 - filled))
    
    printf "\r${BLUE}进度: ["
    printf "%${filled}s" | tr ' ' '█'
    printf "%${empty}s" | tr ' ' '░'
    printf "] %d%% - %s${NC}" $percent "$step_name"
    
    if [ $current -eq $total ]; then
        echo ""
    fi
}

# 主函数
main() {
    show_welcome
    
    local total_steps=4
    local current_step=0
    
    # 步骤1: 清理
    current_step=$((current_step + 1))
    show_progress $current_step $total_steps "清理构建文件"
    execute_step "步骤1" "./scripts/clean_integrated.sh" "清理所有构建文件和临时文件"
    
    # 步骤2: 编译
    current_step=$((current_step + 1))
    show_progress $current_step $total_steps "编译前后端"
    execute_step "步骤2" "./scripts/compile_integrated.sh" "编译Go后端和Qt前端"
    
    # 步骤3: 构建
    current_step=$((current_step + 1))
    show_progress $current_step $total_steps "构建应用包"
    execute_step "步骤3" "./scripts/build_integrated.sh" "构建完整的应用包"
    
    # 步骤4: 运行
    current_step=$((current_step + 1))
    show_progress $current_step $total_steps "启动应用"
    execute_step "步骤4" "./scripts/run_integrated.sh" "启动整合版应用"
    
    # 显示完成信息
    echo -e "${GREEN}"
    echo "╔══════════════════════════════════════════════════════════════╗"
    echo "║                        🎉 构建完成！                          ║"
    echo "╚══════════════════════════════════════════════════════════════╝"
    echo -e "${NC}"
    
    success "整合版应用已成功构建并启动！"
    log ""
    log "构建结果:"
    
    APP_PATH="qt-frontend/build_simple_integrated/build_simple_integrated/release/md2docx_simple_integrated.app"
    if [ -d "$APP_PATH" ]; then
        log "  📦 应用包: $APP_PATH"
        log "  📏 大小: $(du -sh "$APP_PATH" | cut -f1)"
        log "  🚀 状态: 已启动"
    fi
    
    log ""
    log "下次启动方式:"
    log "  快速启动: ./launch_integrated_simple.sh"
    log "  完整重建: ./scripts/all_in_one_integrated.sh"
    log ""
    log "应用特点:"
    log "  ✓ 单一程序，无需分别启动前后端"
    log "  ✓ 内嵌Go后端服务，自动启动"
    log "  ✓ 完整的GUI界面，所有功能已整合"
    log "  ✓ 用户友好，双击即可使用"
    
    success "=== 全部完成 ==="
}

# 捕获中断信号
trap 'echo -e "\n${RED}构建被中断${NC}"; exit 1' INT TERM

# 执行主函数
main "$@"
