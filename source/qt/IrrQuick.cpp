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
	p = "media/irrlicht2_lf.jpg";
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
		break;
	case 2:
		init = &GLRenderer::_load_example_2;
		break;
	case 11:
		init = &GLRenderer::_load_example_11;
		break;
	}

	createCube(); // this garantue of two differend materials in scene
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
	video::IVideoDriver* driver = m_device->getVideoDriver();
	scene::ISceneManager* smgr = m_device->getSceneManager();

	m_device->getFileSystem()->addFileArchive(getExampleMediaPath() + "map-20kdm2.pk3");
	scene::IAnimatedMesh* mesh = smgr->getMesh("20kdm2.bsp");
	scene::ISceneNode* node = 0;

	if (mesh)
		node = smgr->addOctreeSceneNode(mesh->getMesh(0), 0, -1, 1024);
//		node = smgr->addMeshSceneNode(mesh->getMesh(0));

	if (node)
		node->setPosition(core::vector3df(-1300,-144,-1249));

	smgr->addCameraSceneNodeFPS(0,1.0f);

	/*
	The mouse cursor needs not be visible, so we hide it via the
	irr::IrrlichtDevice::ICursorControl.
	*/
//	m_device->getCursorControl()->setVisible(false);

	/** set epmty fuctions pointer */
	init = &GLRenderer::_empty_init;
}

void GLRenderer::_load_example_11()
{
	video::IVideoDriver* driver = m_device->getVideoDriver();
	scene::ISceneManager* smgr = m_device->getSceneManager();
	gui::IGUIEnvironment* env = m_device->getGUIEnvironment();

	driver->setTextureCreationFlag(video::ETCF_ALWAYS_32_BIT, true);

	const io::path mediaPath = getExampleMediaPath();

	// add irrlicht logo
	env->addImage(driver->getTexture(mediaPath + "irrlichtlogo3.png"),
	    core::position2d<s32>(10,10));

	// add camera
	scene::ICameraSceneNode* camera = smgr->addCameraSceneNodeFPS();
	camera->setPosition(core::vector3df(-200,200,-200));

	// disable mouse cursor
//	m_device->getCursorControl()->setVisible(false);
	driver->setFog(video::SColor(0,138,125,81), video::EFT_FOG_LINEAR, 250, 1000, .003f, true, false);

	scene::IAnimatedMesh* roomMesh = smgr->getMesh(mediaPath + "room.3ds");
	scene::ISceneNode* room = 0;
	scene::ISceneNode* earth = 0;

	if (roomMesh)
	{
		smgr->getMeshManipulator()->makePlanarTextureMapping(
		        roomMesh->getMesh(0), 0.003f);

		video::ITexture* normalMap =
		    driver->getTexture(mediaPath + "rockwall_height.bmp");

		if (normalMap)
			driver->makeNormalMapTexture(normalMap, 9.0f);
/*
		// The Normal Map and the displacement map/height map in the alpha channel
		video::ITexture* normalMap =
			driver->getTexture(mediaPath + "rockwall_NRM.tga");
*/
		scene::IMesh* tangentMesh = smgr->getMeshManipulator()->
		        createMeshWithTangents(roomMesh->getMesh(0));

		room = smgr->addMeshSceneNode(tangentMesh);
		room->setMaterialTexture(0,
		        driver->getTexture(mediaPath + "rockwall.jpg"));
		room->setMaterialTexture(1, normalMap);

		// Stones don't glitter..
		room->getMaterial(0).SpecularColor.set(0,0,0,0);
		room->getMaterial(0).Shininess = 0.f;

		room->setMaterialFlag(video::EMF_FOG_ENABLE, true);
		room->setMaterialType(video::EMT_PARALLAX_MAP_SOLID);
		// adjust height for parallax effect
		room->getMaterial(0).MaterialTypeParam = 1.f / 64.f;

		// drop mesh because we created it with a create.. call.
		tangentMesh->drop();
	}

	scene::IAnimatedMesh* earthMesh = smgr->getMesh(mediaPath + "earth.x");
	if (earthMesh)
	{
		//perform various task with the mesh manipulator
		scene::IMeshManipulator *manipulator = smgr->getMeshManipulator();

		// create mesh copy with tangent information from original earth.x mesh
		scene::IMesh* tangentSphereMesh =
		    manipulator->createMeshWithTangents(earthMesh->getMesh(0));

		// set the alpha value of all vertices to 200
		manipulator->setVertexColorAlpha(tangentSphereMesh, 200);

		// scale the mesh by factor 50
		core::matrix4 m;
		m.setScale ( core::vector3df(50,50,50) );
		manipulator->transform( tangentSphereMesh, m );

		earth = smgr->addMeshSceneNode(tangentSphereMesh);

		earth->setPosition(core::vector3df(-70,130,45));

		// load heightmap, create normal map from it and set it
		video::ITexture* earthNormalMap = driver->getTexture(mediaPath + "earthbump.jpg");
		if (earthNormalMap)
		{
			driver->makeNormalMapTexture(earthNormalMap, 20.0f);
			earth->setMaterialTexture(1, earthNormalMap);
			earth->setMaterialType(video::EMT_NORMAL_MAP_TRANSPARENT_VERTEX_ALPHA);
		}

		// adjust material settings
		earth->setMaterialFlag(video::EMF_FOG_ENABLE, true);

		// add rotation animator
		scene::ISceneNodeAnimator* anim =
		    smgr->createRotationAnimator(core::vector3df(0,0.1f,0));
		earth->addAnimator(anim);
		anim->drop();

		// drop mesh because we created it with a create.. call.
		tangentSphereMesh->drop();
	}

	scene::ILightSceneNode* light1 =
	    smgr->addLightSceneNode(0, core::vector3df(0,0,0),
	    video::SColorf(0.5f, 1.0f, 0.5f, 0.0f), 800.0f);

	// add fly circle animator to light 1
	scene::ISceneNodeAnimator* anim =
	    smgr->createFlyCircleAnimator (core::vector3df(50,300,0),190.0f, -0.003f);
	light1->addAnimator(anim);
	anim->drop();

	// attach billboard to the light
	scene::IBillboardSceneNode* bill =
	    smgr->addBillboardSceneNode(light1, core::dimension2d<f32>(60, 60));

	bill->setMaterialFlag(video::EMF_LIGHTING, false);
	bill->setMaterialFlag(video::EMF_ZWRITE_ENABLE, false);
	bill->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
	bill->setMaterialTexture(0, driver->getTexture(mediaPath + "particlegreen.jpg"));

	// add light 2 (red)
	scene::ISceneNode* light2 =
	    smgr->addLightSceneNode(0, core::vector3df(0,0,0),
	    video::SColorf(1.0f, 0.2f, 0.2f, 0.0f), 800.0f);

	// add fly circle animator to light 2
	anim = smgr->createFlyCircleAnimator(core::vector3df(0,150,0), 200.0f,
	        0.001f, core::vector3df(0.2f, 0.9f, 0.f));
	light2->addAnimator(anim);
	anim->drop();

	// attach billboard to light
	bill = smgr->addBillboardSceneNode(light2, core::dimension2d<f32>(120, 120));
	bill->setMaterialFlag(video::EMF_LIGHTING, false);
	bill->setMaterialFlag(video::EMF_ZWRITE_ENABLE, false);
	bill->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
	bill->setMaterialTexture(0, driver->getTexture(mediaPath + "particlered.bmp"));

	// add particle system
	scene::IParticleSystemSceneNode* ps =
	    smgr->addParticleSystemSceneNode(false, light2);

	// create and set emitter
	scene::IParticleEmitter* em = ps->createBoxEmitter(
	    core::aabbox3d<f32>(-3,0,-3,3,1,3),
	    core::vector3df(0.0f,0.03f,0.0f),
	    80,100,
	    video::SColor(10,255,255,255), video::SColor(10,255,255,255),
	    400,1100);
	em->setMinStartSize(core::dimension2d<f32>(30.0f, 40.0f));
	em->setMaxStartSize(core::dimension2d<f32>(30.0f, 40.0f));

	ps->setEmitter(em);
	em->drop();

	// create and set affector
	scene::IParticleAffector* paf = ps->createFadeOutParticleAffector();
	ps->addAffector(paf);
	paf->drop();

	// adjust some material settings
	ps->setMaterialFlag(video::EMF_LIGHTING, false);
	ps->setMaterialFlag(video::EMF_ZWRITE_ENABLE, false);
	ps->setMaterialTexture(0, driver->getTexture(mediaPath + "fireball.bmp"));
	ps->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);

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

