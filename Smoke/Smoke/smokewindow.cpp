#include "smokewindow.h"
#include "ui_smokewindow.h"
#include <QFileDialog>
#include <QDebug>

SmokeWindow::SmokeWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SmokeWindow)
{
    ui->setupUi(this);
    connect(ui->actionSave, &QAction::triggered, this, &SmokeWindow::actionSave);
    connect(ui->actionPauze, &QAction::triggered, this, &SmokeWindow::actionPauze);
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
    actionPauze();
}

void SmokeWindow::actionPauze()
{
    int state = ui->VisualizationWidget->toggle_frozen();

    if (state == 0) {
        ui->playPauzeButton->setText("Pauze");
        ui->actionPauze->setText("Pauze");
    } else {
        ui->playPauzeButton->setText("Play");
        ui->actionPauze->setText("Play");
    }
}

void SmokeWindow::actionSave()
{
    actionPauze();
    QString file = QFileDialog::getSaveFileName(this, "Save mesh", "", tr("PNG (*.png)" ));
    if (!file.isEmpty()){
        QImage img = ui->VisualizationWidget->grabFramebuffer();
        img.save(file);
        actionPauze();
    }
}

void SmokeWindow::on_NumberColors_valueChanged(int value)
{
    //TODO: Change number of colors based on this
    //Colors range from 2 to 256
}

void SmokeWindow::on_Saturation_valueChanged(int value)
{
    //TODO: Change saturation based on this
}

void SmokeWindow::on_Hue_valueChanged(int value)
{
    //TODO: Change Hue based on this
}

void SmokeWindow::on_horizontalSlider_valueChanged(int value)
{
    //TODO: Change minimal value of clipping to this
}

void SmokeWindow::on_horizontalSlider_2_valueChanged(int value)
{
    //TODO: Change maximal value of clipping to this
}
