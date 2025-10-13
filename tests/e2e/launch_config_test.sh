#!/bin/bash

# 启动配置测试脚本
# 测试VSCode launch.json和tasks.json配置是否正确

set -e

echo "=== 启动配置测试 ==="
echo "测试时间: $(date)"
echo "========================"

# 项目根目录
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
cd "$PROJECT_ROOT"

# 测试函数
test_build_task() {
    local task_name="$1"
    local expected_dir="$2"
    local expected_binary="$3"
    
    echo "🧪 测试构建任务: $task_name"
    
    # 清理旧构建
    if [ -d "qt-frontend/$expected_dir" ]; then
        rm -rf "qt-frontend/$expected_dir"
    fi
    
    # 执行构建任务
    case "$task_name" in
        "build-qt-complete-macos")
            export PATH="/opt/homebrew/opt/qt@5/bin:$PATH"
            cd qt-frontend
            mkdir "$expected_dir"
            cd "$expected_dir"
            qmake ../md2docx_app.pro
            make
            cd ../..
            ;;
        "build-qt-complete-debug-macos")
            export PATH="/opt/homebrew/opt/qt@5/bin:$PATH"
            cd qt-frontend
            mkdir "$expected_dir"
            cd "$expected_dir"
            qmake CONFIG+=debug ../md2docx_app.pro
            make
            cd ../..
            ;;
    esac
    
    # 检查构建结果
    if [ -f "qt-frontend/$expected_dir/build/$expected_binary" ]; then
        echo "✅ 构建任务 $task_name 成功"
        echo "   生成文件: qt-frontend/$expected_dir/build/$expected_binary"
        
        # 检查文件大小
        local file_size=$(stat -f%z "qt-frontend/$expected_dir/build/$expected_binary" 2>/dev/null || echo "0")
        echo "   文件大小: ${file_size} bytes"
        
        if [ "$file_size" -gt 1000000 ]; then
            echo "   ✅ 文件大小正常 (>1MB)"
        else
            echo "   ⚠️  文件大小较小，可能构建不完整"
        fi
    else
        echo "❌ 构建任务 $task_name 失败"
        echo "   预期文件: qt-frontend/$expected_dir/build/$expected_binary"
        return 1
    fi
}

test_launch_script() {
    local script_name="$1"
    local expected_frontend_binary="$2"
    
    echo "🧪 测试启动脚本: $script_name"
    
    # 检查脚本文件存在
    if [ ! -f "scripts/$script_name" ]; then
        echo "❌ 启动脚本不存在: scripts/$script_name"
        return 1
    fi
    
    # 检查脚本中的前端二进制路径
    if grep -q "$expected_frontend_binary" "scripts/$script_name"; then
        echo "✅ 启动脚本配置正确"
        echo "   前端路径: $expected_frontend_binary"
    else
        echo "❌ 启动脚本配置错误"
        echo "   未找到预期路径: $expected_frontend_binary"
        return 1
    fi
    
    # 检查前端二进制文件是否存在
    local frontend_path=$(grep -o "qt-frontend/[^\"]*" "scripts/$script_name" | head -1)
    if [ -f "$frontend_path" ]; then
        echo "✅ 前端二进制文件存在: $frontend_path"
    else
        echo "⚠️  前端二进制文件不存在: $frontend_path"
        echo "   需要先构建前端应用"
    fi
}

test_vscode_config() {
    echo "🧪 测试VSCode配置文件"
    
    # 检查launch.json
    if [ ! -f ".vscode/launch.json" ]; then
        echo "❌ launch.json 不存在"
        return 1
    fi
    
    # 检查tasks.json
    if [ ! -f ".vscode/tasks.json" ]; then
        echo "❌ tasks.json 不存在"
        return 1
    fi
    
    # 检查关键配置
    local errors=0
    
    # 检查macOS前端启动配置
    if grep -q "build_md2docx_app/build/md2docx_app.app" ".vscode/launch.json"; then
        echo "✅ macOS前端启动配置正确"
    else
        echo "❌ macOS前端启动配置错误"
        errors=$((errors + 1))
    fi
    
    # 检查Windows前端启动配置
    if grep -q "build_md2docx_app/build/md2docx_app.exe" ".vscode/launch.json"; then
        echo "✅ Windows前端启动配置正确"
    else
        echo "❌ Windows前端启动配置错误"
        errors=$((errors + 1))
    fi
    
    # 检查构建任务配置
    if grep -q "build_md2docx_app" ".vscode/tasks.json"; then
        echo "✅ 构建任务配置正确"
    else
        echo "❌ 构建任务配置错误"
        errors=$((errors + 1))
    fi
    
    if [ $errors -eq 0 ]; then
        echo "✅ VSCode配置文件检查通过"
        return 0
    else
        echo "❌ VSCode配置文件检查失败，发现 $errors 个错误"
        return 1
    fi
}

# 主测试流程
main() {
    echo "开始测试启动配置..."
    
    local test_passed=0
    local test_total=0
    
    # 测试1: VSCode配置文件
    echo ""
    test_total=$((test_total + 1))
    if test_vscode_config; then
        test_passed=$((test_passed + 1))
    fi
    
    # 测试2: macOS构建任务
    echo ""
    test_total=$((test_total + 1))
    if test_build_task "build-qt-complete-macos" "build_md2docx_app" "md2docx_app.app/Contents/MacOS/md2docx_app"; then
        test_passed=$((test_passed + 1))
    fi
    
    # 测试3: macOS启动脚本
    echo ""
    test_total=$((test_total + 1))
    if test_launch_script "launch_complete_app_macos.js" "md2docx_app.app"; then
        test_passed=$((test_passed + 1))
    fi
    
    # 测试4: Windows启动脚本
    echo ""
    test_total=$((test_total + 1))
    if test_launch_script "launch_complete_app_windows.js" "md2docx_app.exe"; then
        test_passed=$((test_passed + 1))
    fi
    
    # 测试结果汇总
    echo ""
    echo "========================"
    echo "测试结果汇总:"
    echo "通过: $test_passed/$test_total"
    
    if [ $test_passed -eq $test_total ]; then
        echo "🎉 所有启动配置测试通过！"
        echo ""
        echo "现在可以使用以下方式启动应用："
        echo "1. VSCode: F5 -> '启动完整应用 (macOS)'"
        echo "2. 命令行: node scripts/launch_complete_app_macos.js"
        echo "3. VSCode: F5 -> '启动前后端 (macOS)'"
        return 0
    else
        echo "❌ 部分测试失败，请检查配置"
        return 1
    fi
}

# 执行主函数
main "$@"
