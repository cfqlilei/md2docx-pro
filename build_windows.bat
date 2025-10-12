@echo off
REM Windows构建脚本 - Markdown转Word工具

echo === Windows Qt构建脚本 ===
echo 构建时间: %date% %time%
echo ============================

REM 检查Qt环境
echo 1. 检查Qt环境...
where qmake >nul 2>&1
if %errorlevel% neq 0 (
    echo ❌ 错误: 未找到qmake，请安装Qt开发环境
    echo 请确保Qt的bin目录在PATH环境变量中
    echo 例如: C:\Qt\5.15.2\msvc2019_64\bin
    pause
    exit /b 1
)

for /f "tokens=*" %%i in ('qmake -version') do (
    echo ✅ %%i
    goto :qt_found
)
:qt_found

REM 检查编译器
echo.
echo 2. 检查编译器...
where cl >nul 2>&1
if %errorlevel% neq 0 (
    echo ❌ 错误: 未找到MSVC编译器
    echo 请安装Visual Studio或Visual Studio Build Tools
    echo 并运行vcvarsall.bat设置环境变量
    pause
    exit /b 1
)

echo ✅ MSVC编译器已找到

REM 检查Go环境
echo.
echo 3. 检查Go环境...
where go >nul 2>&1
if %errorlevel% neq 0 (
    echo ❌ 错误: 未找到Go，请安装Go语言环境
    pause
    exit /b 1
)

for /f "tokens=*" %%i in ('go version') do (
    echo ✅ %%i
    goto :go_found
)
:go_found

REM 检查Pandoc
echo.
echo 4. 检查Pandoc...
where pandoc >nul 2>&1
if %errorlevel% neq 0 (
    echo ❌ 错误: 未找到Pandoc，请安装Pandoc
    echo 下载地址: https://pandoc.org/installing.html
    pause
    exit /b 1
)

for /f "tokens=*" %%i in ('pandoc --version') do (
    echo ✅ %%i
    goto :pandoc_found
)
:pandoc_found

REM 创建构建目录
echo.
echo 5. 准备构建环境...
cd qt-frontend
if exist build rmdir /s /q build
mkdir build
cd build

echo ✅ 构建目录已创建

REM 生成Makefile
echo.
echo 6. 生成Makefile...
qmake ..\md2docx_simple.pro
if %errorlevel% neq 0 (
    echo ❌ qmake失败
    pause
    exit /b 1
)

echo ✅ Makefile生成成功

REM 编译项目
echo.
echo 7. 编译项目...
nmake
if %errorlevel% neq 0 (
    echo ❌ 编译失败
    pause
    exit /b 1
)

echo ✅ 编译成功

REM 检查生成的可执行文件
echo.
echo 8. 检查生成文件...
if exist release\md2docx.exe (
    echo ✅ 可执行文件: release\md2docx.exe
) else if exist debug\md2docx.exe (
    echo ✅ 可执行文件: debug\md2docx.exe
) else (
    echo ❌ 未找到可执行文件
    pause
    exit /b 1
)

REM 创建启动脚本
echo.
echo 9. 创建启动脚本...
cd ..\..

echo @echo off > start_app_windows.bat
echo echo === Markdown转Word工具 - Windows版 === >> start_app_windows.bat
echo echo 启动时间: %%date%% %%time%% >> start_app_windows.bat
echo echo ======================================= >> start_app_windows.bat
echo. >> start_app_windows.bat
echo echo 1. 启动Go后端服务... >> start_app_windows.bat
echo start /b go run cmd\server\main.go >> start_app_windows.bat
echo timeout /t 3 /nobreak ^>nul >> start_app_windows.bat
echo. >> start_app_windows.bat
echo echo 2. 启动Qt前端应用... >> start_app_windows.bat
if exist qt-frontend\build\release\md2docx.exe (
    echo start qt-frontend\build\release\md2docx.exe >> start_app_windows.bat
) else (
    echo start qt-frontend\build\debug\md2docx.exe >> start_app_windows.bat
)
echo. >> start_app_windows.bat
echo echo ======================================= >> start_app_windows.bat
echo echo 应用程序已启动！ >> start_app_windows.bat
echo echo 后端服务: http://localhost:8080 >> start_app_windows.bat
echo echo ======================================= >> start_app_windows.bat
echo pause >> start_app_windows.bat

echo ✅ Windows启动脚本已创建: start_app_windows.bat

echo.
echo ============================
echo Windows构建完成！
echo.
echo 生成的文件:
if exist qt-frontend\build\release\md2docx.exe (
    echo   可执行文件: qt-frontend\build\release\md2docx.exe
) else (
    echo   可执行文件: qt-frontend\build\debug\md2docx.exe
)
echo   启动脚本: start_app_windows.bat
echo.
echo 使用方法:
echo   1. 双击 start_app_windows.bat 启动完整应用
echo   2. 或者手动启动后端和前端
echo.
echo 注意事项:
echo   - 确保Qt DLL文件在PATH中或与exe文件同目录
echo   - 如需发布，请使用windeployqt工具打包依赖
echo ============================
pause
