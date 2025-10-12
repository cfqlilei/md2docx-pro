#include "mainwindow.h"
#include "singleconverter.h"
#include "batchconverter.h"
#include "configmanager.h"
#include "httpapi.h"

#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QStatusBar>
#include <QLabel>
#include <QProgressBar>
#include <QAction>
#include <QMessageBox>
#include <QApplication>
#include <QTimer>
#include <QProcess>
#include <QDir>
#include <QStandardPaths>
#include <QCloseEvent>
#include <QDesktopServices>
#include <QUrl>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_tabWidget(nullptr)
    , m_singleConverter(nullptr)
    , m_batchConverter(nullptr)
    , m_configManager(nullptr)
    , m_statusLabel(nullptr)
    , m_serverStatusLabel(nullptr)
    , m_progressBar(nullptr)
    , m_httpApi(nullptr)
    , m_statusTimer(nullptr)
    , m_backendProcess(nullptr)
    , m_serverOnline(false)
    , m_conversionInProgress(false)
{
    setupUI();
    setupMenuBar();
    setupStatusBar();
    setupConnections();
    
    // 启动后端服务
    startBackendServer();
    
    // 开始检查服务器状态
    m_statusTimer = new QTimer(this);
    connect(m_statusTimer, &QTimer::timeout, this, &MainWindow::checkServerStatus);
    m_statusTimer->start(5000); // 每5秒检查一次
    
    // 立即检查一次
    QTimer::singleShot(2000, this, &MainWindow::checkServerStatus);
}

MainWindow::~MainWindow()
{
    stopBackendServer();
}

void MainWindow::setupUI()
{
    setWindowTitle("Markdown转Word工具 v1.0.0");
    setMinimumSize(800, 600);
    resize(1000, 700);
    
    // 创建中央部件
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    
    // 创建标签页控件
    m_tabWidget = new QTabWidget(this);
    
    // 创建HTTP API客户端
    m_httpApi = new HttpApi(this);
    
    // 创建各个页面
    m_singleConverter = new SingleConverter(m_httpApi, this);
    m_batchConverter = new BatchConverter(m_httpApi, this);
    m_configManager = new ConfigManager(m_httpApi, this);
    
    // 添加标签页
    m_tabWidget->addTab(m_singleConverter, "单文件转换");
    m_tabWidget->addTab(m_batchConverter, "批量转换");
    m_tabWidget->addTab(m_configManager, "设置");
    
    // 设置标签页图标
    m_tabWidget->setTabIcon(0, QIcon(":/icons/single_file.png"));
    m_tabWidget->setTabIcon(1, QIcon(":/icons/batch_files.png"));
    m_tabWidget->setTabIcon(2, QIcon(":/icons/settings.png"));
    
    mainLayout->addWidget(m_tabWidget);
}

void MainWindow::setupMenuBar()
{
    // 文件菜单
    QMenu *fileMenu = menuBar()->addMenu("文件(&F)");
    
    m_exitAction = new QAction("退出(&X)", this);
    m_exitAction->setShortcut(QKeySequence::Quit);
    m_exitAction->setStatusTip("退出应用程序");
    connect(m_exitAction, &QAction::triggered, this, &QWidget::close);
    fileMenu->addAction(m_exitAction);
    
    // 工具菜单
    QMenu *toolsMenu = menuBar()->addMenu("工具(&T)");
    
    m_configAction = new QAction("配置(&C)", this);
    m_configAction->setStatusTip("打开配置页面");
    connect(m_configAction, &QAction::triggered, [this]() {
        m_tabWidget->setCurrentIndex(2); // 切换到配置页面
    });
    toolsMenu->addAction(m_configAction);
    
    // 帮助菜单
    QMenu *helpMenu = menuBar()->addMenu("帮助(&H)");
    
    m_helpAction = new QAction("使用帮助(&H)", this);
    m_helpAction->setStatusTip("查看使用帮助");
    connect(m_helpAction, &QAction::triggered, this, &MainWindow::showHelp);
    helpMenu->addAction(m_helpAction);
    
    helpMenu->addSeparator();
    
    m_aboutAction = new QAction("关于(&A)", this);
    m_aboutAction->setStatusTip("关于此应用程序");
    connect(m_aboutAction, &QAction::triggered, this, &MainWindow::showAbout);
    helpMenu->addAction(m_aboutAction);
}

void MainWindow::setupStatusBar()
{
    // 创建状态标签
    m_statusLabel = new QLabel("就绪", this);
    statusBar()->addWidget(m_statusLabel);
    
    // 创建进度条
    m_progressBar = new QProgressBar(this);
    m_progressBar->setVisible(false);
    m_progressBar->setMaximumWidth(200);
    statusBar()->addPermanentWidget(m_progressBar);
    
    // 创建服务器状态标签
    m_serverStatusLabel = new QLabel("服务器: 离线", this);
    m_serverStatusLabel->setStyleSheet("QLabel { color: red; font-weight: bold; }");
    statusBar()->addPermanentWidget(m_serverStatusLabel);
}

void MainWindow::setupConnections()
{
    // 连接转换器信号
    connect(m_singleConverter, &SingleConverter::conversionStarted,
            this, &MainWindow::onConversionStarted);
    connect(m_singleConverter, &SingleConverter::conversionFinished,
            this, &MainWindow::onConversionFinished);
    
    connect(m_batchConverter, &BatchConverter::conversionStarted,
            this, &MainWindow::onConversionStarted);
    connect(m_batchConverter, &BatchConverter::conversionFinished,
            this, &MainWindow::onConversionFinished);
    
    // 连接配置管理器信号
    connect(m_configManager, &ConfigManager::configChanged,
            this, &MainWindow::onConfigChanged);
    
    // 连接HTTP API信号
    connect(m_httpApi, &HttpApi::healthCheckFinished,
            this, &MainWindow::onServerStatusChanged);
}

void MainWindow::startBackendServer()
{
    if (m_backendProcess) {
        return; // 已经在运行
    }
    
    // 查找后端可执行文件
    QString backendPath;
    
#ifdef Q_OS_WIN
    backendPath = QApplication::applicationDirPath() + "/md2docx-server.exe";
#else
    backendPath = QApplication::applicationDirPath() + "/md2docx-server";
#endif
    
    // 如果在应用程序目录找不到，尝试相对路径
    if (!QFile::exists(backendPath)) {
        backendPath = "../cmd/server/main.go";
    }
    
    m_backendProcess = new QProcess(this);
    
    connect(m_backendProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [this](int exitCode, QProcess::ExitStatus exitStatus) {
                Q_UNUSED(exitCode)
                Q_UNUSED(exitStatus)
                m_serverOnline = false;
                updateServerStatus(false);
            });
    
    // 启动后端服务
    if (backendPath.endsWith(".go")) {
        // 开发模式，使用go run
        m_backendProcess->start("go", QStringList() << "run" << backendPath);
    } else {
        // 生产模式，直接运行可执行文件
        m_backendProcess->start(backendPath);
    }
    
    if (!m_backendProcess->waitForStarted(5000)) {
        QMessageBox::warning(this, "警告", 
            QString("无法启动后端服务: %1").arg(m_backendProcess->errorString()));
        delete m_backendProcess;
        m_backendProcess = nullptr;
    }
}

void MainWindow::stopBackendServer()
{
    if (m_backendProcess) {
        m_backendProcess->terminate();
        if (!m_backendProcess->waitForFinished(5000)) {
            m_backendProcess->kill();
        }
        delete m_backendProcess;
        m_backendProcess = nullptr;
    }
}

void MainWindow::checkServerStatus()
{
    if (m_httpApi) {
        m_httpApi->checkHealth();
    }
}

void MainWindow::onServerStatusChanged(bool isOnline)
{
    m_serverOnline = isOnline;
    updateServerStatus(isOnline);
}

void MainWindow::updateServerStatus(bool isOnline)
{
    if (isOnline) {
        m_serverStatusLabel->setText("服务器: 在线");
        m_serverStatusLabel->setStyleSheet("QLabel { color: green; font-weight: bold; }");
    } else {
        m_serverStatusLabel->setText("服务器: 离线");
        m_serverStatusLabel->setStyleSheet("QLabel { color: red; font-weight: bold; }");
    }
    
    // 启用/禁用转换功能
    m_singleConverter->setEnabled(isOnline);
    m_batchConverter->setEnabled(isOnline);
}

void MainWindow::onConversionStarted()
{
    m_conversionInProgress = true;
    m_statusLabel->setText("转换中...");
    m_progressBar->setVisible(true);
    m_progressBar->setRange(0, 0); // 不确定进度
}

void MainWindow::onConversionFinished(bool success, const QString &message)
{
    m_conversionInProgress = false;
    m_progressBar->setVisible(false);
    
    if (success) {
        m_statusLabel->setText("转换完成");
        QMessageBox::information(this, "成功", message);
    } else {
        m_statusLabel->setText("转换失败");
        QMessageBox::warning(this, "错误", message);
    }
    
    // 3秒后恢复就绪状态
    QTimer::singleShot(3000, [this]() {
        if (!m_conversionInProgress) {
            m_statusLabel->setText("就绪");
        }
    });
}

void MainWindow::onConfigChanged()
{
    // 配置改变后重新检查服务器状态
    QTimer::singleShot(1000, this, &MainWindow::checkServerStatus);
}

void MainWindow::showAbout()
{
    QMessageBox::about(this, "关于 Markdown转Word工具",
        "<h3>Markdown转Word工具 v1.0.0</h3>"
        "<p>这是一个简单易用的Markdown到DOCX格式转换工具。</p>"
        "<p><b>主要功能:</b></p>"
        "<ul>"
        "<li>单文件转换</li>"
        "<li>批量文件转换</li>"
        "<li>可配置的转换参数</li>"
        "<li>支持自定义模板</li>"
        "</ul>"
        "<p><b>技术栈:</b></p>"
        "<ul>"
        "<li>前端: Qt框架</li>"
        "<li>后端: Go语言</li>"
        "<li>转换引擎: Pandoc</li>"
        "</ul>"
        "<p>Copyright © 2024 MD2DOCX</p>");
}

void MainWindow::showHelp()
{
    QString helpText = 
        "<h3>使用帮助</h3>"
        "<h4>单文件转换:</h4>"
        "<ol>"
        "<li>点击'单文件转换'标签页</li>"
        "<li>选择要转换的Markdown文件</li>"
        "<li>可选择输出目录和文件名</li>"
        "<li>点击'开始转换'按钮</li>"
        "</ol>"
        "<h4>批量转换:</h4>"
        "<ol>"
        "<li>点击'批量转换'标签页</li>"
        "<li>选择多个Markdown文件</li>"
        "<li>可选择统一的输出目录</li>"
        "<li>点击'开始转换'按钮</li>"
        "</ol>"
        "<h4>配置设置:</h4>"
        "<ol>"
        "<li>点击'设置'标签页</li>"
        "<li>配置Pandoc路径（通常会自动检测）</li>"
        "<li>可选择参考模板文件</li>"
        "<li>点击'保存配置'按钮</li>"
        "</ol>"
        "<p><b>注意:</b> 使用前请确保系统已安装Pandoc。</p>";
    
    QMessageBox::information(this, "使用帮助", helpText);
}
