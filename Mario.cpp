#include "Mario.h"
#include "Obstacle.h"
#include "GameScene.h"
#include "MovingObstacle.h"
#include <QList>
#include <QGraphicsItem>
#include "QtGui/qevent.h"
#include <QBrush>
#include <QGraphicsScene>
#include <QKeyEvent>

Mario::Mario(QGraphicsItem *parent)
    // : QGraphicsPixmapItem(parent), velocityX(0), velocityY(0), onGround(true)
    : QGraphicsPixmapItem(parent), velocityX(0), velocityY(0), onGround(true),
    canDoubleJump(false), invincible(false), speedMultiplier(1.0f), hasDoubleJumped(false)
{
    setPixmap(QPixmap("/Users/janawael/supermario/supermario.png").scaled(100, 100));
    // Enable focus so Mario can receive key events
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFocus();

    // Initialize the physics timer
    physicsTimer = new QTimer(this);
    connect(physicsTimer, &QTimer::timeout, this, &Mario::updatePosition);
    physicsTimer->start(20); // Update every 20ms

    cooldownTimer = new QTimer(this);
    cooldownTimer->setSingleShot(true); // Only trigger once per cooldown period
    connect(cooldownTimer, &QTimer::timeout, [this]() { collisionCooldown = false; });
}

void Mario::keyPressEvent(QKeyEvent *event) {
    qDebug() << "Key Pressed:" << event->key();
    switch (event->key()) {
    case Qt::Key_Left:
        velocityX = -5 * speedMultiplier;
        break;
    case Qt::Key_Right:
        velocityX = 5 * speedMultiplier;
        break;
    case Qt::Key_Space:
        if (onGround && !canDoubleJump) {
            velocityY = -15; // Jump upwards
            onGround = false;
            hasDoubleJumped = false; // Reset double jump
        } else if (onGround && canDoubleJump && !hasDoubleJumped) {
            velocityY = -30; // Perform double jump
            onGround = false;
            hasDoubleJumped = true;
        }
        break;
    default:
        break;
    }
    qDebug() << "Updated velocityX:" << velocityX << " after Key Press";

}


void Mario::keyReleaseEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Left || event->key() == Qt::Key_Right) {
        if (speedMultiplier == 1.0) {
            velocityX = 0;
        }
    }
    qDebug() << "Key Released:" << event->key() << " velocityX set to:" << velocityX;
}


void Mario::move() {
    qDebug() << "Before Moving Mario. Position:" << pos()
    << " velocityX:" << velocityX
    << " velocityY:" << velocityY
    << " onGround:" << onGround;

    setPos(x() + velocityX, y()); // Move Mario based on velocity
    if (!onGround) {
        velocityY += 1; // Simulate gravity
        setPos(x(), y() + velocityY);
        if (y() >= 500) { // Example ground level
            setPos(x(), 500);
            velocityY = 0;
            onGround = true;
        }
    }
    qDebug() << "After Moving Mario. New Position:" << pos();
}
void Mario::applyGravity() {
    if (!onGround) {
        velocityY += 1; // Gravity effect (increase downward velocity)
    }
}

void Mario::updatePosition() {
    // Apply horizontal movement
    setPos(x() + velocityX, y());

    // Apply gravity if Mario is not on the ground
    applyGravity();

    // Apply vertical movement
    setPos(x(), y() + velocityY);

    // Apply a slight horizontal speed increase while jumping
    if (!onGround) {
        velocityX = 5; // Mario moves forward while jumping
    }

    // Check for collisions
    QList<QGraphicsItem *> collidingItems = scene()->collidingItems(this);
    for (QGraphicsItem *item : collidingItems) {
        // Check for static obstacle collision
        Obstacle *obstacle = dynamic_cast<Obstacle *>(item);
        if (obstacle) {
            QRectF marioBounds = boundingRect().translated(pos());
            QRectF obstacleBounds = obstacle->boundingRect().translated(obstacle->pos());

            if (marioBounds.bottom() <= obstacleBounds.top()) {
                // Mario lands on top of the obstacle
                velocityY = 0; // Stop vertical motion
                setPos(x(), obstacleBounds.top() - marioBounds.height());
                onGround = true;
            } else if (!invincible && !collisionCooldown) {
                handleCollision(); // Handle collision with static obstacle
                return; // Prevent multiple collision handling
            }
        }

        // Check for moving obstacle collision
        MovingObstacle *movingObstacle = dynamic_cast<MovingObstacle *>(item);
        if (movingObstacle) {
            QRectF marioBounds = boundingRect().translated(pos());
            QRectF obstacleBounds = movingObstacle->boundingRect().translated(movingObstacle->pos());

            qDebug() << "Checking collision with moving obstacle. Mario bounds:" << marioBounds
                     << "Obstacle bounds:" << obstacleBounds;

            if (marioBounds.intersects(obstacleBounds) && !invincible && !collisionCooldown) {
                qDebug() << "Collision with moving obstacle detected.";
                handleCollision();
                return;
            }
        }
    }

    // Check if Mario has landed on the ground
    if (y() >= 400) { // Ground level
        setPos(x(), 400);
        velocityY = 0;
        onGround = true;
        velocityX = 0;
    }
}

// Handle collision logic for any obstacle
void Mario::handleCollision() {
    emit marioHitObstacle(); // Signal collision

    // Query GameScene for remaining lives
    GameScene *gameScene = dynamic_cast<GameScene *>(scene()->views().first()->parentWidget());
    if (gameScene) {
        qDebug() << "Mario collided. Lives left:" << gameScene->getLives();
        if (gameScene->getLives() > 0) {
            // Start collision cooldown only if Mario has lives left
            startCollisionCooldown();
        } else {
            // If game over, stop the cooldown timer to avoid further updates
            stopCollisionCooldown();
            qDebug() << "Game Over triggered from Mario.";
        }
    }
}

// Helper function to start cooldown
void Mario::startCollisionCooldown() {
    collisionCooldown = true;
    cooldownTimer->start(2000); // Set cooldown period (2 seconds)
}

// Helper function to stop cooldown
void Mario::stopCollisionCooldown() {
    collisionCooldown = false;
    if (cooldownTimer->isActive()) {
        cooldownTimer->stop(); // Stop the cooldown timer immediately
        qDebug() << "Collision cooldown stopped.";
    }
}
// Ability-related methods
void Mario::setSpeed(float multiplier) {
    speedMultiplier = multiplier; // Adjust Mario's speed
}

void Mario::setCanDoubleJump(bool enabled) {
    canDoubleJump = enabled; // Enable/disable double jump
}

void Mario::setInvincible(bool enabled) {
    invincible = enabled; // Enable/disable invincibility
}
