#include "theme_manager.h"

ThemeManager::ThemeManager(QObject *parent) : QObject(parent)
{
}

bool ThemeManager::dark() const { return m_dark; }

void ThemeManager::setDark(bool dark)
{
    if (m_dark == dark)
        return;
    m_dark = dark;
    emit darkChanged();
    emit paletteChanged();
}

QColor ThemeManager::background() const { return m_dark ? QColor("#08111f") : QColor("#edf2f7"); }
QColor ThemeManager::surface() const { return m_dark ? QColor("#101d2e") : QColor("#ffffff"); }
QColor ThemeManager::elevated() const { return m_dark ? QColor("#17263a") : QColor("#f5f8fc"); }
QColor ThemeManager::foreground() const { return m_dark ? QColor("#f3f7fc") : QColor("#152238"); }
QColor ThemeManager::muted() const { return m_dark ? QColor("#91a3ba") : QColor("#607086"); }
QColor ThemeManager::accent() const { return QColor("#42d6c5"); }
QColor ThemeManager::warning() const { return QColor("#ffbf4a"); }
QColor ThemeManager::danger() const { return QColor("#ff5f68"); }
