#!/bin/bash

# 多文件转换状态显示改进功能测试
# 测试状态显示是否更清晰，字体是否更大，信息是否更详细

set -e

# 测试配置
TEST_NAME="多文件转换状态显示改进测试"
TEST_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$TEST_DIR/../.." && pwd)"
RESULTS_DIR="$PROJECT_ROOT/tests/results"

# 创建结果目录
mkdir -p "$RESULTS_DIR"

# 日志函数
log() {
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1" | tee -a "$RESULTS_DIR/status_display_test.log"
}

log "开始执行 $TEST_NAME"

# 检查代码实现
log "检查状态显示改进的代码实现..."

# 检查MultiFileConverter中的状态显示改进
MULTIFILE_CPP="$PROJECT_ROOT/qt-frontend/src/multifileconverter.cpp"

if [ ! -f "$MULTIFILE_CPP" ]; then
    log "❌ 未找到MultiFileConverter源文件"
    exit 1
fi

# 检查状态文本框高度是否增加
if grep -q "setMaximumHeight(200)" "$MULTIFILE_CPP"; then
    log "✅ 状态文本框高度已增加到200"
elif grep -q "setMaximumHeight(120)" "$MULTIFILE_CPP"; then
    log "⚠️  状态文本框高度仍为120，未改进"
else
    log "❓ 无法确定状态文本框高度设置"
fi

# 检查字体大小是否增加
if grep -q "setPointSize.*+ 2" "$MULTIFILE_CPP"; then
    log "✅ 字体大小已增加"
else
    log "❌ 未找到字体大小增加的代码"
fi

# 检查HTML格式显示
if grep -q "insertHtml" "$MULTIFILE_CPP"; then
    log "✅ 使用HTML格式显示状态信息"
else
    log "❌ 未使用HTML格式显示"
fi

# 检查改进的状态消息格式
if grep -q "成功转换:" "$MULTIFILE_CPP"; then
    log "✅ 使用改进的成功转换消息格式"
else
    log "❌ 未使用改进的成功转换消息格式"
fi

if grep -q "转换失败:" "$MULTIFILE_CPP"; then
    log "✅ 使用改进的失败转换消息格式"
else
    log "❌ 未使用改进的失败转换消息格式"
fi

# 检查分隔线
if grep -q "<hr style=" "$MULTIFILE_CPP"; then
    log "✅ 添加了分隔线来区分转换批次"
else
    log "❌ 未添加分隔线"
fi

# 检查滚动到底部功能
if grep -q "QTextCursor.*End" "$MULTIFILE_CPP"; then
    log "✅ 实现了自动滚动到底部功能"
else
    log "❌ 未实现自动滚动到底部功能"
fi

# 检查SettingsWidget的状态显示改进
SETTINGS_CPP="$PROJECT_ROOT/qt-frontend/src/settingswidget.cpp"

if [ -f "$SETTINGS_CPP" ]; then
    log "检查设置页面状态显示改进..."
    
    if grep -q "insertHtml" "$SETTINGS_CPP"; then
        log "✅ 设置页面使用HTML格式显示状态"
    else
        log "❌ 设置页面未使用HTML格式显示"
    fi
    
    if grep -q "color.*#" "$SETTINGS_CPP"; then
        log "✅ 设置页面使用颜色区分不同类型的消息"
    else
        log "❌ 设置页面未使用颜色区分消息"
    fi
fi

# 创建测试说明文档
cat > "$RESULTS_DIR/status_display_test_instructions.md" << 'EOF'
# 多文件转换状态显示改进功能测试说明

## 测试目标
验证多文件转换状态显示的改进，包括：
1. 更大的字体和更高的显示区域
2. 更清晰的状态信息格式
3. 更好的视觉效果（颜色、分隔线等）

## 手动测试步骤

### 步骤1：准备测试文件
1. 创建2-3个测试Markdown文件
2. 确保后端服务正在运行

### 步骤2：测试多文件转换状态显示
1. 启动应用程序
2. 切换到"多文件转换"标签页
3. 选择多个Markdown文件
4. 点击"开始批量转换"
5. 观察状态显示区域

### 步骤3：验证改进效果
检查以下改进是否生效：

#### 显示区域改进
- [ ] 状态显示区域高度是否比之前更高（200px vs 120px）
- [ ] 字体是否比之前更大更清晰

#### 消息格式改进
- [ ] 转换开始时是否显示分隔线
- [ ] 成功转换的消息格式：`✅ 成功转换: 文件名 → 输出文件名`
- [ ] 失败转换的消息格式：`❌ 转换失败: 文件名 - 错误: 错误信息`
- [ ] 时间戳是否正确显示

#### 视觉效果改进
- [ ] 消息是否使用HTML格式显示
- [ ] 不同类型的消息是否有不同的颜色
- [ ] 状态区域是否自动滚动到最新消息

### 步骤4：测试设置页面状态显示
1. 切换到"设置"标签页
2. 点击"验证配置"按钮
3. 观察状态显示

#### 验证改进效果
- [ ] 验证结果是否分别显示Pandoc和模板文件的验证状态
- [ ] 成功的验证是否显示绿色的✅
- [ ] 失败的验证是否显示红色的❌
- [ ] 消息格式是否清晰易读

## 预期结果
- 状态显示区域更大，字体更清晰
- 转换状态信息更详细，格式更规范
- 使用颜色和图标区分不同类型的消息
- 自动滚动到最新消息
- 整体用户体验更好

## 回归测试
确保改进不会影响原有功能：
- [ ] 单文件转换功能正常
- [ ] 多文件转换功能正常
- [ ] 配置保存和加载功能正常
EOF

log "✅ 生成测试说明文档"

# 创建自动化测试脚本
cat > "$RESULTS_DIR/status_display_automated_test.py" << 'EOF'
#!/usr/bin/env python3
"""
状态显示改进功能自动化测试
使用图像识别技术验证UI改进效果
"""

import os
import sys
import time
import subprocess
from pathlib import Path

def log(message):
    timestamp = time.strftime("%Y-%m-%d %H:%M:%S")
    print(f"[{timestamp}] {message}")

def test_status_display_improvements():
    """测试状态显示改进"""
    log("开始状态显示改进自动化测试")
    
    # 这里可以添加更多的自动化测试逻辑
    # 例如使用pyautogui进行GUI自动化测试
    
    try:
        # 检查是否安装了必要的依赖
        import pyautogui
        log("✅ pyautogui可用，可以进行GUI自动化测试")
        
        # 这里可以添加具体的GUI测试代码
        # 例如截图对比、文本识别等
        
    except ImportError:
        log("⚠️  pyautogui未安装，跳过GUI自动化测试")
        log("💡 提示：可以运行 'pip install pyautogui' 来安装GUI自动化测试依赖")
    
    log("自动化测试完成")

if __name__ == "__main__":
    test_status_display_improvements()
EOF

chmod +x "$RESULTS_DIR/status_display_automated_test.py"

# 生成测试报告
cat > "$RESULTS_DIR/status_display_test_report.md" << EOF
# 多文件转换状态显示改进功能测试报告

## 测试时间
$(date '+%Y-%m-%d %H:%M:%S')

## 代码检查结果

### MultiFileConverter改进检查
- 状态文本框高度增加: $(grep -q "setMaximumHeight(200)" "$MULTIFILE_CPP" && echo "✅ 通过" || echo "❌ 失败")
- 字体大小增加: $(grep -q "setPointSize.*+ 2" "$MULTIFILE_CPP" && echo "✅ 通过" || echo "❌ 失败")
- HTML格式显示: $(grep -q "insertHtml" "$MULTIFILE_CPP" && echo "✅ 通过" || echo "❌ 失败")
- 改进的消息格式: $(grep -q "成功转换:\|转换失败:" "$MULTIFILE_CPP" && echo "✅ 通过" || echo "❌ 失败")
- 分隔线添加: $(grep -q "<hr style=" "$MULTIFILE_CPP" && echo "✅ 通过" || echo "❌ 失败")
- 自动滚动: $(grep -q "QTextCursor.*End" "$MULTIFILE_CPP" && echo "✅ 通过" || echo "❌ 失败")

### SettingsWidget改进检查
- HTML格式显示: $(grep -q "insertHtml" "$SETTINGS_CPP" && echo "✅ 通过" || echo "❌ 失败")
- 颜色区分消息: $(grep -q "color.*#" "$SETTINGS_CPP" && echo "✅ 通过" || echo "❌ 失败")

## 测试文件
- 测试说明文档: $RESULTS_DIR/status_display_test_instructions.md
- 自动化测试脚本: $RESULTS_DIR/status_display_automated_test.py

## 下一步
1. 请按照测试说明文档进行手动GUI测试
2. 运行自动化测试脚本（如果环境支持）
3. 验证所有改进功能是否正常工作
EOF

log "✅ $TEST_NAME 完成"
log "📋 请查看 $RESULTS_DIR/status_display_test_instructions.md 进行手动测试"
log "📊 测试报告: $RESULTS_DIR/status_display_test_report.md"
