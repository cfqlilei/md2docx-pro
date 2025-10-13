#include "mainwindow_md2docx.h"
#include "aboutwidget.h"
#include "httpapi.h"
#include "multifileconverter.h"
#include "settingswidget.h"
#include "singlefileconverter.h"

#include <QAction>
#include <QApplication>
#include <QLabel>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QTabWidget>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

MainWindowMd2Docx::MainWindowMd2Docx(QWidget *parent)
    : QMainWindow(parent), m_tabWidget(nullptr), m_singleFileConverter(nullptr),
      m_multiFileConverter(nullptr), m_settingsWidget(nullptr),
      m_aboutWidget(nullptr), m_httpApi(nullptr), m_connectionTimer(nullptr) {
  setupUI();
  setupHttpApi();
  setupConnections();

  // 延迟检查后端连接
  QTimer::singleShot(1000, this, &MainWindowMd2Docx::checkBackendConnection);
}

MainWindowMd2Docx::~MainWindowMd2Docx() {}

void MainWindowMd2Docx::setupUI() {
  setWindowTitle("Markdown转Word工具 v1.0.0");
  setMinimumSize(900, 700);
  resize(1100, 800);

  // 创建中央标签页控件
  m_tabWidget = new QTabWidget(this);
  setCentralWidget(m_tabWidget);

  // 创建状态栏
  statusBar()->showMessage("正在初始化...");

  // 创建菜单栏
  setupMenuBar();

  // 创建页签组件（先创建但不添加到标签页，等API连接成功后再添加）
  // 这样可以避免在后端未连接时用户进行操作
}

void MainWindowMd2Docx::setupMenuBar() {
  QMenuBar *menuBar = this->menuBar();

  // 文件菜单
  QMenu *fileMenu = menuBar->addMenu("文件(&F)");

  QAction *exitAction = fileMenu->addAction("退出(&X)");
  exitAction->setShortcut(QKeySequence::Quit);
  connect(exitAction, &QAction::triggered, this, &QWidget::close);

  // 工具菜单
  QMenu *toolsMenu = menuBar->addMenu("工具(&T)");

  QAction *checkConnectionAction = toolsMenu->addAction("检查后端连接(&C)");
  connect(checkConnectionAction, &QAction::triggered, this,
          &MainWindowMd2Docx::checkBackendConnection);

  // 帮助菜单
  QMenu *helpMenu = menuBar->addMenu("帮助(&H)");

  QAction *aboutAction = helpMenu->addAction("关于(&A)");
  connect(aboutAction, &QAction::triggered, this,
          &MainWindowMd2Docx::showAbout);
}

void MainWindowMd2Docx::setupHttpApi() {
  m_httpApi = new HttpApi(this);

  // 连接健康检查信号
  connect(m_httpApi, &HttpApi::healthCheckFinished, this,
          &MainWindowMd2Docx::onHealthCheckFinished);
}

void MainWindowMd2Docx::setupConnections() {
  // 设置定时器定期检查连接状态
  m_connectionTimer = new QTimer(this);
  m_connectionTimer->setInterval(30000); // 30秒检查一次
  connect(m_connectionTimer, &QTimer::timeout, this,
          &MainWindowMd2Docx::checkBackendConnection);
}

void MainWindowMd2Docx::checkBackendConnection() {
  statusBar()->showMessage("正在检查后端连接...");
  if (m_httpApi) {
    m_httpApi->checkHealth();
  }
}

void MainWindowMd2Docx::onHealthCheckFinished(bool success) {
  if (success) {
    statusBar()->showMessage("后端连接正常 - 所有功能可用", 3000);
    setTabsEnabled(true);

    // 如果页签还没有创建，现在创建它们
    if (m_tabWidget->count() == 0) {
      // 创建单文件转换页签
      m_singleFileConverter = new SingleFileConverter(m_httpApi, this);
      m_tabWidget->addTab(m_singleFileConverter, "单文件转换");

      // 创建多文件转换页签
      m_multiFileConverter = new MultiFileConverter(m_httpApi, this);
      m_tabWidget->addTab(m_multiFileConverter, "多文件转换");

      // 创建设置页签
      m_settingsWidget = new SettingsWidget(m_httpApi, this);
      m_tabWidget->addTab(m_settingsWidget, "设置");

      // 创建关于页签
      m_aboutWidget = new AboutWidget(this);
      m_tabWidget->addTab(m_aboutWidget, "关于");

      // 连接转换信号
      connect(m_singleFileConverter, &SingleFileConverter::conversionStarted,
              this, &MainWindowMd2Docx::onConversionStarted);
      connect(m_singleFileConverter, &SingleFileConverter::conversionFinished,
              this, &MainWindowMd2Docx::onConversionFinished);

      connect(m_multiFileConverter, &MultiFileConverter::conversionStarted,
              this, &MainWindowMd2Docx::onConversionStarted);
      connect(m_multiFileConverter, &MultiFileConverter::conversionFinished,
              this, &MainWindowMd2Docx::onConversionFinished);

      connect(m_settingsWidget, &SettingsWidget::configChanged, this,
              &MainWindowMd2Docx::onConfigChanged);
    }

    // 启动定期检查
    if (!m_connectionTimer->isActive()) {
      m_connectionTimer->start();
    }
  } else {
    statusBar()->showMessage("后端连接失败 - 请启动Go服务器", 5000);
    setTabsEnabled(false);

    // 显示连接失败提示
    if (m_tabWidget->count() == 0) {
      QWidget *placeholderWidget = new QWidget(this);
      QVBoxLayout *layout = new QVBoxLayout(placeholderWidget);

      QLabel *titleLabel = new QLabel("后端服务未连接", placeholderWidget);
      titleLabel->setAlignment(Qt::AlignCenter);
      titleLabel->setStyleSheet(
          "font-size: 18px; font-weight: bold; color: #d32f2f; margin: 20px;");

      QLabel *messageLabel =
          new QLabel("无法连接到后端服务器。\n\n"
                     "请确保Go后端服务正在运行：\n"
                     "1. 打开终端\n"
                     "2. 进入项目目录\n"
                     "3. 运行: ./build/md2docx-server-macos\n"
                     "   或使用: node scripts/launch_complete_app_macos.js\n\n"
                     "服务启动后，点击菜单 \"工具 -> 检查后端连接\" 重新连接。",
                     placeholderWidget);
      messageLabel->setAlignment(Qt::AlignCenter);
      messageLabel->setStyleSheet(
          "font-size: 14px; color: #666; line-height: 1.5;");

      layout->addStretch();
      layout->addWidget(titleLabel);
      layout->addWidget(messageLabel);
      layout->addStretch();

      m_tabWidget->addTab(placeholderWidget, "连接状态");
    }

    // 停止定期检查，改为手动检查
    if (m_connectionTimer->isActive()) {
      m_connectionTimer->stop();
    }
  }
}

void MainWindowMd2Docx::onConversionStarted() {
  statusBar()->showMessage("转换进行中...");
}

void MainWindowMd2Docx::onConversionFinished(bool success,
                                             const QString &message) {
  if (success) {
    statusBar()->showMessage("转换完成", 3000);
  } else {
    statusBar()->showMessage(QString("转换失败: %1").arg(message), 5000);
  }
}

void MainWindowMd2Docx::onConfigChanged() {
  statusBar()->showMessage("配置已更改", 2000);
}

void MainWindowMd2Docx::showAbout() {
  QMessageBox::about(this, "关于",
                     "Markdown转Word工具\n\n"
                     "功能特性：\n"
                     "• 单文件转换 - 转换单个Markdown文件为Word文档\n"
                     "• 批量转换 - 同时转换多个Markdown文件\n"
                     "• 自定义设置 - 配置Pandoc路径和转换模板\n"
                     "• 实时状态 - 显示转换进度和结果\n\n"
                     "技术栈: Qt 5.15 + Go 1.25 + Pandoc\n"
                     "版本: 2.0.0\n\n"
                     "使用说明：\n"
                     "1. 确保后端服务正在运行\n"
                     "2. 在设置页签中配置Pandoc路径\n"
                     "3. 选择要转换的Markdown文件\n"
                     "4. 点击转换按钮开始转换");
}

void MainWindowMd2Docx::setTabsEnabled(bool enabled) {
  if (m_singleFileConverter) {
    m_singleFileConverter->setEnabled(enabled);
  }
  if (m_multiFileConverter) {
    m_multiFileConverter->setEnabled(enabled);
  }
  if (m_settingsWidget) {
    m_settingsWidget->setEnabled(enabled);
  }
}
