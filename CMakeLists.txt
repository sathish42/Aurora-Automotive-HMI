cmake_minimum_required(VERSION 3.21)

project(AutoHMI VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Gui Quick QuickControls2)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(auto-hmi
    src/main.cpp
    src/app_logger.cpp
    src/can_simulation.cpp
    src/display_manager.cpp
    src/navigation_manager.cpp
    src/theme_manager.cpp
    src/settings_manager.cpp
    src/vehicle_data.cpp
)
if(ANDROID)
    set_target_properties(auto-hmi PROPERTIES
        QT_ANDROID_PACKAGE_NAME "org.example.autohmi"
    )
endif()

set(qml_resource_files
    qml/Main.qml
    qml/components/Gauge.qml
    qml/components/MetricCard.qml
    qml/components/SectionTitle.qml
    qml/pages/SplashPage.qml
    qml/pages/DashboardPage.qml
    qml/pages/ClusterPage.qml
    qml/pages/VehicleInfoPage.qml
    qml/pages/NavigationPage.qml
    qml/pages/ClimatePage.qml
    qml/pages/MediaPage.qml
    qml/pages/SettingsPage.qml
    qml/pages/NotificationsPage.qml
    qml/pages/DiagnosticsPage.qml
    qml/pages/ADASPage.qml
    qml/pages/InstrumentClusterWindow.qml
)
foreach(qml_file IN LISTS qml_resource_files)
    get_filename_component(qml_alias "${qml_file}" NAME)
    set_source_files_properties("${qml_file}" PROPERTIES QT_RESOURCE_ALIAS "${qml_alias}")
endforeach()

qt_add_qml_module(auto-hmi
    URI Auto.HMI
    VERSION 1.0
    QML_FILES ${qml_resource_files}
)

target_include_directories(auto-hmi PRIVATE src)
target_link_libraries(auto-hmi PRIVATE Qt6::Quick Qt6::QuickControls2)
if(ANDROID)
    set_target_properties(auto-hmi PROPERTIES
        QT_ANDROID_APP_NAME "AutoHMI"
        QT_ANDROID_MIN_SDK_VERSION 28
        QT_ANDROID_TARGET_SDK_VERSION 35
        QT_ANDROID_VERSION_CODE 1
        QT_ANDROID_VERSION_NAME "${PROJECT_VERSION}"
    )
endif()

include(CTest)
if(BUILD_TESTING)
    find_package(Qt6 6.5 REQUIRED COMPONENTS Gui Test)
    qt_add_executable(auto-hmi-tests
        tests/tst_services.cpp
        src/can_simulation.cpp
        src/navigation_manager.cpp
        src/theme_manager.cpp
        src/settings_manager.cpp
        src/vehicle_data.cpp
    )
    target_include_directories(auto-hmi-tests PRIVATE src)
    target_link_libraries(auto-hmi-tests PRIVATE Qt6::Test Qt6::Gui)
    add_test(NAME auto-hmi-services COMMAND auto-hmi-tests)

    qt_add_executable(auto-hmi-qml-tests
        tests/tst_qml.cpp
        src/app_logger.cpp
        src/can_simulation.cpp
        src/display_manager.cpp
        src/navigation_manager.cpp
        src/theme_manager.cpp
        src/settings_manager.cpp
        src/vehicle_data.cpp
    )
    qt_add_qml_module(auto-hmi-qml-tests
        URI Auto.HMI
        VERSION 1.0
        OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qml-tests/Auto/HMI"
        QML_FILES ${qml_resource_files}
    )
    qt_add_resources(auto-hmi-qml-tests "qml-test-baselines"
        PREFIX "/test-baselines"
        FILES
            tests/baselines/cluster.png
            tests/baselines/dashboard.png
            tests/baselines/diagnostics.png
    )
    target_include_directories(auto-hmi-qml-tests PRIVATE src)
    target_link_libraries(auto-hmi-qml-tests PRIVATE
        Qt6::Test Qt6::Gui Qt6::Quick Qt6::QuickControls2
    )
    add_test(NAME auto-hmi-qml COMMAND auto-hmi-qml-tests)
    set_tests_properties(auto-hmi-qml PROPERTIES
        ENVIRONMENT "QT_QPA_PLATFORM=offscreen;QT_QUICK_BACKEND=software"
    )
endif()

if(ANDROID)
    install(TARGETS auto-hmi LIBRARY DESTINATION lib)
else()
    install(TARGETS auto-hmi RUNTIME DESTINATION bin)
endif()
