#ifndef MAINWINDOW_MD2DOCX_H
#define MAINWINDOW_MD2DOCX_H

#include <QMainWindow>
#include <QMenuBar>
#include <QStatusBar>
#include <QTabWidget>
#include <QTimer>

QT_BEGIN_NAMESPACE
class QTabWidget;
class QMenuBar;
class QStatusBar;
QT_END_NAMESPACE

class HttpApi;
class SingleFileConverter;
class MultiFileConverter;
class SettingsWidget;
class AboutWidget;

/**
 * @brief Markdown转Word工具主窗口
 *
 * 包含3个页签：
 * 1. 单文件转换
 * 2. 多文件转换
 * 3. 设置
 */
class MainWindowMd2Docx : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindowMd2Docx(QWidget *parent = nullptr);
  ~MainWindowMd2Docx();

private slots:
  void checkBackendConnection();
  void onHealthCheckFinished(bool success);
  void onConversionStarted();
  void onConversionFinished(bool success, const QString &message);
  void onConfigChanged();
  void showAbout();

private:
  void setupUI();
  void setupMenuBar();
  void setupHttpApi();
  void setupConnections();
  void setTabsEnabled(bool enabled);

  // UI组件
  QTabWidget *m_tabWidget;

  // 页签组件
  SingleFileConverter *m_singleFileConverter;
  MultiFileConverter *m_multiFileConverter;
  SettingsWidget *m_settingsWidget;
  AboutWidget *m_aboutWidget;

  // 后端API
  HttpApi *m_httpApi;

  // 状态检查定时器
  QTimer *m_connectionTimer;
};

#endif // MAINWINDOW_MD2DOCX_H
