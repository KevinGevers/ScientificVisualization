#include "cylinder.h"

#include <math.h>

#include <QQuaternion>
#include <QOpenGLFunctions>

#include <QDebug>

Cylinder::Cylinder(int sections, float radius)
{
    this->sections = sections;
    this->radius = radius;
    generate();
}

void Cylinder::generate()
{
    QVector3D top = QVector3D(0,0,1);

    QVector<QVector3D> circle;
    float t = 2 * static_cast<float>(M_PI) / static_cast<float>(sections);
    for (int i = 0; i < sections; i++) {
        QVector3D point = QVector3D(radius * cos(t * i), -radius * sin(t*i), 0);
        circle.push_back(point);
    }

    for (int i = 1; i <= sections; ++i) {
        // p1 = circle[i-1]
        // p2 = circle[i%sections]
        // p3 = circle[i-1] + top
        // p4 = circle[i%sections] + top

        // norm(p1,p2,p3) = p2 - p3, p1 - p2
        // End faces are not twisted, so normals are equal
        QVector3D norm = QVector3D::crossProduct(circle[i%sections] - (circle[i-1] + top), circle[i-1] - circle[i%sections]);
        norm.normalize();
        face_normals.push_back(norm);
    }

    for (int i = 1; i <= sections; ++i) {
        vertex_normals.push_back(((face_normals[i%sections] + face_normals[i-1]) / 2.0f).normalized());
        vertex_normals.push_back(((face_normals[i%sections] + face_normals[(i+1)%sections]) / 2.0f).normalized());
        vertex_normals.push_back(((face_normals[i%sections] + face_normals[i-1]) / 2.0f).normalized());
        vertex_normals.push_back(((face_normals[i%sections] + face_normals[(i+1)%sections]) / 2.0f).normalized());

        points.push_back(circle[i-1]);
        points.push_back(circle[i%sections]);
        points.push_back(circle[i-1] + top);
        points.push_back(circle[i%sections] + top);
    }
}


void Cylinder::draw(QVector3D bottom, QVector3D top)
{
    QVector3D d = top - bottom;
    float scale = d.length();

    QQuaternion rotation = QQuaternion::fromDirection(d.normalized(), QVector3D(0,0,1));

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_TRIANGLES);

    QVector3D n1, n2, n3, n4, p1, p2, p3, p4;
    for (int i = 0; i < points.length(); i+=4) {
        n1 = rotation * vertex_normals[i];
        n2 = rotation * vertex_normals[i+1];
        n3 = rotation * vertex_normals[i+2];
        n4 = rotation * vertex_normals[i+3];

        p1 = (rotation * points[i] * scale) + bottom;
        p2 = (rotation * points[i+1] * scale) + bottom;
        p3 = (rotation * points[i+2] * scale) + bottom;
        p4 = (rotation * points[i+3] * scale) + bottom;

        glNormal3f(n1.x(), n1.y(), n1.z());
        glVertex3f(p1.x(), p1.y(), p1.z());

        glNormal3f(n2.x(), n2.y(), n2.z());
        glVertex3f(p2.x(), p2.y(), p2.z());

        glNormal3f(n3.x(), n3.y(), n3.z());
        glVertex3f(p3.x(), p3.y(), p3.z());


        glNormal3f(n2.x(), n2.y(), n2.z());
        glVertex3f(p2.x(), p2.y(), p2.z());

        glNormal3f(n3.x(), n3.y(), n3.z());
        glVertex3f(p3.x(), p3.y(), p3.z());

        glNormal3f(n4.x(), n4.y(), n4.z());
        glVertex3f(p4.x(), p4.y(), p4.z());
    }

    glEnd();
    glNormal3f(0, 0, 1);
}



//for (int i = 1; i <= n; ++i) {
//    QVector3D p1 = points[i-1] + point_1;
//    QVector3D p2 = points[i%n] + point_1;
//    QVector3D p3 = points[i-1] + point_2;
//    QVector3D p4 = points[i%n] + point_2;

//    QVector3D norm = QVector3D::crossProduct(p2 - p3, p1 - p2);
//    norm.normalize();
//    glNormal3f(norm.x(), norm.y(), norm.z());
//    glVertex3f(p1.x(), p1.y(), p1.z());
//    glNormal3f(norm.x(), norm.y(), norm.z());
//    glVertex3f(p2.x(), p2.y(), p2.z());
//    glNormal3f(norm.x(), norm.y(), norm.z());
//    glVertex3f(p3.x(), p3.y(), p3.z());

//    glNormal3f(norm.x(), norm.y(), norm.z());
//    glVertex3f(p2.x(), p2.y(), p2.z());
//    glNormal3f(norm.x(), norm.y(), norm.z());
//    glVertex3f(p3.x(), p3.y(), p3.z());
//    glNormal3f(norm.x(), norm.y(), norm.z());
//    glVertex3f(p4.x(), p4.y(), p4.z());
//}
