#!/bin/bash

# 端到端最终验证测试脚本
# 验证所有修复的功能是否正常工作

set -e

echo "=== Markdown转Word工具 - 最终验证测试 ==="
echo "测试时间: $(date)"
echo "============================================"

# 项目根目录
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
cd "$PROJECT_ROOT"

# 测试配置
API_BASE="http://localhost:8080/api"
TEST_MD_FILE="test_final_verification.md"

# 颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# 打印函数
print_success() {
    echo -e "${GREEN}✅ $1${NC}"
}

print_error() {
    echo -e "${RED}❌ $1${NC}"
}

print_info() {
    echo -e "${YELLOW}ℹ️  $1${NC}"
}

# 检查后端服务是否运行
check_backend() {
    print_info "检查后端服务状态..."
    
    if curl -s "$API_BASE/health" > /dev/null; then
        print_success "后端服务运行正常"
        return 0
    else
        print_error "后端服务未运行，请先启动服务"
        return 1
    fi
}

# 测试1：双重后缀修复
test_double_suffix_fix() {
    print_info "测试1: 双重后缀修复"
    
    # 测试用例1：用户指定文件名不含扩展名
    local output_name="测试文件1"
    local response=$(curl -s -X POST "$API_BASE/convert/single" \
        -H "Content-Type: application/json" \
        -d "{
            \"input_file\": \"$TEST_MD_FILE\",
            \"output_dir\": \".\",
            \"output_name\": \"$output_name\"
        }")
    
    local output_file=$(echo "$response" | grep -o '"output_file":"[^"]*"' | cut -d'"' -f4)
    
    if [[ "$output_file" == "${output_name}.docx" ]]; then
        print_success "测试1.1: 文件名不含扩展名 - 通过 ($output_file)"
    else
        print_error "测试1.1: 文件名不含扩展名 - 失败 (期望: ${output_name}.docx, 实际: $output_file)"
        return 1
    fi
    
    # 测试用例2：用户指定文件名含扩展名
    local output_name_with_ext="测试文件2.docx"
    local response2=$(curl -s -X POST "$API_BASE/convert/single" \
        -H "Content-Type: application/json" \
        -d "{
            \"input_file\": \"$TEST_MD_FILE\",
            \"output_dir\": \".\",
            \"output_name\": \"$output_name_with_ext\"
        }")
    
    local output_file2=$(echo "$response2" | grep -o '"output_file":"[^"]*"' | cut -d'"' -f4)
    
    if [[ "$output_file2" == "$output_name_with_ext" ]]; then
        print_success "测试1.2: 文件名含扩展名 - 通过 ($output_file2)"
    else
        print_error "测试1.2: 文件名含扩展名 - 失败 (期望: $output_name_with_ext, 实际: $output_file2)"
        return 1
    fi
    
    # 测试用例3：中文文件名
    local chinese_name="6-表单管理模块详细设计方案"
    local response3=$(curl -s -X POST "$API_BASE/convert/single" \
        -H "Content-Type: application/json" \
        -d "{
            \"input_file\": \"$TEST_MD_FILE\",
            \"output_dir\": \".\",
            \"output_name\": \"$chinese_name\"
        }")
    
    local output_file3=$(echo "$response3" | grep -o '"output_file":"[^"]*"' | cut -d'"' -f4)
    
    if [[ "$output_file3" == "${chinese_name}.docx" ]]; then
        print_success "测试1.3: 中文文件名 - 通过 ($output_file3)"
    else
        print_error "测试1.3: 中文文件名 - 失败 (期望: ${chinese_name}.docx, 实际: $output_file3)"
        return 1
    fi
    
    print_success "测试1: 双重后缀修复 - 全部通过"
    return 0
}

# 测试2：图片嵌入功能
test_image_embedding() {
    print_info "测试2: 图片嵌入功能"
    
    local output_name="图片嵌入测试"
    local response=$(curl -s -X POST "$API_BASE/convert/single" \
        -H "Content-Type: application/json" \
        -d "{
            \"input_file\": \"$TEST_MD_FILE\",
            \"output_dir\": \".\",
            \"output_name\": \"$output_name\"
        }")
    
    local output_file=$(echo "$response" | grep -o '"output_file":"[^"]*"' | cut -d'"' -f4)
    
    if [[ -f "$output_file" ]]; then
        local file_size=$(stat -f%z "$output_file" 2>/dev/null || stat -c%s "$output_file" 2>/dev/null)
        
        # 检查文件大小，含图片的文档应该大于20KB
        if [[ $file_size -gt 20000 ]]; then
            print_success "测试2: 图片嵌入功能 - 通过 (文件大小: ${file_size} bytes)"
        else
            print_error "测试2: 图片嵌入功能 - 失败 (文件大小过小: ${file_size} bytes，可能图片未嵌入)"
            return 1
        fi
    else
        print_error "测试2: 图片嵌入功能 - 失败 (输出文件不存在: $output_file)"
        return 1
    fi
    
    return 0
}

# 测试3：API健康检查
test_api_health() {
    print_info "测试3: API健康检查"
    
    local health_response=$(curl -s "$API_BASE/health")
    
    if echo "$health_response" | grep -q '"status":"ok"'; then
        print_success "测试3: API健康检查 - 通过"
    else
        print_error "测试3: API健康检查 - 失败"
        return 1
    fi
    
    return 0
}

# 测试4：配置获取
test_config_api() {
    print_info "测试4: 配置API"
    
    local config_response=$(curl -s "$API_BASE/config")
    
    if echo "$config_response" | grep -q '"pandoc_path"'; then
        print_success "测试4: 配置API - 通过"
    else
        print_error "测试4: 配置API - 失败"
        return 1
    fi
    
    return 0
}

# 清理测试文件
cleanup_test_files() {
    print_info "清理测试文件..."
    
    rm -f "测试文件1.docx"
    rm -f "测试文件2.docx"
    rm -f "6-表单管理模块详细设计方案.docx"
    rm -f "图片嵌入测试.docx"
    
    print_success "测试文件清理完成"
}

# 主测试函数
main() {
    local failed_tests=0
    
    # 检查后端服务
    if ! check_backend; then
        exit 1
    fi
    
    # 运行所有测试
    echo ""
    echo "开始运行测试..."
    echo ""
    
    if ! test_api_health; then
        ((failed_tests++))
    fi
    
    if ! test_config_api; then
        ((failed_tests++))
    fi
    
    if ! test_double_suffix_fix; then
        ((failed_tests++))
    fi
    
    if ! test_image_embedding; then
        ((failed_tests++))
    fi
    
    # 清理
    cleanup_test_files
    
    # 输出结果
    echo ""
    echo "============================================"
    if [[ $failed_tests -eq 0 ]]; then
        print_success "所有测试通过！🎉"
        echo ""
        print_success "修复验证总结:"
        print_success "✅ 双重后缀问题已修复"
        print_success "✅ 图片嵌入功能正常"
        print_success "✅ API服务运行正常"
        print_success "✅ 配置功能正常"
        echo ""
        print_success "Markdown转Word工具 v1.0.0 所有功能验证通过！"
    else
        print_error "有 $failed_tests 个测试失败"
        exit 1
    fi
}

# 运行主函数
main "$@"
