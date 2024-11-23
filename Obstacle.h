#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <QGraphicsPixmapItem>

class Obstacle : public QGraphicsPixmapItem {
public:
    explicit Obstacle(QGraphicsItem *parent = nullptr);
};

#endif // OBSTACLE_H
