#ifndef VISUALIZATION_H
#define VISUALIZATION_H

#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <QMouseEvent>
#include <bitset>
#include <fftw3.h>

#include "cone.h"
#include "cylinder.h"
#include "simulation.h"


class Visualization : public QOpenGLWidget
{
    Q_OBJECT
public:
    explicit Visualization(QWidget *parent = nullptr);
    void reset_simulation();
    void reset();
    void rainbow(float value,float* R,float* G,float* B);
    void set_colormap(float vy);
    void direction_to_color(float x, float y);
    void visualize(void);
    void display(void);
    void reshape(int w, int h);
    void keyboard(unsigned char key, int x, int y);



    Simulation* get_simulation();
    void set_draw_vectors(int state);
    void set_vector_colors(int state);
    void set_vector_scale(int scale);
    void set_vector_jitter(int state);
    void set_draw_smoke(int state);
    void set_smoke_colors(int mode);
    int toggle_frozen();
    void set_hue(float new_hue);
    void set_saturation(float new_saturation);
    void set_number_colors(int value);
    void correctColor(float *R, float *G, float *B);

    void set_scaled_colormap(float vy);
    void set_clipping_max(float value);
    void set_clipping_min(float value);

    void set_glyph_x_amount(int value);
    void set_glyph_y_amount(int value);
    void set_shape(int option);
    void set_vector_field(int option);
    void set_color_based_on(int option);
    void set_scale_colors(int value);
    void set_smoke_mode(int mode);
    void set_isolines(int state);
    void set_threshold(int value);
    void set_number_isolines(int value);
    void set_min_rho(int value);
    void set_max_rho(int value);
    void set_heightplot(int state);
    void set_heightplot_scale(int value);
    void set_rotation(int value);
    void set_isoline_color(int state);
    void set_draw_scale(int state);

    float get_color_max();
    float get_color_min();

    int get_frozen();
public slots:
    void do_one_simulation_step();

protected:
    void initializeGL();
    void paintGL();

    void mouseMoveEvent(QMouseEvent *event);

    void paintSmoke(float wn, float hn);
    void paintVectors(float wn, float hn);
    void paintLegend(float wn, float hn);
    void draw_arrows(QVector2D data, float wn, float hn, float i, float j);
    void draw_circles(QVector2D data, float wn, float hn, float i, float j);
    void draw_cones(QVector2D data, float wn, float hn, float i, float j);
    void draw_hedgehogs(QVector2D data, float wn, float hn, float i, float j);
    QVector2D interpolateData(float adj_i, float adj_j);
    //QVector2D calcDatapoint(int i, int j, float adj_i, float adj_j);
    float get_scalar(int idx);
    void paintIsolines(float threshold, float wn, float hn);
private:
    Simulation* simulation;
    Cone cone = Cone(8, 1/3.0f);
    Cylinder cylinder = Cylinder(8, 1/3.0f);
    float hue = 0.0;
    float saturation = 1.0;
    int nlevels = 256;
    int glyphXAmount = 50;
    int glyphYAmount = 50;
    int shape = 0;
    int vectorField = 0;
    int colorBasedOn = 0;
    int smokeMode = 0;
    int jitter_seed = 0;

    //--- VISUALIZATION PARAMETERS ---------------------------------------------------------------------
    int   color_dir = 1;            //use direction color-coding or not
    float vec_scale = 1000;			//scaling of hedgehogs
    int   vec_jitter = 1;
    int   draw_smoke = 1;           //draw the smoke or not
    int   draw_vecs = 1;            //draw the vector field or not
    int   draw_scale = 1;
    int   draw_isolines = 0;
    const int COLOR_BLACKWHITE=0;   //different types of color mapping: black-and-white, rainbow, banded
    const int COLOR_RAINBOW=1;
    const int COLOR_BANDS=2;
    const int COLOR_WHITETORED=3;
    int   scalar_col = 0;           //method for scalar coloring
    int   frozen = 0;               //toggles on/off the animation
    int   scale_colors = 1;
    float clipping_min = 0;
    float clipping_max = 1;
    float threshold = 0.1;
    int numberIsolines = 1;
    float minRho = 0.0;
    float maxRho = 1.0;
    int height_plot=0;
    int height_plot_scale = 50;
    int rotation = 0;
    int isolineColor = 0;
    float calcOffset(float threshold, float value1, float value2);
};

#endif // VISUALIZATION_H
