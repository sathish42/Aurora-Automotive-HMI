#include "navigation_manager.h"

#include <QLoggingCategory>

Q_LOGGING_CATEGORY(navigationLog, "auto.hmi.navigation")

NavigationManager::NavigationManager(QObject *parent)
    : QObject(parent),
      m_screens({QStringLiteral("Splash"), QStringLiteral("Dashboard"), QStringLiteral("Cluster"),
                 QStringLiteral("VehicleInfo"), QStringLiteral("Navigation"), QStringLiteral("Climate"),
                 QStringLiteral("Media"), QStringLiteral("Settings"), QStringLiteral("Notifications"),
                 QStringLiteral("Diagnostics"),
                 QStringLiteral("ADAS")})
{
}

QString NavigationManager::currentScreen() const
{
    return m_currentScreen;
}

int NavigationManager::screenIndex() const
{
    return m_screens.indexOf(m_currentScreen);
}

QStringList NavigationManager::screens() const
{
    return m_screens.mid(1);
}

void NavigationManager::navigate(const QString &screen)
{
    if (!m_screens.contains(screen) || screen == QStringLiteral("Splash")) {
        qCWarning(navigationLog) << "Rejected invalid navigation target:" << screen;
        return;
    }
    if (m_currentScreen == screen)
        return;
    m_currentScreen = screen;
    emit currentScreenChanged();
}
