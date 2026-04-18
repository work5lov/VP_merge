#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "appcontroller.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    qmlRegisterType<AppController>("com.vpmerge", 1, 0, "AppController");
    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    engine.load(url);

    if (engine.rootObjects().isEmpty())
        return -1;
    return app.exec();
}
