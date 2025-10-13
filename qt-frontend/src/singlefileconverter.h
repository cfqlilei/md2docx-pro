#ifndef SINGLEFILECONVERTER_H
#define SINGLEFILECONVERTER_H

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
QT_END_NAMESPACE

class HttpApi;
struct ConversionResponse;

/**
 * @brief 单文件转换器组件
 *
 * 功能：
 * - 选择单个Markdown文件
 * - 设置输出路径和文件名
 * - 执行转换操作
 * - 显示转换状态
 */
class SingleFileConverter : public QWidget {
  Q_OBJECT

public:
  explicit SingleFileConverter(HttpApi *api, QWidget *parent = nullptr);
  ~SingleFileConverter();

  // 公共接口
  void setEnabled(bool enabled);
  void resetForm();

signals:
  void conversionStarted();
  void conversionFinished(bool success, const QString &message);

private slots:
  void selectInputFile();
  void selectOutputDir();
  void startConversion();
  void clearAll();
  void onInputFileChanged();
  void onOutputDirChanged();
  void onOutputNameChanged();
  void onConversionFinished(const ConversionResponse &response);

private:
  void setupUI();
  void setupConnections();
  void updateUI();
  bool validateInputs();
  void showStatus(const QString &message, bool isError = false);
  void clearStatus();
  QString getDefaultOutputName() const;
  QString getOutputFilePath() const;

  // UI组件
  QGroupBox *m_inputGroup;
  QLineEdit *m_inputFileEdit;
  QPushButton *m_selectInputButton;

  QGroupBox *m_outputGroup;
  QLineEdit *m_outputDirEdit;
  QPushButton *m_selectOutputButton;
  QLineEdit *m_outputNameEdit;

  QGroupBox *m_actionGroup;
  QPushButton *m_convertButton;
  QPushButton *m_clearButton;

  QGroupBox *m_statusGroup;
  QTextEdit *m_statusText;
  QProgressBar *m_progressBar;

  // 后端API
  HttpApi *m_httpApi;

  // 状态变量
  bool m_conversionInProgress;
  QString m_lastInputFile;
  QString m_lastOutputDir;
};

#endif // SINGLEFILECONVERTER_H
