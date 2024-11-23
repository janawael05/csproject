#include "Obstacle.h"
#include <QGraphicsPixmapItem>

Obstacle::Obstacle(QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent) {
    setPixmap(QPixmap("/Users/janawael/supermario/finalObstacle.png").scaled(150, 150)); // Load obstacle sprite
}
