#ifndef MAINWINDOW_INTEGRATED_H
#define MAINWINDOW_INTEGRATED_H

#include <QMainWindow>

class QTabWidget;
class QLabel;
class QProgressBar;
class QTimer;
class QSystemTrayIcon;
class QMenu;
class QAction;
class QCloseEvent;

// 前向声明
class SingleFileConverter;
class MultiFileConverter;
class SettingsWidget;
class AboutWidget;
class HttpApi;
class EmbeddedServer;

/**
 * 整合版主窗口
 * 内嵌Go后端服务，实现单一程序运行
 */
class MainWindowIntegrated : public QMainWindow {
  Q_OBJECT

public:
  MainWindowIntegrated(QWidget *parent = nullptr);
  ~MainWindowIntegrated();

protected:
  void closeEvent(QCloseEvent *event) override;
  void changeEvent(QEvent *event) override;

private slots:
  // 服务器状态
  void onServerStarted();
  void onServerStopped();
  void onServerError(const QString &error);
  void onServerHealthChanged(bool isHealthy);

  // 应用控制
  void showMainWindow();
  void quitApplication();
  void aboutApplication();

  // 状态更新
  void updateServerStatus();
  void updateStatusBar();

private:
  void setupUI();
  void setupMenuBar();
  void setupStatusBar();
  void setupSystemTray();
  void setupConnections();

  void startEmbeddedServer();
  void stopEmbeddedServer();

  void showServerStartupProgress();
  void hideServerStartupProgress();

  // UI组件
  QTabWidget *m_tabWidget;
  SingleFileConverter *m_singleConverter;
  MultiFileConverter *m_multiConverter;
  SettingsWidget *m_settingsWidget;
  AboutWidget *m_aboutWidget;

  // 状态栏
  QLabel *m_statusLabel;
  QLabel *m_serverStatusLabel;
  QProgressBar *m_progressBar;

  // 系统托盘
  QSystemTrayIcon *m_systemTray;
  QMenu *m_trayMenu;
  QAction *m_showAction;
  QAction *m_quitAction;

  // 服务器和API
  EmbeddedServer *m_embeddedServer;
  HttpApi *m_httpApi;

  // 状态
  bool m_serverRunning;
  bool m_serverHealthy;
  bool m_startupInProgress;

  // 定时器
  QTimer *m_statusUpdateTimer;
};

#endif // MAINWINDOW_INTEGRATED_H
