#!/bin/bash

# macOS打包脚本
# 创建完整的macOS应用程序包

set -e

echo "=== Markdown转Word工具 - macOS打包 ==="
echo "打包时间: $(date)"
echo

# 检查环境
echo "1. 检查环境依赖..."

# 检查Go
if ! command -v go &> /dev/null; then
    echo "❌ Go未安装，请先安装Go"
    exit 1
fi
echo "✅ Go版本: $(go version)"

# 检查Qt
if ! command -v qmake &> /dev/null; then
    echo "❌ Qt未安装，请先安装Qt"
    echo "   macOS: brew install qt@5"
    exit 1
fi
echo "✅ QMake版本: $(qmake --version | head -1)"

# 检查Pandoc
if ! command -v pandoc &> /dev/null; then
    echo "⚠️  Pandoc未安装，用户需要自行安装"
    echo "   macOS: brew install pandoc"
else
    echo "✅ Pandoc版本: $(pandoc --version | head -1)"
fi

echo

# 设置变量
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_DIR/dist/macos"
APP_NAME="Markdown转Word工具"
APP_BUNDLE="$BUILD_DIR/$APP_NAME.app"
VERSION="1.0.0"

echo "2. 准备打包目录..."
cd "$PROJECT_DIR"

# 清理并创建打包目录
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
echo "✅ 打包目录: $BUILD_DIR"

echo

# 构建Go后端
echo "3. 构建Go后端..."
echo "   初始化Go模块..."
if [ ! -f "go.mod" ]; then
    go mod init md2docx
fi
go mod tidy

echo "   编译Go后端..."
mkdir -p build
CGO_ENABLED=1 GOOS=darwin GOARCH=arm64 go build -ldflags="-s -w" -o build/md2docx-server-macos ./cmd/server

if [ ! -f "build/md2docx-server-macos" ]; then
    echo "❌ Go后端编译失败"
    exit 1
fi
echo "✅ Go后端编译完成: build/md2docx-server-macos"

echo

# 构建Qt前端
echo "4. 构建Qt前端..."
export PATH="/opt/homebrew/opt/qt@5/bin:$PATH"

cd qt-frontend
rm -rf build_complete_test
mkdir build_complete_test
cd build_complete_test

echo "   运行qmake..."
qmake CONFIG+=release ../complete_test.pro

echo "   运行make..."
make

if [ ! -f "build/complete_test.app/Contents/MacOS/complete_test" ]; then
    echo "❌ Qt前端编译失败"
    exit 1
fi
echo "✅ Qt前端编译完成"

cd "$PROJECT_DIR"

echo

# 创建应用程序包
echo "5. 创建应用程序包..."

# 复制Qt应用程序包
cp -R qt-frontend/build_complete_test/build/complete_test.app "$APP_BUNDLE"

# 重命名应用程序包
mv "$APP_BUNDLE/Contents/MacOS/complete_test" "$APP_BUNDLE/Contents/MacOS/$APP_NAME"

# 创建后端服务目录
mkdir -p "$APP_BUNDLE/Contents/Resources/backend"
cp build/md2docx-server-macos "$APP_BUNDLE/Contents/Resources/backend/"

# 复制资源文件
mkdir -p "$APP_BUNDLE/Contents/Resources/docs"
cp -R docs/* "$APP_BUNDLE/Contents/Resources/docs/" 2>/dev/null || true

# 复制示例文件
mkdir -p "$APP_BUNDLE/Contents/Resources/examples"
cp tests/api_test/*.md "$APP_BUNDLE/Contents/Resources/examples/" 2>/dev/null || true

echo "✅ 应用程序包创建完成"

echo

# 更新Info.plist
echo "6. 更新应用程序信息..."
INFO_PLIST="$APP_BUNDLE/Contents/Info.plist"

# 备份原始Info.plist
cp "$INFO_PLIST" "$INFO_PLIST.backup"

# 更新Info.plist
cat > "$INFO_PLIST" << EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleDisplayName</key>
    <string>$APP_NAME</string>
    <key>CFBundleExecutable</key>
    <string>$APP_NAME</string>
    <key>CFBundleIdentifier</key>
    <string>com.md2docx.app</string>
    <key>CFBundleInfoDictionaryVersion</key>
    <string>6.0</string>
    <key>CFBundleName</key>
    <string>$APP_NAME</string>
    <key>CFBundlePackageType</key>
    <string>APPL</string>
    <key>CFBundleShortVersionString</key>
    <string>$VERSION</string>
    <key>CFBundleVersion</key>
    <string>$VERSION</string>
    <key>LSMinimumSystemVersion</key>
    <string>10.13</string>
    <key>NSHighResolutionCapable</key>
    <true/>
    <key>NSHumanReadableCopyright</key>
    <string>Copyright © 2025 MD2DOCX. All rights reserved.</string>
    <key>CFBundleDocumentTypes</key>
    <array>
        <dict>
            <key>CFBundleTypeExtensions</key>
            <array>
                <string>md</string>
                <string>markdown</string>
            </array>
            <key>CFBundleTypeName</key>
            <string>Markdown Document</string>
            <key>CFBundleTypeRole</key>
            <string>Editor</string>
            <key>LSHandlerRank</key>
            <string>Alternate</string>
        </dict>
    </array>
</dict>
</plist>
EOF

echo "✅ 应用程序信息更新完成"

echo

# 处理Qt依赖
echo "7. 处理Qt依赖..."
if command -v macdeployqt &> /dev/null; then
    echo "   运行macdeployqt..."
    macdeployqt "$APP_BUNDLE" -verbose=2
    echo "✅ Qt依赖处理完成"
else
    echo "⚠️  macdeployqt未找到，跳过Qt依赖处理"
    echo "   用户需要确保系统已安装Qt运行时"
fi

echo

# 创建启动脚本
echo "8. 创建启动脚本..."
LAUNCHER_SCRIPT="$APP_BUNDLE/Contents/MacOS/launcher.sh"
cat > "$LAUNCHER_SCRIPT" << 'EOF'
#!/bin/bash

# 获取应用程序包路径
APP_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
RESOURCES_DIR="$APP_DIR/Contents/Resources"
BACKEND_DIR="$RESOURCES_DIR/backend"

# 启动后端服务
echo "启动后端服务..."
cd "$BACKEND_DIR"
./md2docx-server-macos &
BACKEND_PID=$!

# 等待后端服务启动
sleep 2

# 启动前端应用
echo "启动前端应用..."
cd "$APP_DIR/Contents/MacOS"
exec "./Markdown转Word工具"
EOF

chmod +x "$LAUNCHER_SCRIPT"
echo "✅ 启动脚本创建完成"

echo

# 创建安装包
echo "9. 创建安装包..."
DMG_NAME="Markdown转Word工具-v$VERSION-macOS.dmg"
DMG_PATH="$BUILD_DIR/$DMG_NAME"

# 创建临时挂载点
TEMP_DMG="$BUILD_DIR/temp.dmg"
MOUNT_POINT="$BUILD_DIR/mount"

# 计算所需空间（MB）
APP_SIZE=$(du -sm "$APP_BUNDLE" | cut -f1)
DMG_SIZE=$((APP_SIZE + 50))

echo "   创建磁盘映像..."
hdiutil create -size ${DMG_SIZE}m -fs HFS+ -volname "$APP_NAME" "$TEMP_DMG"

echo "   挂载磁盘映像..."
hdiutil attach "$TEMP_DMG" -mountpoint "$MOUNT_POINT"

echo "   复制应用程序..."
cp -R "$APP_BUNDLE" "$MOUNT_POINT/"

# 创建应用程序文件夹的符号链接
ln -s /Applications "$MOUNT_POINT/Applications"

# 创建README文件
cat > "$MOUNT_POINT/README.txt" << EOF
Markdown转Word工具 v$VERSION

安装说明:
1. 将"$APP_NAME.app"拖拽到Applications文件夹
2. 确保系统已安装Pandoc: brew install pandoc
3. 双击应用程序图标启动

功能特性:
- 单文件Markdown转Word
- 批量文件转换
- 自定义模板支持
- 配置管理

技术支持: https://github.com/md2docx
版权所有 © 2025 MD2DOCX
EOF

echo "   卸载磁盘映像..."
hdiutil detach "$MOUNT_POINT"

echo "   压缩磁盘映像..."
hdiutil convert "$TEMP_DMG" -format UDZO -o "$DMG_PATH"

# 清理临时文件
rm -f "$TEMP_DMG"
rm -rf "$MOUNT_POINT"

echo "✅ 安装包创建完成: $DMG_PATH"

echo

# 创建压缩包
echo "10. 创建压缩包..."
ZIP_NAME="Markdown转Word工具-v$VERSION-macOS.zip"
ZIP_PATH="$BUILD_DIR/$ZIP_NAME"

cd "$BUILD_DIR"
zip -r "$ZIP_NAME" "$APP_NAME.app" README.txt

echo "✅ 压缩包创建完成: $ZIP_PATH"

echo

# 显示打包结果
echo "=== 打包完成 ==="
echo "📦 应用程序包: $APP_BUNDLE"
echo "💿 安装包: $DMG_PATH"
echo "📁 压缩包: $ZIP_PATH"
echo
echo "📊 文件大小:"
ls -lh "$BUILD_DIR"/*.dmg "$BUILD_DIR"/*.zip 2>/dev/null || true
echo
echo "🚀 安装说明:"
echo "1. 双击.dmg文件打开安装包"
echo "2. 将应用程序拖拽到Applications文件夹"
echo "3. 确保系统已安装Pandoc: brew install pandoc"
echo "4. 双击应用程序图标启动"
echo
echo "✅ macOS打包完成！"
