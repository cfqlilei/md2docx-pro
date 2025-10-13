#include "multifileconverter.h"
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
#include <QProgressBar>
#include <QPushButton>
#include <QStandardPaths>
#include <QTextEdit>
#include <QUrl>
#include <QVBoxLayout>
#include <QWidget>

MultiFileConverter::MultiFileConverter(HttpApi *api, QWidget *parent)
    : QWidget(parent), m_inputGroup(nullptr), m_fileListText(nullptr),
      m_selectFilesButton(nullptr), m_removeSelectedButton(nullptr),
      m_clearFilesButton(nullptr), m_fileCountLabel(nullptr),
      m_outputGroup(nullptr), m_outputDirEdit(nullptr),
      m_selectOutputButton(nullptr), m_actionGroup(nullptr),
      m_convertButton(nullptr), m_resetButton(nullptr), m_statusGroup(nullptr),
      m_statusText(nullptr), m_progressBar(nullptr), m_httpApi(api),
      m_conversionInProgress(false) {
  setupUI();
  setupConnections();
  updateUI();
}

MultiFileConverter::~MultiFileConverter() {}

void MultiFileConverter::setupUI() {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);

  // 输入文件组
  m_inputGroup = new QGroupBox("输入文件", this);
  QVBoxLayout *inputLayout = new QVBoxLayout(m_inputGroup);

  // 文件列表显示
  m_fileListText = new QTextEdit(this);
  m_fileListText->setMaximumHeight(120);
  m_fileListText->setPlaceholderText(
      "选择的Markdown文件将在这里显示，每行一个文件...");
  m_fileListText->setReadOnly(true);
  inputLayout->addWidget(m_fileListText);

  // 文件操作按钮
  QHBoxLayout *fileButtonLayout = new QHBoxLayout();
  m_selectFilesButton = new QPushButton("选择文件...", this);
  m_clearFilesButton = new QPushButton("清空列表", this);
  m_fileCountLabel = new QLabel("已选择: 0 个文件", this);

  fileButtonLayout->addWidget(m_selectFilesButton);
  fileButtonLayout->addWidget(m_clearFilesButton);
  fileButtonLayout->addStretch();
  fileButtonLayout->addWidget(m_fileCountLabel);

  inputLayout->addLayout(fileButtonLayout);
  mainLayout->addWidget(m_inputGroup);

  // 输出设置组
  m_outputGroup = new QGroupBox("输出设置", this);
  QGridLayout *outputLayout = new QGridLayout(m_outputGroup);

  outputLayout->addWidget(new QLabel("保存路径:"), 0, 0);
  m_outputDirEdit = new QLineEdit(this);
  m_outputDirEdit->setPlaceholderText("留空则保存到各文件所在目录");
  outputLayout->addWidget(m_outputDirEdit, 0, 1);

  m_selectOutputButton = new QPushButton("浏览...", this);
  outputLayout->addWidget(m_selectOutputButton, 0, 2);

  mainLayout->addWidget(m_outputGroup);

  // 操作按钮组
  m_actionGroup = new QGroupBox("操作", this);
  QHBoxLayout *actionLayout = new QHBoxLayout(m_actionGroup);

  m_convertButton = new QPushButton("开始批量转换", this);
  m_convertButton->setEnabled(false);
  actionLayout->addWidget(m_convertButton);

  m_resetButton = new QPushButton("重置", this);
  actionLayout->addWidget(m_resetButton);

  actionLayout->addStretch();
  mainLayout->addWidget(m_actionGroup);

  // 状态显示组
  m_statusGroup = new QGroupBox("转换状态", this);
  QVBoxLayout *statusLayout = new QVBoxLayout(m_statusGroup);

  m_statusText = new QTextEdit(this);
  m_statusText->setMaximumHeight(120);
  m_statusText->setReadOnly(true);
  m_statusText->setPlaceholderText("批量转换状态和结果将在这里显示...");
  statusLayout->addWidget(m_statusText);

  m_progressBar = new QProgressBar(this);
  m_progressBar->setVisible(false);
  statusLayout->addWidget(m_progressBar);

  mainLayout->addWidget(m_statusGroup);

  // 添加弹性空间
  mainLayout->addStretch();
}

void MultiFileConverter::setupConnections() {
  // 按钮连接
  connect(m_selectFilesButton, &QPushButton::clicked, this,
          &MultiFileConverter::selectInputFiles);
  connect(m_clearFilesButton, &QPushButton::clicked, this,
          &MultiFileConverter::clearAllFiles);
  connect(m_selectOutputButton, &QPushButton::clicked, this,
          &MultiFileConverter::selectOutputDir);
  connect(m_convertButton, &QPushButton::clicked, this,
          &MultiFileConverter::startBatchConversion);
  connect(m_resetButton, &QPushButton::clicked, this,
          &MultiFileConverter::resetForm);

  // 输入框变化连接
  connect(m_outputDirEdit, &QLineEdit::textChanged, this,
          &MultiFileConverter::onOutputDirChanged);

  // HTTP API连接
  if (m_httpApi) {
    connect(m_httpApi, &HttpApi::batchConversionFinished, this,
            &MultiFileConverter::onBatchConversionFinished);
  }
}

void MultiFileConverter::selectInputFiles() {
  QStringList fileNames = QFileDialog::getOpenFileNames(
      this, "选择Markdown文件",
      m_lastOutputDir.isEmpty()
          ? QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
          : m_lastOutputDir,
      "Markdown文件 (*.md *.markdown *.mdown *.mkd);;所有文件 (*)");

  if (!fileNames.isEmpty()) {
    addFilesToList(fileNames);
    updateUI();
    showStatus(QString("已添加 %1 个文件").arg(fileNames.size()));
  }
}

void MultiFileConverter::clearAllFiles() {
  m_inputFiles.clear();
  m_fileListText->clear();
  updateUI();
  showStatus("已清空文件列表");
}

void MultiFileConverter::selectOutputDir() {
  QString dirName = QFileDialog::getExistingDirectory(
      this, "选择保存目录",
      m_lastOutputDir.isEmpty()
          ? QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
          : m_lastOutputDir);

  if (!dirName.isEmpty()) {
    m_outputDirEdit->setText(dirName);
    m_lastOutputDir = dirName;
    updateUI();
    showStatus(QString("已选择保存目录: %1").arg(dirName));
  }
}

void MultiFileConverter::startBatchConversion() {
  if (!validateInputs()) {
    return;
  }

  m_conversionInProgress = true;
  m_progressBar->setVisible(true);
  m_progressBar->setRange(0, m_inputFiles.size());
  m_progressBar->setValue(0);
  m_convertButton->setEnabled(false);
  m_convertButton->setText("转换中...");

  showStatus(QString("开始批量转换 %1 个文件...").arg(m_inputFiles.size()));
  emit conversionStarted();

  // 调用HTTP API进行批量转换
  if (m_httpApi) {
    BatchConversionRequest request;
    request.inputFiles = m_inputFiles;
    request.outputDir = m_outputDirEdit->text();
    request.templateFile = ""; // 暂时不使用模板
    m_httpApi->convertBatch(request);
  }
}

void MultiFileConverter::onBatchConversionFinished(
    const ConversionResponse &response) {
  m_conversionInProgress = false;
  m_progressBar->setVisible(false);
  m_convertButton->setEnabled(true);
  m_convertButton->setText("开始批量转换");

  if (response.success) {
    int successCount = 0;
    int failCount = 0;

    for (const auto &result : response.results) {
      if (result.success) {
        successCount++;
        showStatus(QString("✅ %1 -> %2")
                       .arg(QFileInfo(result.inputFile).fileName(),
                            QFileInfo(result.outputFile).fileName()));
      } else {
        failCount++;
        showStatus(
            QString("❌ %1: %2")
                .arg(QFileInfo(result.inputFile).fileName(), result.error),
            true);
      }
    }

    showStatus(QString("批量转换完成！成功: %1, 失败: %2")
                   .arg(successCount)
                   .arg(failCount));

    if (successCount > 0) {
      // 询问是否打开输出目录
      QMessageBox::StandardButton reply = QMessageBox::question(
          this, "转换完成",
          QString("批量转换完成！\n成功转换: %1 个文件\n失败: %2 "
                  "个文件\n\n是否打开输出目录？")
              .arg(successCount)
              .arg(failCount),
          QMessageBox::Yes | QMessageBox::No);

      if (reply == QMessageBox::Yes && !response.results.isEmpty()) {
        QString outputDir = m_outputDirEdit->text();
        if (outputDir.isEmpty() &&
            !response.results.first().outputFile.isEmpty()) {
          outputDir =
              QFileInfo(response.results.first().outputFile).absolutePath();
        }
        if (!outputDir.isEmpty()) {
          QDesktopServices::openUrl(QUrl::fromLocalFile(outputDir));
        }
      }
    }
  } else {
    showStatus(QString("❌ 批量转换失败！\n错误信息: %1").arg(response.message),
               true);
  }

  emit conversionFinished(response.success, response.message);
}

void MultiFileConverter::setEnabled(bool enabled) {
  QWidget::setEnabled(enabled);
  if (!enabled) {
    m_conversionInProgress = false;
    m_progressBar->setVisible(false);
    m_convertButton->setText("开始批量转换");
  }
  updateUI();
}

void MultiFileConverter::resetForm() {
  clearAllFiles();
  m_outputDirEdit->clear();
  clearStatus();
  updateUI();
  showStatus("已重置所有设置");
}

void MultiFileConverter::onFileListChanged() { updateUI(); }

void MultiFileConverter::onOutputDirChanged() { updateUI(); }

void MultiFileConverter::updateUI() {
  bool hasFiles = !m_inputFiles.isEmpty();
  bool canConvert = hasFiles && !m_conversionInProgress && isEnabled();

  m_convertButton->setEnabled(canConvert);
  m_selectFilesButton->setEnabled(!m_conversionInProgress && isEnabled());
  m_clearFilesButton->setEnabled(hasFiles && !m_conversionInProgress &&
                                 isEnabled());
  m_selectOutputButton->setEnabled(!m_conversionInProgress && isEnabled());
  m_resetButton->setEnabled(!m_conversionInProgress && isEnabled());

  // 更新文件计数标签
  m_fileCountLabel->setText(
      QString("已选择: %1 个文件").arg(m_inputFiles.size()));
}

bool MultiFileConverter::validateInputs() {
  // 基本验证在updateUI中已经完成
  return !m_inputFiles.isEmpty();
}

void MultiFileConverter::showStatus(const QString &message, bool isError) {
  QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
  QString prefix = isError ? "❌" : "ℹ️";
  m_statusText->append(QString("[%1] %2 %3").arg(timestamp, prefix, message));
}

void MultiFileConverter::clearStatus() { m_statusText->clear(); }

void MultiFileConverter::addFilesToList(const QStringList &files) {
  for (const QString &file : files) {
    if (!m_inputFiles.contains(file)) {
      m_inputFiles.append(file);
      m_fileListText->append(file);
    }
  }
}

QStringList MultiFileConverter::getInputFiles() const { return m_inputFiles; }
