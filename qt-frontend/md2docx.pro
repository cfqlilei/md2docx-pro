QT += core widgets network

CONFIG += c++17

TARGET = md2docx
TEMPLATE = app

# 定义应用程序信息
VERSION = 1.0.0
QMAKE_TARGET_COMPANY = "Markdown转Word工具"
QMAKE_TARGET_PRODUCT = "MD2DOCX"
QMAKE_TARGET_DESCRIPTION = "Markdown转Word转换工具"
QMAKE_TARGET_COPYRIGHT = "Copyright 2024"

# 源文件
SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/singleconverter.cpp \
    src/batchconverter.cpp \
    src/configmanager.cpp \
    src/httpapi.cpp

# 头文件
HEADERS += \
    src/mainwindow.h \
    src/singleconverter.h \
    src/batchconverter.h \
    src/configmanager.h \
    src/httpapi.h

# UI文件 - 使用代码创建UI，不需要.ui文件
# FORMS += \
#     ui/mainwindow.ui \
#     ui/singleconverter.ui \
#     ui/batchconverter.ui \
#     ui/configmanager.ui

# 资源文件 - 暂时注释掉，因为图标文件还未创建
# RESOURCES += \
#     resources/resources.qrc

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

# 平台特定配置
win32 {
    # RC_ICONS = resources/icon.ico
    QMAKE_TARGET_PRODUCT = "MD2DOCX"
}

macx {
    # ICON = resources/icon.icns
    # QMAKE_INFO_PLIST = resources/Info.plist
}

unix:!macx {
    # Linux配置
    target.path = /usr/local/bin
    INSTALLS += target
}

# 编译器标志
QMAKE_CXXFLAGS += -std=c++17

# 调试配置
CONFIG(debug, debug|release) {
    DEFINES += DEBUG_MODE
    TARGET = $$join(TARGET,,,_debug)
}

# 发布配置
CONFIG(release, debug|release) {
    DEFINES += QT_NO_DEBUG_OUTPUT
}
