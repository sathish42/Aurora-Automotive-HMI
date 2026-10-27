#include "settings_manager.h"

#include <QLoggingCategory>
#include <QSettings>

Q_LOGGING_CATEGORY(settingsLog, "auto.hmi.settings")

SettingsManager::SettingsManager(QObject *parent) : QObject(parent)
{
    m_volume = qBound(0, m_settings.value(QStringLiteral("audio/volume"), m_volume).toInt(), 100);
    m_metricUnits = m_settings.value(QStringLiteral("vehicle/metricUnits"), m_metricUnits).toBool();
    m_darkTheme = m_settings.value(QStringLiteral("display/darkTheme"), m_darkTheme).toBool();
    m_notificationsEnabled = m_settings.value(QStringLiteral("system/notificationsEnabled"), m_notificationsEnabled).toBool();
    m_clusterWindowEnabled = m_settings.value(QStringLiteral("display/clusterWindowEnabled"), m_clusterWindowEnabled).toBool();
    m_syncTimer.setSingleShot(true);
    m_syncTimer.setInterval(300);
    connect(&m_syncTimer, &QTimer::timeout, this, &SettingsManager::flushSettings);
}

SettingsManager::~SettingsManager()
{
    m_syncTimer.stop();
    flushSettings();
}

int SettingsManager::volume() const { return m_volume; }
bool SettingsManager::metricUnits() const { return m_metricUnits; }
bool SettingsManager::darkTheme() const { return m_darkTheme; }
bool SettingsManager::notificationsEnabled() const { return m_notificationsEnabled; }
bool SettingsManager::clusterWindowEnabled() const { return m_clusterWindowEnabled; }

void SettingsManager::setVolume(int volume)
{
    const int bounded = qBound(0, volume, 100);
    if (m_volume == bounded)
        return;
    m_volume = bounded;
    persist(QStringLiteral("audio/volume"), m_volume);
    emit volumeChanged();
}

void SettingsManager::setMetricUnits(bool metricUnits)
{
    if (m_metricUnits == metricUnits)
        return;
    m_metricUnits = metricUnits;
    persist(QStringLiteral("vehicle/metricUnits"), m_metricUnits);
    emit metricUnitsChanged();
}

void SettingsManager::setDarkTheme(bool darkTheme)
{
    if (m_darkTheme == darkTheme)
        return;
    m_darkTheme = darkTheme;
    persist(QStringLiteral("display/darkTheme"), m_darkTheme);
    emit darkThemeChanged();
}

void SettingsManager::setNotificationsEnabled(bool enabled)
{
    if (m_notificationsEnabled == enabled)
        return;
    m_notificationsEnabled = enabled;
    persist(QStringLiteral("system/notificationsEnabled"), m_notificationsEnabled);
    emit notificationsEnabledChanged();
}

void SettingsManager::setClusterWindowEnabled(bool enabled)
{
    if (m_clusterWindowEnabled == enabled)
        return;
    m_clusterWindowEnabled = enabled;
    persist(QStringLiteral("display/clusterWindowEnabled"), m_clusterWindowEnabled);
    emit clusterWindowEnabledChanged();
}

void SettingsManager::persist(const QString &key, const QVariant &value)
{
    m_settings.setValue(key, value);
    m_syncTimer.start();
}

void SettingsManager::flushSettings()
{
    m_settings.sync();
    if (m_settings.status() != QSettings::NoError)
        qCWarning(settingsLog) << "Failed to synchronize application settings";
}
