# Windows整合版构建说明

## 前提条件
- Windows 10 或更高版本
- Qt 5.15.x (MSVC 2019)
- Visual Studio 2019 或更高版本

## 构建步骤

1. 打开Qt命令提示符
2. 进入项目目录
3. 运行Windows构建脚本：

```cmd
scripts\compile_integrated_windows.bat
scripts\build_integrated_windows.bat
```

## 注意事项
- 确保Qt路径正确设置
- 可能需要运行 windeployqt 部署依赖
- 最终应用将输出到 build\release\ 目录
