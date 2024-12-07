#include "MovingObstacle.h" #include <QGraphicsScene>
MovingObstacle::MovingObstacle(qreal startX, qreal endX, qreal y, int speed, QObject *parent) : QObject(parent), startX(startX), endX(endX), y(y), speed(speed), direction(1), range(0) {
    // Set initial position
    setPos(startX, y);
    setPixmap(QPixmap("/Users/janawael/supermario/bird2.png").scaled(70, 70)); // Make sure the image path is correct
    // Initialize movement timer
    movementTimer = new QTimer(this);
    connect(movementTimer, &QTimer::timeout, this, &MovingObstacle::move); movementTimer->start(16); // ~60 FPS (16 ms per frame)
}
void MovingObstacle::setSpeed(int speed) {
    this->speed = speed; // Set the speed of the obstacle's movement
}
void MovingObstacle::setRange(int range) {
    this->range = range; // Set the range of the up and down movement
}
void MovingObstacle::move() { // Get the current X position
    qreal currentX = x();
    // Move the bird horizontally based on speed and direction
    if (direction == 1) {
    // Move right
        if (currentX + speed <= endX) {
        setPos(currentX + speed, y); }
        else {
        // If reached endX, reverse direction
        direction = -1;
        }
    }
 else {
    // Move left
    if (currentX - speed >= startX) {
        setPos(currentX - speed, y); } else {
        // If reached startX, reverse direction direction = 1;
    } }
}
