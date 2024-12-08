#ifndef MOVINGOBSTACLE_H
#define MOVINGOBSTACLE_H
#include <QGraphicsPixmapItem>
#include <QObject>
#include <QTimer>
class MovingObstacle : public QObject, public QGraphicsPixmapItem { Q_OBJECT
public:
    MovingObstacle(qreal startX, qreal endX, qreal y, int speed, QObject *parent = nullptr);
    void setSpeed(int speed);
    void setRange(int range);
public slots:
    void move();
private:
    qreal startX;
    qreal endX;
    qreal y;
    int speed;
    int range;
    int direction;
    QTimer *movementTimer;
};
#endif // MOVINGOBSTACLE_H
