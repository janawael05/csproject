#include "GameScene.h"
#include "ui_GameScene.h"
#include "MainWindow.h"
#include "Obstacle.h"
#include "Mario.h"
#include "QMessageBox"
#include <QGraphicsPixmapItem>
#include <QGraphicsProxyWidget>
#include <QFont>
#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>


GameScene::GameScene(QWidget *parent)
    //: QMainWindow(parent), score(0), lives(3)
    : QMainWindow(parent), score(0), coins(0), lives(3), currentLevel(1), speedBoostActive(false), doubleJumpActive(false), invincibilityActive(false), abilityDuration(5000) // 5 seconds for temporary abilities

{
    // Create the QGraphicsScene
    scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, 8000, 600); // Set the virtual game world size

    // Create a QGraphicsView to display the scene
    view = new QGraphicsView(this);
    view->setScene(scene);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setCentralWidget(view); // Set the view as the central widget of the window

    setFixedSize(800, 600); // Fixed view size (800x600 viewport)

    //Add background
    QGraphicsPixmapItem *background = new QGraphicsPixmapItem(QPixmap("/Users/janawael/supermario/background2.png"));
    background->setZValue(-1); // Ensure it's at the lowest z-index
    scene->addItem(background);

    //add score and life bar
    addUI();

    // Add obstacles
    createObstacles();
    addMovingObstacle();


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

            //mario->move(); // Move Mario

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

    // Timer for ability deactivation
    abilityTimer = new QTimer(this);
    connect(abilityTimer, &QTimer::timeout, this, &GameScene::deactivateAbilities);
    abilityTimer->setSingleShot(true); // Will trigger only once when the ability ends
 }

GameScene::~GameScene()
 {
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

    // Add coins display
    coinsText = new QGraphicsTextItem("Coins: 0");
    coinsText->setDefaultTextColor(Qt::yellow);
    coinsText->setFont(QFont("Arial", 24));
    scene->addItem(coinsText);
    coinsText->setZValue(2); // Ensure it renders above the background

    // Add store button
    storeButton = new QPushButton("Store", this);
    storeButton->setGeometry(700, 10, 80, 40); // Position the button in the top-right corner
    connect(storeButton, &QPushButton::clicked, this, &GameScene::openStore);

    // Add ability status display
    abilityStatusText = new QGraphicsTextItem("");
    abilityStatusText->setDefaultTextColor(Qt::blue);
    abilityStatusText->setFont(QFont("Arial", 18));
    scene->addItem(abilityStatusText);
    abilityStatusText->setZValue(2); // Render above the background

    // Add "Level: X" display
    levelText = new QGraphicsTextItem("Level: 1");
    levelText->setDefaultTextColor(Qt::white);
    levelText->setFont(QFont("Arial", 24));
    scene->addItem(levelText);
    levelText->setZValue(2); // Ensure it renders above the background


    // Position relative to the view
    updateUIPositions();
}

void GameScene::updateUIPositions() {
    // Ensure UI elements remain fixed relative to the viewport
    QPointF topLeft = view->mapToScene(0, 0); // Top-left of the viewport
    QPointF bottomRight = view->mapToScene(view->viewport()->width(), view->viewport()->height()); // Bottom-right of the viewport

    qreal viewCenterX = (topLeft.x() + bottomRight.x()) / 2; // Horizontal center of the viewport

    // Position score text at the top
    scoreText->setPos(topLeft.x() + 10, topLeft.y());

    // Position coins text directly below the score
    coinsText->setPos(topLeft.x() + 10, topLeft.y() + 40);

    // Position life bar below the coins text
    lifeBar->setPos(topLeft.x() + 10, topLeft.y() + 60);

    // Position ability status below life bar
    abilityStatusText->setPos(topLeft.x() + 10, topLeft.y() + 120);

    // Position level text at the top middle of the screen
    //levelText->setPos(view->width() / 2 - levelText->boundingRect().width() / 2, topLeft.y() + 10);
    levelText->setPos(viewCenterX - levelText->boundingRect().width() / 2, topLeft.y() + 10);

}

void GameScene::createObstacles() {
    int obstacleCount = 5 + currentLevel * 2;  // Increase number of obstacles as levels progress
    for (int i = 0;i < obstacleCount; ++i) {
             Obstacle *obstacle = new Obstacle();
             obstacle->setPos(300 + i * 500, 425); // Spread obstacles along the X-axis
             obstacles.append(obstacle);
             scene->addItem(obstacle);
         }
}

void GameScene::addMovingObstacle() {
    int movingObstacleCount = 5 + currentLevel * 2;  // Increase number of obstacles as levels progress
    for (int i = 0; i < movingObstacleCount; ++i) {
    MovingObstacle *bird = new MovingObstacle(300 + (i*700), 800, 300,2);
    bird->setSpeed(3);
    bird->setRange(200);
    scene->addItem(bird);
    }
}
void GameScene::updateScore(int points) {
    if (score < winscore) {
        score += points; // Increment score by the given points

        // Check if coins should increment (for every 10 points of score)
        int newCoins = score / 2; // Calculate coins based on total score
        if (newCoins > coins) {
            coins = newCoins; // Update coins only if newCoins is greater
            coinsText->setPlainText("Coins: " + QString::number(coins)); // Update coins UI
        }

        scoreText->setPlainText("Score: " + QString::number(score)); // Update score UI

        // Dynamically update winscore based on current level
        int levelThreshold = currentLevel * 100;  // Level 1 needs 100, Level 2 needs 200, etc.
        if (score >= levelThreshold) {
            finishLevel(); // Trigger finish level when score reaches the current level threshold
        }
    }
}


void GameScene::updateLives() {
    if (lives > 0) {
        lives--; // Decrement lives safely
        qDebug() << "Lives decremented. Remaining lives:" << lives;

        if (lives > 0) {
            // Update life bar width
            lifeBar->setRect(10, 30, 200 * (lives / 3.0), 20);
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


void GameScene::activateSpeedBoost() {
    qDebug() << "Speed Boost Activated";
    if (!speedBoostActive) {
        speedBoostActive = true;
        mario->setSpeed(2.0); // Double Mario's speed
        if (!abilityTimer->isActive()) { // Avoid overlapping timers
            abilityTimer->start(abilityDuration);
        }
    }
}

void GameScene::activateDoubleJump() {
    qDebug() << "Double Jump Activated";
    if (!doubleJumpActive) {
        doubleJumpActive = true;
        mario->setCanDoubleJump(true); // Enable double jump ability for Mario
        abilityTimer->start(abilityDuration); // Start the timer to deactivate after 5 seconds
    }
}

void GameScene::activateInvincibility() {
    qDebug() << "Invincibility Activated";

    if (!invincibilityActive) {
        invincibilityActive = true;
        mario->setInvincible(true); // Make Mario invincible
        abilityTimer->start(abilityDuration); // Start the timer to deactivate after 5 seconds
    }
}

void GameScene::deactivateAbilities() {
    qDebug() << "Deactivating Abilities";
    // Deactivate all abilities after the timer expires
    speedBoostActive = false;

    doubleJumpActive = false;

    invincibilityActive = false;

    mario->setSpeed(1.0); // Reset speed
    mario->setCanDoubleJump(false); // Disable double jump
    mario->setInvincible(false); // Disable invincibility

    updateAbilityUI(); // Update UI to reflect abilities being inactive
}

void GameScene::openStore() {
    // Create a QDialog for the store interface
    QDialog *storeDialog = new QDialog(this);
    storeDialog->setWindowTitle("Store");
    storeDialog->setFixedSize(300, 200);

    // Create a layout for the dialog
    QVBoxLayout *layout = new QVBoxLayout(storeDialog);

    // Create a label to show the coins and store description
    QLabel *storeLabel = new QLabel("Coins: " + QString::number(coins) + "\n\nSelect an ability to purchase:");
    layout->addWidget(storeLabel);

    // Create buttons for the three abilities
    QPushButton *speedBoostButton = new QPushButton("1. Speed Boost - 10 coins");
    QPushButton *doubleJumpButton = new QPushButton("2. Double Jump - 15 coins");
    QPushButton *invincibilityButton = new QPushButton("3. Invincibility - 20 coins");

    // Add buttons to the layout
    layout->addWidget(speedBoostButton);
    layout->addWidget(doubleJumpButton);
    layout->addWidget(invincibilityButton);

    // Connect the buttons to their respective slots
    connect(speedBoostButton, &QPushButton::clicked, [this, storeDialog]() {
        purchaseAbility("Speed Boost");
        storeDialog->accept(); // Close the dialog
        mario->setFocus();

    });
    connect(doubleJumpButton, &QPushButton::clicked, [this, storeDialog]() {
        purchaseAbility("Double Jump");
        storeDialog->accept(); // Close the dialog
        mario->setFocus();

    });
    connect(invincibilityButton, &QPushButton::clicked, [this, storeDialog]() {
        purchaseAbility("Invincibility");
        storeDialog->accept(); // Close the dialog
        mario->setFocus();

    });

    // Show the dialog
    storeDialog->show();
}


void GameScene::purchaseAbility(const QString &ability) {
    if (ability == "Speed Boost" && coins >= 10) {
        coins -= 10;
        activateSpeedBoost();
    } else if (ability == "Double Jump" && coins >= 15) {
        coins -= 15;
        activateDoubleJump();
    } else if (ability == "Invincibility" && coins >= 20) {
        coins -= 20;
        activateInvincibility();
    } else {
        QMessageBox::warning(this, "Not Enough Coins", "You do not have enough coins to buy this ability.");
        return;
    }
    updateAbilityUI(); // Update coins and ability status
}


void GameScene::updateAbilityUI() {
    // Update coins UI
    coinsText->setPlainText("Coins: " + QString::number(coins));

    // Update ability status
    if (speedBoostActive) {
        abilityStatusText->setPlainText("Ability: Speed Boost Active!");
    } else if (doubleJumpActive) {
        abilityStatusText->setPlainText("Ability: Double Jump Active!");
    } else if (invincibilityActive) {
        abilityStatusText->setPlainText("Ability: Invincibility Active!");
    } else {
        abilityStatusText->setPlainText("No Active Abilities");
    }
}

void GameScene::finishLevel() {
    // Clean up obstacles from the current level
    for (Obstacle *obstacle : obstacles) {
        scene->removeItem(obstacle);
        delete obstacle;
    }
    obstacles.clear();

    if (currentLevel<totalLevels) {

    // Show level completion message
    QGraphicsTextItem *levelCompleteText = new QGraphicsTextItem("CONGRATULATIONS! YOU FINISHED LEVEL " + QString::number(currentLevel));
    levelCompleteText->setDefaultTextColor(Qt::green);
    levelCompleteText->setFont(QFont("Arial", 30, QFont::Bold));
    //levelCompleteText->setPos(view->width() / 2 - 150, view->height() / 2 - 50);
    QPointF topLeft = view->mapToScene(0, 0); // Top-left of the current viewport
    QPointF bottomRight = view->mapToScene(view->viewport()->width(), view->viewport()->height()); // Bottom-right of the viewport
    // Calculate center of the current view
    qreal centerX = (topLeft.x() + bottomRight.x()) / 2.0;
    qreal centerY = (topLeft.y() + bottomRight.y()) / 2.0;

    // Position the text centered on the current view
    levelCompleteText->setPos(centerX - levelCompleteText->boundingRect().width() / 2,
                              centerY - levelCompleteText->boundingRect().height() / 2);

    scene->addItem(levelCompleteText);

    // Set a timer to remove the levelCompleteText after 3 seconds
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [=]() {
        scene->removeItem(levelCompleteText);
        delete levelCompleteText;
        timer->deleteLater(); // Clean up the timer
    });
    timer->start(3000); // 3 seconds (3000 milliseconds)

    // Stop timers (movement, score updates) to prevent further progress
    if (movementTimer) movementTimer->stop();
    if (scoreTimer) scoreTimer->stop();

    // Increment level number and update win score for the next level
    currentLevel++;
    winscore = currentLevel * 100;  // Set the winscore for the next level (100, 200, 300, etc.)

    // Update "Level: X" text
    levelText->setPlainText("Level: " + QString::number(currentLevel));

    // If it's not the last level, continue to the next level
    if (currentLevel <= totalLevels) {
        // Recreate obstacles and other level elements
        createObstacles();
        addMovingObstacle();

        // Restart timers for the new level
        if (movementTimer) movementTimer->start();
        if (scoreTimer) scoreTimer->start();
    }

    }

    else {
        // Display game complete or congratulatory message if last level is reached
        QGraphicsTextItem *gameCompleteText = new QGraphicsTextItem("CONGRATULATIONS! YOU FINISHED ALL LEVELS!");
        gameCompleteText->setDefaultTextColor(Qt::green);
        gameCompleteText->setFont(QFont("Arial", 30, QFont::Bold));
        //gameCompleteText->setPos(view->width() / 2 - 250, view->height() / 2 - 50);
        //scene->addItem(gameCompleteText);
        QPointF topLeft = view->mapToScene(0, 0); // Top-left of the current viewport
        QPointF bottomRight = view->mapToScene(view->viewport()->width(), view->viewport()->height()); // Bottom-right of the viewport
        // Calculate center of the current view
        qreal centerX = (topLeft.x() + bottomRight.x()) / 2.0;
        qreal centerY = (topLeft.y() + bottomRight.y()) / 2.0;
        // Position the text centered on the current view
        gameCompleteText->setPos(centerX - gameCompleteText->boundingRect().width() / 2,
                                  centerY - gameCompleteText->boundingRect().height() / 2);

        scene->addItem(gameCompleteText);

    }

}
