#ifndef MULTIFILECONVERTER_H
#define MULTIFILECONVERTER_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLabel;
class QLineEdit;
class QPushButton;
class QTextEdit;
class QProgressBar;
class QGroupBox;
class QListWidget;
QT_END_NAMESPACE

class HttpApi;
struct ConversionResponse;

/**
 * @brief 多文件转换器组件
 *
 * 功能：
 * - 选择多个Markdown文件
 * - 设置统一输出路径
 * - 批量执行转换操作
 * - 显示转换进度和状态
 */
class MultiFileConverter : public QWidget {
  Q_OBJECT

public:
  explicit MultiFileConverter(HttpApi *api, QWidget *parent = nullptr);
  ~MultiFileConverter();

  // 公共接口
  void setEnabled(bool enabled);
  void resetForm();

signals:
  void conversionStarted();
  void conversionFinished(bool success, const QString &message);

private slots:
  void selectInputFiles();

  void clearAllFiles();
  void selectOutputDir();
  void startBatchConversion();
  void onFileListChanged();
  void onOutputDirChanged();
  void onBatchConversionFinished(const ConversionResponse &response);

private:
  void setupUI();
  void setupConnections();
  void updateUI();
  bool validateInputs();
  void showStatus(const QString &message, bool isError = false);
  void clearStatus();
  void addFilesToList(const QStringList &files);
  QStringList getInputFiles() const;

  // UI组件
  QGroupBox *m_inputGroup;
  QTextEdit *m_fileListText;
  QPushButton *m_selectFilesButton;
  QPushButton *m_removeSelectedButton;
  QPushButton *m_clearFilesButton;
  QLabel *m_fileCountLabel;

  QGroupBox *m_outputGroup;
  QLineEdit *m_outputDirEdit;
  QPushButton *m_selectOutputButton;

  QGroupBox *m_actionGroup;
  QPushButton *m_convertButton;
  QPushButton *m_resetButton;

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

#endif // MULTIFILECONVERTER_H
