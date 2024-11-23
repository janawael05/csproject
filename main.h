#ifndef MAIN_H
#define MAIN_H

#include <QMainWindow>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class Main;
}
QT_END_NAMESPACE

class Main : public QWidget
{
    Q_OBJECT

public:
    Main(QWidget *parent = nullptr);
    ~Main();
};
#endif // MAIN_H
