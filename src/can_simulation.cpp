#include "can_simulation.h"

#include "vehicle_data.h"

#include <QLoggingCategory>
#include <QtMath>

Q_LOGGING_CATEGORY(canSimulationLog, "auto.hmi.can-simulation")

namespace
{
constexpr quint32 telemetryFrameId = 0x180;
constexpr qsizetype telemetryPayloadSize = 12;
constexpr qint64 staleAfterMs = 1000;

void appendLittleEndian(QByteArray &payload, quint16 value)
{
    payload.append(static_cast<char>(value & 0xff));
    payload.append(static_cast<char>((value >> 8) & 0xff));
}

quint16 readLittleEndian(const QByteArray &payload, qsizetype offset)
{
    const auto low = static_cast<quint8>(payload.at(offset));
    const auto high = static_cast<quint8>(payload.at(offset + 1));
    return static_cast<quint16>(low | (high << 8));
}

quint8 encodePercent(double value)
{
    return static_cast<quint8>(qBound(0, qRound(value), 100));
}

quint8 encodePressure(double value)
{
    return static_cast<quint8>(qBound(0, qRound(value * 10.0), 50));
}

bool decodeTelemetry(const QByteArray &payload, CanTelemetry *telemetry)
{
    if (payload.size() != telemetryPayloadSize)
        return false;

    const auto speedDeciKph = readLittleEndian(payload, 0);
    const auto rpm = readLittleEndian(payload, 2);
    const auto fuel = static_cast<quint8>(payload.at(4));
    const auto battery = static_cast<quint8>(payload.at(5));
    const auto temperature = static_cast<qint8>(payload.at(6));
    const auto pressureFL = static_cast<quint8>(payload.at(7));
    const auto pressureFR = static_cast<quint8>(payload.at(8));
    const auto pressureRL = static_cast<quint8>(payload.at(9));
    const auto pressureRR = static_cast<quint8>(payload.at(10));
    const auto gear = static_cast<quint8>(payload.at(11));

    if (speedDeciKph > 2500 || rpm < 750 || rpm > 6500 || fuel > 100 || battery > 100
        || temperature < -40 || temperature > 85 || pressureFL < 5 || pressureFL > 50
        || pressureFR < 5 || pressureFR > 50 || pressureRL < 5 || pressureRL > 50
        || pressureRR < 5 || pressureRR > 50 || gear > 4) {
        return false;
    }

    telemetry->speedKph = speedDeciKph / 10.0;
    telemetry->rpm = rpm;
    telemetry->fuelPercent = fuel;
    telemetry->batterySoc = battery;
    telemetry->temperatureC = temperature;
    telemetry->tirePressureFrontLeft = pressureFL / 10.0;
    telemetry->tirePressureFrontRight = pressureFR / 10.0;
    telemetry->tirePressureRearLeft = pressureRL / 10.0;
    telemetry->tirePressureRearRight = pressureRR / 10.0;
    telemetry->gear = gear == 0 ? QStringLiteral("P") : QString::number(gear);
    return true;
}
}

CanFrameSimulator::CanFrameSimulator(QObject *parent) : QObject(parent)
{
    m_timer.setInterval(250);
    connect(&m_timer, &QTimer::timeout, this, &CanFrameSimulator::sendFrame);
}

bool CanFrameSimulator::running() const
{
    return m_timer.isActive();
}

void CanFrameSimulator::setScenario(const QString &scenario)
{
    m_scenario = scenario;
}

void CanFrameSimulator::start()
{
    if (m_timer.isActive())
        return;
    m_timer.start();
    emit runningChanged();
    sendFrame();
}

void CanFrameSimulator::stop()
{
    if (!m_timer.isActive())
        return;
    m_timer.stop();
    emit runningChanged();
}

void CanFrameSimulator::sendFrame()
{
    m_elapsedSeconds += 0.25;
    emit frameReceived(telemetryFrameId, encodeTelemetry());
}

QByteArray CanFrameSimulator::encodeTelemetry() const
{
    const double speed = qMax(0.0, 62.0 + 42.0 * qSin(m_elapsedSeconds * 0.12));
    const int rpm = qBound(750, qRound(850.0 + speed * 38.0
                                       + 300.0 * qSin(m_elapsedSeconds * 0.7)), 6500);
    const double fuel = m_scenario == QStringLiteral("Low fuel") ? 12.5 : 72.0;
    const double temperature = m_scenario == QStringLiteral("Hot cabin")
                                   ? 38.0 + 0.5 * qSin(m_elapsedSeconds * 0.04)
                                   : 22.0 + 1.5 * qSin(m_elapsedSeconds * 0.04);
    const double tirePressureFL = m_scenario == QStringLiteral("Low tire pressure")
                                      ? 1.65 + 0.01 * qSin(m_elapsedSeconds * 0.2)
                                      : 2.4 + 0.02 * qSin(m_elapsedSeconds * 0.2);
    const double tirePressureFR = 2.4 + 0.02 * qSin(m_elapsedSeconds * 0.21);
    const double tirePressureRL = 2.5 + 0.02 * qSin(m_elapsedSeconds * 0.19);
    const double tirePressureRR = 2.5 + 0.02 * qSin(m_elapsedSeconds * 0.18);
    const quint8 gear = speed < 1.0 ? 0
                        : speed < 18.0 ? 1
                        : speed < 38.0 ? 2
                        : speed < 62.0 ? 3 : 4;

    QByteArray payload;
    payload.reserve(telemetryPayloadSize);
    appendLittleEndian(payload, static_cast<quint16>(qRound(speed * 10.0)));
    appendLittleEndian(payload, static_cast<quint16>(rpm));
    payload.append(static_cast<char>(encodePercent(fuel)));
    payload.append(static_cast<char>(encodePercent(82.0 + 4.0 * qSin(m_elapsedSeconds * 0.025))));
    payload.append(static_cast<char>(static_cast<qint8>(qRound(temperature))));
    payload.append(static_cast<char>(encodePressure(tirePressureFL)));
    payload.append(static_cast<char>(encodePressure(tirePressureFR)));
    payload.append(static_cast<char>(encodePressure(tirePressureRL)));
    payload.append(static_cast<char>(encodePressure(tirePressureRR)));
    payload.append(static_cast<char>(gear));
    return payload;
}

CanTelemetryAdapter::CanTelemetryAdapter(VehicleData *vehicle, QObject *parent)
    : QObject(parent), m_vehicle(vehicle)
{
    Q_ASSERT(m_vehicle);
    m_freshnessTimer.setInterval(250);
    connect(&m_freshnessTimer, &QTimer::timeout, this,
            &CanTelemetryAdapter::diagnosticsChanged);
    m_freshnessTimer.start();
}

bool CanTelemetryAdapter::dataValid() const
{
    return m_dataValid && !stale();
}

int CanTelemetryAdapter::acceptedFrames() const
{
    return m_acceptedFrames;
}

int CanTelemetryAdapter::rejectedFrames() const
{
    return m_rejectedFrames;
}

QString CanTelemetryAdapter::status() const
{
    if (!m_hasReceivedFrame)
        return QStringLiteral("Waiting for a valid simulated CAN frame");
    if (stale())
        return QStringLiteral("Simulated CAN data is stale");
    if (!m_dataValid)
        return QStringLiteral("Last simulated CAN frame was rejected");
    if (!faults().isEmpty())
        return QStringLiteral("Valid simulated data with active demo faults");
    return QStringLiteral("Valid simulated CAN data");
}

QStringList CanTelemetryAdapter::faults() const
{
    if (!dataValid())
        return {QStringLiteral("No current valid CAN telemetry")};

    QStringList active;
    if (m_lastTelemetry.fuelPercent < 20.0)
        active.append(QStringLiteral("Low fuel"));
    if (m_lastTelemetry.temperatureC > 35.0)
        active.append(QStringLiteral("High cabin temperature"));
    if (m_lastTelemetry.tirePressureFrontLeft < 1.9)
        active.append(QStringLiteral("Low tire pressure (front left)"));
    return active;
}

qint64 CanTelemetryAdapter::dataAgeMs() const
{
    return m_hasReceivedFrame ? m_dataAgeClock.elapsed() : -1;
}

bool CanTelemetryAdapter::stale() const
{
    return m_hasReceivedFrame && dataAgeMs() > staleAfterMs;
}

void CanTelemetryAdapter::ingestFrame(quint32 identifier, const QByteArray &payload)
{
    CanTelemetry telemetry;
    if (identifier != telemetryFrameId || !decodeTelemetry(payload, &telemetry)) {
        m_dataValid = false;
        ++m_rejectedFrames;
        qCWarning(canSimulationLog) << "Rejected invalid simulated CAN frame" << Qt::hex
                                    << identifier << "payload bytes:" << payload.size();
        emit diagnosticsChanged();
        return;
    }

    m_lastTelemetry = telemetry;
    m_dataValid = true;
    m_hasReceivedFrame = true;
    m_dataAgeClock.restart();
    ++m_acceptedFrames;
    m_vehicle->applyCanTelemetry(telemetry.speedKph, telemetry.rpm, telemetry.fuelPercent,
                                 telemetry.batterySoc, telemetry.temperatureC,
                                 telemetry.tirePressureFrontLeft, telemetry.tirePressureFrontRight,
                                 telemetry.tirePressureRearLeft, telemetry.tirePressureRearRight,
                                 telemetry.gear);
    emit diagnosticsChanged();
}
