#!/bin/bash

# 最终配置功能测试
echo "=== 最终配置功能测试 ==="

CONFIG_FILE="$HOME/.md2docx/config.json"
SERVER_PORT=$(cat "$CONFIG_FILE" | jq -r '.server_port // 8080')
SERVER_URL="http://localhost:$SERVER_PORT"

echo "服务器地址: $SERVER_URL"

# 测试1: 获取配置
echo ""
echo "测试1: 获取配置"
CONFIG_RESPONSE=$(curl -s "$SERVER_URL/api/config")
echo "$CONFIG_RESPONSE" | jq .

# 测试2: 更新配置（设置模板文件）
echo ""
echo "测试2: 更新配置（设置模板文件）"
UPDATE_RESPONSE=$(curl -s -X POST -H "Content-Type: application/json" \
  -d '{"pandoc_path":"/opt/homebrew/bin/pandoc","template_file":"/tmp/test.docx","server_port":8081}' \
  "$SERVER_URL/api/config")
echo "$UPDATE_RESPONSE" | jq .

# 测试3: 验证配置文件更新
echo ""
echo "测试3: 验证配置文件更新"
cat "$CONFIG_FILE" | jq .

# 测试4: 清空模板文件
echo ""
echo "测试4: 清空模板文件"
CLEAR_RESPONSE=$(curl -s -X POST -H "Content-Type: application/json" \
  -d '{"pandoc_path":"/opt/homebrew/bin/pandoc","template_file":"","server_port":8081}' \
  "$SERVER_URL/api/config")
echo "$CLEAR_RESPONSE" | jq .

# 测试5: 验证清空后的配置
echo ""
echo "测试5: 验证清空后的配置"
cat "$CONFIG_FILE" | jq .

# 测试6: 配置验证
echo ""
echo "测试6: 配置验证"
VALIDATE_RESPONSE=$(curl -s -X POST "$SERVER_URL/api/config/validate")
echo "$VALIDATE_RESPONSE" | jq .

echo ""
echo "=== 测试完成 ==="
