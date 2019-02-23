#ifndef VISUALIZATION_H
#define VISUALIZATION_H

#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>

class Visualization : public QOpenGLWidget
{
    Q_OBJECT
public:
    explicit Visualization(QWidget *parent = nullptr);

signals:

public slots:

protected:
    void initializeGL();
};

#endif // VISUALIZATION_H
