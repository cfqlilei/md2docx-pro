#include "embeddedserver.h"
#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QRandomGenerator>
#include <QStandardPaths>
#include <QTcpSocket>

EmbeddedServer::EmbeddedServer(QObject *parent)
    : QObject(parent), m_serverProcess(nullptr),
      m_healthCheckTimer(new QTimer(this)),
      m_networkManager(new QNetworkAccessManager(this)), m_serverPort(8080),
      m_serverRunning(false), m_serverHealthy(false) {
  // 首先尝试从配置文件读取端口
  loadPortFromConfig();

  // 如果配置文件中没有端口或端口不可用，查找可用端口
  if (m_serverPort == 0 || !isPortAvailable(m_serverPort)) {
    findAvailablePort();
  }

  m_serverUrl = QString("http://localhost:%1").arg(m_serverPort);

  // 设置健康检查定时器
  setupHealthCheckTimer();
}

EmbeddedServer::~EmbeddedServer() { stopServer(); }

bool EmbeddedServer::startServer() {
  if (m_serverRunning) {
    qDebug() << "服务器已经在运行";
    return true;
  }

  QString serverPath = getServerExecutablePath();
  if (serverPath.isEmpty()) {
    emit serverError("找不到服务器可执行文件");
    return false;
  }

  qDebug() << "启动嵌入式服务器:" << serverPath;
  qDebug() << "服务器端口:" << m_serverPort;

  // 创建服务器进程
  m_serverProcess = new QProcess(this);

  // 连接信号
  connect(m_serverProcess, &QProcess::started, this,
          &EmbeddedServer::onServerStarted);
  connect(m_serverProcess,
          QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this,
          &EmbeddedServer::onServerFinished);
  connect(m_serverProcess, &QProcess::errorOccurred, this,
          &EmbeddedServer::onServerError);

  // 设置环境变量
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  env.insert("SERVER_PORT", QString::number(m_serverPort));
  m_serverProcess->setProcessEnvironment(env);

  // 启动服务器
  QStringList arguments;
  if (serverPath.endsWith(".go")) {
    // 开发模式
    m_serverProcess->start("go", QStringList() << "run" << serverPath);
  } else {
    // 生产模式
    m_serverProcess->start(serverPath, arguments);
  }

  // 等待启动
  if (!m_serverProcess->waitForStarted(STARTUP_TIMEOUT)) {
    QString error =
        QString("服务器启动失败: %1").arg(m_serverProcess->errorString());
    emit serverError(error);
    delete m_serverProcess;
    m_serverProcess = nullptr;
    return false;
  }

  // 等待服务器就绪
  QTimer::singleShot(2000, this, &EmbeddedServer::performHealthCheck);

  return true;
}

void EmbeddedServer::stopServer() {
  if (!m_serverRunning || !m_serverProcess) {
    return;
  }

  qDebug() << "停止嵌入式服务器";

  // 停止健康检查
  m_healthCheckTimer->stop();

  // 优雅关闭服务器
  m_serverProcess->terminate();
  if (!m_serverProcess->waitForFinished(5000)) {
    qDebug() << "强制关闭服务器";
    m_serverProcess->kill();
    m_serverProcess->waitForFinished(2000);
  }

  delete m_serverProcess;
  m_serverProcess = nullptr;

  m_serverRunning = false;
  m_serverHealthy = false;

  emit serverStopped();
}

void EmbeddedServer::checkHealth() { performHealthCheck(); }

void EmbeddedServer::onServerStarted() {
  qDebug() << "服务器进程已启动";
  m_serverRunning = true;

  // 开始健康检查
  m_healthCheckTimer->start();

  emit serverStarted();
}

void EmbeddedServer::onServerFinished(int exitCode,
                                      QProcess::ExitStatus exitStatus) {
  qDebug() << "服务器进程结束，退出代码:" << exitCode << "状态:" << exitStatus;

  m_serverRunning = false;
  m_serverHealthy = false;
  m_healthCheckTimer->stop();

  if (m_serverProcess) {
    delete m_serverProcess;
    m_serverProcess = nullptr;
  }

  emit serverStopped();

  if (exitStatus == QProcess::CrashExit) {
    emit serverError(QString("服务器异常退出，退出代码: %1").arg(exitCode));
  }
}

void EmbeddedServer::onServerError(QProcess::ProcessError error) {
  QString errorString;
  switch (error) {
  case QProcess::FailedToStart:
    errorString = "服务器启动失败";
    break;
  case QProcess::Crashed:
    errorString = "服务器崩溃";
    break;
  case QProcess::Timedout:
    errorString = "服务器超时";
    break;
  case QProcess::WriteError:
    errorString = "服务器写入错误";
    break;
  case QProcess::ReadError:
    errorString = "服务器读取错误";
    break;
  default:
    errorString = "服务器未知错误";
    break;
  }

  qDebug() << "服务器错误:" << errorString;
  emit serverError(errorString);
}

void EmbeddedServer::onHealthCheckFinished() {
  QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
  if (!reply)
    return;

  bool isHealthy = (reply->error() == QNetworkReply::NoError);

  if (isHealthy != m_serverHealthy) {
    m_serverHealthy = isHealthy;
    qDebug() << "服务器健康状态变化:" << (isHealthy ? "健康" : "不健康");
    emit healthCheckResult(isHealthy);
  }

  reply->deleteLater();
}

void EmbeddedServer::performHealthCheck() {
  if (!m_serverRunning) {
    return;
  }

  QNetworkRequest request(QUrl(m_serverUrl + "/api/health"));
  request.setRawHeader("User-Agent", "EmbeddedServer/1.0");
  // request.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
  // QNetworkRequest::NoRedirectPolicy); // Qt 5.15兼容性

  QNetworkReply *reply = m_networkManager->get(request);
  connect(reply, &QNetworkReply::finished, this,
          &EmbeddedServer::onHealthCheckFinished);

  // 设置超时
  QTimer::singleShot(HEALTH_CHECK_TIMEOUT, reply, &QNetworkReply::abort);
}

void EmbeddedServer::findAvailablePort() {
  // 尝试从8080开始找可用端口
  for (int port = 8080; port <= 8090; ++port) {
    QTcpSocket socket;
    socket.connectToHost("localhost", port);
    if (!socket.waitForConnected(100)) {
      m_serverPort = port;
      qDebug() << "找到可用端口:" << port;
      return;
    }
    socket.disconnectFromHost();
  }

  // 如果都不可用，使用随机端口
  m_serverPort = 8080 + QRandomGenerator::global()->bounded(1000);
  qDebug() << "使用随机端口:" << m_serverPort;
}

QString EmbeddedServer::getServerExecutablePath() {
  QString appDir = QApplication::applicationDirPath();
  QStringList possiblePaths;

#ifdef Q_OS_WIN
  // Windows路径
  possiblePaths << appDir + "/md2docx-server.exe"
                << appDir + "/md2docx-server-windows.exe"
                << appDir + "/../build/md2docx-server-windows.exe";
#elif defined(Q_OS_MAC)
  // macOS路径
  possiblePaths
      << appDir + "/md2docx-server" << appDir + "/md2docx-server-macos"
      << appDir + "/../build/md2docx-server-macos"
      << appDir + "/../../../../build/md2docx-server-macos"; // 从.app包内部
#else
  // Linux路径
  possiblePaths << appDir + "/md2docx-server"
                << appDir + "/md2docx-server-linux"
                << appDir + "/../build/md2docx-server-linux";
#endif

  // 开发模式路径
  possiblePaths << appDir + "/../cmd/server/main.go"
                << appDir + "/../../cmd/server/main.go"
                << appDir + "/../../../cmd/server/main.go";

  // 查找第一个存在的文件
  for (const QString &path : possiblePaths) {
    QFileInfo fileInfo(path);
    if (fileInfo.exists()) {
      qDebug() << "找到服务器可执行文件:" << fileInfo.absoluteFilePath();
      return fileInfo.absoluteFilePath();
    }
  }

  qDebug() << "未找到服务器可执行文件，搜索路径:";
  for (const QString &path : possiblePaths) {
    qDebug() << "  " << path;
  }

  return QString();
}

void EmbeddedServer::setupHealthCheckTimer() {
  m_healthCheckTimer->setInterval(HEALTH_CHECK_INTERVAL);
  m_healthCheckTimer->setSingleShot(false);
  connect(m_healthCheckTimer, &QTimer::timeout, this,
          &EmbeddedServer::performHealthCheck);
}

void EmbeddedServer::loadPortFromConfig() {
  // 获取配置文件路径
  QString configPath = getConfigFilePath();

  QFileInfo configFile(configPath);
  if (!configFile.exists()) {
    qDebug() << "配置文件不存在，使用默认端口:" << configPath;
    return;
  }

  // 读取配置文件
  QFile file(configPath);
  if (!file.open(QIODevice::ReadOnly)) {
    qDebug() << "无法读取配置文件:" << configPath;
    return;
  }

  QByteArray data = file.readAll();
  file.close();

  // 解析JSON
  QJsonParseError error;
  QJsonDocument doc = QJsonDocument::fromJson(data, &error);
  if (error.error != QJsonParseError::NoError) {
    qDebug() << "配置文件JSON解析失败:" << error.errorString();
    return;
  }

  QJsonObject config = doc.object();
  if (config.contains("server_port")) {
    int port = config["server_port"].toInt();
    if (port > 0 && port <= 65535) {
      m_serverPort = port;
      qDebug() << "从配置文件读取端口:" << port;
    }
  }
}

QString EmbeddedServer::getConfigFilePath() {
  QString appDir = QApplication::applicationDirPath();

  // 尝试多个可能的配置文件位置
  QStringList possiblePaths;

  // 1. 与可执行文件在同一目录（应用包内）
  possiblePaths << appDir + "/config.json";

  // 2. 项目根目录（开发模式）
  possiblePaths << appDir + "/../../../config.json";
  possiblePaths << appDir + "/../../config.json";
  possiblePaths << appDir + "/../config.json";

  // 3. build目录（后端运行位置）
  possiblePaths << appDir + "/../../../build/config.json";
  possiblePaths << appDir + "/../../build/config.json";
  possiblePaths << appDir + "/../build/config.json";

  // 查找第一个存在的配置文件
  for (const QString &path : possiblePaths) {
    QFileInfo fileInfo(path);
    if (fileInfo.exists()) {
      qDebug() << "找到配置文件:" << fileInfo.absoluteFilePath();
      return fileInfo.absoluteFilePath();
    }
  }

  // 如果都不存在，返回默认路径（应用包内）
  QString defaultPath = appDir + "/config.json";
  qDebug() << "使用默认配置文件路径:" << defaultPath;
  return defaultPath;
}

bool EmbeddedServer::isPortAvailable(int port) {
  QTcpSocket socket;
  socket.connectToHost("localhost", port);
  bool available = !socket.waitForConnected(100);
  socket.disconnectFromHost();
  return available;
}
