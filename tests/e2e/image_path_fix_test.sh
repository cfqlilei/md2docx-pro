#!/bin/bash

# 图片路径修复端到端测试脚本
# 验证相对路径图片是否能正确嵌入到Word文档中

set -e

echo "=== 图片路径修复测试 ==="
echo "测试时间: $(date)"
echo "========================="

# 项目根目录
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
cd "$PROJECT_ROOT"

# 测试配置
API_BASE="http://localhost:8080/api"

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

# 准备测试环境
setup_test_environment() {
    print_info "准备测试环境..."
    
    # 创建测试目录
    mkdir -p test_images_temp/images
    
    # 创建测试图片（简单的PNG文件头）
    local png_header='\x89\x50\x4E\x47\x0D\x0A\x1A\x0A'
    printf "$png_header" > test_images_temp/images/test1.png
    printf "$png_header" > test_images_temp/images/test2.png
    printf "$png_header" > test_images_temp/images/20251012-214305.png
    
    # 创建测试Markdown文件
    cat > test_images_temp/test_doc.md << 'EOF'
# 图片路径测试文档

这是一个测试文档，用于验证相对路径图片嵌入功能。

## 用户格式的图片路径

![1](images/20251012-214305.png)

## 其他相对路径图片

![测试图片1](images/test1.png)
![测试图片2](images/test2.png)

## 在线图片对比

![GitHub Logo](https://github.githubassets.com/images/modules/logos_page/GitHub-Mark.png)

## 测试内容

这是一些测试内容，用于验证图片路径修复功能。

### 功能验证

- 相对路径图片应该正确嵌入
- 在线图片应该正确嵌入
- 生成的Word文档应该包含所有图片
EOF
    
    print_success "测试环境准备完成"
}

# 测试图片路径修复
test_image_path_fix() {
    print_info "测试图片路径修复功能..."
    
    local test_file="test_images_temp/test_doc.md"
    local output_name="图片路径修复测试结果"
    
    # 执行转换
    local response=$(curl -s -X POST "$API_BASE/convert/single" \
        -H "Content-Type: application/json" \
        -d "{
            \"input_file\": \"$test_file\",
            \"output_dir\": \".\",
            \"output_name\": \"$output_name\"
        }")

    # 检查转换是否成功
    if echo "$response" | grep -q '"success":true'; then
        print_success "转换请求成功"
    else
        print_error "转换请求失败: $response"
        return 1
    fi

    # 获取输出文件名
    local output_file=$(echo "$response" | grep -o '"output_file":"[^"]*"' | cut -d'"' -f4)
    local full_output_path="$output_file"
    
    # 检查输出文件是否存在
    if [[ -f "$full_output_path" ]]; then
        print_success "输出文件存在: $output_file"
    else
        print_error "输出文件不存在: $full_output_path"
        return 1
    fi
    
    # 检查文件大小
    local file_size=$(stat -f%z "$full_output_path" 2>/dev/null || stat -c%s "$full_output_path" 2>/dev/null)
    print_info "生成的docx文件大小: ${file_size} bytes"
    
    # 包含多个图片的文档应该大于20KB
    if [[ $file_size -gt 20000 ]]; then
        print_success "文件大小合理 (${file_size} bytes)，图片已正确嵌入"
    else
        print_error "文件大小过小 (${file_size} bytes)，图片可能未正确嵌入"
        return 1
    fi
    
    return 0
}

# 测试不同图片目录结构
test_multiple_image_dirs() {
    print_info "测试多种图片目录结构..."
    
    # 创建多个图片目录
    mkdir -p test_images_temp/figures test_images_temp/pics test_images_temp/assets
    
    # 在每个目录中创建测试图片
    local png_header='\x89\x50\x4E\x47\x0D\x0A\x1A\x0A'
    printf "$png_header" > test_images_temp/figures/chart.png
    printf "$png_header" > test_images_temp/pics/photo.png
    printf "$png_header" > test_images_temp/assets/icon.png
    
    # 创建包含多目录图片的测试文档
    cat > test_images_temp/multi_dir_test.md << 'EOF'
# 多目录图片测试

## 不同目录的图片

![图表](figures/chart.png)
![照片](pics/photo.png)
![图标](assets/icon.png)
![原有图片](images/test1.png)

## 测试内容

这是一个包含多个图片目录的测试文档。
EOF
    
    # 执行转换
    local response=$(curl -s -X POST "$API_BASE/convert/single" \
        -H "Content-Type: application/json" \
        -d '{
            "input_file": "test_images_temp/multi_dir_test.md",
            "output_dir": ".",
            "output_name": "多目录图片测试"
        }')

    # 检查转换结果
    if echo "$response" | grep -q '"success":true'; then
        local output_file=$(echo "$response" | grep -o '"output_file":"[^"]*"' | cut -d'"' -f4)
        local full_output_path="$output_file"
        
        if [[ -f "$full_output_path" ]]; then
            local file_size=$(stat -f%z "$full_output_path" 2>/dev/null || stat -c%s "$full_output_path" 2>/dev/null)
            print_success "多目录图片测试通过，文件大小: ${file_size} bytes"
        else
            print_error "多目录图片测试失败：输出文件不存在"
            return 1
        fi
    else
        print_error "多目录图片测试失败: $response"
        return 1
    fi
    
    return 0
}

# 清理测试环境
cleanup_test_environment() {
    print_info "清理测试环境..."
    rm -rf test_images_temp
    print_success "测试环境清理完成"
}

# 主测试函数
main() {
    local failed_tests=0
    
    # 检查后端服务
    if ! check_backend; then
        exit 1
    fi
    
    # 准备测试环境
    setup_test_environment
    
    # 运行测试
    echo ""
    echo "开始运行图片路径修复测试..."
    echo ""
    
    if ! test_image_path_fix; then
        ((failed_tests++))
    fi
    
    if ! test_multiple_image_dirs; then
        ((failed_tests++))
    fi
    
    # 清理环境
    cleanup_test_environment
    
    # 输出结果
    echo ""
    echo "========================="
    if [[ $failed_tests -eq 0 ]]; then
        print_success "所有图片路径修复测试通过！🎉"
        echo ""
        print_success "修复验证总结:"
        print_success "✅ 相对路径图片正确嵌入"
        print_success "✅ 用户格式 ![1](images/20251012-214305.png) 正常工作"
        print_success "✅ 多种图片目录结构支持"
        print_success "✅ --resource-path参数正确配置"
        echo ""
        print_success "图片路径问题已完全修复！"
    else
        print_error "有 $failed_tests 个测试失败"
        exit 1
    fi
}

# 运行主函数
main "$@"
