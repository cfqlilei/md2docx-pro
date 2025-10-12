#include <QApplication>
#include <QDir>
#include <QStandardPaths>
#include <QMessageBox>
#include <QStyleFactory>
#include <QTranslator>
#include <QLibraryInfo>
#include <QLocale>
#include <QSplashScreen>
#include <QPixmap>
#include <QTimer>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // 设置应用程序信息
    app.setApplicationName("MD2DOCX");
    app.setApplicationDisplayName("Markdown转Word工具");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("MD2DOCX");
    app.setOrganizationDomain("md2docx.local");
    
    // 设置应用程序图标
    app.setWindowIcon(QIcon(":/icons/app_icon.png"));
    
    // 设置样式
#ifdef Q_OS_WIN
    app.setStyle(QStyleFactory::create("Fusion"));
#endif
    
    // 设置应用程序样式表
    QString styleSheet = R"(
        QMainWindow {
            background-color: #f5f5f5;
        }
        
        QTabWidget::pane {
            border: 1px solid #c0c0c0;
            background-color: white;
        }
        
        QTabWidget::tab-bar {
            alignment: center;
        }
        
        QTabBar::tab {
            background-color: #e0e0e0;
            border: 1px solid #c0c0c0;
            padding: 8px 16px;
            margin-right: 2px;
        }
        
        QTabBar::tab:selected {
            background-color: white;
            border-bottom: 1px solid white;
        }
        
        QTabBar::tab:hover {
            background-color: #f0f0f0;
        }
        
        QPushButton {
            background-color: #0078d4;
            color: white;
            border: none;
            padding: 8px 16px;
            border-radius: 4px;
            font-weight: bold;
        }
        
        QPushButton:hover {
            background-color: #106ebe;
        }
        
        QPushButton:pressed {
            background-color: #005a9e;
        }
        
        QPushButton:disabled {
            background-color: #cccccc;
            color: #666666;
        }
        
        QLineEdit, QTextEdit {
            border: 1px solid #c0c0c0;
            padding: 6px;
            border-radius: 4px;
            background-color: white;
        }
        
        QLineEdit:focus, QTextEdit:focus {
            border: 2px solid #0078d4;
        }
        
        QGroupBox {
            font-weight: bold;
            border: 2px solid #c0c0c0;
            border-radius: 5px;
            margin-top: 10px;
            padding-top: 10px;
        }
        
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px 0 5px;
        }
        
        QStatusBar {
            background-color: #f0f0f0;
            border-top: 1px solid #c0c0c0;
        }
        
        QProgressBar {
            border: 1px solid #c0c0c0;
            border-radius: 4px;
            text-align: center;
        }
        
        QProgressBar::chunk {
            background-color: #0078d4;
            border-radius: 3px;
        }
    )";
    
    app.setStyleSheet(styleSheet);
    
    // 国际化支持
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "md2docx_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            app.installTranslator(&translator);
            break;
        }
    }
    
    // 显示启动画面
    QPixmap pixmap(":/images/splash.png");
    if (pixmap.isNull()) {
        // 如果没有启动画面图片，创建一个简单的
        pixmap = QPixmap(400, 200);
        pixmap.fill(Qt::white);
    }
    
    QSplashScreen splash(pixmap);
    splash.show();
    splash.showMessage("正在启动 Markdown转Word工具...", Qt::AlignBottom | Qt::AlignCenter, Qt::black);
    
    app.processEvents();
    
    // 模拟启动延迟
    QTimer::singleShot(1000, [&]() {
        splash.showMessage("正在初始化界面...", Qt::AlignBottom | Qt::AlignCenter, Qt::black);
    });
    
    QTimer::singleShot(2000, [&]() {
        splash.showMessage("正在连接后端服务...", Qt::AlignBottom | Qt::AlignCenter, Qt::black);
    });
    
    // 创建主窗口
    MainWindow window;
    
    // 延迟显示主窗口
    QTimer::singleShot(3000, [&]() {
        splash.finish(&window);
        window.show();
    });
    
    return app.exec();
}
