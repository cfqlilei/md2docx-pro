# Markdown 转 Word 工具 - 整合版

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Go Version](https://img.shields.io/badge/Go-1.25+-blue.svg)](https://golang.org)
[![Qt Version](https://img.shields.io/badge/Qt-5.15-green.svg)](https://www.qt.io)
[![Pandoc Version](https://img.shields.io/badge/Pandoc-3.8+-red.svg)](https://pandoc.org)

## 📖 项目概况

这是一个基于 **Qt + Go** 架构的跨平台 Markdown 转 Word 转换工具，提供直观的图形界面和强大的转换功能。项目采用前后端分离设计，支持单文件转换、批量转换和灵活的配置管理。

### ✨ 主要特点

- 🖥️ **跨平台支持**: 支持 macOS 和 Windows 系统
- 🎯 **整合版设计**: 前后端合并为单一程序，双击即可运行
- 🔄 **多种转换模式**: 单文件转换、批量转换
- ⚙️ **灵活配置**: 支持自定义 Pandoc 路径和 Word 模板
- 🚀 **动态端口**: 自动分配可用端口，避免冲突
- 📊 **实时状态**: 转换进度和状态实时显示
- 🎨 **现代界面**: 基于 Qt 的现代化用户界面

## 🛠️ 技术栈

### 前端技术

- **Qt 5.15**: C++ GUI 框架，提供跨平台界面
- **Qt Widgets**: 传统桌面应用组件
- **Qt Network**: HTTP 客户端通信

### 后端技术

- **Go 1.25+**: 高性能后端服务
- **Gin Framework**: 轻量级 Web 框架
- **Gorilla Mux**: HTTP 路由器
- **JSON**: 数据交换格式

### 转换引擎

- **Pandoc 3.8+**: 强大的文档转换工具
- **Word 模板**: 支持自定义 .docx 模板

### 开发工具

- **VSCode**: 推荐开发环境
- **Git**: 版本控制
- **Go Modules**: 依赖管理
- **QMake**: Qt 项目构建

## 🚀 功能说明

### 1. 单文件转换

- 选择单个 Markdown 文件进行转换
- 支持自定义输出文件名和路径
- 实时显示转换进度和结果

### 2. 批量转换

- 同时转换多个 Markdown 文件
- 支持文件夹批量选择
- 统一输出目录管理

### 3. 配置管理

- **Pandoc 路径配置**: 自动检测或手动设置 Pandoc 路径
- **模板文件配置**: 支持自定义 Word 模板文件
- **配置验证**: 一键验证所有配置是否正确
- **配置持久化**: 自动保存和加载配置

### 4. 状态监控

- 实时显示转换状态
- 详细的错误信息和日志
- 服务器连接状态监控

### 5. 用户体验

- 直观的拖拽操作
- 快捷键支持
- 多语言界面（中文）
- 响应式布局

## 📋 系统要求

### macOS

- macOS 10.13 或更高版本
- Xcode Command Line Tools
- Homebrew (推荐)

### Windows

- Windows 10 或更高版本
- Visual Studio 2019 或更高版本 (含 MSVC 编译器)
- Git for Windows

### 通用要求

- Go 1.25 或更高版本
- Qt 5.15.x
- Pandoc 3.8 或更高版本
- 至少 4GB RAM
- 500MB 可用磁盘空间

## 🔧 快速开始

### 1. 环境安装

#### macOS 环境

```bash
# 安装 Homebrew
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# 安装依赖
brew install go qt@5 pandoc

# 设置环境变量
echo 'export PATH="/opt/homebrew/opt/qt@5/bin:$PATH"' >> ~/.zshrc
source ~/.zshrc
```

#### Windows 环境

```cmd
# 1. 从官网下载并安装 Go: https://golang.org/dl/
# 2. 从官网下载并安装 Qt 5.15.2: https://www.qt.io/download-qt-installer
# 3. 从官网下载并安装 Pandoc: https://pandoc.org/installing.html
# 4. 安装 Visual Studio 2019 或更高版本

# 设置环境变量（添加到系统 PATH）
# C:\Qt\5.15.2\msvc2019_64\bin
# C:\Go\bin
# Pandoc 安装目录
```

### 2. 构建项目

#### 一键构建（推荐）

```bash
# macOS
./scripts/all_in_one_integrated.sh

# Windows
scripts\all_in_one_integrated_windows.bat
```

#### 分步构建

```bash
# macOS
./scripts/clean_integrated.sh      # 清理
./scripts/compile_integrated.sh    # 编译
./scripts/build_integrated.sh      # 构建
./scripts/run_integrated.sh        # 运行

# Windows
scripts\clean_integrated_windows.bat
scripts\compile_integrated_windows.bat
scripts\build_integrated_windows.bat
scripts\run_integrated_windows.bat
```

### 3. 启动应用

```bash
# macOS
./launch_integrated.sh

# Windows
launch_integrated.bat
```

## 📁 项目结构

```
md2docx-src/
├── cmd/                    # Go 应用入口
│   └── server/            # 后端服务器
├── internal/              # 内部包
│   ├── api/              # API 处理器
│   ├── config/           # 配置管理
│   ├── converter/        # 转换逻辑
│   └── models/           # 数据模型
├── pkg/                   # 公共包
├── qt-frontend/           # Qt 前端
│   ├── src/              # C++ 源码
│   ├── ui/               # UI 文件
│   └── resources/        # 资源文件
├── web/                   # Web 资源
│   ├── static/           # 静态文件
│   └── templates/        # HTML 模板
├── scripts/               # 构建脚本
├── tests/                 # 测试文件
├── docs/                  # 文档
└── build/                 # 构建输出
```

## 🔧 详细编译说明

### VSCode 开发环境配置

#### 1. 安装 VSCode 扩展

必需扩展:

- Go (Google)
- C/C++ (Microsoft)
- Qt tools (tonka3000)

推荐扩展:

- GitLens
- Markdown All in One
- Thunder Client (API 测试)

#### 2. 项目配置

项目已包含完整的 VSCode 配置:

- `.vscode/launch.json` - 调试配置
- `.vscode/tasks.json` - 构建任务
- `.vscode/settings.json` - 项目设置

### 使用 VSCode 构建（推荐）

1. **打开项目**

   ```bash
   cd md2docx-src
   code .
   ```

2. **构建项目**

   - 按 `Ctrl+Shift+P` (Windows) 或 `Cmd+Shift+P` (macOS)
   - 输入 "Tasks: Run Task"
   - 选择对应的构建任务

3. **运行调试**
   - 按 `F5` 或点击调试面板的运行按钮
   - 选择对应的启动配置

### 命令行构建

#### macOS 命令行构建

```bash
# 1. 克隆项目
git clone <repository-url>
cd md2docx-src

# 2. 构建Go后端
go mod tidy
mkdir -p build
CGO_ENABLED=1 GOOS=darwin GOARCH=arm64 go build -buildvcs=false -o build/md2docx-server-macos ./cmd/server

# 3. 构建Qt前端
export PATH="/opt/homebrew/opt/qt@5/bin:$PATH"
cd qt-frontend
rm -rf build_md2docx_app
mkdir build_md2docx_app
cd build_md2docx_app
qmake ../md2docx_app.pro
make

# 4. 验证构建结果
cd ../../
ls -la build/md2docx-server-macos
ls -la qt-frontend/build_md2docx_app/build/md2docx_app.app
```

#### Windows 命令行构建

```cmd
REM 1. 克隆项目
git clone <repository-url>
cd md2docx-src

REM 2. 构建Go后端
go mod tidy
mkdir build
set CGO_ENABLED=1
set GOOS=windows
set GOARCH=amd64
go build -buildvcs=false -o build\md2docx-server-windows.exe .\cmd\server

REM 3. 构建Qt前端
set PATH=C:\Qt\5.15.2\msvc2019_64\bin;%PATH%
cd qt-frontend
rmdir /s /q build_md2docx_app
mkdir build_md2docx_app
cd build_md2docx_app
qmake ..\md2docx_app.pro
nmake

REM 4. 验证构建结果
cd ..\..\
dir build\md2docx-server-windows.exe
dir qt-frontend\build_md2docx_app\build\md2docx_app.exe
```

## 🧪 测试

### 运行测试

```bash
# 运行所有测试
./tests/run_all_tests.sh

# 运行单元测试
go test ./...

# 运行集成测试
./tests/run_improvements_tests.sh
```

### API 测试

```bash
# 测试 API 功能
./test_api_features.sh

# 手动测试健康检查
curl http://localhost:8080/api/health
```

## 🐛 调试说明

### Go 后端调试

1. **VSCode 调试**

   - 在 Go 代码中设置断点
   - 选择 "调试 Go 后端服务 (macOS/Windows)"
   - 按 F5 启动调试

2. **命令行调试**

   ```bash
   # 安装delve调试器
   go install github.com/go-delve/delve/cmd/dlv@latest

   # 启动调试
   dlv debug ./cmd/server
   ```

### Qt 前端调试

1. **VSCode 调试**

   - 在 C++代码中设置断点
   - 选择 "调试 Qt 前端 (macOS/Windows)"
   - 按 F5 启动调试

2. **Qt Creator 调试**
   - 打开 `qt-frontend/complete_test.pro`
   - 设置断点
   - 按 F5 启动调试

## 📦 打包发布

### macOS 打包

```bash
./scripts/package_macos.sh
```

输出: `dist/macos/Markdown转Word工具-v1.0.0-macOS.dmg`

### Windows 打包

```cmd
scripts\package_windows.bat
```

输出: `dist\windows\Markdown转Word工具-v1.0.0-Windows.zip`

## 🐛 常见问题

### Q: 应用启动后只显示命令行界面？

**A**: 请使用正确的启动脚本：

- macOS: `./launch_integrated.sh`
- Windows: `launch_integrated.bat`

不要直接运行后端服务器文件。

### Q: 转换失败，提示 Pandoc 未找到？

**A**: 请确保 Pandoc 已正确安装并添加到系统 PATH 中。可以在设置界面点击"验证配置"检查。

### Q: 端口冲突问题？

**A**: 应用支持动态端口分配（8080-8090），会自动选择可用端口。

## 🤝 贡献指南

1. Fork 本项目
2. 创建特性分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 开启 Pull Request

## 📄 许可证

本项目采用 MIT 许可证 - 查看 [LICENSE](LICENSE) 文件了解详情。

## 📞 技术支持

- 🐛 问题反馈: [GitHub Issues](https://github.com/md2docx/issues)
- 📖 开发文档: [docs/](docs/) 目录
- 💬 讨论交流: [GitHub Discussions](https://github.com/md2docx/discussions)

---

**祝您使用愉快！** 🎉
