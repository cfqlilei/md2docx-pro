#include "appsettings.h"
#include "mainwindow_integrated.h"

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFont>
#include <QLibraryInfo>
#include <QLocale>
#include <QMessageBox>
#include <QPainter>
#include <QPalette>
#include <QPixmap>
#include <QSplashScreen>
#include <QStandardPaths>
#include <QStyleFactory>
#include <QTimer>
#include <QTranslator>

// 应用程序信息
static const QString APP_NAME = "Markdown转Word工具";
static const QString APP_VERSION = "1.0.0";
static const QString APP_ORGANIZATION = "MD2DOCX";
static const QString APP_DOMAIN = "md2docx.local";

// 设置应用程序信息
void setupApplicationInfo() {
  QApplication::setApplicationName(APP_NAME);
  QApplication::setApplicationVersion(APP_VERSION);
  QApplication::setOrganizationName(APP_ORGANIZATION);
  QApplication::setOrganizationDomain(APP_DOMAIN);

  // 设置应用程序属性
  QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
}

// 设置应用程序样式
void setupApplicationStyle(QApplication &app) {
  // 设置现代化样式
  app.setStyle(QStyleFactory::create("Fusion"));

  // 设置深色主题（可选）
  QPalette darkPalette;
  darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
  darkPalette.setColor(QPalette::WindowText, Qt::white);
  darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
  darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
  darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
  darkPalette.setColor(QPalette::ToolTipText, Qt::white);
  darkPalette.setColor(QPalette::Text, Qt::white);
  darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
  darkPalette.setColor(QPalette::ButtonText, Qt::white);
  darkPalette.setColor(QPalette::BrightText, Qt::red);
  darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
  darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
  darkPalette.setColor(QPalette::HighlightedText, Qt::black);

  // 根据用户设置决定是否使用深色主题
  // AppSettings settings;
  // bool useDarkTheme = settings.value("ui/dark_theme", false).toBool();
  bool useDarkTheme = false; // 暂时禁用深色主题

  if (useDarkTheme) {
    app.setPalette(darkPalette);
  }

  // 设置字体
  QFont font = app.font();
  font.setPointSize(10);
  app.setFont(font);
}

// 检查单实例运行
bool checkSingleInstance() {
  // 简单的单实例检查（可以使用更复杂的方法）
  QString lockFilePath =
      QStandardPaths::writableLocation(QStandardPaths::TempLocation) +
      "/md2docx_integrated.lock";

  QFile lockFile(lockFilePath);
  if (lockFile.exists()) {
    // 检查进程是否还在运行
    QMessageBox::information(
        nullptr, APP_NAME, "应用程序已经在运行中。\n请检查系统托盘或任务栏。");
    return false;
  }

  // 创建锁文件
  if (lockFile.open(QIODevice::WriteOnly)) {
    lockFile.write(QByteArray::number(QApplication::applicationPid()));
    lockFile.close();
  }

  return true;
}

// 清理锁文件
void cleanupLockFile() {
  QString lockFilePath =
      QStandardPaths::writableLocation(QStandardPaths::TempLocation) +
      "/md2docx_integrated.lock";
  QFile::remove(lockFilePath);
}

// 显示启动画面
QSplashScreen *showSplashScreen() {
  // 创建启动画面
  QPixmap pixmap(400, 300);
  pixmap.fill(QColor(45, 45, 45));

  QPainter painter(&pixmap);
  painter.setRenderHint(QPainter::Antialiasing);

  // 绘制标题
  painter.setPen(Qt::white);
  painter.setFont(QFont("Arial", 24, QFont::Bold));
  painter.drawText(pixmap.rect(), Qt::AlignCenter, APP_NAME);

  // 绘制版本信息
  painter.setFont(QFont("Arial", 12));
  painter.drawText(QRect(0, pixmap.height() - 60, pixmap.width(), 30),
                   Qt::AlignCenter, QString("版本 %1").arg(APP_VERSION));

  // 绘制状态信息
  painter.drawText(QRect(0, pixmap.height() - 30, pixmap.width(), 30),
                   Qt::AlignCenter, "正在启动服务器...");

  QSplashScreen *splash = new QSplashScreen(pixmap);
  splash->show();

  return splash;
}

// 主函数
int main(int argc, char *argv[]) {
  // 设置应用程序信息
  setupApplicationInfo();

  // 创建应用程序
  QApplication app(argc, argv);

  // 检查单实例运行
  if (!checkSingleInstance()) {
    return 0;
  }

  // 设置样式
  setupApplicationStyle(app);

  // 显示启动画面
  QSplashScreen *splash = showSplashScreen();
  app.processEvents();

  // 初始化应用设置
  AppSettings::instance();

  // 创建主窗口
  MainWindowIntegrated mainWindow;

  // 延迟显示主窗口，让启动画面显示一会儿
  QTimer::singleShot(2000, [&]() {
    splash->finish(&mainWindow);
    mainWindow.show();
    delete splash;
  });

  // 设置退出时清理
  QObject::connect(&app, &QApplication::aboutToQuit,
                   []() { cleanupLockFile(); });

  // 运行应用程序
  int result = app.exec();

  // 清理
  cleanupLockFile();

  return result;
}
