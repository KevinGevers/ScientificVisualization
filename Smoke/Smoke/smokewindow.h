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

    void on_viscositySlider_valueChanged(int value);

    void on_timestepSlider_valueChanged(int value);

    void on_vectorScale_valueChanged(int value);

    void on_playPauzeButton_clicked();

    void actionPauze();

    void actionSave();

    void on_NumberColors_valueChanged(int value);

    void on_Saturation_valueChanged(int value);

    void on_Hue_valueChanged(int value);

    void on_smokeColor_currentIndexChanged(const QString &option);

    void on_ClippingMin_valueChanged(int value);

    void on_ClippingMax_valueChanged(int value);

    void on_GlyphXSampler_valueChanged(int value);

    void on_GlyphYSampler_valueChanged(int value);

    void on_Shape_currentIndexChanged(const QString &option);

    void on_VectorField_currentIndexChanged(const QString &option);

    void on_ColorBasedOn_currentIndexChanged(const QString &option);

private:
    Ui::SmokeWindow *ui;
};

#endif // SMOKEWINDOW_H
