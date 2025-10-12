#!/bin/bash

# Markdown转Word工具 - Qt前端构建脚本

echo "=== Markdown转Word工具 - Qt前端构建 ==="
echo "开始时间: $(date '+%Y-%m-%d %H:%M:%S')"
echo "========================================"

# 检查Qt环境
echo "1. 检查Qt环境..."

if ! command -v qmake >/dev/null 2>&1; then
    echo "❌ 错误: 未找到qmake，请确保Qt开发环境已正确安装"
    echo "请安装Qt开发工具包，或者将qmake添加到PATH环境变量中"
    exit 1
fi

QMAKE_VERSION=$(qmake -version | head -n 1)
echo "✅ 找到qmake: $QMAKE_VERSION"

# 检查编译器
echo
echo "2. 检查编译器..."

if command -v g++ >/dev/null 2>&1; then
    GCC_VERSION=$(g++ --version | head -n 1)
    echo "✅ 找到g++: $GCC_VERSION"
elif command -v clang++ >/dev/null 2>&1; then
    CLANG_VERSION=$(clang++ --version | head -n 1)
    echo "✅ 找到clang++: $CLANG_VERSION"
else
    echo "❌ 错误: 未找到C++编译器"
    exit 1
fi

# 进入Qt前端目录
cd qt-frontend || {
    echo "❌ 错误: 无法进入qt-frontend目录"
    exit 1
}

echo
echo "3. 清理之前的构建..."
rm -rf build
mkdir -p build

echo
echo "4. 生成Makefile..."
qmake md2docx.pro -o build/Makefile

if [ $? -ne 0 ]; then
    echo "❌ 错误: qmake失败"
    exit 1
fi

echo "✅ Makefile生成成功"

echo
echo "5. 开始编译..."
cd build

# 获取CPU核心数用于并行编译
if command -v nproc >/dev/null 2>&1; then
    CORES=$(nproc)
elif command -v sysctl >/dev/null 2>&1; then
    CORES=$(sysctl -n hw.ncpu)
else
    CORES=2
fi

echo "使用 $CORES 个核心进行并行编译..."

make -j$CORES

if [ $? -ne 0 ]; then
    echo "❌ 编译失败"
    exit 1
fi

echo "✅ 编译成功"

echo
echo "6. 检查生成的可执行文件..."

if [ -f "md2docx" ] || [ -f "md2docx.exe" ]; then
    echo "✅ 可执行文件生成成功"
    
    # 显示文件信息
    if [ -f "md2docx" ]; then
        ls -la md2docx
        echo "可执行文件路径: $(pwd)/md2docx"
    else
        ls -la md2docx.exe
        echo "可执行文件路径: $(pwd)/md2docx.exe"
    fi
else
    echo "❌ 错误: 未找到生成的可执行文件"
    exit 1
fi

echo
echo "7. 创建启动脚本..."

# 回到项目根目录
cd ../..

# 创建启动脚本
cat > start_app.sh << 'EOF'
#!/bin/bash

echo "=== 启动Markdown转Word工具 ==="

# 启动后端服务
echo "1. 启动后端服务..."
go run cmd/server/main.go &
BACKEND_PID=$!

# 等待后端启动
sleep 3

# 启动Qt前端
echo "2. 启动Qt前端..."
if [ -f "qt-frontend/build/md2docx" ]; then
    ./qt-frontend/build/md2docx
elif [ -f "qt-frontend/build/md2docx.exe" ]; then
    ./qt-frontend/build/md2docx.exe
else
    echo "❌ 错误: 未找到Qt前端可执行文件"
    kill $BACKEND_PID 2>/dev/null
    exit 1
fi

# 清理后端进程
echo "3. 清理后端进程..."
kill $BACKEND_PID 2>/dev/null

echo "应用程序已退出"
EOF

chmod +x start_app.sh

echo "✅ 启动脚本创建成功: start_app.sh"

echo
echo "========================================"
echo "构建完成！"
echo "结束时间: $(date '+%Y-%m-%d %H:%M:%S')"
echo
echo "使用方法:"
echo "1. 运行完整应用: ./start_app.sh"
echo "2. 只运行Qt前端: ./qt-frontend/build/md2docx"
echo "3. 只运行后端: go run cmd/server/main.go"
echo
echo "注意: 运行前请确保系统已安装Pandoc"
echo "========================================"
