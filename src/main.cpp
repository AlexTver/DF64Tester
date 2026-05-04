#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

// #include <QtLogging>
#include "src/fn_log.h"
#include "Globals.h"

int main(int argc, char *argv[])
{

    // qDebug("Luna tester start");
    int LogInit = FNLog::LoadAndStartDefaultLogger("./log_param.yaml");

    if (LogInit != 0) {
        std::cerr << "Ошибка инициализации логгера" << std::endl;
        LogInit = FNLog::FastStartDefaultLogger();
    }
    LogInfo() << "Luna tester start";

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    QQmlContext *context = engine.rootContext();
    QVector <QQmlContext::PropertyPair> ctxProps;
    ctxProps.push_back(QQmlContext::PropertyPair{"qmlLayer", QVariant::fromValue(&qmlLayer)});
    context->setContextProperties(ctxProps);


    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("Df64Tester", "App");

    auto retVal = app.exec();
    LogDebug() << retVal;

    return retVal;
}
