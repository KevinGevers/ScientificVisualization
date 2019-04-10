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
    connect(ui->actionReset_simulation, &QAction::triggered, this, &SmokeWindow::actionResetSimulation);
    connect(ui->actionReset_visualization, &QAction::triggered, this, &SmokeWindow::actionResetVisualization);
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

void SmokeWindow::actionResetSimulation()
{
    ui->VisualizationWidget->reset_simulation();
}

void SmokeWindow::actionResetVisualization()
{
    // Reset simulation settings
    ui->timestepSlider->setValue(399);
    ui->viscositySlider->setValue(0);

    // Reset glyphs
    ui->showVectors->setChecked(true);
    ui->vectorColor->setChecked(true);
    ui->jitterVectors->setChecked(true);
    ui->vectorScale->setValue(1000);
    ui->Shape->setCurrentText("Hedgehogs");
    ui->VectorField->setCurrentText("Velocity");
    ui->GlyphXSampler->setValue(50);
    ui->GlyphYSampler->setValue(50);

    // Reset height plot
    ui->HeightPlot->setChecked(false);
    ui->RotateSlider->setValue(0);
    ui->HeightPlotScaleSlider->setValue(50);

    // Reset smoke
    ui->showSmoke->setChecked(true);
    ui->SmokeMode->setCurrentText("Density");
    ui->smokeColor->setCurrentText("Greyscale");
    ui->NumberColors->setValue(256);
    ui->Saturation->setValue(1000);
    ui->Hue->setValue(0);
    ui->colorScaling->setChecked(true);
    ui->ClippingMax->setValue(100);
    ui->ClippingMin->setValue(0);

    // Reset isolines
    ui->Isoline->setChecked(false);
    ui->IsolineColorCheckbox->setChecked(false);
    ui->ThresholdSlider->setValue(0);
    ui->NumberIsolinesSlider->setValue(1);
    ui->MinRhoSlider->setValue(0);
    ui->MaxRhoSlider->setValue(1);
}

void SmokeWindow::on_NumberColors_valueChanged(int value)
{
    ui->VisualizationWidget->set_number_colors(value);
    ui->NumberColorsLabel->setText(QStringLiteral("Number of colors: %1").arg(value));

}

void SmokeWindow::on_Saturation_valueChanged(int value)
{
    ui->VisualizationWidget->set_saturation(value / 1000.0f);
}

void SmokeWindow::on_Hue_valueChanged(int value)
{
    ui->VisualizationWidget->set_hue(value / 1000.0f);
}

void SmokeWindow::on_smokeColor_currentIndexChanged(const QString &option)
{
    if (option.compare("Rainbow") == 0)
        ui->VisualizationWidget->set_smoke_colors(1);
    else if (option.compare("White-to-red") == 0)
        ui->VisualizationWidget->set_smoke_colors(3);
    else
        // Greyscale
        ui->VisualizationWidget->set_smoke_colors(0);
}

void SmokeWindow::on_ClippingMin_valueChanged(int value)
{
    // Range between 0 and 10
    ui->VisualizationWidget->set_clipping_min(value / 100.0f);
    ui->MinClippingLabel->setText(QStringLiteral("Min clipping: %1").arg(value / 100.0f));

}

void SmokeWindow::on_ClippingMax_valueChanged(int value)
{
    // Range between 0 and 10
    ui->VisualizationWidget->set_clipping_max(value / 100.0f);
    ui->MaxClippingLabel->setText(QStringLiteral("Max clipping: %1").arg(value / 100.0f));

}

void SmokeWindow::on_GlyphXSampler_valueChanged(int value)
{
    ui->VisualizationWidget->set_glyph_x_amount(value);
    ui->XSamplingLabel->setText(QStringLiteral("X Sampling: %1").arg(value));

}

void SmokeWindow::on_GlyphYSampler_valueChanged(int value)
{
    ui->VisualizationWidget->set_glyph_y_amount(value);
    ui->YSamplingLabel->setText(QStringLiteral("Y Sampling: %1").arg(value));
}

void SmokeWindow::on_Shape_currentIndexChanged(const QString &option)
{
    if (option.compare("Hedgehogs") == 0)
        ui->VisualizationWidget->set_shape(0);
    else if (option.compare("Cones") == 0)
        ui->VisualizationWidget->set_shape(1);
    else if (option.compare("Arrows") == 0)
        ui->VisualizationWidget->set_shape(2);
    else if (option.compare("Circles") == 0)
        ui->VisualizationWidget->set_shape(3);
}

void SmokeWindow::on_VectorField_currentIndexChanged(const QString &option)
{
    if (option.compare("Velocity") == 0)
        ui->VisualizationWidget->set_vector_field(0);
    else if (option.compare("Force") == 0)
        ui->VisualizationWidget->set_vector_field(1);
}

void SmokeWindow::on_ColorBasedOn_currentIndexChanged(const QString &option)
{
    if (option.compare("None") == 0)
        ui->VisualizationWidget->set_color_based_on(0);
    else if (option.compare("Density") == 0)
        ui->VisualizationWidget->set_color_based_on(1);
    else if (option.compare("Fluid velocity magnitude") == 0)
        ui->VisualizationWidget->set_color_based_on(2);
    else if (option.compare("Force field magnitude") == 0)
        ui->VisualizationWidget->set_color_based_on(3);
}

void SmokeWindow::on_colorScaling_stateChanged(int state)
{
    ui->VisualizationWidget->set_scale_colors(state/2);
}

void SmokeWindow::on_SmokeMode_currentIndexChanged(const QString &mode)
{
    if (mode.compare("Density") == 0)
        ui->VisualizationWidget->set_smoke_mode(0);
    else if (mode.compare("Divergence on Velocity") == 0)
        ui->VisualizationWidget->set_smoke_mode(1);
    else if (mode.compare("Divergence on Force") == 0)
        ui->VisualizationWidget->set_smoke_mode(2);
}

void SmokeWindow::on_jitterVectors_stateChanged(int state)
{
    ui->VisualizationWidget->set_vector_jitter(state/2);
}

void SmokeWindow::on_Isoline_stateChanged(int state)
{
    ui->VisualizationWidget->set_isolines(state/2);
}

void SmokeWindow::on_ThresholdSlider_valueChanged(int value)
{
    ui->VisualizationWidget->set_threshold(value);
    ui->ThresholdLabel->setText(QStringLiteral("Threshold: %1").arg(value));

}

void SmokeWindow::on_NumberIsolinesSlider_valueChanged(int value)
{
    ui->VisualizationWidget->set_number_isolines(value);
    ui->NumberIsolinesLabel->setText(QStringLiteral("Number of isolines: %1").arg(value));

}

void SmokeWindow::on_MinRhoSlider_valueChanged(int value)
{
    ui->VisualizationWidget->set_min_rho(value);
    ui->MinRhoLabel->setText(QStringLiteral("Min rho: %1").arg(value));

}

void SmokeWindow::on_MaxRhoSlider_valueChanged(int value)
{
    ui->VisualizationWidget->set_max_rho(value);
    ui->MaxRhoLabel->setText(QStringLiteral("Max rho: %1").arg(value));
}

void SmokeWindow::on_HeightPlot_stateChanged(int state)
{
    ui->VisualizationWidget->set_heightplot(state/2);
}

void SmokeWindow::on_RotateSlider_valueChanged(int value)
{
    ui->VisualizationWidget->set_rotation(value);
    ui->RotateLabel->setText(QStringLiteral("Rotate: %1 degrees").arg(value));
}

void SmokeWindow::on_IsolineColorCheckbox_stateChanged(int state)
{
    ui->VisualizationWidget->set_isoline_color(state/2);
}

void SmokeWindow::on_HeightPlotScaleSlider_valueChanged(int value)
{
    ui->VisualizationWidget->set_heightplot_scale(value);
    ui->HeightPlotScaleLabel->setText(QStringLiteral("Scale: %1").arg(value));
}
