#include "appsettings.h"
#include <QDir>
#include <QStandardPaths>

// 静态成员初始化
AppSettings *AppSettings::s_instance = nullptr;

// 设置键名常量
const QString AppSettings::KEY_LAST_INPUT_DIR = "directories/lastInputDir";
const QString AppSettings::KEY_LAST_OUTPUT_DIR = "directories/lastOutputDir";
const QString AppSettings::KEY_LAST_TEMPLATE_DIR =
    "directories/lastTemplateDir";
const QString AppSettings::KEY_LAST_MULTI_INPUT_DIR =
    "directories/lastMultiInputDir";
const QString AppSettings::KEY_WINDOW_GEOMETRY = "window/geometry";
const QString AppSettings::KEY_WINDOW_STATE = "window/state";
const QString AppSettings::KEY_PANDOC_PATH = "pandoc/path";
const QString AppSettings::KEY_TEMPLATE_FILE = "template/file";
const QString AppSettings::KEY_USE_TEMPLATE = "template/use";
const QString AppSettings::KEY_RECENT_FILES = "files/recent";

AppSettings::AppSettings(QObject *parent) : QObject(parent) {
  // 使用应用程序名称和组织名称创建设置
  m_settings = new QSettings("Markdown转Word工具", "MD2DOCX", this);
}

AppSettings::~AppSettings() {
  if (s_instance == this) {
    s_instance = nullptr;
  }
}

AppSettings *AppSettings::instance() {
  if (!s_instance) {
    s_instance = new AppSettings();
  }
  return s_instance;
}

QString AppSettings::getLastInputDir() const {
  return m_settings
      ->value(KEY_LAST_INPUT_DIR, QStandardPaths::writableLocation(
                                      QStandardPaths::DocumentsLocation))
      .toString();
}

void AppSettings::setLastInputDir(const QString &dir) {
  if (!dir.isEmpty() && QDir(dir).exists()) {
    m_settings->setValue(KEY_LAST_INPUT_DIR, dir);
  }
}

QString AppSettings::getLastOutputDir() const {
  return m_settings
      ->value(KEY_LAST_OUTPUT_DIR, QStandardPaths::writableLocation(
                                       QStandardPaths::DocumentsLocation))
      .toString();
}

void AppSettings::setLastOutputDir(const QString &dir) {
  if (!dir.isEmpty() && QDir(dir).exists()) {
    m_settings->setValue(KEY_LAST_OUTPUT_DIR, dir);
  }
}

QString AppSettings::getLastTemplateDir() const {
  return m_settings
      ->value(KEY_LAST_TEMPLATE_DIR, QStandardPaths::writableLocation(
                                         QStandardPaths::DocumentsLocation))
      .toString();
}

void AppSettings::setLastTemplateDir(const QString &dir) {
  if (!dir.isEmpty() && QDir(dir).exists()) {
    m_settings->setValue(KEY_LAST_TEMPLATE_DIR, dir);
  }
}

QString AppSettings::getLastMultiInputDir() const {
  return m_settings
      ->value(KEY_LAST_MULTI_INPUT_DIR, QStandardPaths::writableLocation(
                                            QStandardPaths::DocumentsLocation))
      .toString();
}

void AppSettings::setLastMultiInputDir(const QString &dir) {
  if (!dir.isEmpty() && QDir(dir).exists()) {
    m_settings->setValue(KEY_LAST_MULTI_INPUT_DIR, dir);
  }
}

QByteArray AppSettings::getWindowGeometry() const {
  return m_settings->value(KEY_WINDOW_GEOMETRY).toByteArray();
}

void AppSettings::setWindowGeometry(const QByteArray &geometry) {
  m_settings->setValue(KEY_WINDOW_GEOMETRY, geometry);
}

QByteArray AppSettings::getWindowState() const {
  return m_settings->value(KEY_WINDOW_STATE).toByteArray();
}

void AppSettings::setWindowState(const QByteArray &state) {
  m_settings->setValue(KEY_WINDOW_STATE, state);
}

QString AppSettings::getPandocPath() const {
  return m_settings->value(KEY_PANDOC_PATH, "").toString();
}

void AppSettings::setPandocPath(const QString &path) {
  m_settings->setValue(KEY_PANDOC_PATH, path);
}

QString AppSettings::getTemplateFile() const {
  return m_settings->value(KEY_TEMPLATE_FILE, "").toString();
}

void AppSettings::setTemplateFile(const QString &file) {
  m_settings->setValue(KEY_TEMPLATE_FILE, file);
}

bool AppSettings::getUseTemplate() const {
  return m_settings->value(KEY_USE_TEMPLATE, false).toBool();
}

void AppSettings::setUseTemplate(bool use) {
  m_settings->setValue(KEY_USE_TEMPLATE, use);
}

QStringList AppSettings::getRecentFiles() const {
  return m_settings->value(KEY_RECENT_FILES).toStringList();
}

void AppSettings::addRecentFile(const QString &file) {
  if (file.isEmpty())
    return;

  QStringList recent = getRecentFiles();

  // 移除已存在的相同文件
  recent.removeAll(file);

  // 添加到开头
  recent.prepend(file);

  // 限制最多10个文件
  while (recent.size() > 10) {
    recent.removeLast();
  }

  m_settings->setValue(KEY_RECENT_FILES, recent);
}

void AppSettings::clearRecentFiles() { m_settings->remove(KEY_RECENT_FILES); }

void AppSettings::resetToDefaults() { m_settings->clear(); }
