#include "IrrQuick.h"
#include <QQuickWindow>
#include <irrlicht.h>

GLView::GLView(QQuickItem *parent)
    : QQuickItem(parent)
    , m_t(0)
    , m_renderer(nullptr)
{
    connect( this, &QQuickItem::windowChanged, this, &GLView::handleWindowChanged );
}

void GLView::setT(qreal t)
{
    if( t == m_t )
        return;
    m_t = t;
    emit tChanged();
    if(window())
        window()->update();
}

void GLView::sync()
{
    if(!m_renderer)
    {
        m_renderer = new GLRenderer();
        connect( window(), &QQuickWindow::beforeRendering, m_renderer, &GLRenderer::paint, Qt::DirectConnection );
    }
    m_renderer->setViewportSize( window()->size() * window()->devicePixelRatio() );
    m_renderer->setT(m_t);
    m_renderer->setWindow( window() );

}

void GLView::cleanup()
{
    if(m_renderer)
    {
        delete m_renderer;
        m_renderer = nullptr;
    }
}

void GLView::handleWindowChanged(QQuickWindow *window)
{
    if(window)
    {
        connect(window, &QQuickWindow::beforeSynchronizing, this, &GLView::sync, Qt::DirectConnection );
        connect(window, &QQuickWindow::sceneGraphInvalidated, this, &GLView::cleanup, Qt::DirectConnection );
        window->setClearBeforeRendering(false);
    }
}

void GLView::touchEvent(QTouchEvent *e)
{
    // Save mouse press position
//    mousePressPosition =
    if(!m_renderer)
        return;

    const QList<QTouchEvent::TouchPoint> points = e->touchPoints();

    int count = points.size();
    if(count = 1)
    {
        QTouchEvent::TouchPoint current = *points.begin();

//        m_renderer->m_angularSpeed
        switch(current.state())
        {
        case Qt::TouchPointPressed:
            m_pressPos = current.pos();
            break;
        case Qt::TouchPointMoved:
//            {
//                if(m_pressPos == current.startPos())
//                {
//                    int i = 0;
//                }
//                // Mouse release position - mouse press position
//                QVector2D diff = QVector2D(e->pos()) - m_pressPos;

//                // Rotation axis is perpendicular to the mouse position difference
//                // vector
//                QVector3D n = QVector3D(diff.y(), diff.x(), 0.0).normalized();

//                // Accelerate angular speed relative to the length of the mouse sweep
//                qreal acc = diff.length() / 100.0;

//                // Calculate new rotation axis as weighted sum
//                m_renderer->m_rotationAxis = (m_renderer->m_rotationAxis * m_renderer->m_angularSpeed + n * acc).normalized();

//                // Increase angular speed
//                m_renderer->m_angularSpeed += acc;
//            }
            break;
        case Qt::TouchPointStationary:
            break;
        case Qt::TouchPointReleased:
            {
                if(m_pressPos == current.startPos())
                {
                    int i = 0;
                }
                // Mouse release position - mouse press position
                QVector2D diff = QVector2D(current.pos()) - QVector2D(m_pressPos.x(), m_pressPos.y() );

                // Rotation axis is perpendicular to the mouse position difference
                // vector
                QVector3D n = QVector3D(diff.y(), diff.x(), 0.0).normalized();

                // Accelerate angular speed relative to the length of the mouse sweep
                qreal acc = diff.length() / 100.0;

                // Calculate new rotation axis as weighted sum
                m_renderer->m_rotationAxis = (m_renderer->m_rotationAxis * m_renderer->m_angularSpeed + n * acc).normalized();

                // Increase angular speed
                m_renderer->m_angularSpeed += acc;
            }
            break;
        }
    }
//    else
//    {
//        QList<QTouchEvent::TouchPoint>::const_iterator
//                it = points.begin(),
//                end = points.end();
//        for(; it != end; it++)
//        {
//        }
    //    }
}

void GLView::mousePressEvent(QMouseEvent *e)
{
    m_pressPos = e->localPos();
}

void GLView::mouseReleaseEvent(QMouseEvent *e)
{
    // Mouse release position - mouse press position
    QVector2D diff = QVector2D(e->localPos()) - QVector2D(m_pressPos.x(), m_pressPos.y() );

    // Rotation axis is perpendicular to the mouse position difference
    // vector
    QVector3D n = QVector3D(diff.y(), diff.x(), 0.0).normalized();

    // Accelerate angular speed relative to the length of the mouse sweep
    qreal acc = diff.length() / 100.0;

    // Calculate new rotation axis as weighted sum
    m_renderer->m_rotationAxis = (m_renderer->m_rotationAxis * m_renderer->m_angularSpeed + n * acc).normalized();

    // Increase angular speed
    m_renderer->m_angularSpeed += acc;
}

struct VertexData
{
    QVector3D position;
    QVector2D texCoord;
};

GLRenderer::GLRenderer(QObject *parent)
    : QObject(parent)
    , m_t(0)
    , m_program(nullptr)
    , m_indexBuf(nullptr)
    , m_arrayBuf(nullptr)
{
//    m_rotation = 0;
}

GLRenderer::~GLRenderer()
{
    delete m_program;
    delete m_texture;
    m_arrayBuf->destroy();
    m_indexBuf->destroy();
    delete m_arrayBuf;
    delete m_indexBuf;
}

void GLRenderer::setViewportSize(const QSize &size)
{
    m_viewportSize = size;
    // Calculate aspect ratio
    qreal aspect = qreal(size.width()) / qreal(size.height() ? size.height() : 1);

    // Set near plane to 3.0, far plane to 7.0, field of view 45 degrees
    const qreal zNear = 3.0, zFar = 500.0, fov = 45.0;

    // Reset projection
    m_projection.setToIdentity();

    // Set perspective projection
    m_projection.perspective(fov, aspect, zNear, zFar);
}

void GLRenderer::init()
{

    m_indexBuf = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    m_arrayBuf = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);

    m_arrayBuf->create();
    m_indexBuf->create();
}

void GLRenderer::createCube()
{
    // For cube we would need only 8 vertices but we have to
        // duplicate vertex for each face because texture coordinate
        // is different.
        VertexData vertices[] = {
            // Vertex data for face 0
            {QVector3D(-1.0f, -1.0f,  1.0f), QVector2D(0.0f, 0.0f)},  // v0
            {QVector3D( 1.0f, -1.0f,  1.0f), QVector2D(0.33f, 0.0f)}, // v1
            {QVector3D(-1.0f,  1.0f,  1.0f), QVector2D(0.0f, 0.5f)},  // v2
            {QVector3D( 1.0f,  1.0f,  1.0f), QVector2D(0.33f, 0.5f)}, // v3

            // Vertex data for face 1
            {QVector3D( 1.0f, -1.0f,  1.0f), QVector2D( 0.0f, 0.5f)}, // v4
            {QVector3D( 1.0f, -1.0f, -1.0f), QVector2D(0.33f, 0.5f)}, // v5
            {QVector3D( 1.0f,  1.0f,  1.0f), QVector2D(0.0f, 1.0f)},  // v6
            {QVector3D( 1.0f,  1.0f, -1.0f), QVector2D(0.33f, 1.0f)}, // v7

            // Vertex data for face 2
            {QVector3D( 1.0f, -1.0f, -1.0f), QVector2D(0.66f, 0.5f)}, // v8
            {QVector3D(-1.0f, -1.0f, -1.0f), QVector2D(1.0f, 0.5f)},  // v9
            {QVector3D( 1.0f,  1.0f, -1.0f), QVector2D(0.66f, 1.0f)}, // v10
            {QVector3D(-1.0f,  1.0f, -1.0f), QVector2D(1.0f, 1.0f)},  // v11

            // Vertex data for face 3
            {QVector3D(-1.0f, -1.0f, -1.0f), QVector2D(0.66f, 0.0f)}, // v12
            {QVector3D(-1.0f, -1.0f,  1.0f), QVector2D(1.0f, 0.0f)},  // v13
            {QVector3D(-1.0f,  1.0f, -1.0f), QVector2D(0.66f, 0.5f)}, // v14
            {QVector3D(-1.0f,  1.0f,  1.0f), QVector2D(1.0f, 0.5f)},  // v15

            // Vertex data for face 4
            {QVector3D(-1.0f, -1.0f, -1.0f), QVector2D(0.33f, 0.0f)}, // v16
            {QVector3D( 1.0f, -1.0f, -1.0f), QVector2D(0.66f, 0.0f)}, // v17
            {QVector3D(-1.0f, -1.0f,  1.0f), QVector2D(0.33f, 0.5f)}, // v18
            {QVector3D( 1.0f, -1.0f,  1.0f), QVector2D(0.66f, 0.5f)}, // v19

            // Vertex data for face 5
            {QVector3D(-1.0f,  1.0f,  1.0f), QVector2D(0.33f, 0.5f)}, // v20
            {QVector3D( 1.0f,  1.0f,  1.0f), QVector2D(0.66f, 0.5f)}, // v21
            {QVector3D(-1.0f,  1.0f, -1.0f), QVector2D(0.33f, 1.0f)}, // v22
            {QVector3D( 1.0f,  1.0f, -1.0f), QVector2D(0.66f, 1.0f)}  // v23
        };

        GLushort indices[] = {
            0,  1,  2,  3,  3,     // Face 0 - triangle strip ( v0,  v1,  v2,  v3)
            4,  4,  5,  6,  7,  7, // Face 1 - triangle strip ( v4,  v5,  v6,  v7)
            8,  8,  9, 10, 11, 11, // Face 2 - triangle strip ( v8,  v9, v10, v11)
            12, 12, 13, 14, 15, 15, // Face 3 - triangle strip (v12, v13, v14, v15)
            16, 16, 17, 18, 19, 19, // Face 4 - triangle strip (v16, v17, v18, v19)
            20, 20, 21, 22, 23      // Face 5 - triangle strip (v20, v21, v22, v23)
        };

        // Transfer vertex data to VBO 0
        m_arrayBuf->bind();
        m_arrayBuf->allocate(vertices, 24 * sizeof(VertexData));

        // Transfer index data to VBO 1
        m_indexBuf->bind();
        m_indexBuf->allocate(indices, 34 * sizeof(GLushort));
        m_angularSpeed = 0.5;
        m_rotationAxis.setZ(1);
        m_rotationAxis.setY(1);
}

void GLRenderer::setCamera()
{
    // Decrease angular speed (friction)
    m_angularSpeed *= 0.99;

    // Stop rotation when speed goes below threshold
    if (m_angularSpeed < 0.01) {
        m_angularSpeed = 0.0;
    } else {
        // Update rotation
        m_rotation = QQuaternion::fromAxisAndAngle(m_rotationAxis, m_angularSpeed) * m_rotation;

        // Request an update
//        update();
    }
}

void GLRenderer::paint()
{
    if(!m_program)
    {
        initializeOpenGLFunctions();

        // Generate 2 VBOs
        init();
        createCube();

        m_program = new QOpenGLShaderProgram();
        m_program->addShaderFromSourceFile(
                    QOpenGLShader::Vertex,
                    ":/resources/shaders/vertex.vsh"
        );
        m_program->addShaderFromSourceFile(
                    QOpenGLShader::Fragment,
                    ":/resources/shaders/fragment.fsh"
        );
//        m_program->bindAttributeLocation("vertices",0);
        m_program->link();

        m_texture = new QOpenGLTexture(QImage(":/resources/kot-pushnot.jpg"));
        m_texture->setMinificationFilter(QOpenGLTexture::Nearest);
        m_texture->setMagnificationFilter(QOpenGLTexture::Linear);
        m_texture->setWrapMode(QOpenGLTexture::Repeat);

    }

    glViewport(0,0, m_viewportSize.width(), m_viewportSize.height());
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);


    m_program->bind();
    m_arrayBuf->bind();
    m_indexBuf->bind();
    m_texture->bind();
//    setCamera();
    m_rotation = QQuaternion::fromAxisAndAngle(m_rotationAxis, m_angularSpeed) * m_rotation;
    // Offset for position
    quintptr offset = 0;

    // Calculate model view transformation
    QMatrix4x4 matrix;
    matrix.translate(0.0, 0.0, -8.0);
    matrix.rotate(m_rotation);

    // Set modelview-projection matrix
    m_program->setUniformValue("mvp_matrix", m_projection * matrix);
    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = m_program->attributeLocation("a_position");
    m_program->enableAttributeArray(vertexLocation);
    m_program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

    // Offset for texture coordinate
    offset += sizeof(QVector3D);

    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    int texcoordLocation = m_program->attributeLocation("a_texcoord");
    m_program->enableAttributeArray(texcoordLocation);
    m_program->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(VertexData));

    // Draw cube geometry using indices from VBO 1
    glDrawElements(GL_TRIANGLE_STRIP, 34, GL_UNSIGNED_SHORT, 0);

    m_window->resetOpenGLState();
}

