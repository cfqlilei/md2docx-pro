#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QObject>
#include <QSettings>
#include <QString>

/**
 * 应用设置管理类
 * 负责保存和恢复应用的各种设置，如最后打开的目录等
 */
class AppSettings : public QObject {
  Q_OBJECT

public:
  explicit AppSettings(QObject *parent = nullptr);
  ~AppSettings();

  // 目录设置
  QString getLastInputDir() const;
  void setLastInputDir(const QString &dir);

  QString getLastOutputDir() const;
  void setLastOutputDir(const QString &dir);

  QString getLastTemplateDir() const;
  void setLastTemplateDir(const QString &dir);

  QString getLastMultiInputDir() const;
  void setLastMultiInputDir(const QString &dir);

  // 窗口设置
  QByteArray getWindowGeometry() const;
  void setWindowGeometry(const QByteArray &geometry);

  QByteArray getWindowState() const;
  void setWindowState(const QByteArray &state);

  // 应用设置
  QString getPandocPath() const;
  void setPandocPath(const QString &path);

  QString getTemplateFile() const;
  void setTemplateFile(const QString &file);

  bool getUseTemplate() const;
  void setUseTemplate(bool use);

  // 最近使用的文件
  QStringList getRecentFiles() const;
  void addRecentFile(const QString &file);
  void clearRecentFiles();

  // 重置所有设置
  void resetToDefaults();

  // 获取单例实例
  static AppSettings *instance();

private:
  QSettings *m_settings;
  static AppSettings *s_instance;

  // 设置键名常量
  static const QString KEY_LAST_INPUT_DIR;
  static const QString KEY_LAST_OUTPUT_DIR;
  static const QString KEY_LAST_TEMPLATE_DIR;
  static const QString KEY_LAST_MULTI_INPUT_DIR; // 多文件转换最后使用的输入目录
  static const QString KEY_WINDOW_GEOMETRY;
  static const QString KEY_WINDOW_STATE;
  static const QString KEY_PANDOC_PATH;
  static const QString KEY_TEMPLATE_FILE;
  static const QString KEY_USE_TEMPLATE;
  static const QString KEY_RECENT_FILES;
};

#endif // APPSETTINGS_H
