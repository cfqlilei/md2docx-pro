QT += core widgets network

CONFIG += c++17

# 应用程序信息
TARGET = md2docx_simple_integrated
TEMPLATE = app

# 版本信息 - 从VERSION文件读取
VERSION = $$cat($$PWD/../VERSION)
isEmpty(VERSION): VERSION = dev

# 定义版本宏
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

# 源文件 - 使用现有的文件，只修改main
SOURCES += \
    src/main_simple_integrated.cpp \
    src/embeddedserver.cpp \
    src/singlefileconverter.cpp \
    src/multifileconverter.cpp \
    src/settingswidget.cpp \
    src/aboutwidget.cpp \
    src/httpapi.cpp \
    src/appsettings.cpp

# 头文件
HEADERS += \
    src/embeddedserver.h \
    src/singlefileconverter.h \
    src/multifileconverter.h \
    src/settingswidget.h \
    src/aboutwidget.h \
    src/httpapi.h \
    src/appsettings.h

# 包含路径
INCLUDEPATH += src

# 构建目录 - 统一使用 build 目录结构
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
macx {
    # macOS配置

    # 复制后端可执行文件到应用包
    QMAKE_POST_LINK += $$quote(mkdir -p $$DESTDIR/$${TARGET}.app/Contents/MacOS/)
    QMAKE_POST_LINK += && $$quote(cp $$PWD/../build/bin/md2docx-server $$DESTDIR/$${TARGET}.app/Contents/MacOS/ 2>/dev/null || true)

    # macOS特定编译选项
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.13
    QMAKE_CXXFLAGS += -stdlib=libc++
}

win32 {
    # Windows配置

    # 复制后端可执行文件到输出目录
    QMAKE_POST_LINK += $$quote(copy /Y $$shell_path($$PWD/../build/bin/md2docx-server.exe) $$shell_path($$DESTDIR/) 2>nul || echo.)

    # Windows特定编译选项
    QMAKE_CXXFLAGS += /utf-8
    DEFINES += WIN32_LEAN_AND_MEAN

    # Windows GUI应用程序配置（不显示控制台）
    CONFIG += windows
}

# 编译器警告
QMAKE_CXXFLAGS += -Wall -Wextra

# 调试信息
CONFIG(debug, debug|release) {
    DEFINES += DEBUG
    QMAKE_CXXFLAGS += -g
} else {
    DEFINES += QT_NO_DEBUG_OUTPUT
    QMAKE_CXXFLAGS += -O2
}

# 消息输出
message("构建简单整合版 Markdown转Word工具")
message("目标平台: $$QMAKESPEC")
message("Qt版本: $$[QT_VERSION]")
message("输出目录: $$DESTDIR")
