#include "aboutwidget.h"
#include "appsettings.h"
#include "embeddedserver.h"
#include "httpapi.h"
#include "multifileconverter.h"
#include "settingswidget.h"
#include "singlefileconverter.h"

#include <QApplication>
#include <QCloseEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>
#include <QProgressBar>
#include <QSplashScreen>
#include <QStatusBar>
#include <QTabWidget>
#include <QTimer>
#include <QVBoxLayout>

/**
 * 简单整合版主窗口
 * 内嵌Go后端服务，实现单一程序运行
 */
class SimpleIntegratedMainWindow : public QMainWindow {
  Q_OBJECT

public:
  SimpleIntegratedMainWindow(QWidget *parent = nullptr)
      : QMainWindow(parent), m_embeddedServer(nullptr), m_httpApi(nullptr),
        m_tabWidget(nullptr), m_statusLabel(nullptr),
        m_serverStatusLabel(nullptr), m_progressBar(nullptr),
        m_serverRunning(false) {
#ifdef APP_VERSION
    setWindowTitle(QString("Markdown转Word工具 - 整合版 v%1").arg(APP_VERSION));
#else
    setWindowTitle("Markdown转Word工具 - 整合版");
#endif
    resize(900, 700);

    setupUI();
    setupStatusBar();
    startEmbeddedServer();
  }

  ~SimpleIntegratedMainWindow() { stopEmbeddedServer(); }

protected:
  void closeEvent(QCloseEvent *event) override {
    stopEmbeddedServer();
    event->accept();
  }

private slots:
  void onServerStarted() {
    m_serverRunning = true;
    updateServerStatus();

    // 更新HTTP API的服务器地址
    if (m_httpApi && m_embeddedServer) {
      m_httpApi->setServerUrl(m_embeddedServer->serverUrl());
    }

    // 隐藏进度条
    if (m_progressBar) {
      m_progressBar->setVisible(false);
    }

    // 服务器启动成功，不显示提示框，静默启动
  }

  void onServerStopped() {
    m_serverRunning = false;
    updateServerStatus();
  }

  void onServerError(const QString &error) {
    QMessageBox::critical(this, "服务器错误",
                          QString("服务器发生错误：\n%1").arg(error));
    updateServerStatus();
  }

  void onServerHealthChanged(bool isHealthy) {
    Q_UNUSED(isHealthy)
    updateServerStatus();
  }

private:
  void setupUI() {
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
    SingleFileConverter *singleConverter =
        new SingleFileConverter(m_httpApi, this);
    MultiFileConverter *multiConverter =
        new MultiFileConverter(m_httpApi, this);
    SettingsWidget *settingsWidget = new SettingsWidget(m_httpApi, this);
    AboutWidget *aboutWidget = new AboutWidget(this);

    // 添加标签页
    m_tabWidget->addTab(singleConverter, "单文件转换");
    m_tabWidget->addTab(multiConverter, "多文件转换");
    m_tabWidget->addTab(settingsWidget, "设置");
    m_tabWidget->addTab(aboutWidget, "关于");
  }

  void setupStatusBar() {
    m_statusLabel = new QLabel("正在启动服务器...", this);
    statusBar()->addWidget(m_statusLabel);

    statusBar()->addPermanentWidget(new QLabel(" | "));

    m_serverStatusLabel = new QLabel("服务器启动中...", this);
    statusBar()->addPermanentWidget(m_serverStatusLabel);

    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 0); // 不确定进度
    statusBar()->addPermanentWidget(m_progressBar);
  }

  void startEmbeddedServer() {
    if (!m_embeddedServer) {
      m_embeddedServer = new EmbeddedServer(this);

      // 连接信号
      connect(m_embeddedServer, &EmbeddedServer::serverStarted, this,
              &SimpleIntegratedMainWindow::onServerStarted);
      connect(m_embeddedServer, &EmbeddedServer::serverStopped, this,
              &SimpleIntegratedMainWindow::onServerStopped);
      connect(m_embeddedServer, &EmbeddedServer::serverError, this,
              &SimpleIntegratedMainWindow::onServerError);
      connect(m_embeddedServer, &EmbeddedServer::healthCheckResult, this,
              &SimpleIntegratedMainWindow::onServerHealthChanged);
    }

    if (!m_embeddedServer->startServer()) {
      QMessageBox::critical(this, "错误",
                            "无法启动内嵌服务器！\n请检查程序文件是否完整。");
      QApplication::quit();
    }
  }

  void stopEmbeddedServer() {
    if (m_embeddedServer) {
      m_embeddedServer->stopServer();
    }
  }

  void updateServerStatus() {
    if (!m_serverStatusLabel)
      return;

    if (m_serverRunning) {
      m_serverStatusLabel->setText("服务器运行正常");
      m_serverStatusLabel->setStyleSheet("color: green;");
      m_statusLabel->setText("准备就绪");
    } else {
      m_serverStatusLabel->setText("服务器已停止");
      m_serverStatusLabel->setStyleSheet("color: red;");
      m_statusLabel->setText("服务器未运行");
    }
  }

private:
  EmbeddedServer *m_embeddedServer;
  HttpApi *m_httpApi;
  QTabWidget *m_tabWidget;
  QLabel *m_statusLabel;
  QLabel *m_serverStatusLabel;
  QProgressBar *m_progressBar;
  bool m_serverRunning;
};

// 显示启动画面
QSplashScreen *showSplashScreen() {
  QPixmap pixmap(400, 200);
  pixmap.fill(QColor(45, 45, 45));

  QPainter painter(&pixmap);
  painter.setRenderHint(QPainter::Antialiasing);

  // 绘制标题
  painter.setPen(Qt::white);
  painter.setFont(QFont("Arial", 20, QFont::Bold));
  painter.drawText(pixmap.rect(), Qt::AlignCenter,
                   "Markdown转Word工具\n整合版");

  // 绘制状态信息
  painter.setFont(QFont("Arial", 10));
  painter.drawText(QRect(0, pixmap.height() - 30, pixmap.width(), 30),
                   Qt::AlignCenter, "正在启动内嵌服务器...");

  QSplashScreen *splash = new QSplashScreen(pixmap);
  splash->show();

  return splash;
}

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  // 设置应用程序信息
  app.setApplicationName("Markdown转Word工具");
#ifdef APP_VERSION
  app.setApplicationVersion(APP_VERSION);
#else
  app.setApplicationVersion("dev");
#endif
  app.setOrganizationName("MD2DOCX");

  // 显示启动画面
  QSplashScreen *splash = showSplashScreen();
  app.processEvents();

  // 创建主窗口
  SimpleIntegratedMainWindow mainWindow;

  // 延迟显示主窗口
  QTimer::singleShot(1500, [&]() {
    splash->finish(&mainWindow);
    mainWindow.show();
    delete splash;
  });

  return app.exec();
}

#include "main_simple_integrated.moc"
