#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QTimer>

class VehicleData final : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double speedKph READ speedKph NOTIFY telemetryChanged)
    Q_PROPERTY(int rpm READ rpm NOTIFY telemetryChanged)
    Q_PROPERTY(double fuelPercent READ fuelPercent NOTIFY telemetryChanged)
    Q_PROPERTY(double batterySoc READ batterySoc NOTIFY telemetryChanged)
    Q_PROPERTY(double temperatureC READ temperatureC NOTIFY telemetryChanged)
    Q_PROPERTY(double tirePressureFrontLeft READ tirePressureFrontLeft NOTIFY telemetryChanged)
    Q_PROPERTY(double tirePressureFrontRight READ tirePressureFrontRight NOTIFY telemetryChanged)
    Q_PROPERTY(double tirePressureRearLeft READ tirePressureRearLeft NOTIFY telemetryChanged)
    Q_PROPERTY(double tirePressureRearRight READ tirePressureRearRight NOTIFY telemetryChanged)
    Q_PROPERTY(QString gear READ gear NOTIFY telemetryChanged)
    Q_PROPERTY(double odometerKm READ odometerKm NOTIFY telemetryChanged)
    Q_PROPERTY(QString driveMode READ driveMode WRITE setDriveMode NOTIFY driveModeChanged)
    Q_PROPERTY(QStringList scenarios READ scenarios CONSTANT)
    Q_PROPERTY(QString scenario READ scenario NOTIFY scenarioChanged)
    Q_PROPERTY(QString scenarioDescription READ scenarioDescription NOTIFY scenarioChanged)
    Q_PROPERTY(bool simulationRunning READ simulationRunning NOTIFY simulationRunningChanged)
    Q_PROPERTY(QString telemetrySource READ telemetrySource NOTIFY telemetrySourceChanged)
    Q_PROPERTY(QStringList dataSources READ dataSources CONSTANT)
    Q_PROPERTY(QStringList warnings READ warnings NOTIFY telemetryChanged)
    Q_PROPERTY(bool leftTurnSignal READ leftTurnSignal NOTIFY telemetryChanged)
    Q_PROPERTY(bool rightTurnSignal READ rightTurnSignal NOTIFY telemetryChanged)

public:
    explicit VehicleData(QObject *parent = nullptr);

    double speedKph() const;
    int rpm() const;
    double fuelPercent() const;
    double batterySoc() const;
    double temperatureC() const;
    double tirePressureFrontLeft() const;
    double tirePressureFrontRight() const;
    double tirePressureRearLeft() const;
    double tirePressureRearRight() const;
    QString gear() const;
    double odometerKm() const;
    QString driveMode() const;
    QStringList scenarios() const;
    QString scenario() const;
    QString scenarioDescription() const;
    bool simulationRunning() const;
    QString telemetrySource() const;
    QStringList dataSources() const;
    QStringList warnings() const;
    bool leftTurnSignal() const;
    bool rightTurnSignal() const;

    Q_INVOKABLE void setDriveMode(const QString &mode);
    Q_INVOKABLE void setScenario(const QString &scenario);
    Q_INVOKABLE void setTelemetrySource(const QString &source);
    Q_INVOKABLE void startSimulation();
    Q_INVOKABLE void pauseSimulation();
    Q_INVOKABLE void resetSimulation();
    void applyCanTelemetry(double speedKph, int rpm, double fuelPercent, double batterySoc,
                           double temperatureC, double tirePressureFrontLeft,
                           double tirePressureFrontRight, double tirePressureRearLeft,
                           double tirePressureRearRight, const QString &gear);

public slots:
    void updateSimulation();

signals:
    void telemetryChanged();
    void driveModeChanged();
    void scenarioChanged();
    void simulationRunningChanged();
    void telemetrySourceChanged();
    void canSimulationRequested(bool enabled);

private:
    void applyScenarioDefaults();
    void resetTelemetry();

    QTimer m_timer;
    double m_elapsedSeconds = 0.0;
    double m_speedKph = 0.0;
    int m_rpm = 850;
    double m_fuelPercent = 72.0;
    double m_batterySoc = 84.0;
    double m_temperatureC = 21.0;
    double m_tirePressureFrontLeft = 2.4;
    double m_tirePressureFrontRight = 2.4;
    double m_tirePressureRearLeft = 2.5;
    double m_tirePressureRearRight = 2.5;
    double m_odometerKm = 24816.4;
    QString m_gear = QStringLiteral("P");
    QString m_driveMode = QStringLiteral("Comfort");
    QString m_scenario = QStringLiteral("Normal drive");
    QString m_telemetrySource = QStringLiteral("Internal simulator");
    bool m_simulationRunning = true;
};
