#ifndef LEVEL_H
#define LEVEL_H
#include <QString>
#include <QList>
#include <QPointF>
#include <QGraphicsPixmapItem>
// Structure to represent a single level struct Level {
QString backgroundPath; // Path to the background image
QList<QPointF> obstaclePositions; // List of obstacle positions for this level
int obstacleSpeed; // Speed of obstacles for this level
int levelScoreRequirement; // Score required to finish this level
// Constructor to easily create levels
Level(const QString& background, const QList<QPointF>& positions, int speed, int scoreReq)
    : backgroundPath(background), obstaclePositions(positions), obstacleSpeed(speed), levelScoreRequirement(scoreReq) {}
};
class LevelManager {
public:
    LevelManager();
    void loadLevel(int levelIndex); // Load a specific level
    void finishLevel();
    void resetGame();
    int getCurrentLevel() const;
private:
    QList<Level> levels;
    int currentLevelIndex;
    void initializeLevels();
public:
    // Signals and slots if you want to connect with game events
    void levelCompleted(); // Signal that a level has been completed
};
#endif // LEVEL_H
