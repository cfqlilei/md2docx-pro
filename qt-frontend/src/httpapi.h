#ifndef HTTPAPI_H
#define HTTPAPI_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QUrl>

struct ConversionRequest {
    QString inputFile;
    QString outputDir;
    QString outputName;
    QString templateFile;
};

struct BatchConversionRequest {
    QStringList inputFiles;
    QString outputDir;
    QString templateFile;
};

struct ConversionResult {
    QString inputFile;
    QString outputFile;
    bool success;
    QString error;
};

struct ConversionResponse {
    bool success;
    QString message;
    QString outputFile;
    QList<ConversionResult> results;
    QString error;
};

struct ConfigData {
    QString pandocPath;
    QString templateFile;
    int serverPort;
};

class HttpApi : public QObject
{
    Q_OBJECT

public:
    explicit HttpApi(QObject *parent = nullptr);
    ~HttpApi();

    // 设置服务器地址
    void setServerUrl(const QString &url);
    QString serverUrl() const { return m_serverUrl; }

    // API调用方法
    void checkHealth();
    void getConfig();
    void updateConfig(const ConfigData &config);
    void validateConfig();
    void convertSingle(const ConversionRequest &request);
    void convertBatch(const BatchConversionRequest &request);

    // 状态查询
    bool isServerOnline() const { return m_serverOnline; }

signals:
    void healthCheckFinished(bool isOnline);
    void configReceived(const ConfigData &config);
    void configUpdated(bool success, const QString &message);
    void configValidated(bool success, const QString &message);
    void singleConversionFinished(const ConversionResponse &response);
    void batchConversionFinished(const ConversionResponse &response);
    void errorOccurred(const QString &error);

private slots:
    void onHealthCheckFinished();
    void onGetConfigFinished();
    void onUpdateConfigFinished();
    void onValidateConfigFinished();
    void onSingleConversionFinished();
    void onBatchConversionFinished();
    void onNetworkError(QNetworkReply::NetworkError error);

private:
    QNetworkRequest createRequest(const QString &endpoint);
    void handleNetworkReply(QNetworkReply *reply, const QString &operation);
    ConversionResponse parseConversionResponse(const QJsonObject &json);
    ConfigData parseConfigData(const QJsonObject &json);

    QNetworkAccessManager *m_networkManager;
    QString m_serverUrl;
    bool m_serverOnline;
    
    // 请求超时定时器
    QTimer *m_timeoutTimer;
    static const int REQUEST_TIMEOUT = 30000; // 30秒
};

#endif // HTTPAPI_H
