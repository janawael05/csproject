#ifndef MARIO_H
#define MARIO_H

#include <QGraphicsPixmapItem>
#include <QObject>
#include <QKeyEvent>
#include <QTimer>

class Mario : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT

public:
    explicit Mario(QGraphicsItem *parent = nullptr);
    void move();
    void stopCollisionCooldown();

signals:
    void marioHitObstacle(); // Signal emitted when Mario hits an obstacle

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    float velocityX;
    float velocityY;
    bool onGround;
    QTimer *physicsTimer; // Timer for physics updates (gravity, motion)
    void checkCollisions();
    void applyGravity();  // Handles gravity
    void updatePosition(); // Updates Mario's position based on velocity
    bool collisionCooldown; // Prevent repeated collisions
    QTimer *cooldownTimer;  // Timer for managing cooldown
    void startCollisionCooldown(); // Helper function to start cooldown

};

#endif // MARIO_H
