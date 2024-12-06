#include "GameScene.h"
#include "ui_GameScene.h"
#include "MainWindow.h"
#include "Obstacle.h"
#include "MovingObstacle.h"
#include "Mario.h"
#include <QGraphicsPixmapItem>
#include <QGraphicsProxyWidget>
#include <QFont>
#include <QLabel>
#include <QProgressBar>


GameScene::GameScene(QWidget *parent)
    : QMainWindow(parent), score(0), lives(3)

{
    // Create the QGraphicsScene
    scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, 1600, 600); // Set the virtual game world size

    // Create a QGraphicsView to display the scene
    view = new QGraphicsView(this);
    view->setScene(scene);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setCentralWidget(view); // Set the view as the central widget of the window

    setFixedSize(800, 600); // Fixed view size (800x600 viewport)

    //Add background
    QGraphicsPixmapItem *background = new QGraphicsPixmapItem(QPixmap("/Users/janawael/supermario/background.png"));
    background->setZValue(-1); // Ensure it's at the lowest z-index
    scene->addItem(background);

    // // // Add score display
    // scoreText = new QGraphicsTextItem("Score: 0");
    // scoreText->setDefaultTextColor(Qt::white);
    // scoreText->setFont(QFont("Arial", 24));
    // scoreText->setPos(10, 10);
    // scoreText->setZValue(1); // Ensure it renders above the background
    // scene->addItem(scoreText);

    // // // Add life bar
    // lifeBar = new QGraphicsRectItem(10, 40, 200, 20); // Initial size of life bar
    // lifeBar->setBrush(Qt::green);
    // lifeBar->setZValue(1); // Ensure it renders above the background
    // scene->addItem(lifeBar);

    //add score and life bar
    addUI();

    // Add obstacles
    createObstacles();

    // Add Mario
    mario = new Mario();
    mario->setPos(100, 400); // Starting position
    scene->addItem(mario);

    // Ensure Mario has focus
    mario->setFocus();

    // Add a timer to handle Mario's movement
    movementTimer = new QTimer(this); // Use the member variable here
    connect(movementTimer, &QTimer::timeout, [this]() {
        if (mario) {
            mario->move();
            centerViewOnMario();
        }
    });
    movementTimer->start(16); // ~60 FPS

    // Update score based on Mario's progress
    scoreTimer = new QTimer(this);
    connect(scoreTimer, &QTimer::timeout, [this]() {
        if (mario) {
            static int lastX = mario->x(); // Store the last X position of Mario

            mario->move(); // Move Mario

            // Check if Mario moved horizontally
            if (mario->x() > lastX) {
                updateScore(1); // Increment score when Mario moves right
                lastX = mario->x(); // Update the last X position
            }
        }
    });

    scoreTimer->start(100); // Update score every 100ms

    // Connect Mario's signal to update lives
    connect(mario, &Mario::marioHitObstacle, this, &GameScene::updateLives); 
 }

GameScene::~GameScene() {
    if (movementTimer) {
        movementTimer->stop();
        delete movementTimer;
    }
    if (scoreTimer) {
        scoreTimer->stop();
        delete scoreTimer;
    }
    delete scene;
}

void GameScene::addUI() {
    // Add score display
    scoreText = new QGraphicsTextItem("Score: 0");
    scoreText->setDefaultTextColor(Qt::white);
    scoreText->setFont(QFont("Arial", 24));
    scoreText->setPos(10, 10);
    scene->addItem(scoreText);
    scoreText->setZValue(2); // Ensure it renders above the background

    // Add life bar
    lifeBar = new QGraphicsRectItem();
    lifeBar->setBrush(Qt::green);
    scene->addItem(lifeBar);
    lifeBar->setRect(0, 30, 200, 20); // Set initial size
    lifeBar->setZValue(2); // Ensure it renders above the background

    // Position score and life bar relative to the view
    updateUIPositions();
}

void GameScene::updateUIPositions() {
    // Ensure UI elements remain fixed relative to the viewport
    QPointF topLeft = view->mapToScene(0, 0); // Top-left of the viewport
    scoreText->setPos(topLeft.x() + 10, topLeft.y());
    lifeBar->setPos(topLeft.x() + 10, topLeft.y());
}

void GameScene::createObstacles() {
    // Example obstacle setup
    for (int i = 0; i < 5; ++i) {
        Obstacle *obstacle = new Obstacle();
        obstacle->setPos(300 + i * 500, 425); // Spread obstacles along X-axis
        obstacles.append(obstacle);
        scene->addItem(obstacle);
    }
}

void GameScene::updateScore(int points) {
    if (score < winscore) {
        score += points; // Increment score by the given points
        scoreText->setPlainText("Score: " + QString::number(score));

        // Check if the level is completed
        if (score >= winscore) { // Trigger when score reaches or exceeds 15
            // Display "Congratulations" message
            for (Obstacle *obstacle : obstacles) {
                scene->removeItem(obstacle);
                delete obstacle;
            }
            obstacles.clear();

            // Show "CONGRATULATIONS!" message
            QGraphicsTextItem *levelCompleteText = new QGraphicsTextItem("CONGRATULATIONS! YOU FINISHED LEVEL 1");
            levelCompleteText->setDefaultTextColor(Qt::green);
            levelCompleteText->setFont(QFont("Arial", 48, QFont::Bold));
            levelCompleteText->setPos(view->width() / 2 - 150, view->height() / 2 - 50);
            scene->addItem(levelCompleteText);

            // Stop timers to prevent further updates
            if (movementTimer) movementTimer->stop();
            if (scoreTimer) scoreTimer->stop();

            // Return to the main menu after a delay
            QTimer::singleShot(3000, [this]() {
                MainWindow *open = new MainWindow;
                open->show(); // Show the main window
                this->close(); // Close the game scene
            });
        }
    }
}
void GameScene::updateLives() {
    if (lives > 0) {
        lives--; // Decrement lives safely
        qDebug() << "Lives decremented. Remaining lives:" << lives;

        if (lives > 0) {
            // Update life bar width
            lifeBar->setRect(10, 40, 200 * (lives / 3.0), 20);
        } else {
            qDebug() << "Game Over triggered from GameScene!";
            lives = 0;

            // Stop timers to prevent further updates
            if (movementTimer) {
                movementTimer->stop();
                qDebug() << "Movement timer stopped.";
            }
            if (scoreTimer) {
                scoreTimer->stop();
                qDebug() << "Score timer stopped.";
            }

            // Remove Mario from the scene
            if (mario) {
                mario->stopCollisionCooldown();
            }

            // Clear all obstacles
            for (Obstacle *obstacle : obstacles) {
                scene->removeItem(obstacle);
                delete obstacle;
            }
            obstacles.clear();

            // Show "Game Over" message
            QGraphicsTextItem *gameOverText = new QGraphicsTextItem("GAME OVER");
            gameOverText->setDefaultTextColor(Qt::red);
            gameOverText->setFont(QFont("Arial", 48, QFont::Bold));
            // gameOverText->setPos(view->width() / 2 - 150, view->height() / 2 - 50);
            QPointF topLeft = view->mapToScene(0, 0); // Top-left of the current viewport
            QPointF bottomRight = view->mapToScene(view->viewport()->width(), view->viewport()->height()); // Bottom-right of the viewport

            // Calculate center of the current view
            qreal centerX = (topLeft.x() + bottomRight.x()) / 2.0;
            qreal centerY = (topLeft.y() + bottomRight.y()) / 2.0;

            // Position the text centered on the current view
            gameOverText->setPos(centerX - gameOverText->boundingRect().width() / 2,
                                 centerY - gameOverText->boundingRect().height() / 2);

            scene->addItem(gameOverText);

            // Return to the main menu after a delay
            QTimer::singleShot(3000, [this]() {
                MainWindow *open = new MainWindow;
                open->show(); // Show the main window
                this->close(); // Close the game scene
            });
        }
    } else {
        qDebug() << "No lives left to decrement.";
    }
}

void GameScene::centerViewOnMario() {
    QPointF target = mario->pos();

    // Define scene boundaries for scrolling
    qreal halfWidth = view->viewport()->width() / 2.0;
    qreal halfHeight = view->viewport()->height() / 2.0;

    qreal x = qMax(halfWidth, qMin(scene->width() - halfWidth, target.x()));
    qreal y = qMax(halfHeight, qMin(scene->height() - halfHeight, target.y()));

    view->centerOn(x, y);
    updateUIPositions();
}

void GameScene::finishLevel() {
    // Stop the game
    for (Obstacle *obstacle : obstacles) {
        scene->removeItem(obstacle);
        delete obstacle; // Clean up memory
    }
    obstacles.clear();

    scene->removeItem(mario);
    delete mario;
    mario = nullptr;

    QGraphicsTextItem *congratsText = new QGraphicsTextItem("Congratulations, you finished level 1!");
    congratsText->setDefaultTextColor(Qt::green);
    congratsText->setFont(QFont("Arial", 36, QFont::Bold));
    congratsText->setPos(scene->width() / 2 - 300, scene->height() / 2 - 50); // Center text
    scene->addItem(congratsText);

    QTimer::singleShot(3000, [this]() {
        MainWindow *open = new MainWindow;
        open->show(); // Return to main menu
        close();      // Close the current GameScene
    });
}


