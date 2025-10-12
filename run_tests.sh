#!/bin/bash

# Markdown转Word工具 - 自动化测试脚本

echo "=== Markdown转Word工具 - 自动化测试 ==="
echo "开始时间: $(date '+%Y-%m-%d %H:%M:%S')"
echo "=========================================="

# 获取项目根目录
PROJECT_ROOT=$(pwd)
echo "项目根目录: $PROJECT_ROOT"
echo

# 创建测试结果目录
mkdir -p tests/results

# 测试结果文件
REPORT_FILE="tests/results/test_report_$(date '+%Y%m%d_%H%M%S').txt"

# 初始化测试结果
TOTAL_TESTS=0
PASSED_TESTS=0

# 记录测试开始
echo "Markdown转Word工具 - 测试报告" > "$REPORT_FILE"
echo "================================" >> "$REPORT_FILE"
echo "生成时间: $(date '+%Y-%m-%d %H:%M:%S')" >> "$REPORT_FILE"
echo "" >> "$REPORT_FILE"

# 函数：运行测试并记录结果
run_test() {
    local package=$1
    local name=$2
    
    echo "运行测试: $name ($package)"
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    
    # 运行测试
    if go test -v "$package" > temp_test_output.txt 2>&1; then
        echo "✅ 通过: $name"
        PASSED_TESTS=$((PASSED_TESTS + 1))
        echo "包: $package" >> "$REPORT_FILE"
        echo "状态: 通过" >> "$REPORT_FILE"
    else
        echo "❌ 失败: $name"
        echo "包: $package" >> "$REPORT_FILE"
        echo "状态: 失败" >> "$REPORT_FILE"
    fi
    
    echo "输出:" >> "$REPORT_FILE"
    cat temp_test_output.txt >> "$REPORT_FILE"
    echo "$(printf '%*s' 50 | tr ' ' '-')" >> "$REPORT_FILE"
    echo "" >> "$REPORT_FILE"
    
    rm -f temp_test_output.txt
}

# 1. 运行单元测试
echo "1. 运行单元测试..."
run_test "./internal/config" "配置管理模块测试"
run_test "./pkg/utils" "工具函数测试"
run_test "./internal/converter" "转换器测试"
run_test "./internal/api" "API处理器测试"

echo

# 2. 检查Pandoc是否可用
echo "2. 检查Pandoc可用性..."
if command -v pandoc >/dev/null 2>&1; then
    echo "✅ 检测到Pandoc，运行集成测试..."
    export RUN_INTEGRATION_TESTS=1
    run_test "./tests" "集成测试"
else
    echo "⚠️  未检测到Pandoc，跳过集成测试"
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    echo "包: ./tests" >> "$REPORT_FILE"
    echo "状态: 跳过（Pandoc未安装）" >> "$REPORT_FILE"
    echo "输出: 跳过集成测试，因为系统未安装Pandoc" >> "$REPORT_FILE"
    echo "$(printf '%*s' 50 | tr ' ' '-')" >> "$REPORT_FILE"
    echo "" >> "$REPORT_FILE"
fi

echo

# 3. 生成测试覆盖率报告
echo "3. 生成测试覆盖率报告..."
TOTAL_TESTS=$((TOTAL_TESTS + 1))

if go test -cover ./internal/... ./pkg/... > temp_coverage.txt 2>&1; then
    echo "✅ 覆盖率报告生成成功"
    PASSED_TESTS=$((PASSED_TESTS + 1))
    echo "包: 覆盖率测试" >> "$REPORT_FILE"
    echo "状态: 通过" >> "$REPORT_FILE"
else
    echo "❌ 覆盖率报告生成失败"
    echo "包: 覆盖率测试" >> "$REPORT_FILE"
    echo "状态: 失败" >> "$REPORT_FILE"
fi

echo "输出:" >> "$REPORT_FILE"
cat temp_coverage.txt >> "$REPORT_FILE"
echo "$(printf '%*s' 50 | tr ' ' '-')" >> "$REPORT_FILE"
echo "" >> "$REPORT_FILE"

rm -f temp_coverage.txt

# 4. 测试服务器启动（可选）
echo "4. 测试服务器启动..."
TOTAL_TESTS=$((TOTAL_TESTS + 1))

# 启动服务器并测试
go run cmd/server/main.go > temp_server.txt 2>&1 &
SERVER_PID=$!
sleep 5

# 测试健康检查端点
if curl -s http://localhost:8080/api/health > /dev/null 2>&1; then
    echo "✅ 服务器启动测试通过"
    PASSED_TESTS=$((PASSED_TESTS + 1))
    echo "包: 服务器启动测试" >> "$REPORT_FILE"
    echo "状态: 通过" >> "$REPORT_FILE"
else
    echo "❌ 服务器启动测试失败"
    echo "包: 服务器启动测试" >> "$REPORT_FILE"
    echo "状态: 失败" >> "$REPORT_FILE"
fi

# 停止服务器
kill $SERVER_PID 2>/dev/null || true
wait $SERVER_PID 2>/dev/null || true

echo "输出:" >> "$REPORT_FILE"
cat temp_server.txt >> "$REPORT_FILE"
echo "$(printf '%*s' 50 | tr ' ' '-')" >> "$REPORT_FILE"
echo "" >> "$REPORT_FILE"

rm -f temp_server.txt

# 输出测试总结
echo
echo "=========================================="
echo "测试总结:"
echo "总测试数: $TOTAL_TESTS"
echo "通过: $PASSED_TESTS"
echo "失败: $((TOTAL_TESTS - PASSED_TESTS))"

if [ $TOTAL_TESTS -gt 0 ]; then
    SUCCESS_RATE=$(echo "scale=1; $PASSED_TESTS * 100 / $TOTAL_TESTS" | bc -l 2>/dev/null || echo "0")
    echo "成功率: ${SUCCESS_RATE}%"
else
    echo "成功率: 0%"
fi

echo "结束时间: $(date '+%Y-%m-%d %H:%M:%S')"
echo "测试报告: $REPORT_FILE"

if [ $PASSED_TESTS -eq $TOTAL_TESTS ]; then
    echo "🎉 所有测试通过！"
    exit 0
else
    echo "❌ 部分测试失败"
    exit 1
fi
