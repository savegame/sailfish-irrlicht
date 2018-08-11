#include "IrrQuick.h"
#include <QQuickWindow>
#include <irrlicht.h>
#include "CQGLFunctionsDriver.h"

using namespace scene;
using namespace gui;
using namespace video;
using namespace core;

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

void IrrQuickItem::loadExample(int index)
{
	if(m_renderer)
		m_renderer->loadExample(index);
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

void IrrQuickItem::keyPressEvent(QKeyEvent *e)
{
	sendKeyEventToIrrlicht( e, true );
}

void IrrQuickItem::keyReleaseEvent(QKeyEvent *e)
{
	sendKeyEventToIrrlicht( e, false );
}

struct SIrrlichtKey
{
	irr::EKEY_CODE code;
	wchar_t ch;
};

SIrrlichtKey convertToIrrlichtKey( int key )
{
	SIrrlichtKey irrKey;
	irrKey.code = (irr::EKEY_CODE)(0);
	irrKey.ch = (wchar_t)(0);

	// Letters A..Z and numbers 0..9 are mapped directly
	if ( (key >= Qt::Key_A && key <= Qt::Key_Z) || (key >= Qt::Key_0 && key <= Qt::Key_9) )
	{
		irrKey.code = (irr::EKEY_CODE)( key );
		irrKey.ch = (wchar_t)( key );
	}
	else

		// Dang, map keys individually
		switch( key )
		{
		case Qt::Key_Up:
			irrKey.code = irr::KEY_UP;
			break;

		case Qt::Key_Down:
			irrKey.code = irr::KEY_DOWN;
			break;

		case Qt::Key_Left:
			irrKey.code = irr::KEY_LEFT;
			break;

		case Qt::Key_Right:
			irrKey.code = irr::KEY_RIGHT;
			break;
		}
	return irrKey;
}

void IrrQuickItem::sendKeyEventToIrrlicht(QKeyEvent *event, bool pressedDown)
{
	irr::SEvent irrEvent;

	irrEvent.EventType = irr::EET_KEY_INPUT_EVENT;

	SIrrlichtKey irrKey = convertToIrrlichtKey( event->key() );

	if ( irrKey.code == 0 ) return; // Could not find a match for this key

	irrEvent.KeyInput.Key = irrKey.code;
	irrEvent.KeyInput.Control = ((event->modifiers() & Qt::ControlModifier) != 0);
	irrEvent.KeyInput.Shift = ((event->modifiers() & Qt::ShiftModifier) != 0);
	irrEvent.KeyInput.Char = irrKey.ch;
	irrEvent.KeyInput.PressedDown = pressedDown;

	if(m_renderer && m_renderer->m_device)
		m_renderer->m_device->postEventFromUser( irrEvent );
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
	scene::ISceneNode *cube = scene->addCubeSceneNode(2);
	io::path p = _IRRDIR;
	p += "/../../interfaces/InterfaceQt/resources/textures/roof_0.jpg";
	video::ITexture *texture = driver->getTexture(p);
	if(texture)
		cube->setMaterialTexture(0,texture);

	if(scene->getActiveCamera() == NULL)
	{
		scene::ICameraSceneNode *cam = scene->addCameraSceneNode();
		cam->setPosition( core::vector3df(-5,0,0) );
		cam->setTarget(cube->getPosition());
	}
	
	scene::ISceneNodeAnimator *anim = scene->createRotationAnimator( core::vector3df(1.0,1.0,0) );
	cube->addAnimator(anim);
	anim->drop();

	cube = scene->addBillboardSceneNode(NULL, core::dimension2df(3,3) );
	cube->setMaterialType(EMT_TRANSPARENT_ALPHA_CHANNEL);
	cube->setMaterialTexture(0,texture);
}

void GLRenderer::setCamera()
{
	
}

void GLRenderer::loadExample(int index)
{
	if(!clear_scene()) {
		init = &GLRenderer::_empty_init;
		return;
	}

	switch (index) {
	case 1:
		init = &GLRenderer::_load_example_1;
		createCube(); // this garantue of two differend materials in scene
		break;
	case 2:
		init = &GLRenderer::_load_example_2;
		break;
	}

	// some hack ?while error of shader program not fixed

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
	params.OGLES2ShaderPath += "media/Shaders/";
	params.Vsync = false;
	params.qOpenGLFunctions = this;
	
	m_device = createDeviceEx(params);
	m_driver = (CQGLFunctionsDriver*)m_device->getVideoDriver();
	createCube();
	
	init = &GLRenderer::_empty_init;
}

bool GLRenderer::clear_scene()
{
	if(!m_device)
		return  false;
	m_device->getSceneManager()->clear();
	return true;
}

const io::path GLRenderer::getExampleMediaPath()
{
	io::path p = _IRRDIR;
	p += "media/";
	return  p;
}

void GLRenderer::_load_example_1()
{
	/** begin example 1 code */
	IVideoDriver* driver = m_device->getVideoDriver();
	ISceneManager* smgr = m_device->getSceneManager();
	IGUIEnvironment* guienv = m_device->getGUIEnvironment();

	guienv->addStaticText(L"Hello World! This is Irrlicht with the burnings software renderer!",
	    rect<s32>(10,10,260,22), true);

	const io::path mediaPath =  getExampleMediaPath();

	IAnimatedMesh* mesh = smgr->getMesh(mediaPath + "sydney.md2");
	if (!mesh)
	{
		// TODO here need use signal of bad scene, for QML
		clear_scene();
		init = &GLRenderer::_empty_init;
		return;
	}
	IAnimatedMeshSceneNode* node = smgr->addAnimatedMeshSceneNode( mesh );

	if (node)
	{
		node->setMaterialFlag(EMF_LIGHTING, false);
		node->setMD2Animation(scene::EMAT_STAND);
		node->setMaterialTexture( 0, driver->getTexture(mediaPath + "sydney.bmp") );
	}

	smgr->addCameraSceneNode(0, vector3df(0,30,-40), vector3df(0,5,0));
	/** end example 1 */

	/** set epmty fuctions pointer */
	init = &GLRenderer::_empty_init;
}

void GLRenderer::_load_example_2()
{
	/*
	Get a pointer to the video driver and the SceneManager so that
	we do not always have to call irr::IrrlichtDevice::getVideoDriver() and
	irr::IrrlichtDevice::getSceneManager().
	*/
	video::IVideoDriver* driver = m_device->getVideoDriver();
	scene::ISceneManager* smgr = m_device->getSceneManager();

	/*
	To display the Quake 3 map, we first need to load it. Quake 3 maps
	are packed into .pk3 files which are nothing else than .zip files.
	So we add the .pk3 file to our irr::io::IFileSystem. After it was added,
	we can read from the files in that archive as if they were stored on disk.
	*/
	m_device->getFileSystem()->addFileArchive(getExampleMediaPath() + "map-20kdm2.pk3");

	/*
	Now we can load the mesh by calling	irr::scene::ISceneManager::getMesh().
	We get a pointer returned to an	irr::scene::IAnimatedMesh. Quake 3 maps are
	not	really animated, they are only a chunk of static geometry with
	some materials attached. Hence the IAnimatedMesh consists of only one
	frame, so we get the "first frame" of the "animation", which is our
	quake level and create an Octree scene node with it, using
	irr::scene::ISceneManager::addOctreeSceneNode().
	The Octree optimizes the scene a little bit, trying to draw only geometry
	which is currently visible. An alternative to the Octree would be a
	irr::scene::IMeshSceneNode, which would always draw the complete
	geometry of the mesh, without optimization. Try it: Use
	irr::scene::ISceneManager::addMeshSceneNode() instead of
	addOctreeSceneNode() and compare the primitives drawn by the video
	driver. (There is a irr::video::IVideoDriver::getPrimitiveCountDrawn()
	method in the irr::video::IVideoDriver class). Note that this
	optimization with the Octree is only useful when drawing huge meshes
	consisting of lots of geometry and if users can't see the whole scene at
	once.
	*/
	scene::IAnimatedMesh* mesh = smgr->getMesh("20kdm2.bsp");
	scene::ISceneNode* node = 0;

	if (mesh)
		node = smgr->addOctreeSceneNode(mesh->getMesh(0), 0, -1, 1024);
//		node = smgr->addMeshSceneNode(mesh->getMesh(0));

	/*
	Because the level was not modeled around the origin (0,0,0), we
	translate the whole level a little bit. This is done on
	irr::scene::ISceneNode level using the methods
	irr::scene::ISceneNode::setPosition() (in this case),
	irr::scene::ISceneNode::setRotation(), and
	irr::scene::ISceneNode::setScale().
	*/
	if (node)
		node->setPosition(core::vector3df(-1300,-144,-1249));

	/*
	Now we need a camera to look at the Quake 3 map.
	We want to create a user controlled camera. There are some
	cameras available in the Irrlicht engine. For example the
	MayaCamera which can be controlled like the camera in Maya:
	Rotate with left mouse button pressed, Zoom with both buttons pressed,
	translate with right mouse button pressed. This could be created with
	irr::scene::ISceneManager::addCameraSceneNodeMaya(). But for this
	example, we want to create a camera which behaves like the ones in
	first person shooter games (FPS) and hence use
	irr::scene::ISceneManager::addCameraSceneNodeFPS().
	*/
	smgr->addCameraSceneNodeFPS(0,1.0f);

	/*
	The mouse cursor needs not be visible, so we hide it via the
	irr::IrrlichtDevice::ICursorControl.
	*/
//	m_device->getCursorControl()->setVisible(false);

	/** set epmty fuctions pointer */
	init = &GLRenderer::_empty_init;
}

void GLRenderer::paint()
{
	(this->*init)();
//	reinterpret_cast<CQGLFunctionsDriver*>(m_device)->m_functions = m_window->openglContext()->functions();
//	m_driver->setMaterial(IdentityMaterial);
//	/*m_window->openglContext()->functions()->*/glUseProgram( 0 );
	m_device->run();
	m_device->getVideoDriver()->beginScene(true,true, SColor(255,140,140,200) );
	m_device->getSceneManager()->drawAll();
	m_device->getGUIEnvironment()->drawAll();
	m_device->getVideoDriver()->endScene();
	m_window->resetOpenGLState();
}

