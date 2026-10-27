#include "navigation_manager.h"
#include "can_simulation.h"
#include "settings_manager.h"
#include "theme_manager.h"
#include "vehicle_data.h"

#include <QCoreApplication>
#include <QDir>
#include <QSettings>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QtTest>

class ServiceTests final : public QObject
{
    Q_OBJECT

private slots:
    void navigationAcceptsKnownScreens()
    {
        NavigationManager navigation;
        QSignalSpy spy(&navigation, &NavigationManager::currentScreenChanged);
        navigation.navigate(QStringLiteral("Dashboard"));
        QCOMPARE(navigation.currentScreen(), QStringLiteral("Dashboard"));
        QCOMPARE(navigation.screenIndex(), 1);
        QCOMPARE(spy.count(), 1);
    }

    void navigationRejectsUnknownScreens()
    {
        NavigationManager navigation;
        navigation.navigate(QStringLiteral("Unknown"));
        QCOMPARE(navigation.currentScreen(), QStringLiteral("Splash"));
    }

    void themeEmitsOnlyOnChange()
    {
        ThemeManager theme;
        QSignalSpy spy(&theme, &ThemeManager::paletteChanged);
        theme.setDark(false);
        theme.setDark(false);
        QCOMPARE(spy.count(), 1);
        QVERIFY(theme.background() != theme.surface());
    }

    void settingsClampAndPersistVolume()
    {
        QTemporaryDir directory;
        QVERIFY(directory.isValid());
        QCoreApplication::setOrganizationName(QStringLiteral("AutoHMITests"));
        QCoreApplication::setApplicationName(QStringLiteral("ServiceTests"));
        QSettings::setDefaultFormat(QSettings::IniFormat);
        QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, directory.path());

        {
            SettingsManager settings;
            QSignalSpy spy(&settings, &SettingsManager::volumeChanged);
            settings.setVolume(120);
            QCOMPARE(settings.volume(), 100);
            settings.setVolume(-1);
            QCOMPARE(settings.volume(), 0);
            settings.setClusterWindowEnabled(true);
            QCOMPARE(spy.count(), 2);
        }

        QSettings persisted;
        QCOMPARE(persisted.value(QStringLiteral("audio/volume")).toInt(), 0);
        QCOMPARE(persisted.value(QStringLiteral("display/clusterWindowEnabled")).toBool(), true);
    }

    void driveModeRejectsUnknownValues()
    {
        VehicleData vehicle;
        QSignalSpy spy(&vehicle, &VehicleData::driveModeChanged);
        vehicle.setDriveMode(QStringLiteral("Sport"));
        vehicle.setDriveMode(QStringLiteral("Track"));
        QCOMPARE(vehicle.driveMode(), QStringLiteral("Sport"));
        QCOMPARE(spy.count(), 1);
    }

    void simulationScenariosAreRepeatable()
    {
        VehicleData vehicle;
        const QStringList scenarios = vehicle.scenarios();
        QCOMPARE(scenarios.size(), 4);
        QCOMPARE(vehicle.scenario(), QStringLiteral("Normal drive"));
        QVERIFY(vehicle.warnings().isEmpty());

        vehicle.setScenario(QStringLiteral("Low fuel"));
        QCOMPARE(vehicle.fuelPercent(), 12.5);
        QVERIFY(vehicle.warnings().contains(QStringLiteral("Low fuel")));

        vehicle.setScenario(QStringLiteral("Low tire pressure"));
        QCOMPARE(vehicle.tirePressureFrontLeft(), 1.65);
        QVERIFY(vehicle.warnings().contains(QStringLiteral("Low tire pressure (front left)")));

        vehicle.setScenario(QStringLiteral("Hot cabin"));
        QCOMPARE(vehicle.temperatureC(), 38.0);
        QVERIFY(vehicle.warnings().contains(QStringLiteral("High cabin temperature")));

        QSignalSpy scenarioSpy(&vehicle, &VehicleData::scenarioChanged);
        vehicle.setScenario(QStringLiteral("Not a scenario"));
        QCOMPARE(vehicle.scenario(), QStringLiteral("Hot cabin"));
        QCOMPARE(scenarioSpy.count(), 0);

        vehicle.setScenario(QStringLiteral("Normal drive"));
        QVERIFY(vehicle.warnings().isEmpty());
        QCOMPARE(vehicle.fuelPercent(), 72.0);
        QCOMPARE(vehicle.tirePressureFrontLeft(), 2.4);
    }

    void simulationPauseResumeAndReset()
    {
        VehicleData vehicle;
        vehicle.pauseSimulation();
        QVERIFY(!vehicle.simulationRunning());

        vehicle.updateSimulation();
        QCOMPARE(vehicle.speedKph(), 0.0);
        QCOMPARE(vehicle.odometerKm(), 24816.4);

        vehicle.startSimulation();
        QVERIFY(vehicle.simulationRunning());
        vehicle.updateSimulation();
        QVERIFY(vehicle.speedKph() > 0.0);

        vehicle.setDriveMode(QStringLiteral("Sport"));
        vehicle.setScenario(QStringLiteral("Low fuel"));
        QCOMPARE(vehicle.driveMode(), QStringLiteral("Comfort"));
        QCOMPARE(vehicle.odometerKm(), 24816.4);
        QCOMPARE(vehicle.fuelPercent(), 12.5);

        vehicle.pauseSimulation();
        vehicle.setDriveMode(QStringLiteral("Sport"));
        vehicle.resetSimulation();
        QVERIFY(vehicle.simulationRunning());
        QCOMPARE(vehicle.speedKph(), 0.0);
        QCOMPARE(vehicle.odometerKm(), 24816.4);
        QCOMPARE(vehicle.driveMode(), QStringLiteral("Comfort"));
        QCOMPARE(vehicle.scenario(), QStringLiteral("Low fuel"));
        QVERIFY(vehicle.warnings().contains(QStringLiteral("Low fuel")));
    }

    void canFramesDecodeIntoVehicleTelemetry()
    {
        VehicleData vehicle;
        CanFrameSimulator simulator;
        CanTelemetryAdapter adapter(&vehicle);
        connect(&simulator, &CanFrameSimulator::frameReceived,
                &adapter, &CanTelemetryAdapter::ingestFrame);

        vehicle.setTelemetrySource(QStringLiteral("CAN frame simulator"));
        vehicle.setScenario(QStringLiteral("Low tire pressure"));
        simulator.setScenario(vehicle.scenario());
        simulator.start();

        QVERIFY(adapter.dataValid());
        QCOMPARE(adapter.acceptedFrames(), 1);
        QCOMPARE(adapter.rejectedFrames(), 0);
        QCOMPARE(vehicle.tirePressureFrontLeft(), 1.7);
        QVERIFY(vehicle.warnings().contains(QStringLiteral("Low tire pressure (front left)")));
        simulator.stop();
    }

    void canAdapterRejectsInvalidFramesAndRanges()
    {
        VehicleData vehicle;
        vehicle.setTelemetrySource(QStringLiteral("CAN frame simulator"));
        CanTelemetryAdapter adapter(&vehicle);

        adapter.ingestFrame(0x181, QByteArray(12, '\0'));
        adapter.ingestFrame(0x180, QByteArray(3, '\0'));
        QCOMPARE(adapter.rejectedFrames(), 2);
        QVERIFY(!adapter.dataValid());
        QVERIFY(adapter.faults().contains(QStringLiteral("No current valid CAN telemetry")));

        QByteArray invalidTelemetry(12, '\0');
        invalidTelemetry[2] = static_cast<char>(0x40);
        invalidTelemetry[3] = static_cast<char>(0x1f);
        adapter.ingestFrame(0x180, invalidTelemetry);
        QCOMPARE(adapter.rejectedFrames(), 3);
        QVERIFY(!adapter.dataValid());
    }

    void canAdapterMarksTelemetryStaleAfterFramesStop()
    {
        VehicleData vehicle;
        CanFrameSimulator simulator;
        CanTelemetryAdapter adapter(&vehicle);
        connect(&simulator, &CanFrameSimulator::frameReceived,
                &adapter, &CanTelemetryAdapter::ingestFrame);

        simulator.start();
        QVERIFY(adapter.dataValid());
        QVERIFY(!adapter.stale());
        simulator.stop();

        QTRY_VERIFY_WITH_TIMEOUT(adapter.stale(), 1800);
        QVERIFY(!adapter.dataValid());
        QVERIFY(adapter.dataAgeMs() > 1000);
        QCOMPARE(adapter.status(), QStringLiteral("Simulated CAN data is stale"));
        QVERIFY(adapter.faults().contains(QStringLiteral("No current valid CAN telemetry")));

        simulator.start();
        QVERIFY(adapter.dataValid());
        QVERIFY(!adapter.stale());
        QVERIFY(adapter.dataAgeMs() >= 0);
        QCOMPARE(adapter.status(), QStringLiteral("Valid simulated CAN data"));
        simulator.stop();
    }

    void simulationStaysWithinExpectedRanges()
    {
        VehicleData vehicle;
        for (int i = 0; i < 20; ++i)
            vehicle.updateSimulation();
        QVERIFY(vehicle.speedKph() >= 0.0);
        QVERIFY(vehicle.speedKph() <= 105.0);
        QVERIFY(vehicle.rpm() >= 750);
        QVERIFY(vehicle.rpm() <= 6500);
        QVERIFY(vehicle.fuelPercent() >= 0.0);
        QVERIFY(vehicle.batterySoc() >= 0.0);
        QVERIFY(vehicle.batterySoc() <= 100.0);
        QVERIFY(!vehicle.gear().isEmpty());
        QVERIFY(vehicle.tirePressureFrontLeft() > 0.0);
        QVERIFY(vehicle.tirePressureFrontRight() > 0.0);
        QVERIFY(vehicle.tirePressureRearLeft() > 0.0);
        QVERIFY(vehicle.tirePressureRearRight() > 0.0);
        QVERIFY(vehicle.warnings().isEmpty());
    }
};

QTEST_GUILESS_MAIN(ServiceTests)
#include "tst_services.moc"
