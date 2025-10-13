#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLabel;
class QLineEdit;
class QPushButton;
class QTextEdit;
class QGroupBox;
class QCheckBox;
class QProcess;
class QProgressBar;
class QMessageBox;
QT_END_NAMESPACE

class HttpApi;
struct ConfigData;

/**
 * @brief 设置组件
 *
 * 功能：
 * - 配置Pandoc路径
 * - 配置转换模板
 * - 验证配置
 * - 保存和加载配置
 */
class SettingsWidget : public QWidget {
  Q_OBJECT

public:
  explicit SettingsWidget(HttpApi *api, QWidget *parent = nullptr);
  ~SettingsWidget();

  // 公共接口
  void setEnabled(bool enabled);
  void loadConfig();
  void resetForm();

signals:
  void configChanged();

private slots:
  void selectPandocPath();
  void testPandocPath();
  void installPandoc();
  void selectTemplateFile();
  void clearTemplateFile();
  void loadCurrentConfig();
  void saveConfig();
  void validateConfig();
  void resetToDefaults();
  void onPandocPathChanged();
  void onTemplateFileChanged();
  void onConfigReceived(const ConfigData &config);
  void onConfigUpdated(bool success, const QString &message);
  void onConfigValidated(bool success, const QString &message);
  void onInstallProcessFinished(int exitCode, int exitStatus);
  void onInstallProcessError(int error);
  void onInstallProcessOutput();

private:
  void setupUI();
  void setupConnections();
  void updateUI();
  void showStatus(const QString &message, bool isError = false);
  void clearStatus();
  QString detectPandocPath();
  bool validatePandocPath(const QString &path);
  bool validateTemplateFile(const QString &path);

  // Pandoc安装相关方法
  QString detectOperatingSystem();
  QString detectRegion();
  bool isPandocInstalled();
  QString getPandocInstallCommand();
  void startPandocInstallation();

  // UI组件
  QGroupBox *m_pandocGroup;
  QLineEdit *m_pandocPathEdit;
  QPushButton *m_selectPandocButton;
  QPushButton *m_testPandocButton;
  QPushButton *m_installPandocButton;
  QLabel *m_pandocStatusLabel;
  QProgressBar *m_installProgressBar;

  QGroupBox *m_templateGroup;
  QLineEdit *m_templateFileEdit;
  QPushButton *m_selectTemplateButton;
  QPushButton *m_clearTemplateButton;
  QCheckBox *m_useTemplateCheckBox;

  QGroupBox *m_actionGroup;
  QPushButton *m_saveButton;
  QPushButton *m_validateButton;
  QPushButton *m_resetButton;

  QGroupBox *m_statusGroup;
  QTextEdit *m_statusText;

  // 后端API
  HttpApi *m_httpApi;

  // 状态变量
  bool m_configLoaded;
  QString m_currentPandocPath;
  QString m_currentTemplateFile;

  // Pandoc安装相关
  QProcess *m_installProcess;
  bool m_isInstalling;
};

#endif // SETTINGSWIDGET_H
