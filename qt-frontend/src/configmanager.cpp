#include "configmanager.h"
#include "httpapi.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>
#include <QProcess>
#include <QTimer>

ConfigManager::ConfigManager(HttpApi *httpApi, QWidget *parent)
    : QWidget(parent)
    , m_httpApi(httpApi)
    , m_pandocPathEdit(nullptr)
    , m_templateFileEdit(nullptr)
    , m_statusEdit(nullptr)
    , m_loadButton(nullptr)
    , m_saveButton(nullptr)
    , m_validateButton(nullptr)
    , m_resetButton(nullptr)
{
    setupUI();
    setupConnections();
    
    // 自动加载配置
    QTimer::singleShot(1000, this, &ConfigManager::loadConfig);
}

ConfigManager::~ConfigManager()
{
}

void ConfigManager::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Pandoc配置组
    QGroupBox *pandocGroup = new QGroupBox("Pandoc配置", this);
    QGridLayout *pandocLayout = new QGridLayout(pandocGroup);
    
    pandocLayout->addWidget(new QLabel("Pandoc路径:"), 0, 0);
    m_pandocPathEdit = new QLineEdit(this);
    m_pandocPathEdit->setPlaceholderText("Pandoc可执行文件路径（留空自动检测）");
    pandocLayout->addWidget(m_pandocPathEdit, 0, 1);
    
    QPushButton *browsePandocButton = new QPushButton("浏览...", this);
    connect(browsePandocButton, &QPushButton::clicked, this, &ConfigManager::browsePandocPath);
    pandocLayout->addWidget(browsePandocButton, 0, 2);
    
    QPushButton *detectPandocButton = new QPushButton("自动检测", this);
    connect(detectPandocButton, &QPushButton::clicked, this, &ConfigManager::detectPandoc);
    pandocLayout->addWidget(detectPandocButton, 0, 3);
    
    mainLayout->addWidget(pandocGroup);
    
    // 模板配置组
    QGroupBox *templateGroup = new QGroupBox("模板配置", this);
    QGridLayout *templateLayout = new QGridLayout(templateGroup);
    
    templateLayout->addWidget(new QLabel("参考模板:"), 0, 0);
    m_templateFileEdit = new QLineEdit(this);
    m_templateFileEdit->setPlaceholderText("可选：DOCX模板文件路径");
    templateLayout->addWidget(m_templateFileEdit, 0, 1);
    
    QPushButton *browseTemplateButton = new QPushButton("浏览...", this);
    connect(browseTemplateButton, &QPushButton::clicked, this, &ConfigManager::browseTemplateFile);
    templateLayout->addWidget(browseTemplateButton, 0, 2);
    
    QPushButton *clearTemplateButton = new QPushButton("清空", this);
    connect(clearTemplateButton, &QPushButton::clicked, [this]() {
        m_templateFileEdit->clear();
    });
    templateLayout->addWidget(clearTemplateButton, 0, 3);
    
    mainLayout->addWidget(templateGroup);
    
    // 操作按钮
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    m_loadButton = new QPushButton("加载配置", this);
    connect(m_loadButton, &QPushButton::clicked, this, &ConfigManager::loadConfig);
    buttonLayout->addWidget(m_loadButton);
    
    m_saveButton = new QPushButton("保存配置", this);
    connect(m_saveButton, &QPushButton::clicked, this, &ConfigManager::saveConfig);
    buttonLayout->addWidget(m_saveButton);
    
    m_validateButton = new QPushButton("验证配置", this);
    connect(m_validateButton, &QPushButton::clicked, this, &ConfigManager::validateConfig);
    buttonLayout->addWidget(m_validateButton);
    
    m_resetButton = new QPushButton("重置默认", this);
    connect(m_resetButton, &QPushButton::clicked, this, &ConfigManager::resetToDefaults);
    buttonLayout->addWidget(m_resetButton);
    
    buttonLayout->addStretch();
    mainLayout->addLayout(buttonLayout);
    
    // 状态显示区域
    QGroupBox *statusGroup = new QGroupBox("配置状态", this);
    QVBoxLayout *statusLayout = new QVBoxLayout(statusGroup);
    
    m_statusEdit = new QTextEdit(this);
    m_statusEdit->setReadOnly(true);
    m_statusEdit->setMaximumHeight(150);
    m_statusEdit->setPlaceholderText("配置状态和验证结果将在这里显示...");
    statusLayout->addWidget(m_statusEdit);
    
    mainLayout->addWidget(statusGroup);
    
    // 帮助信息
    QGroupBox *helpGroup = new QGroupBox("帮助信息", this);
    QVBoxLayout *helpLayout = new QVBoxLayout(helpGroup);
    
    QLabel *helpLabel = new QLabel(
        "<b>配置说明:</b><br>"
        "• <b>Pandoc路径:</b> 指定Pandoc可执行文件的完整路径。如果留空，系统会自动在PATH中查找。<br>"
        "• <b>参考模板:</b> 可选的DOCX模板文件，用于控制输出文档的样式和格式。<br>"
        "• 点击'验证配置'可以检查当前配置是否正确。<br>"
        "• 配置保存后会立即生效。",
        this
    );
    helpLabel->setWordWrap(true);
    helpLabel->setStyleSheet("QLabel { color: #666; font-size: 12px; }");
    helpLayout->addWidget(helpLabel);
    
    mainLayout->addWidget(helpGroup);
    
    // 添加弹性空间
    mainLayout->addStretch();
}

void ConfigManager::setupConnections()
{
    // 连接HTTP API信号
    connect(m_httpApi, &HttpApi::configReceived,
            this, &ConfigManager::onConfigReceived);
    connect(m_httpApi, &HttpApi::configUpdated,
            this, &ConfigManager::onConfigUpdated);
    connect(m_httpApi, &HttpApi::configValidated,
            this, &ConfigManager::onConfigValidated);
}

void ConfigManager::browsePandocPath()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "选择Pandoc可执行文件",
        "/usr/local/bin",
#ifdef Q_OS_WIN
        "可执行文件 (*.exe);;所有文件 (*)"
#else
        "所有文件 (*)"
#endif
    );
    
    if (!fileName.isEmpty()) {
        m_pandocPathEdit->setText(fileName);
    }
}

void ConfigManager::browseTemplateFile()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "选择DOCX模板文件",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        "Word文档 (*.docx);;所有文件 (*)"
    );
    
    if (!fileName.isEmpty()) {
        m_templateFileEdit->setText(fileName);
    }
}

void ConfigManager::detectPandoc()
{
    m_statusEdit->append("正在自动检测Pandoc...");
    
    // 尝试在常见位置查找Pandoc
    QStringList possiblePaths;
    
#ifdef Q_OS_WIN
    possiblePaths << "pandoc.exe"
                  << "C:/Program Files/Pandoc/pandoc.exe"
                  << "C:/Program Files (x86)/Pandoc/pandoc.exe"
                  << "C:/Users/" + qgetenv("USERNAME") + "/AppData/Local/Pandoc/pandoc.exe";
#elif defined(Q_OS_MAC)
    possiblePaths << "pandoc"
                  << "/usr/local/bin/pandoc"
                  << "/opt/homebrew/bin/pandoc"
                  << "/usr/bin/pandoc";
#else
    possiblePaths << "pandoc"
                  << "/usr/local/bin/pandoc"
                  << "/usr/bin/pandoc"
                  << "/snap/bin/pandoc";
#endif
    
    for (const QString &path : possiblePaths) {
        QProcess process;
        process.start(path, QStringList() << "--version");
        
        if (process.waitForFinished(3000) && process.exitCode() == 0) {
            QString output = process.readAllStandardOutput();
            if (output.contains("pandoc")) {
                m_pandocPathEdit->setText(path);
                m_statusEdit->append(QString("✅ 找到Pandoc: %1").arg(path));
                
                // 显示版本信息
                QStringList lines = output.split('\n');
                if (!lines.isEmpty()) {
                    m_statusEdit->append(QString("版本: %1").arg(lines.first().trimmed()));
                }
                return;
            }
        }
    }
    
    m_statusEdit->append("❌ 未找到Pandoc，请手动指定路径");
}

void ConfigManager::loadConfig()
{
    m_statusEdit->append("正在加载配置...");
    m_httpApi->getConfig();
}

void ConfigManager::saveConfig()
{
    QString pandocPath = m_pandocPathEdit->text().trimmed();
    QString templateFile = m_templateFileEdit->text().trimmed();
    
    m_statusEdit->append("正在保存配置...");
    m_httpApi->updateConfig(pandocPath, templateFile);
}

void ConfigManager::validateConfig()
{
    m_statusEdit->append("正在验证配置...");
    m_httpApi->validateConfig();
}

void ConfigManager::resetToDefaults()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "重置配置",
        "确定要重置为默认配置吗？这将清空所有当前设置。",
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        m_pandocPathEdit->clear();
        m_templateFileEdit->clear();
        m_statusEdit->append("配置已重置为默认值");
        
        // 自动检测Pandoc
        detectPandoc();
    }
}

void ConfigManager::onConfigReceived(bool success, const QString &pandocPath, 
                                   const QString &templateFile, const QString &error)
{
    if (success) {
        m_pandocPathEdit->setText(pandocPath);
        m_templateFileEdit->setText(templateFile);
        m_statusEdit->append("✅ 配置加载成功");
        
        if (!pandocPath.isEmpty()) {
            m_statusEdit->append(QString("Pandoc路径: %1").arg(pandocPath));
        }
        if (!templateFile.isEmpty()) {
            m_statusEdit->append(QString("模板文件: %1").arg(templateFile));
        }
    } else {
        m_statusEdit->append("❌ 配置加载失败");
        m_statusEdit->append(QString("错误: %1").arg(error));
    }
}

void ConfigManager::onConfigUpdated(bool success, const QString &message)
{
    if (success) {
        m_statusEdit->append("✅ 配置保存成功");
        emit configChanged();
    } else {
        m_statusEdit->append("❌ 配置保存失败");
        m_statusEdit->append(QString("错误: %1").arg(message));
    }
}

void ConfigManager::onConfigValidated(bool success, const QString &message)
{
    if (success) {
        m_statusEdit->append("✅ 配置验证通过");
        m_statusEdit->append("所有配置项都正确，可以正常使用转换功能");
    } else {
        m_statusEdit->append("❌ 配置验证失败");
        m_statusEdit->append(QString("错误: %1").arg(message));
        m_statusEdit->append("请检查并修正配置后重试");
    }
}
