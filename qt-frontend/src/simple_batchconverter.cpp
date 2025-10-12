#include "simple_batchconverter.h"
#include <QDateTime>
#include <QDesktopServices>
#include <QFileInfo>
#include <QLabel>
#include <QUrl>

SimpleBatchConverter::SimpleBatchConverter(HttpApi *httpApi, QWidget *parent)
    : QWidget(parent), m_httpApi(httpApi), m_inputGroup(nullptr),
      m_fileListWidget(nullptr), m_selectFilesButton(nullptr),
      m_clearFilesButton(nullptr), m_outputGroup(nullptr),
      m_outputDirEdit(nullptr), m_selectOutputButton(nullptr),
      m_actionGroup(nullptr), m_convertButton(nullptr), m_statusGroup(nullptr),
      m_statusEdit(nullptr), m_progressBar(nullptr),
      m_conversionInProgress(false) {
  setupUI();
  setupConnections();

  // 设置默认输出目录
  m_lastOutputDir =
      QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
  m_outputDirEdit->setText(m_lastOutputDir);
}

void SimpleBatchConverter::setupUI() {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);

  // 文件路径区域
  m_inputGroup = new QGroupBox("文件路径区域", this);
  QVBoxLayout *inputLayout = new QVBoxLayout(m_inputGroup);

  m_fileListWidget = new QListWidget(this);
  m_fileListWidget->setMaximumHeight(150);
  m_fileListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
  inputLayout->addWidget(m_fileListWidget);

  QHBoxLayout *inputButtonLayout = new QHBoxLayout();
  m_selectFilesButton = new QPushButton("选择文件...", this);
  m_clearFilesButton = new QPushButton("清空列表", this);
  inputButtonLayout->addWidget(m_selectFilesButton);
  inputButtonLayout->addWidget(m_clearFilesButton);
  inputButtonLayout->addStretch();
  inputLayout->addLayout(inputButtonLayout);

  mainLayout->addWidget(m_inputGroup);

  // 保存路径区域
  m_outputGroup = new QGroupBox("保存路径区域", this);
  QGridLayout *outputLayout = new QGridLayout(m_outputGroup);

  outputLayout->addWidget(new QLabel("输出目录:"), 0, 0);
  m_outputDirEdit = new QLineEdit(this);
  m_outputDirEdit->setReadOnly(true);
  m_outputDirEdit->setPlaceholderText("选择输出目录");
  outputLayout->addWidget(m_outputDirEdit, 0, 1);

  m_selectOutputButton = new QPushButton("浏览...", this);
  outputLayout->addWidget(m_selectOutputButton, 0, 2);

  mainLayout->addWidget(m_outputGroup);

  // 操作区
  m_actionGroup = new QGroupBox("操作区", this);
  QHBoxLayout *actionLayout = new QHBoxLayout(m_actionGroup);

  m_convertButton = new QPushButton("开始批量转换", this);
  m_convertButton->setEnabled(false);
  actionLayout->addWidget(m_convertButton);
  actionLayout->addStretch();

  mainLayout->addWidget(m_actionGroup);

  // 状态区域
  m_statusGroup = new QGroupBox("状态区域", this);
  QVBoxLayout *statusLayout = new QVBoxLayout(m_statusGroup);

  m_statusEdit = new QTextEdit(this);
  m_statusEdit->setReadOnly(true);
  m_statusEdit->setMaximumHeight(150);
  m_statusEdit->setPlaceholderText("批量转换状态和进度将在这里显示...");
  statusLayout->addWidget(m_statusEdit);

  m_progressBar = new QProgressBar(this);
  m_progressBar->setVisible(false);
  statusLayout->addWidget(m_progressBar);

  mainLayout->addWidget(m_statusGroup);

  // 添加弹性空间
  mainLayout->addStretch();
}

void SimpleBatchConverter::setupConnections() {
  connect(m_selectFilesButton, &QPushButton::clicked, this,
          &SimpleBatchConverter::selectInputFiles);
  connect(m_clearFilesButton, &QPushButton::clicked, this,
          &SimpleBatchConverter::clearAllFiles);
  connect(m_selectOutputButton, &QPushButton::clicked, this,
          &SimpleBatchConverter::selectOutputDir);
  connect(m_convertButton, &QPushButton::clicked, this,
          &SimpleBatchConverter::startBatchConversion);

  // 连接HTTP API信号
  connect(m_httpApi, &HttpApi::batchConversionFinished, this,
          &SimpleBatchConverter::onConversionFinished);
}

void SimpleBatchConverter::selectInputFiles() {
  QStringList files = QFileDialog::getOpenFileNames(
      this, "选择Markdown文件", m_lastOutputDir,
      "Markdown文件 (*.md *.markdown);;所有文件 (*)");

  if (!files.isEmpty()) {
    for (const QString &file : files) {
      if (!m_inputFiles.contains(file)) {
        m_inputFiles.append(file);
      }
    }
    updateFileList();
    updateStatus(QString("已添加 %1 个文件").arg(files.size()));
  }
}

void SimpleBatchConverter::clearAllFiles() {
  m_inputFiles.clear();
  updateFileList();
  updateStatus("文件列表已清空");
}

void SimpleBatchConverter::selectOutputDir() {
  QString dir =
      QFileDialog::getExistingDirectory(this, "选择输出目录", m_lastOutputDir);

  if (!dir.isEmpty()) {
    m_lastOutputDir = dir;
    m_outputDirEdit->setText(dir);
    updateStatus(QString("输出目录设置为: %1").arg(dir));
  }
}

void SimpleBatchConverter::startBatchConversion() {
  if (m_inputFiles.isEmpty()) {
    QMessageBox::warning(this, "警告", "请先选择要转换的Markdown文件");
    return;
  }

  if (m_outputDirEdit->text().isEmpty()) {
    QMessageBox::warning(this, "警告", "请选择输出目录");
    return;
  }

  if (m_conversionInProgress) {
    QMessageBox::information(this, "提示", "转换正在进行中，请稍候...");
    return;
  }

  m_conversionInProgress = true;
  m_convertButton->setEnabled(false);
  m_progressBar->setVisible(true);
  m_progressBar->setRange(0, 0); // 不确定进度

  updateStatus(QString("开始批量转换 %1 个文件...").arg(m_inputFiles.size()));
  emit conversionStarted();

  // 创建批量转换请求
  BatchConversionRequest request;
  request.inputFiles = m_inputFiles;
  request.outputDir = m_outputDirEdit->text();
  request.templateFile = ""; // 简化版本不支持模板

  m_httpApi->convertBatch(request);
}

void SimpleBatchConverter::onConversionFinished(
    const ConversionResponse &response) {
  m_conversionInProgress = false;
  m_convertButton->setEnabled(true);
  m_progressBar->setVisible(false);

  if (response.success) {
    updateStatus("✅ 批量转换完成！");

    // 显示转换结果
    if (!response.results.isEmpty()) {
      updateStatus(QString("成功转换 %1 个文件:").arg(response.results.size()));
      for (const auto &result : response.results) {
        if (result.success) {
          updateStatus(QString("  ✅ %1 -> %2")
                           .arg(result.inputFile, result.outputFile));
        } else {
          updateStatus(
              QString("  ❌ %1: %2").arg(result.inputFile, result.error));
        }
      }
    }

    emit conversionFinished(true, "批量转换完成");

    // 询问是否打开输出目录
    int ret = QMessageBox::question(this, "转换完成",
                                    "批量转换已完成！是否打开输出目录？",
                                    QMessageBox::Yes | QMessageBox::No);
    if (ret == QMessageBox::Yes) {
      QDesktopServices::openUrl(QUrl::fromLocalFile(m_outputDirEdit->text()));
    }
  } else {
    updateStatus(QString("❌ 批量转换失败: %1").arg(response.error));
    emit conversionFinished(false, response.error);

    QMessageBox::critical(this, "转换失败",
                          QString("批量转换失败:\n%1").arg(response.error));
  }
}

void SimpleBatchConverter::updateStatus(const QString &message) {
  QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
  m_statusEdit->append(QString("[%1] %2").arg(timestamp, message));

  // 自动滚动到底部
  QTextCursor cursor = m_statusEdit->textCursor();
  cursor.movePosition(QTextCursor::End);
  m_statusEdit->setTextCursor(cursor);
}

void SimpleBatchConverter::updateFileList() {
  m_fileListWidget->clear();

  for (const QString &file : m_inputFiles) {
    QFileInfo fileInfo(file);
    QString displayText = QString("%1 (%2)")
                              .arg(fileInfo.fileName())
                              .arg(fileInfo.absolutePath());
    m_fileListWidget->addItem(displayText);
  }

  // 更新转换按钮状态
  m_convertButton->setEnabled(!m_inputFiles.isEmpty() &&
                              !m_outputDirEdit->text().isEmpty());
}
