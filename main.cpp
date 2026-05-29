#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QIcon>
#include "TetrisController.h"

using namespace Qt::StringLiterals;

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);
    app.setApplicationName("TetrisClock");
    app.setWindowIcon(QIcon(":/TetrisClock/tetris-clock.svg"));

    TetrisController controller;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(u"controller"_s, &controller);

    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreationFailed,
        &app,    [](){ QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    // Qt 6.4+ では loadFromModule が推奨 (qrc パスの変更を回避)
    engine.loadFromModule("TetrisClock", "Main");

    return app.exec();
}
