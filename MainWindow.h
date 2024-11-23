#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "GameScene.h"  // Include the GameScene class

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void startGame();  // Slot for starting the game
    void exitGame();  // Slot for exiting the game

    void on_startGameButton_clicked();

    void on_exitGameButton_clicked();

private:
    Ui::MainWindow *ui;
    GameScene *gameScene; // Pointer to the GameScene (for the actual gameplay)
};

#endif // MAINWINDOW_H

