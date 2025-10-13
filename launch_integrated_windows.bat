@echo off
echo === Markdown转Word工具 - 整合版 ===
echo 启动整合版应用...

set APP_PATH=qt-frontend\build_simple_integrated\release\md2docx_simple_integrated.exe

if exist "%APP_PATH%" (
    echo ✅ 找到整合版应用: %APP_PATH%
    echo 🚀 启动应用...
    start "" "%APP_PATH%"
    echo ✅ 整合版应用已启动！
    echo.
    echo 特点：
    echo   ✓ 单一程序，无需分别启动前后端
    echo   ✓ 内嵌后端服务，自动启动
    echo   ✓ 完整的GUI界面
    echo   ✓ 所有功能都已整合
) else (
    echo ❌ 错误: 整合版应用不存在
    echo 请先在Windows环境下构建整合版应用
    pause
)
