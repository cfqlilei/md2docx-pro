# Markdown 转 Word 工具 - 编译、调试、构建说明

## 项目概述

本项目是一个基于 Qt + Go 的跨平台 Markdown 转 Word 转换工具，支持单文件转换、批量转换和配置管理功能。

**技术栈**:

- 前端: Qt 5.15 (C++)
- 后端: Go 1.25+
- 转换引擎: Pandoc 3.8+

## 系统要求

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
- VSCode (推荐开发环境)

## 环境安装

### macOS 环境安装

```bash
# 1. 安装Homebrew (如果未安装)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# 2. 安装Go
brew install go

# 3. 安装Qt 5.15
brew install qt@5

# 4. 安装Pandoc
brew install pandoc

# 5. 设置Qt环境变量
echo 'export PATH="/opt/homebrew/opt/qt@5/bin:$PATH"' >> ~/.zshrc
source ~/.zshrc

# 6. 验证安装
go version
qmake --version
pandoc --version
```

### Windows 环境安装

```cmd
# 1. 安装Go
# 从 https://golang.org/dl/ 下载并安装Go

# 2. 安装Qt 5.15.2
# 从 https://www.qt.io/download-qt-installer 下载Qt在线安装器
# 选择Qt 5.15.2 MSVC2019 64-bit组件

# 3. 安装Pandoc
# 从 https://pandoc.org/installing.html 下载并安装

# 4. 安装Visual Studio 2019或更高版本
# 确保包含MSVC编译器和Windows SDK

# 5. 设置环境变量
# 将以下路径添加到系统PATH:
# C:\Qt\5.15.2\msvc2019_64\bin
# C:\Go\bin
# Pandoc安装目录

# 6. 验证安装
go version
qmake -v
pandoc --version
```

## VSCode 开发环境配置

### 1. 安装 VSCode 扩展

必需扩展:

- Go (Google)
- C/C++ (Microsoft)
- Qt tools (tonka3000)

推荐扩展:

- GitLens
- Markdown All in One
- Thunder Client (API 测试)

### 2. 项目配置

项目已包含完整的 VSCode 配置:

- `.vscode/launch.json` - 调试配置
- `.vscode/tasks.json` - 构建任务
- `.vscode/settings.json` - 项目设置

## 编译构建

### 使用 VSCode (推荐)

1. **打开项目**

   ```bash
   cd md2docx-src
   code .
   ```

2. **构建项目**

   - 按 `Ctrl+Shift+P` (Windows) 或 `Cmd+Shift+P` (macOS)
   - 输入 "Tasks: Run Task"
   - 选择对应的构建任务:
     - `build-complete-app-macos` - 构建 macOS 版本
     - `build-complete-app-windows` - 构建 Windows 版本

3. **运行调试**
   - 按 `F5` 或点击调试面板的运行按钮
   - 选择对应的启动配置:
     - `启动前后端 (macOS)` - 同时启动前后端
     - `启动前后端 (Windows)` - 同时启动前后端
     - `调试前后端 (macOS)` - 调试模式启动
     - `调试前后端 (Windows)` - 调试模式启动

### 已验证的启动配置

所有 VSCode 启动配置已经过测试和修复：

#### Go 后端配置 ✅

- `启动Go后端服务 (macOS)` - 正常工作
- `启动Go后端服务 (Windows)` - 配置正确
- `调试Go后端服务 (macOS)` - 正常工作
- `调试Go后端服务 (Windows)` - 配置正确

#### Qt 前端配置 ✅

- `启动Qt前端 (macOS)` - 正常工作 (使用 single_test)
- `启动Qt前端 (Windows)` - 配置正确
- `调试Qt前端 (macOS)` - 正常工作
- `调试Qt前端 (Windows)` - 配置正确

#### 完整应用配置 ✅

- `启动完整应用 (macOS)` - 已创建启动脚本
- `启动完整应用 (Windows)` - 已创建启动脚本

#### 复合配置 ✅

- `启动前后端 (macOS)` - 正常工作
- `启动前后端 (Windows)` - 配置正确
- `调试前后端 (macOS)` - 正常工作
- `调试前后端 (Windows)` - 配置正确

### 命令行构建

#### macOS 命令行构建

```bash
# 1. 克隆项目
git clone <repository-url>
cd md2docx-src

# 2. 构建Go后端 (修复VCS问题)
go mod tidy
mkdir -p build
CGO_ENABLED=1 GOOS=darwin GOARCH=arm64 go build -buildvcs=false -o build/md2docx-server-macos ./cmd/server

# 3. 构建Qt前端 (使用md2docx_app项目)
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

# 5. 运行测试
./test_api_features.sh
```

**注意事项：**

- 添加了 `-buildvcs=false` 标志解决 VCS 冲突问题
- 使用 `md2docx_app.pro` 构建完整的应用程序
- 确保 Qt 路径正确设置：`/opt/homebrew/opt/qt@5/bin`

#### Windows 命令行构建

```cmd
REM 1. 克隆项目
git clone <repository-url>
cd md2docx-src

REM 2. 构建Go后端 (修复VCS问题)
go mod tidy
mkdir build
set CGO_ENABLED=1
set GOOS=windows
set GOARCH=amd64
go build -buildvcs=false -o build\md2docx-server-windows.exe .\cmd\server

REM 3. 构建Qt前端 (使用md2docx_app项目)
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

REM 5. 运行测试
test_api_features.sh
```

**注意事项：**

- 同样添加了 `-buildvcs=false` 标志
- Windows 版本使用 `md2docx_app.pro` 构建完整应用
- 确保 Qt 路径正确：`C:\Qt\5.15.2\msvc2019_64\bin`

### 快速构建和启动

#### 🎯 整合版本 (推荐) - 单一程序

**特点**: 前后端合并为一个程序，双击即可运行，无需分别启动前后端服务。

```bash
# 构建整合版本
cd qt-frontend
mkdir -p build_simple_integrated && cd build_simple_integrated
qmake ../md2docx_simple_integrated.pro && make

# 启动整合版本
cd ../../
./launch_integrated_simple.sh
```

**优势**:

- ✅ 单一程序，用户友好
- ✅ 内嵌后端服务，自动启动
- ✅ 无需技术知识，双击即用
- ✅ 完全自包含，便于分发

#### 分离版本 - 前后端独立运行

```bash
# 构建完整应用 (前后端分离)
./scripts/build_complete_app.sh
```

这个脚本会：

- 检查构建环境 (Go, Qt, Pandoc)
- 构建 macOS 和 Windows 后端
- 构建 macOS 前端
- 创建启动脚本
- 验证构建结果

#### 快速启动脚本

构建完成后，可以使用以下方式启动：

##### macOS

```bash
# 使用Node.js启动脚本 (推荐)
node scripts/launch_complete_app_macos.js

# 或使用简单启动脚本
./launch_macos.sh

# 或手动启动
./build/md2docx-server-macos &
./qt-frontend/build_md2docx_app/build/md2docx_app.app/Contents/MacOS/md2docx_app
```

##### Windows

```cmd
REM 使用Node.js启动脚本 (推荐)
node scripts\launch_complete_app_windows.js

REM 或手动启动
start build\md2docx-server-windows.exe
start qt-frontend\build_md2docx_app\build\md2docx_app.exe
```

**启动脚本功能：**

- 自动检查构建文件是否存在
- 按顺序启动后端服务和前端应用
- 等待后端服务就绪后再启动前端
- 统一的日志输出和错误处理
- 优雅的关闭处理 (Ctrl+C)

### 构建结果说明

成功构建后，项目目录结构如下：

```
md2docx-src/
├── build/                                    # 后端构建输出
│   ├── md2docx-server-macos                 # macOS后端可执行文件
│   └── md2docx-server-windows.exe           # Windows后端可执行文件
├── qt-frontend/
│   └── build_md2docx_app/                   # 前端构建目录
│       └── build/
│           └── md2docx_app.app/             # macOS应用包
│               └── Contents/MacOS/md2docx_app
├── scripts/
│   ├── build_complete_app.sh                # 一键构建脚本
│   ├── launch_complete_app_macos.js         # macOS启动脚本
│   └── launch_complete_app_windows.js       # Windows启动脚本
└── launch_macos.sh                          # 简单启动脚本
```

**文件说明：**

- `build/md2docx-server-macos`: macOS 后端服务器，提供 API 接口
- `build/md2docx-server-windows.exe`: Windows 后端服务器
- `qt-frontend/build_md2docx_app/build/md2docx_app.app`: macOS 前端应用包
- `launch_macos.sh`: 简单的 bash 启动脚本
- `scripts/launch_complete_app_*.js`: 功能完整的 Node.js 启动脚本

## 调试说明

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

### API 调试

使用内置的 API 测试脚本:

```bash
# 运行完整API测试
./test_api_features.sh

# 或使用curl手动测试
curl -s http://localhost:8080/api/health
```

## 打包发布

### macOS 打包

```bash
# 使用VSCode任务
# Ctrl+Shift+P -> Tasks: Run Task -> package-macos

# 或命令行执行
chmod +x scripts/package_macos.sh
./scripts/package_macos.sh
```

输出文件:

- `dist/macos/Markdown转Word工具.app` - 应用程序包
- `dist/macos/Markdown转Word工具-v1.0.0-macOS.dmg` - 安装包
- `dist/macos/Markdown转Word工具-v1.0.0-macOS.zip` - 压缩包

### Windows 打包

```cmd
REM 使用VSCode任务
REM Ctrl+Shift+P -> Tasks: Run Task -> package-windows

REM 或命令行执行
scripts\package_windows.bat
```

输出文件:

- `dist\windows\Markdown转Word工具\` - 应用程序目录
- `dist\windows\Markdown转Word工具-v1.0.0-Windows.zip` - 安装包
- `dist\windows\Markdown转Word工具-v1.0.0-Windows-Portable.zip` - 便携版

## 常见问题

### 编译问题

1. **Qt 找不到**

   ```bash
   # macOS
   export PATH="/opt/homebrew/opt/qt@5/bin:$PATH"

   # Windows
   set PATH=C:\Qt\5.15.2\msvc2019_64\bin;%PATH%
   ```

2. **Go 模块问题**

   ```bash
   go clean -modcache
   go mod tidy
   ```

3. **Pandoc 未找到**

   ```bash
   # macOS
   brew install pandoc

   # Windows
   # 从官网下载安装: https://pandoc.org/installing.html
   ```

### 运行问题

1. **后端服务启动失败**

   - 检查 8080 端口是否被占用
   - 检查 Pandoc 是否正确安装
   - 查看日志文件: `/tmp/md2docx_server.log`

2. **前端界面无法连接后端**

   - 确保后端服务正在运行
   - 检查防火墙设置
   - 验证 API 端点: `curl http://localhost:8080/api/health`

3. **转换失败**
   - 检查输入文件是否存在
   - 检查输出目录权限
   - 验证 Pandoc 安装: `pandoc --version`

### 调试技巧

1. **查看详细日志**

   ```bash
   # 启动后端时显示详细日志
   go run cmd/server/main.go -v
   ```

2. **API 测试**

   ```bash
   # 使用内置测试脚本
   ./test_api_features.sh

   # 手动测试API
   curl -X POST http://localhost:8080/api/convert/single \
     -H "Content-Type: application/json" \
     -d '{"input_file":"test.md","output_dir":"./","output_name":"test"}'
   ```

3. **性能分析**
   ```bash
   # Go性能分析
   go run cmd/server/main.go -cpuprofile=cpu.prof
   go tool pprof cpu.prof
   ```

## 开发工作流

### 日常开发

1. **启动开发环境**

   ```bash
   # 启动后端服务
   go run cmd/server/main.go

   # 另一个终端启动前端
   cd qt-frontend/build_complete_test
   ./build/complete_test.app/Contents/MacOS/complete_test  # macOS
   # 或
   build\complete_test.exe  # Windows
   ```

2. **代码修改后重新构建**

   - Go 代码修改后自动重启 (使用 air 工具)
   - Qt 代码修改后需要重新编译: `make`

3. **提交前检查**

   ```bash
   # 运行测试
   go test ./...
   ./test_api_features.sh

   # 代码格式化
   go fmt ./...
   ```

### 发布流程

1. **更新版本号**

   - 修改 `scripts/package_*.sh` 中的 VERSION 变量
   - 更新 `README.md` 中的版本信息

2. **构建和测试**

   ```bash
   # 构建所有平台版本
   ./scripts/package_macos.sh
   ./scripts/package_windows.bat

   # 测试安装包
   # 在干净的系统上测试安装和运行
   ```

3. **发布**
   - 上传到 GitHub Releases
   - 更新文档和说明

## 技术支持

- 项目仓库: https://github.com/md2docx
- 问题反馈: GitHub Issues
- 开发文档: docs/目录

---

**祝您开发愉快！** 🚀
