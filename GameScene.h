#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsTextItem>
#include <QGraphicsRectItem>
#include <QTimer>
#include <QPushButton>
#include "Mario.h"
#include "Obstacle.h"
#include "MovingObstacle.h"

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
    QGraphicsTextItem *coinsText;
    QGraphicsTextItem *abilityStatusText;
    QGraphicsTextItem *levelText;
    QGraphicsRectItem *lifeBar;
    QPushButton *storeButton; // Button to open the store
    QTimer *movementTimer;
    QTimer *scoreTimer;
    Mario *mario; // Player character
    QList<Obstacle *> obstacles;
    QList<QGraphicsPixmapItem *> backgrounds; // Holds background images

    int score;
    int coins;
    int lives;
    int currentLevel;
    const int totalLevels = 5;
    int winscore = 100;

    void createObstacles();
    void addMovingObstacle();
    void updateScore(int points);
    void updateLives();
    void centerViewOnMario();
    void finishLevel();
    void addUI();
    void updateUIPositions();

    // Abilities
    bool speedBoostActive;
    bool doubleJumpActive;
    bool invincibilityActive;

    QTimer *abilityTimer; // Timer to handle temporary abilities
    int abilityDuration; // Duration for which abilities are active

    // Ability functions
    void activateSpeedBoost();
    void activateDoubleJump();
    void activateInvincibility();
    void deactivateAbilities();

    // Store functions
    void openStore();
    void purchaseAbility(const QString &ability);
    void updateAbilityUI();    


};

#endif // GAMESCENE_H

