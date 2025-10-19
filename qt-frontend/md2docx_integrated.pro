QT += core widgets network

CONFIG += c++17

# 应用程序信息
TARGET = md2docx_integrated
TEMPLATE = app

# 版本信息
VERSION = 1.0.0
QMAKE_TARGET_COMPANY = "MD2DOCX"
QMAKE_TARGET_PRODUCT = "Markdown转Word工具 - 整合版"
QMAKE_TARGET_DESCRIPTION = "Markdown转Word转换工具，内嵌后端服务"
QMAKE_TARGET_COPYRIGHT = "Copyright (C) 2025"

# 源文件
SOURCES += \
    src/main_integrated.cpp \
    src/mainwindow_integrated.cpp \
    src/embeddedserver.cpp \
    src/singlefileconverter.cpp \
    src/multifileconverter.cpp \
    src/settingswidget.cpp \
    src/aboutwidget.cpp \
    src/httpapi.cpp \
    src/appsettings.cpp

# 头文件
HEADERS += \
    src/mainwindow_integrated.h \
    src/embeddedserver.h \
    src/singlefileconverter.h \
    src/multifileconverter.h \
    src/settingswidget.h \
    src/aboutwidget.h \
    src/httpapi.h \
    src/appsettings.h

# 资源文件
RESOURCES += resources/resources.qrc

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
win32 {
    # Windows配置
    RC_ICONS = resources/icons/app.ico

    # 复制后端可执行文件到输出目录
    CONFIG(debug, debug|release) {
        QMAKE_POST_LINK += $$quote(copy /Y $$shell_path($$PWD/../build/bin/md2docx-server.exe) $$shell_path($$DESTDIR/))
    } else {
        QMAKE_POST_LINK += $$quote(copy /Y $$shell_path($$PWD/../build/bin/md2docx-server.exe) $$shell_path($$DESTDIR/))
    }

    # Windows特定编译选项
    QMAKE_CXXFLAGS += /utf-8
    DEFINES += WIN32_LEAN_AND_MEAN
}

macx {
    # macOS配置
    ICON = resources/icons/app.icns

    # 应用程序包配置
    QMAKE_INFO_PLIST = resources/Info.plist

    # 复制后端可执行文件到应用包
    QMAKE_POST_LINK += $$quote(mkdir -p $$DESTDIR/$${TARGET}.app/Contents/MacOS/)
    QMAKE_POST_LINK += && $$quote(cp $$PWD/../build/bin/md2docx-server $$DESTDIR/$${TARGET}.app/Contents/MacOS/)

    # macOS特定编译选项
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.13
    QMAKE_CXXFLAGS += -stdlib=libc++
}

unix:!macx {
    # Linux配置

    # 复制后端可执行文件
    QMAKE_POST_LINK += $$quote(cp $$PWD/../build/bin/md2docx-server $$DESTDIR/ 2>/dev/null || true)

    # 安装配置
    target.path = /usr/local/bin
    INSTALLS += target
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

# 依赖库
LIBS += -lz

# 预编译头文件（可选）
# PRECOMPILED_HEADER = src/pch.h

# 自定义目标：清理
clean_all.commands = rm -rf build_integrated
QMAKE_EXTRA_TARGETS += clean_all

# 自定义目标：部署
deploy.commands = echo "部署整合版应用程序..."
macx {
    deploy.commands += && macdeployqt $$DESTDIR/$${TARGET}.app
}
win32 {
    deploy.commands += && windeployqt $$DESTDIR/$${TARGET}.exe
}
QMAKE_EXTRA_TARGETS += deploy

# 消息输出
message("构建整合版 Markdown转Word工具")
message("目标平台: $$QMAKESPEC")
message("Qt版本: $$[QT_VERSION]")
message("输出目录: $$DESTDIR")
