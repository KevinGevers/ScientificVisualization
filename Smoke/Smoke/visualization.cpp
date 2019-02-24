#include <stdio.h>              //for printing the help text
#include <stdlib.h>
#include <math.h>               //for various math functions
#include <math.h>
#include "visualization.h"
#include <QDebug>

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

//set_colormap: Sets three different types of colormaps
void Visualization::set_colormap(float vy)
{
   float R,G,B;

   if (scalar_col==COLOR_BLACKWHITE)
       R = G = B = vy;
   else if (scalar_col==COLOR_RAINBOW)
       rainbow(vy,&R,&G,&B);
   else if (scalar_col==COLOR_BANDS) {
      const int NLEVELS = 7;
      vy *= NLEVELS; vy = static_cast<int>(vy); vy/= NLEVELS;
      rainbow(vy,&R,&G,&B);
   }
   else if (scalar_col==COLOR_WHITETORED) {
      R = 1.0f;
      G = B = 1.0f - vy;
   }
   else
       rainbow(vy,&R,&G,&B);

   glColor3f(R,G,B);
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

//visualize: This is the main visualization function
void Visualization::visualize()
{
    int        i, j, idx; double px,py;
    double  wn = static_cast<double>(width()) / static_cast<double>(simulation->get_dim() + 1);   // Grid cell width
    double  hn = static_cast<double>(height()) / static_cast<double>(simulation->get_dim() + 1);  // Grid cell heigh

    if (draw_smoke)
    {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    for (j = 0; j < simulation->get_dim() - 1; j++)			//draw smoke
    {
        glBegin(GL_TRIANGLE_STRIP);

        i = 0;
        px = wn + static_cast<double>(i) * wn;
        py = hn + static_cast<double>(j) * hn;
        idx = (j * simulation->get_dim()) + i;
        glColor3f(simulation->get_rho(idx), simulation->get_rho(idx), simulation->get_rho(idx));
        glVertex2f(px,py);

        for (i = 0; i < simulation->get_dim() - 1; i++)
        {
            px = wn + static_cast<double>(i) * wn;
            py = hn + static_cast<double>(j + 1) * hn;
            idx = ((j + 1) * simulation->get_dim()) + i;
            set_colormap(simulation->get_rho(idx));
            glVertex2f(px, py);
            px = wn + static_cast<double>(i + 1) * wn;
            py = hn + static_cast<double>(j) * hn;
            idx = (j * simulation->get_dim()) + (i + 1);
            set_colormap(simulation->get_rho(idx));
            glVertex2f(px, py);
        }

        px = wn + static_cast<double>(simulation->get_dim() - 1) * wn;
        py = hn + static_cast<double>(j + 1) * hn;
        idx = ((j + 1) * simulation->get_dim()) + (simulation->get_dim() - 1);
        set_colormap(simulation->get_rho(idx));
        glVertex2f(px, py);
        glEnd();
    }
    }

    if (draw_vecs)
    {
      glBegin(GL_LINES);				//draw velocities
      for (i = 0; i < simulation->get_dim(); i++)
        for (j = 0; j < simulation->get_dim(); j++)
        {
          idx = (j * simulation->get_dim()) + i;
          direction_to_color(simulation->get_vx(idx), simulation->get_vy(idx), color_dir);
          glVertex2f(wn + static_cast<double>(i) * wn, hn + static_cast<double>(j) * hn);
          glVertex2f((wn + static_cast<double>(i) * wn) + vec_scale * simulation->get_vx(idx), (hn + static_cast<double>(j) * hn) + vec_scale * simulation->get_vy(idx));
        }
      glEnd();
    }
}


void Visualization::initializeGL()
{
    glClearColor(0.0,0.0,0.0,0.0);
}


void Visualization::paintGL()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, (GLdouble)width(), 0.0, (GLdouble)height(), -1, 1);
    visualize();
    glFlush();
}


void Visualization::mouseMoveEvent(QMouseEvent *event)
{
    float mx = event->localPos().x();
    float my = event->localPos().y();
    simulation->drag(mx,my, width(), height());
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

int Visualization::toggle_frozen()
{
    frozen = 1 - frozen;
    return frozen;
}
