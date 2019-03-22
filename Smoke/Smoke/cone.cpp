#include "cone.h"

#include <math.h>

#include <QQuaternion>
#include <QOpenGLFunctions>

#include <QDebug>

Cone::Cone(int sections, float radius)
{
    this->sections = sections;
    this->radius = radius;
    generate();
}

void Cone::generate()
{
    QVector3D top = QVector3D(0,0,1);

    QVector<QVector3D> circle;
    float t = 2 * static_cast<float>(M_PI) / static_cast<float>(sections);
    for (int i = 0; i < sections; i++) {
        QVector3D point = QVector3D(radius * cos(t * i), -radius * sin(t*i), 0);
        circle.push_back(point);
    }

    for (int i = 1; i <= sections; ++i) {
        QVector3D norm = QVector3D::crossProduct(circle[i%sections] - top, circle[i-1] - circle[i%sections]);
        norm.normalize();
        face_normals.push_back(norm);
    }

    for (int i = 1; i <= sections; ++i) {
        vertex_normals.push_back(face_normals[i%sections]);
        vertex_normals.push_back(((face_normals[i%sections] + face_normals[(i+1)%sections]) / 2.0f).normalized());
        vertex_normals.push_back(((face_normals[i%sections] + face_normals[i-1]) / 2.0f).normalized());

        points.push_back(top);
        points.push_back(circle[i%sections]);
        points.push_back(circle[i-1]);
    }
}


void Cone::draw(QVector3D bottom, QVector3D top)
{
    QVector3D d = top - bottom;
    float scale = d.length();

    QQuaternion rotation = QQuaternion::fromDirection(d.normalized(), QVector3D(0,0,1));

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_TRIANGLES);

    for (int i = 0; i < points.length(); i+=3) {
        QVector3D n = rotation * vertex_normals[i];
        QVector3D p = (rotation * points[i] * scale) + bottom;
        glNormal3f(n.x(), n.y(), n.z());
        glVertex3f(p.x(), p.y(), p.z());

        n = rotation * vertex_normals[i+1];
        p = (rotation * points[i+1] * scale) + bottom;
        glNormal3f(n.x(), n.y(), n.z());
        glVertex3f(p.x(), p.y(), p.z());

        n = rotation * vertex_normals[i+2];
        p = (rotation * points[i+2] * scale) + bottom;
        glNormal3f(n.x(), n.y(), n.z());
        glVertex3f(p.x(), p.y(), p.z());
    }

    glEnd();
    glNormal3f(0, 0, 1);
}
