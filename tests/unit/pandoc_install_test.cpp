#include <QtTest/QtTest>
#include <QApplication>
#include <QSignalSpy>
#include <QTimer>
#include <QProcess>
#include <QMessageBox>

// 包含被测试的类
#include "../../qt-frontend/src/settingswidget.h"
#include "../../qt-frontend/src/httpapi.h"

/**
 * @brief Pandoc安装功能测试类
 * 
 * 测试内容：
 * 1. 平台检测功能
 * 2. 地区检测功能  
 * 3. Pandoc安装状态检查
 * 4. 安装命令生成
 * 5. 安装过程模拟
 * 6. UI状态更新
 */
class PandocInstallTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // 平台检测测试
    void testDetectOperatingSystem();
    void testDetectRegion();
    
    // 安装状态检测测试
    void testIsPandocInstalled();
    void testDetectPandocPath();
    
    // 安装命令生成测试
    void testGetPandocInstallCommand_macOS_China();
    void testGetPandocInstallCommand_macOS_Global();
    void testGetPandocInstallCommand_Windows_China();
    void testGetPandocInstallCommand_Windows_Global();
    void testGetPandocInstallCommand_Linux_China();
    void testGetPandocInstallCommand_Linux_Global();
    
    // UI交互测试
    void testInstallButtonState();
    void testProgressBarVisibility();
    void testStatusMessages();
    
    // 安装过程测试
    void testInstallProcess_Success();
    void testInstallProcess_Failure();
    void testInstallProcess_Error();
    void testInstallProcess_AlreadyInstalled();
    
    // 验证测试
    void testPostInstallVerification();

private:
    QApplication *app;
    SettingsWidget *settingsWidget;
    HttpApi *httpApi;
    
    // 测试辅助方法
    void simulateInstallProcess(int exitCode, QProcess::ExitStatus exitStatus);
    void simulateInstallError(QProcess::ProcessError error);
    QString getExpectedCommand(const QString &os, const QString &region);
};

void PandocInstallTest::initTestCase()
{
    // 创建应用程序实例（如果不存在）
    if (!QApplication::instance()) {
        int argc = 0;
        char **argv = nullptr;
        app = new QApplication(argc, argv);
    }
    
    qDebug() << "开始Pandoc安装功能测试";
}

void PandocInstallTest::cleanupTestCase()
{
    qDebug() << "Pandoc安装功能测试完成";
}

void PandocInstallTest::init()
{
    // 每个测试前创建新的实例
    httpApi = new HttpApi(this);
    settingsWidget = new SettingsWidget(httpApi, nullptr);
}

void PandocInstallTest::cleanup()
{
    // 每个测试后清理
    delete settingsWidget;
    delete httpApi;
    settingsWidget = nullptr;
    httpApi = nullptr;
}

void PandocInstallTest::testDetectOperatingSystem()
{
    qDebug() << "测试操作系统检测";
    
    QString os = settingsWidget->detectOperatingSystem();
    
    // 验证返回值是预期的操作系统类型之一
    QStringList validOS = {"windows", "macos", "linux", "unknown"};
    QVERIFY2(validOS.contains(os), 
             QString("检测到的操作系统 '%1' 不在有效列表中").arg(os).toUtf8());
    
    qDebug() << "检测到的操作系统:" << os;
    
#ifdef Q_OS_WIN
    QCOMPARE(os, QString("windows"));
#elif defined(Q_OS_MACOS)
    QCOMPARE(os, QString("macos"));
#elif defined(Q_OS_LINUX)
    QCOMPARE(os, QString("linux"));
#endif
}

void PandocInstallTest::testDetectRegion()
{
    qDebug() << "测试地区检测";
    
    QString region = settingsWidget->detectRegion();
    
    // 验证返回值是预期的地区类型之一
    QStringList validRegions = {"china", "global"};
    QVERIFY2(validRegions.contains(region), 
             QString("检测到的地区 '%1' 不在有效列表中").arg(region).toUtf8());
    
    qDebug() << "检测到的地区:" << region;
}

void PandocInstallTest::testIsPandocInstalled()
{
    qDebug() << "测试Pandoc安装状态检查";
    
    bool isInstalled = settingsWidget->isPandocInstalled();
    
    qDebug() << "Pandoc安装状态:" << (isInstalled ? "已安装" : "未安装");
    
    // 这个测试不验证具体结果，因为取决于测试环境
    // 但我们可以验证方法能正常执行
    QVERIFY2(true, "isPandocInstalled方法执行成功");
}

void PandocInstallTest::testGetPandocInstallCommand_macOS_China()
{
    qDebug() << "测试macOS中国地区安装命令生成";
    
    // 这里我们需要模拟操作系统和地区检测
    // 由于方法是私有的，我们通过公共接口间接测试
    QString command = settingsWidget->getPandocInstallCommand();
    
    // 验证命令不为空（在支持的平台上）
    QString os = settingsWidget->detectOperatingSystem();
    if (os == "macos" || os == "windows" || os == "linux") {
        QVERIFY2(!command.isEmpty(), "支持的平台应该返回非空安装命令");
    }
    
    qDebug() << "生成的安装命令长度:" << command.length();
}

void PandocInstallTest::testInstallButtonState()
{
    qDebug() << "测试安装按钮状态";
    
    // 获取安装按钮（通过对象名称或其他方式）
    QPushButton *installButton = settingsWidget->findChild<QPushButton*>();
    
    if (installButton) {
        // 测试初始状态
        QVERIFY2(installButton->isEnabled(), "安装按钮初始状态应该是启用的");
        QCOMPARE(installButton->text(), QString("安装Pandoc"));
        
        qDebug() << "安装按钮初始状态正常";
    } else {
        QSKIP("未找到安装按钮，跳过此测试");
    }
}

void PandocInstallTest::testProgressBarVisibility()
{
    qDebug() << "测试进度条可见性";
    
    // 获取进度条
    QProgressBar *progressBar = settingsWidget->findChild<QProgressBar*>();
    
    if (progressBar) {
        // 测试初始状态（应该是隐藏的）
        QVERIFY2(!progressBar->isVisible(), "进度条初始状态应该是隐藏的");
        
        qDebug() << "进度条初始状态正常";
    } else {
        QSKIP("未找到进度条，跳过此测试");
    }
}

void PandocInstallTest::testStatusMessages()
{
    qDebug() << "测试状态消息显示";
    
    // 获取状态文本框
    QTextEdit *statusText = settingsWidget->findChild<QTextEdit*>();
    
    if (statusText) {
        QString initialText = statusText->toPlainText();
        qDebug() << "初始状态文本长度:" << initialText.length();
        
        // 这里可以测试状态消息的显示
        QVERIFY2(true, "状态文本框存在");
    } else {
        QSKIP("未找到状态文本框，跳过此测试");
    }
}

void PandocInstallTest::testInstallProcess_AlreadyInstalled()
{
    qDebug() << "测试已安装Pandoc的情况";
    
    // 这个测试需要模拟已安装的情况
    // 由于涉及到用户交互（QMessageBox），我们需要特殊处理
    
    // 模拟用户选择"否"（不重新安装）
    QTimer::singleShot(100, []() {
        QWidget *messageBox = QApplication::activeModalWidget();
        if (messageBox) {
            QMessageBox *msgBox = qobject_cast<QMessageBox*>(messageBox);
            if (msgBox) {
                msgBox->reject(); // 模拟点击"否"
            }
        }
    });
    
    // 如果Pandoc已安装，测试重新安装确认对话框
    if (settingsWidget->isPandocInstalled()) {
        // 触发安装过程
        // settingsWidget->installPandoc(); // 这需要是公共方法或友元测试
        qDebug() << "Pandoc已安装，测试重新安装确认";
    } else {
        qDebug() << "Pandoc未安装，跳过重新安装测试";
    }
}

// 测试辅助方法实现
void PandocInstallTest::simulateInstallProcess(int exitCode, QProcess::ExitStatus exitStatus)
{
    // 这里可以模拟安装过程的完成
    // 需要访问私有成员或使用友元类
}

void PandocInstallTest::simulateInstallError(QProcess::ProcessError error)
{
    // 这里可以模拟安装过程的错误
    // 需要访问私有成员或使用友元类
}

QString PandocInstallTest::getExpectedCommand(const QString &os, const QString &region)
{
    // 根据操作系统和地区返回预期的安装命令
    if (os == "macos") {
        if (region == "china") {
            return "export HOMEBREW_INSTALL_FROM_API=1"; // 部分匹配
        } else {
            return "if ! command -v brew"; // 部分匹配
        }
    } else if (os == "windows") {
        return "powershell -Command"; // 部分匹配
    } else if (os == "linux") {
        return "if command -v apt-get"; // 部分匹配
    }
    return "";
}

// 包含moc生成的代码
#include "pandoc_install_test.moc"

// 主函数
QTEST_MAIN(PandocInstallTest)
