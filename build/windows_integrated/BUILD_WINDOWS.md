# Windows整合版构建说明

## 前提条件
- Windows 10 或更高版本
- Qt 5.15.x (MSVC 2019)
- Visual Studio 2019 或更高版本

## 构建步骤

1. 打开Qt命令提示符
2. 进入项目目录
3. 运行以下命令：

```cmd
cd qt-frontend
mkdir build_simple_integrated
cd build_simple_integrated
qmake ..\md2docx_simple_integrated.pro
nmake
```

4. 构建完成后，将 md2docx-server-windows.exe 复制到可执行文件目录

## 注意事项
- 确保Qt路径正确设置
- 可能需要运行 windeployqt 部署依赖
