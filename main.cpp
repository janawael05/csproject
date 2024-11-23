#include "main.h"
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QApplication>

Main::Main(QWidget *parent) : QWidget(parent) {
    // Constructor implementation
}

Main::~Main() {
    // Destructor implementation
}
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    MainWindow * open = new MainWindow;
    open->show(); //show the main window

    return a.exec(); // Enter the Qt event loop
}
