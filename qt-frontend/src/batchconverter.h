#ifndef BATCHCONVERTER_H
#define BATCHCONVERTER_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QListWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QProgressBar>
#include <QGroupBox>
#include <QSplitter>

QT_BEGIN_NAMESPACE
class QLineEdit;
class QPushButton;
class QTextEdit;
class QListWidget;
class QProgressBar;
class QGroupBox;
class QSplitter;
QT_END_NAMESPACE

class HttpApi;
struct ConversionResponse;

class BatchConverter : public QWidget
{
    Q_OBJECT

public:
    explicit BatchConverter(HttpApi *api, QWidget *parent = nullptr);
    ~BatchConverter();

    // 公共接口
    void setEnabled(bool enabled);
    void resetForm();

signals:
    void conversionStarted();
    void conversionFinished(bool success, const QString &message);

private slots:
    void selectInputFiles();
    void removeSelectedFiles();
    void clearAllFiles();
    void selectOutputDir();
    void startBatchConversion();
    void onConversionFinished(const ConversionResponse &response);
    void onFileListChanged();
    void onOutputDirChanged();

private:
    void setupUI();
    void setupConnections();
    void updateUI();
    void validateInputs();
    void showStatus(const QString &message, bool isError = false);
    void clearStatus();
    void addFilesToList(const QStringList &files);
    QStringList getSelectedFiles() const;

    // UI组件
    QSplitter *m_mainSplitter;
    
    // 输入文件组
    QGroupBox *m_inputGroup;
    QListWidget *m_fileList;
    QPushButton *m_addFilesButton;
    QPushButton *m_removeFilesButton;
    QPushButton *m_clearFilesButton;
    QLabel *m_fileCountLabel;
    
    // 输出设置组
    QGroupBox *m_outputGroup;
    QLineEdit *m_outputDirEdit;
    QPushButton *m_selectOutputButton;
    
    // 操作组
    QGroupBox *m_actionGroup;
    QPushButton *m_convertButton;
    QPushButton *m_resetButton;
    
    // 状态组
    QGroupBox *m_statusGroup;
    QTextEdit *m_statusText;
    QProgressBar *m_progressBar;
    
    // 后端API
    HttpApi *m_httpApi;
    
    // 状态变量
    bool m_conversionInProgress;
    QStringList m_inputFiles;
    QString m_lastOutputDir;
};

#endif // BATCHCONVERTER_H
