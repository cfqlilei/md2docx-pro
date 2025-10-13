#ifndef ABOUTWIDGET_H
#define ABOUTWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QHBoxLayout;
class QLabel;
class QPushButton;
class QTextEdit;
class QGroupBox;
QT_END_NAMESPACE

/**
 * @brief 关于页面组件
 *
 * 功能：
 * - 显示应用程序信息
 * - 显示版本信息
 * - 显示作者和联系方式
 * - 显示开源地址
 * - 显示技术栈信息
 */
class AboutWidget : public QWidget {
  Q_OBJECT

public:
  explicit AboutWidget(QWidget *parent = nullptr);
  ~AboutWidget();

private slots:
  void openGitHubRepository();
  void openEmailContact();
  void copyContactInfo();

private:
  void setupUI();
  void setupConnections();

  // UI组件
  QGroupBox *m_appInfoGroup;
  QLabel *m_appNameLabel;
  QLabel *m_versionLabel;
  QLabel *m_descriptionLabel;

  QGroupBox *m_authorGroup;
  QLabel *m_authorLabel;
  QLabel *m_contactLabel;
  QPushButton *m_emailButton;
  QPushButton *m_copyContactButton;

  QGroupBox *m_projectGroup;
  QLabel *m_gitHubLabel;
  QPushButton *m_gitHubButton;
  QTextEdit *m_gitHubUrlText;

  QGroupBox *m_techGroup;
  QTextEdit *m_techInfoText;

  QGroupBox *m_licenseGroup;
  QTextEdit *m_licenseText;
};

#endif // ABOUTWIDGET_H
