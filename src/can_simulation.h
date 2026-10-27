#pragma once

#include <QObject>
#include <QByteArray>
#include <QElapsedTimer>
#include <QPointer>
#include <QTimer>
#include <QStringList>

class VehicleData;

struct CanTelemetry
{
    double speedKph = 0.0;
    int rpm = 0;
    double fuelPercent = 0.0;
    double batterySoc = 0.0;
    double temperatureC = 0.0;
    double tirePressureFrontLeft = 0.0;
    double tirePressureFrontRight = 0.0;
    double tirePressureRearLeft = 0.0;
    double tirePressureRearRight = 0.0;
    QString gear;
};

class CanFrameSimulator final : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool running READ running NOTIFY runningChanged)

public:
    explicit CanFrameSimulator(QObject *parent = nullptr);

    bool running() const;
    void setScenario(const QString &scenario);
    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();

signals:
    void frameReceived(quint32 identifier, const QByteArray &payload);
    void runningChanged();

private slots:
    void sendFrame();

private:
    QByteArray encodeTelemetry() const;

    QTimer m_timer;
    double m_elapsedSeconds = 0.0;
    QString m_scenario = QStringLiteral("Normal drive");
};

class CanTelemetryAdapter final : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool dataValid READ dataValid NOTIFY diagnosticsChanged)
    Q_PROPERTY(int acceptedFrames READ acceptedFrames NOTIFY diagnosticsChanged)
    Q_PROPERTY(int rejectedFrames READ rejectedFrames NOTIFY diagnosticsChanged)
    Q_PROPERTY(QString status READ status NOTIFY diagnosticsChanged)
    Q_PROPERTY(QStringList faults READ faults NOTIFY diagnosticsChanged)
    Q_PROPERTY(qint64 dataAgeMs READ dataAgeMs NOTIFY diagnosticsChanged)
    Q_PROPERTY(bool stale READ stale NOTIFY diagnosticsChanged)

public:
    explicit CanTelemetryAdapter(VehicleData *vehicle, QObject *parent = nullptr);

    bool dataValid() const;
    int acceptedFrames() const;
    int rejectedFrames() const;
    QString status() const;
    QStringList faults() const;
    qint64 dataAgeMs() const;
    bool stale() const;

public slots:
    void ingestFrame(quint32 identifier, const QByteArray &payload);

signals:
    void diagnosticsChanged();

private:
    VehicleData *m_vehicle;
    bool m_dataValid = false;
    int m_acceptedFrames = 0;
    int m_rejectedFrames = 0;
    CanTelemetry m_lastTelemetry;
    QElapsedTimer m_dataAgeClock;
    QTimer m_freshnessTimer;
    bool m_hasReceivedFrame = false;
};
