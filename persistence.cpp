#include "persistence.h"
#include <QFile>
#include <QJsonDocument>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

// ==========================
// HIGH SCORE
// ==========================

int Persistence::getHighScore(int levelNumber) {
    QJsonObject data = loadSaveFile();
    QString key = QString("highscore_level_%1").arg(levelNumber);
    return data.contains(key) ? data[key].toInt() : 0;
}

void Persistence::saveScore(int levelNumber, int score) {
    QJsonObject data = loadSaveFile();
    QString key = QString("highscore_level_%1").arg(levelNumber);
    if (score > data[key].toInt()) {
        data[key] = score;
        writeSaveFile(data);
        qDebug() << "Novo recorde no nível" << levelNumber << ":" << score;
    }
}

// ==========================
// BEST TIME
// ==========================

int Persistence::getBestTime(int levelNumber) {
    QJsonObject data = loadSaveFile();
    QString key = QString("besttime_level_%1").arg(levelNumber);
    return data.contains(key) ? data[key].toInt() : 0;
}

void Persistence::saveBestTime(int levelNumber, int seconds) {
    QJsonObject data = loadSaveFile();
    QString key = QString("besttime_level_%1").arg(levelNumber);
    int currentBest = data.contains(key) ? data[key].toInt() : 0;
    if (currentBest == 0 || seconds < currentBest) {
        data[key] = seconds;
        writeSaveFile(data);
        qDebug() << "⏱️ Novo recorde de tempo no nível" << levelNumber
                 << ":" << seconds << "s";
    }
}

// ==========================
// COMPLETED LEVELS
// ==========================

bool Persistence::isLevelCompleted(int levelNumber) {
    QJsonObject data = loadSaveFile();
    QString key = QString("completed_level_%1").arg(levelNumber);
    return data[key].toBool(false);
}

void Persistence::markLevelCompleted(int levelNumber) {
    QJsonObject data = loadSaveFile();
    QString key = QString("completed_level_%1").arg(levelNumber);
    data[key] = true;
    writeSaveFile(data);
}

// ==========================
// RESET / APAGAR PROGRESSO
// ==========================

void Persistence::clearAllProgress() {
    // Sobrescreve o ficheiro com um objecto vazio.
    // Apaga recordes, tempos e níveis concluídos de uma só vez.
    QJsonObject empty;
    writeSaveFile(empty);
    qDebug() << "🗑️ Progresso apagado: savegame.json reinicializado.";
}

// ==========================
// PRIVATE HELPERS
// ==========================

QString Persistence::saveFilePath() {
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    QDir().mkpath(dir);
    return dir + "/savegame.json";
}

QJsonObject Persistence::loadSaveFile() {
    QFile file(saveFilePath());
    if (!file.open(QIODevice::ReadOnly))
        return QJsonObject();
    QByteArray raw = file.readAll();
    file.close();
    return QJsonDocument::fromJson(raw).object();
}

void Persistence::writeSaveFile(const QJsonObject& obj) {
    QFile file(saveFilePath());
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(obj).toJson());
        file.close();
    }
}
