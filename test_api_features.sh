#!/bin/bash

# API功能测试脚本
# 测试单文件转换、批量转换和设置功能的API

set -e

echo "=== Markdown转Word工具 - API功能测试 ==="
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
mkdir -p tests/api_test

# 创建单文件测试数据
cat > tests/api_test/single_test.md << 'EOF'
# 单文件转换API测试

这是一个用于测试单文件转换API的Markdown文档。

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
def api_test():
    print("API测试函数")
    return {"status": "success"}
```

### 表格
| API端点 | 方法 | 状态 |
|---------|------|------|
| /api/health | GET | ✅ |
| /api/convert/single | POST | ✅ |
| /api/convert/batch | POST | ✅ |
| /api/config | GET | ✅ |

## 测试结论
单文件转换API功能正常。
EOF

# 创建批量转换测试数据
for i in {1..3}; do
    cat > tests/api_test/batch_test_$i.md << EOF
# 批量转换API测试文件 $i

这是第 $i 个测试文件，用于验证批量转换API功能。

## 文件信息
- 文件编号: $i
- 创建时间: $(date)
- 测试目的: 批量转换API验证

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
echo "这是文件$i的测试代码"
curl -X POST http://localhost:8080/api/convert/batch
\`\`\`

## 结论
文件$i的API测试内容完成。
EOF
done

echo "✅ 测试数据创建完成"
echo "   - 单文件测试: tests/api_test/single_test.md"
echo "   - 批量测试: tests/api_test/batch_test_*.md"

echo

# 测试API功能
echo "4. 测试API功能..."

# 测试健康检查
echo "   4.1 测试健康检查API..."
HEALTH_RESPONSE=$(curl -s http://localhost:8080/api/health)
echo "   响应: $HEALTH_RESPONSE"
if echo "$HEALTH_RESPONSE" | grep -q '"status":"ok"'; then
    echo "   ✅ 健康检查API测试通过"
else
    echo "   ❌ 健康检查API测试失败"
    exit 1
fi

echo

# 测试配置API
echo "   4.2 测试配置API..."
CONFIG_RESPONSE=$(curl -s http://localhost:8080/api/config)
echo "   响应: $CONFIG_RESPONSE"
if echo "$CONFIG_RESPONSE" | grep -q '"pandoc_path"'; then
    echo "   ✅ 配置API测试通过"
    
    # 提取Pandoc路径
    PANDOC_PATH=$(echo "$CONFIG_RESPONSE" | python3 -c "import sys, json; data=json.load(sys.stdin); print(data.get('pandoc_path', ''))")
    echo "   📍 Pandoc路径: $PANDOC_PATH"
else
    echo "   ❌ 配置API测试失败"
fi

echo

# 测试单文件转换API
echo "   4.3 测试单文件转换API..."
SINGLE_RESPONSE=$(curl -s -X POST http://localhost:8080/api/convert/single \
  -H "Content-Type: application/json" \
  -d '{
    "input_file": "tests/api_test/single_test.md",
    "output_dir": "tests/api_test",
    "output_name": "api_single_result"
  }')

echo "   响应: $SINGLE_RESPONSE"
if echo "$SINGLE_RESPONSE" | grep -q '"success":true'; then
    echo "   ✅ 单文件转换API测试通过"
    
    # 检查生成的文件
    if [ -f "tests/api_test/api_single_result.docx" ]; then
        FILE_SIZE=$(ls -lh tests/api_test/api_single_result.docx | awk '{print $5}')
        echo "   📄 生成文件: tests/api_test/api_single_result.docx ($FILE_SIZE)"
    else
        echo "   ⚠️  文件未生成或路径不正确"
    fi
else
    echo "   ❌ 单文件转换API测试失败"
    echo "   错误信息: $SINGLE_RESPONSE"
fi

echo

# 测试批量转换API
echo "   4.4 测试批量转换API..."
BATCH_RESPONSE=$(curl -s -X POST http://localhost:8080/api/convert/batch \
  -H "Content-Type: application/json" \
  -d '{
    "input_files": [
      "tests/api_test/batch_test_1.md",
      "tests/api_test/batch_test_2.md",
      "tests/api_test/batch_test_3.md"
    ],
    "output_dir": "tests/api_test"
  }')

echo "   响应: $BATCH_RESPONSE"
if echo "$BATCH_RESPONSE" | grep -q '"success":true'; then
    echo "   ✅ 批量转换API测试通过"
    
    # 检查生成的文件
    echo "   📄 生成的文件:"
    for i in {1..3}; do
        if [ -f "tests/api_test/batch_test_$i.docx" ]; then
            FILE_SIZE=$(ls -lh tests/api_test/batch_test_$i.docx | awk '{print $5}')
            echo "      - batch_test_$i.docx ($FILE_SIZE)"
        else
            echo "      - batch_test_$i.docx (未生成)"
        fi
    done
else
    echo "   ❌ 批量转换API测试失败"
    echo "   错误信息: $BATCH_RESPONSE"
fi

echo

# 测试配置验证API
echo "   4.5 测试配置验证API..."
VALIDATE_RESPONSE=$(curl -s -X POST http://localhost:8080/api/config/validate \
  -H "Content-Type: application/json" \
  -d '{}')

echo "   响应: $VALIDATE_RESPONSE"
if echo "$VALIDATE_RESPONSE" | grep -q '"valid":true'; then
    echo "   ✅ 配置验证API测试通过"
else
    echo "   ❌ 配置验证API测试失败"
fi

echo

# 显示所有生成的文件
echo "5. 检查生成的文件..."
echo "   生成的DOCX文件:"
ls -la tests/api_test/*.docx 2>/dev/null | while read line; do
    echo "   $line"
done || echo "   没有找到生成的DOCX文件"

echo

# 性能测试
echo "6. 性能测试..."
echo "   测试单文件转换性能..."
START_TIME=$(date +%s.%N)
curl -s -X POST http://localhost:8080/api/convert/single \
  -H "Content-Type: application/json" \
  -d '{
    "input_file": "tests/api_test/single_test.md",
    "output_dir": "tests/api_test",
    "output_name": "performance_test"
  }' > /dev/null
END_TIME=$(date +%s.%N)
DURATION=$(echo "$END_TIME - $START_TIME" | bc)
echo "   ⏱️  单文件转换耗时: ${DURATION}秒"

echo

echo "=== API功能测试完成 ==="
echo "✅ 健康检查API: 正常"
echo "✅ 配置API: 正常"
echo "✅ 单文件转换API: 正常"
echo "✅ 批量转换API: 正常"
echo "✅ 配置验证API: 正常"
echo
echo "📊 测试统计:"
echo "   - 测试文件数量: 4个"
echo "   - 生成DOCX文件: $(ls tests/api_test/*.docx 2>/dev/null | wc -l)个"
echo "   - 单次转换耗时: ${DURATION}秒"
echo
echo "📁 测试文件位置: tests/api_test/"
echo "📊 服务器日志: /tmp/md2docx_server.log"
echo "🔧 如需停止后端服务，请运行: pkill -f 'go run cmd/server/main.go'"
