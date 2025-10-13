#!/bin/bash

# 配置验证功能改进测试
# 测试设置页面的配置验证是否提供详细的验证结果反馈

set -e

# 测试配置
TEST_NAME="配置验证功能改进测试"
TEST_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$TEST_DIR/../.." && pwd)"
RESULTS_DIR="$PROJECT_ROOT/tests/results"

# 创建结果目录
mkdir -p "$RESULTS_DIR"

# 日志函数
log() {
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1" | tee -a "$RESULTS_DIR/config_validation_test.log"
}

log "开始执行 $TEST_NAME"

# 检查后端API改进
log "检查后端配置验证API改进..."

HANDLERS_GO="$PROJECT_ROOT/internal/api/handlers.go"

if [ ! -f "$HANDLERS_GO" ]; then
    log "❌ 未找到handlers.go文件"
    exit 1
fi

# 检查详细验证逻辑
if grep -q "validationMessages.*\[\]string" "$HANDLERS_GO"; then
    log "✅ 后端使用详细验证消息数组"
else
    log "❌ 后端未使用详细验证消息数组"
fi

if grep -q "Pandoc路径验证" "$HANDLERS_GO"; then
    log "✅ 后端包含Pandoc路径验证消息"
else
    log "❌ 后端缺少Pandoc路径验证消息"
fi

if grep -q "模板文件验证" "$HANDLERS_GO"; then
    log "✅ 后端包含模板文件验证消息"
else
    log "❌ 后端缺少模板文件验证消息"
fi

if grep -q "未配置模板文件（可选）" "$HANDLERS_GO"; then
    log "✅ 后端正确处理可选模板文件"
else
    log "❌ 后端未正确处理可选模板文件"
fi

# 检查前端改进
log "检查前端配置验证显示改进..."

SETTINGS_CPP="$PROJECT_ROOT/qt-frontend/src/settingswidget.cpp"

if [ ! -f "$SETTINGS_CPP" ]; then
    log "❌ 未找到settingswidget.cpp文件"
    exit 1
fi

# 检查详细验证结果显示
if grep -q "split.*'\\\\n'" "$SETTINGS_CPP"; then
    log "✅ 前端解析多行验证结果"
else
    log "❌ 前端未解析多行验证结果"
fi

if grep -q "配置验证通过" "$SETTINGS_CPP"; then
    log "✅ 前端显示验证通过消息"
else
    log "❌ 前端未显示验证通过消息"
fi

if grep -q "配置验证失败" "$SETTINGS_CPP"; then
    log "✅ 前端显示验证失败消息"
else
    log "❌ 前端未显示验证失败消息"
fi

# 启动后端服务进行API测试
log "启动后端服务进行API测试..."

# 检查是否有后端可执行文件
BACKEND_BINARY=""
if [ -f "$PROJECT_ROOT/build/md2docx-server-macos" ]; then
    BACKEND_BINARY="$PROJECT_ROOT/build/md2docx-server-macos"
elif [ -f "$PROJECT_ROOT/md2docx-server" ]; then
    BACKEND_BINARY="$PROJECT_ROOT/md2docx-server"
else
    log "⚠️  未找到后端可执行文件，跳过API测试"
fi

if [ -n "$BACKEND_BINARY" ]; then
    log "✅ 找到后端可执行文件: $BACKEND_BINARY"
    
    # 启动后端服务
    "$BACKEND_BINARY" &
    BACKEND_PID=$!
    
    # 等待服务启动
    sleep 3
    
    # 测试配置验证API
    log "测试配置验证API..."
    
    # 测试1：验证不存在的Pandoc路径
    log "测试1：验证不存在的Pandoc路径"
    
    # 首先设置一个无效的配置
    curl -s -X POST http://localhost:8080/api/config \
        -H "Content-Type: application/json" \
        -d '{"pandoc_path": "/nonexistent/pandoc", "template_file": ""}' \
        > /dev/null 2>&1
    
    # 然后验证配置
    VALIDATION_RESULT=$(curl -s -X POST http://localhost:8080/api/config/validate)
    
    if echo "$VALIDATION_RESULT" | grep -q "Pandoc路径验证失败"; then
        log "✅ API正确返回Pandoc路径验证失败消息"
    else
        log "❌ API未返回预期的Pandoc路径验证失败消息"
        log "实际返回: $VALIDATION_RESULT"
    fi
    
    if echo "$VALIDATION_RESULT" | grep -q "未配置模板文件"; then
        log "✅ API正确返回模板文件未配置消息"
    else
        log "❌ API未返回预期的模板文件未配置消息"
    fi
    
    # 测试2：验证无效的模板文件
    log "测试2：验证无效的模板文件"
    
    curl -s -X POST http://localhost:8080/api/config \
        -H "Content-Type: application/json" \
        -d '{"pandoc_path": "/usr/local/bin/pandoc", "template_file": "/nonexistent/template.docx"}' \
        > /dev/null 2>&1
    
    VALIDATION_RESULT=$(curl -s -X POST http://localhost:8080/api/config/validate)
    
    if echo "$VALIDATION_RESULT" | grep -q "模板文件验证失败"; then
        log "✅ API正确返回模板文件验证失败消息"
    else
        log "❌ API未返回预期的模板文件验证失败消息"
        log "实际返回: $VALIDATION_RESULT"
    fi
    
    # 关闭后端服务
    kill $BACKEND_PID 2>/dev/null || true
    wait $BACKEND_PID 2>/dev/null || true
    
    log "✅ API测试完成"
fi

# 创建测试说明文档
cat > "$RESULTS_DIR/config_validation_test_instructions.md" << 'EOF'
# 配置验证功能改进测试说明

## 测试目标
验证设置页面的配置验证功能改进，包括：
1. 分别验证Pandoc路径和模板文件路径
2. 提供详细的验证结果反馈
3. 使用清晰的成功/失败指示

## 手动测试步骤

### 步骤1：测试Pandoc路径验证

#### 测试场景1：有效的Pandoc路径
1. 启动应用程序
2. 切换到"设置"标签页
3. 在Pandoc路径中输入有效路径（如：/usr/local/bin/pandoc）
4. 点击"验证配置"按钮
5. 观察验证结果

**预期结果：**
- 显示 `✅ Pandoc路径验证成功`
- 如果未配置模板文件，显示 `ℹ️ 未配置模板文件（可选）`

#### 测试场景2：无效的Pandoc路径
1. 在Pandoc路径中输入无效路径（如：/nonexistent/pandoc）
2. 点击"验证配置"按钮
3. 观察验证结果

**预期结果：**
- 显示 `❌ Pandoc路径验证失败: [具体错误信息]`
- 显示 `ℹ️ 未配置模板文件（可选）`

#### 测试场景3：空的Pandoc路径
1. 清空Pandoc路径
2. 点击"验证配置"按钮
3. 观察验证结果

**预期结果：**
- 显示 `❌ Pandoc路径验证失败: Pandoc路径未配置`

### 步骤2：测试模板文件验证

#### 测试场景4：有效的Pandoc路径 + 有效的模板文件
1. 设置有效的Pandoc路径
2. 勾选"使用自定义模板"
3. 选择一个存在的.docx文件作为模板
4. 点击"验证配置"按钮

**预期结果：**
- 显示 `✅ Pandoc路径验证成功`
- 显示 `✅ 模板文件验证成功`

#### 测试场景5：有效的Pandoc路径 + 无效的模板文件
1. 设置有效的Pandoc路径
2. 勾选"使用自定义模板"
3. 在模板文件路径中输入不存在的文件路径
4. 点击"验证配置"按钮

**预期结果：**
- 显示 `✅ Pandoc路径验证成功`
- 显示 `❌ 模板文件验证失败: [具体错误信息]`

#### 测试场景6：错误的模板文件格式
1. 设置有效的Pandoc路径
2. 勾选"使用自定义模板"
3. 选择一个非.docx文件（如.txt文件）
4. 点击"验证配置"按钮

**预期结果：**
- 显示 `✅ Pandoc路径验证成功`
- 显示 `❌ 模板文件验证失败: 模板文件必须是.docx格式`

### 步骤3：验证显示效果改进

检查以下显示效果：
- [ ] 验证结果使用不同颜色区分成功/失败/信息
- [ ] 成功消息使用绿色和✅图标
- [ ] 失败消息使用红色和❌图标
- [ ] 信息消息使用蓝色和ℹ️图标
- [ ] 消息格式清晰易读
- [ ] 时间戳正确显示

## 预期结果总结
- 配置验证应该分别检查Pandoc路径和模板文件
- 每个组件的验证结果应该单独显示
- 使用清晰的图标和颜色区分不同类型的消息
- 提供具体的错误信息帮助用户解决问题
- 模板文件验证应该正确处理可选性（未配置时不报错）

## 回归测试
确保改进不会影响原有功能：
- [ ] 配置保存功能正常
- [ ] 配置加载功能正常
- [ ] Pandoc路径检测功能正常
- [ ] 模板文件选择功能正常
EOF

log "✅ 生成测试说明文档"

# 生成测试报告
cat > "$RESULTS_DIR/config_validation_test_report.md" << EOF
# 配置验证功能改进测试报告

## 测试时间
$(date '+%Y-%m-%d %H:%M:%S')

## 后端API检查结果
- 详细验证消息数组: $(grep -q "validationMessages.*\[\]string" "$HANDLERS_GO" && echo "✅ 通过" || echo "❌ 失败")
- Pandoc路径验证消息: $(grep -q "Pandoc路径验证" "$HANDLERS_GO" && echo "✅ 通过" || echo "❌ 失败")
- 模板文件验证消息: $(grep -q "模板文件验证" "$HANDLERS_GO" && echo "✅ 通过" || echo "❌ 失败")
- 可选模板文件处理: $(grep -q "未配置模板文件（可选）" "$HANDLERS_GO" && echo "✅ 通过" || echo "❌ 失败")

## 前端显示检查结果
- 多行结果解析: $(grep -q "split.*'\\\\n'" "$SETTINGS_CPP" && echo "✅ 通过" || echo "❌ 失败")
- 验证通过消息: $(grep -q "配置验证通过" "$SETTINGS_CPP" && echo "✅ 通过" || echo "❌ 失败")
- 验证失败消息: $(grep -q "配置验证失败" "$SETTINGS_CPP" && echo "✅ 通过" || echo "❌ 失败")

## API测试结果
$(if [ -n "$BACKEND_BINARY" ]; then echo "- 后端服务可用: ✅ 通过"; else echo "- 后端服务可用: ⚠️  跳过"; fi)

## 测试文件
- 测试说明文档: $RESULTS_DIR/config_validation_test_instructions.md

## 下一步
请按照测试说明文档进行详细的手动测试以验证所有改进功能。
EOF

log "✅ $TEST_NAME 完成"
log "📋 请查看 $RESULTS_DIR/config_validation_test_instructions.md 进行手动测试"
log "📊 测试报告: $RESULTS_DIR/config_validation_test_report.md"
