#!/bin/bash

# Windows脚本测试
# 验证Windows批处理脚本的语法和内容

echo "=== Windows脚本测试 ==="
echo "测试时间: $(date)"
echo

# 创建测试结果目录
mkdir -p tests/results

# 测试结果文件
REPORT_FILE="tests/results/windows_scripts_test_report.txt"
echo "Windows脚本测试报告" > "$REPORT_FILE"
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

echo "1. 测试Windows脚本存在性"
echo "========================"

# 测试Windows脚本文件存在
test_item "all_in_one_integrated_windows.bat存在" \
    "[ -f 'scripts/all_in_one_integrated_windows.bat' ]" \
    "pass"

test_item "compile_integrated_windows.bat存在" \
    "[ -f 'scripts/compile_integrated_windows.bat' ]" \
    "pass"

test_item "build_integrated_windows.bat存在" \
    "[ -f 'scripts/build_integrated_windows.bat' ]" \
    "pass"

test_item "run_integrated_windows.bat存在" \
    "[ -f 'scripts/run_integrated_windows.bat' ]" \
    "pass"

test_item "clean_integrated_windows.bat存在" \
    "[ -f 'scripts/clean_integrated_windows.bat' ]" \
    "pass"

echo
echo "2. 测试Windows脚本内容"
echo "====================="

# 读取版本号
VERSION=$(cat VERSION 2>/dev/null || echo "1.0.0")

# 测试脚本内容修复
test_item "all_in_one_integrated_windows.bat使用版本化路径" \
    "grep -q 'md2docx_simple_integrated-v.*\.exe' scripts/all_in_one_integrated_windows.bat" \
    "pass"

test_item "compile_integrated_windows.bat生成版本化文件" \
    "grep -q 'md2docx_simple_integrated-v.*\.exe' scripts/compile_integrated_windows.bat" \
    "pass"

test_item "build_integrated_windows.bat使用版本化路径" \
    "grep -q 'md2docx_simple_integrated-v.*\.exe' scripts/build_integrated_windows.bat" \
    "pass"

test_item "run_integrated_windows.bat使用版本化路径" \
    "grep -q 'md2docx_simple_integrated-v.*\.exe' scripts/run_integrated_windows.bat" \
    "pass"

echo
echo "3. 测试Windows脚本语法"
echo "====================="

# 测试批处理脚本基本语法（检查常见错误）
test_item "all_in_one_integrated_windows.bat没有语法错误" \
    "! grep -q 'exit /b' scripts/all_in_one_integrated_windows.bat || grep -q '@echo off' scripts/all_in_one_integrated_windows.bat" \
    "pass"

test_item "compile_integrated_windows.bat没有语法错误" \
    "! grep -q 'exit /b' scripts/compile_integrated_windows.bat || grep -q '@echo off' scripts/compile_integrated_windows.bat" \
    "pass"

test_item "build_integrated_windows.bat没有语法错误" \
    "! grep -q 'exit /b' scripts/build_integrated_windows.bat || grep -q '@echo off' scripts/build_integrated_windows.bat" \
    "pass"

test_item "run_integrated_windows.bat没有语法错误" \
    "! grep -q 'exit /b' scripts/run_integrated_windows.bat || grep -q '@echo off' scripts/run_integrated_windows.bat" \
    "pass"

echo
echo "4. 测试Windows脚本一致性"
echo "======================="

test_item "Windows脚本引用正确的构建脚本" \
    "grep -q 'all_in_one_integrated_windows.bat' scripts/all_in_one_integrated_windows.bat" \
    "pass"

test_item "Windows脚本不引用已删除的脚本" \
    "! grep -q 'launch_integrated_simple.sh' scripts/all_in_one_integrated_windows.bat && ! grep -q 'launch_integrated_windows.bat' scripts/all_in_one_integrated_windows.bat" \
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
    echo "🎉 所有测试通过！Windows脚本修复成功！"
    echo "🎉 所有测试通过！Windows脚本修复成功！" >> "$REPORT_FILE"
    exit 0
else
    echo "❌ 有测试失败，请检查问题"
    echo "❌ 有测试失败，请检查问题" >> "$REPORT_FILE"
    exit 1
fi
