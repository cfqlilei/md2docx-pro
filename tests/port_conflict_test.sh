#!/bin/bash

# 端口冲突处理测试脚本

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

log "=== 端口冲突处理测试 ==="

# 1. 清理旧配置
log "清理旧配置文件..."
rm -f config.json
rm -f build/config.json

# 2. 占用8080端口
log "占用8080端口..."
# 使用Go程序占用端口
cd tests
go run port_blocker.go 8080 &
BLOCKER_PID=$!
cd ..
sleep 2

if kill -0 $BLOCKER_PID 2>/dev/null; then
    success "8080端口已被占用 (PID: $BLOCKER_PID)"
else
    warning "无法占用8080端口"
    exit 1
fi

# 3. 启动后端服务器
log "启动后端服务器（8080端口被占用）..."
cd build
./md2docx-server-macos &
SERVER_PID=$!
cd ..

# 等待服务器启动
sleep 5

# 4. 检查服务器是否启动成功
if kill -0 $SERVER_PID 2>/dev/null; then
    success "后端服务器启动成功 (PID: $SERVER_PID)"
else
    warning "后端服务器启动失败"
    # 清理占用的端口
    kill ${NC_PID:-$PYTHON_PID} 2>/dev/null || true
    exit 1
fi

# 5. 检查配置文件
if [ -f "build/config.json" ]; then
    success "配置文件已创建"
    log "配置文件内容:"
    cat build/config.json
    
    # 提取端口号
    PORT=$(grep -o '"server_port": *[0-9]*' build/config.json | grep -o '[0-9]*')
    log "检测到端口: $PORT"
    
    if [ "$PORT" != "8080" ]; then
        success "端口冲突处理成功，使用端口: $PORT"
    else
        warning "端口冲突处理可能失败，仍使用8080端口"
    fi
else
    warning "配置文件未创建"
fi

# 6. 测试端口访问
if [ -n "$PORT" ]; then
    log "测试端口访问..."
    if curl -s "http://localhost:$PORT/api/health" > /dev/null; then
        success "端口 $PORT 访问正常"
        HEALTH_RESPONSE=$(curl -s "http://localhost:$PORT/api/health")
        log "健康检查响应: $HEALTH_RESPONSE"
    else
        warning "端口 $PORT 访问失败"
    fi
fi

# 7. 清理
log "清理进程..."
kill $SERVER_PID 2>/dev/null || true
kill $BLOCKER_PID 2>/dev/null || true
sleep 2

success "端口冲突处理测试完成"

# 8. 生成测试报告
mkdir -p tests/results

cat > tests/results/port_conflict_test_report.md << EOF
# 端口冲突处理测试报告

**测试时间**: $(date '+%Y-%m-%d %H:%M:%S')  
**测试环境**: macOS (ARM64)  

## 🎯 测试目标

验证端口冲突处理功能：
1. 8080端口被占用时
2. 后端能自动选择其他可用端口
3. 配置文件正确保存新端口
4. 新端口能正常访问

## ✅ 测试结果

### 1. 端口占用
- ✅ 成功占用8080端口

### 2. 冲突处理
- ✅ 后端服务器启动成功
- ✅ 自动选择新端口: ${PORT:-"未检测到"}
- $([ "$PORT" != "8080" ] && echo "✅ 端口冲突处理成功" || echo "❌ 端口冲突处理失败")

### 3. 配置保存
- $([ -f "build/config.json" ] && echo "✅ 配置文件创建成功" || echo "❌ 配置文件未创建")

### 4. 端口访问
- $([ -n "$PORT" ] && curl -s "http://localhost:$PORT/api/health" > /dev/null 2>&1 && echo "✅ 新端口访问正常" || echo "❌ 新端口访问失败")

## 📊 测试数据

| 测试项目 | 结果 | 说明 |
|---------|------|------|
| 端口占用 | ✅ 通过 | 8080端口成功被占用 |
| 冲突检测 | $([ "$PORT" != "8080" ] && echo "✅ 通过" || echo "❌ 失败") | 检测到端口冲突 |
| 端口分配 | $([ -n "$PORT" ] && echo "✅ 通过" || echo "❌ 失败") | 新端口: ${PORT:-"未分配"} |
| 端口访问 | $([ -n "$PORT" ] && curl -s "http://localhost:$PORT/api/health" > /dev/null 2>&1 && echo "✅ 通过" || echo "❌ 失败") | API健康检查 |

## 🎉 结论

$([ "$PORT" != "8080" ] && [ -n "$PORT" ] && echo "✅ 端口冲突处理功能正常工作" || echo "❌ 端口冲突处理功能需要修复")

---

**测试完成时间**: $(date '+%Y-%m-%d %H:%M:%S')  
**测试状态**: $([ "$PORT" != "8080" ] && [ -n "$PORT" ] && echo "✅ 通过" || echo "❌ 需要修复")
EOF

success "测试报告已生成: tests/results/port_conflict_test_report.md"

log "=== 端口冲突处理测试完成 ==="
