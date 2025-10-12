#!/bin/bash

# 单文件转换功能测试脚本

echo "=== 单文件转换功能测试 ==="
echo "测试时间: $(date '+%Y-%m-%d %H:%M:%S')"
echo "================================"

# 检查环境
echo "1. 检查环境依赖..."

# 检查Go
if ! command -v go >/dev/null 2>&1; then
    echo "❌ 错误: 未找到Go，请安装Go语言环境"
    exit 1
fi
echo "✅ Go版本: $(go version)"

# 检查Pandoc
if ! command -v pandoc >/dev/null 2>&1; then
    echo "❌ 错误: 未找到Pandoc，请安装Pandoc"
    exit 1
fi
echo "✅ Pandoc版本: $(pandoc --version | head -n 1)"

# 检查Qt
if ! command -v qmake >/dev/null 2>&1; then
    echo "❌ 错误: 未找到qmake，请安装Qt开发环境"
    exit 1
fi
echo "✅ Qt环境: $(qmake -version | head -n 1)"

# 设置Qt环境变量
export PATH="/opt/homebrew/opt/qt@5/bin:$PATH"

echo
echo "2. 启动Go后端服务..."

# 检查后端是否已经在运行
if curl -s http://localhost:8080/api/health >/dev/null 2>&1; then
    echo "✅ 后端服务已在运行"
    BACKEND_RUNNING=true
else
    echo "启动Go后端服务..."
    go run cmd/server/main.go &
    BACKEND_PID=$!
    echo "后端服务PID: $BACKEND_PID"
    BACKEND_RUNNING=false
    
    # 等待后端启动
    echo "等待后端服务启动..."
    sleep 3
    
    # 检查后端是否启动成功
    if curl -s http://localhost:8080/api/health >/dev/null 2>&1; then
        echo "✅ 后端服务启动成功"
    else
        echo "❌ 后端服务启动失败"
        if [ ! -z "$BACKEND_PID" ]; then
            kill $BACKEND_PID 2>/dev/null
        fi
        exit 1
    fi
fi

echo
echo "3. 构建Qt单文件转换测试程序..."

cd qt-frontend

# 清理之前的构建
rm -rf build_single_test
mkdir -p build_single_test
cd build_single_test

# 生成Makefile
qmake ../single_test.pro
if [ $? -ne 0 ]; then
    echo "❌ qmake失败"
    exit 1
fi

# 编译
make
if [ $? -ne 0 ]; then
    echo "❌ 编译失败"
    exit 1
fi

echo "✅ Qt程序编译成功"

echo
echo "4. 创建测试数据..."

cd ../..

# 创建测试目录
mkdir -p tests/single_conversion
cd tests/single_conversion

# 创建测试Markdown文件
cat > test_single.md << 'EOF'
# 单文件转换测试

这是一个用于测试单文件转换功能的Markdown文档。

## 功能特性

- **单文件转换**: 将单个Markdown文件转换为Word文档
- **路径配置**: 支持自定义输出路径和文件名
- **模板支持**: 可选择Word模板文件
- **状态反馈**: 实时显示转换状态和结果

## 测试内容

### 文本格式

这里有一些**粗体文本**和*斜体文本*。

### 列表

1. 第一项
2. 第二项
3. 第三项

- 无序列表项1
- 无序列表项2
- 无序列表项3

### 代码块

```python
def hello_world():
    print("Hello, World!")
    return True
```

### 表格

| 功能 | 状态 | 说明 |
|------|------|------|
| 单文件转换 | ✅ | 已实现 |
| 批量转换 | 🔧 | 开发中 |
| 配置管理 | ✅ | 已实现 |

## 结论

这个测试文档包含了各种Markdown元素，用于验证转换功能的完整性。
EOF

echo "✅ 测试数据创建完成: tests/single_conversion/test_single.md"

echo
echo "5. 启动Qt测试程序..."

cd ../../qt-frontend/build_single_test

# 启动Qt应用程序
echo "正在启动Qt单文件转换测试程序..."
echo
echo "================================"
echo "测试程序已启动！"
echo
echo "使用说明:"
echo "1. 在Qt界面中选择输入文件: tests/single_conversion/test_single.md"
echo "2. 设置输出目录（可选）"
echo "3. 设置输出文件名（可选）"
echo "4. 点击'开始转换'按钮"
echo "5. 查看状态信息和转换结果"
echo
echo "后端API地址: http://localhost:8080"
echo "测试文件位置: $(pwd)/../../tests/single_conversion/test_single.md"
echo "================================"

# 启动Qt应用程序
if [ -f "single_test.app/Contents/MacOS/single_test" ]; then
    open single_test.app
elif [ -f "single_test" ]; then
    ./single_test
else
    echo "❌ 未找到可执行文件"
    exit 1
fi

# 等待用户测试
echo
echo "按任意键结束测试..."
read -n 1

# 清理
echo
echo "6. 清理测试环境..."

if [ "$BACKEND_RUNNING" = false ] && [ ! -z "$BACKEND_PID" ]; then
    echo "停止后端服务..."
    kill $BACKEND_PID 2>/dev/null
    echo "✅ 后端服务已停止"
fi

echo
echo "================================"
echo "单文件转换功能测试完成"
echo "结束时间: $(date '+%Y-%m-%d %H:%M:%S')"
echo "================================"
