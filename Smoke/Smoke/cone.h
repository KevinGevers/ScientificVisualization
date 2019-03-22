#ifndef CONE_H
#define CONE_H

#include <QVector>
#include <QVector3D>

class Cone
{
public:
    Cone(int sections, float radius);
    void generate();
    void draw(QVector3D bottom, QVector3D top);
private:
    int sections = 10;
    float radius = 1.0;

    QVector<QVector3D> points;
    QVector<QVector3D> face_normals;
    QVector<QVector3D> vertex_normals;
};

#endif // CONE_H
