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

# 输出目录 - 统一使用 build 目录结构
CONFIG(debug, debug|release) {
    DESTDIR = $$PWD/../build/bin
    OBJECTS_DIR = $$PWD/../build/intermediate/qt/$${TARGET}/debug/obj
    MOC_DIR = $$PWD/../build/intermediate/qt/$${TARGET}/debug/moc
    RCC_DIR = $$PWD/../build/intermediate/qt/$${TARGET}/debug/rcc
    UI_DIR = $$PWD/../build/intermediate/qt/$${TARGET}/debug/ui
} else {
    DESTDIR = $$PWD/../build/bin
    OBJECTS_DIR = $$PWD/../build/intermediate/qt/$${TARGET}/release/obj
    MOC_DIR = $$PWD/../build/intermediate/qt/$${TARGET}/release/moc
    RCC_DIR = $$PWD/../build/intermediate/qt/$${TARGET}/release/rcc
    UI_DIR = $$PWD/../build/intermediate/qt/$${TARGET}/release/ui
}

# 编译器标志
QMAKE_CXXFLAGS += -std=c++17

# 发布配置
CONFIG(release, debug|release) {
    DEFINES += QT_NO_DEBUG_OUTPUT
}
