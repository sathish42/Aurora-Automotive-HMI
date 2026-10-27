#pragma once

#include <QObject>
#include <QPointer>
#include <QSize>
#include <QStringList>

class QScreen;
class QWindow;

class DisplayManager final : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList displayNames READ displayNames NOTIFY displaysChanged)
    Q_PROPERTY(int selectedDisplayIndex READ selectedDisplayIndex
                   WRITE setSelectedDisplayIndex NOTIFY selectedDisplayChanged)
    Q_PROPERTY(bool simulatedLayoutEnabled READ simulatedLayoutEnabled
                   WRITE setSimulatedLayoutEnabled NOTIFY displaysChanged)

public:
    explicit DisplayManager(QObject *parent = nullptr);

    QStringList displayNames() const;
    int selectedDisplayIndex() const;
    void setSelectedDisplayIndex(int index);
    bool simulatedLayoutEnabled() const;
    void setSimulatedLayoutEnabled(bool enabled);
    Q_INVOKABLE void attachClusterWindow(QWindow *window);

signals:
    void displaysChanged();
    void selectedDisplayChanged();

private:
    void refreshDisplays();
    void placeClusterWindow();

    QStringList m_displayNames;
    QList<QScreen *> m_screens;
    QList<QSize> m_targetSizes;
    QList<int> m_targetScreenIndexes;
    QPointer<QWindow> m_clusterWindow;
    QString m_preferredScreenName;
    int m_selectedDisplayIndex = 0;
    bool m_simulatedLayoutEnabled = false;
};
