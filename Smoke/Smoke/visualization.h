#ifndef VISUALIZATION_H
#define VISUALIZATION_H

#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <QMouseEvent>

#include <fftw3.h>

#include "simulation.h"


class Visualization : public QOpenGLWidget
{
    Q_OBJECT
public:
    explicit Visualization(QWidget *parent = nullptr);
    void rainbow(float value,float* R,float* G,float* B);
    void set_colormap(float vy);
    void direction_to_color(float x, float y, int method);
    void visualize(void);
    void display(void);
    void reshape(int w, int h);
    void keyboard(unsigned char key, int x, int y);



    Simulation* get_simulation();
    void set_draw_vectors(int state);
    void set_vector_colors(int state);
    void set_vector_scale(int scale);
    void set_draw_smoke(int state);
    void set_smoke_colors(int mode);
    int toggle_frozen();
    void set_hue(float new_hue);
    void set_saturation(float new_saturation);
    void set_number_colors(int value);
    void correctColor(float *R, float *G, float *B);
public slots:
    void do_one_simulation_step();

protected:
    void initializeGL();
    void paintGL();

    void mouseMoveEvent(QMouseEvent *event);

    void paintSmoke(float wn, float hn);
    void paintVectors(float wn, float hn);
    void paintLegend(float wn, float hn);
private:
    Simulation* simulation;
    float hue = 0.0;
    float saturation = 1.0;
    int nlevels = 256;

    //--- VISUALIZATION PARAMETERS ---------------------------------------------------------------------
    int   color_dir = 0;            //use direction color-coding or not
    float vec_scale = 1000;			//scaling of hedgehogs
    int   draw_smoke = 1;           //draw the smoke or not
    int   draw_vecs = 0;            //draw the vector field or not
    int   draw_scale = 1;
    const int COLOR_BLACKWHITE=0;   //different types of color mapping: black-and-white, rainbow, banded
    const int COLOR_RAINBOW=1;
    const int COLOR_BANDS=2;
    const int COLOR_WHITETORED=3;
    int   scalar_col = 1;           //method for scalar coloring
    int   frozen = 0;               //toggles on/off the animation
};

#endif // VISUALIZATION_H
