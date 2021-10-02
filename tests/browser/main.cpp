#include <QDir>
#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qputenv("QML2_IMPORT_PATH", qPrintable(QDir::cleanPath(app.applicationDirPath() + "/../../qml")));

    QQmlApplicationEngine engine;
    engine.load(QUrl("qrc:main.qml"));

    return app.exec();
}
