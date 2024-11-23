#ifndef MOVINGOBSTACLE_H
#define MOVINGOBSTACLE_H

#include <QGraphicsPixmapItem>
#include <QObject>
#include <QTimer>

class MovingObstacle : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT

public:
    explicit MovingObstacle(QGraphicsItem *parent = nullptr);

    void setSpeed(int speed);        // Set the speed of the obstacle's movement
    void setRange(int range);        // Set the range of the up-down movement

signals:
    void obstacleOutOfBounds();     // Signal emitted when the obstacle goes out of the scene

private slots:
    void move();                    // Slot for handling movement logic

private:
    QTimer *movementTimer;          // Timer to control the movement
    int direction;                  // Direction of movement: 1 (down) or -1 (up)
    int speed;                      // Speed of the movement
    int range;                      // Range of the up and down movement
    int currentDistance;            // Tracks how far the obstacle has moved in the current direction
};

#endif // MOVINGOBSTACLE_H
