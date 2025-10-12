QT += core widgets network

CONFIG += c++17

TARGET = md2docx
TEMPLATE = app

# 源文件
SOURCES += \
    src/main_simple.cpp \
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
