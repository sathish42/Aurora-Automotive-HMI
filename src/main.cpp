#include "app_logger.h"
#include "can_simulation.h"
#include "display_manager.h"
#include "navigation_manager.h"
#include "settings_manager.h"
#include "theme_manager.h"
#include "vehicle_data.h"

#include <QGuiApplication>
#include <QLoggingCategory>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QQuickStyle>
#include <QQmlApplicationEngine>
#include <qqml.h>

int main(int argc, char *argv[])
{
    QQuickStyle::setStyle(QStringLiteral("Basic"));
    QGuiApplication app(argc, argv);
    QGuiApplication::setApplicationName(QStringLiteral("Aurora Automotive HMI"));
    QGuiApplication::setOrganizationName(QStringLiteral("Example Automotive"));
    QGuiApplication::setApplicationVersion(QStringLiteral("1.0.0"));

    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("Aurora Automotive HMI desktop demonstration"));
    parser.addHelpOption();
    parser.addVersionOption();
    const QCommandLineOption virtualDisplaysOption(
        QStringLiteral("virtual-displays"),
        QStringLiteral("Open the secondary cluster window with simulated display targets enabled."));
    parser.addOption(virtualDisplaysOption);
    parser.process(app);

    AppLogger::initialize();

    NavigationManager navigation;
    ThemeManager theme;
    SettingsManager settings;
    VehicleData vehicle;
    CanFrameSimulator canSimulator;
    CanTelemetryAdapter canAdapter(&vehicle);
    DisplayManager displayManager;
    if (parser.isSet(virtualDisplaysOption)) {
        displayManager.setSimulatedLayoutEnabled(true);
        settings.setClusterWindowEnabled(true);
    }
    canSimulator.setScenario(vehicle.scenario());
    QObject::connect(&canSimulator, &CanFrameSimulator::frameReceived,
                     &canAdapter, &CanTelemetryAdapter::ingestFrame);
    QObject::connect(&vehicle, &VehicleData::canSimulationRequested, &canSimulator,
                     [&canSimulator](bool enabled) {
                         if (enabled)
                             canSimulator.start();
                         else
                             canSimulator.stop();
                     });
    QObject::connect(&vehicle, &VehicleData::scenarioChanged, &canSimulator,
                     [&vehicle, &canSimulator] {
                         canSimulator.setScenario(vehicle.scenario());
                     });
    theme.setDark(settings.darkTheme());

    qmlRegisterSingletonInstance("Auto.HMI", 1, 0, "Navigation", &navigation);
    qmlRegisterSingletonInstance("Auto.HMI", 1, 0, "Theme", &theme);
    qmlRegisterSingletonInstance("Auto.HMI", 1, 0, "Settings", &settings);
    qmlRegisterSingletonInstance("Auto.HMI", 1, 0, "Vehicle", &vehicle);
    qmlRegisterSingletonInstance("Auto.HMI", 1, 0, "CanDiagnostics", &canAdapter);
    qmlRegisterSingletonInstance("Auto.HMI", 1, 0, "DisplayManager", &displayManager);

    QQmlApplicationEngine engine;
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed, &app, [] {
        QCoreApplication::exit(EXIT_FAILURE);
    }, Qt::QueuedConnection);
    engine.loadFromModule("Auto.HMI", "Main");
    return app.exec();
}
