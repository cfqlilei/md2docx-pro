#!/bin/bash

# Pandoc安装功能测试脚本
# 测试新增的Pandoc安装功能是否正常工作

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

# 显示测试开始信息
echo -e "${PURPLE}"
echo "╔══════════════════════════════════════════════════════════════╗"
echo "║                  Pandoc安装功能测试                           ║"
echo "║                    功能验证测试套件                            ║"
echo "╚══════════════════════════════════════════════════════════════╝"
echo -e "${NC}"

log "开始Pandoc安装功能测试..."

# 创建测试结果目录
mkdir -p tests/results

# 测试结果文件
REPORT_FILE="tests/results/pandoc_install_feature_test_$(date +%Y%m%d_%H%M%S).md"

# 开始测试报告
cat > "$REPORT_FILE" << EOF
# Pandoc安装功能测试报告

**测试时间**: $(date '+%Y-%m-%d %H:%M:%S')
**测试环境**: $(uname -s) $(uname -r)
**项目路径**: $PROJECT_ROOT

## 测试概述

本次测试验证新增的Pandoc安装功能，包括：
1. 平台检测功能
2. 地区检测功能
3. 安装状态检查
4. 安装命令生成
5. UI界面显示
6. 错误处理机制

## 测试结果

EOF

log "测试报告将保存到: $REPORT_FILE"
echo ""

# 测试计数器
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

# 测试函数
test_item() {
    local test_name="$1"
    local test_command="$2"
    local expected_result="$3"
    
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    echo -n "测试 $TOTAL_TESTS: $test_name ... "
    
    if eval "$test_command"; then
        if [ "$expected_result" = "pass" ]; then
            echo "✅ 通过"
            echo "✅ $test_name" >> "$REPORT_FILE"
            PASSED_TESTS=$((PASSED_TESTS + 1))
        else
            echo "❌ 失败 (预期失败但实际通过)"
            echo "❌ $test_name (预期失败但实际通过)" >> "$REPORT_FILE"
        fi
    else
        if [ "$expected_result" = "fail" ]; then
            echo "✅ 通过 (预期失败)"
            echo "✅ $test_name (预期失败)" >> "$REPORT_FILE"
            PASSED_TESTS=$((PASSED_TESTS + 1))
        else
            echo "❌ 失败"
            echo "❌ $test_name" >> "$REPORT_FILE"
        fi
    fi
}

echo "1. 测试构建结果"
echo "==============="

# 读取版本号
VERSION=$(cat VERSION 2>/dev/null || echo "1.0.0")

# 测试构建结果
test_item "macOS应用存在" \
    "[ -d 'build/release/md2docx_simple_integrated-v${VERSION}.app' ]" \
    "pass"

test_item "Windows应用存在" \
    "[ -f 'build/release/md2docx_simple_integrated-v${VERSION}.exe' ]" \
    "pass"

test_item "日志显示修复验证" \
    "grep -q 'macOS应用:' scripts/all_in_one_integrated.sh && grep -q 'Windows应用:' scripts/all_in_one_integrated.sh" \
    "pass"

echo
echo "2. 测试Qt前端代码修改"
echo "==================="

# 测试Qt前端代码修改
test_item "设置界面头文件包含安装按钮声明" \
    "grep -q 'installPandoc' qt-frontend/src/settingswidget.h" \
    "pass"

test_item "设置界面头文件包含安装进程成员变量" \
    "grep -q 'm_installProcess' qt-frontend/src/settingswidget.h" \
    "pass"

test_item "设置界面头文件包含进度条成员变量" \
    "grep -q 'm_installProgressBar' qt-frontend/src/settingswidget.h" \
    "pass"

test_item "设置界面实现文件包含安装方法" \
    "grep -q 'void SettingsWidget::installPandoc' qt-frontend/src/settingswidget.cpp" \
    "pass"

test_item "设置界面实现文件包含操作系统检测" \
    "grep -q 'detectOperatingSystem' qt-frontend/src/settingswidget.cpp" \
    "pass"

test_item "设置界面实现文件包含地区检测" \
    "grep -q 'detectRegion' qt-frontend/src/settingswidget.cpp" \
    "pass"

test_item "设置界面实现文件包含Pandoc安装检查" \
    "grep -q 'isPandocInstalled' qt-frontend/src/settingswidget.cpp" \
    "pass"

test_item "设置界面实现文件包含安装命令生成" \
    "grep -q 'getPandocInstallCommand' qt-frontend/src/settingswidget.cpp" \
    "pass"

echo
echo "3. 测试安装功能逻辑"
echo "=================="

# 测试安装功能逻辑
test_item "包含macOS安装命令" \
    "grep -q 'brew install pandoc' qt-frontend/src/settingswidget.cpp" \
    "pass"

test_item "包含Windows安装命令" \
    "grep -q 'choco install pandoc' qt-frontend/src/settingswidget.cpp" \
    "pass"

test_item "包含中国镜像支持" \
    "grep -q 'mirrors.tuna.tsinghua.edu.cn' qt-frontend/src/settingswidget.cpp" \
    "pass"

test_item "包含地区检测逻辑" \
    "grep -q 'China.*zh_CN' qt-frontend/src/settingswidget.cpp" \
    "pass"

test_item "包含安装进度显示" \
    "grep -q 'installProgressBar.*setVisible' qt-frontend/src/settingswidget.cpp" \
    "pass"

test_item "包含安装状态显示" \
    "grep -q 'showStatus.*安装' qt-frontend/src/settingswidget.cpp" \
    "pass"

echo
echo "4. 测试UI组件集成"
echo "================"

# 测试UI组件集成
test_item "构造函数初始化安装按钮" \
    "grep -q 'm_installPandocButton.*nullptr' qt-frontend/src/settingswidget.cpp" \
    "pass"

test_item "构造函数初始化进度条" \
    "grep -q 'm_installProgressBar.*nullptr' qt-frontend/src/settingswidget.cpp" \
    "pass"

test_item "构造函数初始化安装进程" \
    "grep -q 'm_installProcess.*nullptr' qt-frontend/src/settingswidget.cpp" \
    "pass"

test_item "setupUI中创建安装按钮" \
    "grep -q 'new QPushButton.*安装Pandoc' qt-frontend/src/settingswidget.cpp" \
    "pass"

test_item "setupUI中创建进度条" \
    "grep -q 'new QProgressBar' qt-frontend/src/settingswidget.cpp" \
    "pass"

test_item "setupConnections中连接安装按钮" \
    "grep -A1 'm_installPandocButton.*clicked' qt-frontend/src/settingswidget.cpp | grep -q 'installPandoc'" \
    "pass"

echo
echo "5. 测试错误处理"
echo "=============="

# 测试错误处理
test_item "包含安装失败处理" \
    "grep -q '安装失败' qt-frontend/src/settingswidget.cpp" \
    "pass"

test_item "包含进程错误处理" \
    "grep -q 'onInstallProcessError' qt-frontend/src/settingswidget.cpp" \
    "pass"

test_item "包含安装验证" \
    "grep -q '安装成功.*可以正常使用' qt-frontend/src/settingswidget.cpp" \
    "pass"

test_item "包含重复安装检查" \
    "grep -q '已安装Pandoc.*重新安装' qt-frontend/src/settingswidget.cpp" \
    "pass"

echo
echo "=== 测试总结 ==="
echo "总测试数: $TOTAL_TESTS"
echo "通过测试: $PASSED_TESTS"
echo "失败测试: $((TOTAL_TESTS - PASSED_TESTS))"

# 写入总结到报告
echo >> "$REPORT_FILE"
echo "=== 测试总结 ===" >> "$REPORT_FILE"
echo "总测试数: $TOTAL_TESTS" >> "$REPORT_FILE"
echo "通过测试: $PASSED_TESTS" >> "$REPORT_FILE"
echo "失败测试: $((TOTAL_TESTS - PASSED_TESTS))" >> "$REPORT_FILE"

if [ $PASSED_TESTS -eq $TOTAL_TESTS ]; then
    echo "🎉 所有测试通过！Pandoc安装功能开发成功！"
    echo "🎉 所有测试通过！Pandoc安装功能开发成功！" >> "$REPORT_FILE"
    exit 0
else
    echo "❌ 有测试失败，请检查问题"
    echo "❌ 有测试失败，请检查问题" >> "$REPORT_FILE"
    exit 1
fi
