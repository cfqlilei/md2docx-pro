@echo off
echo === Markdown转Word工具 - 整合版 ===
echo 启动整合版应用...

set APP_PATH=build\release\md2docx_simple_integrated-v1.0.0.exe

if exist "%APP_PATH%" (
    echo ✅ 找到整合版应用: %APP_PATH%
    echo 🚀 启动应用...
    start "" "%APP_PATH%"
    echo ✅ 整合版应用已启动！
    echo.
    echo 特点：
    echo   ✓ 单一程序，包含完整的后端服务器
    echo   ✓ 可通过Web界面访问 (http://localhost:8080)
    echo   ✓ 支持命令行参数
    echo   ✓ 自动端口分配，避免冲突
) else (
    echo ❌ 错误: 整合版应用不存在
    echo 请先运行构建脚本:
    echo   scripts\all_in_one_integrated.sh
    pause
)
