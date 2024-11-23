#include "Mario.h"
#include "Obstacle.h"
#include <QList>
#include <QGraphicsItem>
#include "QtGui/qevent.h"
#include <QBrush>
#include <QGraphicsScene>
#include <QKeyEvent>

Mario::Mario(QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent), velocityX(0), velocityY(0), onGround(true)
{
    setPixmap(QPixmap("/Users/janawael/supermario/supermario.png").scaled(100, 100));
    // Enable focus so Mario can receive key events
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFocus();

    // Initialize the physics timer
    physicsTimer = new QTimer(this);
    connect(physicsTimer, &QTimer::timeout, this, &Mario::updatePosition);
    physicsTimer->start(20); // Update every 20ms
}

void Mario::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Left:
        velocityX = -5;
        break;
    case Qt::Key_Right:
        velocityX = 5;
        break;
    case Qt::Key_Space:
        if (onGround) {
            velocityY = -15; // Jump upwards
            onGround = false;
        }
        break;
    default:
        break;
    }
}


void Mario::keyReleaseEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Left || event->key() == Qt::Key_Right) {
        velocityX = 0; // Stop moving when key is released
    }
}


void Mario::move() {
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

    // Check for collisions
    QList<QGraphicsItem*> collidingItems = scene()->collidingItems(this);
    for (QGraphicsItem* item : collidingItems) {
        Obstacle* obstacle = dynamic_cast<Obstacle*>(item);
        if (obstacle) {
            // Check if Mario is landing on top of the obstacle
            QRectF marioBounds = boundingRect().translated(pos());
            QRectF obstacleBounds = obstacle->boundingRect().translated(obstacle->pos());

            if (marioBounds.bottom() <= obstacleBounds.top() + 5 && velocityY > 0) {
                // Mario lands on top of the obstacle
                setPos(x(), obstacleBounds.top() - marioBounds.height()); // Snap Mario to obstacle's top
                velocityY = 0; // Stop vertical motion
                onGround = true; // Mario is now on the ground
            } else {
                // Side collision, Mario loses a life
                emit marioHitObstacle(); // Signal to reduce Mario's lives
            }
        }
    }


    // While jumping, make Mario land further
    if (!onGround) {
        // Apply a slight horizontal speed increase while jumping
        velocityX = 5; // Mario moves forward while jumping
    }

    // Check if Mario has landed on the ground
    if (y() >= 400) { // Ground level (adjust as necessary)
        setPos(x(), 400); // Correct position on the ground
        velocityY = 0;     // Reset vertical velocity
        onGround = true;   // Mario is now on the ground

        // Stop horizontal movement after landing
        velocityX = 0;     // Stop horizontal movement after landing
    }
}
void Mario::checkCollisions() {
    QList<QGraphicsItem *> collidingItems = scene()->collidingItems(this);

    for (QGraphicsItem *item : collidingItems) {
        Obstacle *obstacle = dynamic_cast<Obstacle *>(item);
        if (obstacle) {
            // Check if Mario lands on top of the obstacle
            if (y() + boundingRect().height() <= obstacle->y()) {
                // Landed on top of obstacle
                setPos(x(), obstacle->y() - boundingRect().height());
                velocityY = 0; // Stop downward motion
                onGround = true;
            } else {
                // Hit the side of an obstacle
                emit marioHitObstacle();
                return;
            }
        }
    }
}
