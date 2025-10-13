#!/bin/bash

# UI改进测试脚本
# 测试设置界面状态信息显示和启动脚本清理

set -e

# 颜色定义
GREEN='\033[0;32m'
RED='\033[0;31m'
BLUE='\033[0;34m'
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

log "开始UI改进测试"

# 测试1: 检查启动脚本清理
log "测试1: 检查启动脚本清理"

if [ ! -f "launch_integrated_simple.sh" ]; then
    success "已删除多余的启动脚本: launch_integrated_simple.sh"
else
    error "多余的启动脚本仍然存在: launch_integrated_simple.sh"
    exit 1
fi

if [ ! -f "launch_integrated_windows.bat" ]; then
    success "已删除多余的启动脚本: launch_integrated_windows.bat"
else
    error "多余的启动脚本仍然存在: launch_integrated_windows.bat"
    exit 1
fi

# 检查保留的启动脚本
if [ -f "launch_integrated.sh" ]; then
    success "保留了主要的macOS启动脚本: launch_integrated.sh"
else
    error "主要的macOS启动脚本丢失: launch_integrated.sh"
    exit 1
fi

if [ -f "launch_integrated.bat" ]; then
    success "保留了主要的Windows启动脚本: launch_integrated.bat"
else
    error "主要的Windows启动脚本丢失: launch_integrated.bat"
    exit 1
fi

# 测试2: 检查README.md文件
log "测试2: 检查README.md文件"

if [ -f "README.md" ]; then
    success "README.md文件已创建"
    
    # 检查关键内容
    if grep -q "项目概况" README.md; then
        success "README.md包含项目概况"
    else
        error "README.md缺少项目概况"
        exit 1
    fi
    
    if grep -q "技术栈" README.md; then
        success "README.md包含技术栈说明"
    else
        error "README.md缺少技术栈说明"
        exit 1
    fi
    
    if grep -q "功能说明" README.md; then
        success "README.md包含功能说明"
    else
        error "README.md缺少功能说明"
        exit 1
    fi
    
    if grep -q "编译说明" README.md; then
        success "README.md包含编译说明"
    else
        error "README.md缺少编译说明"
        exit 1
    fi
    
else
    error "README.md文件不存在"
    exit 1
fi

# 测试3: 检查BUILD.md是否已删除
log "测试3: 检查BUILD.md是否已删除"

if [ ! -f "BUILD.md" ]; then
    success "BUILD.md文件已删除"
else
    error "BUILD.md文件仍然存在，应该已被删除"
    exit 1
fi

# 测试4: 检查Qt前端代码修改
log "测试4: 检查Qt前端代码修改"

if [ -f "qt-frontend/src/settingswidget.cpp" ]; then
    success "settingswidget.cpp文件存在"
    
    # 检查showStatus方法的修改
    if grep -q "insertHtml(htmlMessage)" qt-frontend/src/settingswidget.cpp; then
        success "showStatus方法包含HTML插入代码"
    else
        error "showStatus方法缺少HTML插入代码"
        exit 1
    fi
    
    if grep -q "ensureCursorVisible" qt-frontend/src/settingswidget.cpp; then
        success "showStatus方法包含滚动到底部的代码"
    else
        error "showStatus方法缺少滚动到底部的代码"
        exit 1
    fi
    
else
    error "settingswidget.cpp文件不存在"
    exit 1
fi

# 测试5: 检查启动脚本内容
log "测试5: 检查启动脚本内容"

if grep -q "应用特点" launch_integrated.sh; then
    success "macOS启动脚本包含应用特点描述"
else
    error "macOS启动脚本缺少应用特点描述"
    exit 1
fi

if grep -q "动态端口分配" launch_integrated.sh; then
    success "macOS启动脚本提到动态端口分配"
else
    error "macOS启动脚本未提到动态端口分配"
    exit 1
fi

if grep -q "内嵌Go后端服务" launch_integrated.sh; then
    success "macOS启动脚本提到内嵌后端服务"
else
    error "macOS启动脚本未提到内嵌后端服务"
    exit 1
fi

log "所有UI改进测试通过！"

# 生成测试报告
REPORT_FILE="tests/results/ui_improvements_test_report.txt"
mkdir -p tests/results

cat > "$REPORT_FILE" << EOF
UI改进测试报告
================

测试时间: $(date)
测试结果: 通过

测试项目:
1. ✅ 启动脚本清理 - 删除了多余的启动脚本
2. ✅ README.md创建 - 包含完整的项目信息
3. ✅ BUILD.md删除 - 旧文件已清理
4. ✅ Qt代码修改 - 状态信息显示改进
5. ✅ 启动脚本内容 - 描述信息更新

改进内容:
- 修复了设置界面验证配置状态信息显示问题，确保每个日志都有换行
- 清理了多余的启动脚本，只保留功能完整的版本
- 将BUILD.md重命名为README.md并完善了项目信息
- 更新了相关文档中的脚本引用

所有测试项目均通过，UI改进成功完成。
EOF

success "测试报告已生成: $REPORT_FILE"
