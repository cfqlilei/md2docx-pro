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

BatchConverter::BatchConverter(HttpApi *httpApi, QWidget *parent)
    : QWidget(parent), m_httpApi(httpApi), m_fileListWidget(nullptr),
      m_outputDirEdit(nullptr), m_templateFileEdit(nullptr),
      m_statusEdit(nullptr), m_convertButton(nullptr), m_clearButton(nullptr) {
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

  m_fileListWidget = new QListWidget(this);
  m_fileListWidget->setMaximumHeight(150);
  m_fileListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
  inputLayout->addWidget(m_fileListWidget);

  QHBoxLayout *fileButtonLayout = new QHBoxLayout();

  QPushButton *addFilesButton = new QPushButton("添加文件...", this);
  connect(addFilesButton, &QPushButton::clicked, this,
          &BatchConverter::addFiles);
  fileButtonLayout->addWidget(addFilesButton);

  QPushButton *removeFilesButton = new QPushButton("移除选中", this);
  connect(removeFilesButton, &QPushButton::clicked, this,
          &BatchConverter::removeSelectedFiles);
  fileButtonLayout->addWidget(removeFilesButton);

  QPushButton *clearFilesButton = new QPushButton("清空列表", this);
  connect(clearFilesButton, &QPushButton::clicked, this,
          &BatchConverter::clearFileList);
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
          &BatchConverter::browseOutputDir);
  outputLayout->addWidget(browseOutputButton, 0, 2);

  outputLayout->addWidget(new QLabel("参考模板:"), 1, 0);
  m_templateFileEdit = new QLineEdit(this);
  m_templateFileEdit->setPlaceholderText("可选：选择DOCX模板文件");
  outputLayout->addWidget(m_templateFileEdit, 1, 1);

  QPushButton *browseTemplateButton = new QPushButton("浏览...", this);
  connect(browseTemplateButton, &QPushButton::clicked, this,
          &BatchConverter::browseTemplateFile);
  outputLayout->addWidget(browseTemplateButton, 1, 2);

  mainLayout->addWidget(outputGroup);

  // 操作按钮
  QHBoxLayout *buttonLayout = new QHBoxLayout();

  m_convertButton = new QPushButton("开始批量转换", this);
  m_convertButton->setEnabled(false);
  connect(m_convertButton, &QPushButton::clicked, this,
          &BatchConverter::startConversion);
  buttonLayout->addWidget(m_convertButton);

  m_clearButton = new QPushButton("清空所有", this);
  connect(m_clearButton, &QPushButton::clicked, this,
          &BatchConverter::clearAll);
  buttonLayout->addWidget(m_clearButton);

  buttonLayout->addStretch();
  mainLayout->addLayout(buttonLayout);

  // 状态显示区域
  QGroupBox *statusGroup = new QGroupBox("转换状态", this);
  QVBoxLayout *statusLayout = new QVBoxLayout(statusGroup);

  m_statusEdit = new QTextEdit(this);
  m_statusEdit->setReadOnly(true);
  m_statusEdit->setMaximumHeight(150);
  m_statusEdit->setPlaceholderText("批量转换状态和结果将在这里显示...");
  statusLayout->addWidget(m_statusEdit);

  mainLayout->addWidget(statusGroup);

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
    for (int i = 0; i < m_fileListWidget->count(); ++i) {
      if (m_fileListWidget->item(i)->data(Qt::UserRole).toString() ==
          fileName) {
        exists = true;
        break;
      }
    }

    if (!exists) {
      QListWidgetItem *item =
          new QListWidgetItem(QFileInfo(fileName).fileName());
      item->setData(Qt::UserRole, fileName);
      item->setToolTip(fileName);
      m_fileListWidget->addItem(item);
    }
  }

  updateConvertButton();
}

void BatchConverter::removeSelectedFiles() {
  QList<QListWidgetItem *> selectedItems = m_fileListWidget->selectedItems();
  for (QListWidgetItem *item : selectedItems) {
    delete m_fileListWidget->takeItem(m_fileListWidget->row(item));
  }

  updateConvertButton();
}

void BatchConverter::clearFileList() {
  m_fileListWidget->clear();
  updateConvertButton();
}

void BatchConverter::browseOutputDir() {
  QString dirName = QFileDialog::getExistingDirectory(
      this, "选择输出目录",
      QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));

  if (!dirName.isEmpty()) {
    m_outputDirEdit->setText(dirName);
  }
}

void BatchConverter::browseTemplateFile() {
  QString fileName = QFileDialog::getOpenFileName(
      this, "选择DOCX模板文件",
      QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
      "Word文档 (*.docx);;所有文件 (*)");

  if (!fileName.isEmpty()) {
    m_templateFileEdit->setText(fileName);
  }
}

void BatchConverter::startConversion() {
  if (m_fileListWidget->count() == 0) {
    QMessageBox::warning(this, "错误", "请添加要转换的文件");
    return;
  }

  // 收集所有文件路径
  QStringList inputFiles;
  for (int i = 0; i < m_fileListWidget->count(); ++i) {
    QString filePath = m_fileListWidget->item(i)->data(Qt::UserRole).toString();

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

void BatchConverter::clearAll() {
  m_fileListWidget->clear();
  m_outputDirEdit->clear();
  m_templateFileEdit->clear();
  m_statusEdit->clear();

  updateConvertButton();
}

void BatchConverter::updateConvertButton() {
  bool hasFiles = m_fileListWidget->count() > 0;
  m_convertButton->setEnabled(hasFiles && isEnabled());
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
