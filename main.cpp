#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    // Criar a instância única do motor do jogo em C++
    GameController gameCtrl;

    // Disponibilizar o objeto C++ para dentro de qualquer ficheiro QML com o nome "gameCtrl"
    engine.rootContext()->setContextProperty("gameCtrl", &gameCtrl);

    // Carrega o ficheiro principal de interface QML
    const QUrl url(u"qrc:/qt/qml/ParkOut/Main.qml"_s)

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
        &app, []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
        
    engine.load(url);

    return app.exec();
}