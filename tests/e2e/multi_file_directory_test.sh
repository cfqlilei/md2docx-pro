#!/bin/bash

# 多文件转换界面文件夹路径记录功能测试
# 测试选择文件后是否正确记录文件夹路径，下次选择时是否默认打开上次的文件夹

set -e

# 测试配置
TEST_NAME="多文件转换界面文件夹路径记录测试"
TEST_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$TEST_DIR/../.." && pwd)"
RESULTS_DIR="$PROJECT_ROOT/tests/results"

# 创建结果目录
mkdir -p "$RESULTS_DIR"

# 日志函数
log() {
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1" | tee -a "$RESULTS_DIR/multi_file_directory_test.log"
}

log "开始执行 $TEST_NAME"

# 检查是否有Qt前端可执行文件
QT_APP=""
if [ -f "$PROJECT_ROOT/qt-frontend/build/md2docx" ]; then
    QT_APP="$PROJECT_ROOT/qt-frontend/build/md2docx"
elif [ -f "$PROJECT_ROOT/qt-frontend/build/md2docx.app/Contents/MacOS/md2docx" ]; then
    QT_APP="$PROJECT_ROOT/qt-frontend/build/md2docx.app/Contents/MacOS/md2docx"
else
    log "❌ 未找到Qt前端可执行文件，跳过GUI测试"
    exit 0
fi

log "✅ 找到Qt应用程序: $QT_APP"

# 创建测试文件
TEST_FILES_DIR="$RESULTS_DIR/test_files"
mkdir -p "$TEST_FILES_DIR"

# 创建测试Markdown文件
cat > "$TEST_FILES_DIR/test1.md" << 'EOF'
# 测试文档1

这是第一个测试文档。

## 内容

- 项目1
- 项目2
- 项目3
EOF

cat > "$TEST_FILES_DIR/test2.md" << 'EOF'
# 测试文档2

这是第二个测试文档。

## 功能

1. 功能A
2. 功能B
3. 功能C
EOF

log "✅ 创建测试文件完成"

# 检查AppSettings配置文件位置
# Qt应用程序通常将设置保存在用户目录下
SETTINGS_LOCATIONS=(
    "$HOME/.config/Markdown转Word工具/MD2DOCX.conf"
    "$HOME/Library/Preferences/com.Markdown转Word工具.MD2DOCX.plist"
    "$HOME/.local/share/Markdown转Word工具/MD2DOCX.conf"
)

log "检查可能的设置文件位置..."
for location in "${SETTINGS_LOCATIONS[@]}"; do
    if [ -f "$location" ]; then
        log "✅ 找到设置文件: $location"
        SETTINGS_FILE="$location"
        break
    fi
done

# 备份现有设置文件（如果存在）
if [ -n "$SETTINGS_FILE" ] && [ -f "$SETTINGS_FILE" ]; then
    cp "$SETTINGS_FILE" "$SETTINGS_FILE.backup"
    log "✅ 备份现有设置文件"
fi

# 测试说明
cat > "$RESULTS_DIR/multi_file_directory_test_instructions.md" << 'EOF'
# 多文件转换界面文件夹路径记录功能测试说明

## 测试目标
验证多文件转换界面能够记录用户选择文件所在的文件夹，并在下次选择文件时默认打开上次的文件夹目录。

## 手动测试步骤

### 步骤1：首次选择文件
1. 启动应用程序
2. 切换到"多文件转换"标签页
3. 点击"选择文件..."按钮
4. 导航到测试文件目录并选择文件
5. 确认文件被添加到列表中

### 步骤2：验证路径记录
1. 再次点击"选择文件..."按钮
2. 验证文件选择对话框是否默认打开上次选择文件的目录
3. 如果是，则测试通过

### 步骤3：更换目录测试
1. 在文件选择对话框中导航到不同的目录
2. 选择其他目录中的文件
3. 再次点击"选择文件..."按钮
4. 验证是否打开最新选择文件的目录

## 预期结果
- 应用程序应该记录最后选择文件所在的目录
- 下次打开文件选择对话框时应该默认打开该目录
- 配置应该持久化保存，重启应用后仍然有效

## 技术实现验证点
- AppSettings类应该包含getLastMultiInputDir()和setLastMultiInputDir()方法
- MultiFileConverter::selectInputFiles()方法应该使用AppSettings保存和恢复目录
- 配置应该保存在系统标准位置
EOF

log "✅ 生成测试说明文档"

# 自动化测试：检查代码实现
log "检查代码实现..."

# 检查AppSettings类是否包含新方法
if grep -q "getLastMultiInputDir" "$PROJECT_ROOT/qt-frontend/src/appsettings.h"; then
    log "✅ AppSettings头文件包含getLastMultiInputDir方法"
else
    log "❌ AppSettings头文件缺少getLastMultiInputDir方法"
fi

if grep -q "setLastMultiInputDir" "$PROJECT_ROOT/qt-frontend/src/appsettings.h"; then
    log "✅ AppSettings头文件包含setLastMultiInputDir方法"
else
    log "❌ AppSettings头文件缺少setLastMultiInputDir方法"
fi

# 检查实现文件
if grep -q "getLastMultiInputDir" "$PROJECT_ROOT/qt-frontend/src/appsettings.cpp"; then
    log "✅ AppSettings实现文件包含getLastMultiInputDir方法"
else
    log "❌ AppSettings实现文件缺少getLastMultiInputDir方法"
fi

if grep -q "setLastMultiInputDir" "$PROJECT_ROOT/qt-frontend/src/appsettings.cpp"; then
    log "✅ AppSettings实现文件包含setLastMultiInputDir方法"
else
    log "❌ AppSettings实现文件缺少setLastMultiInputDir方法"
fi

# 检查MultiFileConverter是否使用AppSettings
if grep -q "AppSettings::instance" "$PROJECT_ROOT/qt-frontend/src/multifileconverter.cpp"; then
    log "✅ MultiFileConverter使用AppSettings"
else
    log "❌ MultiFileConverter未使用AppSettings"
fi

if grep -q "setLastMultiInputDir" "$PROJECT_ROOT/qt-frontend/src/multifileconverter.cpp"; then
    log "✅ MultiFileConverter调用setLastMultiInputDir"
else
    log "❌ MultiFileConverter未调用setLastMultiInputDir"
fi

# 恢复设置文件（如果有备份）
if [ -n "$SETTINGS_FILE" ] && [ -f "$SETTINGS_FILE.backup" ]; then
    mv "$SETTINGS_FILE.backup" "$SETTINGS_FILE"
    log "✅ 恢复设置文件"
fi

log "✅ $TEST_NAME 完成"
log "📋 请查看 $RESULTS_DIR/multi_file_directory_test_instructions.md 进行手动测试"

# 生成测试报告
cat > "$RESULTS_DIR/multi_file_directory_test_report.md" << EOF
# 多文件转换界面文件夹路径记录功能测试报告

## 测试时间
$(date '+%Y-%m-%d %H:%M:%S')

## 代码检查结果
- AppSettings头文件方法检查: $(grep -q "getLastMultiInputDir\|setLastMultiInputDir" "$PROJECT_ROOT/qt-frontend/src/appsettings.h" && echo "✅ 通过" || echo "❌ 失败")
- AppSettings实现文件检查: $(grep -q "getLastMultiInputDir\|setLastMultiInputDir" "$PROJECT_ROOT/qt-frontend/src/appsettings.cpp" && echo "✅ 通过" || echo "❌ 失败")
- MultiFileConverter集成检查: $(grep -q "AppSettings::instance\|setLastMultiInputDir" "$PROJECT_ROOT/qt-frontend/src/multifileconverter.cpp" && echo "✅ 通过" || echo "❌ 失败")

## 测试文件
- 测试文件目录: $TEST_FILES_DIR
- 测试说明文档: $RESULTS_DIR/multi_file_directory_test_instructions.md

## 下一步
请按照测试说明文档进行手动GUI测试以验证功能完整性。
EOF

log "✅ 生成测试报告: $RESULTS_DIR/multi_file_directory_test_report.md"
