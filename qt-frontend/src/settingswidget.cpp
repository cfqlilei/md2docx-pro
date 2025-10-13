#include "settingswidget.h"
#include "httpapi.h"

#include <QCheckBox>
#include <QDateTime>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QProcess>
#include <QPushButton>
#include <QStandardPaths>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

SettingsWidget::SettingsWidget(HttpApi *api, QWidget *parent)
    : QWidget(parent), m_pandocGroup(nullptr), m_pandocPathEdit(nullptr),
      m_selectPandocButton(nullptr), m_testPandocButton(nullptr),
      m_pandocStatusLabel(nullptr), m_templateGroup(nullptr),
      m_templateFileEdit(nullptr), m_selectTemplateButton(nullptr),
      m_clearTemplateButton(nullptr), m_useTemplateCheckBox(nullptr),
      m_actionGroup(nullptr), m_loadButton(nullptr), m_saveButton(nullptr),
      m_validateButton(nullptr), m_resetButton(nullptr), m_statusGroup(nullptr),
      m_statusText(nullptr), m_httpApi(api), m_configLoaded(false) {
  setupUI();
  setupConnections();
  updateUI();
  loadConfig(); // 自动加载配置
}

SettingsWidget::~SettingsWidget() {}

void SettingsWidget::setupUI() {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);

  // Pandoc配置组
  m_pandocGroup = new QGroupBox("Pandoc配置", this);
  QGridLayout *pandocLayout = new QGridLayout(m_pandocGroup);

  pandocLayout->addWidget(new QLabel("Pandoc路径:"), 0, 0);
  m_pandocPathEdit = new QLineEdit(this);
  m_pandocPathEdit->setPlaceholderText("留空使用系统默认路径");
  pandocLayout->addWidget(m_pandocPathEdit, 0, 1);

  m_selectPandocButton = new QPushButton("浏览...", this);
  pandocLayout->addWidget(m_selectPandocButton, 0, 2);

  m_testPandocButton = new QPushButton("测试", this);
  pandocLayout->addWidget(m_testPandocButton, 0, 3);

  m_pandocStatusLabel = new QLabel("未测试", this);
  m_pandocStatusLabel->setStyleSheet("color: gray;");
  pandocLayout->addWidget(m_pandocStatusLabel, 1, 1, 1, 3);

  mainLayout->addWidget(m_pandocGroup);

  // 模板配置组
  m_templateGroup = new QGroupBox("转换模板", this);
  QGridLayout *templateLayout = new QGridLayout(m_templateGroup);

  m_useTemplateCheckBox = new QCheckBox("使用自定义模板", this);
  templateLayout->addWidget(m_useTemplateCheckBox, 0, 0, 1, 4);

  templateLayout->addWidget(new QLabel("模板文件:"), 1, 0);
  m_templateFileEdit = new QLineEdit(this);
  m_templateFileEdit->setPlaceholderText("选择Word模板文件(.docx)");
  m_templateFileEdit->setEnabled(false);
  templateLayout->addWidget(m_templateFileEdit, 1, 1);

  m_selectTemplateButton = new QPushButton("浏览...", this);
  m_selectTemplateButton->setEnabled(false);
  templateLayout->addWidget(m_selectTemplateButton, 1, 2);

  m_clearTemplateButton = new QPushButton("清空", this);
  m_clearTemplateButton->setEnabled(false);
  templateLayout->addWidget(m_clearTemplateButton, 1, 3);

  mainLayout->addWidget(m_templateGroup);

  // 操作按钮组
  m_actionGroup = new QGroupBox("操作", this);
  QHBoxLayout *actionLayout = new QHBoxLayout(m_actionGroup);

  m_loadButton = new QPushButton("加载配置", this);
  actionLayout->addWidget(m_loadButton);

  m_saveButton = new QPushButton("保存配置", this);
  actionLayout->addWidget(m_saveButton);

  m_validateButton = new QPushButton("验证配置", this);
  actionLayout->addWidget(m_validateButton);

  m_resetButton = new QPushButton("重置默认", this);
  actionLayout->addWidget(m_resetButton);

  actionLayout->addStretch();
  mainLayout->addWidget(m_actionGroup);

  // 状态显示组
  m_statusGroup = new QGroupBox("状态信息", this);
  QVBoxLayout *statusLayout = new QVBoxLayout(m_statusGroup);

  m_statusText = new QTextEdit(this);
  m_statusText->setMaximumHeight(120);
  m_statusText->setReadOnly(true);
  m_statusText->setPlaceholderText("配置状态和操作结果将在这里显示...");
  statusLayout->addWidget(m_statusText);

  mainLayout->addWidget(m_statusGroup);

  // 添加弹性空间
  mainLayout->addStretch();
}

void SettingsWidget::setupConnections() {
  // 按钮连接
  connect(m_selectPandocButton, &QPushButton::clicked, this,
          &SettingsWidget::selectPandocPath);
  connect(m_testPandocButton, &QPushButton::clicked, this,
          &SettingsWidget::testPandocPath);
  connect(m_selectTemplateButton, &QPushButton::clicked, this,
          &SettingsWidget::selectTemplateFile);
  connect(m_clearTemplateButton, &QPushButton::clicked, this,
          &SettingsWidget::clearTemplateFile);
  connect(m_loadButton, &QPushButton::clicked, this,
          &SettingsWidget::loadCurrentConfig);
  connect(m_saveButton, &QPushButton::clicked, this,
          &SettingsWidget::saveConfig);
  connect(m_validateButton, &QPushButton::clicked, this,
          &SettingsWidget::validateConfig);
  connect(m_resetButton, &QPushButton::clicked, this,
          &SettingsWidget::resetToDefaults);

  // 输入框变化连接
  connect(m_pandocPathEdit, &QLineEdit::textChanged, this,
          &SettingsWidget::onPandocPathChanged);
  connect(m_templateFileEdit, &QLineEdit::textChanged, this,
          &SettingsWidget::onTemplateFileChanged);
  connect(m_useTemplateCheckBox, &QCheckBox::toggled, this,
          [this](bool checked) {
            m_templateFileEdit->setEnabled(checked);
            m_selectTemplateButton->setEnabled(checked);
            m_clearTemplateButton->setEnabled(checked);
            updateUI();
          });

  // HTTP API连接
  if (m_httpApi) {
    connect(m_httpApi, &HttpApi::configReceived, this,
            &SettingsWidget::onConfigReceived);
    connect(m_httpApi, &HttpApi::configUpdated, this,
            &SettingsWidget::onConfigUpdated);
    connect(m_httpApi, &HttpApi::configValidated, this,
            &SettingsWidget::onConfigValidated);
  }
}

void SettingsWidget::selectPandocPath() {
  QString fileName = QFileDialog::getOpenFileName(
      this, "选择Pandoc可执行文件",
      m_pandocPathEdit->text().isEmpty()
          ? "/usr/local/bin"
          : QFileInfo(m_pandocPathEdit->text()).absolutePath(),
      "可执行文件 (*);;所有文件 (*)");

  if (!fileName.isEmpty()) {
    m_pandocPathEdit->setText(fileName);
    showStatus(QString("已选择Pandoc路径: %1").arg(fileName));
    updateUI();
  }
}

void SettingsWidget::testPandocPath() {
  QString pandocPath =
      m_pandocPathEdit->text().isEmpty() ? "pandoc" : m_pandocPathEdit->text();

  showStatus("正在测试Pandoc...");
  m_pandocStatusLabel->setText("测试中...");
  m_pandocStatusLabel->setStyleSheet("color: orange;");

  QProcess process;
  process.start(pandocPath, QStringList() << "--version");

  if (process.waitForFinished(5000)) {
    if (process.exitCode() == 0) {
      QString output = process.readAllStandardOutput();
      QString version = output.split('\n').first();
      m_pandocStatusLabel->setText(QString("✅ %1").arg(version));
      m_pandocStatusLabel->setStyleSheet("color: green;");
      showStatus(QString("Pandoc测试成功: %1").arg(version));
    } else {
      m_pandocStatusLabel->setText("❌ 测试失败");
      m_pandocStatusLabel->setStyleSheet("color: red;");
      showStatus("Pandoc测试失败: 程序返回错误", true);
    }
  } else {
    m_pandocStatusLabel->setText("❌ 无法执行");
    m_pandocStatusLabel->setStyleSheet("color: red;");
    showStatus("Pandoc测试失败: 无法执行程序或超时", true);
  }

  updateUI();
}

void SettingsWidget::selectTemplateFile() {
  QString fileName = QFileDialog::getOpenFileName(
      this, "选择Word模板文件",
      m_templateFileEdit->text().isEmpty()
          ? QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
          : QFileInfo(m_templateFileEdit->text()).absolutePath(),
      "Word文档 (*.docx);;所有文件 (*)");

  if (!fileName.isEmpty()) {
    m_templateFileEdit->setText(fileName);
    showStatus(
        QString("已选择模板文件: %1").arg(QFileInfo(fileName).fileName()));
    updateUI();
  }
}

void SettingsWidget::clearTemplateFile() {
  m_templateFileEdit->clear();
  showStatus("已清空模板文件");
  updateUI();
}

void SettingsWidget::loadCurrentConfig() {
  showStatus("正在加载配置...");
  if (m_httpApi) {
    m_httpApi->getConfig();
  }
}

void SettingsWidget::saveConfig() {
  showStatus("正在保存配置...");
  if (m_httpApi) {
    ConfigData config;
    config.pandocPath = m_pandocPathEdit->text();
    config.templateFile =
        m_useTemplateCheckBox->isChecked() ? m_templateFileEdit->text() : "";
    config.serverPort = 8080; // 默认端口
    m_httpApi->updateConfig(config);
  }
}

void SettingsWidget::validateConfig() {
  showStatus("正在验证配置...");
  if (m_httpApi) {
    m_httpApi->validateConfig();
  }
}

void SettingsWidget::resetToDefaults() {
  m_pandocPathEdit->clear();
  m_templateFileEdit->clear();
  m_useTemplateCheckBox->setChecked(false);
  m_pandocStatusLabel->setText("未测试");
  m_pandocStatusLabel->setStyleSheet("color: gray;");

  showStatus("已重置为默认设置");
  updateUI();
}

void SettingsWidget::onPandocPathChanged() {
  m_pandocStatusLabel->setText("未测试");
  m_pandocStatusLabel->setStyleSheet("color: gray;");
  updateUI();
}

void SettingsWidget::onTemplateFileChanged() { updateUI(); }

void SettingsWidget::onConfigReceived(const ConfigData &config) {
  m_pandocPathEdit->setText(config.pandocPath);
  m_templateFileEdit->setText(config.templateFile);
  m_useTemplateCheckBox->setChecked(!config.templateFile.isEmpty());

  m_configLoaded = true;
  m_currentPandocPath = config.pandocPath;
  m_currentTemplateFile = config.templateFile;

  showStatus("配置加载成功");
  updateUI();

  // 自动测试Pandoc
  if (!config.pandocPath.isEmpty()) {
    testPandocPath();
  }
}

void SettingsWidget::onConfigUpdated(bool success, const QString &message) {
  if (success) {
    showStatus("配置保存成功");
    m_currentPandocPath = m_pandocPathEdit->text();
    m_currentTemplateFile =
        m_useTemplateCheckBox->isChecked() ? m_templateFileEdit->text() : "";
    emit configChanged();
  } else {
    showStatus(QString("配置保存失败: %1").arg(message), true);
  }
  updateUI();
}

void SettingsWidget::onConfigValidated(bool success, const QString &message) {
  if (success) {
    showStatus("配置验证成功");
  } else {
    showStatus(QString("配置验证失败: %1").arg(message), true);
  }
  updateUI();
}

void SettingsWidget::setEnabled(bool enabled) {
  QWidget::setEnabled(enabled);
  updateUI();
}

void SettingsWidget::loadConfig() { loadCurrentConfig(); }

void SettingsWidget::resetForm() { resetToDefaults(); }

void SettingsWidget::updateUI() {
  bool isEnabled = this->isEnabled();

  m_selectPandocButton->setEnabled(isEnabled);
  m_testPandocButton->setEnabled(isEnabled &&
                                 !m_pandocPathEdit->text().isEmpty());
  m_selectTemplateButton->setEnabled(isEnabled &&
                                     m_useTemplateCheckBox->isChecked());
  m_clearTemplateButton->setEnabled(isEnabled &&
                                    m_useTemplateCheckBox->isChecked() &&
                                    !m_templateFileEdit->text().isEmpty());
  m_loadButton->setEnabled(isEnabled);
  m_saveButton->setEnabled(isEnabled);
  m_validateButton->setEnabled(isEnabled);
  m_resetButton->setEnabled(isEnabled);

  m_templateFileEdit->setEnabled(isEnabled &&
                                 m_useTemplateCheckBox->isChecked());
}

void SettingsWidget::showStatus(const QString &message, bool isError) {
  QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
  QString prefix = isError ? "❌" : "ℹ️";
  m_statusText->append(QString("[%1] %2 %3").arg(timestamp, prefix, message));
}

void SettingsWidget::clearStatus() { m_statusText->clear(); }

bool SettingsWidget::validatePandocPath(const QString &path) {
  if (path.isEmpty()) {
    return true; // 空路径表示使用系统默认
  }

  QFileInfo fileInfo(path);
  return fileInfo.exists() && fileInfo.isExecutable();
}

bool SettingsWidget::validateTemplateFile(const QString &path) {
  if (path.isEmpty()) {
    return true; // 空路径表示不使用模板
  }

  QFileInfo fileInfo(path);
  return fileInfo.exists() && fileInfo.isFile() &&
         path.endsWith(".docx", Qt::CaseInsensitive);
}
