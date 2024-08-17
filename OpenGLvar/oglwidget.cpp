#include "oglwidget.h"
#include <QDebug>
#include <QString>
#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include <QKeySequence>
#include "vertex.h"
#include "input.h"

static const Vertex a4_frame_Lines[] = {
    Vertex( QVector3D(-105.0f,-148.5f, 1.0f), QVector3D(0.0f, 0.0f, 0.0f)),
    Vertex( QVector3D(-105.0f, 148.5f, 1.0f), QVector3D(0.0f, 0.0f, 0.0f)),

    Vertex( QVector3D(-105.0f, 148.5f, 1.0f), QVector3D(0.0f, 0.0f, 0.0f)),
    Vertex( QVector3D( 105.0f, 148.5f, 1.0f), QVector3D(0.0f, 0.0f, 0.0f)),

    Vertex( QVector3D( 105.0f, 148.5f, 1.0f), QVector3D(0.0f, 0.0f, 0.0f)),
    Vertex( QVector3D( 105.0f,-148.5f, 1.0f), QVector3D(0.0f, 0.0f, 0.0f)),

    Vertex( QVector3D( 105.0f,-148.5f, 1.0f), QVector3D(0.0f, 0.0f, 0.0f)),
    Vertex( QVector3D(-105.0f,-148.5f, 1.0f), QVector3D(0.0f, 0.0f, 0.0f)),

    //List 2
    Vertex( QVector3D(-105.0f, -158.5f, 1.0f), QVector3D(0.0f, 0.0f, 0.0f)),
    Vertex( QVector3D( 105.0f, -158.5f, 1.0f), QVector3D(0.0f, 0.0f, 0.0f)),

    Vertex( QVector3D( 105.0f, -158.5f, 1.0f), QVector3D(0.0f, 0.0f, 0.0f)),
    Vertex( QVector3D( 105.0f,-455.5f, 1.0f), QVector3D(0.0f, 0.0f, 0.0f)),

    Vertex( QVector3D( 105.0f,-455.5f, 1.0f), QVector3D(0.0f, 0.0f, 0.0f)),
    Vertex( QVector3D(-105.0f,-455.5f, 1.0f), QVector3D(0.0f, 0.0f, 0.0f)),

    Vertex( QVector3D(-105.0f,-455.5f, 1.0f), QVector3D(0.0f, 0.0f, 0.0f)),
    Vertex( QVector3D(-105.0f, -158.5f, 1.0f), QVector3D(0.0f, 0.0f, 0.0f))
};

OGLWidget::OGLWidget(QWidget *parent)
: QOpenGLWidget(parent)
{
    m_transform.translate(0.0f, 0.0f, -150.0f);//
    margin = 0;
    verticalOffset = 0;
    horizontalOffset = 0;
}

OGLWidget::~OGLWidget()
{

}

void OGLWidget::getHorizontalOffst(qreal x)
{
    static const float transSpeed = 1.0f;
    float dy = x * transSpeed;
    m_camera.translate(-dy * m_camera.forward());
    // Перерисовываем OpenGL виджет
    update();
}

void OGLWidget::getVerticalOffst(qreal y)
{
//    verticalOffset = y;
    static const float transSpeed = 10.0f;
    float dy = -y * transSpeed;
    verticalOffset = dy;
    m_camera.translate(dy * m_camera.up());

    // Перерисовываем OpenGL виджет
    update();
}


void OGLWidget::setPageSize(const QSizeF &size)
{
    pageSize = size;
    update();
}

void OGLWidget::setMargins(float mm)
{
    margin = mm;
    update();
}

void OGLWidget::initializeGL()
{
    // Initialize OpenGL Backend
    initializeOpenGLFunctions();
    connect(context(), SIGNAL(aboutToBeDestroyed()), this, SLOT(teardownGL()), Qt::DirectConnection);
    connect(this, SIGNAL(frameSwapped()), this, SLOT(update()));
    printVersionInformation();

    // Set global information
    glEnable(GL_CULL_FACE);
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
  //  initializeOpenGLFunctions();

    // Application-specific initialization
    {
      // Create Shader (Do not release until VAO is created)
      m_program = new QOpenGLShaderProgram();
      m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/simple.vert");
      m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/simple.frag");
      m_program->link();
      m_program->bind();

      // Cache Uniform Locations
      u_modelToWorld = m_program->uniformLocation("modelToWorld");
      u_worldToCamera = m_program->uniformLocation("worldToCamera");
      u_cameraToView = m_program->uniformLocation("cameraToView");

      // Create Buffer (Do not release until VAO is created)
      m_vertex.create();
      m_vertex.bind();
      m_vertex.setUsagePattern(QOpenGLBuffer::StaticDraw);
  //    m_vertex.allocate(a4_frame_vertices, sizeof(a4_frame_vertices));
      m_vertex.allocate(a4_frame_Lines, sizeof(a4_frame_Lines));

      // Create Vertex Array Object
      m_object.create();
      m_object.bind();
      m_program->enableAttributeArray(0);
      m_program->enableAttributeArray(1);
      m_program->setAttributeBuffer(0, GL_FLOAT, Vertex::positionOffset(), Vertex::PositionTupleSize, Vertex::stride());
      m_program->setAttributeBuffer(1, GL_FLOAT, Vertex::colorOffset(), Vertex::ColorTupleSize, Vertex::stride());

      // Release (unbind) all
      m_object.release();
      m_vertex.release();
      m_program->release();
    }
}

void OGLWidget::drawA4Sheet()
{
    glColor3f(1.0f, 1.0f, 1.0f); // Белый цвет листа
    glBegin(GL_QUADS);
    glVertex2f(-210.0f, -297.0f); // Начальная точка листа A4 (в мм)
    glVertex2f(210.0f, -297.0f);
    glVertex2f(210.0f, 297.0f);
    glVertex2f(-210.0f, 297.0f);
    glEnd();
}

void OGLWidget::drawFrame()
{
    glColor3f(0.0f, 0.0f, 0.0f); // Черный цвет рамки
    glBegin(GL_LINE_LOOP);
    glVertex2f(-205.0f, -292.0f); // Начальная точка рамки (в мм)
    glVertex2f(205.0f, -292.0f);
    glVertex2f(205.0f, 292.0f);
    glVertex2f(-205.0f, 292.0f);
    glEnd();
}

void OGLWidget::drawViewArea()
{
    glEnable(GL_TEXTURE_2D);
    // Размеры текстуры листа (512x512 пикселей)
    float textureWidth = 512.0f;
    float textureHeight = 512.0f;

    // Относительные координаты и размеры области просмотра (0-99)
    float widthRatio = 10 / 99.0f;
    float heightRatio = 10 / 99.0f;

    // Координаты и размеры части текстуры листа, которую вы хотите отобразить
    float left = -210.0f + (10 / 99.0f) * (420.0f - textureWidth / 10.0f);
    float right = left + textureWidth / 10.0f * widthRatio;
    float bottom = -297.0f + (10 / 99.0f) * (594.0f - textureHeight / 10.0f);
    float top = bottom + textureHeight / 10.0f * heightRatio;

    // Отрисовка прямоугольника с частью текстуры листа
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(left, bottom);

    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(right, bottom);

    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(right, top);

    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(left, top);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void OGLWidget::renderText(double x, double y, double z, const QString &str, const QFont &font)
{
    int width = this->width();
    int height = this->height();

    GLdouble model[4][4], proj[4][4];
    GLint view[4];
//    glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
    glGetDoublev(GL_MODELVIEW_MATRIX, &model[0][0]);
    glGetDoublev(GL_PROJECTION_MATRIX, &proj[0][0]);
    glGetIntegerv(GL_VIEWPORT, &view[0]);
    GLdouble textPosX = 0, textPosY = 0, textPosZ = 0;

    project(x, y, z,
                &model[0][0], &proj[0][0], &view[0],
                &textPosX, &textPosY, &textPosZ);

    textPosY = height - textPosY; // y is inverted
    QPainter painter(this);
    painter.setPen(Qt::black);
    painter.setFont(font);
//    painter.setFont(QFont("Helvetica", 25));
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    painter.drawText(textPosX, textPosY, str); // z = pointT4.z + distOverOp / 4
    painter.end();
}

GLint OGLWidget::project(GLdouble objx, GLdouble objy, GLdouble objz, const GLdouble model[], const GLdouble proj[], const GLint viewport[], GLdouble *winx, GLdouble *winy, GLdouble *winz)
{
    GLdouble in[4], out[4];

    in[0] = objx;
    in[1] = objy;
    in[2] = objz;
    in[3] = 1.0;
    transformPoint(out, model, in);
    transformPoint(in, proj, out);

    if (in[3] == 0.0)
        return GL_FALSE;

    in[0] /= in[3];
    in[1] /= in[3];
    in[2] /= in[3];

    *winx = viewport[0] + (1 + in[0]) * viewport[2] / 2;
    *winy = viewport[1] + (1 + in[1]) * viewport[3] / 2;

    *winz = (1 + in[2]) / 2;
    return GL_TRUE;
}

void OGLWidget::transformPoint(GLdouble out[], const GLdouble m[], const GLdouble in[])
{
#define M(row,col)  m[col*4+row]
    out[0] =
        M(0, 0) * in[0] + M(0, 1) * in[1] + M(0, 2) * in[2] + M(0, 3) * in[3];
    out[1] =
        M(1, 0) * in[0] + M(1, 1) * in[1] + M(1, 2) * in[2] + M(1, 3) * in[3];
    out[2] =
        M(2, 0) * in[0] + M(2, 1) * in[1] + M(2, 2) * in[2] + M(2, 3) * in[3];
    out[3] =
        M(3, 0) * in[0] + M(3, 1) * in[1] + M(3, 2) * in[2] + M(3, 3) * in[3];
#undef M
}

void OGLWidget::update()
{


      // Camera Transformation
      if (Input::buttonPressed(Qt::RightButton))
      {
        static const float transSpeed = 0.5f;
        static const float rotSpeed   = 0.5f;

        // Получение текущих координат объекта
        QVector3D currentPosition = m_camera.translation(); // или m_camera.translation()

        // Вывод текущих координат в консоль
        qDebug() << "Current position:" << currentPosition;
    //    qDebug() << Input::keyState(Qt::Key_W);
        // Handle rotations
    //    m_camera.rotate(-rotSpeed * Input::mouseDelta().x(), Camera3D::LocalUp);
    //    m_camera.rotate(-rotSpeed * Input::mouseDelta().y(), m_camera.right());

        // Определите скорость перемещения на основе изменения координат мыши
        float dx = Input::mouseDelta().x() * transSpeed;
        float dy = Input::mouseDelta().y() * transSpeed;

        // Перемещение вправо/влево
    //    m_camera.translate(-dx * m_camera.right());

        // Перемещение вверх/вниз
        m_camera.translate(dy * m_camera.up());

        // Handle translations
        QVector3D translation;
        if (Input::keyPressed(Qt::Key_W))
        {
          translation += m_camera.forward();
    //      qDebug() << Input::keyPressed(Qt::Key_W);
          qDebug() << "Key pressed:" << QKeySequence(Qt::Key_W).toString();
        }
        if (Input::keyPressed(Qt::Key_S))
        {
          translation -= m_camera.forward();
        }
        if (Input::keyPressed(Qt::Key_A))
        {
          translation -= m_camera.right();
        }
        if (Input::keyPressed(Qt::Key_D))
        {
          translation += m_camera.right();
        }
        if (Input::keyPressed(Qt::Key_Q))
        {
          translation -= m_camera.up();
        }
        if (Input::keyPressed(Qt::Key_E))
        {
          translation += m_camera.up();
        }
        m_camera.translate(transSpeed * translation);
      }

      // Update instance information
    //  m_transform.rotate(1.0f, QVector3D(0.4f, 0.3f, 0.3f));

//      // Schedule a redraw
//      update();
}

void OGLWidget::printVersionInformation()
{
    QString glType;
    QString glVersion;
    QString glProfile;

    // Get Version Information
    glType = (context()->isOpenGLES()) ? "OpenGL ES" : "OpenGL";
    glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));

    // Get Profile Information
  #define CASE(c) case QSurfaceFormat::c: glProfile = #c; break
    switch (format().profile())
    {
      CASE(NoProfile);
      CASE(CoreProfile);
      CASE(CompatibilityProfile);
    }
  #undef CASE

    // qPrintable() will print our QString w/o quotes around it.
    qDebug() << qPrintable(glType) << qPrintable(glVersion) << "(" << qPrintable(glProfile) << ")";
}

void OGLWidget::resizeGL(int w, int h)
{
    m_projection.setToIdentity();
  //  m_projection.perspective(60.0f, width / float(height), 0.1f, 1000.0f);
    m_projection.perspective(40.0f, 2.0f, 1.0f, 1000.0f);
}

void drawSquare(float size) {
    float halfSize = size / 2.0f;
    glBegin(GL_QUADS);
        glVertex2f(-halfSize, -halfSize); // Нижний левый угол
        glVertex2f(halfSize, -halfSize);  // Нижний правый угол
        glVertex2f(halfSize, halfSize);   // Верхний правый угол
        glVertex2f(-halfSize, halfSize);  // Верхний левый угол
    glEnd();
}

void OGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();    

    // Clear
    glClear(GL_COLOR_BUFFER_BIT);

    // Render using our shader
    m_program->bind();
    m_program->setUniformValue(u_worldToCamera, m_camera.toMatrix());
    m_program->setUniformValue(u_cameraToView, m_projection);
    {
      m_object.bind();
      m_program->setUniformValue(u_modelToWorld, m_transform.toMatrix());
  //    glDrawArrays(GL_LINE_LOOP, 0, sizeof(a4_frame_vertices) / sizeof(a4_frame_vertices[0]));
      glDrawArrays(GL_LINES, 0, sizeof(a4_frame_Lines) / sizeof(a4_frame_Lines[0]));
      m_object.release();
    }
    m_program->release();



  //  renderText(10, 10 , 0, QString::fromUtf8("Вы набрали 10 очков:"), QFont());

    // Рисуем текст
    QFont font("GOST type B", 12);
//    glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
    renderText(0, 0, 0.5, "Hello", font);

}
