QT += core widgets network

CONFIG += c++17

TARGET = simple_complete
TEMPLATE = app

# 源文件
SOURCES += \
    src/main_simple_complete.cpp \
    src/httpapi.cpp \
    src/singleconverter.cpp \
    src/simple_batchconverter.cpp

# 头文件
HEADERS += \
    src/httpapi.h \
    src/singleconverter.h \
    src/simple_batchconverter.h

# 包含路径
INCLUDEPATH += src

# 输出目录
DESTDIR = build

# 编译器标志
QMAKE_CXXFLAGS += -Wall -Wextra

# macOS特定设置
macx {
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.13
    QMAKE_CXXFLAGS += -std=c++17
}

# Windows特定设置
win32 {
    QMAKE_CXXFLAGS += /std:c++17
}

# 调试信息
CONFIG(debug, debug|release) {
    DEFINES += QT_DEBUG
    TARGET = $$TARGET"_debug"
}

# 发布版本优化
CONFIG(release, debug|release) {
    DEFINES += QT_NO_DEBUG_OUTPUT
    QMAKE_CXXFLAGS += -O2
}
