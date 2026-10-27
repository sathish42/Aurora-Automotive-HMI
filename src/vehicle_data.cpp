#include "vehicle_data.h"

#include <QLoggingCategory>
#include <QtMath>

Q_LOGGING_CATEGORY(vehicleLog, "auto.hmi.vehicle")

VehicleData::VehicleData(QObject *parent) : QObject(parent)
{
    m_timer.setInterval(250);
    connect(&m_timer, &QTimer::timeout, this, &VehicleData::updateSimulation);
    applyScenarioDefaults();
    m_timer.start();
}

double VehicleData::speedKph() const { return m_speedKph; }
int VehicleData::rpm() const { return m_rpm; }
double VehicleData::fuelPercent() const { return m_fuelPercent; }
double VehicleData::batterySoc() const { return m_batterySoc; }
double VehicleData::temperatureC() const { return m_temperatureC; }
double VehicleData::tirePressureFrontLeft() const { return m_tirePressureFrontLeft; }
double VehicleData::tirePressureFrontRight() const { return m_tirePressureFrontRight; }
double VehicleData::tirePressureRearLeft() const { return m_tirePressureRearLeft; }
double VehicleData::tirePressureRearRight() const { return m_tirePressureRearRight; }
QString VehicleData::gear() const { return m_gear; }
double VehicleData::odometerKm() const { return m_odometerKm; }
QString VehicleData::driveMode() const { return m_driveMode; }
QStringList VehicleData::scenarios() const
{
    return {QStringLiteral("Normal drive"), QStringLiteral("Low fuel"),
            QStringLiteral("Low tire pressure"), QStringLiteral("Hot cabin")};
}

QString VehicleData::scenario() const { return m_scenario; }

QString VehicleData::scenarioDescription() const
{
    if (m_scenario == QStringLiteral("Low fuel"))
        return QStringLiteral("Fuel warning active — use this to demo the alert path.");
    if (m_scenario == QStringLiteral("Low tire pressure"))
        return QStringLiteral("Front-left tire warning active — simulated sensor value.");
    if (m_scenario == QStringLiteral("Hot cabin"))
        return QStringLiteral("Cabin temperature warning active — simulated sensor value.");
    return QStringLiteral("Nominal telemetry — no simulated vehicle warnings.");
}

bool VehicleData::simulationRunning() const { return m_simulationRunning; }
QString VehicleData::telemetrySource() const { return m_telemetrySource; }
QStringList VehicleData::dataSources() const
{
    return {QStringLiteral("Internal simulator"), QStringLiteral("CAN frame simulator")};
}
bool VehicleData::leftTurnSignal() const { return qSin(m_elapsedSeconds * 3.141592653589793) > 0.0; }
bool VehicleData::rightTurnSignal() const { return qSin(m_elapsedSeconds * 3.141592653589793) < 0.0; }

QStringList VehicleData::warnings() const
{
    QStringList active;
    if (m_fuelPercent < 20.0)
        active.append(QStringLiteral("Low fuel"));
    if (m_temperatureC > 35.0)
        active.append(QStringLiteral("High cabin temperature"));
    if (m_tirePressureFrontLeft < 1.9)
        active.append(QStringLiteral("Low tire pressure (front left)"));
    return active;
}

void VehicleData::setDriveMode(const QString &mode)
{
    static const QStringList modes{QStringLiteral("Eco"), QStringLiteral("Comfort"), QStringLiteral("Sport")};
    if (!modes.contains(mode)) {
        qCWarning(vehicleLog) << "Rejected invalid drive mode:" << mode;
        return;
    }
    if (m_driveMode == mode)
        return;
    m_driveMode = mode;
    emit driveModeChanged();
}

void VehicleData::setScenario(const QString &scenario)
{
    if (!scenarios().contains(scenario)) {
        qCWarning(vehicleLog) << "Rejected invalid simulation scenario:" << scenario;
        return;
    }
    if (m_scenario == scenario)
        return;

    m_scenario = scenario;
    resetTelemetry();
    applyScenarioDefaults();
    emit scenarioChanged();
    emit telemetryChanged();
    emit canSimulationRequested(m_telemetrySource == QStringLiteral("CAN frame simulator")
                                && m_simulationRunning);
}

void VehicleData::setTelemetrySource(const QString &source)
{
    if (!dataSources().contains(source) || source == m_telemetrySource)
        return;

    const bool wasCanSimulation = m_telemetrySource == QStringLiteral("CAN frame simulator");
    m_telemetrySource = source;
    if (wasCanSimulation)
        emit canSimulationRequested(false);
    if (m_simulationRunning) {
        if (source == QStringLiteral("CAN frame simulator"))
            m_timer.stop();
        else
            m_timer.start();
    }
    emit telemetrySourceChanged();
    emit telemetryChanged();
    emit canSimulationRequested(source == QStringLiteral("CAN frame simulator")
                                && m_simulationRunning);
}

void VehicleData::startSimulation()
{
    if (m_simulationRunning)
        return;
    m_simulationRunning = true;
    if (m_telemetrySource == QStringLiteral("CAN frame simulator"))
        emit canSimulationRequested(true);
    else
        m_timer.start();
    emit simulationRunningChanged();
}

void VehicleData::pauseSimulation()
{
    if (!m_simulationRunning)
        return;
    m_simulationRunning = false;
    m_timer.stop();
    emit canSimulationRequested(false);
    emit simulationRunningChanged();
}

void VehicleData::resetSimulation()
{
    resetTelemetry();
    applyScenarioDefaults();
    const bool wasRunning = m_simulationRunning;
    m_simulationRunning = true;
    if (m_telemetrySource == QStringLiteral("CAN frame simulator"))
        emit canSimulationRequested(true);
    else
        m_timer.start();
    if (!wasRunning)
        emit simulationRunningChanged();
    emit telemetryChanged();
}

void VehicleData::applyCanTelemetry(double speedKph, int rpm, double fuelPercent,
                                    double batterySoc, double temperatureC,
                                    double tirePressureFrontLeft, double tirePressureFrontRight,
                                    double tirePressureRearLeft, double tirePressureRearRight,
                                    const QString &gear)
{
    if (!m_simulationRunning || m_telemetrySource != QStringLiteral("CAN frame simulator"))
        return;

    m_speedKph = speedKph;
    m_rpm = rpm;
    m_fuelPercent = fuelPercent;
    m_batterySoc = batterySoc;
    m_temperatureC = temperatureC;
    m_tirePressureFrontLeft = tirePressureFrontLeft;
    m_tirePressureFrontRight = tirePressureFrontRight;
    m_tirePressureRearLeft = tirePressureRearLeft;
    m_tirePressureRearRight = tirePressureRearRight;
    m_gear = gear;
    m_odometerKm += m_speedKph * 0.25 / 3600.0;
    m_elapsedSeconds += 0.25;
    emit telemetryChanged();
}

void VehicleData::applyScenarioDefaults()
{
    m_fuelPercent = m_scenario == QStringLiteral("Low fuel") ? 12.5 : 72.0;
    m_temperatureC = m_scenario == QStringLiteral("Hot cabin") ? 38.0 : 21.0;
    m_tirePressureFrontLeft = m_scenario == QStringLiteral("Low tire pressure") ? 1.65 : 2.4;
    m_tirePressureFrontRight = 2.4;
    m_tirePressureRearLeft = 2.5;
    m_tirePressureRearRight = 2.5;
}

void VehicleData::resetTelemetry()
{
    m_elapsedSeconds = 0.0;
    m_speedKph = 0.0;
    m_rpm = 850;
    m_gear = QStringLiteral("P");
    m_batterySoc = 84.0;
    m_odometerKm = 24816.4;
    if (m_driveMode != QStringLiteral("Comfort")) {
        m_driveMode = QStringLiteral("Comfort");
        emit driveModeChanged();
    }
}

void VehicleData::updateSimulation()
{
    if (!m_simulationRunning)
        return;

    constexpr double intervalSeconds = 0.25;
    m_elapsedSeconds += intervalSeconds;
    m_speedKph = 62.0 + 42.0 * qSin(m_elapsedSeconds * 0.12);
    if (m_speedKph < 0.0)
        m_speedKph = 0.0;
    const double rpmMultiplier = m_driveMode == QStringLiteral("Sport") ? 48.0
                                 : m_driveMode == QStringLiteral("Eco") ? 30.0 : 38.0;
    m_rpm = qBound(750, qRound(850.0 + m_speedKph * rpmMultiplier + 300.0 * qSin(m_elapsedSeconds * 0.7)), 6500);
    m_gear = m_speedKph < 1.0 ? QStringLiteral("P")
            : m_speedKph < 18.0 ? QStringLiteral("1")
            : m_speedKph < 38.0 ? QStringLiteral("2")
            : m_speedKph < 62.0 ? QStringLiteral("3")
                                : QStringLiteral("4");
    m_fuelPercent = qMax(0.0, m_fuelPercent - 0.00015);
    m_batterySoc = 82.0 + 4.0 * qSin(m_elapsedSeconds * 0.025);
    if (m_scenario == QStringLiteral("Hot cabin"))
        m_temperatureC = 38.0 + 0.5 * qSin(m_elapsedSeconds * 0.04);
    else
        m_temperatureC = 22.0 + 1.5 * qSin(m_elapsedSeconds * 0.04);

    if (m_scenario == QStringLiteral("Low tire pressure"))
        m_tirePressureFrontLeft = 1.65 + 0.01 * qSin(m_elapsedSeconds * 0.2);
    else
        m_tirePressureFrontLeft = 2.4 + 0.02 * qSin(m_elapsedSeconds * 0.2);
    m_tirePressureFrontRight = 2.4 + 0.02 * qSin(m_elapsedSeconds * 0.21);
    m_tirePressureRearLeft = 2.5 + 0.02 * qSin(m_elapsedSeconds * 0.19);
    m_tirePressureRearRight = 2.5 + 0.02 * qSin(m_elapsedSeconds * 0.18);
    m_odometerKm += m_speedKph * intervalSeconds / 3600.0;
    emit telemetryChanged();
}
