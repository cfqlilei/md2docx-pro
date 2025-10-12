#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include "httpapi.h"

class SimpleMainWindow : public QWidget
{
    Q_OBJECT

public:
    SimpleMainWindow(QWidget *parent = nullptr) : QWidget(parent)
    {
        setWindowTitle("MD2DOCX - 简化版本");
        setMinimumSize(400, 300);
        
        QVBoxLayout *layout = new QVBoxLayout(this);
        
        QLabel *titleLabel = new QLabel("Markdown转Word工具", this);
        titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; margin: 20px;");
        titleLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(titleLabel);
        
        QLabel *statusLabel = new QLabel("简化版本 - 仅测试Qt环境", this);
        statusLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(statusLabel);
        
        QPushButton *testButton = new QPushButton("测试HTTP API", this);
        connect(testButton, &QPushButton::clicked, this, &SimpleMainWindow::testApi);
        layout->addWidget(testButton);
        
        QPushButton *exitButton = new QPushButton("退出", this);
        connect(exitButton, &QPushButton::clicked, this, &QWidget::close);
        layout->addWidget(exitButton);
        
        layout->addStretch();
        
        // 创建HTTP API客户端
        m_httpApi = new HttpApi(this);
        connect(m_httpApi, &HttpApi::healthCheckFinished, 
                this, &SimpleMainWindow::onHealthCheckFinished);
    }

private slots:
    void testApi()
    {
        QMessageBox::information(this, "测试", "正在测试HTTP API连接...");
        m_httpApi->checkHealth();
    }
    
    void onHealthCheckFinished(bool isOnline)
    {
        if (isOnline) {
            QMessageBox::information(this, "成功", "后端服务连接成功！");
        } else {
            QMessageBox::warning(this, "失败", "无法连接到后端服务。\n请确保Go后端正在运行。");
        }
    }

private:
    HttpApi *m_httpApi;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    app.setApplicationName("MD2DOCX");
    app.setApplicationDisplayName("Markdown转Word工具");
    app.setApplicationVersion("1.0.0");
    
    SimpleMainWindow window;
    window.show();
    
    return app.exec();
}

#include "main_simple.moc"
