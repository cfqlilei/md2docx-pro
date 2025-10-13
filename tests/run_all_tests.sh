#!/bin/bash

# Markdown转Word工具 - 测试运行脚本
# 运行所有测试用例

set -e

echo "=== Markdown转Word工具测试套件 ==="
echo "开始时间: $(date)"
echo "==============================="

# 项目根目录
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$PROJECT_ROOT"

# 创建测试结果目录
mkdir -p tests/results

# 运行单元测试
echo ""
echo "🧪 运行单元测试..."
echo "-------------------"
if [ -d "tests/unit" ] && [ "$(ls -A tests/unit/*.go 2>/dev/null)" ]; then
    go test ./tests/unit -v -timeout=30s | tee tests/results/unit_test_$(date +%Y%m%d_%H%M%S).log
else
    echo "⚠️  没有找到单元测试文件"
fi

# 运行集成测试
echo ""
echo "🔗 运行集成测试..."
echo "-------------------"
if [ -d "tests/integration" ] && [ "$(ls -A tests/integration/*.go 2>/dev/null)" ]; then
    go test ./tests/integration -v -timeout=60s | tee tests/results/integration_test_$(date +%Y%m%d_%H%M%S).log
else
    echo "⚠️  没有找到集成测试文件"
fi

# 运行端到端测试
echo ""
echo "🎯 运行端到端测试..."
echo "-------------------"
if [ -d "tests/e2e" ] && [ "$(ls -A tests/e2e/*.go 2>/dev/null)" ]; then
    go test ./tests/e2e -v -timeout=120s | tee tests/results/e2e_test_$(date +%Y%m%d_%H%M%S).log
else
    echo "⚠️  没有找到Go端到端测试文件"
fi

# 运行端到端验证脚本
if [[ -f "tests/e2e/final_verification_test.sh" ]]; then
    echo ""
    echo "🔍 运行最终验证测试..."
    echo "-------------------"
    bash tests/e2e/final_verification_test.sh | tee tests/results/final_verification_$(date +%Y%m%d_%H%M%S).log
else
    echo "⚠️  没有找到最终验证测试脚本"
fi

# 运行包测试
echo ""
echo "📦 运行包测试..."
echo "-------------------"
go test ./pkg/... -v -timeout=30s | tee tests/results/pkg_test_$(date +%Y%m%d_%H%M%S).log

echo ""
echo "📊 运行内部包测试..."
echo "-------------------"
go test ./internal/... -v -timeout=30s | tee tests/results/internal_test_$(date +%Y%m%d_%H%M%S).log

echo ""
echo "==============================="
echo "✅ 测试完成时间: $(date)"
echo "📁 测试结果保存在: tests/results/"
echo "==============================="
