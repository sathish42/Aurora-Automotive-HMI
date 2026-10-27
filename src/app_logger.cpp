#include "app_logger.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QLoggingCategory>
#include <QMutex>
#include <QMutexLocker>
#include <QStandardPaths>
#include <QTextStream>
#include <cstdio>

Q_LOGGING_CATEGORY(appLog, "auto.hmi")

namespace
{
QFile logFile;
QMutex logMutex;

void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &message)
{
    const auto timestamp = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
    const auto category = context.category ? QString::fromUtf8(context.category) : QStringLiteral("default");
    const auto line = QStringLiteral("%1 [%2] [%3] %4")
                          .arg(timestamp, QString::number(static_cast<int>(type)), category, message);

    QMutexLocker lock(&logMutex);
    if (logFile.isOpen()) {
        QTextStream stream(&logFile);
        stream << line << Qt::endl;
        if (type == QtCriticalMsg || type == QtFatalMsg)
            logFile.flush();
    }
    fprintf(stderr, "%s\n", qPrintable(line));
}
}

void AppLogger::initialize()
{
    const auto directory = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    if (!QDir().mkpath(directory)) {
        fprintf(stderr, "Unable to create application log directory: %s\n", qPrintable(directory));
    } else {
        logFile.setFileName(directory + QStringLiteral("/auto-hmi.log"));
        if (!logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
            fprintf(stderr, "Unable to open application log: %s\n", qPrintable(logFile.errorString()));
    }
    qInstallMessageHandler(messageHandler);
    qCInfo(appLog) << "Logging initialized";
}
