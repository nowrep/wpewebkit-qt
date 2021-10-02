#include <QDir>
#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QByteArray importPath = qgetenv("QML2_IMPORT_PATH");
    if (!importPath.isEmpty())
        importPath.append(":");
    importPath.append(QDir::cleanPath(app.applicationDirPath() + "/../../qml").toLocal8Bit());
    qputenv("QML2_IMPORT_PATH", importPath.constData());

    QQmlApplicationEngine engine;
    engine.load(QUrl("qrc:main.qml"));

    return app.exec();
}
