#!/bin/bash

# 简化的Qt构建脚本 - 只构建基本功能

echo "=== 简化Qt构建 ==="

# 设置Qt环境
export PATH="/opt/homebrew/opt/qt@5/bin:$PATH"
export LDFLAGS="-L/opt/homebrew/opt/qt@5/lib"
export CPPFLAGS="-I/opt/homebrew/opt/qt@5/include"

# 进入Qt前端目录
cd qt-frontend || exit 1

# 创建简化的项目文件
cat > md2docx_simple.pro << 'EOF'
QT += core widgets network

CONFIG += c++17

TARGET = md2docx
TEMPLATE = app

# 源文件
SOURCES += \
    src/main.cpp \
    src/httpapi.cpp

# 头文件
HEADERS += \
    src/httpapi.h

# 输出目录
DESTDIR = build

# 编译器标志
QMAKE_CXXFLAGS += -std=c++17

# 发布配置
CONFIG(release, debug|release) {
    DEFINES += QT_NO_DEBUG_OUTPUT
}
EOF

# 清理并构建
rm -rf build
mkdir -p build

echo "生成Makefile..."
qmake md2docx_simple.pro -o build/Makefile

if [ $? -ne 0 ]; then
    echo "❌ qmake失败"
    exit 1
fi

echo "开始编译..."
cd build
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 2)

if [ $? -eq 0 ]; then
    echo "✅ 简化版本编译成功"
    ls -la md2docx* 2>/dev/null || echo "可执行文件未找到"
else
    echo "❌ 编译失败"
    exit 1
fi
