#!/bin/bash

# 整合版应用测试脚本
# 测试整合版应用的配置功能和基本功能

echo "=== 整合版应用测试 ==="

# 获取版本信息
VERSION=$(cat VERSION 2>/dev/null || echo "1.0.0")
APP_PATH="build/release/md2docx_simple_integrated-v${VERSION}.app"

# 检查应用是否存在
if [ ! -d "$APP_PATH" ]; then
    echo "❌ 整合版应用不存在: $APP_PATH"
    echo "请先运行构建脚本: ./scripts/all_in_one_integrated.sh"
    exit 1
fi

echo "✅ 找到整合版应用: $APP_PATH"

# 检查应用包完整性
FRONTEND_EXEC="$APP_PATH/Contents/MacOS/md2docx_simple_integrated"
BACKEND_EXEC="$APP_PATH/Contents/MacOS/md2docx-server-macos"

if [ ! -f "$FRONTEND_EXEC" ]; then
    echo "❌ 前端可执行文件不存在"
    exit 1
fi

if [ ! -f "$BACKEND_EXEC" ]; then
    echo "❌ 后端可执行文件不存在"
    exit 1
fi

echo "✅ 应用包完整性检查通过"

# 测试后端服务器功能
echo ""
echo "=== 测试后端服务器功能 ==="

# 启动后端服务器进行测试
echo "启动后端服务器进行测试..."
"$BACKEND_EXEC" &
BACKEND_PID=$!

# 等待服务器启动
sleep 3

# 查找服务器端口
SERVER_PORT=""
for port in {8080..8090}; do
    if curl -s "http://localhost:$port/api/health" >/dev/null 2>&1; then
        SERVER_PORT=$port
        break
    fi
done

if [ -z "$SERVER_PORT" ]; then
    echo "❌ 后端服务器未启动或端口不可用"
    kill $BACKEND_PID 2>/dev/null
    exit 1
fi

echo "✅ 后端服务器运行在端口: $SERVER_PORT"

# 测试健康检查
echo "测试健康检查..."
HEALTH_RESPONSE=$(curl -s "http://localhost:$SERVER_PORT/api/health")
if echo "$HEALTH_RESPONSE" | grep -q '"status":"ok"'; then
    echo "✅ 健康检查通过"
else
    echo "❌ 健康检查失败"
    echo "响应: $HEALTH_RESPONSE"
fi

# 测试配置获取
echo "测试配置获取..."
CONFIG_RESPONSE=$(curl -s "http://localhost:$SERVER_PORT/api/config")
if echo "$CONFIG_RESPONSE" | grep -q '"success":true'; then
    echo "✅ 配置获取成功"
    
    # 解析配置
    PANDOC_PATH=$(echo "$CONFIG_RESPONSE" | jq -r '.pandoc_path // ""')
    if [ -n "$PANDOC_PATH" ] && [ -f "$PANDOC_PATH" ]; then
        echo "✅ Pandoc路径有效: $PANDOC_PATH"
    else
        echo "⚠️  Pandoc路径可能无效: $PANDOC_PATH"
    fi
else
    echo "❌ 配置获取失败"
    echo "响应: $CONFIG_RESPONSE"
fi

# 测试配置更新
echo "测试配置更新..."
UPDATE_DATA='{"pandoc_path":"'$PANDOC_PATH'","template_file":"","server_port":'$SERVER_PORT'}'
UPDATE_RESPONSE=$(curl -s -X POST -H "Content-Type: application/json" -d "$UPDATE_DATA" "http://localhost:$SERVER_PORT/api/config")
if echo "$UPDATE_RESPONSE" | grep -q '"success":true'; then
    echo "✅ 配置更新成功"
else
    echo "❌ 配置更新失败"
    echo "响应: $UPDATE_RESPONSE"
fi

# 停止后端服务器
kill $BACKEND_PID 2>/dev/null
wait $BACKEND_PID 2>/dev/null

echo "✅ 后端服务器已停止"

# 检查配置文件
echo ""
echo "=== 检查配置文件 ==="
CONFIG_FILE="$HOME/.md2docx/config.json"
if [ -f "$CONFIG_FILE" ]; then
    echo "✅ 配置文件存在: $CONFIG_FILE"
    echo "配置文件内容:"
    cat "$CONFIG_FILE" | jq . 2>/dev/null || cat "$CONFIG_FILE"
else
    echo "⚠️  配置文件不存在: $CONFIG_FILE"
fi

# 显示应用信息
echo ""
echo "=== 应用信息 ==="
echo "应用路径: $APP_PATH"
echo "应用大小: $(du -sh "$APP_PATH" | cut -f1)"
echo "前端大小: $(du -sh "$FRONTEND_EXEC" | cut -f1)"
echo "后端大小: $(du -sh "$BACKEND_EXEC" | cut -f1)"

echo ""
echo "=== 测试完成 ==="
echo "✅ 整合版应用构建和基本功能测试通过"
echo ""
echo "启动应用:"
echo "  方式1: 双击应用包 $APP_PATH"
echo "  方式2: 运行 ./launch_integrated.sh"
echo "  方式3: 运行 open '$APP_PATH'"
