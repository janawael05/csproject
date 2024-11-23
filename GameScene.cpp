#include "GameScene.h"
#include "ui_GameScene.h"
#include "MainWindow.h"
#include "Obstacle.h"
#include "MovingObstacle.h"
#include <QGraphicsPixmapItem>
#include <QFont>

GameScene::GameScene(QWidget *parent)
    : QMainWindow(parent), score(0), lives(3) {
    // Create the QGraphicsScene
    scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, 1600, 600); // Set the virtual game world size

    // Create a QGraphicsView to display the scene
    view = new QGraphicsView(this);
    view->setScene(scene);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setCentralWidget(view); // Set the view as the central widget of the window

    // Resize the view to match the scene size
    setFixedSize(scene->width(), scene->height());

    // Add background
    QGraphicsPixmapItem *background = new QGraphicsPixmapItem(QPixmap("/Users/janawael/supermario/background.png"));
    background->setZValue(-1); // Ensure it's at the lowest z-index
    scene->addItem(background);

    // Add score display
    scoreText = new QGraphicsTextItem("Score: 0");
    scoreText->setDefaultTextColor(Qt::white);
    scoreText->setFont(QFont("Arial", 24));
    scoreText->setPos(10, 10);
    scoreText->setZValue(1); // Ensure it renders above the background
    scene->addItem(scoreText);

    // Add life bar
    lifeBar = new QGraphicsRectItem(10, 40, 200, 20); // Initial size of life bar
    lifeBar->setBrush(Qt::green);
    lifeBar->setZValue(1); // Ensure it renders above the background
    scene->addItem(lifeBar);

    // Add obstacles
    createObstacles();

    // Add Mario
    mario = new Mario();
    mario->setPos(100, 400); // Starting position
    scene->addItem(mario);

    // Ensure Mario has focus
    mario->setFocus();

    // Add a timer to handle Mario's movement
    QTimer *movementTimer = new QTimer(this);
    connect(movementTimer, &QTimer::timeout, [this]() {
        if (mario) mario->move(); // Move Mario periodically
    });
    movementTimer->start(16); // Call the move function every 16ms (~60 FPS)

    // Update score based on Mario's progress
    QTimer *scoreTimer = new QTimer(this);
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
    delete scene;
}

void GameScene::createObstacles() {
    // Example obstacle setup
    for (int i = 0; i < 5; ++i) {
        Obstacle *obstacle = new Obstacle();
        obstacle->setPos(300 + i * 500, 390); // Spread obstacles along X-axis
        obstacles.append(obstacle);
        scene->addItem(obstacle);
    }
}

void GameScene::updateScore(int points) {
    score += points; // Increment score by the given points
    scoreText->setPlainText("Score: " + QString::number(score));
    // Check if the level is completed
    if (score >= 500) {
        // Display "Congratulations" message
        QGraphicsTextItem *levelCompleteText = new QGraphicsTextItem("CONGRATULATIONS! YOU FINISHED LEVEL 1");
        levelCompleteText->setDefaultTextColor(Qt::yellow);
        levelCompleteText->setFont(QFont("Arial", 36, QFont::Bold));
        levelCompleteText->setPos(scene->width() / 2 - 300, scene->height() / 2 - 50);
        scene->addItem(levelCompleteText);

        // Stop the game
        for (Obstacle *obstacle : obstacles) {
            scene->removeItem(obstacle);
            delete obstacle;
        }
        obstacles.clear();

        scene->removeItem(mario);
        delete mario;
        mario = nullptr;

        return; // Exit the function
    }
}

void GameScene::updateLives() {
    lives--; // Decrement lives

    if (lives > 0) {
        // Update life bar width
        lifeBar->setRect(10, 40, 200 * (lives / 3.0), 20);
    } else {
        lives = 0;

        // Stop the game
        for (Obstacle *obstacle : obstacles) {
            scene->removeItem(obstacle);
            delete obstacle; // Clean up memory
        }
        obstacles.clear();

        scene->removeItem(mario);
        delete mario;
        mario = nullptr;

        // Show "Game Over" message
        QGraphicsTextItem *gameOverText = new QGraphicsTextItem("GAME OVER");
        gameOverText->setDefaultTextColor(Qt::red);
        gameOverText->setFont(QFont("Arial", 48, QFont::Bold));
        gameOverText->setPos(scene->width() / 2 - 150, scene->height() / 2 - 50); // Center text
        scene->addItem(gameOverText);

        MainWindow *open = new MainWindow;
        open->show(); // Show the main window
        return;
    }
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

    // Show "Congratulations" message
    QGraphicsTextItem *congratsText = new QGraphicsTextItem("Congratulations, you finished level 1!");
    congratsText->setDefaultTextColor(Qt::green);
    congratsText->setFont(QFont("Arial", 36, QFont::Bold));
    congratsText->setPos(scene->width() / 2 - 300, scene->height() / 2 - 50); // Center text
    scene->addItem(congratsText);

    // Optionally, add a delay before moving to the next level
    QTimer::singleShot(3000, [this]() {
        MainWindow *open = new MainWindow;
        open->show(); // Return to main menu
        close();      // Close the current GameScene
    });
}
