#ifndef SIMPLE_BATCHCONVERTER_H
#define SIMPLE_BATCHCONVERTER_H

#include <QAbstractItemView>
#include <QFileDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QStandardPaths>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

#include "httpapi.h"

class SimpleBatchConverter : public QWidget {
  Q_OBJECT

public:
  explicit SimpleBatchConverter(HttpApi *httpApi, QWidget *parent = nullptr);

signals:
  void conversionStarted();
  void conversionFinished(bool success, const QString &message);

private slots:
  void selectInputFiles();
  void clearAllFiles();
  void selectOutputDir();
  void startBatchConversion();
  void onConversionFinished(const ConversionResponse &response);

private:
  void setupUI();
  void setupConnections();
  void updateStatus(const QString &message);
  void updateFileList();

private:
  HttpApi *m_httpApi;

  // UI组件
  QGroupBox *m_inputGroup;
  QListWidget *m_fileListWidget;
  QPushButton *m_selectFilesButton;
  QPushButton *m_clearFilesButton;

  QGroupBox *m_outputGroup;
  QLineEdit *m_outputDirEdit;
  QPushButton *m_selectOutputButton;

  QGroupBox *m_actionGroup;
  QPushButton *m_convertButton;

  QGroupBox *m_statusGroup;
  QTextEdit *m_statusEdit;
  QProgressBar *m_progressBar;

  // 数据
  QStringList m_inputFiles;
  QString m_lastOutputDir;
  bool m_conversionInProgress;
};

#endif // SIMPLE_BATCHCONVERTER_H
