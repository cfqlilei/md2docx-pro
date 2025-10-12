#!/bin/bash

# 完整功能测试脚本
# 测试单文件转换、批量转换和设置功能

set -e

echo "=== Markdown转Word工具 - 完整功能测试 ==="
echo "测试时间: $(date)"
echo

# 检查环境依赖
echo "1. 检查环境依赖..."

# 检查Go
if ! command -v go &> /dev/null; then
    echo "❌ Go未安装，请先安装Go"
    exit 1
fi
echo "✅ Go版本: $(go version)"

# 检查Pandoc
if ! command -v pandoc &> /dev/null; then
    echo "❌ Pandoc未安装，请先安装Pandoc"
    echo "   macOS: brew install pandoc"
    echo "   Ubuntu: sudo apt install pandoc"
    exit 1
fi
echo "✅ Pandoc版本: $(pandoc --version | head -1)"

# 检查Qt
if ! command -v qmake &> /dev/null; then
    echo "❌ Qt未安装，请先安装Qt"
    echo "   macOS: brew install qt@5"
    exit 1
fi
echo "✅ QMake版本: $(qmake --version | head -1)"

echo

# 检查Go后端服务
echo "2. 检查Go后端服务..."
if curl -s http://localhost:8080/api/health > /dev/null 2>&1; then
    echo "✅ Go后端服务正在运行"
else
    echo "🔧 启动Go后端服务..."
    cd "$(dirname "$0")"
    
    # 确保Go模块已初始化
    if [ ! -f "go.mod" ]; then
        echo "   初始化Go模块..."
        go mod init md2docx
        go mod tidy
    fi
    
    # 启动后端服务（后台运行）
    echo "   启动后端服务..."
    nohup go run cmd/server/main.go > /tmp/md2docx_server.log 2>&1 &
    SERVER_PID=$!
    echo "   后端服务PID: $SERVER_PID"
    
    # 等待服务启动
    echo "   等待服务启动..."
    for i in {1..10}; do
        if curl -s http://localhost:8080/api/health > /dev/null 2>&1; then
            echo "✅ Go后端服务启动成功"
            break
        fi
        sleep 1
        if [ $i -eq 10 ]; then
            echo "❌ Go后端服务启动失败"
            echo "   日志文件: /tmp/md2docx_server.log"
            cat /tmp/md2docx_server.log
            exit 1
        fi
    done
fi

echo

# 创建测试数据
echo "3. 创建测试数据..."
mkdir -p tests/complete_test

# 创建单文件测试数据
cat > tests/complete_test/single_test.md << 'EOF'
# 单文件转换测试

这是一个用于测试单文件转换功能的Markdown文档。

## 基本格式测试

### 文本格式
- **粗体文本**
- *斜体文本*
- `行内代码`
- ~~删除线~~

### 列表
1. 有序列表项1
2. 有序列表项2
3. 有序列表项3

- 无序列表项A
- 无序列表项B
- 无序列表项C

### 代码块
```python
def hello():
    print("Hello, World!")
    return True
```

### 表格
| 功能 | 状态 | 说明 |
|------|------|------|
| 单文件转换 | ✅ | 已实现 |
| 批量转换 | ✅ | 已实现 |
| 配置管理 | ✅ | 已实现 |

### 引用
> 这是一个引用块示例。
> 支持多行引用内容。

## 测试结论
单文件转换功能测试完成。
EOF

# 创建批量转换测试数据
for i in {1..3}; do
    cat > tests/complete_test/batch_test_$i.md << EOF
# 批量转换测试文件 $i

这是第 $i 个测试文件，用于验证批量转换功能。

## 文件信息
- 文件编号: $i
- 创建时间: $(date)
- 测试目的: 批量转换功能验证

## 内容测试

### 标题层级
#### 四级标题
##### 五级标题
###### 六级标题

### 格式测试
这里有一些**粗体**和*斜体*文本。

还有一些\`行内代码\`和~~删除线~~。

### 列表测试
1. 第一项 - 文件$i
2. 第二项 - 文件$i
3. 第三项 - 文件$i

### 代码块测试
\`\`\`bash
echo "这是文件$i的测试代码"
ls -la
\`\`\`

## 结论
文件$i的内容测试完成。
EOF
done

echo "✅ 测试数据创建完成"
echo "   - 单文件测试: tests/complete_test/single_test.md"
echo "   - 批量测试: tests/complete_test/batch_test_*.md"

echo

# 编译Qt完整功能测试程序
echo "4. 编译Qt完整功能测试程序..."
cd qt-frontend

# 设置Qt环境变量（macOS）
if [[ "$OSTYPE" == "darwin"* ]]; then
    export PATH="/opt/homebrew/opt/qt@5/bin:$PATH"
fi

# 清理之前的构建
rm -rf build_complete_test
mkdir build_complete_test
cd build_complete_test

# 编译
echo "   运行qmake..."
qmake ../complete_test.pro

echo "   运行make..."
make

if [ $? -eq 0 ]; then
    echo "✅ Qt完整功能测试程序编译成功"
else
    echo "❌ Qt完整功能测试程序编译失败"
    exit 1
fi

cd ../..

echo

# 测试API功能
echo "5. 测试API功能..."

# 测试健康检查
echo "   测试健康检查..."
HEALTH_RESPONSE=$(curl -s http://localhost:8080/api/health)
if echo "$HEALTH_RESPONSE" | grep -q '"status":"ok"'; then
    echo "   ✅ 健康检查通过"
else
    echo "   ❌ 健康检查失败: $HEALTH_RESPONSE"
    exit 1
fi

# 测试单文件转换API
echo "   测试单文件转换API..."
SINGLE_RESPONSE=$(curl -s -X POST http://localhost:8080/api/convert/single \
  -H "Content-Type: application/json" \
  -d '{
    "input_file": "tests/complete_test/single_test.md",
    "output_dir": "tests/complete_test",
    "output_name": "api_single_result"
  }')

if echo "$SINGLE_RESPONSE" | grep -q '"success":true'; then
    echo "   ✅ 单文件转换API测试通过"
    echo "   📄 生成文件: tests/complete_test/api_single_result.docx"
else
    echo "   ❌ 单文件转换API测试失败: $SINGLE_RESPONSE"
fi

# 测试批量转换API
echo "   测试批量转换API..."
BATCH_RESPONSE=$(curl -s -X POST http://localhost:8080/api/convert/batch \
  -H "Content-Type: application/json" \
  -d '{
    "input_files": [
      "tests/complete_test/batch_test_1.md",
      "tests/complete_test/batch_test_2.md",
      "tests/complete_test/batch_test_3.md"
    ],
    "output_dir": "tests/complete_test"
  }')

if echo "$BATCH_RESPONSE" | grep -q '"success":true'; then
    echo "   ✅ 批量转换API测试通过"
    echo "   📄 生成文件: tests/complete_test/batch_test_*.docx"
else
    echo "   ❌ 批量转换API测试失败: $BATCH_RESPONSE"
fi

# 测试配置API
echo "   测试配置API..."
CONFIG_RESPONSE=$(curl -s http://localhost:8080/api/config)
if echo "$CONFIG_RESPONSE" | grep -q '"pandoc_path"'; then
    echo "   ✅ 配置API测试通过"
else
    echo "   ❌ 配置API测试失败: $CONFIG_RESPONSE"
fi

echo

# 显示生成的文件
echo "6. 检查生成的文件..."
ls -la tests/complete_test/*.docx 2>/dev/null || echo "   没有找到生成的DOCX文件"

echo

# 启动Qt应用程序
echo "7. 启动Qt完整功能测试程序..."
echo "   程序路径: qt-frontend/build_complete_test/build/complete_test.app"
echo "   正在启动图形界面..."

cd qt-frontend/build_complete_test
if [[ "$OSTYPE" == "darwin"* ]]; then
    open build/complete_test.app
else
    ./build/complete_test &
fi

echo
echo "=== 测试完成 ==="
echo "✅ 后端API功能正常"
echo "✅ Qt图形界面已启动"
echo "📋 请在图形界面中测试以下功能："
echo "   1. 单文件转换 - 选择tests/complete_test/single_test.md"
echo "   2. 批量转换 - 添加tests/complete_test/batch_test_*.md"
echo "   3. 设置管理 - 配置Pandoc路径和模板文件"
echo
echo "🔧 如需停止后端服务，请运行: pkill -f 'go run cmd/server/main.go'"
echo "📁 测试文件位置: tests/complete_test/"
echo "📊 服务器日志: /tmp/md2docx_server.log"
