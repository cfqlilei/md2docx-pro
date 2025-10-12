#include "batchconverter.h"
#include "httpapi.h"

#include <QDesktopServices>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QStandardPaths>
#include <QTextEdit>
#include <QUrl>
#include <QVBoxLayout>
#include <QWidget>

BatchConverter::BatchConverter(HttpApi *httpApi, QWidget *parent)
    : QWidget(parent), m_mainSplitter(nullptr), m_inputGroup(nullptr),
      m_fileList(nullptr), m_addFilesButton(nullptr),
      m_removeFilesButton(nullptr), m_clearFilesButton(nullptr),
      m_fileCountLabel(nullptr), m_outputGroup(nullptr),
      m_outputDirEdit(nullptr), m_selectOutputButton(nullptr),
      m_actionGroup(nullptr), m_convertButton(nullptr), m_resetButton(nullptr),
      m_statusGroup(nullptr), m_statusText(nullptr), m_progressBar(nullptr),
      m_httpApi(httpApi), m_conversionInProgress(false) {
  setupUI();
  setupConnections();
}

BatchConverter::~BatchConverter() {}

void BatchConverter::setupUI() {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);

  // 输入文件组
  QGroupBox *inputGroup = new QGroupBox("输入文件", this);
  QVBoxLayout *inputLayout = new QVBoxLayout(inputGroup);

  QLabel *fileListLabel = new QLabel("Markdown文件列表:", this);
  inputLayout->addWidget(fileListLabel);

  m_fileList = new QListWidget(this);
  m_fileList->setMaximumHeight(150);
  m_fileList->setSelectionMode(QAbstractItemView::ExtendedSelection);
  inputLayout->addWidget(m_fileList);

  QHBoxLayout *fileButtonLayout = new QHBoxLayout();

  QPushButton *addFilesButton = new QPushButton("添加文件...", this);
  connect(addFilesButton, &QPushButton::clicked, this,
          &BatchConverter::selectInputFiles);
  fileButtonLayout->addWidget(addFilesButton);

  QPushButton *removeFilesButton = new QPushButton("移除选中", this);
  connect(removeFilesButton, &QPushButton::clicked, this,
          &BatchConverter::removeSelectedFiles);
  fileButtonLayout->addWidget(removeFilesButton);

  QPushButton *clearFilesButton = new QPushButton("清空列表", this);
  connect(clearFilesButton, &QPushButton::clicked, this,
          &BatchConverter::clearAllFiles);
  fileButtonLayout->addWidget(clearFilesButton);

  fileButtonLayout->addStretch();
  inputLayout->addLayout(fileButtonLayout);

  mainLayout->addWidget(inputGroup);

  // 输出设置组
  QGroupBox *outputGroup = new QGroupBox("输出设置", this);
  QGridLayout *outputLayout = new QGridLayout(outputGroup);

  outputLayout->addWidget(new QLabel("统一输出目录:"), 0, 0);
  m_outputDirEdit = new QLineEdit(this);
  m_outputDirEdit->setPlaceholderText("留空则使用各文件所在目录");
  outputLayout->addWidget(m_outputDirEdit, 0, 1);

  QPushButton *browseOutputButton = new QPushButton("浏览...", this);
  connect(browseOutputButton, &QPushButton::clicked, this,
          &BatchConverter::selectOutputDir);
  outputLayout->addWidget(browseOutputButton, 0, 2);

  mainLayout->addWidget(outputGroup);

  // 操作按钮
  QHBoxLayout *buttonLayout = new QHBoxLayout();

  m_convertButton = new QPushButton("开始批量转换", this);
  m_convertButton->setEnabled(false);
  connect(m_convertButton, &QPushButton::clicked, this,
          &BatchConverter::startBatchConversion);
  buttonLayout->addWidget(m_convertButton);

  m_resetButton = new QPushButton("重置", this);
  connect(m_resetButton, &QPushButton::clicked, this,
          &BatchConverter::resetForm);
  buttonLayout->addWidget(m_resetButton);

  buttonLayout->addStretch();
  mainLayout->addLayout(buttonLayout);

  // 状态显示区域
  m_statusGroup = new QGroupBox("转换状态", this);
  QVBoxLayout *statusLayout = new QVBoxLayout(m_statusGroup);

  m_statusText = new QTextEdit(this);
  m_statusText->setReadOnly(true);
  m_statusText->setMaximumHeight(150);
  m_statusText->setPlaceholderText("批量转换状态和结果将在这里显示...");
  statusLayout->addWidget(m_statusText);

  mainLayout->addWidget(m_statusGroup);

  // 添加弹性空间
  mainLayout->addStretch();
}

void BatchConverter::setupConnections() {
  // 连接HTTP API信号
  connect(m_httpApi, &HttpApi::batchConversionFinished, this,
          [this](const ConversionResponse &response) {
            onConversionFinished(
                response.success,
                QStringList(), // 需要从response中提取输出文件列表
                response.message);
          });

  // 监听文件列表变化
  connect(m_fileListWidget, &QListWidget::itemSelectionChanged, this,
          &BatchConverter::updateConvertButton);
}

void BatchConverter::addFiles() {
  QStringList fileNames = QFileDialog::getOpenFileNames(
      this, "选择Markdown文件",
      QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
      "Markdown文件 (*.md *.markdown);;所有文件 (*)");

  for (const QString &fileName : fileNames) {
    // 检查是否已经存在
    bool exists = false;
    for (int i = 0; i < m_fileList->count(); ++i) {
      if (m_fileList->item(i)->data(Qt::UserRole).toString() == fileName) {
        exists = true;
        break;
      }
    }

    if (!exists) {
      QListWidgetItem *item =
          new QListWidgetItem(QFileInfo(fileName).fileName());
      item->setData(Qt::UserRole, fileName);
      item->setToolTip(fileName);
      m_fileList->addItem(item);
    }
  }

  updateConvertButton();
}

void BatchConverter::removeSelectedFiles() {
  QList<QListWidgetItem *> selectedItems = m_fileList->selectedItems();
  for (QListWidgetItem *item : selectedItems) {
    delete m_fileList->takeItem(m_fileList->row(item));
  }

  updateUI();
}

void BatchConverter::clearAllFiles() {
  m_fileList->clear();
  updateUI();
}

void BatchConverter::selectOutputDir() {
  QString dirName = QFileDialog::getExistingDirectory(
      this, "选择输出目录",
      QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));

  if (!dirName.isEmpty()) {
    m_outputDirEdit->setText(dirName);
    m_lastOutputDir = dirName;
  }
}

void BatchConverter::startBatchConversion() {
  if (m_fileList->count() == 0) {
    QMessageBox::warning(this, "错误", "请添加要转换的文件");
    return;
  }

  // 收集所有文件路径
  QStringList inputFiles;
  for (int i = 0; i < m_fileList->count(); ++i) {
    QString filePath = m_fileList->item(i)->data(Qt::UserRole).toString();

    // 检查文件是否存在
    if (!QFile::exists(filePath)) {
      QMessageBox::warning(
          this, "错误",
          QString("文件不存在: %1").arg(QFileInfo(filePath).fileName()));
      return;
    }

    inputFiles.append(filePath);
  }

  QString outputDir = m_outputDirEdit->text().trimmed();
  QString templateFile = m_templateFileEdit->text().trimmed();

  // 验证模板文件（如果指定了）
  if (!templateFile.isEmpty() && !QFile::exists(templateFile)) {
    QMessageBox::warning(this, "错误", "指定的模板文件不存在");
    return;
  }

  // 禁用转换按钮
  m_convertButton->setEnabled(false);
  m_convertButton->setText("批量转换中...");

  // 显示状态
  m_statusEdit->append(
      QString("开始批量转换 %1 个文件").arg(inputFiles.count()));
  m_statusEdit->append(
      QString("输出目录: %1")
          .arg(outputDir.isEmpty() ? "使用各文件所在目录" : outputDir));
  if (!templateFile.isEmpty()) {
    m_statusEdit->append(
        QString("使用模板: %1").arg(QFileInfo(templateFile).fileName()));
  }
  m_statusEdit->append("正在转换...");

  emit conversionStarted();

  // 构建批量转换请求
  BatchConversionRequest request;
  request.inputFiles = inputFiles;
  request.outputDir = outputDir;
  request.templateFile = templateFile;

  // 调用API
  m_httpApi->convertBatch(request);
}

void BatchConverter::resetForm() {
  m_fileList->clear();
  m_outputDirEdit->clear();
  m_statusText->clear();
  m_inputFiles.clear();

  updateUI();
}

void BatchConverter::updateUI() {
  bool hasFiles = m_fileList->count() > 0;
  m_convertButton->setEnabled(hasFiles && !m_conversionInProgress);
}

void BatchConverter::onConversionFinished(bool success,
                                          const QStringList &outputFiles,
                                          const QString &message) {
  // 恢复转换按钮
  m_convertButton->setEnabled(true);
  m_convertButton->setText("开始批量转换");

  if (success) {
    m_statusEdit->append("✅ 批量转换完成！");
    m_statusEdit->append(
        QString("成功转换 %1 个文件").arg(outputFiles.count()));

    for (const QString &outputFile : outputFiles) {
      m_statusEdit->append(
          QString("  ✓ %1").arg(QFileInfo(outputFile).fileName()));
    }

    // 询问是否打开输出目录
    if (!outputFiles.isEmpty()) {
      QMessageBox::StandardButton reply = QMessageBox::question(
          this, "批量转换完成",
          QString("批量转换成功！\n共转换 %1 个文件\n\n是否打开输出目录？")
              .arg(outputFiles.count()),
          QMessageBox::Yes | QMessageBox::No);

      if (reply == QMessageBox::Yes) {
        // 打开第一个输出文件所在的目录
        QFileInfo fileInfo(outputFiles.first());
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absolutePath()));
      }
    }
  } else {
    m_statusEdit->append("❌ 批量转换失败！");
    m_statusEdit->append(QString("错误信息: %1").arg(message));
  }

  emit conversionFinished(success, message);
}

void BatchConverter::setEnabled(bool enabled) {
  QWidget::setEnabled(enabled);
  updateConvertButton();
}
