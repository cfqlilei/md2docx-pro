#include "settingswidget.h"
#include "httpapi.h"

#include <QCheckBox>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QLocale>
#include <QMessageBox>
#include <QProcess>
#include <QProgressBar>
#include <QPushButton>
#include <QStandardPaths>
#include <QSysInfo>
#include <QTextCursor>
#include <QTextEdit>
#include <QThread>
#include <QVBoxLayout>
#include <QWidget>

SettingsWidget::SettingsWidget(HttpApi *api, QWidget *parent)
    : QWidget(parent), m_pandocGroup(nullptr), m_pandocPathEdit(nullptr),
      m_selectPandocButton(nullptr), m_testPandocButton(nullptr),
      m_installPandocButton(nullptr), m_pandocStatusLabel(nullptr),
      m_installProgressBar(nullptr), m_templateGroup(nullptr),
      m_templateFileEdit(nullptr), m_selectTemplateButton(nullptr),
      m_clearTemplateButton(nullptr), m_useTemplateCheckBox(nullptr),
      m_actionGroup(nullptr), m_saveButton(nullptr), m_validateButton(nullptr),
      m_resetButton(nullptr), m_statusGroup(nullptr), m_statusText(nullptr),
      m_httpApi(api), m_configLoaded(false), m_installProcess(nullptr),
      m_isInstalling(false) {
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

  m_installPandocButton = new QPushButton("安装Pandoc", this);
  pandocLayout->addWidget(m_installPandocButton, 0, 4);

  m_pandocStatusLabel = new QLabel("未测试", this);
  m_pandocStatusLabel->setStyleSheet("color: gray;");
  pandocLayout->addWidget(m_pandocStatusLabel, 1, 1, 1, 4);

  // 安装进度条
  m_installProgressBar = new QProgressBar(this);
  m_installProgressBar->setVisible(false);
  pandocLayout->addWidget(m_installProgressBar, 2, 1, 1, 4);

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

  m_saveButton = new QPushButton("保存配置", this);
  actionLayout->addWidget(m_saveButton);

  m_validateButton = new QPushButton("验证配置", this);
  actionLayout->addWidget(m_validateButton);

  m_resetButton = new QPushButton("重置为默认配置", this);
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
  connect(m_installPandocButton, &QPushButton::clicked, this,
          &SettingsWidget::installPandoc);
  connect(m_selectTemplateButton, &QPushButton::clicked, this,
          &SettingsWidget::selectTemplateFile);
  connect(m_clearTemplateButton, &QPushButton::clicked, this,
          &SettingsWidget::clearTemplateFile);
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
  // 询问用户是否确认保存
  QMessageBox::StandardButton reply = QMessageBox::question(
      this, "确认保存", "确定要保存当前配置吗？",
      QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

  if (reply != QMessageBox::Yes) {
    return;
  }

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
  // 询问用户是否确认重置
  QMessageBox::StandardButton reply = QMessageBox::question(
      this, "确认重置", "确定要重置为默认配置吗？\n这将清除当前所有设置。",
      QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

  if (reply != QMessageBox::Yes) {
    return;
  }

  showStatus("正在重置为默认配置...");

  // 清空当前设置
  m_templateFileEdit->clear();
  m_useTemplateCheckBox->setChecked(false);

  // 自动检测pandoc路径
  QString pandocPath = detectPandocPath();
  if (!pandocPath.isEmpty()) {
    m_pandocPathEdit->setText(pandocPath);
    m_pandocStatusLabel->setText("自动检测");
    m_pandocStatusLabel->setStyleSheet("color: blue;");
    showStatus(QString("已自动检测到Pandoc路径: %1").arg(pandocPath));

    // 自动验证检测到的路径
    testPandocPath();
  } else {
    m_pandocPathEdit->clear();
    m_pandocStatusLabel->setText("未找到");
    m_pandocStatusLabel->setStyleSheet("color: red;");
    showStatus("未能自动检测到Pandoc路径，请手动设置");
  }

  updateUI();
}

void SettingsWidget::onPandocPathChanged() {
  m_pandocStatusLabel->setText("未测试");
  m_pandocStatusLabel->setStyleSheet("color: gray;");

  // 如果路径发生变化，提示用户保存配置
  if (m_configLoaded && !m_pandocPathEdit->text().isEmpty()) {
    showStatus("Pandoc路径已更改，请点击'保存配置'使更改生效", false);
  }

  updateUI();
}

void SettingsWidget::onTemplateFileChanged() { updateUI(); }

void SettingsWidget::onConfigReceived(const ConfigData &config) {
  QString pandocPath = config.pandocPath;

  // 如果配置中的Pandoc路径为空，尝试自动检测
  if (pandocPath.isEmpty()) {
    showStatus("配置中Pandoc路径为空，正在自动检测...");
    pandocPath = detectPandocPath();
    if (!pandocPath.isEmpty()) {
      showStatus(QString("自动检测到Pandoc路径: %1").arg(pandocPath));
    } else {
      showStatus("未能自动检测到Pandoc路径，请手动设置", true);
    }
  } else {
    // 验证配置中的路径是否有效
    if (!validatePandocPath(pandocPath)) {
      showStatus(QString("配置中的Pandoc路径无效: %1，正在自动检测...")
                     .arg(pandocPath),
                 true);
      QString detectedPath = detectPandocPath();
      if (!detectedPath.isEmpty()) {
        pandocPath = detectedPath;
        showStatus(QString("自动检测到有效的Pandoc路径: %1").arg(pandocPath));
      } else {
        showStatus("未能自动检测到有效的Pandoc路径，保留原配置", true);
      }
    }
  }

  m_pandocPathEdit->setText(pandocPath);
  m_templateFileEdit->setText(config.templateFile);
  m_useTemplateCheckBox->setChecked(!config.templateFile.isEmpty());

  m_configLoaded = true;
  m_currentPandocPath = pandocPath;
  m_currentTemplateFile = config.templateFile;

  showStatus("配置加载完成");
  updateUI();

  // 自动测试Pandoc路径
  if (!pandocPath.isEmpty()) {
    testPandocPath();
  } else {
    m_pandocStatusLabel->setText("未配置");
    m_pandocStatusLabel->setStyleSheet("color: gray;");
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
    showStatus("✅ 配置验证通过！");
    // 显示详细的验证结果
    QStringList lines = message.split('\n');
    for (const QString &line : lines) {
      if (!line.trimmed().isEmpty()) {
        showStatus(line.trimmed());
      }
    }
  } else {
    showStatus("❌ 配置验证失败！", true);
    // 显示详细的验证结果
    QStringList lines = message.split('\n');
    for (const QString &line : lines) {
      if (!line.trimmed().isEmpty()) {
        bool isError = line.contains("❌");
        showStatus(line.trimmed(), isError);
      }
    }
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
  m_saveButton->setEnabled(isEnabled);
  m_validateButton->setEnabled(isEnabled);
  m_resetButton->setEnabled(isEnabled);

  m_templateFileEdit->setEnabled(isEnabled &&
                                 m_useTemplateCheckBox->isChecked());
}

void SettingsWidget::showStatus(const QString &message, bool isError) {
  QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");

  // 确定图标和颜色
  QString icon;
  QString color;

  if (message.contains("✅") || message.contains("成功") ||
      message.contains("通过")) {
    icon = "✓"; // 使用勾号表示成功
    color = "#388e3c";
  } else if (message.contains("❌") || message.contains("失败") || isError) {
    icon = "✗"; // 使用叉号表示错误
    color = "#d32f2f";
  } else if (message.contains("ℹ️") || message.contains("信息")) {
    icon = "•"; // 使用圆点表示信息
    color = "#1976d2";
  } else {
    icon = isError ? "✗" : "•";
    color = isError ? "#d32f2f" : "#1976d2";
  }

  // 清理消息中的原有图标
  QString cleanMessage = message;
  cleanMessage = cleanMessage.remove("✅").remove("❌").remove("ℹ️").trimmed();

  // 使用div确保每条消息都换行显示，并添加明确的换行
  QString htmlMessage =
      QString("<div style='margin: 4px 0; padding: 4px; line-height: 1.5; "
              "border-left: 3px solid %1; padding-left: 8px; display: block;'>"
              "<span style='color: #666; font-size: 11px;'>[%2]</span> "
              "<span style='color: %3; font-weight: bold; font-size: "
              "15px;'>%4</span> "
              "<span style='font-size: 13px; margin-left: 5px;'>%5</span>"
              "</div><br>")
          .arg(color, timestamp, color, icon, cleanMessage.toHtmlEscaped());

  // 移动到文档末尾并插入HTML
  QTextCursor cursor = m_statusText->textCursor();
  cursor.movePosition(QTextCursor::End);
  m_statusText->setTextCursor(cursor);
  m_statusText->insertHtml(htmlMessage);

  // 确保滚动到底部
  cursor.movePosition(QTextCursor::End);
  m_statusText->setTextCursor(cursor);
  m_statusText->ensureCursorVisible();
}

void SettingsWidget::clearStatus() { m_statusText->clear(); }

QString SettingsWidget::detectPandocPath() {
  // 常见的pandoc安装路径
  QStringList possiblePaths = {
      "/usr/local/bin/pandoc",        // Homebrew on macOS
      "/opt/homebrew/bin/pandoc",     // Homebrew on Apple Silicon
      "/usr/bin/pandoc",              // Linux系统包管理器
      "/usr/local/pandoc/bin/pandoc", // 手动安装
      "pandoc"                        // 系统PATH中
  };

  // 首先尝试通过which命令查找
  QProcess process;
  process.start("which", QStringList() << "pandoc");
  process.waitForFinished(3000);

  if (process.exitCode() == 0) {
    QString path = process.readAllStandardOutput().trimmed();
    if (!path.isEmpty() && QFile::exists(path)) {
      return path;
    }
  }

  // 如果which命令失败，尝试预定义路径
  for (const QString &path : possiblePaths) {
    if (path == "pandoc") {
      // 测试pandoc是否在PATH中
      QProcess testProcess;
      testProcess.start("pandoc", QStringList() << "--version");
      testProcess.waitForFinished(3000);
      if (testProcess.exitCode() == 0) {
        return "pandoc";
      }
    } else if (QFile::exists(path)) {
      return path;
    }
  }

  return QString(); // 未找到
}

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

// Pandoc安装相关方法
QString SettingsWidget::detectOperatingSystem() {
#ifdef Q_OS_WIN
  return "windows";
#elif defined(Q_OS_MACOS)
  return "macos";
#elif defined(Q_OS_LINUX)
  return "linux";
#else
  return "unknown";
#endif
}

QString SettingsWidget::detectRegion() {
  QLocale locale = QLocale::system();
  QString country = QLocale::countryToString(locale.country());

  // 检查是否为中国地区
  if (country == "China" || locale.name().startsWith("zh_CN")) {
    return "china";
  }

  return "global";
}

bool SettingsWidget::isPandocInstalled() {
  QProcess process;
  process.start("pandoc", QStringList() << "--version");
  process.waitForFinished(3000);

  return process.exitCode() == 0;
}

QString SettingsWidget::getPandocInstallCommand() {
  QString os = detectOperatingSystem();
  QString region = detectRegion();

  if (os == "macos") {
    if (region == "china") {
      // 使用中国镜像源 - 改进的安装命令
      return "export HOMEBREW_INSTALL_FROM_API=1 && "
             "export "
             "HOMEBREW_API_DOMAIN=\"https://mirrors.tuna.tsinghua.edu.cn/"
             "homebrew-bottles/api\" && "
             "export "
             "HOMEBREW_BOTTLE_DOMAIN=\"https://mirrors.tuna.tsinghua.edu.cn/"
             "homebrew-bottles\" && "
             "export "
             "HOMEBREW_BREW_GIT_REMOTE=\"https://mirrors.tuna.tsinghua.edu.cn/"
             "git/homebrew/brew.git\" && "
             "export "
             "HOMEBREW_CORE_GIT_REMOTE=\"https://mirrors.tuna.tsinghua.edu.cn/"
             "git/homebrew/homebrew-core.git\" && "
             "if ! command -v brew &> /dev/null; then "
             "/bin/bash -c \"$(curl -fsSL "
             "https://mirrors.tuna.tsinghua.edu.cn/homebrew-install/"
             "install.sh)\"; "
             "fi && "
             "brew install pandoc";
    } else {
      // 使用官方源 - 改进的安装命令
      return "if ! command -v brew &> /dev/null; then "
             "/bin/bash -c \"$(curl -fsSL "
             "https://raw.githubusercontent.com/Homebrew/install/HEAD/"
             "install.sh)\"; "
             "fi && "
             "brew install pandoc";
    }
  } else if (os == "windows") {
    if (region == "china") {
      // Windows中国镜像安装 - 改进的安装命令
      return "powershell -Command \"& {"
             "Set-ExecutionPolicy Bypass -Scope Process -Force; "
             "[System.Net.ServicePointManager]::SecurityProtocol = "
             "[System.Net.ServicePointManager]::SecurityProtocol -bor 3072; "
             "if (!(Get-Command choco -ErrorAction SilentlyContinue)) { "
             "  iex ((New-Object "
             "System.Net.WebClient).DownloadString('https://"
             "mirrors.tuna.tsinghua.edu.cn/chocolatey/install.ps1')); "
             "}; "
             "choco install pandoc -y"
             "}\"";
    } else {
      // Windows官方安装 - 改进的安装命令
      return "powershell -Command \"& {"
             "Set-ExecutionPolicy Bypass -Scope Process -Force; "
             "[System.Net.ServicePointManager]::SecurityProtocol = "
             "[System.Net.ServicePointManager]::SecurityProtocol -bor 3072; "
             "if (!(Get-Command choco -ErrorAction SilentlyContinue)) { "
             "  iex ((New-Object "
             "System.Net.WebClient).DownloadString('https://chocolatey.org/"
             "install.ps1')); "
             "}; "
             "choco install pandoc -y"
             "}\"";
    }
  } else if (os == "linux") {
    if (region == "china") {
      // Linux中国镜像安装
      return "if command -v apt-get &> /dev/null; then "
             "sudo apt-get update && sudo apt-get install -y pandoc; "
             "elif command -v yum &> /dev/null; then "
             "sudo yum install -y pandoc; "
             "elif command -v dnf &> /dev/null; then "
             "sudo dnf install -y pandoc; "
             "elif command -v pacman &> /dev/null; then "
             "sudo pacman -S --noconfirm pandoc; "
             "else "
             "echo '不支持的Linux发行版，请手动安装pandoc'; "
             "fi";
    } else {
      // Linux官方安装
      return "if command -v apt-get &> /dev/null; then "
             "sudo apt-get update && sudo apt-get install -y pandoc; "
             "elif command -v yum &> /dev/null; then "
             "sudo yum install -y pandoc; "
             "elif command -v dnf &> /dev/null; then "
             "sudo dnf install -y pandoc; "
             "elif command -v pacman &> /dev/null; then "
             "sudo pacman -S --noconfirm pandoc; "
             "else "
             "echo '不支持的Linux发行版，请手动安装pandoc'; "
             "fi";
    }
  }

  return "";
}

void SettingsWidget::installPandoc() {
  if (m_isInstalling) {
    showStatus("正在安装中，请等待...", false);
    return;
  }

  // 检查是否已安装
  if (isPandocInstalled()) {
    int ret = QMessageBox::question(
        this, "Pandoc已安装", "检测到系统已安装Pandoc，是否重新安装？",
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (ret != QMessageBox::Yes) {
      return;
    }
  }

  startPandocInstallation();
}

void SettingsWidget::startPandocInstallation() {
  QString command = getPandocInstallCommand();
  if (command.isEmpty()) {
    showStatus("❌ 不支持的操作系统，无法自动安装Pandoc", true);
    return;
  }

  m_isInstalling = true;
  m_installPandocButton->setEnabled(false);
  m_installPandocButton->setText("安装中...");
  m_installProgressBar->setVisible(true);
  m_installProgressBar->setRange(0, 0); // 不确定进度

  // 清空之前的状态信息
  clearStatus();

  QString os = detectOperatingSystem();
  QString region = detectRegion();

  showStatus("🚀 开始安装Pandoc...", false);
  showStatus(QString("💻 操作系统: %1").arg(os), false);
  showStatus(
      QString("🌍 地区: %1")
          .arg(region == "china" ? "中国（使用镜像源）" : "全球（使用官方源）"),
      false);

  // 根据操作系统显示不同的提示信息
  if (os == "macos") {
    showStatus("📝 macOS系统将使用Homebrew安装Pandoc", false);
    if (region == "china") {
      showStatus("🔄 使用清华大学镜像源加速下载", false);
    }
  } else if (os == "windows") {
    showStatus("📝 Windows系统将使用Chocolatey安装Pandoc", false);
    if (region == "china") {
      showStatus("🔄 使用清华大学镜像源加速下载", false);
    }
  } else if (os == "linux") {
    showStatus("📝 Linux系统将使用包管理器安装Pandoc", false);
  }

  showStatus("⏳ 安装过程可能需要几分钟，请耐心等待...", false);

  // 创建安装进程
  if (m_installProcess) {
    m_installProcess->deleteLater();
  }

  m_installProcess = new QProcess(this);

  // 连接信号 - 使用lambda避免类型问题
  connect(m_installProcess,
          static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(
              &QProcess::finished),
          [this](int exitCode, QProcess::ExitStatus exitStatus) {
            onInstallProcessFinished(exitCode, static_cast<int>(exitStatus));
          });
  connect(m_installProcess, &QProcess::errorOccurred,
          [this](QProcess::ProcessError error) {
            onInstallProcessError(static_cast<int>(error));
          });
  connect(m_installProcess, &QProcess::readyReadStandardOutput, this,
          &SettingsWidget::onInstallProcessOutput);
  connect(m_installProcess, &QProcess::readyReadStandardError, this,
          &SettingsWidget::onInstallProcessOutput);

  // 启动安装进程
  if (os == "windows") {
    m_installProcess->start("cmd", QStringList() << "/c" << command);
  } else {
    m_installProcess->start("bash", QStringList() << "-c" << command);
  }
}

void SettingsWidget::onInstallProcessFinished(int exitCode, int exitStatus) {
  m_isInstalling = false;
  m_installPandocButton->setEnabled(true);
  m_installPandocButton->setText("安装Pandoc");
  m_installProgressBar->setVisible(false);

  showStatus("📋 安装过程结束", false);
  showStatus(
      QString("📊 退出状态: %1, 退出码: %2").arg(exitStatus).arg(exitCode),
      false);

  if (exitStatus == 0 && exitCode == 0) { // 0 = NormalExit
    showStatus("🎉 Pandoc安装过程完成！", false);
    showStatus("🔍 正在验证安装结果...", false);

    // 等待一下让系统更新PATH
    QThread::msleep(2000);

    // 验证安装是否成功
    if (isPandocInstalled()) {
      showStatus("✅ Pandoc安装成功，可以正常使用！", false);

      // 自动检测并更新Pandoc路径
      QString pandocPath = detectPandocPath();
      if (!pandocPath.isEmpty()) {
        m_pandocPathEdit->setText(pandocPath);
        m_pandocStatusLabel->setText("✅ 已安装");
        m_pandocStatusLabel->setStyleSheet("color: green; font-weight: bold;");
        showStatus(QString("📍 Pandoc路径: %1").arg(pandocPath), false);
      }

      // 显示版本信息
      QProcess versionProcess;
      versionProcess.start("pandoc", QStringList() << "--version");
      if (versionProcess.waitForFinished(3000)) {
        QString version = versionProcess.readAllStandardOutput();
        QStringList lines = version.split('\n');
        if (!lines.isEmpty()) {
          showStatus(QString("📋 %1").arg(lines.first().trimmed()), false);
        }
      }

      showStatus("🎊 安装完成！现在可以使用Pandoc进行文档转换了。", false);

      // 弹出成功提示
      QMessageBox::information(this, "安装成功",
                               "Pandoc已成功安装！\n\n"
                               "现在您可以使用Markdown转Word功能了。");
    } else {
      showStatus("❌ Pandoc安装可能失败，请检查安装日志", true);
      showStatus("💡 建议：请尝试手动安装Pandoc或检查网络连接", false);

      // 弹出失败提示
      QMessageBox::warning(this, "安装可能失败",
                           "Pandoc安装过程完成，但验证失败。\n\n"
                           "请检查安装日志或尝试手动安装。");
    }
  } else {
    showStatus(QString("❌ Pandoc安装失败，退出码: %1").arg(exitCode), true);
    showStatus("💡 建议：请检查网络连接或尝试手动安装", false);

    // 弹出失败提示
    QMessageBox::critical(this, "安装失败",
                          QString("Pandoc安装失败！\n\n"
                                  "退出码: %1\n"
                                  "请检查安装日志获取详细信息。")
                              .arg(exitCode));
  }
}

void SettingsWidget::onInstallProcessError(int error) {
  m_isInstalling = false;
  m_installPandocButton->setEnabled(true);
  m_installPandocButton->setText("安装Pandoc");
  m_installProgressBar->setVisible(false);

  QString errorMsg;
  QString suggestion;

  switch (error) {
  case 0: // FailedToStart
    errorMsg = "进程启动失败";
    suggestion = "请检查系统权限或尝试以管理员身份运行";
    break;
  case 1: // Crashed
    errorMsg = "进程崩溃";
    suggestion = "可能是网络问题或系统环境问题，请重试";
    break;
  case 2: // Timedout
    errorMsg = "进程超时";
    suggestion = "网络连接可能较慢，请检查网络或稍后重试";
    break;
  case 3: // WriteError
    errorMsg = "写入错误";
    suggestion = "可能是磁盘空间不足或权限问题";
    break;
  case 4: // ReadError
    errorMsg = "读取错误";
    suggestion = "系统I/O错误，请重试";
    break;
  default:
    errorMsg = "未知错误";
    suggestion = "请尝试手动安装Pandoc";
    break;
  }

  showStatus(QString("❌ 安装过程出错: %1").arg(errorMsg), true);
  showStatus(QString("💡 建议: %1").arg(suggestion), false);

  // 弹出错误提示
  QMessageBox::critical(this, "安装错误",
                        QString("Pandoc安装过程中发生错误！\n\n"
                                "错误类型: %1\n"
                                "建议: %2\n\n"
                                "您可以尝试重新安装或手动安装Pandoc。")
                            .arg(errorMsg)
                            .arg(suggestion));
}

void SettingsWidget::onInstallProcessOutput() {
  if (!m_installProcess) {
    return;
  }

  // 读取标准输出
  QByteArray stdOut = m_installProcess->readAllStandardOutput();
  if (!stdOut.isEmpty()) {
    QString output = QString::fromUtf8(stdOut).trimmed();
    if (!output.isEmpty()) {
      showStatus(QString("📝 %1").arg(output), false);
    }
  }

  // 读取错误输出
  QByteArray stdErr = m_installProcess->readAllStandardError();
  if (!stdErr.isEmpty()) {
    QString error = QString::fromUtf8(stdErr).trimmed();
    if (!error.isEmpty()) {
      showStatus(QString("⚠️ %1").arg(error), false);
    }
  }
}
