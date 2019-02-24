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

private slots:
    void on_showVectors_stateChanged(int arg1);

    void on_vectorColor_stateChanged(int arg1);

    void on_showSmoke_stateChanged(int arg1);

    void on_smokeColor0_clicked();

    void on_smokeColor1_clicked();

    void on_smokeColor3_clicked();

    void on_viscositySlider_valueChanged(int value);

    void on_timestepSlider_valueChanged(int value);

    void on_vectorScale_valueChanged(int value);

    void on_playPauzeButton_clicked();

private:
    Ui::SmokeWindow *ui;
};

#endif // SMOKEWINDOW_H
