#include <QApplication>
#include <QMainWindow>
#include <QMessageBox>
#include <QStatusBar>
#include <QTabWidget>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include "httpapi.h"
#include "singleconverter.h"

class TestMainWindow : public QMainWindow {
  Q_OBJECT

public:
  TestMainWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
    setWindowTitle("Markdown转Word工具 - 单文件转换测试");
    setMinimumSize(800, 600);

    // 创建HTTP API客户端
    m_httpApi = new HttpApi(this);
    m_httpApi->setServerUrl("http://localhost:8080");

    // 创建中央窗口部件
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // 创建布局
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    // 创建标签页
    QTabWidget *tabWidget = new QTabWidget(this);

    // 创建单文件转换页面
    m_singleConverter = new SingleConverter(m_httpApi, this);
    tabWidget->addTab(m_singleConverter, "单文件转换");

    layout->addWidget(tabWidget);

    // 连接信号
    connect(m_httpApi, &HttpApi::healthCheckFinished, this,
            &TestMainWindow::onHealthCheckFinished);
    connect(m_httpApi, &HttpApi::errorOccurred, this,
            &TestMainWindow::onApiError);

    // 启动时检查后端服务
    QTimer::singleShot(1000, this, &TestMainWindow::checkBackendService);
  }

private slots:
  void checkBackendService() {
    statusBar()->showMessage("正在检查后端服务...");
    m_httpApi->checkHealth();
  }

  void onHealthCheckFinished(bool isOnline) {
    if (isOnline) {
      statusBar()->showMessage("后端服务连接正常", 3000);
      m_singleConverter->setEnabled(true);
    } else {
      statusBar()->showMessage("后端服务连接失败 - 请确保Go服务器正在运行", 0);
      m_singleConverter->setEnabled(false);

      QMessageBox::warning(this, "连接失败",
                           "无法连接到后端服务 (http://localhost:8080)\n\n"
                           "请确保Go后端服务器正在运行:\n"
                           "cd md2docx-src && go run cmd/server/main.go");
    }
  }

  void onApiError(const QString &error) {
    statusBar()->showMessage(QString("API错误: %1").arg(error), 5000);
  }

private:
  HttpApi *m_httpApi;
  SingleConverter *m_singleConverter;
};

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  // 设置应用程序信息
  app.setApplicationName("Markdown转Word工具");
  app.setApplicationVersion("1.0.0");
  app.setOrganizationName("MD2DOCX");

  // 创建主窗口
  TestMainWindow window;
  window.show();

  return app.exec();
}

#include "main_single_test.moc"
