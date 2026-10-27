#pragma once

#include <QObject>
#include <QSettings>
#include <QTimer>

class SettingsManager final : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(bool metricUnits READ metricUnits WRITE setMetricUnits NOTIFY metricUnitsChanged)
    Q_PROPERTY(bool darkTheme READ darkTheme WRITE setDarkTheme NOTIFY darkThemeChanged)
    Q_PROPERTY(bool notificationsEnabled READ notificationsEnabled WRITE setNotificationsEnabled NOTIFY notificationsEnabledChanged)
    Q_PROPERTY(bool clusterWindowEnabled READ clusterWindowEnabled WRITE setClusterWindowEnabled NOTIFY clusterWindowEnabledChanged)

public:
    explicit SettingsManager(QObject *parent = nullptr);
    ~SettingsManager() override;

    int volume() const;
    void setVolume(int volume);
    bool metricUnits() const;
    void setMetricUnits(bool metricUnits);
    bool darkTheme() const;
    void setDarkTheme(bool darkTheme);
    bool notificationsEnabled() const;
    void setNotificationsEnabled(bool enabled);
    bool clusterWindowEnabled() const;
    void setClusterWindowEnabled(bool enabled);

signals:
    void volumeChanged();
    void metricUnitsChanged();
    void darkThemeChanged();
    void notificationsEnabledChanged();
    void clusterWindowEnabledChanged();

private:
    void persist(const QString &key, const QVariant &value);
    void flushSettings();
    QSettings m_settings;
    QTimer m_syncTimer;
    int m_volume = 45;
    bool m_metricUnits = true;
    bool m_darkTheme = true;
    bool m_notificationsEnabled = true;
    bool m_clusterWindowEnabled = false;
};
