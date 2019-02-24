#include "smokewindow.h"
#include "ui_smokewindow.h"
#include <QDebug>

SmokeWindow::SmokeWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SmokeWindow)
{
    ui->setupUi(this);
}

SmokeWindow::~SmokeWindow()
{
    delete ui;
}

void SmokeWindow::on_showVectors_stateChanged(int state)
{
    ui->VisualizationWidget->set_draw_vectors(state/2);
}

void SmokeWindow::on_vectorColor_stateChanged(int state)
{
    ui->VisualizationWidget->set_vector_colors(state/2);
}

void SmokeWindow::on_showSmoke_stateChanged(int state)
{
    ui->VisualizationWidget->set_draw_smoke(state/2);
}

void SmokeWindow::on_smokeColor0_clicked()
{
    ui->VisualizationWidget->set_smoke_colors(0);
}

void SmokeWindow::on_smokeColor1_clicked()
{
    ui->VisualizationWidget->set_smoke_colors(1);
}

void SmokeWindow::on_smokeColor3_clicked()
{
    ui->VisualizationWidget->set_smoke_colors(3);
}

void SmokeWindow::on_viscositySlider_valueChanged(int value)
{
    ui->VisualizationWidget->get_simulation()->set_viscosity((value + 1) / 1000.0f);
}

void SmokeWindow::on_timestepSlider_valueChanged(int value)
{
    ui->VisualizationWidget->get_simulation()->set_dt((value + 1) / 1000.0f);
}

void SmokeWindow::on_vectorScale_valueChanged(int value)
{
    ui->VisualizationWidget->set_vector_scale(value);
}

void SmokeWindow::on_playPauzeButton_clicked()
{
    int state = ui->VisualizationWidget->toggle_frozen();

    if (state == 1)
        ui->playPauzeButton->setText("Pauze");
    else
        ui->playPauzeButton->setText("Play");
}
