#include "visualization.h"
#include <QDebug>

Visualization::Visualization(QWidget *parent) : QOpenGLWidget(parent)
{
    qDebug() << "Constructor of GLWidget";
}

void Visualization::initializeGL()
{
    glClearColor(1.0,0.0,1.0,1.0);
}
