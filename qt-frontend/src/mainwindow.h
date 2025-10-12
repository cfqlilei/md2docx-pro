#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAction>
#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QMenuBar>
#include <QMessageBox>
#include <QProcess>
#include <QProgressBar>
#include <QStatusBar>
#include <QTabWidget>
#include <QTimer>
#include <QVBoxLayout>

QT_BEGIN_NAMESPACE
class QTabWidget;
QT_END_NAMESPACE

class SingleConverter;
class BatchConverter;
class ConfigManager;
class HttpApi;

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private slots:
  void showAbout();
  void showHelp();
  void checkServerStatus();
  void onServerStatusChanged(bool isOnline);
  void onConversionStarted();
  void onConversionFinished(bool success, const QString &message);
  void onConfigChanged();

private:
  void setupUI();
  void setupMenuBar();
  void setupStatusBar();
  void setupConnections();
  void startBackendServer();
  void stopBackendServer();
  void updateServerStatus(bool isOnline);

  // UI组件
  QTabWidget *m_tabWidget;
  SingleConverter *m_singleConverter;
  BatchConverter *m_batchConverter;
  ConfigManager *m_configManager;

  // 状态栏组件
  QLabel *m_statusLabel;
  QLabel *m_serverStatusLabel;
  QProgressBar *m_progressBar;

  // 菜单和动作
  QAction *m_exitAction;
  QAction *m_aboutAction;
  QAction *m_helpAction;
  QAction *m_configAction;

  // 后端服务
  HttpApi *m_httpApi;
  QTimer *m_statusTimer;
  QProcess *m_backendProcess;

  // 状态变量
  bool m_serverOnline;
  bool m_conversionInProgress;
};

#endif // MAINWINDOW_H
