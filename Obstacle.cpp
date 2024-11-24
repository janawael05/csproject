#include "Obstacle.h"
#include <QGraphicsPixmapItem>

Obstacle::Obstacle(QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent) {
    setPixmap(QPixmap("/Users/janawael/supermario/finalObstacle.png").scaled(100, 100)); // Load obstacle sprite
}
