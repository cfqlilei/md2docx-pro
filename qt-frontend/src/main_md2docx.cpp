#include <QApplication>
#include <QStyleFactory>
#include <QDir>
#include <QStandardPaths>
#include <QMessageBox>
#include <QSplashScreen>
#include <QPixmap>
#include <QTimer>

#include "mainwindow_md2docx.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // 设置应用程序信息
    app.setApplicationName("Markdown转Word工具");
    app.setApplicationVersion("2.0.0");
    app.setOrganizationName("MD2DOCX");
    app.setOrganizationDomain("md2docx.local");
    
    // 设置应用程序样式
    app.setStyle(QStyleFactory::create("Fusion"));
    
    // 设置应用程序图标（如果有的话）
    // app.setWindowIcon(QIcon(":/icons/app.png"));
    
    // 创建启动画面（可选）
    QSplashScreen *splash = nullptr;
    /*
    QPixmap pixmap(":/images/splash.png");
    if (!pixmap.isNull()) {
        splash = new QSplashScreen(pixmap);
        splash->show();
        splash->showMessage("正在启动Markdown转Word工具...", Qt::AlignBottom | Qt::AlignCenter, Qt::white);
        app.processEvents();
    }
    */
    
    // 检查必要的目录
    QString documentsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    if (documentsPath.isEmpty()) {
        QMessageBox::critical(nullptr, "错误", "无法访问文档目录，程序无法正常运行。");
        return -1;
    }
    
    // 创建主窗口
    MainWindowMd2Docx mainWindow;
    
    // 如果有启动画面，延迟显示主窗口
    if (splash) {
        QTimer::singleShot(2000, [&]() {
            splash->finish(&mainWindow);
            mainWindow.show();
            delete splash;
        });
    } else {
        mainWindow.show();
    }
    
    return app.exec();
}
