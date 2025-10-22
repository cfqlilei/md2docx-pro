#include "httpapi.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QStandardPaths>
#include <QTimer>
#include <QUrl>

HttpApi::HttpApi(QObject *parent)
    : QObject(parent), m_networkManager(new QNetworkAccessManager(this)),
      m_serverUrl("http://localhost:8080"), m_serverOnline(false),
      m_timeoutTimer(new QTimer(this)) {
  m_timeoutTimer->setSingleShot(true);
  m_timeoutTimer->setInterval(REQUEST_TIMEOUT);

  // 尝试从配置文件读取服务器端口
  loadServerPortFromConfig();
}

HttpApi::~HttpApi() {}

void HttpApi::setServerUrl(const QString &url) { m_serverUrl = url; }

void HttpApi::checkHealth() {
  QNetworkRequest request = createRequest("/api/health");
  QNetworkReply *reply = m_networkManager->get(request);

  connect(reply, &QNetworkReply::finished, this,
          &HttpApi::onHealthCheckFinished);
  connect(reply,
          QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
          this, &HttpApi::onNetworkError);
}

void HttpApi::getConfig() {
  QNetworkRequest request = createRequest("/api/config");
  QNetworkReply *reply = m_networkManager->get(request);

  connect(reply, &QNetworkReply::finished, this, &HttpApi::onGetConfigFinished);
  connect(reply,
          QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
          this, &HttpApi::onNetworkError);
}

void HttpApi::updateConfig(const ConfigData &config) {
  QNetworkRequest request = createRequest("/api/config");

  QJsonObject data;
  data["pandoc_path"] = config.pandocPath;
  data["template_file"] = config.templateFile;
  data["server_port"] = config.serverPort;

  QJsonDocument doc(data);
  QByteArray jsonData = doc.toJson();

  QNetworkReply *reply = m_networkManager->post(request, jsonData);

  connect(reply, &QNetworkReply::finished, this,
          &HttpApi::onUpdateConfigFinished);
  connect(reply,
          QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
          this, &HttpApi::onNetworkError);
}

void HttpApi::validateConfig() {
  QNetworkRequest request = createRequest("/api/config/validate");
  QNetworkReply *reply = m_networkManager->post(request, QByteArray());

  connect(reply, &QNetworkReply::finished, this,
          &HttpApi::onValidateConfigFinished);
  connect(reply,
          QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
          this, &HttpApi::onNetworkError);
}

void HttpApi::convertSingle(const ConversionRequest &request) {
  QNetworkRequest netRequest = createRequest("/api/convert/single");

  QJsonObject data;
  data["input_file"] = request.inputFile;
  data["output_dir"] = request.outputDir;
  data["output_name"] = request.outputName;
  data["template_file"] = request.templateFile;

  QJsonDocument doc(data);
  QByteArray jsonData = doc.toJson();

  QNetworkReply *reply = m_networkManager->post(netRequest, jsonData);

  connect(reply, &QNetworkReply::finished, this,
          &HttpApi::onSingleConversionFinished);
  connect(reply,
          QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
          this, &HttpApi::onNetworkError);
}

void HttpApi::convertBatch(const BatchConversionRequest &request) {
  QNetworkRequest netRequest = createRequest("/api/convert/batch");

  QJsonObject data;
  QJsonArray inputFiles;
  for (const QString &file : request.inputFiles) {
    inputFiles.append(file);
  }
  data["input_files"] = inputFiles;
  data["output_dir"] = request.outputDir;
  data["template_file"] = request.templateFile;

  QJsonDocument doc(data);
  QByteArray jsonData = doc.toJson();

  QNetworkReply *reply = m_networkManager->post(netRequest, jsonData);

  connect(reply, &QNetworkReply::finished, this,
          &HttpApi::onBatchConversionFinished);
  connect(reply,
          QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
          this, &HttpApi::onNetworkError);
}

QNetworkRequest HttpApi::createRequest(const QString &endpoint) {
  QUrl url(m_serverUrl + endpoint);
  QNetworkRequest request(url);
  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
  return request;
}

void HttpApi::handleNetworkReply(QNetworkReply *reply,
                                 const QString &operation) {
  if (reply->error() != QNetworkReply::NoError) {
    QString errorMsg =
        QString("网络错误 (%1): %2").arg(operation).arg(reply->errorString());
    emit errorOccurred(errorMsg);
    return;
  }

  QByteArray data = reply->readAll();
  QJsonParseError parseError;
  QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

  if (parseError.error != QJsonParseError::NoError) {
    QString errorMsg = QString("JSON解析错误 (%1): %2")
                           .arg(operation)
                           .arg(parseError.errorString());
    emit errorOccurred(errorMsg);
    return;
  }
}

void HttpApi::onHealthCheckFinished() {
  QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
  if (!reply)
    return;

  bool isOnline = (reply->error() == QNetworkReply::NoError);
  m_serverOnline = isOnline;

  emit healthCheckFinished(isOnline);
  reply->deleteLater();
}

void HttpApi::onGetConfigFinished() {
  QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
  if (!reply)
    return;

  if (reply->error() != QNetworkReply::NoError) {
    emit errorOccurred(QString("获取配置失败: %1").arg(reply->errorString()));
    reply->deleteLater();
    return;
  }

  QByteArray data = reply->readAll();
  QJsonDocument doc = QJsonDocument::fromJson(data);
  QJsonObject json = doc.object();

  ConfigData config = parseConfigData(json);
  emit configReceived(config);

  reply->deleteLater();
}

void HttpApi::onUpdateConfigFinished() {
  QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
  if (!reply)
    return;

  bool success = (reply->error() == QNetworkReply::NoError);
  QString message;

  if (success) {
    message = "配置更新成功";
  } else {
    message = QString("配置更新失败: %1").arg(reply->errorString());
  }

  emit configUpdated(success, message);
  reply->deleteLater();
}

void HttpApi::onValidateConfigFinished() {
  QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
  if (!reply)
    return;

  bool success = (reply->error() == QNetworkReply::NoError);
  QString message;

  if (success) {
    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject json = doc.object();

    if (json.contains("valid") && json["valid"].toBool()) {
      message = "配置验证通过";
    } else {
      success = false;
      message = json.value("message").toString("配置验证失败");
    }
  } else {
    message = QString("配置验证失败: %1").arg(reply->errorString());
  }

  emit configValidated(success, message);
  reply->deleteLater();
}

void HttpApi::onSingleConversionFinished() {
  QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
  if (!reply)
    return;

  ConversionResponse response;

  if (reply->error() != QNetworkReply::NoError) {
    response.success = false;
    response.error = reply->errorString();
  } else {
    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject json = doc.object();

    response = parseConversionResponse(json);
  }

  emit singleConversionFinished(response);
  reply->deleteLater();
}

void HttpApi::onBatchConversionFinished() {
  QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
  if (!reply)
    return;

  ConversionResponse response;

  if (reply->error() != QNetworkReply::NoError) {
    response.success = false;
    response.error = reply->errorString();
  } else {
    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject json = doc.object();

    response = parseConversionResponse(json);
  }

  emit batchConversionFinished(response);
  reply->deleteLater();
}

void HttpApi::onNetworkError(QNetworkReply::NetworkError error) {
  QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
  if (!reply)
    return;

  QString errorMsg = QString("网络错误: %1").arg(reply->errorString());
  emit errorOccurred(errorMsg);
}

ConversionResponse HttpApi::parseConversionResponse(const QJsonObject &json) {
  ConversionResponse response;
  response.success = json.value("success").toBool();
  response.message = json.value("message").toString();
  response.outputFile = json.value("output_file").toString();
  response.error = json.value("error").toString();

  // 解析批量转换的结果数组
  if (json.contains("results")) {
    QJsonArray resultsArray = json.value("results").toArray();
    for (const QJsonValue &value : resultsArray) {
      QJsonObject resultObj = value.toObject();
      ConversionResult result;
      result.inputFile = resultObj.value("input_file").toString();
      result.outputFile = resultObj.value("output_file").toString();
      result.success = resultObj.value("success").toBool();
      result.error = resultObj.value("error").toString();
      response.results.append(result);
    }
  }

  return response;
}

ConfigData HttpApi::parseConfigData(const QJsonObject &json) {
  ConfigData config;
  config.pandocPath = json.value("pandoc_path").toString();
  config.templateFile = json.value("template_file").toString();
  config.serverPort = json.value("server_port").toInt(8080);

  return config;
}

void HttpApi::loadServerPortFromConfig() {
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
      m_serverUrl = QString("http://localhost:%1").arg(port);
      qDebug() << "从配置文件读取服务器端口:" << port;
    }
  }
}

QString HttpApi::getConfigFilePath() {
  // 首先尝试用户主目录下的配置文件
  QString homeDir =
      QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
  if (!homeDir.isEmpty()) {
    QString configPath = QDir(homeDir).filePath(".md2docx/config.json");
    if (QFileInfo(configPath).exists()) {
      return configPath;
    }
  }

  // 然后尝试应用程序目录
  QString appDir = QDir::currentPath();
  QString configPath = QDir(appDir).filePath("config.json");
  if (QFileInfo(configPath).exists()) {
    return configPath;
  }

  // 返回默认路径（用户主目录）
  return QDir(homeDir).filePath(".md2docx/config.json");
}
