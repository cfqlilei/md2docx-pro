@echo off
REM 整合版一次性执行脚本 - Windows版本
REM 清理 -> 编译 -> 构建 -> 运行

echo.
echo ╔══════════════════════════════════════════════════════════════╗
echo ║                Markdown转Word工具 - 整合版                    ║
echo ║                    一次性构建和运行脚本                        ║
echo ╚══════════════════════════════════════════════════════════════╝
echo.
echo 项目目录: %CD%
echo 开始完整的构建流程...
echo.

REM 执行步骤函数
goto :main

:execute_step
set step_name=%1
set script_path=%2
set description=%3

echo 🔄 %step_name%: %description%

if not exist "%script_path%" (
    echo ❌ 脚本不存在: %script_path%
    pause
    exit /b 1
)

call "%script_path%"
if errorlevel 1 (
    echo ❌ %step_name% 失败
    echo 请检查错误信息并修复问题后重试
    pause
    exit /b 1
)

echo ✅ %step_name% 完成
echo.
goto :eof

:main
REM 步骤1: 清理
call :execute_step "步骤1" "scripts\clean_integrated_windows.bat" "清理所有构建文件和临时文件"

REM 步骤2: 编译
call :execute_step "步骤2" "scripts\compile_integrated_windows.bat" "编译Go后端和Qt前端"

REM 步骤3: 构建
call :execute_step "步骤3" "scripts\build_integrated_windows.bat" "构建完整的应用包"

REM 步骤4: 运行
call :execute_step "步骤4" "scripts\run_integrated_windows.bat" "启动整合版应用"

REM 显示完成信息
echo.
echo ╔══════════════════════════════════════════════════════════════╗
echo ║                        🎉 构建完成！                          ║
echo ╚══════════════════════════════════════════════════════════════╝
echo.

echo ✅ 整合版应用已成功构建并启动！
echo.
echo 构建结果:

set APP_PATH=qt-frontend\build_simple_integrated\release\md2docx_simple_integrated.exe
if exist "%APP_PATH%" (
    echo   📦 应用程序: %APP_PATH%
    for %%f in ("%APP_PATH%") do echo   📏 大小: %%~zf 字节
    echo   🚀 状态: 已启动
)

echo.
echo 下次启动方式:
echo   快速启动: launch_integrated_windows.bat
echo   完整重建: scripts\all_in_one_integrated_windows.bat
echo.
echo 应用特点:
echo   ✓ 单一程序，无需分别启动前后端
echo   ✓ 内嵌Go后端服务，自动启动
echo   ✓ 完整的GUI界面，所有功能已整合
echo   ✓ 用户友好，双击即可使用
echo.
echo ✅ === 全部完成 ===
pause
