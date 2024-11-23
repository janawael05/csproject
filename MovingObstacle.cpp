#include "MovingObstacle.h"
#include <QGraphicsScene>

MovingObstacle::MovingObstacle(QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent),
    direction(1),            // Start by moving down
    speed(2),                // Default speed
    range(100),              // Default range
    currentDistance(0)       // Start at initial position
{
    // Load the bird image or graphic (adjust the path as needed)
    setPixmap(QPixmap("/Users/janawael/supermario/bird.png").scaled(30, 30));
    // Create a timer for movement
    movementTimer = new QTimer(this);

    // Connect the timer to the move() slot
    connect(movementTimer, &QTimer::timeout, this, &MovingObstacle::move);

    // Start the timer
    movementTimer->start(30);  // Update every 30 ms
}

void MovingObstacle::setSpeed(int speed) {
    this->speed = speed;  // Set the speed of the obstacle's movement
}

void MovingObstacle::setRange(int range) {
    this->range = range;  // Set the range of the up and down movement
}

void MovingObstacle::move() {
    // Move the obstacle up or down based on the current direction
    setY(y() + direction * speed);
    currentDistance += speed;

    // Reverse direction if the obstacle exceeds the range
    if (currentDistance >= range || currentDistance <= 0) {
        direction *= -1;  // Reverse the direction
    }

    // Emit a signal if the obstacle moves out of the scene bounds
    if (scene() && (y() < 0 || y() > scene()->height())) {
        emit obstacleOutOfBounds();
    }
}
