#!/bin/bash

# 配置功能综合测试脚本
# 测试配置的读取、保存、更新、验证功能

echo "=== 配置功能综合测试 ==="

# 配置文件路径
CONFIG_FILE="$HOME/.md2docx/config.json"
BACKUP_FILE="$CONFIG_FILE.backup"

# 备份原配置文件
if [ -f "$CONFIG_FILE" ]; then
    cp "$CONFIG_FILE" "$BACKUP_FILE"
    echo "✅ 已备份原配置文件"
fi

# 从配置文件读取端口
if [ -f "$CONFIG_FILE" ]; then
    SERVER_PORT=$(cat "$CONFIG_FILE" | jq -r '.server_port // 8080')
else
    SERVER_PORT=8080
fi

SERVER_URL="http://localhost:$SERVER_PORT"
echo "服务器地址: $SERVER_URL"

# 测试1: 获取当前配置
echo ""
echo "=== 测试1: 获取当前配置 ==="
CONFIG_RESPONSE=$(curl -s "$SERVER_URL/api/config")
if [ $? -eq 0 ]; then
    echo "✅ 配置获取成功"
    echo "$CONFIG_RESPONSE" | jq .
    
    ORIGINAL_PANDOC=$(echo "$CONFIG_RESPONSE" | jq -r '.pandoc_path')
    ORIGINAL_TEMPLATE=$(echo "$CONFIG_RESPONSE" | jq -r '.template_file')
    echo "原始Pandoc路径: $ORIGINAL_PANDOC"
    echo "原始模板文件: $ORIGINAL_TEMPLATE"
else
    echo "❌ 配置获取失败"
    exit 1
fi

# 测试2: 更新配置
echo ""
echo "=== 测试2: 更新配置 ==="
TEST_TEMPLATE="/tmp/test_template.docx"
UPDATE_DATA="{\"pandoc_path\":\"$ORIGINAL_PANDOC\",\"template_file\":\"$TEST_TEMPLATE\",\"server_port\":$SERVER_PORT}"

UPDATE_RESPONSE=$(curl -s -X POST -H "Content-Type: application/json" -d "$UPDATE_DATA" "$SERVER_URL/api/config")
if [ $? -eq 0 ]; then
    echo "✅ 配置更新请求成功"
    echo "$UPDATE_RESPONSE" | jq .
    
    # 检查更新是否成功
    SUCCESS=$(echo "$UPDATE_RESPONSE" | jq -r '.success')
    if [ "$SUCCESS" = "true" ]; then
        echo "✅ 配置更新成功"
    else
        echo "❌ 配置更新失败"
        echo "$UPDATE_RESPONSE" | jq -r '.error // .message'
    fi
else
    echo "❌ 配置更新请求失败"
fi

# 测试3: 验证配置文件是否更新
echo ""
echo "=== 测试3: 验证配置文件更新 ==="
if [ -f "$CONFIG_FILE" ]; then
    echo "配置文件内容:"
    cat "$CONFIG_FILE" | jq .
    
    SAVED_TEMPLATE=$(cat "$CONFIG_FILE" | jq -r '.template_file')
    if [ "$SAVED_TEMPLATE" = "$TEST_TEMPLATE" ]; then
        echo "✅ 配置文件正确更新"
    else
        echo "❌ 配置文件未正确更新"
        echo "期望: $TEST_TEMPLATE"
        echo "实际: $SAVED_TEMPLATE"
    fi
else
    echo "❌ 配置文件不存在"
fi

# 测试4: 验证配置
echo ""
echo "=== 测试4: 验证配置 ==="
VALIDATE_RESPONSE=$(curl -s -X POST "$SERVER_URL/api/config/validate")
if [ $? -eq 0 ]; then
    echo "✅ 配置验证请求成功"
    echo "$VALIDATE_RESPONSE" | jq .
    
    SUCCESS=$(echo "$VALIDATE_RESPONSE" | jq -r '.success')
    MESSAGE=$(echo "$VALIDATE_RESPONSE" | jq -r '.message')
    
    echo "验证结果: $SUCCESS"
    echo "验证信息:"
    echo "$MESSAGE"
else
    echo "❌ 配置验证请求失败"
fi

# 测试5: 恢复原配置
echo ""
echo "=== 测试5: 恢复原配置 ==="
RESTORE_DATA="{\"pandoc_path\":\"$ORIGINAL_PANDOC\",\"template_file\":\"$ORIGINAL_TEMPLATE\",\"server_port\":$SERVER_PORT}"

RESTORE_RESPONSE=$(curl -s -X POST -H "Content-Type: application/json" -d "$RESTORE_DATA" "$SERVER_URL/api/config")
if [ $? -eq 0 ]; then
    echo "✅ 配置恢复成功"
    echo "$RESTORE_RESPONSE" | jq .
else
    echo "❌ 配置恢复失败"
fi

# 清理备份文件
if [ -f "$BACKUP_FILE" ]; then
    rm "$BACKUP_FILE"
    echo "✅ 已清理备份文件"
fi

echo ""
echo "=== 综合测试完成 ==="
