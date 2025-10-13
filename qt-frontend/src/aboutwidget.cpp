#include "aboutwidget.h"

#include <QApplication>
#include <QClipboard>
#include <QDesktopServices>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTextEdit>
#include <QUrl>
#include <QVBoxLayout>
#include <QWidget>

AboutWidget::AboutWidget(QWidget *parent)
    : QWidget(parent), m_appInfoGroup(nullptr), m_appNameLabel(nullptr),
      m_versionLabel(nullptr), m_descriptionLabel(nullptr),
      m_authorGroup(nullptr), m_authorLabel(nullptr), m_contactLabel(nullptr),
      m_emailButton(nullptr), m_copyContactButton(nullptr),
      m_projectGroup(nullptr), m_gitHubLabel(nullptr), m_gitHubButton(nullptr),
      m_gitHubUrlText(nullptr), m_techGroup(nullptr), m_techInfoText(nullptr),
      m_licenseGroup(nullptr), m_licenseText(nullptr) {
  setupUI();
  setupConnections();
}

AboutWidget::~AboutWidget() {}

void AboutWidget::setupUI() {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);

  // 应用程序信息组
  m_appInfoGroup = new QGroupBox("应用程序信息", this);
  QVBoxLayout *appInfoLayout = new QVBoxLayout(m_appInfoGroup);

  m_appNameLabel = new QLabel("<h2>Markdown转Word工具</h2>", this);
  m_appNameLabel->setAlignment(Qt::AlignCenter);
  appInfoLayout->addWidget(m_appNameLabel);

  m_versionLabel = new QLabel("<h3>版本 v1.0.0</h3>", this);
  m_versionLabel->setAlignment(Qt::AlignCenter);
  m_versionLabel->setStyleSheet("color: #0066CC;");
  appInfoLayout->addWidget(m_versionLabel);

  m_descriptionLabel = new QLabel("一个功能强大的Markdown到Word文档转换工具\n"
                                  "支持单文件转换、批量转换、图片嵌入等功能",
                                  this);
  m_descriptionLabel->setAlignment(Qt::AlignCenter);
  m_descriptionLabel->setWordWrap(true);
  appInfoLayout->addWidget(m_descriptionLabel);

  mainLayout->addWidget(m_appInfoGroup);

  // 作者信息组
  m_authorGroup = new QGroupBox("作者信息", this);
  QVBoxLayout *authorLayout = new QVBoxLayout(m_authorGroup);

  m_authorLabel = new QLabel("<b>作者：</b>微易软件", this);
  authorLayout->addWidget(m_authorLabel);

  m_contactLabel = new QLabel("<b>联系方式：</b>cfq@wesoftcn.com", this);
  authorLayout->addWidget(m_contactLabel);

  QHBoxLayout *contactButtonLayout = new QHBoxLayout();
  m_emailButton = new QPushButton("发送邮件", this);
  m_copyContactButton = new QPushButton("复制联系方式", this);
  contactButtonLayout->addWidget(m_emailButton);
  contactButtonLayout->addWidget(m_copyContactButton);
  contactButtonLayout->addStretch();
  authorLayout->addLayout(contactButtonLayout);

  mainLayout->addWidget(m_authorGroup);

  // 项目信息组
  m_projectGroup = new QGroupBox("开源项目", this);
  QVBoxLayout *projectLayout = new QVBoxLayout(m_projectGroup);

  m_gitHubLabel = new QLabel("<b>GitHub开源地址：</b>", this);
  projectLayout->addWidget(m_gitHubLabel);

  m_gitHubUrlText = new QTextEdit(this);
  m_gitHubUrlText->setMaximumHeight(60);
  m_gitHubUrlText->setReadOnly(true);
  m_gitHubUrlText->setText("https://github.com/cfqlilei/md2docx");
  projectLayout->addWidget(m_gitHubUrlText);

  m_gitHubButton = new QPushButton("访问GitHub仓库", this);
  projectLayout->addWidget(m_gitHubButton);

  mainLayout->addWidget(m_projectGroup);

  // 技术栈信息组
  m_techGroup = new QGroupBox("技术栈", this);
  QVBoxLayout *techLayout = new QVBoxLayout(m_techGroup);

  m_techInfoText = new QTextEdit(this);
  m_techInfoText->setMaximumHeight(120);
  m_techInfoText->setReadOnly(true);
  m_techInfoText->setText("• 前端：Qt 5.15.17 + C++17\n"
                          "• 后端：Go 1.25.0 + Gin框架\n"
                          "• 转换引擎：Pandoc 3.8.2\n"
                          "• 构建工具：qmake + Go modules\n"
                          "• 支持平台：macOS, Linux, Windows");
  techLayout->addWidget(m_techInfoText);

  mainLayout->addWidget(m_techGroup);

  // 许可证信息组
  m_licenseGroup = new QGroupBox("许可证", this);
  QVBoxLayout *licenseLayout = new QVBoxLayout(m_licenseGroup);

  m_licenseText = new QTextEdit(this);
  m_licenseText->setMaximumHeight(80);
  m_licenseText->setReadOnly(true);
  m_licenseText->setText("本项目采用Apache许可证 2.0 开源\n"
                         "详细许可证信息请查看项目中的LICENSE文件");
  licenseLayout->addWidget(m_licenseText);

  mainLayout->addWidget(m_licenseGroup);

  // 添加弹性空间
  mainLayout->addStretch();
}

void AboutWidget::setupConnections() {
  connect(m_gitHubButton, &QPushButton::clicked, this,
          &AboutWidget::openGitHubRepository);
  connect(m_emailButton, &QPushButton::clicked, this,
          &AboutWidget::openEmailContact);
  connect(m_copyContactButton, &QPushButton::clicked, this,
          &AboutWidget::copyContactInfo);
}

void AboutWidget::openGitHubRepository() {
  QString url = "https://github.com/cfqlilei/md2docx";
  if (!QDesktopServices::openUrl(QUrl(url))) {
    QMessageBox::warning(
        this, "打开失败",
        QString("无法打开浏览器访问：%1\n请手动复制链接到浏览器中打开")
            .arg(url));
  }
}

void AboutWidget::openEmailContact() {
  QString emailUrl = "mailto:cfq@wesoftcn.com?subject=Markdown转Word工具反馈";
  if (!QDesktopServices::openUrl(QUrl(emailUrl))) {
    QMessageBox::information(
        this, "邮件客户端",
        "无法打开默认邮件客户端\n请手动发送邮件到：cfq@wesoftcn.com");
  }
}

void AboutWidget::copyContactInfo() {
  QString contactInfo = "微易软件 - cfq@wesoftcn.com";
  QApplication::clipboard()->setText(contactInfo);
  QMessageBox::information(
      this, "复制成功",
      QString("联系方式已复制到剪贴板：\n%1").arg(contactInfo));
}
