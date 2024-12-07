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

    // New ability-related functions
    void setSpeed(float multiplier); // Set Mario's speed
    void setCanDoubleJump(bool enabled); // Enable/disable double jump
    void setInvincible(bool enabled); // Enable/disable invincibility
    //void setSpeedBoostActive(bool active);

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

    // Ability-related variables
    bool canDoubleJump; // Determines if Mario can double-jump
    bool invincible;    // Determines if Mario is invincible
    float speedMultiplier; // Adjusts Mario's movement speed
    bool hasDoubleJumped; // Tracks if Mario has performed a double jump

};

#endif // MARIO_H
