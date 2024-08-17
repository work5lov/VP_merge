#ifndef OGLWIDGET_H
#define OGLWIDGET_H

#include <QWidget>
#include <QtOpenGL>
#include <GL/gl.h>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLDebugLogger>
#include <QPainter>

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QMatrix4x4>
#include "transform3d.h"
#include "camera3d.h"


class OGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    OGLWidget(QWidget *parent = 0);
    ~OGLWidget();

public slots:
    void getHorizontalOffst(qreal x);
    void getVerticalOffst(qreal y);
    void setPageSize(const QSizeF &size);
    void setMargins(float mm);

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void redrawSquare(qreal offsetX, qreal offsetY);
    // Рисуем лист формата A4

    void drawA4Sheet();
    // Рисуем рамку листа
    void drawFrame();
    void drawViewArea();

    void renderText(double x, double y, double z, const QString & str, const QFont & font = QFont());
    inline GLint project(GLdouble objx, GLdouble objy, GLdouble objz,
                         const GLdouble model[16], const GLdouble proj[16],
                         const GLint viewport[4],
                         GLdouble * winx, GLdouble * winy, GLdouble * winz);
    inline void transformPoint(GLdouble out[4], const GLdouble m[16], const GLdouble in[4]);
    void teardownGL();
    void update();


private:
    QSizeF pageSize;
    float margin;
    int verticalOffset;
    int horizontalOffset;

    QOpenGLBuffer m_vertex;
    QOpenGLVertexArrayObject m_object;
    QOpenGLShaderProgram *m_program;

    // Shader Information
    int u_modelToWorld;
    int u_worldToCamera;
    int u_cameraToView;
    QMatrix4x4 m_projection;
    Camera3D m_camera;
    Transform3D m_transform;

    // Private Helpers
    void printVersionInformation();
};

#endif // OGLWIDGET_H
