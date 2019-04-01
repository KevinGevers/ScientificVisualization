#include <stdio.h>              //for printing the help text
#include <stdlib.h>
#include <math.h>               //for various math functions
#include "visualization.h"
#include <QDebug>
#include <QColor>
#include <QPainter>
#include <QVector3D>
#include <QVector2D>
#include <bitset>
#include <QTimer>
#include <QVector3D>
#include <QQuaternion>

using namespace std;

Visualization::Visualization(QWidget *parent) : QOpenGLWidget(parent)
{
    qDebug() << "Constructor of Visualization";
    simulation = new Simulation(50);
    QTimer *timer = new QTimer;
    timer->start();
    QObject::connect(timer,SIGNAL(timeout()),this,SLOT(do_one_simulation_step()));
}


void Visualization::do_one_simulation_step()
{
    if(!frozen){
        simulation->do_one_simulation_step();
        if (smokeMode)
        {
            simulation->calc_divergence(smokeMode);
        }
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
//                    the color mapping method 'color_dir'. If color_dir==1, map the vector direction
//                    using a rainbow colormap. If color_dir==0, simply use the white color
void Visualization::direction_to_color(float x, float y)
{
    float r,g,b,f;
    if (color_dir)
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

    glVertex2f(x1,y1);
    glVertex2f(x2,y2);
    glEnd();
}

void draw_circle(QVector2D p1, float rx, float ry, int n)
{
    float t = 2 * static_cast<float>(M_PI) / static_cast<float>(n);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < n; i++) {
        QVector2D point = p1 + QVector2D(rx * cos(t * i), -ry * sin(t*i));
        glVertex2f(point.x(),point.y());
    }
    glEnd();
}

void Visualization::draw_circles(QVector2D data, float wn, float hn, float i, float j)
{
    QVector2D p1 = QVector2D(wn + i * wn, hn + j * hn);
    draw_circle(p1, vec_scale * data.x(), vec_scale * data.y(), 10);
}


void Visualization::draw_cones(QVector2D data, float wn, float hn, float i, float j)
{
    QVector3D p1 = QVector3D(wn + i * wn, hn + j * hn, 0);
    QVector3D p2 = QVector3D((wn + i * wn) + vec_scale * data.x(), (hn + j * hn) + vec_scale * data.y(), 0);
//    draw_cone(p1, p2, p1.distanceToPoint(p2) / 3.0f, 10);
    cone.draw(p1, p2);
}


void Visualization::draw_arrows(QVector2D data, float wn, float hn, float i, float j)
{
    QVector3D p1 = QVector3D(wn + i * wn - vec_scale * data.x() / 2.0f, hn + j * hn - vec_scale * data.y() / 2.0f, 0);
    QVector3D p2 = QVector3D((wn + i * wn) + vec_scale * data.x() / 2.0f, (hn + j * hn) + vec_scale * data.y() / 2.0f, 0);

    cone.draw(p1 + (p2 - p1) / 3, p2);
    cylinder.draw(p1, p1 + (p2 - p1) / 3);
}

QVector2D Visualization::interpolateData(float adj_i, float adj_j)
{
    int dim = simulation->get_dim();
    QVector2D point1, point2, point3, point4, target, point1Data, point2Data, point3Data, point4Data;

    int idx_i1 = floor(adj_i);
    int idx_i2 = ceil(adj_i);
    int idx_j1 = floor(adj_j);
    int idx_j2 = ceil(adj_j);

    if (idx_i1 == idx_i2)
        idx_i2++;

    if (idx_j1 == idx_j2)
        idx_j2++;

    point1 = QVector2D(idx_i1, idx_j2); //Top Left
    point2 = QVector2D(idx_i2, idx_j2); //Top Right
    point3 = QVector2D(idx_i1, idx_j1); //Bottom Left
    point4 = QVector2D(idx_i2, idx_j1); //Bottom Right
    target = QVector2D(adj_i, adj_j); //Target point


    if (vectorField) // force field f
    {
        point1Data = QVector2D(simulation->get_fxf((point1.y() * dim) + point1.x()), simulation->get_fyf((point1.y() * dim) + point1.x()));
        point2Data = QVector2D(simulation->get_fxf((point2.y() * dim) + point2.x()), simulation->get_fyf((point2.y() * dim) + point2.x()));
        point3Data = QVector2D(simulation->get_fxf((point3.y() * dim) + point3.x()), simulation->get_fyf((point3.y() * dim) + point3.x()));
        point4Data = QVector2D(simulation->get_fxf((point4.y() * dim) + point4.x()), simulation->get_fyf((point4.y() * dim) + point4.x()));
    } else { // fluid velocity v
        point1Data = QVector2D(simulation->get_vxf((point1.y() * dim) + point1.x()), simulation->get_vyf((point1.y() * dim) + point1.x()));
        point2Data = QVector2D(simulation->get_vxf((point2.y() * dim) + point2.x()), simulation->get_vyf((point2.y() * dim) + point2.x()));
        point3Data = QVector2D(simulation->get_vxf((point3.y() * dim) + point3.x()), simulation->get_vyf((point3.y() * dim) + point3.x()));
        point4Data = QVector2D(simulation->get_vxf((point4.y() * dim) + point4.x()), simulation->get_vyf((point4.y() * dim) + point4.x()));
    }
    float xDistTop, yDistTop, xDistBottom, yDistBottom;
    xDistTop = point2.x() - target.x();
    yDistTop = point1.y() - target.y();
    xDistBottom = target.x() - point3.x();
    yDistBottom = target.y() - point3.y();
    float solutionX = (
                point3Data.x() * xDistTop * yDistTop +
                point4Data.x() * xDistBottom * yDistTop +
                point1Data.x() * xDistTop * yDistBottom +
                point2Data.x() * xDistBottom * yDistBottom
                );
    float solutionY = (
                point3Data.y() * xDistTop * yDistTop +
                point4Data.y() * xDistBottom * yDistTop +
                point1Data.y() * xDistTop * yDistBottom +
                point2Data.y() * xDistBottom * yDistBottom
                );
    return QVector2D(solutionX, solutionY);
}

void Visualization::paintVectors(float wn, float hn)
{
    float adj_i, adj_j;
    QVector2D data;
    int dim = simulation->get_dim();
    srand(jitter_seed);
    // This draws a glyph for every raster point in the set dimension (standard is 50)
    // We will need to alter this so it's adjustable
    for (int i = 0; i < glyphXAmount; i++)
    {
        for (int j = 0; j < glyphYAmount; j++)
        {
            adj_i = i/(float)glyphXAmount * dim;
            adj_j = j/(float)glyphYAmount * dim;

            if (vec_jitter) {
                adj_i += ((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) - .5f) * dim / static_cast <float> (glyphXAmount);
                adj_j += ((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) - .5f) * dim / static_cast <float> (glyphYAmount);
                if (adj_i < 0 || adj_i > dim-1 || adj_j < 0 || adj_j > dim-1)
                    continue;
            }

            data = interpolateData(adj_i, adj_j);
            direction_to_color(data.x(), data.y());

            switch(shape) {
                case 0: draw_hedgehogs(data, wn, hn, adj_i, adj_j); break;
                case 1: draw_cones(data, wn, hn, adj_i, adj_j); break;
                case 2: draw_arrows(data, wn, hn, adj_i, adj_j); break;
                case 3: draw_circles(data, wn, hn, adj_i, adj_j); break;
            }
        }
    }
}

void Visualization::paintIsolines(float threshold, float wn, float hn)
{
    int dim = simulation->get_dim();
    std::bitset<4> marchingsq;
    float point1, point2, point3, point4;
    float x1, x2, y1, y2;

    glBegin(GL_LINES);
    for(int i=1; i<dim+1; i++) {
        for(int j=1; j<dim+1; j++) {
            marchingsq.reset();

            point1 = simulation->get_rhof((j-1) * dim + i-1);
            point2 = simulation->get_rhof((j-1) * dim + i);
            point3 = simulation->get_rhof(j * dim + i);
            point4 = simulation->get_rhof(j * dim + i-1);

            if(point1>threshold) marchingsq.flip(3);
            if(point2>threshold) marchingsq.flip(2);
            if(point3>threshold) marchingsq.flip(1);
            if(point4>threshold) marchingsq.flip(0);

            switch(marchingsq.to_ulong()) {
                case 0: break;
                case 1: x1 =  i*wn;
                        y1 = (j+0.5)*hn;
                        x2 = (i+0.5)*wn;
                        y2 = (j+1)*hn;
                break;
                case 2: x1 = (i+1)*wn;
                        y1 = (j+0.5)*hn;
                        x2 = (i+0.5)*wn;
                        y2 = (j+1)*hn;
                break;
                case 3: x1 =  i*wn;
                        y1 = (j+0.5)*hn;
                        x2 = (i+1)*wn;
                        y2 = (j+0.5)*hn;
                break;
                case 4: x1 = (i+1)*wn;
                        y1 = (j+0.5)*hn;
                        x2 = (i+0.5)*wn;
                        y2 = j*hn;
                break;
                case 5: //has 2 cases, so draw both
                        x1 = (i+1)*wn;
                        y1 = (j+0.5)*hn;
                        x2 = (i+0.5)*wn;
                        y2 = (j+1)*hn;
                        glVertex2f(x1-1, y1-1);
                        glVertex2f(x2-1, y2-1);

                        x1 = i*wn;
                        y1 = (j+0.5)*hn;
                        x2 = (i+0.5)*wn;
                        y2 = j*hn;
                break;
                case 6: x1 = (i+0.5)*wn;
                        y1 = j*hn;
                        x2 = (i+0.5)*wn;
                        y2 = (j+1)*hn;
                break;
                case 7: x1 = i*wn;
                        y1 = (j+0.5)*hn;
                        x2 = (i+0.5)*wn;
                        y2 = j*hn;
                break;
                case 8: x1 = i*wn;
                        y1 = (j+0.5)*hn;
                        x2 = (i+0.5)*wn;
                        y2 = j*hn;
                break;
                case 9: x1 = (i+0.5)*wn;
                        y1 = j*hn;
                        x2 = (i+0.5)*wn;
                        y2 = (j+1)*hn;
                break;
                case 10: //has 2 cases, so draw both
                         x1 = (i+1)*wn;
                         y1 = (j+0.5)*hn;
                         x2 = (i+0.5)*wn;
                         y2 = (j)*hn;
                         glVertex2f(x1-1, y1-1);
                         glVertex2f(x2-1, y2-1);

                         x1 = i*wn;
                         y1 = (j+0.5)*hn;
                         x2 = (i+0.5)*wn;
                         y2 = (j+1)*hn;
                break;
                case 11: x1 = (i+1)*wn;
                         y1 = (j+0.5)*hn;
                         x2 = (i+0.5)*wn;
                         y2 = j*hn;
                break;
                case 12: x1 = i*wn;
                         y1 = (j+0.5)*hn;
                         x2 = (i+1)*wn;
                         y2 = (j+0.5)*hn;
                break;
                case 13: x1 = (i+1)*wn;
                         y1 = (j+0.5)*hn;
                         x2 = (i+0.5)*wn;
                         y2 = (j+1)*hn;
                break;
                case 14: x1 = i*wn;
                         y1 = (j+0.5)*hn;
                         x2 = (i+0.5)*wn;
                         y2 = (j+1)*hn;
                break;
                case 15: break;
            }
            glLineWidth(1.0);
            set_scaled_colormap(threshold);
            glVertex2f(x1, y1);
            glVertex2f(x2, y2);

        }
    }
    glEnd();
}

float Visualization:: get_scalar(int idx){
    if (smokeMode)
    { // divergence
        // the results here are not completely correct due to scaling and clipping!
        return simulation->get_divergence(idx);
    } else { // density
        return simulation->get_rhof(idx);
    }
}


void Visualization::paintSmoke(float wn, float hn)
{
    int idx0, idx1, idx2, idx3;
    float px0, py0, px1, py1, px2, py2, px3, py3;

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

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

            if (height_plot) {
                glBegin(GL_QUADS);
                set_colormap(simulation->get_rhof(idx0));    glVertex3f(px0, py0, simulation->get_rhof(idx0));
                set_colormap(simulation->get_rhof(idx1));    glVertex3f(px1, py1, simulation->get_rhof(idx1));
                set_colormap(simulation->get_rhof(idx2));    glVertex3f(px2, py2, simulation->get_rhof(idx2));
                set_colormap(simulation->get_rhof(idx3));    glVertex3f(px3, py3, simulation->get_rhof(idx3));
            } else {
                glBegin(GL_TRIANGLES);
                set_colormap(get_scalar(idx0));    glVertex2f(px0, py0);
                set_colormap(get_scalar(idx1));    glVertex2f(px1, py1);
                set_colormap(get_scalar(idx2));    glVertex2f(px2, py2);

                set_colormap(get_scalar(idx0));    glVertex2f(px0, py0);
                set_colormap(get_scalar(idx2));    glVertex2f(px2, py2);
                set_colormap(get_scalar(idx3));    glVertex2f(px3, py3);
            }
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
    float wn, hn;
    if (height_plot) {
        wn = static_cast<float>(width()) / static_cast<float>(simulation->get_dim() + 1)/5*3;   // Grid cell width
        hn = static_cast<float>(height()) / static_cast<float>(simulation->get_dim() + 1)/5*3;  // Grid cell heigh
    } else {
        wn = static_cast<float>(width()) / static_cast<float>(simulation->get_dim() + 1);   // Grid cell width
        hn = static_cast<float>(height()) / static_cast<float>(simulation->get_dim() + 1);  // Grid cell heigh
    }

    if (draw_smoke)
        paintSmoke(wn, hn);

    if (draw_vecs)
        paintVectors(wn, hn);

    if (draw_scale && !height_plot)
        paintLegend(wn, hn);

    if (draw_isolines && numberIsolines > 1) {
        float stepsize = (maxRho-minRho)/(numberIsolines+1);
        for(int i=1; i<=numberIsolines; i++) {
            paintIsolines(minRho+(stepsize*i), wn, hn);
        }
    } else if(draw_isolines) {
        paintIsolines(threshold, wn, hn);
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

    if (height_plot) {
        glOrtho(width()/5*-1, width()/10*7, height()/5*-1, height()/10*7, -50, 50);
        glRotatef(325.0, 1.0, 0.0, 0.0);
        glRotatef(10.0, 0.0, 1.0, 0.0);
        glRotatef(325.0, 0.0, 0.0, 1.0);
    } else {
        glOrtho(0.0, static_cast<GLdouble>(width()), 0.0, static_cast<GLdouble>(height()), -50, 50);
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);

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
    jitter_seed = rand();
}

void Visualization::set_vector_colors(int state)
{
    color_dir = state;
}

void Visualization::set_vector_scale(int scale)
{
    vec_scale = scale;
}

void Visualization::set_vector_jitter(int state)
{
    vec_jitter = state;
    jitter_seed = rand();
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
    jitter_seed = rand();
}

void Visualization::set_glyph_y_amount(int value)
{
    glyphYAmount = value;
    jitter_seed = rand();
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

void Visualization::set_smoke_mode(int mode)
{
    smokeMode = mode;
}

void Visualization::set_isolines(int state)
{
    draw_isolines = state;
}

void Visualization::set_threshold(int value)
{
    threshold = (float)value/10;
}

void Visualization::set_number_isolines(int value)
{
    numberIsolines = value;
}

void Visualization::set_min_rho(int value)
{
    minRho = (float)value/10;
}

void Visualization::set_max_rho(int value)
{
    maxRho = (float)value/10;
}

void Visualization::set_heightplot(int state)
{
    height_plot = state;
}
