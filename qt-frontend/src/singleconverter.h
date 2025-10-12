#ifndef SINGLECONVERTER_H
#define SINGLECONVERTER_H

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

class SingleConverter : public QWidget {
  Q_OBJECT

public:
  explicit SingleConverter(HttpApi *api, QWidget *parent = nullptr);
  ~SingleConverter();

  // 公共接口
  void setEnabled(bool enabled);
  void resetForm();

signals:
  void conversionStarted();
  void conversionFinished(bool success, const QString &message);

private slots:
  void browseInputFile();
  void browseOutputDir();
  void browseTemplateFile();
  void startConversion();
  void clearAll();
  void updateConvertButton();
  void onConversionFinished(bool success, const QString &outputFile,
                            const QString &message);

private:
  void setupUI();
  void setupConnections();
  void updateUI();
  void validateInputs();
  void showStatus(const QString &message, bool isError = false);
  void clearStatus();
  QString getDefaultOutputName() const;

  // UI组件
  QGroupBox *m_inputGroup;
  QLineEdit *m_inputFileEdit;
  QPushButton *m_selectInputButton;

  QGroupBox *m_outputGroup;
  QLineEdit *m_outputDirEdit;
  QPushButton *m_selectOutputButton;
  QLineEdit *m_outputNameEdit;
  QLineEdit *m_templateFileEdit;
  QPushButton *m_selectTemplateButton;

  QGroupBox *m_actionGroup;
  QPushButton *m_convertButton;
  QPushButton *m_clearButton;

  QGroupBox *m_statusGroup;
  QTextEdit *m_statusEdit;
  QProgressBar *m_progressBar;

  // 后端API
  HttpApi *m_httpApi;

  // 状态变量
  bool m_conversionInProgress;
  QString m_lastInputFile;
  QString m_lastOutputDir;
};

#endif // SINGLECONVERTER_H
