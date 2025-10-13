QT += core widgets network

CONFIG += c++17

TARGET = md2docx_app
TEMPLATE = app

# 设置输出目录
DESTDIR = build

# 设置目标平台
macx {
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.13
    CONFIG += app_bundle
}

win32 {
    CONFIG += console
}

# 源文件
SOURCES += \
    src/main_md2docx.cpp \
    src/mainwindow_md2docx.cpp \
    src/singlefileconverter.cpp \
    src/multifileconverter.cpp \
    src/settingswidget.cpp \
    src/aboutwidget.cpp \
    src/httpapi.cpp \
    src/appsettings.cpp

# 头文件
HEADERS += \
    src/mainwindow_md2docx.h \
    src/singlefileconverter.h \
    src/multifileconverter.h \
    src/settingswidget.h \
    src/aboutwidget.h \
    src/httpapi.h \
    src/appsettings.h

# 包含路径
INCLUDEPATH += src

# 编译器标志
QMAKE_CXXFLAGS += -Wall -Wextra

# 调试信息
CONFIG(debug, debug|release) {
    DEFINES += DEBUG
    TARGET = $$join(TARGET,,,_debug)
}

# 发布版本优化
CONFIG(release, debug|release) {
    DEFINES += QT_NO_DEBUG_OUTPUT
    QMAKE_CXXFLAGS += -O2
}

# 资源文件（如果有的话）
# RESOURCES += resources.qrc

# 应用程序信息
VERSION = 2.0.0
QMAKE_TARGET_COMPANY = "MD2DOCX"
QMAKE_TARGET_PRODUCT = "Markdown转Word工具"
QMAKE_TARGET_DESCRIPTION = "将Markdown文件转换为Word文档的工具"
QMAKE_TARGET_COPYRIGHT = "Copyright (c) 2024"

# macOS特定设置
macx {
    QMAKE_INFO_PLIST = Info.plist
    ICON = app.icns
    
    # 设置应用程序包信息
    QMAKE_TARGET_BUNDLE_PREFIX = com.md2docx
    
    # 代码签名（如果需要）
    # QMAKE_POST_LINK += codesign -s "Developer ID Application: Your Name" $$DESTDIR/$${TARGET}.app
}

# Windows特定设置
win32 {
    RC_ICONS = app.ico
    
    # 设置版本信息
    RC_FILE = version.rc
}

# 清理规则
QMAKE_CLEAN += $$DESTDIR/*
