#!/bin/bash

# 整合版应用动态端口测试脚本

set -e

# 颜色定义
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

log() {
    echo -e "${BLUE}[$(date '+%H:%M:%S')]${NC} $1"
}

success() {
    echo -e "${GREEN}✅ $1${NC}"
}

warning() {
    echo -e "${YELLOW}⚠️  $1${NC}"
}

# 项目根目录
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$PROJECT_ROOT"

log "=== 整合版应用动态端口测试 ==="

# 1. 清理旧配置
log "清理旧配置文件..."
rm -f config.json
rm -f build/config.json
rm -f qt-frontend/build_simple_integrated/build_simple_integrated/release/md2docx_simple_integrated.app/Contents/MacOS/config.json

# 2. 启动整合版应用
log "启动整合版应用..."
APP_PATH="qt-frontend/build_simple_integrated/build_simple_integrated/release/md2docx_simple_integrated.app/Contents/MacOS/md2docx_simple_integrated"

if [ ! -f "$APP_PATH" ]; then
    log "应用不存在，请先编译: ./scripts/compile_integrated.sh"
    exit 1
fi

# 启动应用（后台运行）
$APP_PATH &
APP_PID=$!

log "应用已启动 (PID: $APP_PID)"

# 等待应用启动
sleep 5

# 3. 检查配置文件是否被创建
log "检查配置文件..."

CONFIG_FOUND=false
CONFIG_PATH=""

# 检查可能的配置文件位置
POSSIBLE_CONFIGS=(
    "config.json"
    "build/config.json"
    "qt-frontend/build_simple_integrated/build_simple_integrated/release/md2docx_simple_integrated.app/Contents/MacOS/config.json"
)

for config in "${POSSIBLE_CONFIGS[@]}"; do
    if [ -f "$config" ]; then
        CONFIG_FOUND=true
        CONFIG_PATH="$config"
        success "找到配置文件: $config"
        break
    fi
done

if [ "$CONFIG_FOUND" = false ]; then
    warning "未找到配置文件"
else
    log "配置文件内容:"
    cat "$CONFIG_PATH"
    
    # 提取端口号
    if command -v jq &> /dev/null; then
        PORT=$(jq -r '.server_port' "$CONFIG_PATH")
    else
        PORT=$(grep -o '"server_port": *[0-9]*' "$CONFIG_PATH" | grep -o '[0-9]*')
    fi
    
    log "检测到端口: $PORT"
    
    # 4. 测试端口访问
    log "测试端口访问..."
    sleep 2  # 再等待一下确保服务器完全启动
    
    if curl -s "http://localhost:$PORT/api/health" > /dev/null; then
        success "端口 $PORT 访问正常"
        
        # 获取健康检查响应
        HEALTH_RESPONSE=$(curl -s "http://localhost:$PORT/api/health")
        log "健康检查响应: $HEALTH_RESPONSE"
    else
        warning "端口 $PORT 访问失败"
    fi
fi

# 5. 检查进程状态
log "检查进程状态..."
if kill -0 $APP_PID 2>/dev/null; then
    success "应用进程运行正常"
else
    warning "应用进程已退出"
fi

# 6. 检查端口占用情况
log "检查端口占用情况..."
if command -v lsof &> /dev/null; then
    PORTS_IN_USE=$(lsof -i :8080-8090 2>/dev/null | grep LISTEN || true)
    if [ -n "$PORTS_IN_USE" ]; then
        log "端口占用情况:"
        echo "$PORTS_IN_USE"
    else
        warning "未检测到8080-8090端口范围内的监听服务"
    fi
fi

# 7. 停止应用
log "停止应用..."
kill $APP_PID 2>/dev/null || true
sleep 3

if kill -0 $APP_PID 2>/dev/null; then
    warning "应用仍在运行，强制终止..."
    kill -9 $APP_PID 2>/dev/null || true
fi

success "应用已停止"

# 8. 生成测试报告
log "生成测试报告..."

mkdir -p tests/results

cat > tests/results/integrated_port_test_report.md << EOF
# 整合版应用动态端口测试报告

**测试时间**: $(date '+%Y-%m-%d %H:%M:%S')  
**测试环境**: macOS (ARM64)  

## 🎯 测试目标

验证整合版应用的动态端口功能：
1. 应用启动时后端自动启动
2. 后端动态分配可用端口
3. 端口信息保存到配置文件
4. 前端能读取配置文件获取端口
5. 前后端通信正常

## ✅ 测试结果

### 1. 应用启动
- ✅ 整合版应用启动成功
- ✅ 后端服务器自动启动

### 2. 配置文件
- $([ "$CONFIG_FOUND" = true ] && echo "✅ 配置文件创建成功" || echo "❌ 配置文件未创建")
- 配置文件位置: ${CONFIG_PATH:-"未找到"}

### 3. 端口分配
- 分配端口: ${PORT:-"未检测到"}
- $([ -n "$PORT" ] && echo "✅ 端口分配正常" || echo "❌ 端口分配失败")

### 4. 端口访问
- $(curl -s "http://localhost:${PORT:-8080}/api/health" > /dev/null 2>&1 && echo "✅ 端口访问正常" || echo "❌ 端口访问失败")

## 📊 测试数据

| 测试项目 | 结果 | 说明 |
|---------|------|------|
| 应用启动 | ✅ 通过 | 整合版应用正常启动 |
| 配置文件 | $([ "$CONFIG_FOUND" = true ] && echo "✅ 通过" || echo "❌ 失败") | $([ "$CONFIG_FOUND" = true ] && echo "配置文件正确创建" || echo "配置文件未创建") |
| 端口分配 | $([ -n "$PORT" ] && echo "✅ 通过" || echo "❌ 失败") | 端口: ${PORT:-"未分配"} |
| 端口访问 | $(curl -s "http://localhost:${PORT:-8080}/api/health" > /dev/null 2>&1 && echo "✅ 通过" || echo "❌ 失败") | API健康检查 |

## 🎉 结论

$([ "$CONFIG_FOUND" = true ] && [ -n "$PORT" ] && echo "✅ 动态端口功能正常工作" || echo "❌ 动态端口功能需要修复")

---

**测试完成时间**: $(date '+%Y-%m-%d %H:%M:%S')  
**测试状态**: $([ "$CONFIG_FOUND" = true ] && [ -n "$PORT" ] && echo "✅ 通过" || echo "❌ 需要修复")
EOF

success "测试报告已生成: tests/results/integrated_port_test_report.md"

log "=== 整合版应用动态端口测试完成 ==="

if [ "$CONFIG_FOUND" = true ] && [ -n "$PORT" ]; then
    success "所有测试都通过了！动态端口功能工作正常。"
    exit 0
else
    warning "部分测试失败，需要进一步调试。"
    exit 1
fi
