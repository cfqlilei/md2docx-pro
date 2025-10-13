#include "singlefileconverter.h"
#include "appsettings.h"
#include "httpapi.h"

#include <QDateTime>
#include <QDesktopServices>
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
#include <QProgressBar>
#include <QPushButton>
#include <QStandardPaths>
#include <QTextEdit>
#include <QUrl>
#include <QVBoxLayout>
#include <QWidget>

SingleFileConverter::SingleFileConverter(HttpApi *api, QWidget *parent)
    : QWidget(parent), m_inputGroup(nullptr), m_inputFileEdit(nullptr),
      m_selectInputButton(nullptr), m_outputGroup(nullptr),
      m_outputDirEdit(nullptr), m_selectOutputButton(nullptr),
      m_outputNameEdit(nullptr), m_actionGroup(nullptr),
      m_convertButton(nullptr), m_clearButton(nullptr), m_statusGroup(nullptr),
      m_statusText(nullptr), m_progressBar(nullptr), m_httpApi(api),
      m_conversionInProgress(false) {
  setupUI();
  setupConnections();
  updateUI();
}

SingleFileConverter::~SingleFileConverter() {}

void SingleFileConverter::setupUI() {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);

  // 输入文件组
  m_inputGroup = new QGroupBox("输入文件", this);
  QGridLayout *inputLayout = new QGridLayout(m_inputGroup);

  inputLayout->addWidget(new QLabel("Markdown文件:"), 0, 0);
  m_inputFileEdit = new QLineEdit(this);
  m_inputFileEdit->setPlaceholderText("选择要转换的Markdown文件");
  m_inputFileEdit->setReadOnly(true);
  inputLayout->addWidget(m_inputFileEdit, 0, 1);

  m_selectInputButton = new QPushButton("浏览...", this);
  inputLayout->addWidget(m_selectInputButton, 0, 2);

  mainLayout->addWidget(m_inputGroup);

  // 输出设置组
  m_outputGroup = new QGroupBox("输出设置", this);
  QGridLayout *outputLayout = new QGridLayout(m_outputGroup);

  outputLayout->addWidget(new QLabel("保存路径:"), 0, 0);
  m_outputDirEdit = new QLineEdit(this);
  m_outputDirEdit->setPlaceholderText("留空则保存到源文件目录");
  outputLayout->addWidget(m_outputDirEdit, 0, 1);

  m_selectOutputButton = new QPushButton("浏览...", this);
  outputLayout->addWidget(m_selectOutputButton, 0, 2);

  outputLayout->addWidget(new QLabel("文件名:"), 1, 0);
  m_outputNameEdit = new QLineEdit(this);
  m_outputNameEdit->setPlaceholderText("留空则使用源文件名");
  outputLayout->addWidget(m_outputNameEdit, 1, 1, 1, 2);

  mainLayout->addWidget(m_outputGroup);

  // 操作按钮组
  m_actionGroup = new QGroupBox("操作", this);
  QHBoxLayout *actionLayout = new QHBoxLayout(m_actionGroup);

  m_convertButton = new QPushButton("开始转换", this);
  m_convertButton->setEnabled(false);
  actionLayout->addWidget(m_convertButton);

  m_clearButton = new QPushButton("清空", this);
  actionLayout->addWidget(m_clearButton);

  actionLayout->addStretch();
  mainLayout->addWidget(m_actionGroup);

  // 状态显示组
  m_statusGroup = new QGroupBox("转换状态", this);
  QVBoxLayout *statusLayout = new QVBoxLayout(m_statusGroup);

  m_statusText = new QTextEdit(this);
  m_statusText->setMaximumHeight(120);
  m_statusText->setReadOnly(true);
  m_statusText->setPlaceholderText("转换状态和结果将在这里显示...");
  statusLayout->addWidget(m_statusText);

  m_progressBar = new QProgressBar(this);
  m_progressBar->setVisible(false);
  statusLayout->addWidget(m_progressBar);

  mainLayout->addWidget(m_statusGroup);

  // 添加弹性空间
  mainLayout->addStretch();
}

void SingleFileConverter::setupConnections() {
  // 按钮连接
  connect(m_selectInputButton, &QPushButton::clicked, this,
          &SingleFileConverter::selectInputFile);
  connect(m_selectOutputButton, &QPushButton::clicked, this,
          &SingleFileConverter::selectOutputDir);
  connect(m_convertButton, &QPushButton::clicked, this,
          &SingleFileConverter::startConversion);
  connect(m_clearButton, &QPushButton::clicked, this,
          &SingleFileConverter::clearAll);

  // 输入框变化连接
  connect(m_inputFileEdit, &QLineEdit::textChanged, this,
          &SingleFileConverter::onInputFileChanged);
  connect(m_outputDirEdit, &QLineEdit::textChanged, this,
          &SingleFileConverter::onOutputDirChanged);
  connect(m_outputNameEdit, &QLineEdit::textChanged, this,
          &SingleFileConverter::onOutputNameChanged);

  // HTTP API连接
  if (m_httpApi) {
    connect(m_httpApi, &HttpApi::singleConversionFinished, this,
            &SingleFileConverter::onConversionFinished);
  }
}

void SingleFileConverter::selectInputFile() {
  AppSettings *settings = AppSettings::instance();
  QString lastDir = settings->getLastInputDir();

  QString fileName = QFileDialog::getOpenFileName(
      this, "选择Markdown文件", lastDir,
      "Markdown文件 (*.md *.markdown *.mdown *.mkd);;所有文件 (*)");

  if (!fileName.isEmpty()) {
    m_inputFileEdit->setText(fileName);
    m_lastInputFile = fileName;

    // 保存目录到配置
    QFileInfo fileInfo(fileName);
    settings->setLastInputDir(fileInfo.absolutePath());
    settings->addRecentFile(fileName);

    // 自动设置输出文件名
    if (m_outputNameEdit->text().isEmpty()) {
      m_outputNameEdit->setText(getDefaultOutputName());
    }

    updateUI();
    showStatus(QString("已选择文件: %1").arg(fileInfo.fileName()));
  }
}

void SingleFileConverter::selectOutputDir() {
  AppSettings *settings = AppSettings::instance();
  QString lastDir = settings->getLastOutputDir();

  QString dirName =
      QFileDialog::getExistingDirectory(this, "选择保存目录", lastDir);

  if (!dirName.isEmpty()) {
    m_outputDirEdit->setText(dirName);
    m_lastOutputDir = dirName;

    // 保存目录到配置
    settings->setLastOutputDir(dirName);

    updateUI();
    showStatus(QString("已选择保存目录: %1").arg(dirName));
  }
}

void SingleFileConverter::startConversion() {
  if (!validateInputs()) {
    return;
  }

  QString inputFile = m_inputFileEdit->text();
  QString outputPath = getOutputFilePath();

  m_conversionInProgress = true;
  m_progressBar->setVisible(true);
  m_progressBar->setRange(0, 0); // 不确定进度
  m_convertButton->setEnabled(false);
  m_convertButton->setText("转换中...");

  showStatus("开始转换...");
  emit conversionStarted();

  // 调用HTTP API进行转换
  if (m_httpApi) {
    ConversionRequest request;
    request.inputFile = inputFile;
    request.outputDir = QFileInfo(outputPath).absolutePath();
    request.outputName = QFileInfo(outputPath).fileName();
    request.templateFile = ""; // 暂时不使用模板
    m_httpApi->convertSingle(request);
  }
}

void SingleFileConverter::clearAll() {
  m_inputFileEdit->clear();
  m_outputDirEdit->clear();
  m_outputNameEdit->clear();
  clearStatus();
  updateUI();
  showStatus("已清空所有输入");
}

void SingleFileConverter::onInputFileChanged() { updateUI(); }

void SingleFileConverter::onOutputDirChanged() { updateUI(); }

void SingleFileConverter::onOutputNameChanged() { updateUI(); }

void SingleFileConverter::onConversionFinished(
    const ConversionResponse &response) {
  m_conversionInProgress = false;
  m_progressBar->setVisible(false);
  m_convertButton->setEnabled(true);
  m_convertButton->setText("开始转换");

  if (response.success) {
    showStatus(QString("✅ 转换成功！\n输出文件: %1").arg(response.outputFile));

    // 询问是否打开文件所在目录
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "转换完成",
        QString("转换成功！\n输出文件: %1\n\n是否打开文件所在目录？")
            .arg(QFileInfo(response.outputFile).fileName()),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
      QFileInfo fileInfo(response.outputFile);
      QString dirPath = fileInfo.absolutePath();

#ifdef Q_OS_MAC
      // macOS使用open命令
      QProcess::startDetached("open", QStringList() << dirPath);
#elif defined(Q_OS_WIN)
      // Windows使用explorer
      QProcess::startDetached(
          "explorer", QStringList() << QDir::toNativeSeparators(dirPath));
#else
      // Linux使用xdg-open
      QProcess::startDetached("xdg-open", QStringList() << dirPath);
#endif
    }
  } else {
    showStatus(QString("❌ 转换失败！\n错误信息: %1").arg(response.message),
               true);
  }

  emit conversionFinished(response.success, response.message);
}

void SingleFileConverter::setEnabled(bool enabled) {
  QWidget::setEnabled(enabled);
  if (!enabled) {
    m_conversionInProgress = false;
    m_progressBar->setVisible(false);
    m_convertButton->setText("开始转换");
  }
  updateUI();
}

void SingleFileConverter::resetForm() { clearAll(); }

void SingleFileConverter::updateUI() {
  bool hasInputFile = !m_inputFileEdit->text().isEmpty();
  bool canConvert = hasInputFile && !m_conversionInProgress && isEnabled();

  m_convertButton->setEnabled(canConvert);
  m_selectInputButton->setEnabled(!m_conversionInProgress && isEnabled());
  m_selectOutputButton->setEnabled(!m_conversionInProgress && isEnabled());
  m_clearButton->setEnabled(!m_conversionInProgress && isEnabled());
  m_outputNameEdit->setEnabled(!m_conversionInProgress && isEnabled());
}

bool SingleFileConverter::validateInputs() {
  // 基本验证在updateUI中已经完成
  return !m_inputFileEdit->text().isEmpty();
}

void SingleFileConverter::showStatus(const QString &message, bool isError) {
  QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
  QString prefix = isError ? "❌" : "ℹ️";
  m_statusText->append(QString("[%1] %2 %3").arg(timestamp, prefix, message));
}

void SingleFileConverter::clearStatus() { m_statusText->clear(); }

QString SingleFileConverter::getDefaultOutputName() const {
  if (m_inputFileEdit->text().isEmpty()) {
    return QString();
  }

  QFileInfo fileInfo(m_inputFileEdit->text());
  return fileInfo.baseName(); // 只返回文件名，不包含扩展名
}

QString SingleFileConverter::getOutputFilePath() const {
  QString outputDir = m_outputDirEdit->text();
  QString outputName = m_outputNameEdit->text();

  // 如果没有指定输出目录，使用输入文件所在目录
  if (outputDir.isEmpty()) {
    QFileInfo inputInfo(m_inputFileEdit->text());
    outputDir = inputInfo.absolutePath();
  }

  // 如果没有指定输出文件名，使用默认名称
  if (outputName.isEmpty()) {
    outputName = getDefaultOutputName();
  }

  // 注意：不在这里添加.docx扩展名，让后端处理
  // 后端的DetermineOutputPath函数会自动添加.docx扩展名
  return QDir(outputDir).absoluteFilePath(outputName);
}
