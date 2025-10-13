#!/bin/bash

# 构建系统修复测试脚本
# 测试所有构建脚本修复是否正确

echo "=== 构建系统修复测试 ==="
echo "测试时间: $(date)"
echo "测试目录: $(pwd)"
echo

# 创建测试结果目录
mkdir -p tests/results

# 测试结果文件
REPORT_FILE="tests/results/build_system_fix_test_report.txt"
echo "构建系统修复测试报告" > "$REPORT_FILE"
echo "测试时间: $(date)" >> "$REPORT_FILE"
echo "==============================" >> "$REPORT_FILE"
echo >> "$REPORT_FILE"

# 测试计数器
TOTAL_TESTS=0
PASSED_TESTS=0

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

echo "1. 测试构建结果文件"
echo "==================="

# 读取版本号
VERSION=$(cat VERSION 2>/dev/null || echo "1.0.0")

# 测试应该存在的文件
test_item "带版本号的macOS整合版应用存在" \
    "[ -d 'build/release/md2docx_simple_integrated-v${VERSION}.app' ]" \
    "pass"

test_item "应用包内前端可执行文件存在" \
    "[ -f 'build/release/md2docx_simple_integrated-v${VERSION}.app/Contents/MacOS/md2docx_simple_integrated' ]" \
    "pass"

test_item "应用包内后端服务器存在" \
    "[ -f 'build/release/md2docx_simple_integrated-v${VERSION}.app/Contents/MacOS/md2docx-server-macos' ]" \
    "pass"

echo
echo "2. 测试不应该存在的多余文件"
echo "=========================="

# 测试不应该存在的文件
test_item "不带版本号的后端程序不存在" \
    "[ ! -f 'build/release/md2docx-server-macos' ]" \
    "pass"

test_item "不带版本号的Windows后端程序不存在" \
    "[ ! -f 'build/release/md2docx-server-windows.exe' ]" \
    "pass"

test_item "不带版本号的整合版程序不存在" \
    "[ ! -f 'build/release/md2docx_simple_integrated' ]" \
    "pass"

test_item "不带版本号的整合版应用不存在" \
    "[ ! -d 'build/release/md2docx_simple_integrated.app' ]" \
    "pass"

test_item "带版本号的独立后端程序不存在" \
    "[ ! -f 'build/release/md2docx-server-macos-v${VERSION}' ]" \
    "pass"

test_item "带版本号的独立Windows后端程序不存在" \
    "[ ! -f 'build/release/md2docx-server-windows-v${VERSION}.exe' ]" \
    "pass"

test_item "Windows整合版应用存在" \
    "[ -f 'build/release/md2docx_simple_integrated-v${VERSION}.exe' ]" \
    "pass"

echo
echo "3. 测试脚本修复"
echo "==============="

# 测试脚本内容修复
test_item "run_integrated.sh使用版本化路径" \
    "grep -q 'md2docx_simple_integrated-v.*\.app' scripts/run_integrated.sh" \
    "pass"

test_item "build_integrated.sh引用正确的启动脚本" \
    "grep -q 'launch_integrated.sh' scripts/build_integrated.sh && ! grep -q 'launch_integrated_simple.sh' scripts/build_integrated.sh" \
    "pass"

test_item "all_in_one_integrated_windows.bat使用版本化路径" \
    "grep -q 'md2docx_simple_integrated-v.*\.exe' scripts/all_in_one_integrated_windows.bat" \
    "pass"

test_item "compile_integrated_windows.bat使用临时后端文件" \
    "grep -q 'md2docx-server-windows-temp.exe' scripts/compile_integrated_windows.bat" \
    "pass"

echo
echo "4. 测试清理脚本"
echo "==============="

test_item "clean_integrated.sh包含多余文件清理" \
    "grep -q 'md2docx-server-macos' scripts/clean_integrated.sh && grep -q 'md2docx_simple_integrated' scripts/clean_integrated.sh" \
    "pass"

test_item "clean_integrated_windows.bat包含多余文件清理" \
    "grep -q 'md2docx-server-macos' scripts/clean_integrated_windows.bat && grep -q 'md2docx_simple_integrated' scripts/clean_integrated_windows.bat" \
    "pass"

echo
echo "5. 测试启动脚本"
echo "==============="

test_item "launch_integrated.sh存在" \
    "[ -f 'launch_integrated.sh' ]" \
    "pass"

test_item "launch_integrated.bat存在" \
    "[ -f 'launch_integrated.bat' ]" \
    "pass"

test_item "多余的启动脚本不存在" \
    "[ ! -f 'launch_integrated_simple.sh' ] && [ ! -f 'launch_integrated_windows.bat' ]" \
    "pass"

echo
echo "6. 测试文档"
echo "==========="

test_item "README.md存在" \
    "[ -f 'README.md' ]" \
    "pass"

test_item "BUILD.md不存在" \
    "[ ! -f 'BUILD.md' ]" \
    "pass"

test_item "BUILD_WINDOWS.md不存在" \
    "[ ! -f 'build/release/BUILD_WINDOWS.md' ]" \
    "pass"

test_item "README.md包含项目概况" \
    "grep -q '项目概况' README.md && grep -q '技术栈' README.md" \
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
    echo "🎉 所有测试通过！构建系统修复成功！"
    echo "🎉 所有测试通过！构建系统修复成功！" >> "$REPORT_FILE"
    exit 0
else
    echo "❌ 有测试失败，请检查问题"
    echo "❌ 有测试失败，请检查问题" >> "$REPORT_FILE"
    exit 1
fi
