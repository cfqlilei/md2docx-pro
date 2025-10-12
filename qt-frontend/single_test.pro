QT += core widgets network

CONFIG += c++17

TARGET = single_test
TEMPLATE = app

# 源文件
SOURCES += \
    src/main_single_test.cpp \
    src/httpapi.cpp \
    src/singleconverter.cpp

# 头文件
HEADERS += \
    src/httpapi.h \
    src/singleconverter.h

# 输出目录
DESTDIR = build

# 编译器标志
QMAKE_CXXFLAGS += -std=c++17

# 发布配置
CONFIG(release, debug|release) {
    DEFINES += QT_NO_DEBUG_OUTPUT
}

# 调试配置
CONFIG(debug, debug|release) {
    DEFINES += QT_DEBUG
}

# 包含路径
INCLUDEPATH += src

# 库路径
LIBS += 

# 平台特定设置
macx {
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.13
}

win32 {
    CONFIG += console
}
