#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "gamecontroller.h"
#include "gametests.h"

// Adiciona isto para permitir o uso do novo sufixo moderno do Qt 6 (_s)
using namespace Qt::StringLiterals;

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    //Crie o controlador antes de carregar o QML para que a propriedade de contexto
    // nunca seja nula
    GameController gameCtrl;
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("gameCtrl", &gameCtrl);

    // Executar testes unitários
    GameTests::runAllTests();

    // Carrega a UI por último
    const QUrl url(u"qrc:/qt/qml/ParkOut/Main.qml"_s);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
                     &app, []() { QCoreApplication::exit(-1); },
                     Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}