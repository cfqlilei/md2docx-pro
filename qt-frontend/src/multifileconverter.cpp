#include "multifileconverter.h"
#include "appsettings.h"
#include "httpapi.h"

#include <QApplication>
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
#include <QTextCursor>
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
  m_statusText->setMaximumHeight(200); // 增大高度从120到200
  m_statusText->setReadOnly(true);
  m_statusText->setPlaceholderText("批量转换状态和结果将在这里显示...");

  // 设置更大的字体
  QFont statusFont = m_statusText->font();
  statusFont.setPointSize(statusFont.pointSize() + 2); // 增大字体2个点
  m_statusText->setFont(statusFont);

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
  AppSettings *settings = AppSettings::instance();
  QString lastDir = settings->getLastMultiInputDir();

  QStringList fileNames = QFileDialog::getOpenFileNames(
      this, "选择Markdown文件", lastDir,
      "Markdown文件 (*.md *.markdown *.mdown *.mkd);;所有文件 (*)");

  if (!fileNames.isEmpty()) {
    // 保存选择文件所在的目录到配置
    QFileInfo firstFileInfo(fileNames.first());
    settings->setLastMultiInputDir(firstFileInfo.absolutePath());

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

  // 清空之前的状态信息
  clearStatus();

  // 添加分隔线
  m_statusText->insertHtml(
      "<hr style='border: 1px solid #ccc; margin: 5px 0;'>");

  showStatus(QString("开始批量转换 %1 个文件...").arg(m_inputFiles.size()));
  showStatus(QString("输出目录: %1")
                 .arg(m_outputDirEdit->text().isEmpty()
                          ? "各文件所在目录"
                          : m_outputDirEdit->text()));
  showStatus("正在发送转换请求到后端服务...");

  // 强制刷新UI
  m_statusText->repaint();
  QApplication::processEvents();

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
        showStatus(QString("✅ 成功转换: %1 → %2")
                       .arg(QFileInfo(result.inputFile).fileName(),
                            QFileInfo(result.outputFile).fileName()));
      } else {
        failCount++;
        showStatus(
            QString("❌ 转换失败: %1 - 错误: %2")
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

  // 使用更清晰的图标和颜色
  QString icon;
  QString color;
  if (isError) {
    icon = "✗"; // 使用叉号表示错误
    color = "#d32f2f";
  } else if (message.contains("成功转换") || message.contains("完成")) {
    icon = "✓"; // 使用勾号表示成功
    color = "#388e3c";
  } else {
    icon = "•"; // 使用圆点表示一般信息
    color = "#1976d2";
  }

  // 使用div确保每条消息都换行显示，增加行间距
  QString htmlMessage =
      QString("<div style='margin: 4px 0; padding: 4px; line-height: 1.5; "
              "border-left: 3px solid %1; padding-left: 8px;'>"
              "<span style='color: #666; font-size: 11px;'>[%2]</span> "
              "<span style='color: %3; font-weight: bold; font-size: "
              "15px;'>%4</span> "
              "<span style='font-size: 13px; margin-left: 5px;'>%5</span>"
              "</div>")
          .arg(color, timestamp, color, icon, message.toHtmlEscaped());

  m_statusText->insertHtml(htmlMessage);

  // 滚动到底部
  QTextCursor cursor = m_statusText->textCursor();
  cursor.movePosition(QTextCursor::End);
  m_statusText->setTextCursor(cursor);
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
