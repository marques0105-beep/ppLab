persistence.h

#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include <QString>
#include <QJsonObject>

// Manages a local savegame.json file in the app data directory.
// Records best score, best time, and completion status per level.
// Implementation is in persistence.cpp — header contains declarations only.
class Persistence {
public:

    // ==========================
    // HIGH SCORE
    // ==========================
    static int  getHighScore(int levelNumber);
    static void saveScore(int levelNumber, int score);

    // ==========================
    // BEST TIME
    // ==========================
    static int  getBestTime(int levelNumber);
    static void saveBestTime(int levelNumber, int seconds);

    // ==========================
    // COMPLETED LEVELS
    // ==========================
    static bool isLevelCompleted(int levelNumber);
    static void markLevelCompleted(int levelNumber);

private:
    static QString     saveFilePath();
    static QJsonObject loadSaveFile();
    static void        writeSaveFile(const QJsonObject& obj);
};

#endif // PERSISTENCE_H
