# 快速参考指南 - 目录结构规范化

## 📁 新的目录结构

```
md2docx-pro/
├── 源代码/
│   ├── cmd/                    # Go 命令行程序
│   ├── internal/               # Go 内部包
│   ├── pkg/                    # Go 公共包
│   ├── qt-frontend/            # Qt 前端源代码
│   └── web/                    # Web 资源
│
├── build/                      # 编译产物和中间文件
│   ├── bin/                    # ✅ 最终的二进制文件
│   │   ├── md2docx-server      # macOS 后端
│   │   ├── md2docx-server.exe  # Windows 后端
│   │   ├── md2docx_simple_integrated.app/  # macOS 应用
│   │   └── md2docx_simple_integrated.exe   # Windows 应用
│   │
│   └── intermediate/           # ✅ 中间文件
│       ├── go/                 # Go 编译中间文件
│       └── qt/                 # Qt 编译中间文件
│
├── tests/                      # 测试相关
│   ├── unit/                   # 单元测试
│   ├── integration/            # 集成测试
│   ├── e2e/                    # 端到端测试
│   ├── testdata/               # 测试数据
│   ├── results/                # 测试结果
│   └── utils/                  # 测试工具
│
├── scripts/                    # 构建脚本
│   ├── compile_integrated.sh
│   ├── compile_integrated_windows.bat
│   ├── clean_integrated.sh
│   ├── clean_integrated_windows.bat
│   ├── verify_directory_structure.sh
│   └── verify_directory_structure.bat
│
└── docs/                       # 文档
```

## 🔧 常用命令

### Mac

```bash
# 清理所有构建文件
./scripts/clean_integrated.sh

# 编译项目
./scripts/compile_integrated.sh

# 验证目录结构
./scripts/verify_directory_structure.sh

# 一次性完整构建
./scripts/all_in_one_integrated.sh
```

### Windows

```bash
# 清理所有构建文件
scripts\clean_integrated_windows.bat

# 编译项目
scripts\compile_integrated_windows.bat

# 验证目录结构
scripts\verify_directory_structure.bat

# 一次性完整构建
scripts\all_in_one_integrated_windows.bat
```

## 📝 关键变更

### Qt 项目文件 (.pro)

**原来：**
```qmake
DESTDIR = build_simple_integrated/release
OBJECTS_DIR = build_simple_integrated/release/obj
```

**现在：**
```qmake
DESTDIR = $$PWD/../build/bin
OBJECTS_DIR = $$PWD/../build/intermediate/qt/simple_integrated/release/obj
MOC_DIR = $$PWD/../build/intermediate/qt/simple_integrated/release/moc
RCC_DIR = $$PWD/../build/intermediate/qt/simple_integrated/release/rcc
UI_DIR = $$PWD/../build/intermediate/qt/simple_integrated/release/ui
```

### 后端文件路径

**原来：**
```
build/release/md2docx-server-macos
build/release/md2docx-server-windows.exe
```

**现在：**
```
build/bin/md2docx-server
build/bin/md2docx-server.exe
```

## ✅ 修改的文件清单

### Qt 项目文件（8 个）
- [ ] qt-frontend/md2docx_simple_integrated.pro
- [ ] qt-frontend/md2docx_integrated.pro
- [ ] qt-frontend/md2docx_app.pro
- [ ] qt-frontend/md2docx_simple.pro
- [ ] qt-frontend/md2docx.pro
- [ ] qt-frontend/simple_complete.pro
- [ ] qt-frontend/complete_test.pro
- [ ] qt-frontend/single_test.pro

### 构建脚本（4 个）
- [ ] scripts/compile_integrated.sh
- [ ] scripts/compile_integrated_windows.bat
- [ ] scripts/clean_integrated.sh
- [ ] scripts/clean_integrated_windows.bat

### 新增文件（4 个）
- [ ] scripts/verify_directory_structure.sh
- [ ] scripts/verify_directory_structure.bat
- [ ] docs/目录规范化总结.md
- [ ] DIRECTORY_STRUCTURE_REFACTORING_REPORT.md

## 🎯 目录用途说明

| 目录 | 用途 | 说明 |
|------|------|------|
| `build/bin/` | 最终的二进制文件 | 存放所有编译后的可执行文件和应用包 |
| `build/intermediate/go/` | Go 编译中间文件 | 存放 Go 编译过程中的临时文件 |
| `build/intermediate/qt/` | Qt 编译中间文件 | 按项目名称分类存放 Qt 编译中间文件 |
| `tests/unit/` | 单元测试 | 存放单元测试用例 |
| `tests/integration/` | 集成测试 | 存放集成测试用例 |
| `tests/e2e/` | 端到端测试 | 存放端到端测试用例 |
| `tests/testdata/` | 测试数据 | 存放测试所需的数据文件 |
| `tests/results/` | 测试结果 | 存放测试执行结果 |

## 🚀 快速开始

### 第一次构建

```bash
# Mac
./scripts/clean_integrated.sh
./scripts/compile_integrated.sh

# Windows
scripts\clean_integrated_windows.bat
scripts\compile_integrated_windows.bat
```

### 验证构建结果

```bash
# Mac
ls -la build/bin/
ls -la build/intermediate/qt/

# Windows
dir build\bin\
dir build\intermediate\qt\
```

### 清理并重新构建

```bash
# Mac
./scripts/clean_integrated.sh
./scripts/compile_integrated.sh

# Windows
scripts\clean_integrated_windows.bat
scripts\compile_integrated_windows.bat
```

## 📚 详细文档

- 详细的规范化说明：`docs/目录规范化总结.md`
- 完整的重构报告：`DIRECTORY_STRUCTURE_REFACTORING_REPORT.md`

## ⚠️ 注意事项

1. **不要手动修改 build 目录** - build 目录由构建脚本自动管理
2. **定期清理** - 使用 clean 脚本定期清理构建文件
3. **验证结构** - 使用 verify 脚本验证目录结构的一致性
4. **更新 .gitignore** - 确保 build 目录被正确忽略

## 🔍 故障排除

### 问题：找不到编译后的文件

**解决方案：**
```bash
# 检查 build/bin 目录
ls -la build/bin/

# 运行验证脚本
./scripts/verify_directory_structure.sh
```

### 问题：编译失败

**解决方案：**
```bash
# 清理所有构建文件
./scripts/clean_integrated.sh

# 重新编译
./scripts/compile_integrated.sh
```

### 问题：Qt 编译中间文件混乱

**解决方案：**
```bash
# 清理 build/intermediate 目录
rm -rf build/intermediate

# 重新编译
./scripts/compile_integrated.sh
```

## 📞 获取帮助

- 查看详细文档：`docs/目录规范化总结.md`
- 查看完整报告：`DIRECTORY_STRUCTURE_REFACTORING_REPORT.md`
- 运行验证脚本：`./scripts/verify_directory_structure.sh`

