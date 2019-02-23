#ifndef VISUALIZATION_H
#define VISUALIZATION_H

#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>


class Visualization : public QOpenGLWidget
{
    Q_OBJECT
public:
    explicit Visualization(QWidget *parent = nullptr);
    void initializeGL();
    void rainbow(float value,float* R,float* G,float* B);
    void set_colormap(float vy);
    void direction_to_color(float x, float y, int method);
    void visualize(void);
    void display(void);
    void reshape(int w, int h);
    void keyboard(unsigned char key, int x, int y);
    void drag(int mx, int my);



    //--- VISUALIZATION PARAMETERS ---------------------------------------------------------------------
    int   winWidth, winHeight;      //size of the graphics window, in pixels
    int   color_dir = 0;            //use direction color-coding or not
    float vec_scale = 1000;			//scaling of hedgehogs
    int   draw_smoke = 0;           //draw the smoke or not
    int   draw_vecs = 1;            //draw the vector field or not
    const int COLOR_BLACKWHITE=0;   //different types of color mapping: black-and-white, rainbow, banded
    const int COLOR_RAINBOW=1;
    const int COLOR_BANDS=2;
    int   scalar_col = 0;           //method for scalar coloring
    int   frozen = 0;               //toggles on/off the animation



protected:

};

#endif // VISUALIZATION_H
