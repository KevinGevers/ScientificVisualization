#include <stdio.h>              //for printing the help text
#include <stdlib.h>
#include <math.h>               //for various math functions
#include "visualization.h"
#include <QDebug>
#include <QColor>

#include <QTimer>

using namespace std;

Visualization::Visualization(QWidget *parent) : QOpenGLWidget(parent)
{
    qDebug() << "Constructor of GLWidget";
    simulation = new Simulation(50);
    QTimer *timer = new QTimer;
    timer->start();
    QObject::connect(timer,SIGNAL(timeout()),this,SLOT(do_one_simulation_step()));
}


void Visualization::do_one_simulation_step()
{
    if(!frozen){
        simulation->do_one_simulation_step();
        update();
    }
}

//rainbow: Implements a color palette, mapping the scalar 'value' to a rainbow color RGB
void Visualization::rainbow(float value,float* R,float* G,float* B)
{
   const float dx=0.8f;
   if (value<0) value=0; if (value>1) value=1;
   value = (6-2*dx)*value+dx;
   *R = max(0.0f,(3-fabs(value-4)-fabs(value-5))/2);
   *G = max(0.0f,(4-fabs(value-2)-fabs(value-4))/2);
   *B = max(0.0f,(3-fabs(value-1)-fabs(value-2))/2);
}

//correctColor: converst the RGB values to HSV and adjusts for the set Hue and Saturation.
// Then changes it back to RGB values.
void Visualization::correctColor(float *R, float *G, float *B)
{
    QColor color = QColor::fromRgbF(*R, *G, *B);
    float new_hue = fmod(hue + color.hueF(), 1.0);
    color.setHsvF(new_hue,saturation,color.valueF());
    *R = color.redF();
    *G = color.greenF();
    *B = color.blueF();
}


void Visualization::set_scaled_colormap(float vy)
{
    float R,G,B;

    vy *= nlevels; vy = static_cast<int>(vy); vy/= nlevels;

    if (scalar_col==COLOR_BLACKWHITE) {
       R = G = B = vy;
       glColor3f(R,G,B);
       return;
    } else if (scalar_col==COLOR_RAINBOW)
       rainbow(vy,&R,&G,&B);
    else if (scalar_col==COLOR_WHITETORED) {
      R = 1.0f;
      G = B = 1.0f - vy;
    } else
       rainbow(vy,&R,&G,&B);

    correctColor(&R, &G, &B);
    glColor3f(R,G,B);
}

float normalize(float val, float min, float max)
{
    return (val - min) / (max - min);
}



//set_colormap: Sets three different types of colormaps
void Visualization::set_colormap(float vy)
{
    double max = simulation->get_rho_max();
    double min = simulation->get_rho_min();

    vy = normalize(vy, min, max);

    if (vy < clipping_min)
        vy = clipping_min;
    else if (vy > clipping_max)
        vy = clipping_max;

    vy = normalize(vy, clipping_min, clipping_max);
//    qDebug() << vy << max << min;
    set_scaled_colormap(vy);
}


//direction_to_color: Set the current color by mapping a direction vector (x,y), using
//                    the color mapping method 'method'. If method==1, map the vector direction
//                    using a rainbow colormap. If method==0, simply use the white color
void Visualization::direction_to_color(float x, float y, int method)
{
    float r,g,b,f;
    if (method)
    {
      f = atan2(y,x) / 3.1415927f + 1;
      r = f;
      if(r > 1) r = 2 - r;
      g = f + .66667f;
      if(g > 2) g -= 2;
      if(g > 1) g = 2 - g;
      b = f + 2 * .66667f;
      if(b > 2) b -= 2;
      if(b > 1) b = 2 - b;
    }
    else
    { r = g = b = 1; }
    glColor3f(r,g,b);
}

void Visualization::paintVectors(float wn, float hn)
{
    int idx;

    glBegin(GL_LINES);				//draw velocities
    for (int i = 0; i < simulation->get_dim(); i++)
      for (int j = 0; j < simulation->get_dim(); j++)
      {
        idx = (j * simulation->get_dim()) + i;
        direction_to_color(simulation->get_vxf(idx), simulation->get_vyf(idx), color_dir);
        glVertex2f(wn + i * wn, hn + j * hn);
        glVertex2f((wn + i * wn) + vec_scale * simulation->get_vxf(idx), (hn + j * hn) + vec_scale * simulation->get_vyf(idx));
      }
    glEnd();
}

void Visualization::paintSmoke(float wn, float hn)
{
    int idx0, idx1, idx2, idx3;
    float px0, py0, px1, py1, px2, py2, px3, py3;

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_TRIANGLES);
    for (int j = 0; j < simulation->get_dim() - 1; j++)            //draw smoke
    {
        for (int i = 0; i < simulation->get_dim() - 1; i++)
        {
            px0 = wn + i * wn;
            py0 = hn + j * hn;
            idx0 = (j * simulation->get_dim()) + i;


            px1 = wn + i * wn;
            py1 = hn + (j + 1) * hn;
            idx1 = ((j + 1) * simulation->get_dim()) + i;


            px2 = wn + (i + 1) * wn;
            py2 = hn + (j + 1) * hn;
            idx2 = ((j + 1) * simulation->get_dim()) + (i + 1);


            px3 = wn + (i + 1) * wn;
            py3 = hn + j * hn;
            idx3 = (j * simulation->get_dim()) + (i + 1);

            set_colormap(simulation->get_rhof(idx0));    glVertex2f(px0, py0);
            set_colormap(simulation->get_rhof(idx1));    glVertex2f(px1, py1);
            set_colormap(simulation->get_rhof(idx2));    glVertex2f(px2, py2);


            set_colormap(simulation->get_rhof(idx0));    glVertex2f(px0, py0);
            set_colormap(simulation->get_rhof(idx2));    glVertex2f(px2, py2);
            set_colormap(simulation->get_rhof(idx3));    glVertex2f(px3, py3);
        }
    }
    glEnd();
}

void Visualization::paintLegend(float wn, float hn)
{
    int res = 100;
    float voffset = height() * .125f;
    hn = height() * 0.75f;

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_TRIANGLES);

    for (int i=0; i < res; i++)
    {
        set_scaled_colormap(i / static_cast<float>(res));      glVertex2f(wn+0, voffset + i * (hn/res));
        set_scaled_colormap(i / static_cast<float>(res));      glVertex2f(wn+20, voffset + i * (hn/res));
        set_scaled_colormap((i+1) / static_cast<float>(res));  glVertex2f(wn+20, voffset + (i+1) * (hn/res));

        set_scaled_colormap(i / static_cast<float>(res));      glVertex2f(wn+0, voffset + i * (hn/res));
        set_scaled_colormap((i+1) / static_cast<float>(res));  glVertex2f(wn+0, voffset + (i+1) * (hn/res));
        set_scaled_colormap((i+1) / static_cast<float>(res));  glVertex2f(wn+20, voffset + (i+1) * (hn/res));
    }
    glEnd();

    glBegin(GL_LINE_LOOP);
    set_scaled_colormap(1.0);
    glVertex2f(wn + 0, voffset);
    glVertex2f(wn + 20, voffset);
    set_scaled_colormap(0.0);
    glVertex2f(wn + 20, voffset + hn);
    glVertex2f(wn + 0, voffset + hn);
    glEnd();
}

//visualize: This is the main visualization function
void Visualization::visualize()
{
    float  wn = static_cast<float>(width()) / static_cast<float>(simulation->get_dim() + 1);   // Grid cell width
    float  hn = static_cast<float>(height()) / static_cast<float>(simulation->get_dim() + 1);  // Grid cell heigh

    if (draw_smoke)
        paintSmoke(wn, hn);

    if (draw_vecs)
        paintVectors(wn, hn);

    if (draw_scale)
        paintLegend(wn, hn);
}


void Visualization::initializeGL()
{
    glClearColor(0.0,0.0,0.0,0.0);
}


void Visualization::paintGL()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, static_cast<GLdouble>(width()), 0.0, static_cast<GLdouble>(height()), -1, 1);
    visualize();
    glFlush();
}


void Visualization::mouseMoveEvent(QMouseEvent *event)
{
    simulation->drag(event->x(),event->y(), width(), height());
}


Simulation *Visualization::get_simulation()
{
    return simulation;
}

void Visualization::set_draw_vectors(int state)
{
    draw_vecs = state;
}

void Visualization::set_vector_colors(int state)
{
    color_dir = state;
}

void Visualization::set_vector_scale(int scale)
{
    vec_scale = scale;
}

void Visualization::set_draw_smoke(int state)
{
    draw_smoke = state;
}

void Visualization::set_smoke_colors(int mode)
{
    scalar_col = mode;
}

void Visualization::set_hue(float new_hue)
{
    hue = new_hue;
}

void Visualization::set_saturation(float new_saturation)
{
    saturation = new_saturation;
}

void Visualization::set_number_colors(int value)
{
    nlevels = value;
}

void Visualization::set_clipping_max(float value)
{
    clipping_max = value;
}

void Visualization::set_clipping_min(float value)
{
    clipping_min = value;
}

int Visualization::toggle_frozen()
{
    frozen = 1 - frozen;
    return frozen;
}
