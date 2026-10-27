#pragma once

#include <QObject>
#include <QStringList>

class NavigationManager final : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentScreen READ currentScreen NOTIFY currentScreenChanged)
    Q_PROPERTY(int screenIndex READ screenIndex NOTIFY currentScreenChanged)
    Q_PROPERTY(QStringList screens READ screens CONSTANT)

public:
    explicit NavigationManager(QObject *parent = nullptr);

    QString currentScreen() const;
    int screenIndex() const;
    QStringList screens() const;

    Q_INVOKABLE void navigate(const QString &screen);

signals:
    void currentScreenChanged();

private:
    QStringList m_screens;
    QString m_currentScreen = QStringLiteral("Splash");
};
