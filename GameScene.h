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

class GameScene : public QMainWindow {
    Q_OBJECT

public:
    explicit GameScene(QWidget *parent = nullptr);
    ~GameScene();

private:
    QGraphicsScene *scene;
    QGraphicsView *view;
    Mario *mario;
    QList<Obstacle *> obstacles;
    QGraphicsTextItem *scoreText;
    QGraphicsRectItem *lifeBar;

    int score;
    int lives;

    void setupScene();
    void createObstacles();
    void finishLevel();
    void gameOver();


public slots:
    void updateScore(int points);
    void updateLives();
};

#endif // GAMESCENE_H

