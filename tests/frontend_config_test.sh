#!/bin/bash

# 前端配置测试脚本
# 测试前端是否能正确获取后端配置

echo "=== 前端配置获取测试 ==="

# 检查配置文件是否存在
CONFIG_FILE="$HOME/.md2docx/config.json"
if [ -f "$CONFIG_FILE" ]; then
    echo "✅ 配置文件存在: $CONFIG_FILE"
    echo "配置文件内容:"
    cat "$CONFIG_FILE" | jq .
    
    # 从配置文件读取端口
    SERVER_PORT=$(cat "$CONFIG_FILE" | jq -r '.server_port // 8080')
    echo "从配置文件读取的端口: $SERVER_PORT"
else
    echo "❌ 配置文件不存在: $CONFIG_FILE"
    SERVER_PORT=8080
    echo "使用默认端口: $SERVER_PORT"
fi

# 测试服务器连接
echo ""
echo "=== 测试服务器连接 ==="
SERVER_URL="http://localhost:$SERVER_PORT"
echo "服务器地址: $SERVER_URL"

# 健康检查
echo "进行健康检查..."
HEALTH_RESPONSE=$(curl -s "$SERVER_URL/api/health")
if [ $? -eq 0 ]; then
    echo "✅ 服务器连接成功"
    echo "健康检查响应:"
    echo "$HEALTH_RESPONSE" | jq .
else
    echo "❌ 服务器连接失败"
    exit 1
fi

# 获取配置
echo ""
echo "=== 获取配置测试 ==="
CONFIG_RESPONSE=$(curl -s "$SERVER_URL/api/config")
if [ $? -eq 0 ]; then
    echo "✅ 配置获取成功"
    echo "配置响应:"
    echo "$CONFIG_RESPONSE" | jq .
    
    # 解析配置
    PANDOC_PATH=$(echo "$CONFIG_RESPONSE" | jq -r '.pandoc_path // ""')
    TEMPLATE_FILE=$(echo "$CONFIG_RESPONSE" | jq -r '.template_file // ""')
    
    echo ""
    echo "=== 配置解析结果 ==="
    echo "Pandoc路径: $PANDOC_PATH"
    echo "模板文件: $TEMPLATE_FILE"
    
    # 验证Pandoc路径
    if [ -n "$PANDOC_PATH" ] && [ -f "$PANDOC_PATH" ]; then
        echo "✅ Pandoc路径有效"
        echo "Pandoc版本信息:"
        "$PANDOC_PATH" --version | head -1
    else
        echo "❌ Pandoc路径无效或为空"
    fi
    
else
    echo "❌ 配置获取失败"
    exit 1
fi

echo ""
echo "=== 测试完成 ==="
