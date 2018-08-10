#include "IrrQuick.h"
#include <QQuickWindow>
#include <irrlicht.h>
#include "CQGLFunctionsDriver.h"

IrrQuickItem::IrrQuickItem(QQuickItem *parent)
    : QQuickItem(parent)
    , m_t(0)
    , m_renderer(nullptr)
{
	connect( this, &QQuickItem::windowChanged, this, &IrrQuickItem::handleWindowChanged );
}

void IrrQuickItem::setT(qreal t)
{
	if( t == m_t )
		return;
	m_t = t;
	emit tChanged();
	if(window())
		window()->update();
}

void IrrQuickItem::sync()
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

void IrrQuickItem::cleanup()
{
	if(m_renderer)
	{
		delete m_renderer;
		m_renderer = nullptr;
	}
}

void IrrQuickItem::handleWindowChanged(QQuickWindow *window)
{
	if(window)
	{
		connect(window, &QQuickWindow::beforeSynchronizing, this, &IrrQuickItem::sync, Qt::DirectConnection );
		connect(window, &QQuickWindow::sceneGraphInvalidated, this, &IrrQuickItem::cleanup, Qt::DirectConnection );
		window->setClearBeforeRendering(false);
	}
}

void IrrQuickItem::touchEvent(QTouchEvent *e)
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
			
			break;
		case Qt::TouchPointStationary:
			break;
		case Qt::TouchPointReleased:
			
			break;
		}
	}
}

void IrrQuickItem::mousePressEvent(QMouseEvent *e)
{
	m_pressPos = e->localPos();
}

void IrrQuickItem::mouseReleaseEvent(QMouseEvent *e)
{
	
}

struct VertexData
{
	QVector3D position;
	QVector2D texCoord;
};

GLRenderer::GLRenderer(QObject *parent)
    : QObject(parent)
    , m_t(0)
    , m_device(nullptr)
    , m_driver(nullptr)
{
	init = &GLRenderer::_first_init;
	//    m_rotation = 0;
}

GLRenderer::~GLRenderer()
{
	m_device->drop();
}

void GLRenderer::setViewportSize(const QSize &size)
{
	//	return;
	m_viewportSize = size;
	if(!m_device)
		return;
	if(m_device->getSceneManager()->getActiveCamera())
		m_device->getSceneManager()->getActiveCamera()->setAspectRatio((float)size.width()/(float)size.height());
}

void GLRenderer::createCube()
{
	if(!m_device)
		return;
	scene::ISceneManager *scene = m_device->getSceneManager();
	video::IVideoDriver *driver = m_device->getVideoDriver();
	scene::ICameraSceneNode *cam = scene->addCameraSceneNode();
	cam->setPosition( core::vector3df(-5,0,0) );
	scene::ISceneNode *cube = scene->addCubeSceneNode(2);
	video::ITexture *texture = driver->getTexture("/home/andreev/programming/opengl/boxworld/interfaces/InterfaceQt/resources/textures/roof_0.jpg");
	if(texture)
		cube->setMaterialTexture(0,texture);
	cam->setTarget(cube->getPosition());
	
	scene::ISceneNodeAnimator *anim = scene->createRotationAnimator( core::vector3df(1.0,1.0,0) );
	cube->addAnimator(anim);
	anim->drop();
}

void GLRenderer::setCamera()
{
	
}

void GLRenderer::_first_init()
{
	initializeOpenGLFunctions();
	irr::SIrrlichtCreationParameters params;
	
	params.DriverType = EDT_QOGLDUNCTIONS;//driverType;
	params.DeviceType = EIDT_CONSOLE;
	params.WindowId = NULL;
	params.Bits = 32;
	params.Doublebuffer = true;
	params.Fullscreen = false;
	params.IgnoreInput = false;
	params.Stencilbuffer = true;
	params.Stereobuffer = false;
	params.WindowSize.Width = m_window->geometry().width();
	params.WindowSize.Height = m_window->geometry().height();
	params.AntiAlias = 0;
	params.OGLES2ShaderPath = _IRRDIR;
	params.OGLES2ShaderPath += "/media/Shaders/";
	params.Vsync = false;
	params.qOpenGLFunctions = this;
	
	m_device =createDeviceEx(params);
	
	createCube();
	
	init = &GLRenderer::_empty_init;
}

void GLRenderer::paint()
{
	(this->*init)();
//	reinterpret_cast<CQGLFunctionsDriver*>(m_device)->m_functions = m_window->openglContext()->functions();
//	m_window->openglContext()->functions()->glUseProgram( 0 );
	m_device->run();
	m_device->getVideoDriver()->beginScene(true,true, SColor(255,140,140,200) );
	m_device->getSceneManager()->drawAll();
	m_device->getGUIEnvironment()->drawAll();
	m_device->getVideoDriver()->endScene();
//	m_window->resetOpenGLState();
}

