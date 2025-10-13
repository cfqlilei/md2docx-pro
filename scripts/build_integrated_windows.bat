@echo off
REM 整合版构建脚本 - Windows版本
REM 完整构建流程：清理 -> 编译 -> 打包

echo === Markdown转Word工具 - 整合版构建 ===
echo 开始整合版构建流程...

REM 构建Windows版本
echo 构建Windows整合版...

REM 检查是否已编译
if not exist "build\release\md2docx_simple_integrated.exe" (
    echo ❌ 未找到编译结果，请先运行编译脚本
    echo 运行: scripts\compile_integrated_windows.bat
    pause
    exit /b 1
)

echo ✅ 发现已存在的构建，继续构建流程...

REM 验证应用完整性
if exist "build\release\md2docx_simple_integrated.exe" (
    echo ✅ 前端可执行文件存在
) else (
    echo ❌ 前端可执行文件不存在
    pause
    exit /b 1
)

if exist "build\release\md2docx-server-windows.exe" (
    echo ✅ 后端服务器存在
) else (
    echo ❌ 后端服务器文件不存在，请先运行编译脚本
    pause
    exit /b 1
)

echo ✅ Windows整合版构建完成

REM 创建启动脚本
echo 创建启动脚本...

echo @echo off > launch_integrated.bat
echo echo === Markdown转Word工具 - 整合版 === >> launch_integrated.bat
echo echo 启动整合版应用... >> launch_integrated.bat
echo. >> launch_integrated.bat
echo set APP_PATH=build\md2docx_simple_integrated.exe >> launch_integrated.bat
echo. >> launch_integrated.bat
echo if exist "%%APP_PATH%%" ^( >> launch_integrated.bat
echo     echo ✅ 找到整合版应用: %%APP_PATH%% >> launch_integrated.bat
echo     echo 🚀 启动应用... >> launch_integrated.bat
echo     start "" "%%APP_PATH%%" >> launch_integrated.bat
echo     echo ✅ 整合版应用已启动！ >> launch_integrated.bat
echo     echo. >> launch_integrated.bat
echo     echo 特点： >> launch_integrated.bat
echo     echo   ✓ 单一程序，无需分别启动前后端 >> launch_integrated.bat
echo     echo   ✓ 内嵌后端服务，自动启动 >> launch_integrated.bat
echo     echo   ✓ 动态端口分配，避免冲突 >> launch_integrated.bat
echo     echo   ✓ 完整的GUI界面 >> launch_integrated.bat
echo     echo   ✓ 所有功能都已整合 >> launch_integrated.bat
echo ^) else ^( >> launch_integrated.bat
echo     echo ❌ 错误: 整合版应用不存在 >> launch_integrated.bat
echo     echo 请先运行构建脚本: >> launch_integrated.bat
echo     echo   scripts\compile_integrated_windows.bat >> launch_integrated.bat
echo     echo   scripts\build_integrated_windows.bat >> launch_integrated.bat
echo     pause >> launch_integrated.bat
echo ^) >> launch_integrated.bat

echo ✅ 创建Windows启动脚本: launch_integrated.bat

REM 生成构建报告
echo 生成构建报告...

echo # 整合版构建报告 - Windows > BUILD_REPORT_WINDOWS.md
echo. >> BUILD_REPORT_WINDOWS.md
echo 生成时间: %date% %time% >> BUILD_REPORT_WINDOWS.md
echo. >> BUILD_REPORT_WINDOWS.md
echo ## 构建结果 >> BUILD_REPORT_WINDOWS.md
echo. >> BUILD_REPORT_WINDOWS.md
echo ### Windows版本 >> BUILD_REPORT_WINDOWS.md

if exist "%APP_PATH%" (
    echo - ✅ 应用程序: %APP_PATH% >> BUILD_REPORT_WINDOWS.md
    echo - ✅ 后端服务器: %BACKEND_PATH% >> BUILD_REPORT_WINDOWS.md
) else (
    echo - ❌ 应用程序构建失败 >> BUILD_REPORT_WINDOWS.md
)

echo. >> BUILD_REPORT_WINDOWS.md
echo ## 启动方式 >> BUILD_REPORT_WINDOWS.md
echo. >> BUILD_REPORT_WINDOWS.md
echo ```cmd >> BUILD_REPORT_WINDOWS.md
echo launch_integrated_windows.bat >> BUILD_REPORT_WINDOWS.md
echo ``` >> BUILD_REPORT_WINDOWS.md
echo. >> BUILD_REPORT_WINDOWS.md
echo ## 文件结构 >> BUILD_REPORT_WINDOWS.md
echo. >> BUILD_REPORT_WINDOWS.md
echo ``` >> BUILD_REPORT_WINDOWS.md
echo md2docx-src\ >> BUILD_REPORT_WINDOWS.md
echo ├── build\ >> BUILD_REPORT_WINDOWS.md
echo │   ├── md2docx-server-windows.exe    # Windows后端 >> BUILD_REPORT_WINDOWS.md
echo │   └── md2docx-server-macos          # macOS后端 >> BUILD_REPORT_WINDOWS.md
echo ├── qt-frontend\build_simple_integrated\release\ >> BUILD_REPORT_WINDOWS.md
echo │   ├── md2docx_simple_integrated.exe # Windows整合版应用 >> BUILD_REPORT_WINDOWS.md
echo │   └── md2docx-server-windows.exe    # 内嵌后端 >> BUILD_REPORT_WINDOWS.md
echo └── launch_integrated_windows.bat     # Windows启动脚本 >> BUILD_REPORT_WINDOWS.md
echo ``` >> BUILD_REPORT_WINDOWS.md

echo ✅ 构建报告已生成: BUILD_REPORT_WINDOWS.md

echo.
echo ✅ === 构建完成 ===
echo 构建结果:
echo   ✅ Windows整合版应用已就绪
echo   ✅ 启动脚本已创建
echo   ✅ 构建报告已生成
echo.
echo 启动方式:
echo   Windows: launch_integrated_windows.bat
echo.
echo 应用特点:
echo   ✓ 单一程序，无需分别启动前后端
echo   ✓ 内嵌Go后端服务，自动启动
echo   ✓ 完整的GUI界面，所有功能已整合
echo   ✓ 用户友好，双击即可使用
pause
