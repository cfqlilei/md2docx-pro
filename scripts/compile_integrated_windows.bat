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
if not exist "build" mkdir "build"

REM 更新依赖
echo 更新Go依赖...
go mod tidy

REM 编译Windows版本
echo 编译Windows后端...
set CGO_ENABLED=1
set GOOS=windows
set GOARCH=amd64
go build -buildvcs=false -o build\md2docx-server-windows.exe .\cmd\server
if errorlevel 1 (
    echo ❌ Windows后端编译失败
    pause
    exit /b 1
)
echo ✅ Windows后端编译完成

REM 编译macOS版本（交叉编译）
echo 编译macOS后端...
set CGO_ENABLED=1
set GOOS=darwin
set GOARCH=arm64
go build -buildvcs=false -o build\md2docx-server-macos .\cmd\server
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

REM 检查后端文件
if exist "build\md2docx-server-windows.exe" (
    echo ✅ Windows后端: build\md2docx-server-windows.exe
) else (
    echo ❌ Windows后端文件不存在
    pause
    exit /b 1
)

if exist "build\md2docx-server-macos" (
    echo ✅ macOS后端: build\md2docx-server-macos
) else (
    echo ❌ macOS后端文件不存在
    pause
    exit /b 1
)

REM 检查前端可执行文件
if exist "qt-frontend\build_simple_integrated\release\md2docx_simple_integrated.exe" (
    echo ✅ Windows前端: qt-frontend\build_simple_integrated\release\md2docx_simple_integrated.exe
    
    REM 复制后端到前端目录
    copy "build\md2docx-server-windows.exe" "qt-frontend\build_simple_integrated\release\" >nul
    echo ✅ 后端已复制到前端目录
) else (
    echo ❌ Windows前端可执行文件不存在
    pause
    exit /b 1
)

echo.
echo ✅ === 编译完成 ===
echo 编译结果:
echo   Windows后端: build\md2docx-server-windows.exe
echo   macOS后端: build\md2docx-server-macos
echo   Windows整合版应用: qt-frontend\build_simple_integrated\release\md2docx_simple_integrated.exe
echo.
echo 下一步: 运行 scripts\run_integrated_windows.bat 启动应用
pause
