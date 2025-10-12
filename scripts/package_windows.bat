@echo off
setlocal enabledelayedexpansion

REM Windows打包脚本
REM 创建完整的Windows应用程序包

echo === Markdown转Word工具 - Windows打包 ===
echo 打包时间: %date% %time%
echo.

REM 检查环境
echo 1. 检查环境依赖...

REM 检查Go
where go >nul 2>&1
if %errorlevel% neq 0 (
    echo ❌ Go未安装，请先安装Go
    exit /b 1
)
for /f "tokens=*" %%i in ('go version') do echo ✅ Go版本: %%i

REM 检查Qt
where qmake >nul 2>&1
if %errorlevel% neq 0 (
    echo ❌ Qt未安装，请先安装Qt
    echo    Windows: 下载Qt 5.15.2 MSVC2019版本
    exit /b 1
)
for /f "tokens=*" %%i in ('qmake -v ^| findstr "QMake"') do echo ✅ QMake版本: %%i

REM 检查Pandoc
where pandoc >nul 2>&1
if %errorlevel% neq 0 (
    echo ⚠️  Pandoc未安装，用户需要自行安装
    echo    Windows: 从 https://pandoc.org/installing.html 下载安装
) else (
    for /f "tokens=*" %%i in ('pandoc --version ^| findstr "pandoc"') do echo ✅ Pandoc版本: %%i
)

echo.

REM 设置变量
set "SCRIPT_DIR=%~dp0"
set "PROJECT_DIR=%SCRIPT_DIR%.."
set "BUILD_DIR=%PROJECT_DIR%\dist\windows"
set "APP_NAME=Markdown转Word工具"
set "VERSION=1.0.0"

echo 2. 准备打包目录...
cd /d "%PROJECT_DIR%"

REM 清理并创建打包目录
if exist "%BUILD_DIR%" rmdir /s /q "%BUILD_DIR%"
mkdir "%BUILD_DIR%"
echo ✅ 打包目录: %BUILD_DIR%

echo.

REM 构建Go后端
echo 3. 构建Go后端...
echo    初始化Go模块...
if not exist "go.mod" (
    go mod init md2docx
)
go mod tidy

echo    编译Go后端...
if not exist "build" mkdir build
set CGO_ENABLED=1
set GOOS=windows
set GOARCH=amd64
go build -ldflags="-s -w" -o build\md2docx-server-windows.exe .\cmd\server

if not exist "build\md2docx-server-windows.exe" (
    echo ❌ Go后端编译失败
    exit /b 1
)
echo ✅ Go后端编译完成: build\md2docx-server-windows.exe

echo.

REM 构建Qt前端
echo 4. 构建Qt前端...
set "PATH=C:\Qt\5.15.2\msvc2019_64\bin;%PATH%"

cd qt-frontend
if exist "build_complete_test" rmdir /s /q build_complete_test
mkdir build_complete_test
cd build_complete_test

echo    运行qmake...
qmake CONFIG+=release ..\complete_test.pro

echo    运行nmake...
nmake

if not exist "build\complete_test.exe" (
    echo ❌ Qt前端编译失败
    exit /b 1
)
echo ✅ Qt前端编译完成

cd /d "%PROJECT_DIR%"

echo.

REM 创建应用程序目录
echo 5. 创建应用程序目录...
set "APP_DIR=%BUILD_DIR%\%APP_NAME%"
mkdir "%APP_DIR%"
mkdir "%APP_DIR%\backend"
mkdir "%APP_DIR%\docs"
mkdir "%APP_DIR%\examples"

REM 复制Qt应用程序
copy "qt-frontend\build_complete_test\build\complete_test.exe" "%APP_DIR%\%APP_NAME%.exe"

REM 复制后端服务
copy "build\md2docx-server-windows.exe" "%APP_DIR%\backend\"

REM 复制资源文件
if exist "docs" xcopy /s /e /i "docs\*" "%APP_DIR%\docs\"
if exist "tests\api_test\*.md" copy "tests\api_test\*.md" "%APP_DIR%\examples\"

echo ✅ 应用程序目录创建完成

echo.

REM 处理Qt依赖
echo 6. 处理Qt依赖...
where windeployqt >nul 2>&1
if %errorlevel% equ 0 (
    echo    运行windeployqt...
    windeployqt --release --no-translations "%APP_DIR%\%APP_NAME%.exe"
    echo ✅ Qt依赖处理完成
) else (
    echo ⚠️  windeployqt未找到，手动复制Qt依赖...
    
    REM 手动复制必要的Qt DLL
    set "QT_BIN=C:\Qt\5.15.2\msvc2019_64\bin"
    if exist "%QT_BIN%" (
        copy "%QT_BIN%\Qt5Core.dll" "%APP_DIR%\"
        copy "%QT_BIN%\Qt5Gui.dll" "%APP_DIR%\"
        copy "%QT_BIN%\Qt5Widgets.dll" "%APP_DIR%\"
        copy "%QT_BIN%\Qt5Network.dll" "%APP_DIR%\"
        
        REM 复制平台插件
        mkdir "%APP_DIR%\platforms"
        copy "%QT_BIN%\..\plugins\platforms\qwindows.dll" "%APP_DIR%\platforms\"
        
        echo ✅ Qt依赖手动复制完成
    ) else (
        echo ⚠️  Qt安装目录未找到，用户需要确保系统已安装Qt运行时
    )
)

echo.

REM 创建启动脚本
echo 7. 创建启动脚本...
set "LAUNCHER_SCRIPT=%APP_DIR%\启动应用.bat"
(
echo @echo off
echo setlocal
echo.
echo REM 获取应用程序目录
echo set "APP_DIR=%%~dp0"
echo set "BACKEND_DIR=%%APP_DIR%%backend"
echo.
echo REM 启动后端服务
echo echo 启动后端服务...
echo cd /d "%%BACKEND_DIR%%"
echo start /b md2docx-server-windows.exe
echo.
echo REM 等待后端服务启动
echo timeout /t 3 /nobreak ^>nul
echo.
echo REM 启动前端应用
echo echo 启动前端应用...
echo cd /d "%%APP_DIR%%"
echo start "" "%APP_NAME%.exe"
echo.
echo exit
) > "%LAUNCHER_SCRIPT%"

echo ✅ 启动脚本创建完成

echo.

REM 创建README文件
echo 8. 创建说明文件...
set "README_FILE=%APP_DIR%\README.txt"
(
echo Markdown转Word工具 v%VERSION%
echo.
echo 安装说明:
echo 1. 解压所有文件到任意目录
echo 2. 确保系统已安装Pandoc: https://pandoc.org/installing.html
echo 3. 双击"启动应用.bat"启动程序
echo 4. 或者直接双击"%APP_NAME%.exe"启动前端界面
echo.
echo 功能特性:
echo - 单文件Markdown转Word
echo - 批量文件转换
echo - 自定义模板支持
echo - 配置管理
echo.
echo 系统要求:
echo - Windows 10 或更高版本
echo - Pandoc 2.0 或更高版本
echo.
echo 技术支持: https://github.com/md2docx
echo 版权所有 © 2025 MD2DOCX
) > "%README_FILE%"

echo ✅ 说明文件创建完成

echo.

REM 创建安装程序（使用7-Zip）
echo 9. 创建安装包...
set "INSTALLER_NAME=Markdown转Word工具-v%VERSION%-Windows.zip"
set "INSTALLER_PATH=%BUILD_DIR%\%INSTALLER_NAME%"

where 7z >nul 2>&1
if %errorlevel% equ 0 (
    echo    使用7-Zip创建压缩包...
    cd /d "%BUILD_DIR%"
    7z a -tzip "%INSTALLER_NAME%" "%APP_NAME%"
    echo ✅ 安装包创建完成: %INSTALLER_PATH%
) else (
    echo ⚠️  7-Zip未找到，使用PowerShell创建压缩包...
    powershell -command "Compress-Archive -Path '%APP_DIR%' -DestinationPath '%INSTALLER_PATH%' -Force"
    if exist "%INSTALLER_PATH%" (
        echo ✅ 安装包创建完成: %INSTALLER_PATH%
    ) else (
        echo ❌ 压缩包创建失败
    )
)

echo.

REM 创建便携版
echo 10. 创建便携版...
set "PORTABLE_NAME=Markdown转Word工具-v%VERSION%-Windows-Portable.zip"
set "PORTABLE_PATH=%BUILD_DIR%\%PORTABLE_NAME%"

REM 复制便携版文件
set "PORTABLE_DIR=%BUILD_DIR%\Portable"
mkdir "%PORTABLE_DIR%"
xcopy /s /e /i "%APP_DIR%" "%PORTABLE_DIR%\%APP_NAME%"

REM 创建便携版启动器
set "PORTABLE_LAUNCHER=%PORTABLE_DIR%\%APP_NAME%\Markdown转Word工具.bat"
(
echo @echo off
echo title Markdown转Word工具
echo echo 正在启动Markdown转Word工具...
echo.
echo REM 设置便携模式
echo set PORTABLE_MODE=1
echo.
echo REM 启动后端服务
echo cd /d "%%~dp0backend"
echo start /b md2docx-server-windows.exe
echo.
echo REM 等待服务启动
echo timeout /t 2 /nobreak ^>nul
echo.
echo REM 启动前端
echo cd /d "%%~dp0"
echo "%APP_NAME%.exe"
) > "%PORTABLE_LAUNCHER%"

REM 压缩便携版
if exist "%PORTABLE_PATH%" del "%PORTABLE_PATH%"
where 7z >nul 2>&1
if %errorlevel% equ 0 (
    cd /d "%BUILD_DIR%"
    7z a -tzip "%PORTABLE_NAME%" "Portable"
) else (
    powershell -command "Compress-Archive -Path '%PORTABLE_DIR%' -DestinationPath '%PORTABLE_PATH%' -Force"
)

if exist "%PORTABLE_PATH%" (
    echo ✅ 便携版创建完成: %PORTABLE_PATH%
) else (
    echo ❌ 便携版创建失败
)

REM 清理临时文件
rmdir /s /q "%PORTABLE_DIR%"

echo.

REM 显示打包结果
echo === 打包完成 ===
echo 📦 应用程序目录: %APP_DIR%
echo 💿 安装包: %INSTALLER_PATH%
echo 📁 便携版: %PORTABLE_PATH%
echo.
echo 📊 文件大小:
if exist "%INSTALLER_PATH%" dir "%INSTALLER_PATH%" | findstr /v "个文件"
if exist "%PORTABLE_PATH%" dir "%PORTABLE_PATH%" | findstr /v "个文件"
echo.
echo 🚀 安装说明:
echo 1. 解压.zip文件到任意目录
echo 2. 确保系统已安装Pandoc
echo 3. 双击"启动应用.bat"启动程序
echo.
echo ✅ Windows打包完成！

pause
