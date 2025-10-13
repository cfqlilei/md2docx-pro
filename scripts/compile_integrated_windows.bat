@echo off
REM 整合版编译脚本 - Windows版本
REM 编译Go后端和Qt前端

echo === Markdown转Word工具 - 整合版编译 ===
echo 开始编译整合版应用...

REM 检查环境
echo 检查编译环境...

REM 检查Go
go version >nul 2>&1
if errorlevel 1 (
    echo ❌ Go未安装，请先安装Go 1.25+
    pause
    exit /b 1
)
echo ✅ Go版本: 
go version

REM 检查qmake
qmake -v >nul 2>&1
if errorlevel 1 (
    echo ❌ qmake未找到，请确保Qt 5.15已安装并设置PATH
    echo 请设置Qt路径，例如: set PATH=C:\Qt\5.15.2\msvc2019_64\bin;%PATH%
    pause
    exit /b 1
)
echo ✅ qmake版本:
qmake -v | findstr "QMake"

REM 检查pandoc
pandoc --version >nul 2>&1
if errorlevel 1 (
    echo ⚠️  Pandoc未找到，运行时可能需要手动配置路径
) else (
    echo ✅ Pandoc版本:
    pandoc --version | findstr "pandoc"
)

REM 编译Go后端
echo.
echo 编译Go后端...

REM 创建构建目录
if not exist "build\release" mkdir "build\release"

REM 更新依赖
echo 更新Go依赖...
go mod tidy

REM 获取版本信息
set /p VERSION=<VERSION
if "%VERSION%"=="" set VERSION=dev

REM 获取构建时间
for /f "tokens=1-4 delims=/ " %%a in ('date /t') do set BUILD_DATE=%%a-%%b-%%c
for /f "tokens=1-2 delims=: " %%a in ('time /t') do set BUILD_TIME=%%a:%%b
set BUILD_TIME_FULL=%BUILD_DATE% %BUILD_TIME% UTC

REM 获取Git提交
for /f %%i in ('git rev-parse --short HEAD 2^>nul') do set GIT_COMMIT=%%i
if "%GIT_COMMIT%"=="" set GIT_COMMIT=unknown

REM 构建ldflags
set LDFLAGS=-X "main.Version=%VERSION%" -X "main.BuildTime=%BUILD_TIME_FULL%" -X "main.GitCommit=%GIT_COMMIT%"

REM 编译Windows版本（临时文件，用于内嵌到整合版应用中）
echo 编译Windows后端 (版本: %VERSION%)...
set CGO_ENABLED=1
set GOOS=windows
set GOARCH=amd64
go build -buildvcs=false -ldflags="%LDFLAGS%" -o build\release\md2docx-server-windows-temp.exe .\cmd\server
if errorlevel 1 (
    echo ❌ Windows后端编译失败
    pause
    exit /b 1
)
echo ✅ Windows后端编译完成

REM 编译macOS版本（临时文件，用于内嵌到整合版应用中）
echo 编译macOS后端 (版本: %VERSION%)...
set CGO_ENABLED=1
set GOOS=darwin
set GOARCH=arm64
go build -buildvcs=false -ldflags="%LDFLAGS%" -o build\release\md2docx-server-macos-temp .\cmd\server
if errorlevel 1 (
    echo ❌ macOS后端编译失败
    pause
    exit /b 1
)
echo ✅ macOS后端编译完成

REM 编译Qt前端
echo.
echo 编译Qt整合版前端...

cd qt-frontend

REM 创建构建目录
if not exist "build_simple_integrated" mkdir "build_simple_integrated"
cd build_simple_integrated

REM 生成Makefile
echo 生成Makefile...
qmake ..\md2docx_simple_integrated.pro
if errorlevel 1 (
    echo ❌ qmake失败
    cd ..\..
    pause
    exit /b 1
)

REM 编译
echo 编译Qt应用...
nmake
if errorlevel 1 (
    echo ❌ Qt前端编译失败
    cd ..\..
    pause
    exit /b 1
)
echo ✅ Qt前端编译完成

cd ..\..

REM 验证编译结果
echo.
echo 验证编译结果...

REM 检查临时后端文件
if exist "build\release\md2docx-server-windows-temp.exe" (
    echo ✅ Windows后端临时文件: build\release\md2docx-server-windows-temp.exe
) else (
    echo ❌ Windows后端临时文件不存在
    pause
    exit /b 1
)

if exist "build\release\md2docx-server-macos-temp" (
    echo ✅ macOS后端临时文件: build\release\md2docx-server-macos-temp
) else (
    echo ❌ macOS后端临时文件不存在
    pause
    exit /b 1
)

REM 检查前端可执行文件
if exist "qt-frontend\build_simple_integrated\release\md2docx_simple_integrated.exe" (
    echo ✅ Windows前端编译完成

    REM 移动前端到build/release目录并重命名为带版本号的名称
    echo 移动应用到build/release目录并重命名...
    copy "qt-frontend\build_simple_integrated\release\md2docx_simple_integrated.exe" "build\release\md2docx_simple_integrated-v%VERSION%.exe" >nul
    echo ✅ 应用已移动到 build\release\md2docx_simple_integrated-v%VERSION%.exe

    REM 内嵌后端到应用目录中（Windows版本需要在同一目录）
    echo 内嵌后端到应用目录中...
    copy "build\release\md2docx-server-windows-temp.exe" "build\release\md2docx-server-windows.exe" >nul
    echo ✅ 内嵌后端完成

    REM 删除临时后端文件
    del "build\release\md2docx-server-windows-temp.exe" >nul 2>&1
    del "build\release\md2docx-server-macos-temp" >nul 2>&1
    echo ✅ 清理临时文件完成
) else (
    echo ❌ Windows前端可执行文件不存在
    pause
    exit /b 1
)

echo.
echo ✅ === 编译完成 ===
echo 编译结果:
echo   Windows整合版应用: build\release\md2docx_simple_integrated-v%VERSION%.exe
echo.
echo 下一步: 运行 scripts\build_integrated_windows.bat 进行最终构建（如需要）
pause
