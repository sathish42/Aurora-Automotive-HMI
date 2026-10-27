#pragma once

#include <QObject>
#include <QColor>

class ThemeManager final : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool dark READ dark WRITE setDark NOTIFY darkChanged)
    Q_PROPERTY(QColor background READ background NOTIFY paletteChanged)
    Q_PROPERTY(QColor surface READ surface NOTIFY paletteChanged)
    Q_PROPERTY(QColor elevated READ elevated NOTIFY paletteChanged)
    Q_PROPERTY(QColor foreground READ foreground NOTIFY paletteChanged)
    Q_PROPERTY(QColor muted READ muted NOTIFY paletteChanged)
    Q_PROPERTY(QColor accent READ accent NOTIFY paletteChanged)
    Q_PROPERTY(QColor warning READ warning CONSTANT)
    Q_PROPERTY(QColor danger READ danger CONSTANT)

public:
    explicit ThemeManager(QObject *parent = nullptr);

    bool dark() const;
    void setDark(bool dark);
    QColor background() const;
    QColor surface() const;
    QColor elevated() const;
    QColor foreground() const;
    QColor muted() const;
    QColor accent() const;
    QColor warning() const;
    QColor danger() const;

signals:
    void darkChanged();
    void paletteChanged();

private:
    bool m_dark = true;
};
