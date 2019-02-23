#ifndef SMOKEWINDOW_H
#define SMOKEWINDOW_H

#include <QMainWindow>

namespace Ui {
class SmokeWindow;
}

class SmokeWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SmokeWindow(QWidget *parent = nullptr);
    ~SmokeWindow();

private:
    Ui::SmokeWindow *ui;
};

#endif // SMOKEWINDOW_H
