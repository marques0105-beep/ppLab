#include "persistence.h"
#include <QFile>
#include <QJsonDocument>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

// Função para obter o caminho do arquivo de configuração
QString getConfigFilePath() {
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(configDir); // Garante que o diretório existe
    return configDir + "/game_data.json";
}