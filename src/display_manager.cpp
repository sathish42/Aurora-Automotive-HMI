#include "display_manager.h"

#include <QGuiApplication>
#include <QScreen>
#include <QSettings>
#include <QWindow>

DisplayManager::DisplayManager(QObject *parent) : QObject(parent)
{
    const QSettings settings;
    m_preferredScreenName = settings.value(QStringLiteral("display/clusterScreenName")).toString();
    m_simulatedLayoutEnabled = settings.value(
        QStringLiteral("display/simulatedLayoutEnabled"), false).toBool();
    connect(qGuiApp, &QGuiApplication::screenAdded, this, [this] { refreshDisplays(); });
    connect(qGuiApp, &QGuiApplication::screenRemoved, this, [this] { refreshDisplays(); });
    refreshDisplays();
}

QStringList DisplayManager::displayNames() const
{
    return m_displayNames;
}

int DisplayManager::selectedDisplayIndex() const
{
    return m_selectedDisplayIndex;
}

bool DisplayManager::simulatedLayoutEnabled() const
{
    return m_simulatedLayoutEnabled;
}

void DisplayManager::setSimulatedLayoutEnabled(bool enabled)
{
    if (m_simulatedLayoutEnabled == enabled)
        return;

    m_simulatedLayoutEnabled = enabled;
    QSettings().setValue(QStringLiteral("display/simulatedLayoutEnabled"), enabled);
    refreshDisplays();
}

void DisplayManager::setSelectedDisplayIndex(int index)
{
    if (index < 0 || index >= m_displayNames.size() || index == m_selectedDisplayIndex)
        return;

    m_selectedDisplayIndex = index;
    if (!m_simulatedLayoutEnabled) {
        m_preferredScreenName = m_screens.at(index)->name();
        QSettings().setValue(QStringLiteral("display/clusterScreenName"), m_preferredScreenName);
    } else {
        QSettings().setValue(QStringLiteral("display/simulatedLayoutIndex"), index);
    }
    emit selectedDisplayChanged();
    placeClusterWindow();
}

void DisplayManager::attachClusterWindow(QWindow *window)
{
    m_clusterWindow = window;
    connect(window, &QWindow::visibleChanged, this, [this] {
        if (m_clusterWindow && m_clusterWindow->isVisible())
            placeClusterWindow();
    });
    connect(window, &QWindow::widthChanged, this, [this] { placeClusterWindow(); });
    connect(window, &QWindow::heightChanged, this, [this] { placeClusterWindow(); });
    placeClusterWindow();
}

void DisplayManager::refreshDisplays()
{
    const QList<QScreen *> screens = QGuiApplication::screens();
    QStringList names;
    QList<QSize> targetSizes;
    QList<int> targetScreenIndexes;
    names.reserve(screens.size());
    int preferredIndex = -1;
    for (int i = 0; i < screens.size(); ++i) {
        QScreen *screen = screens.at(i);
        const QString name = screen->name().isEmpty()
                                 ? QStringLiteral("Display %1").arg(i + 1)
                                 : screen->name();
        const QSize size = screen->availableGeometry().size();
        names.append(QStringLiteral("%1 (%2 x %3)").arg(name).arg(size.width()).arg(size.height()));
        targetSizes.append(size);
        targetScreenIndexes.append(i);
        if (!m_preferredScreenName.isEmpty() && screen->name() == m_preferredScreenName)
            preferredIndex = i;
    }

    const int oldIndex = m_selectedDisplayIndex;
    m_screens = screens;
    if (m_simulatedLayoutEnabled && !screens.isEmpty()) {
        const int physicalIndex = qMax(0, screens.indexOf(QGuiApplication::primaryScreen()));
        names = {
            QStringLiteral("Simulated instrument display (1280 x 720)"),
            QStringLiteral("Simulated center display (1024 x 600)")
        };
        targetSizes = {QSize(1280, 720), QSize(1024, 600)};
        targetScreenIndexes = {physicalIndex, physicalIndex};
        m_selectedDisplayIndex = qBound(
            0, QSettings().value(QStringLiteral("display/simulatedLayoutIndex"), 0).toInt(),
            names.size() - 1);
    } else if (preferredIndex >= 0) {
        m_selectedDisplayIndex = preferredIndex;
    } else if (m_selectedDisplayIndex >= screens.size()) {
        m_selectedDisplayIndex = 0;
    }

    m_displayNames = names;
    m_targetSizes = targetSizes;
    m_targetScreenIndexes = targetScreenIndexes;
    if (m_preferredScreenName.isEmpty() && !m_screens.isEmpty()) {
        if (!m_simulatedLayoutEnabled) {
            const int primaryIndex = m_screens.indexOf(QGuiApplication::primaryScreen());
            m_selectedDisplayIndex = primaryIndex >= 0 ? primaryIndex : 0;
        }
    }

    emit displaysChanged();
    if (oldIndex != m_selectedDisplayIndex)
        emit selectedDisplayChanged();
    placeClusterWindow();
}

void DisplayManager::placeClusterWindow()
{
    if (!m_clusterWindow || m_selectedDisplayIndex < 0
        || m_selectedDisplayIndex >= m_targetScreenIndexes.size()) {
        return;
    }

    const int screenIndex = m_targetScreenIndexes.at(m_selectedDisplayIndex);
    if (screenIndex < 0 || screenIndex >= m_screens.size())
        return;
    QScreen *screen = m_screens.at(screenIndex);
    m_clusterWindow->setScreen(screen);
    const QRect available = screen->availableGeometry();
    const QSize target = m_targetSizes.at(m_selectedDisplayIndex);
    m_clusterWindow->resize(qMin(target.width(), available.width()),
                            qMin(target.height(), available.height()));
    const QPoint position(available.x() + (available.width() - m_clusterWindow->width()) / 2,
                          available.y() + (available.height() - m_clusterWindow->height()) / 2);
    m_clusterWindow->setPosition(position);
}
