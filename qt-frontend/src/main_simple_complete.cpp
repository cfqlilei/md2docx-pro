#include <QApplication>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QStatusBar>
#include <QTabWidget>
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include "httpapi.h"
#include "simple_batchconverter.h"
#include "singleconverter.h"

// 简化的配置管理器
class SimpleConfigManager : public QWidget {
  Q_OBJECT

public:
  explicit SimpleConfigManager(HttpApi *api, QWidget *parent = nullptr)
      : QWidget(parent), m_httpApi(api) {
    setupUI();
    setupConnections();
  }

signals:
  void configChanged();

private slots:
  void loadConfig() {
    m_statusEdit->append("正在加载配置...");
    m_httpApi->getConfig();
  }

  void saveConfig() { m_statusEdit->append("保存配置功能开发中..."); }

  void onConfigReceived(const ConfigData &config) {
    m_pandocPathEdit->setText(config.pandocPath);
    m_templateFileEdit->setText(config.templateFile);
    m_statusEdit->append(
        QString("配置加载完成 - Pandoc路径: %1").arg(config.pandocPath));
    emit configChanged();
  }

private:
  void setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Pandoc配置组
    QGroupBox *pandocGroup = new QGroupBox("Pandoc配置", this);
    QGridLayout *pandocLayout = new QGridLayout(pandocGroup);

    pandocLayout->addWidget(new QLabel("Pandoc路径:"), 0, 0);
    m_pandocPathEdit = new QLineEdit(this);
    m_pandocPathEdit->setPlaceholderText("Pandoc可执行文件路径（自动检测）");
    m_pandocPathEdit->setReadOnly(true);
    pandocLayout->addWidget(m_pandocPathEdit, 0, 1);

    mainLayout->addWidget(pandocGroup);

    // 模板配置组
    QGroupBox *templateGroup = new QGroupBox("模板配置", this);
    QGridLayout *templateLayout = new QGridLayout(templateGroup);

    templateLayout->addWidget(new QLabel("参考模板:"), 0, 0);
    m_templateFileEdit = new QLineEdit(this);
    m_templateFileEdit->setPlaceholderText("可选：DOCX模板文件路径");
    m_templateFileEdit->setReadOnly(true);
    templateLayout->addWidget(m_templateFileEdit, 0, 1);

    mainLayout->addWidget(templateGroup);

    // 操作按钮
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    QPushButton *loadButton = new QPushButton("加载配置", this);
    connect(loadButton, &QPushButton::clicked, this,
            &SimpleConfigManager::loadConfig);
    buttonLayout->addWidget(loadButton);

    QPushButton *saveButton = new QPushButton("保存配置", this);
    connect(saveButton, &QPushButton::clicked, this,
            &SimpleConfigManager::saveConfig);
    buttonLayout->addWidget(saveButton);

    buttonLayout->addStretch();
    mainLayout->addLayout(buttonLayout);

    // 状态显示区域
    QGroupBox *statusGroup = new QGroupBox("配置状态", this);
    QVBoxLayout *statusLayout = new QVBoxLayout(statusGroup);

    m_statusEdit = new QTextEdit(this);
    m_statusEdit->setReadOnly(true);
    m_statusEdit->setMaximumHeight(150);
    m_statusEdit->setPlaceholderText("配置状态和信息将在这里显示...");
    statusLayout->addWidget(m_statusEdit);

    mainLayout->addWidget(statusGroup);

    // 添加弹性空间
    mainLayout->addStretch();
  }

  void setupConnections() {
    connect(m_httpApi, &HttpApi::configReceived, this,
            &SimpleConfigManager::onConfigReceived);
  }

private:
  HttpApi *m_httpApi;
  QLineEdit *m_pandocPathEdit;
  QLineEdit *m_templateFileEdit;
  QTextEdit *m_statusEdit;
};

class CompleteTestMainWindow : public QMainWindow {
  Q_OBJECT

public:
  CompleteTestMainWindow(QWidget *parent = nullptr)
      : QMainWindow(parent), m_httpApi(nullptr), m_tabWidget(nullptr),
        m_singleConverter(nullptr), m_batchConverter(nullptr),
        m_configManager(nullptr) {
    setupUI();
    setupHttpApi();
    setupConnections();

    // 延迟检查后端连接
    QTimer::singleShot(1000, this,
                       &CompleteTestMainWindow::checkBackendConnection);
  }

private slots:
  void checkBackendConnection() {
    statusBar()->showMessage("正在检查后端连接...");
    m_httpApi->checkHealth();
  }

  void onHealthCheckFinished(bool success) {
    if (success) {
      statusBar()->showMessage("后端连接正常 - 所有功能可用", 3000);
      setTabsEnabled(true);
    } else {
      statusBar()->showMessage("后端连接失败 - 请启动Go服务器", 5000);
      setTabsEnabled(false);

      QMessageBox::warning(this, "后端连接失败",
                           "无法连接到后端服务器。\n\n"
                           "请确保Go后端服务正在运行：\n"
                           "cd md2docx-src && go run cmd/server/main.go");
    }
  }

  void onConversionStarted() { statusBar()->showMessage("转换进行中..."); }

  void onConversionFinished(bool success, const QString &message) {
    if (success) {
      statusBar()->showMessage("转换完成", 3000);
    } else {
      statusBar()->showMessage(QString("转换失败: %1").arg(message), 5000);
    }
  }

  void onConfigChanged() { statusBar()->showMessage("配置已更改", 2000); }

private:
  void setupUI() {
    setWindowTitle("Markdown转Word工具 - 完整功能测试");
    setMinimumSize(800, 600);
    resize(1000, 700);

    // 创建中央标签页控件
    m_tabWidget = new QTabWidget(this);
    setCentralWidget(m_tabWidget);

    // 创建状态栏
    statusBar()->showMessage("正在初始化...");

    // 创建菜单栏
    setupMenuBar();
  }

  void setupMenuBar() {
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
            &CompleteTestMainWindow::checkBackendConnection);

    // 帮助菜单
    QMenu *helpMenu = menuBar->addMenu("帮助(&H)");

    QAction *aboutAction = helpMenu->addAction("关于(&A)");
    connect(aboutAction, &QAction::triggered, this, [this]() {
      QMessageBox::about(this, "关于",
                         "Markdown转Word工具\n\n"
                         "技术栈: Qt 5.15 + Go 1.25 + Pandoc\n"
                         "功能: 单文件转换、批量转换、配置管理\n\n"
                         "版本: 1.0.0");
    });
  }

  void setupHttpApi() {
    m_httpApi = new HttpApi(this);

    // 连接健康检查信号
    connect(m_httpApi, &HttpApi::healthCheckFinished, this,
            &CompleteTestMainWindow::onHealthCheckFinished);
  }

  void setupConnections() {
    // 创建各个功能页面
    m_singleConverter = new SingleConverter(m_httpApi, this);
    m_batchConverter = new SimpleBatchConverter(m_httpApi, this);
    m_configManager = new SimpleConfigManager(m_httpApi, this);

    // 添加到标签页 - 这里是关键的标签页结构
    m_tabWidget->addTab(m_singleConverter, "单文件转换");
    m_tabWidget->addTab(m_batchConverter, "多文件转换");
    m_tabWidget->addTab(m_configManager, "设置");

    // 连接转换状态信号
    connect(m_singleConverter, &SingleConverter::conversionStarted, this,
            &CompleteTestMainWindow::onConversionStarted);
    connect(m_singleConverter, &SingleConverter::conversionFinished, this,
            &CompleteTestMainWindow::onConversionFinished);

    connect(m_batchConverter, &SimpleBatchConverter::conversionStarted, this,
            &CompleteTestMainWindow::onConversionStarted);
    connect(m_batchConverter, &SimpleBatchConverter::conversionFinished, this,
            &CompleteTestMainWindow::onConversionFinished);

    // 连接配置变更信号
    connect(m_configManager, &SimpleConfigManager::configChanged, this,
            &CompleteTestMainWindow::onConfigChanged);

    // 初始状态下禁用所有标签页
    setTabsEnabled(false);
  }

  void setTabsEnabled(bool enabled) {
    for (int i = 0; i < m_tabWidget->count(); ++i) {
      m_tabWidget->widget(i)->setEnabled(enabled);
    }

    if (!enabled) {
      m_tabWidget->setCurrentIndex(2); // 切换到设置页面
    }
  }

private:
  HttpApi *m_httpApi;
  QTabWidget *m_tabWidget;
  SingleConverter *m_singleConverter;
  SimpleBatchConverter *m_batchConverter;
  SimpleConfigManager *m_configManager;
};

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  // 设置应用程序信息
  app.setApplicationName("Markdown转Word工具");
  app.setApplicationVersion("1.0.0");
  app.setOrganizationName("MD2DOCX");

  CompleteTestMainWindow window;
  window.show();

  return app.exec();
}

#include "main_simple_complete.moc"
