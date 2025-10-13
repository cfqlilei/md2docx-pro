#ifndef EMBEDDEDSERVER_H
#define EMBEDDEDSERVER_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QProcess>
#include <QTimer>

/**
 * 嵌入式服务器类
 * 在Qt应用内部启动Go后端服务，实现单一程序运行
 */
class EmbeddedServer : public QObject {
  Q_OBJECT

public:
  explicit EmbeddedServer(QObject *parent = nullptr);
  ~EmbeddedServer();

  // 服务器控制
  bool startServer();
  void stopServer();
  bool isRunning() const { return m_serverRunning; }

  // 服务器信息
  QString serverUrl() const { return m_serverUrl; }
  int serverPort() const { return m_serverPort; }

  // 健康检查
  void checkHealth();

signals:
  void serverStarted();
  void serverStopped();
  void serverError(const QString &error);
  void healthCheckResult(bool isHealthy);

private slots:
  void onServerStarted();
  void onServerFinished(int exitCode, QProcess::ExitStatus exitStatus);
  void onServerError(QProcess::ProcessError error);
  void onHealthCheckFinished();
  void performHealthCheck();

private:
  void findAvailablePort();
  QString getServerExecutablePath();
  void setupHealthCheckTimer();
  void loadPortFromConfig();
  QString getConfigFilePath();
  bool isPortAvailable(int port);

  QProcess *m_serverProcess;
  QTimer *m_healthCheckTimer;
  QNetworkAccessManager *m_networkManager;

  QString m_serverUrl;
  int m_serverPort;
  bool m_serverRunning;
  bool m_serverHealthy;

  // 健康检查配置
  static const int HEALTH_CHECK_INTERVAL = 5000; // 5秒
  static const int STARTUP_TIMEOUT = 10000;      // 10秒启动超时
  static const int HEALTH_CHECK_TIMEOUT = 3000;  // 3秒健康检查超时
};

#endif // EMBEDDEDSERVER_H
