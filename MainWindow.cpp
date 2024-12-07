#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "GameScene.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), gameScene(nullptr) {
    ui->setupUi(this);  // Set up the UI from the .ui file

    // Connect the Start Game button to the startGame slot
    connect(ui->startGameButton, &QPushButton::clicked, this, &MainWindow::startGame);

    // Connect the Exit Game button to the exitGame slot
    connect(ui->exitGameButton, &QPushButton::clicked, this, &MainWindow::exitGame);
}

MainWindow::~MainWindow() {
    delete ui;
    delete gameScene; // Delete the gameScene if it was created
}

void MainWindow::startGame() {
    // Ensure gameScene is not already running
    if (!gameScene) {
        gameScene = new GameScene();  // Dynamically allocate memory for the game scene
        gameScene->show();           // Show the game scene
        this->close();               // Close the main window
    }
}

void MainWindow::exitGame() {
    close();  // Close the main window gracefully
}


void MainWindow::on_startGameButton_clicked()
{
    startGame();
}


void MainWindow::on_exitGameButton_clicked()
{
    close();  // Close the main window gracefully
}

