@echo off
REM 整合版运行脚本 - Windows版本
REM 启动整合版应用

echo === Markdown转Word工具 - 整合版运行 ===
echo 启动整合版应用...

REM 读取版本号
set VERSION=1.0.0
if exist "VERSION" (
    set /p VERSION=<VERSION
)

REM 检查应用是否存在
set APP_PATH=build\release\md2docx_simple_integrated-v%VERSION%.exe

if not exist "%APP_PATH%" (
    echo ❌ 整合版应用不存在: %APP_PATH%
    echo 请先运行构建流程:
    echo   1. scripts\clean_integrated_windows.bat    # 清理
    echo   2. scripts\compile_integrated_windows.bat  # 编译
    echo   3. scripts\build_integrated_windows.bat    # 构建
    pause
    exit /b 1
)

echo ✅ 找到整合版应用: %APP_PATH%

REM 检查应用完整性
echo 检查应用完整性...

REM 检查前端可执行文件
if not exist "%APP_PATH%" (
    echo ❌ 前端可执行文件不存在
    pause
    exit /b 1
)
echo ✅ 前端可执行文件存在

REM 检查后端
set BACKEND_PATH=build\release\md2docx-server-windows.exe
if not exist "%BACKEND_PATH%" (
    echo ❌ 后端服务器不存在
    pause
    exit /b 1
)
echo ✅ 后端服务器存在

echo ✅ 应用完整性检查通过

REM 显示应用信息
echo.
echo 应用信息:
for %%f in ("%APP_PATH%") do echo   📦 前端大小: %%~zf 字节
for %%f in ("%BACKEND_PATH%") do echo   🔧 后端大小: %%~zf 字节
echo   📍 应用路径: %APP_PATH%

REM 启动应用
echo.
echo 🚀 启动整合版应用...

start "" "%APP_PATH%"

if errorlevel 1 (
    echo ❌ 应用启动失败
    pause
    exit /b 1
) else (
    echo ✅ 整合版应用已启动！
    echo.
    echo 应用特点:
    echo   ✓ 单一程序，无需分别启动前后端
    echo   ✓ 内嵌Go后端服务，自动启动
    echo   ✓ 动态端口分配，避免冲突
    echo   ✓ 完整的GUI界面
    echo   ✓ 所有功能都已整合
    echo.
    echo 使用说明:
    echo   • 应用启动后会自动启动内嵌的后端服务器
    echo   • 后端会自动分配可用端口，避免冲突
    echo   • 如果看到服务器启动中的提示，请等待几秒钟
    echo   • 状态栏会显示服务器运行状态
    echo   • 关闭应用时会自动停止后端服务器
    echo.
    echo 如果遇到问题:
    echo   • 检查Pandoc是否已安装: pandoc --version
    echo   • 查看应用日志或重启应用
    echo   • 确保端口8080-8090范围内有可用端口
    echo.
    echo ✅ === 启动完成 ===
)

pause
