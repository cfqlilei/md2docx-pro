#include "httpapi.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>
#include <QUrlQuery>
#include <QTimer>
#include <QDebug>

HttpApi::HttpApi(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_baseUrl("http://localhost:8080")
    , m_timeout(30000) // 30秒超时
{
    connect(m_networkManager, &QNetworkAccessManager::finished,
            this, &HttpApi::onRequestFinished);
}

HttpApi::~HttpApi()
{
}

void HttpApi::setBaseUrl(const QString &url)
{
    m_baseUrl = url;
}

void HttpApi::setTimeout(int msecs)
{
    m_timeout = msecs;
}

void HttpApi::checkHealth()
{
    QUrl url(m_baseUrl + "/api/health");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QNetworkReply *reply = m_networkManager->get(request);
    setupTimeout(reply);
    
    m_pendingRequests[reply] = RequestInfo{RequestType::Health, QJsonObject()};
}

void HttpApi::getConfig()
{
    QUrl url(m_baseUrl + "/api/config");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QNetworkReply *reply = m_networkManager->get(request);
    setupTimeout(reply);
    
    m_pendingRequests[reply] = RequestInfo{RequestType::GetConfig, QJsonObject()};
}

void HttpApi::updateConfig(const QString &pandocPath, const QString &templateFile)
{
    QUrl url(m_baseUrl + "/api/config");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QJsonObject data;
    if (!pandocPath.isEmpty()) {
        data["pandoc_path"] = pandocPath;
    }
    if (!templateFile.isEmpty()) {
        data["template_file"] = templateFile;
    }
    
    QJsonDocument doc(data);
    QNetworkReply *reply = m_networkManager->post(request, doc.toJson());
    setupTimeout(reply);
    
    m_pendingRequests[reply] = RequestInfo{RequestType::UpdateConfig, data};
}

void HttpApi::validateConfig()
{
    QUrl url(m_baseUrl + "/api/config/validate");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QNetworkReply *reply = m_networkManager->post(request, QByteArray());
    setupTimeout(reply);
    
    m_pendingRequests[reply] = RequestInfo{RequestType::ValidateConfig, QJsonObject()};
}

void HttpApi::convertSingle(const QString &inputFile, const QString &outputDir, 
                           const QString &outputName, const QString &templateFile)
{
    QUrl url(m_baseUrl + "/api/convert/single");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QJsonObject data;
    data["input_file"] = inputFile;
    if (!outputDir.isEmpty()) {
        data["output_dir"] = outputDir;
    }
    if (!outputName.isEmpty()) {
        data["output_name"] = outputName;
    }
    if (!templateFile.isEmpty()) {
        data["template_file"] = templateFile;
    }
    
    QJsonDocument doc(data);
    QNetworkReply *reply = m_networkManager->post(request, doc.toJson());
    setupTimeout(reply);
    
    m_pendingRequests[reply] = RequestInfo{RequestType::ConvertSingle, data};
}

void HttpApi::convertBatch(const QStringList &inputFiles, const QString &outputDir, 
                          const QString &templateFile)
{
    QUrl url(m_baseUrl + "/api/convert/batch");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QJsonObject data;
    QJsonArray filesArray;
    for (const QString &file : inputFiles) {
        filesArray.append(file);
    }
    data["input_files"] = filesArray;
    
    if (!outputDir.isEmpty()) {
        data["output_dir"] = outputDir;
    }
    if (!templateFile.isEmpty()) {
        data["template_file"] = templateFile;
    }
    
    QJsonDocument doc(data);
    QNetworkReply *reply = m_networkManager->post(request, doc.toJson());
    setupTimeout(reply);
    
    m_pendingRequests[reply] = RequestInfo{RequestType::ConvertBatch, data};
}

void HttpApi::onRequestFinished(QNetworkReply *reply)
{
    reply->deleteLater();
    
    if (!m_pendingRequests.contains(reply)) {
        return;
    }
    
    RequestInfo requestInfo = m_pendingRequests.take(reply);
    
    if (reply->error() != QNetworkReply::NoError) {
        QString errorString = reply->errorString();
        qDebug() << "Network error:" << errorString;
        
        switch (requestInfo.type) {
        case RequestType::Health:
            emit healthCheckFinished(false, errorString);
            break;
        case RequestType::GetConfig:
            emit configReceived(false, QString(), QString(), errorString);
            break;
        case RequestType::UpdateConfig:
            emit configUpdated(false, errorString);
            break;
        case RequestType::ValidateConfig:
            emit configValidated(false, errorString);
            break;
        case RequestType::ConvertSingle:
            emit singleConversionFinished(false, QString(), errorString);
            break;
        case RequestType::ConvertBatch:
            emit batchConversionFinished(false, QStringList(), errorString);
            break;
        }
        return;
    }
    
    QByteArray data = reply->readAll();
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    
    if (parseError.error != QJsonParseError::NoError) {
        QString errorString = QString("JSON解析错误: %1").arg(parseError.errorString());
        qDebug() << errorString;
        
        switch (requestInfo.type) {
        case RequestType::Health:
            emit healthCheckFinished(false, errorString);
            break;
        case RequestType::GetConfig:
            emit configReceived(false, QString(), QString(), errorString);
            break;
        case RequestType::UpdateConfig:
            emit configUpdated(false, errorString);
            break;
        case RequestType::ValidateConfig:
            emit configValidated(false, errorString);
            break;
        case RequestType::ConvertSingle:
            emit singleConversionFinished(false, QString(), errorString);
            break;
        case RequestType::ConvertBatch:
            emit batchConversionFinished(false, QStringList(), errorString);
            break;
        }
        return;
    }
    
    QJsonObject response = doc.object();
    processResponse(requestInfo.type, response);
}

void HttpApi::processResponse(RequestType type, const QJsonObject &response)
{
    switch (type) {
    case RequestType::Health:
        {
            bool success = response["status"].toString() == "ok";
            QString message = response["message"].toString();
            emit healthCheckFinished(success, message);
        }
        break;
        
    case RequestType::GetConfig:
        {
            bool success = response["success"].toBool();
            QString pandocPath = response["pandoc_path"].toString();
            QString templateFile = response["template_file"].toString();
            QString error = response["error"].toString();
            emit configReceived(success, pandocPath, templateFile, error);
        }
        break;
        
    case RequestType::UpdateConfig:
        {
            bool success = response["success"].toBool();
            QString message = response["message"].toString();
            QString error = response["error"].toString();
            emit configUpdated(success, success ? message : error);
        }
        break;
        
    case RequestType::ValidateConfig:
        {
            bool success = response["success"].toBool();
            QString message = response["message"].toString();
            QString error = response["error"].toString();
            emit configValidated(success, success ? message : error);
        }
        break;
        
    case RequestType::ConvertSingle:
        {
            bool success = response["success"].toBool();
            QString outputFile = response["output_file"].toString();
            QString message = response["message"].toString();
            QString error = response["error"].toString();
            emit singleConversionFinished(success, outputFile, success ? message : error);
        }
        break;
        
    case RequestType::ConvertBatch:
        {
            bool success = response["success"].toBool();
            QString message = response["message"].toString();
            QString error = response["error"].toString();
            
            QStringList outputFiles;
            QJsonArray results = response["results"].toArray();
            for (const QJsonValue &value : results) {
                QJsonObject result = value.toObject();
                if (result["success"].toBool()) {
                    outputFiles.append(result["output_file"].toString());
                }
            }
            
            emit batchConversionFinished(success, outputFiles, success ? message : error);
        }
        break;
    }
}

void HttpApi::setupTimeout(QNetworkReply *reply)
{
    QTimer *timer = new QTimer(this);
    timer->setSingleShot(true);
    timer->setInterval(m_timeout);
    
    connect(timer, &QTimer::timeout, [reply, timer]() {
        reply->abort();
        timer->deleteLater();
    });
    
    connect(reply, &QNetworkReply::finished, timer, &QTimer::deleteLater);
    
    timer->start();
}
