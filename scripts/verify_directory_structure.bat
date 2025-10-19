@echo off
REM 验证目录结构规范化脚本 - Windows版本
REM 检查所有文件是否按照新的规范存放

setlocal enabledelayedexpansion

echo === 验证目录结构规范化 ===
echo.

REM 检查计数
set CHECKS_PASSED=0
set CHECKS_FAILED=0

REM 检查目录函数
:check_directory
set dir=%1
set description=%2

if exist "%dir%" (
    echo ✅ 目录存在: %dir% (%description%)
    set /a CHECKS_PASSED+=1
) else (
    echo ❌ 目录不存在: %dir% (%description%)
    set /a CHECKS_FAILED+=1
)
goto :eof

REM 检查文件函数
:check_file
set file=%1
set description=%2

if exist "%file%" (
    echo ✅ 文件存在: %file% (%description%)
    set /a CHECKS_PASSED+=1
) else (
    echo ⚠️  文件不存在: %file% (%description%)
    set /a CHECKS_FAILED+=1
)
goto :eof

REM 验证源代码目录
echo === 验证源代码目录 ===
call :check_directory "cmd" "Go 命令行程序"
call :check_directory "internal" "Go 内部包"
call :check_directory "pkg" "Go 公共包"
call :check_directory "qt-frontend\src" "Qt 前端源代码"
call :check_directory "web" "Web 资源"
call :check_file "go.mod" "Go 模块文件"
call :check_file "go.sum" "Go 模块锁定文件"
echo.

REM 验证编译产物目录
echo === 验证编译产物目录 ===
call :check_directory "build" "编译产物根目录"
call :check_directory "build\bin" "最终的二进制文件目录"
call :check_directory "build\intermediate" "中间文件目录"
call :check_directory "build\intermediate\go" "Go 编译中间文件"
call :check_directory "build\intermediate\qt" "Qt 编译中间文件"
echo.

REM 验证测试目录
echo === 验证测试目录 ===
call :check_directory "tests" "测试根目录"
call :check_directory "tests\unit" "单元测试"
call :check_directory "tests\integration" "集成测试"
call :check_directory "tests\e2e" "端到端测试"
call :check_directory "tests\testdata" "测试数据"
call :check_directory "tests\results" "测试结果"
call :check_directory "tests\utils" "测试工具"
echo.

REM 验证脚本目录
echo === 验证脚本目录 ===
call :check_directory "scripts" "构建脚本目录"
call :check_file "scripts\compile_integrated.sh" "Mac 编译脚本"
call :check_file "scripts\compile_integrated_windows.bat" "Windows 编译脚本"
call :check_file "scripts\clean_integrated.sh" "Mac 清理脚本"
call :check_file "scripts\clean_integrated_windows.bat" "Windows 清理脚本"
echo.

REM 验证文档目录
echo === 验证文档目录 ===
call :check_directory "docs" "文档目录"
echo.

REM 验证 Qt 项目文件
echo === 验证 Qt 项目文件 ===
call :check_file "qt-frontend\md2docx_simple_integrated.pro" "Qt 简单整合版项目文件"
call :check_file "qt-frontend\md2docx_integrated.pro" "Qt 整合版项目文件"
call :check_file "qt-frontend\md2docx_app.pro" "Qt 应用项目文件"
echo.

REM 验证版本文件
echo === 验证版本文件 ===
call :check_file "VERSION" "版本号文件"
echo.

REM 显示验证结果
echo === 验证结果 ===
echo 通过检查: %CHECKS_PASSED%
echo 失败检查: %CHECKS_FAILED%
echo.

if %CHECKS_FAILED% equ 0 (
    echo ✅ === 所有检查都通过了！目录结构规范化成功 ===
    pause
    exit /b 0
) else (
    echo ⚠️  === 有 %CHECKS_FAILED% 个检查失败，请检查上述错误 ===
    pause
    exit /b 1
)

