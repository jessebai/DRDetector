#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QApplication>

#include <stdio.h>
#include <Windows.h>

#include "src/facade/DcmImageView.h"
#include "src/device/XRayDetector.h"
#include "src/data/LocalImage.h"
#include "src/data/RealtimeImage.h"

#include "src/device/XRayGenerator.h"
#include "src/device/PLCController.h"
#include "src/facade/Scheduler.h"

#include "src/device/MyCamera.h"
#include "src/device/MVSCamera.h"
#include "src/facade/MVSImageView.h"

void DRLoggerOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg){
    QByteArray localMsg = msg.toLocal8Bit();
    switch(type){
    case QtDebugMsg:
        fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtInfoMsg:
        fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtWarningMsg:
        fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    qInstallMessageHandler(DRLoggerOutput);

//    QGuiApplication app(argc, argv);
    QApplication app(argc, argv);

    qmlRegisterType<DcmImageView>("DcmImageView", 1, 0, "DcmImageView");
    qmlRegisterType<Scheduler>("Scheduler", 1, 0, "Scheduler");
    qmlRegisterType<MVSImageView>("MVSImageView", 1, 0, "MVSImageView");

    QQmlApplicationEngine engine;
    qmlRegisterSingletonType<LocalImage>("data.LocalImage", 1, 0, "LocalImage", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject*{
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine);
        LocalImage* lI = new LocalImage();
        return lI;
    });
    qmlRegisterSingletonType<RealtimeImage>("data.RealtimeImage", 1, 0, "RealtimeImage", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject*{
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine);
        RealtimeImage* lI = new RealtimeImage();
        return lI;
    });

    qmlRegisterSingletonType<XRayDetector>("device.XRayDetector", 1, 0, "XRayDetector", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject*{
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine);
        XRayDetector* lI = new XRayDetector();
        return lI;
    });

    qmlRegisterSingletonType<XRayGenerator>("device.XRayGenerator", 1, 0, "XRayGenerator", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject*{
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine);
        XRayGenerator* lI = new XRayGenerator();
        return lI;
    });

    qmlRegisterSingletonType<PLCController>("device.PLCController", 1, 0, "PLCController", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject*{
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine);
        PLCController* lI = new PLCController();
        return lI;
    });

    qmlRegisterSingletonType<MVSCamera>("device.MVSCamera", 1, 0, "MVSCamera", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject*{
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine);
        MVSCamera* lI = new MVSCamera();
        return lI;
    });

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}

