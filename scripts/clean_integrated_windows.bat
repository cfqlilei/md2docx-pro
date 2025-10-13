@echo off
REM 整合版清理脚本 - Windows版本
REM 清理所有构建文件和临时文件

echo === Markdown转Word工具 - 整合版清理 ===
echo 开始清理构建文件...

REM 清理build目录（统一构建输出目录）
echo 清理build目录...
if exist "build" (
    rmdir /s /q "build"
    echo ✅ 删除 build\ 目录
) else (
    echo ⚠️  build\ 目录不存在
)

REM 重新创建build目录结构
mkdir build\release
echo ✅ 重新创建 build\release\ 目录

REM 清理Go模块缓存
echo 清理Go模块缓存...
go clean -cache
go clean -modcache
echo ✅ Go缓存清理完成

REM 清理Qt构建文件
echo 清理Qt前端构建文件...

REM 清理整合版构建
if exist "qt-frontend\build_simple_integrated" (
    rmdir /s /q "qt-frontend\build_simple_integrated"
    echo ✅ 删除 qt-frontend\build_simple_integrated\ 目录
) else (
    echo ⚠️  qt-frontend\build_simple_integrated\ 目录不存在
)

REM 清理分离版构建
if exist "qt-frontend\build_md2docx_app" (
    rmdir /s /q "qt-frontend\build_md2docx_app"
    echo ✅ 删除 qt-frontend\build_md2docx_app\ 目录
) else (
    echo ⚠️  qt-frontend\build_md2docx_app\ 目录不存在
)

REM 清理其他构建目录
for %%d in (qt-frontend\build_integrated qt-frontend\build qt-frontend\debug qt-frontend\release) do (
    if exist "%%d" (
        rmdir /s /q "%%d"
        echo ✅ 删除 %%d\ 目录
    )
)

REM 清理临时文件
echo 清理临时文件...
del /s /q "*.tmp" 2>nul
del /s /q "*.log" 2>nul

REM 清理Qt临时文件
for /r qt-frontend %%f in (Makefile* *.o moc_* qrc_* ui_*) do (
    if exist "%%f" del /q "%%f" 2>nul
)

echo ✅ 临时文件清理完成

REM 清理启动脚本
echo 清理生成的启动脚本...
for %%s in (launch_windows.bat deploy_integrated.bat) do (
    if exist "%%s" (
        del /q "%%s"
        echo ✅ 删除 %%s
    )
)

REM 清理测试结果
echo 清理测试结果...
if exist "tests\results" (
    rmdir /s /q "tests\results"
    mkdir "tests\results"
    echo ✅ 清理测试结果目录
)

REM 清理分发文件
echo 清理分发文件...
if exist "dist" (
    rmdir /s /q "dist"
    echo ✅ 删除 dist\ 目录
)

echo.
echo ✅ === 清理完成 ===
echo 已清理的内容:
echo   ✓ Go后端构建文件 (build\)
echo   ✓ Go模块缓存
echo   ✓ Qt前端构建文件
echo   ✓ 临时文件和缓存
echo   ✓ 生成的启动脚本
echo   ✓ 测试结果文件
echo   ✓ 分发文件
echo.
echo 现在可以进行全新构建了！
pause
