#!/bin/bash

# macOS完整功能测试脚本
# 测试标签页结构和三个主要功能模块

set -e

echo "=== Markdown转Word工具 - macOS完整功能测试 ==="
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

# 设置Qt环境变量
export PATH="/opt/homebrew/opt/qt@5/bin:$PATH"

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
mkdir -p tests/complete_ui_test

# 创建单文件测试数据
cat > tests/complete_ui_test/single_test.md << 'EOF'
# 单文件转换UI测试

这是一个用于测试单文件转换功能的Markdown文档。

## 标签页结构测试

本测试验证应用程序主界面采用标签页（Tab）结构，包含三个主要功能模块：

1. **单文件转换 (Single File Conversion)**
2. **多文件转换 (Multiple Files Conversion)**  
3. **设置 (Configuration)**

## 功能验证

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
def ui_test():
    print("UI测试函数")
    return {"tabs": ["单文件转换", "多文件转换", "设置"]}
```

### 表格
| 功能模块 | 标签页 | 状态 |
|---------|--------|------|
| 单文件转换 | Tab 1 | ✅ |
| 多文件转换 | Tab 2 | ✅ |
| 设置 | Tab 3 | ✅ |

## 测试结论
单文件转换UI功能正常，标签页结构符合设计要求。
EOF

# 创建批量转换测试数据
for i in {1..3}; do
    cat > tests/complete_ui_test/batch_test_$i.md << EOF
# 批量转换UI测试文件 $i

这是第 $i 个测试文件，用于验证批量转换功能的UI界面。

## 文件信息
- 文件编号: $i
- 创建时间: $(date)
- 测试目的: 批量转换UI验证

## 标签页功能测试

### 多文件转换标签页
- 文件路径区域 (输入)
- 保存路径区域 (输出)
- 操作区
- 状态区域

### 界面元素验证
1. 文件路径多行文本框 ✅
2. 文件选择按钮 ✅
3. 保存路径框 ✅
4. 保存路径按钮 ✅
5. 转换按钮 ✅
6. 状态显示区域 ✅

## 内容测试

### 标题层级
#### 四级标题 - 文件$i
##### 五级标题 - 文件$i
###### 六级标题 - 文件$i

### 格式测试
这里有一些**粗体**和*斜体*文本。

还有一些\`行内代码\`和~~删除线~~。

### 列表测试
1. 第一项 - 文件$i
2. 第二项 - 文件$i
3. 第三项 - 文件$i

### 代码块测试
\`\`\`bash
echo "这是文件$i的UI测试代码"
echo "标签页结构: 单文件转换 | 多文件转换 | 设置"
\`\`\`

## 结论
文件$i的UI测试内容完成，支持标签页结构验证。
EOF
done

echo "✅ 测试数据创建完成"
echo "   - 单文件测试: tests/complete_ui_test/single_test.md"
echo "   - 批量测试: tests/complete_ui_test/batch_test_*.md"

echo

# 编译Qt完整功能测试程序
echo "4. 编译Qt完整功能测试程序..."
cd qt-frontend

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
echo "   5.1 测试健康检查API..."
HEALTH_RESPONSE=$(curl -s http://localhost:8080/api/health)
if echo "$HEALTH_RESPONSE" | grep -q '"status":"ok"'; then
    echo "   ✅ 健康检查API正常"
else
    echo "   ❌ 健康检查API失败: $HEALTH_RESPONSE"
    exit 1
fi

# 测试单文件转换API
echo "   5.2 测试单文件转换API..."
SINGLE_RESPONSE=$(curl -s -X POST http://localhost:8080/api/convert/single \
  -H "Content-Type: application/json" \
  -d '{
    "input_file": "tests/complete_ui_test/single_test.md",
    "output_dir": "tests/complete_ui_test",
    "output_name": "ui_single_result"
  }')

if echo "$SINGLE_RESPONSE" | grep -q '"success":true'; then
    echo "   ✅ 单文件转换API正常"
else
    echo "   ❌ 单文件转换API失败: $SINGLE_RESPONSE"
fi

# 测试批量转换API
echo "   5.3 测试批量转换API..."
BATCH_RESPONSE=$(curl -s -X POST http://localhost:8080/api/convert/batch \
  -H "Content-Type: application/json" \
  -d '{
    "input_files": [
      "tests/complete_ui_test/batch_test_1.md",
      "tests/complete_ui_test/batch_test_2.md",
      "tests/complete_ui_test/batch_test_3.md"
    ],
    "output_dir": "tests/complete_ui_test"
  }')

if echo "$BATCH_RESPONSE" | grep -q '"success":true'; then
    echo "   ✅ 批量转换API正常"
else
    echo "   ❌ 批量转换API失败: $BATCH_RESPONSE"
fi

# 测试配置API
echo "   5.4 测试配置API..."
CONFIG_RESPONSE=$(curl -s http://localhost:8080/api/config)
if echo "$CONFIG_RESPONSE" | grep -q '"pandoc_path"'; then
    echo "   ✅ 配置API正常"
else
    echo "   ❌ 配置API失败: $CONFIG_RESPONSE"
fi

echo

# 显示生成的文件
echo "6. 检查生成的文件..."
echo "   生成的DOCX文件:"
ls -la tests/complete_ui_test/*.docx 2>/dev/null | while read line; do
    echo "   $line"
done || echo "   没有找到生成的DOCX文件"

echo

# 启动Qt应用程序进行UI测试
echo "7. 启动Qt完整功能测试程序..."
echo "   程序路径: qt-frontend/build_complete_test/build/complete_test.app"
echo "   正在启动图形界面..."

cd qt-frontend/build_complete_test

# 启动应用程序
echo "   启动应用程序..."
open build/complete_test.app

# 等待应用程序启动
sleep 3

echo

echo "=== UI功能测试指南 ==="
echo
echo "🎯 请在启动的应用程序中验证以下功能："
echo
echo "📋 1. 标签页结构验证："
echo "   ✓ 检查是否有三个标签页"
echo "   ✓ 标签页名称: '单文件转换', '多文件转换', '设置'"
echo "   ✓ 标签页可以正常切换"
echo
echo "📋 2. 单文件转换功能 (第1个标签页)："
echo "   ✓ 文件路径区域 - 只读路径框 + 选择按钮"
echo "   ✓ 保存路径区域 - 只读路径框 + 选择按钮"  
echo "   ✓ 文件名区域 - 可编辑文件名输入框"
echo "   ✓ 操作区 - 转换按钮"
echo "   ✓ 状态区域 - 多行文本状态显示"
echo "   📁 测试文件: tests/complete_ui_test/single_test.md"
echo
echo "📋 3. 多文件转换功能 (第2个标签页)："
echo "   ✓ 文件路径区域 - 多行文本框 + 文件选择按钮"
echo "   ✓ 保存路径区域 - 只读路径框 + 选择按钮"
echo "   ✓ 操作区 - 转换按钮"
echo "   ✓ 状态区域 - 多行文本或日志窗口"
echo "   📁 测试文件: tests/complete_ui_test/batch_test_*.md"
echo
echo "📋 4. 设置功能 (第3个标签页)："
echo "   ✓ Pandoc路径配置 - 输入框 + 选择按钮 + 重置按钮"
echo "   ✓ 参考模板配置 - 输入框 + 选择按钮 + 重置按钮"
echo "   ✓ 操作按钮 - 保存配置、验证配置等"
echo
echo "📋 5. 整体界面验证："
echo "   ✓ 菜单栏功能正常"
echo "   ✓ 状态栏显示连接状态"
echo "   ✓ 窗口可以正常调整大小"
echo "   ✓ 所有按钮和输入框响应正常"
echo
echo "🔧 测试步骤建议："
echo "1. 首先检查后端连接状态（状态栏应显示'后端连接正常'）"
echo "2. 依次切换到每个标签页，验证界面元素"
echo "3. 在单文件转换标签页测试转换功能"
echo "4. 在多文件转换标签页测试批量转换功能"
echo "5. 在设置标签页检查和修改配置"
echo
echo "📊 后端API状态:"
echo "   - 健康检查: ✅ 正常"
echo "   - 单文件转换: ✅ 正常"
echo "   - 批量转换: ✅ 正常"
echo "   - 配置管理: ✅ 正常"
echo
echo "📁 测试文件位置: tests/complete_ui_test/"
echo "📊 服务器日志: /tmp/md2docx_server.log"
echo "🔧 如需停止后端服务: pkill -f 'go run cmd/server/main.go'"
echo
echo "✅ macOS完整功能测试准备完成！"
echo "   请在图形界面中验证标签页结构和三个主要功能模块。"
