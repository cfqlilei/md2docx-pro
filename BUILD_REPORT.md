# 整合版构建报告

生成时间: 2025年10月13日 星期一 18时48分11秒 CST

## 构建结果

### macOS版本
- ✅ 应用包: qt-frontend/build_simple_integrated/build_simple_integrated/release/md2docx_simple_integrated.app
- 📦 大小: 260K
- 🔧 前端: 252K
- 🔧 后端: 

### Windows版本
- ✅ 后端: build/md2docx-server-windows.exe ()
- ⚠️  前端: 需要在Windows环境下构建

## 启动方式

### macOS
```bash
./launch_integrated_simple.sh
```

### Windows
```cmd
launch_integrated_windows.bat
```

## 文件结构

```
md2docx-src/
├── build/
│   ├── md2docx-server-macos          # macOS后端
│   ├── md2docx-server-windows.exe    # Windows后端
│   └── windows_integrated/           # Windows构建文件
├── qt-frontend/build_simple_integrated/build_simple_integrated/release/
│   └── md2docx_simple_integrated.app # macOS整合版应用
├── launch_integrated_simple.sh       # macOS启动脚本
└── launch_integrated_windows.bat     # Windows启动脚本
```
