#include <stdio.h>              //for printing the help text
#include <stdlib.h>
#include <math.h>               //for various math functions
#include "visualization.h"
#include <QDebug>
#include <QColor>
#include <QPainter>

#include <QTimer>
#include <QVector3D>
#include <QQuaternion>

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
    float new_hue = fmod(abs(color.hslHueF() + hue), 1.0);
    color.setHslF(new_hue,saturation,color.lightnessF());
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

float Visualization::get_color_max()
{
    float data_max = simulation->get_rho_maxf();
    if (scale_colors)
        return min(data_max, clipping_max);
    return clipping_max;
}

float Visualization::get_color_min()
{
    float data_min = simulation->get_rho_minf();
    if (scale_colors)
        return max(data_min, clipping_min);
    return clipping_min;
}


//set_colormap: Sets three different types of colormaps
void Visualization::set_colormap(float vy)
{
    if (vy < clipping_min)
        vy = clipping_min;
    else if (vy > clipping_max)
        vy = clipping_max;

    vy = normalize(vy, get_color_min(), get_color_max());

    // 0.0 <= vy <= 1.0 should now hold. Make sure to fix this if not.

    if (vy < 0.0)
        vy = 0.0;

    if (vy > 1.0)
        vy = 1.0;

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

void Visualization::draw_hedgehogs(QVector2D data, float wn, float hn, float i, float j)
{
    glBegin(GL_LINES);
    float x1 = wn + i * wn;
    float y1 = hn + j * hn;
    float x2 = (wn + i * wn) + vec_scale * data.x();
    float y2 = (hn + j * hn) + vec_scale * data.y();
    //The line below has all control over the moving of the glyphs
    glVertex2f(x1,y1);
    //The line below has all control over the placement of the glyphs
    glVertex2f(x2,y2);
    glEnd();
}

void draw_cone(QVector3D p1, QVector3D p2, float r, int n)
{
    glEnable(GL_DEPTH_TEST);
    QVector3D d = p2-p1;
    d.normalize();

    QQuaternion rotation = QQuaternion::fromDirection(d, QVector3D(0,0,1));

    QVector<QVector3D> points;
    // calculate cone bottom circl
    float t = 2 * static_cast<float>(M_PI) / static_cast<float>(n);
    for (int i = 0; i < n; i++) {
        QVector3D point = QVector3D(r * cos(t * i), -r * sin(t*i), 0);
        point = rotation * point;
        point += p1;
        points.push_back(point);
    }

    // draw cone (bottomless)
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_TRIANGLES);
    for (int i = 1; i <= n; ++i) {
        QVector3D norm = QVector3D::crossProduct(points[i%n] - p2, points[i-1] - points[i%n]);
        norm.normalize();
        glNormal3f(norm.x(), norm.y(), norm.z());
        glVertex3f(p2.x(), p2.y(), p2.z());
        glNormal3f(norm.x(), norm.y(), norm.z());
        glVertex3f(points[i%n].x(), points[i%n].y(), points[i%n].z());
        glNormal3f(norm.x(), norm.y(), norm.z());
        glVertex3f(points[i-1].x(), points[i-1].y(), points[i-1].z());
    }
    glEnd();

    glDisable(GL_DEPTH_TEST);
    glNormal3f(0, 0, 1);
}

void Visualization::draw_cones(QVector2D data, float wn, float hn, float i, float j)
{
    QVector3D p1 = QVector3D(wn + i * wn, hn + j * hn, 0);
    QVector3D p2 = QVector3D((wn + i * wn) + vec_scale * data.x(), (hn + j * hn) + vec_scale * data.y(), 0);
    draw_cone(p1, p2, p1.distanceToPoint(p2) / 3.0f, 10);
}

void Visualization::draw_arrows(QVector2D data, float wn, float hn, float i, float j)
{
    //TODO: implement the function
    printf("draw_arrows() is not yet implemented!\n");
}


void Visualization::paintVectors(float wn, float hn)
{
    int idx;
    QVector2D vector;
    // This draws a glyph for every raster point in the set dimension (standard is 50)
    // We will need to alter this so it's adjustable
    for (int i = 0; i < simulation->get_dim(); i++) {
        for (int j = 0; j < simulation->get_dim(); j++) {
            idx = (j * simulation->get_dim()) + i;
            if (vectorField) // force field f
                vector = QVector2D(simulation->get_fxf(idx), simulation->get_fyf(idx));
            else // fluid velocity v
                vector = QVector2D(simulation->get_vxf(idx), simulation->get_vyf(idx));

            //The line below has all control over the color of the glyph
            direction_to_color(vector.x(), vector.y(), color_dir);

            switch(shape) {
                case 0: draw_hedgehogs(vector, wn, hn, i, j); break;
                case 1: draw_cones(vector, wn, hn, i, j); break;
                case 2: draw_arrows(vector, wn, hn, i, j); break;
            }
        }
    }
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

    glColor3f(1.0, 1.0, 1.0);
    glVertex2f(wn-5, voffset - 5);
    glVertex2f(wn+55, voffset - 5);
    glVertex2f(wn+55, voffset + hn + 5);

    glVertex2f(wn-5, voffset - 5);
    glVertex2f(wn-5, voffset + hn + 5);
    glVertex2f(wn+55, voffset + hn + 5);

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


    QPainter painter(this);
    painter.drawText(wn + 25, voffset + hn, QString::number(get_color_min(), 'f', 2));
    painter.drawText(wn + 25, voffset + 10, QString::number(get_color_max(), 'f', 2));
    painter.end();
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
    glOrtho(0.0, static_cast<GLdouble>(width()), 0.0, static_cast<GLdouble>(height()), -20, 20);



    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
//    float light[4] = {0,0,0,-1};
//    glLightfv(GL_LIGHT0, GL_AMBIENT, light);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);


//    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
//    GLfloat mat_shininess[] = { 100.0 };
//    GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
//    GLfloat mat_diffuse_back[] = { 0.0, 0.0, 1.0, 1.0 };
//    GLfloat light_position[] = { 0.0, 0.0, 1.0, 0.0 };
//    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
//    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
//    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

//    glColorMaterial(GL_FRONT_AND_BACK,GL_DIFFUSE);

//    glEnable(GL_LIGHTING);
//    glEnable(GL_LIGHT0);
//    /*glPixelStorei(GL_PACK_ROW_LENGTH, 0);
//    glPixelStorei(GL_PACK_ALIGNMENT, 1);*/
//    glEnable(GL_NORMALIZE);
//    glEnable(GL_POINT_SMOOTH);
//    glShadeModel (GL_SMOOTH);
//    glEnable(GL_DEPTH_TEST);
//    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
//    glMaterialfv ( GL_FRONT_AND_BACK , GL_AMBIENT_AND_DIFFUSE , mat_diffuse );
//    glDepthMask(GL_TRUE);


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

void Visualization::set_glyph_x_amount(int value)
{
    glyphXAmount = value;
}

void Visualization::set_glyph_y_amount(int value)
{
    glyphYAmount = value;
}

void Visualization::set_shape(int option)
{
    shape = option;
}

void Visualization::set_vector_field(int option)
{
    vectorField = option;
}

void Visualization::set_color_based_on(int option)
{
    colorBasedOn = option;
}


void Visualization::set_scale_colors(int value)
{
    scale_colors = value;
}
