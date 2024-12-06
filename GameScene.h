#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsTextItem>
#include <QGraphicsRectItem>
#include <QTimer>
#include "Mario.h"
#include "Obstacle.h"

const int winscore = 100;

class GameScene : public QMainWindow
{
    Q_OBJECT

public:
    explicit GameScene(QWidget *parent = nullptr);
    ~GameScene();
    int getLives() const {
        return lives;
    }

private:
    QGraphicsScene *scene;
    QGraphicsView *view;
    QGraphicsTextItem *scoreText;
    QGraphicsRectItem *lifeBar;
    QTimer *movementTimer;
    QTimer *scoreTimer;
    Mario *mario; // Player character
    QList<Obstacle *> obstacles;
    QList<QGraphicsPixmapItem *> backgrounds; // Holds background images

    int score;
    int lives;
    void createObstacles();
    void updateScore(int points);
    void updateLives();
    void centerViewOnMario();
    void finishLevel();
    void addUI();
    void updateUIPositions();

};

#endif // GAMESCENE_H

