#include "navigation_manager.h"
#include "settings_manager.h"
#include "theme_manager.h"
#include "vehicle_data.h"
#include "can_simulation.h"
#include "display_manager.h"

#include <QDir>
#include <QGuiApplication>
#include <QImage>
#include <QQmlApplicationEngine>
#include <QQuickItem>
#include <QQuickStyle>
#include <QQuickWindow>
#include <QtTest>
#include <qqml.h>

class QmlScreenTests final : public QObject
{
    Q_OBJECT

private:
    NavigationManager m_navigation;
    ThemeManager m_theme;
    SettingsManager m_settings;
    VehicleData m_vehicle;
    CanFrameSimulator m_canSimulator;
    CanTelemetryAdapter m_canAdapter{&m_vehicle};
    DisplayManager m_displayManager;
    QQmlApplicationEngine m_engine;
    QQuickWindow *m_window = nullptr;
    QQuickItem *m_stack = nullptr;

    bool saveOptionalScreenshot(const QString &fileName) const
    {
        const QString directory = qEnvironmentVariable("AUTO_HMI_CAPTURE_DIR");
        if (directory.isEmpty())
            return true;
        if (!QDir().mkpath(directory))
            return false;
        return m_window->grabWindow().save(QDir(directory).filePath(fileName));
    }

    void verifyScreenshotBaseline(const QImage &screenshot, const QString &resourcePath)
    {
        const QImage baseline(resourcePath);
        QVERIFY2(!baseline.isNull(), qPrintable(QStringLiteral("Missing screenshot baseline: %1")
                                                    .arg(resourcePath)));
        QCOMPARE(baseline.size(), screenshot.size());
        const QImage actualReduced = screenshot.scaled(128, 80).convertToFormat(QImage::Format_RGB32);
        const QImage baselineReduced = baseline.scaled(128, 80).convertToFormat(QImage::Format_RGB32);
        int changedPixels = 0;
        for (int y = 0; y < actualReduced.height(); ++y) {
            for (int x = 0; x < actualReduced.width(); ++x) {
                const QColor actual = actualReduced.pixelColor(x, y);
                const QColor expected = baselineReduced.pixelColor(x, y);
                if (qAbs(actual.red() - expected.red()) > 24
                    || qAbs(actual.green() - expected.green()) > 24
                    || qAbs(actual.blue() - expected.blue()) > 24) {
                    ++changedPixels;
                }
            }
        }
        const double changedRatio = static_cast<double>(changedPixels)
                                    / (actualReduced.width() * actualReduced.height());
        QVERIFY2(changedRatio <= 0.08, qPrintable(
            QStringLiteral("Screenshot differs from baseline at %1% of sampled pixels")
                .arg(changedRatio * 100.0, 0, 'f', 2)));
    }

private slots:
    void initTestCase()
    {
        m_settings.setDarkTheme(true);
        m_theme.setDark(true);
        m_settings.setMetricUnits(true);
        qmlRegisterSingletonInstance("Auto.HMI", 1, 0, "Navigation", &m_navigation);
        qmlRegisterSingletonInstance("Auto.HMI", 1, 0, "Theme", &m_theme);
        qmlRegisterSingletonInstance("Auto.HMI", 1, 0, "Settings", &m_settings);
        qmlRegisterSingletonInstance("Auto.HMI", 1, 0, "Vehicle", &m_vehicle);
        qmlRegisterSingletonInstance("Auto.HMI", 1, 0, "CanDiagnostics", &m_canAdapter);
        qmlRegisterSingletonInstance("Auto.HMI", 1, 0, "DisplayManager", &m_displayManager);
        connect(&m_canSimulator, &CanFrameSimulator::frameReceived,
                &m_canAdapter, &CanTelemetryAdapter::ingestFrame);
        connect(&m_vehicle, &VehicleData::canSimulationRequested, &m_canSimulator,
                [this](bool enabled) {
                    if (enabled)
                        m_canSimulator.start();
                    else
                        m_canSimulator.stop();
                });
        connect(&m_vehicle, &VehicleData::scenarioChanged, &m_canSimulator,
                [this] { m_canSimulator.setScenario(m_vehicle.scenario()); });
        m_settings.setClusterWindowEnabled(false);
        m_displayManager.setSimulatedLayoutEnabled(false);
        m_engine.loadFromModule("Auto.HMI", "Main");
        QVERIFY2(!m_engine.rootObjects().isEmpty(), "Main.qml failed to load");
        m_window = qobject_cast<QQuickWindow *>(m_engine.rootObjects().constFirst());
        QVERIFY(m_window);
        m_stack = m_window->findChild<QQuickItem *>(QStringLiteral("screenStack"));
        QVERIFY(m_stack);
    }

    void navigationLoadsTheDiagnosticsScreen()
    {
        m_navigation.navigate(QStringLiteral("Diagnostics"));
        QTRY_VERIFY_WITH_TIMEOUT(m_stack->property("currentItem")
                                     .value<QQuickItem *>()->objectName()
                                     == QStringLiteral("DiagnosticsPage"), 1500);
    }

    void displayManagerDiscoversAvailableScreens()
    {
        QVERIFY(!m_displayManager.displayNames().isEmpty());
        QVERIFY(m_displayManager.selectedDisplayIndex() >= 0);
        QVERIFY(m_displayManager.selectedDisplayIndex() < m_displayManager.displayNames().size());
    }

    void simulatedDisplaysSelectDistinctTargetSizesOnAvailableScreen()
    {
        m_displayManager.setSimulatedLayoutEnabled(true);
        QCOMPARE(m_displayManager.displayNames().size(), 2);
        QCOMPARE(m_displayManager.displayNames().at(0),
                 QStringLiteral("Simulated instrument display (1280 x 720)"));
        QCOMPARE(m_displayManager.displayNames().at(1),
                 QStringLiteral("Simulated center display (1024 x 600)"));

        m_settings.setClusterWindowEnabled(true);
        auto *clusterWindow = m_window->findChild<QQuickWindow *>(
            QStringLiteral("secondaryClusterWindow"));
        QVERIFY(clusterWindow);
        QTRY_VERIFY(clusterWindow->isVisible());
        QTRY_COMPARE(clusterWindow->width(),
                     qMin(1280, clusterWindow->screen()->availableGeometry().width()));
        m_displayManager.setSelectedDisplayIndex(1);
        QTRY_COMPARE(clusterWindow->width(),
                     qMin(1024, clusterWindow->screen()->availableGeometry().width()));
        QTRY_COMPARE(clusterWindow->height(),
                     qMin(600, clusterWindow->screen()->availableGeometry().height()));
        QCOMPARE(clusterWindow->screen(), QGuiApplication::primaryScreen());

        m_settings.setClusterWindowEnabled(false);
        m_displayManager.setSimulatedLayoutEnabled(false);
    }

    void dashboardScreenMatchesScreenshotBaseline()
    {
        m_vehicle.setTelemetrySource(QStringLiteral("Internal simulator"));
        m_vehicle.setScenario(QStringLiteral("Normal drive"));
        m_vehicle.resetSimulation();
        for (int i = 0; i < 8; ++i)
            m_vehicle.updateSimulation();
        m_vehicle.pauseSimulation();
        m_window->resize(1024, 640);
        m_navigation.navigate(QStringLiteral("Dashboard"));
        m_window->show();
        QTRY_VERIFY(m_window->isExposed());
        QTest::qWait(250);

        const QImage screenshot = m_window->grabWindow();
        QVERIFY(!screenshot.isNull());
        QVERIFY(saveOptionalScreenshot(QStringLiteral("dashboard.png")));
        verifyScreenshotBaseline(
            screenshot, QStringLiteral(":/test-baselines/tests/baselines/dashboard.png"));
    }

    void diagnosticsScreenMatchesScreenshotBaseline()
    {
        m_vehicle.setScenario(QStringLiteral("Low fuel"));
        m_vehicle.resetSimulation();
        m_vehicle.updateSimulation();
        m_vehicle.pauseSimulation();
        m_navigation.navigate(QStringLiteral("Diagnostics"));
        m_window->show();
        QTRY_VERIFY(m_window->isExposed());
        QTest::qWait(250);

        const QImage screenshot = m_window->grabWindow();
        QVERIFY(!screenshot.isNull());
        QVERIFY(saveOptionalScreenshot(QStringLiteral("diagnostics.png")));
        verifyScreenshotBaseline(
            screenshot, QStringLiteral(":/test-baselines/tests/baselines/diagnostics.png"));
    }

    void warningBindingUpdatesTheDashboard()
    {
        m_navigation.navigate(QStringLiteral("Dashboard"));
        QTRY_VERIFY_WITH_TIMEOUT(m_stack->property("currentItem")
                                     .value<QQuickItem *>()->objectName()
                                     == QStringLiteral("DashboardPage"), 1500);
        m_window->show();
        QTRY_VERIFY(m_window->isExposed());
        QTest::qWait(250);

        m_vehicle.setScenario(QStringLiteral("Low tire pressure"));
        m_vehicle.setTelemetrySource(QStringLiteral("CAN frame simulator"));
        auto *tireCard = m_window->findChild<QQuickItem *>(QStringLiteral("tirePressureCard"));
        QVERIFY(tireCard);
        QTRY_COMPARE(tireCard->property("detail").toString(),
                     QStringLiteral("Front-left low-pressure warning"));
        m_vehicle.updateSimulation();
        QTest::qWait(250);
        QVERIFY(saveOptionalScreenshot(QStringLiteral("dashboard.png")));
    }

    void clusterScreenRendersAtCompactTargetSize()
    {
        m_navigation.navigate(QStringLiteral("Dashboard"));
        m_vehicle.updateSimulation();
        m_vehicle.pauseSimulation();
        m_window->resize(1024, 640);
        m_window->show();
        QTRY_VERIFY(m_window->isExposed());

        m_navigation.navigate(QStringLiteral("Cluster"));
        QTRY_VERIFY_WITH_TIMEOUT(m_stack->property("currentItem")
                                     .value<QQuickItem *>()->objectName()
                                     == QStringLiteral("ClusterPage"), 1500);
        QTest::qWait(250);

        const QImage screenshot = m_window->grabWindow();
        QVERIFY(!screenshot.isNull());
        QCOMPARE(screenshot.size(), QSize(1024, 640));
        QCOMPARE(screenshot.pixelColor(4, 100), m_theme.surface());
        QCOMPARE(screenshot.pixelColor(1018, 638), m_theme.background());

        verifyScreenshotBaseline(
            screenshot, QStringLiteral(":/test-baselines/tests/baselines/cluster.png"));
        QVERIFY(saveOptionalScreenshot(QStringLiteral("cluster.png")));
    }

    void secondaryClusterWindowCanBeEnabled()
    {
        m_settings.setClusterWindowEnabled(true);
        auto *clusterWindow = m_window->findChild<QQuickWindow *>(
            QStringLiteral("secondaryClusterWindow"));
        QVERIFY(clusterWindow);
        QTRY_VERIFY(clusterWindow->isVisible());
        QCOMPARE(clusterWindow->screen(),
                 QGuiApplication::screens().at(m_displayManager.selectedDisplayIndex()));
        QVERIFY(clusterWindow->screen()->availableGeometry().contains(clusterWindow->position()));
        m_settings.setClusterWindowEnabled(false);
        QTRY_VERIFY(!clusterWindow->isVisible());
    }
};

int main(int argc, char *argv[])
{
    QQuickStyle::setStyle(QStringLiteral("Basic"));
    QGuiApplication app(argc, argv);
    QCoreApplication::setApplicationName(QStringLiteral("AutoHMIQmlTests"));
    QCoreApplication::setOrganizationName(QStringLiteral("AutoHMIQmlTests"));
    QmlScreenTests test;
    return QTest::qExec(&test, argc, argv);
}

#include "tst_qml.moc"
