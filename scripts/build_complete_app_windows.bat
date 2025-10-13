@echo off
REM 完整应用构建脚本 - Windows版本
REM 构建前后端完整版本

setlocal enabledelayedexpansion

REM 颜色定义（Windows不支持ANSI颜色，使用echo代替）
set "PROJECT_ROOT=%~dp0.."
cd /d "%PROJECT_ROOT%"

echo [%date% %time%] 开始构建完整应用
echo 项目目录: %PROJECT_ROOT%

REM 检查环境
echo [%date% %time%] 检查构建环境...

REM 检查Go
go version >nul 2>&1
if %errorlevel% neq 0 (
    echo 错误: Go未安装，请先安装Go 1.25+
    exit /b 1
) else (
    echo 成功: Go已安装
    go version
)

REM 检查qmake
qmake -v >nul 2>&1
if %errorlevel% neq 0 (
    echo 错误: qmake未找到，请确保Qt 5.15已安装并设置PATH
    echo 请设置: set PATH=C:\Qt\5.15.2\msvc2019_64\bin;%%PATH%%
    exit /b 1
) else (
    echo 成功: qmake已安装
    qmake -v
)

REM 检查pandoc
pandoc --version >nul 2>&1
if %errorlevel% neq 0 (
    echo 警告: Pandoc未找到，运行时可能需要手动配置路径
) else (
    echo 成功: Pandoc已安装
    pandoc --version | findstr /C:"pandoc"
)

REM 构建Go后端
echo [%date% %time%] 构建Go后端...

REM 清理并创建构建目录
if exist build rmdir /s /q build
mkdir build

REM 更新依赖
go mod tidy

REM 构建Windows版本
echo [%date% %time%] 构建Windows后端...
set CGO_ENABLED=1
set GOOS=windows
set GOARCH=amd64
go build -buildvcs=false -o build\md2docx-server-windows.exe .\cmd\server
if %errorlevel% neq 0 (
    echo 错误: Windows后端构建失败
    exit /b 1
)
echo 成功: Windows后端构建完成

REM 构建macOS版本（交叉编译）
echo [%date% %time%] 构建macOS后端...
set CGO_ENABLED=1
set GOOS=darwin
set GOARCH=arm64
go build -buildvcs=false -o build\md2docx-server-macos .\cmd\server
if %errorlevel% neq 0 (
    echo 警告: macOS后端构建失败（可能需要在macOS上构建）
) else (
    echo 成功: macOS后端构建完成
)

REM 验证构建结果
dir build\

REM 构建Qt前端
echo [%date% %time%] 构建Qt前端...

cd qt-frontend

REM 清理之前的构建
if exist build_md2docx_app rmdir /s /q build_md2docx_app
mkdir build_md2docx_app
cd build_md2docx_app

REM 构建Windows版本
echo [%date% %time%] 构建Windows前端...
qmake ..\md2docx_app.pro
if %errorlevel% neq 0 (
    echo 错误: qmake配置失败
    cd ..\..
    exit /b 1
)

nmake
if %errorlevel% neq 0 (
    echo 错误: 前端编译失败
    cd ..\..
    exit /b 1
)

echo 成功: Windows前端构建完成

REM 验证构建结果
if exist build\md2docx_app.exe (
    echo 成功: Windows应用程序创建成功: build\md2docx_app.exe
) else (
    echo 错误: Windows应用程序创建失败
    cd ..\..
    exit /b 1
)

cd ..\..

REM 测试构建结果
echo [%date% %time%] 测试构建结果...

REM 测试后端
if exist build\md2docx-server-windows.exe (
    echo 成功: Windows后端文件存在
    REM 简单测试启动（后台运行5秒后关闭）
    start /b build\md2docx-server-windows.exe
    timeout /t 3 /nobreak >nul
    taskkill /f /im md2docx-server-windows.exe >nul 2>&1
    echo 成功: Windows后端启动测试完成
) else (
    echo 错误: Windows后端文件不存在
)

REM 测试前端
if exist qt-frontend\build_md2docx_app\build\md2docx_app.exe (
    echo 成功: Windows前端应用程序存在
) else (
    echo 错误: Windows前端应用程序不存在
)

REM 创建启动脚本
echo [%date% %time%] 创建启动脚本...

REM 创建简单的启动脚本
echo @echo off > launch_windows.bat
echo echo === Markdown转Word工具 - Windows版本 === >> launch_windows.bat
echo echo 启动后端服务... >> launch_windows.bat
echo start /b build\md2docx-server-windows.exe >> launch_windows.bat
echo echo 等待后端服务启动... >> launch_windows.bat
echo timeout /t 3 /nobreak ^>nul >> launch_windows.bat
echo echo 启动前端应用... >> launch_windows.bat
echo start qt-frontend\build_md2docx_app\build\md2docx_app.exe >> launch_windows.bat
echo echo 应用启动完成！ >> launch_windows.bat
echo echo 按任意键退出... >> launch_windows.bat
echo pause ^>nul >> launch_windows.bat
echo echo 关闭应用... >> launch_windows.bat
echo taskkill /f /im md2docx_app.exe ^>nul 2^>^&1 >> launch_windows.bat
echo taskkill /f /im md2docx-server-windows.exe ^>nul 2^>^&1 >> launch_windows.bat

echo 成功: 创建Windows启动脚本: launch_windows.bat

REM 主函数完成
echo 成功: === 构建完成 ===
echo [%date% %time%] 构建文件:
echo   后端 (Windows): build\md2docx-server-windows.exe
echo   后端 (macOS): build\md2docx-server-macos
echo   前端 (Windows): qt-frontend\build_md2docx_app\build\md2docx_app.exe
echo   启动脚本: launch_windows.bat
echo.
echo [%date% %time%] 启动方式:
echo   Node.js脚本: node scripts\launch_complete_app_windows.js
echo   简单脚本: launch_windows.bat
echo.
echo macOS版本需要在macOS环境下构建前端部分

pause
