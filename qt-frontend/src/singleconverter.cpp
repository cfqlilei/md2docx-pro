#include "singleconverter.h"
#include "httpapi.h"

#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QStandardPaths>
#include <QTextEdit>
#include <QUrl>
#include <QVBoxLayout>

SingleConverter::SingleConverter(HttpApi *httpApi, QWidget *parent)
    : QWidget(parent), m_httpApi(httpApi), m_inputGroup(nullptr),
      m_inputFileEdit(nullptr), m_selectInputButton(nullptr),
      m_outputGroup(nullptr), m_outputDirEdit(nullptr),
      m_selectOutputButton(nullptr), m_outputNameEdit(nullptr),
      m_templateFileEdit(nullptr), m_selectTemplateButton(nullptr),
      m_actionGroup(nullptr), m_convertButton(nullptr), m_clearButton(nullptr),
      m_statusGroup(nullptr), m_statusEdit(nullptr), m_progressBar(nullptr),
      m_conversionInProgress(false) {
  setupUI();
  setupConnections();
}

SingleConverter::~SingleConverter() {}

void SingleConverter::setupUI() {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);

  // 输入文件组
  QGroupBox *inputGroup = new QGroupBox("输入文件", this);
  QGridLayout *inputLayout = new QGridLayout(inputGroup);

  inputLayout->addWidget(new QLabel("Markdown文件:"), 0, 0);
  m_inputFileEdit = new QLineEdit(this);
  m_inputFileEdit->setReadOnly(true);
  m_inputFileEdit->setPlaceholderText("请选择要转换的Markdown文件");
  inputLayout->addWidget(m_inputFileEdit, 0, 1);

  QPushButton *browseInputButton = new QPushButton("浏览...", this);
  connect(browseInputButton, &QPushButton::clicked, this,
          &SingleConverter::browseInputFile);
  inputLayout->addWidget(browseInputButton, 0, 2);

  mainLayout->addWidget(inputGroup);

  // 输出设置组
  QGroupBox *outputGroup = new QGroupBox("输出设置", this);
  QGridLayout *outputLayout = new QGridLayout(outputGroup);

  outputLayout->addWidget(new QLabel("输出目录:"), 0, 0);
  m_outputDirEdit = new QLineEdit(this);
  m_outputDirEdit->setPlaceholderText("留空则使用输入文件所在目录");
  outputLayout->addWidget(m_outputDirEdit, 0, 1);

  QPushButton *browseOutputButton = new QPushButton("浏览...", this);
  connect(browseOutputButton, &QPushButton::clicked, this,
          &SingleConverter::browseOutputDir);
  outputLayout->addWidget(browseOutputButton, 0, 2);

  outputLayout->addWidget(new QLabel("输出文件名:"), 1, 0);
  m_outputNameEdit = new QLineEdit(this);
  m_outputNameEdit->setPlaceholderText("留空则使用输入文件名（不含扩展名）");
  outputLayout->addWidget(m_outputNameEdit, 1, 1, 1, 2);

  outputLayout->addWidget(new QLabel("参考模板:"), 2, 0);
  m_templateFileEdit = new QLineEdit(this);
  m_templateFileEdit->setPlaceholderText("可选：选择DOCX模板文件");
  outputLayout->addWidget(m_templateFileEdit, 2, 1);

  QPushButton *browseTemplateButton = new QPushButton("浏览...", this);
  connect(browseTemplateButton, &QPushButton::clicked, this,
          &SingleConverter::browseTemplateFile);
  outputLayout->addWidget(browseTemplateButton, 2, 2);

  mainLayout->addWidget(outputGroup);

  // 操作按钮
  QHBoxLayout *buttonLayout = new QHBoxLayout();

  m_convertButton = new QPushButton("开始转换", this);
  m_convertButton->setEnabled(false);
  connect(m_convertButton, &QPushButton::clicked, this,
          &SingleConverter::startConversion);
  buttonLayout->addWidget(m_convertButton);

  m_clearButton = new QPushButton("清空", this);
  connect(m_clearButton, &QPushButton::clicked, this,
          &SingleConverter::clearAll);
  buttonLayout->addWidget(m_clearButton);

  buttonLayout->addStretch();
  mainLayout->addLayout(buttonLayout);

  // 状态显示区域
  QGroupBox *statusGroup = new QGroupBox("状态信息", this);
  QVBoxLayout *statusLayout = new QVBoxLayout(statusGroup);

  m_statusEdit = new QTextEdit(this);
  m_statusEdit->setReadOnly(true);
  m_statusEdit->setMaximumHeight(150);
  m_statusEdit->setPlaceholderText("转换状态和结果将在这里显示...");
  statusLayout->addWidget(m_statusEdit);

  mainLayout->addWidget(statusGroup);

  // 添加弹性空间
  mainLayout->addStretch();
}

void SingleConverter::setupConnections() {
  // 连接HTTP API信号
  connect(m_httpApi, &HttpApi::singleConversionFinished, this,
          [this](const ConversionResponse &response) {
            onConversionFinished(response.success, response.outputFile,
                                 response.message);
          });

  // 监听输入文件变化
  connect(m_inputFileEdit, &QLineEdit::textChanged, this,
          &SingleConverter::updateConvertButton);
}

void SingleConverter::browseInputFile() {
  QString fileName = QFileDialog::getOpenFileName(
      this, "选择Markdown文件",
      QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
      "Markdown文件 (*.md *.markdown);;所有文件 (*)");

  if (!fileName.isEmpty()) {
    m_inputFileEdit->setText(fileName);

    // 自动设置输出文件名
    if (m_outputNameEdit->text().isEmpty()) {
      QFileInfo fileInfo(fileName);
      m_outputNameEdit->setText(fileInfo.baseName());
    }
  }
}

void SingleConverter::browseOutputDir() {
  QString dirName = QFileDialog::getExistingDirectory(
      this, "选择输出目录",
      QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));

  if (!dirName.isEmpty()) {
    m_outputDirEdit->setText(dirName);
  }
}

void SingleConverter::browseTemplateFile() {
  QString fileName = QFileDialog::getOpenFileName(
      this, "选择DOCX模板文件",
      QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
      "Word文档 (*.docx);;所有文件 (*)");

  if (!fileName.isEmpty()) {
    m_templateFileEdit->setText(fileName);
  }
}

void SingleConverter::startConversion() {
  QString inputFile = m_inputFileEdit->text().trimmed();
  if (inputFile.isEmpty()) {
    QMessageBox::warning(this, "错误", "请选择输入文件");
    return;
  }

  // 检查输入文件是否存在
  if (!QFile::exists(inputFile)) {
    QMessageBox::warning(this, "错误", "输入文件不存在");
    return;
  }

  QString outputDir = m_outputDirEdit->text().trimmed();
  QString outputName = m_outputNameEdit->text().trimmed();
  QString templateFile = m_templateFileEdit->text().trimmed();

  // 验证模板文件（如果指定了）
  if (!templateFile.isEmpty() && !QFile::exists(templateFile)) {
    QMessageBox::warning(this, "错误", "指定的模板文件不存在");
    return;
  }

  // 禁用转换按钮
  m_convertButton->setEnabled(false);
  m_convertButton->setText("转换中...");

  // 显示状态
  m_statusEdit->append(
      QString("开始转换: %1").arg(QFileInfo(inputFile).fileName()));
  m_statusEdit->append(
      QString("输出目录: %1")
          .arg(outputDir.isEmpty() ? "使用输入文件目录" : outputDir));
  m_statusEdit->append(
      QString("输出文件名: %1")
          .arg(outputName.isEmpty() ? "使用输入文件名" : outputName));
  if (!templateFile.isEmpty()) {
    m_statusEdit->append(
        QString("使用模板: %1").arg(QFileInfo(templateFile).fileName()));
  }
  m_statusEdit->append("正在转换...");

  emit conversionStarted();

  // 构建请求
  ConversionRequest request;
  request.inputFile = inputFile;
  request.outputDir = outputDir;
  request.outputName = outputName;
  request.templateFile = templateFile;

  // 调用API
  m_httpApi->convertSingle(request);
}

void SingleConverter::clearAll() {
  m_inputFileEdit->clear();
  m_outputDirEdit->clear();
  m_outputNameEdit->clear();
  m_templateFileEdit->clear();
  m_statusEdit->clear();

  updateConvertButton();
}

void SingleConverter::updateConvertButton() {
  bool hasInputFile = !m_inputFileEdit->text().trimmed().isEmpty();
  m_convertButton->setEnabled(hasInputFile && isEnabled());
}

void SingleConverter::onConversionFinished(bool success,
                                           const QString &outputFile,
                                           const QString &message) {
  // 恢复转换按钮
  m_convertButton->setEnabled(true);
  m_convertButton->setText("开始转换");

  if (success) {
    m_statusEdit->append("✅ 转换成功！");
    m_statusEdit->append(QString("输出文件: %1").arg(outputFile));

    // 询问是否打开输出目录
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "转换完成",
        QString("转换成功！\n输出文件: %1\n\n是否打开输出目录？")
            .arg(outputFile),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
      QFileInfo fileInfo(outputFile);
      QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absolutePath()));
    }
  } else {
    m_statusEdit->append("❌ 转换失败！");
    m_statusEdit->append(QString("错误信息: %1").arg(message));
  }

  emit conversionFinished(success, message);
}

void SingleConverter::setEnabled(bool enabled) {
  QWidget::setEnabled(enabled);
  updateConvertButton();
}
