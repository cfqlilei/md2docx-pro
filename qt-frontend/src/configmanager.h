#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QCheckBox>
#include <QFileDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

QT_BEGIN_NAMESPACE
class QLineEdit;
class QPushButton;
class QTextEdit;
class QGroupBox;
class QCheckBox;
class QSpinBox;
QT_END_NAMESPACE

class HttpApi;

// 前向声明，实际定义在httpapi.h中
struct ConfigData;

class ConfigManager : public QWidget {
  Q_OBJECT

public:
  explicit ConfigManager(HttpApi *api, QWidget *parent = nullptr);
  ~ConfigManager();

  // 公共接口
  void setEnabled(bool enabled);
  void loadConfig();
  void resetForm();

signals:
  void configChanged();

private slots:
  void selectPandocPath();
  void selectTemplateFile();
  void loadCurrentConfig();
  void saveConfig();
  void validateConfig();
  void resetToDefaults();
  void onConfigReceived(const ConfigData &config);
  void onConfigUpdated(bool success, const QString &message);
  void onConfigValidated(bool success, const QString &message);
  void onPandocPathChanged();
  void onTemplateFileChanged();

private:
  void setupUI();
  void setupConnections();
  void updateUI();
  void showStatus(const QString &message, bool isError = false);
  void clearStatus();
  void setConfigData(const ConfigData &config);
  ConfigData getConfigData() const;
  bool validatePandocPath(const QString &path);
  bool validateTemplateFile(const QString &path);

  // UI组件
  QGroupBox *m_pandocGroup;
  QLineEdit *m_pandocPathEdit;
  QPushButton *m_selectPandocButton;
  QPushButton *m_testPandocButton;
  QLabel *m_pandocStatusLabel;

  QGroupBox *m_templateGroup;
  QLineEdit *m_templateFileEdit;
  QPushButton *m_selectTemplateButton;
  QPushButton *m_clearTemplateButton;
  QCheckBox *m_useTemplateCheckBox;

  QGroupBox *m_serverGroup;
  QSpinBox *m_serverPortSpinBox;
  QLabel *m_serverStatusLabel;

  QGroupBox *m_actionGroup;
  QPushButton *m_loadButton;
  QPushButton *m_saveButton;
  QPushButton *m_validateButton;
  QPushButton *m_resetButton;

  QGroupBox *m_statusGroup;
  QTextEdit *m_statusText;

  // 后端API
  HttpApi *m_httpApi;

  // 状态变量
  bool m_configLoaded;
  ConfigData *m_currentConfig;
  ConfigData *m_originalConfig;
};

#endif // CONFIGMANAGER_H
