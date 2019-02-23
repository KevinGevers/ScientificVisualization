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
    simulation->drag(10,10, width(), height());
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
   else if (scalar_col==COLOR_BANDS)
       {
          const int NLEVELS = 7;
          vy *= NLEVELS; vy = static_cast<int>(vy); vy/= NLEVELS;
          rainbow(vy,&R,&G,&B);
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
void Visualization::visualize(void)
{
    qDebug() << "visualize";
    int        i, j, idx; double px,py;
    double  wn = static_cast<double>(width()) / static_cast<double>(simulation->get_dim() + 1);   // Grid cell width
    double  hn = static_cast<double>(height()) / static_cast<double>(simulation->get_dim() + 1);  // Grid cell heigh

    if (draw_smoke)
    {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    for (j = 0; j < simulation->get_dim() - 1; j++)			//draw smoke
    {
//        glBegin(GL_TRIANGLE_STRIP);
        glBegin(GL_TRIANGLES);

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
//    glViewport(0.0f, 0.0f, width(), height());
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, (GLdouble)width(), 0.0, (GLdouble)height(), -1, 1);


//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    glMatrixMode(GL_MODELVIEW);
//    glLoadIdentity();
    visualize();
    glFlush();
}


//void Visualization::paintGL()
//{
//        glMatrixMode(GL_PROJECTION);
//        glLoadIdentity();
//        glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    glMatrixMode(GL_MODELVIEW);
//    glLoadIdentity();

//    glShadeModel(GL_SMOOTH);
//    glEnable(GL_LIGHTING);
//    glEnable(GL_LIGHT0);
//    float light[4] = {1,1,1,0};
//    glLightfv(GL_LIGHT0, GL_POSITION, light);
//    glEnable(GL_COLOR_MATERIAL);
//    visualize();
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
//    glFlush();
//}

//------ INTERACTION CODE STARTS HERE -----------------------------------------------------------------

/*display: Handle window redrawing events. Simply delegates to visualize().
void Visualization::display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    visualize();
    glFlush();
//    glutSwapBuffers();
}

//reshape: Handle window resizing (reshaping) events
void Visualization::reshape(int w, int h)
{
    glViewport(0.0f, 0.0f, (GLfloat)w, (GLfloat)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
//    gluOrtho2D(0.0, (GLdouble)w, 0.0, (GLdouble)h);
    glOrtho(0.0, (GLdouble)w, 0.0, (GLdouble)h, -1, 1);
    winWidth = w; winHeight = h;
}

//keyboard: Handle key presses
void Visualization::keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
      case 't': dt -= 0.001; break;
      case 'T': dt += 0.001; break;
      case 'c': color_dir = 1 - color_dir; break;
      case 'S': vec_scale *= 1.2; break;
      case 's': vec_scale *= 0.8; break;
      case 'V': visc *= 5; break;
      case 'v': visc *= 0.2; break;
      case 'x': draw_smoke = 1 - draw_smoke;
            if (draw_smoke==0) draw_vecs = 1; break;
      case 'y': draw_vecs = 1 - draw_vecs;
            if (draw_vecs==0) draw_smoke = 1; break;
      case 'm': scalar_col++; if (scalar_col>COLOR_BANDS) scalar_col=COLOR_BLACKWHITE; break;
      case 'a': frozen = 1-frozen; break;
      case 'q': exit(0);
    }
}


*/
// drag: When the user drags with the mouse, add a force that corresponds to the direction of the mouse
//       cursor movement. Also inject some new matter into the field at the mouse location.


void Visualization::mouseMoveEvent(QMouseEvent *event)
{
    float mx = event->localPos().x();
    float my = event->localPos().y();
    simulation->drag(mx,my, width(), height());
}
