#include "mainwindow_integrated.h"
#include "singlefileconverter.h"
#include "multifileconverter.h"
#include "settingswidget.h"
#include "aboutwidget.h"
#include "httpapi.h"
#include "embeddedserver.h"

#include <QApplication>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QMenuBar>
#include <QStatusBar>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QCloseEvent>
#include <QTimer>
#include <QSplashScreen>
#include <QPixmap>

MainWindowIntegrated::MainWindowIntegrated(QWidget *parent)
    : QMainWindow(parent)
    , m_tabWidget(nullptr)
    , m_singleConverter(nullptr)
    , m_multiConverter(nullptr)
    , m_settingsWidget(nullptr)
    , m_aboutWidget(nullptr)
    , m_statusLabel(nullptr)
    , m_serverStatusLabel(nullptr)
    , m_progressBar(nullptr)
    , m_systemTray(nullptr)
    , m_trayMenu(nullptr)
    , m_showAction(nullptr)
    , m_quitAction(nullptr)
    , m_embeddedServer(nullptr)
    , m_httpApi(nullptr)
    , m_serverRunning(false)
    , m_serverHealthy(false)
    , m_startupInProgress(true)
    , m_statusUpdateTimer(nullptr)
{
    setWindowTitle("Markdown转Word工具");
    setWindowIcon(QIcon(":/icons/app.png"));
    resize(900, 700);
    
    // 显示启动进度
    showServerStartupProgress();
    
    // 设置UI
    setupUI();
    setupMenuBar();
    setupStatusBar();
    setupSystemTray();
    setupConnections();
    
    // 启动嵌入式服务器
    startEmbeddedServer();
    
    // 状态更新定时器
    m_statusUpdateTimer = new QTimer(this);
    connect(m_statusUpdateTimer, &QTimer::timeout, this, &MainWindowIntegrated::updateStatusBar);
    m_statusUpdateTimer->start(1000); // 每秒更新一次
}

MainWindowIntegrated::~MainWindowIntegrated()
{
    stopEmbeddedServer();
}

void MainWindowIntegrated::setupUI()
{
    // 创建中央部件
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    
    // 创建标签页
    m_tabWidget = new QTabWidget(this);
    mainLayout->addWidget(m_tabWidget);
    
    // 创建HTTP API客户端
    m_httpApi = new HttpApi(this);
    
    // 创建各个页面
    m_singleConverter = new SingleFileConverter(m_httpApi, this);
    m_multiConverter = new MultiFileConverter(m_httpApi, this);
    m_settingsWidget = new SettingsWidget(m_httpApi, this);
    m_aboutWidget = new AboutWidget(this);
    
    // 添加标签页
    m_tabWidget->addTab(m_singleConverter, "单文件转换");
    m_tabWidget->addTab(m_multiConverter, "多文件转换");
    m_tabWidget->addTab(m_settingsWidget, "设置");
    m_tabWidget->addTab(m_aboutWidget, "关于");
}

void MainWindowIntegrated::setupMenuBar()
{
    // 文件菜单
    QMenu *fileMenu = menuBar()->addMenu("文件(&F)");
    
    QAction *exitAction = new QAction("退出(&X)", this);
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &MainWindowIntegrated::quitApplication);
    fileMenu->addAction(exitAction);
    
    // 工具菜单
    QMenu *toolsMenu = menuBar()->addMenu("工具(&T)");
    
    QAction *settingsAction = new QAction("设置(&S)", this);
    connect(settingsAction, &QAction::triggered, [this]() {
        m_tabWidget->setCurrentWidget(m_settingsWidget);
    });
    toolsMenu->addAction(settingsAction);
    
    // 帮助菜单
    QMenu *helpMenu = menuBar()->addMenu("帮助(&H)");
    
    QAction *aboutAction = new QAction("关于(&A)", this);
    connect(aboutAction, &QAction::triggered, this, &MainWindowIntegrated::aboutApplication);
    helpMenu->addAction(aboutAction);
}

void MainWindowIntegrated::setupStatusBar()
{
    m_statusLabel = new QLabel("准备就绪", this);
    statusBar()->addWidget(m_statusLabel);
    
    statusBar()->addPermanentWidget(new QLabel(" | "));
    
    m_serverStatusLabel = new QLabel("服务器启动中...", this);
    statusBar()->addPermanentWidget(m_serverStatusLabel);
    
    m_progressBar = new QProgressBar(this);
    m_progressBar->setVisible(false);
    statusBar()->addPermanentWidget(m_progressBar);
}

void MainWindowIntegrated::setupSystemTray()
{
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        return;
    }
    
    // 创建系统托盘图标
    m_systemTray = new QSystemTrayIcon(this);
    m_systemTray->setIcon(QIcon(":/icons/app.png"));
    m_systemTray->setToolTip("Markdown转Word工具");
    
    // 创建托盘菜单
    m_trayMenu = new QMenu(this);
    
    m_showAction = new QAction("显示主窗口", this);
    connect(m_showAction, &QAction::triggered, this, &MainWindowIntegrated::showMainWindow);
    m_trayMenu->addAction(m_showAction);
    
    m_trayMenu->addSeparator();
    
    m_quitAction = new QAction("退出", this);
    connect(m_quitAction, &QAction::triggered, this, &MainWindowIntegrated::quitApplication);
    m_trayMenu->addAction(m_quitAction);
    
    m_systemTray->setContextMenu(m_trayMenu);
    
    // 双击托盘图标显示主窗口
    connect(m_systemTray, &QSystemTrayIcon::activated, [this](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::DoubleClick) {
            showMainWindow();
        }
    });
    
    m_systemTray->show();
}

void MainWindowIntegrated::setupConnections()
{
    // 连接嵌入式服务器信号
    if (m_embeddedServer) {
        connect(m_embeddedServer, &EmbeddedServer::serverStarted, 
                this, &MainWindowIntegrated::onServerStarted);
        connect(m_embeddedServer, &EmbeddedServer::serverStopped, 
                this, &MainWindowIntegrated::onServerStopped);
        connect(m_embeddedServer, &EmbeddedServer::serverError, 
                this, &MainWindowIntegrated::onServerError);
        connect(m_embeddedServer, &EmbeddedServer::healthCheckResult, 
                this, &MainWindowIntegrated::onServerHealthChanged);
    }
}

void MainWindowIntegrated::startEmbeddedServer()
{
    if (!m_embeddedServer) {
        m_embeddedServer = new EmbeddedServer(this);
        setupConnections();
    }
    
    if (!m_embeddedServer->startServer()) {
        QMessageBox::critical(this, "错误", "无法启动内嵌服务器！\n请检查程序文件是否完整。");
        QApplication::quit();
        return;
    }
}

void MainWindowIntegrated::stopEmbeddedServer()
{
    if (m_embeddedServer) {
        m_embeddedServer->stopServer();
    }
}

void MainWindowIntegrated::onServerStarted()
{
    m_serverRunning = true;
    
    // 更新HTTP API的服务器地址
    if (m_httpApi && m_embeddedServer) {
        m_httpApi->setServerUrl(m_embeddedServer->serverUrl());
    }
    
    updateServerStatus();
    hideServerStartupProgress();
    
    // 显示成功消息
    if (m_systemTray) {
        m_systemTray->showMessage("Markdown转Word工具", "服务器启动成功！", 
                                 QSystemTrayIcon::Information, 3000);
    }
}

void MainWindowIntegrated::onServerStopped()
{
    m_serverRunning = false;
    m_serverHealthy = false;
    updateServerStatus();
}

void MainWindowIntegrated::onServerError(const QString &error)
{
    QMessageBox::critical(this, "服务器错误", QString("服务器发生错误：\n%1").arg(error));
    updateServerStatus();
}

void MainWindowIntegrated::onServerHealthChanged(bool isHealthy)
{
    m_serverHealthy = isHealthy;
    updateServerStatus();
}

void MainWindowIntegrated::showMainWindow()
{
    show();
    raise();
    activateWindow();
}

void MainWindowIntegrated::quitApplication()
{
    stopEmbeddedServer();
    QApplication::quit();
}

void MainWindowIntegrated::aboutApplication()
{
    m_tabWidget->setCurrentWidget(m_aboutWidget);
}

void MainWindowIntegrated::updateServerStatus()
{
    if (!m_serverStatusLabel) return;
    
    if (m_startupInProgress) {
        m_serverStatusLabel->setText("服务器启动中...");
        m_serverStatusLabel->setStyleSheet("color: orange;");
    } else if (m_serverRunning && m_serverHealthy) {
        m_serverStatusLabel->setText("服务器运行正常");
        m_serverStatusLabel->setStyleSheet("color: green;");
    } else if (m_serverRunning && !m_serverHealthy) {
        m_serverStatusLabel->setText("服务器连接异常");
        m_serverStatusLabel->setStyleSheet("color: orange;");
    } else {
        m_serverStatusLabel->setText("服务器已停止");
        m_serverStatusLabel->setStyleSheet("color: red;");
    }
}

void MainWindowIntegrated::updateStatusBar()
{
    if (m_statusLabel) {
        QString status = "准备就绪";
        if (m_startupInProgress) {
            status = "正在启动服务器...";
        } else if (m_serverRunning && m_serverHealthy) {
            status = "准备就绪";
        } else if (!m_serverRunning) {
            status = "服务器未运行";
        }
        m_statusLabel->setText(status);
    }
}

void MainWindowIntegrated::showServerStartupProgress()
{
    if (m_progressBar) {
        m_progressBar->setRange(0, 0); // 不确定进度
        m_progressBar->setVisible(true);
    }
}

void MainWindowIntegrated::hideServerStartupProgress()
{
    m_startupInProgress = false;
    if (m_progressBar) {
        m_progressBar->setVisible(false);
    }
}

void MainWindowIntegrated::closeEvent(QCloseEvent *event)
{
    if (m_systemTray && m_systemTray->isVisible()) {
        // 最小化到系统托盘
        hide();
        if (m_systemTray) {
            m_systemTray->showMessage("Markdown转Word工具", "程序已最小化到系统托盘", 
                                     QSystemTrayIcon::Information, 2000);
        }
        event->ignore();
    } else {
        // 直接退出
        quitApplication();
        event->accept();
    }
}

void MainWindowIntegrated::changeEvent(QEvent *event)
{
    QMainWindow::changeEvent(event);
    
    if (event->type() == QEvent::WindowStateChange) {
        if (isMinimized() && m_systemTray && m_systemTray->isVisible()) {
            hide();
            event->ignore();
        }
    }
}
