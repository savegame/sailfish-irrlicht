/** Example 013 Render To Texture

This tutorial shows how to render to a texture using Irrlicht. Render to
texture is a feature with which it is possible to create nice special effects.
In addition, this tutorial shows how to enable specular highlights.

In the beginning, everything as usual. Include the needed headers, ask the user
for the rendering driver, create the Irrlicht Device:
*/

//#define /*NO_XREADER_DEBUG*/

#include <irrlicht.h>
#include <source/Irrlicht/CGUIButton.h>
#include <os.h>
#include "driverChoice.h"
#include "exampleHelper.h"
#include <map>

#ifdef SAILFISH
#include <source/Irrlicht/CIrrDeviceSailfish.h>
#endif

using namespace irr;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

class ScreenShaderCB : public video::IShaderConstantSetCallBack
{
public:
	enum ScreenOrientation : int {
		Normal    = 0,
		Rotate90  = 1,
		Rotate270 = 2
	};
public:
	ScreenShaderCB() : WorldViewProjID(-1), 
//	    TransWorldID(-1), 
//	    InvWorldID(-1),
//	    PositionID(-1),
//	    ColorID(-1), 
	    TextureID0(-1), TextureID1(-1), FirstUpdate(true),
#ifdef SAILFISH
	                    m_screenOrientation(Rotate270),
#else
	                    m_screenOrientation(Normal),
#endif
	                    OrientationID(-1),
	                    ResolutionID(-1)
	{
		m_resolution = core::dimension2df(640.0,480.0);
		m_depth_far = core::vector2df(0.999f,0.0005f);
		m_depth_near = core::vector2df(0.005f,0.01f);
	}

	virtual void OnSetConstants(video::IMaterialRendererServices* services,
	        s32 userData) _IRR_OVERRIDE_
	{
		video::IVideoDriver* driver = services->getVideoDriver();

		// get shader constants id.

		if (FirstUpdate)
		{
			WorldViewProjID = services->getVertexShaderConstantID("mWorldViewProj");
//			TransWorldID = services->getVertexShaderConstantID("mTransWorld");
//			InvWorldID = services->getVertexShaderConstantID("mInvWorld");
//			PositionID = services->getVertexShaderConstantID("mLightPos");
//			ColorID = services->getVertexShaderConstantID("mLightColor");
			TextureID0 = services->getPixelShaderConstantID("Texture0");
			TextureID1 = services->getPixelShaderConstantID("Texture1");
			OrientationID = services->getPixelShaderConstantID("inScreenOrientation");
			ResolutionID = services->getPixelShaderConstantID("inResolution");
			DepthNearID = services->getPixelShaderConstantID("inDepthNear");
			DepthFarID = services->getPixelShaderConstantID("inDepthFar");
			FirstUpdate = false;
		}

		core::matrix4 invWorld = driver->getTransform(video::ETS_WORLD);
		invWorld.makeInverse();

		core::matrix4 worldViewProj;
		worldViewProj = driver->getTransform(video::ETS_PROJECTION);
		worldViewProj *= driver->getTransform(video::ETS_VIEW);
		worldViewProj *= driver->getTransform(video::ETS_WORLD);

		services->setVertexShaderConstant(WorldViewProjID, worldViewProj.pointer(), 16);
		
		video::SColorf col(0.0f,1.0f,1.0f,0.0f);
		core::matrix4 world = driver->getTransform(video::ETS_WORLD);
		world = world.getTransposed();

		s32 TextureLayerID = 0;
		services->setPixelShaderConstant(TextureID0, &TextureLayerID, 1);
		s32 TextureLayerID1 = 1;
		services->setPixelShaderConstant(TextureID1, &TextureLayerID1, 1);
		services->setPixelShaderConstant(OrientationID,&m_screenOrientation, 1);
		services->setPixelShaderConstant(ResolutionID,reinterpret_cast<f32*>(&m_resolution), 2);
		services->setPixelShaderConstant(DepthNearID,reinterpret_cast<f32*>(&m_depth_near), 2);
		services->setPixelShaderConstant(DepthFarID,reinterpret_cast<f32*>(&m_depth_far), 2);
	}

	void setIsFlipped(s32 val)
	{
		m_screenOrientation = val;
	}

private:
	s32 WorldViewProjID;
//	s32 TransWorldID;
//	s32 InvWorldID;
//	s32 PositionID;
//	s32 ColorID;
	s32 TextureID0;
	s32 TextureID1;
	s32 OrientationID;
	s32 ResolutionID;
	s32 DepthNearID;
	s32 DepthFarID;
	
public:	
	s32  m_screenOrientation;
	core::vector2df  m_depth_near;
	core::vector2df  m_depth_far;
	core::dimension2df m_resolution;
	bool FirstUpdate;
};


class EventReseiver : public irr::IEventReceiver
{
public:

	EventReseiver()
	    : IEventReceiver()
	{
#ifdef SAILFISH
		m_device = NULL;
#endif
		m_shader = NULL;
#ifdef SAILFISH
		m_isFlipLandscape = ScreenShaderCB::ScreenOrientation::Rotate270;
#else
		m_isFlipLandscape = ScreenShaderCB::ScreenOrientation::Normal;
#endif
	}

	//! Destructor
//	virtual ~EventReceiver() {}

	//! Called if an event happened.
	/** Please take care that you should only return 'true' when you want to _prevent_ Irrlicht
	* from processing the event any further. So 'true' does mean that an event is completely done.
	* Therefore your return value for all unprocessed events should be 'false'.
	\return True if the event was processed.
	*/
	virtual bool OnEvent(const SEvent& event) _IRR_OVERRIDE_
	{
		switch( event.EventType )
		{
#ifdef SAILFISH
		case irr::EET_ORITENTATION_EVENT:
			eventOrientation(event.OrientationEvent);
			break;
#endif
		case irr::EET_TOUCH_INPUT_EVENT:
			eventTouch(event.TouchInput);
			break;
		case irr::EET_LOG_TEXT_EVENT:
			eventLog(event.LogEvent);
		default:
//			IEventReceiver::OnEvent(event);
			break;
		}
	}

#ifdef SAILFISH
	void setSailfishDevice(irr::CIrrDeviceSailfish *device)
	{
		m_device = device;
		m_device->setQESOrientation(irr::EOET_TRANSFORM_270);
		m_isFlipLandscape = ScreenShaderCB::ScreenOrientation::Rotate270;
	}
#endif

	void setScreenShader(ScreenShaderCB *shader)
	{
		m_shader = shader;
		m_shader->setIsFlipped(m_isFlipLandscape);
	}

	s32 isFlipLandscape() const
	{
		return m_isFlipLandscape;
	}

protected:
#ifdef SAILFISH
	void eventOrientation(const SEvent::SOrientationEvent &event)
	{
		if(!m_device || !m_shader)
			return;
		switch (event.EventType) {
		case irr::EOET_TRANSFORM_90:
			m_device->setQESOrientation(irr::EOET_TRANSFORM_90);
			m_isFlipLandscape = ScreenShaderCB::ScreenOrientation::Rotate90;
			m_shader->setIsFlipped(m_isFlipLandscape);
			break;
		case irr::EOET_TRANSFORM_270:
			m_device->setQESOrientation(irr::EOET_TRANSFORM_270);
			m_isFlipLandscape = ScreenShaderCB::ScreenOrientation::Rotate270;
			m_shader->setIsFlipped(m_isFlipLandscape);
			break;
		}
	}
#endif
	void eventTouch(const SEvent::STouchInput &event)
	{

	}
	
	void eventLog(const SEvent::SLogEvent &event)
	{
		printf("%s\n", event.Text);
	}
private:
#ifdef SAILFISH
	irr::CIrrDeviceSailfish *m_device;
#endif
	ScreenShaderCB *m_shader;
	s32 m_isFlipLandscape;
	//std::map<int> m_touch;
};

class ScreenNode : public scene::ISceneNode
{
public:
	ScreenNode(scene::ISceneNode* parent, scene::ISceneManager* mgr, s32 id)
	    : scene::ISceneNode(parent, mgr, id)
	{
		bool UseHighLevelShaders= true;
		Material.Wireframe = false;
		Material.Lighting = false;
		Material.Thickness=0.f;
		Vertices[0] = video::S3DVertex(-1,-1.0,0, 5,1,0,
		        video::SColor(255,0,255,255), 0, 1);
		Vertices[1] = video::S3DVertex(-1,1,0, 10,0,0,
		        video::SColor(255,255,0,255), 0, 0);
		Vertices[2] = video::S3DVertex(1,1,0, 20,1,1,
		        video::SColor(255,255,255,0), 1, 0);
		Vertices[3] = video::S3DVertex(1,-1,0, 40,0,1,
		        video::SColor(255,0,255,0), 1, 1);
		Box.reset(Vertices[0].Pos);
		for (s32 i=1; i<4; ++i)
			Box.addInternalPoint(Vertices[i].Pos);
		//shaders
#if !defined(_DEBUG) || defined(_IRR_COMPILE_WITH_X11_DEVICE_)
		io::path mediaPath = getExampleMediaPath();
#elif defined(SAILFISH)
		io::path mediaPath = "/home/src1/OpenGL/harbour-irrlicht/irrlicht/media/";
#else
		io::path mediaPath = getExampleMediaPath();
#endif
		io::path psFileName = mediaPath + "Shaders/DFGLES2Screen.fsh";
		io::path vsFileName = mediaPath + "Shaders/DFGLES2Screen.vsh";
#if defined(_OUT_PWD_PATH) && !defined(SAILFISH)
		psFileName = io::path(_OUT_PWD_PATH) + io::path("/") + psFileName;
		vsFileName = io::path(_OUT_PWD_PATH) + io::path("/") + vsFileName;
#endif
		video::IGPUProgrammingServices* gpu = mgr->getVideoDriver()->getGPUProgrammingServices();
		ShaderMaterial = 0;

		if (gpu)
		{
			m_shader = new ScreenShaderCB();

			if (true)
			{
				// Choose the desired shader type. Default is the native
				// shader type for the driver
				const video::E_GPU_SHADING_LANGUAGE shadingLanguage = video::EGSL_DEFAULT;

				// create material from high level shaders (hlsl, glsl)
				irr::os::Printer::log( "Loading shaders: ", irr::ELL_DEBUG );
				core::stringc m = "  ";
				m += mediaPath;
				irr::os::Printer::log( m.c_str(), irr::ELL_DEBUG );

				ShaderMaterial = gpu->addHighLevelShaderMaterialFromFiles(
				    vsFileName, "main", video::EVST_VS_1_1,
				    psFileName, "main", video::EPST_PS_1_1,
				    m_shader, video::EMT_SOLID, 0, shadingLanguage);

				Material.MaterialType = ((video::E_MATERIAL_TYPE)ShaderMaterial);
			}
		}
		u16 indices[] = {0,1,2, 0,2,3, };
		memcpy(m_indices,indices, 6*sizeof(u16));
	}

	~ScreenNode()
	{
		m_shader->drop();
	}

	ScreenShaderCB* getShader() const { return m_shader; }

	virtual void OnRegisterSceneNode() _IRR_OVERRIDE_
	{
		if (IsVisible)
			SceneManager->registerNodeForRendering(this);

		ISceneNode::OnRegisterSceneNode();
	}

	virtual void render() _IRR_OVERRIDE_
	{
		/* Indices into the 'Vertices' array. A triangle needs 3 vertices
		so you have to pass the 3 corresponding indices for each triangle to
		tell which of the vertices should be used for it.	*/
		u16 indices[] = {	0,1,2, 0,2,3, 2,1,0, 3,2,0	};
		video::IVideoDriver* driver = SceneManager->getVideoDriver();

		driver->setMaterial(Material);
		driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);
		driver->drawVertexPrimitiveList(&Vertices[0], 4, &indices[0], 2, video::EVT_STANDARD, scene::EPT_TRIANGLES, video::EIT_16BIT);
	}

	void draw(video::IVideoDriver* driver)
	{
		driver->setMaterial(video::SMaterial());
		driver->setTransform ( video::ETS_PROJECTION, core::IdentityMatrix );
		driver->setTransform ( video::ETS_VIEW, core::IdentityMatrix );
		driver->setTransform ( video::ETS_WORLD, core::IdentityMatrix );

		u16 indices[] = {	0,1,2, 0,2,3 };
		driver->setMaterial(Material);
		driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);
		driver->drawVertexPrimitiveList(Vertices, 4, m_indices, 2, video::EVT_STANDARD, scene::EPT_TRIANGLES, video::EIT_16BIT);
	}

	virtual const core::aabbox3d<f32>& getBoundingBox() const _IRR_OVERRIDE_
	{
		return Box;
	}

	virtual u32 getMaterialCount() const _IRR_OVERRIDE_
	{
		return 1;
	}

	virtual video::SMaterial& getMaterial(u32 i) _IRR_OVERRIDE_
	{
		return Material;
	}

protected:
	ScreenShaderCB*  m_shader;
	core::aabbox3d<f32> Box;
	video::S3DVertex Vertices[4];
	video::SMaterial Material;
	s32 ShaderMaterial;
	u16 m_indices[6];
};


class Button : public gui::CGUIButton
{
public:
	Button(gui::IGUIEnvironment* environment, gui::IGUIElement* parent, s32 id, core::rect<s32> rectangle)
	    : gui::CGUIButton(environment, parent, id, rectangle)
	{
		
	}
	
protected:
	
};

enum GUI_ID : s32
{
	id_Button0 = 100,
	id_scrollFar1,
};

void create_ui(gui::IGUIEnvironment* env, const core::dimension2du &resolution)
{
	env->addButton( core::recti(10,10,128,128), nullptr, id_Button0, L"", L"" );

	gui::IGUIScrollBar *scroll = env->addScrollBar(false, core::recti(10,10, (resolution.Width < resolution.Height)?resolution.Width:resolution.Height, 20 ), 0, id_scrollFar1 );

}

int main()
{

	// ask user for driver
	video::E_DRIVER_TYPE driverType=video::EDT_OGLES2;
	if (driverType==video::EDT_COUNT)
		return 1;

	// create device and exit if creation failed
	core::dimension2du resolution =
#ifdef SAILFISH
	        core::dimension2d<u32>(400, 240);
#else
	        core::dimension2d<u32>(800, 480);
#endif
			
	IrrlichtDevice *device =
	    createDevice(driverType, resolution,
		16, false, false);

	if (device == 0)
		return 1; // could not create selected driver.

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();
	gui::IGUIEnvironment* env = device->getGUIEnvironment();
	ILogger *logger = device->getLogger();
	logger->setLogLevel(irr::ELL_DEBUG);

	EventReseiver *receiver = new EventReseiver();
#ifdef SAILFISH
	receiver->setSailfishDevice( reinterpret_cast<irr::CIrrDeviceSailfish*>(device) );
	device->getCursorControl()->setVisible(false);
#else
	device->getCursorControl()->setVisible(true);
#endif
	device->setEventReceiver(receiver);

	create_ui(env);
	
	const io::path mediaPath = getExampleMediaPath();
	
	/*
	Now, we load an animated mesh to be displayed. As in most examples,
	we'll take the fairy md2 model. The difference here: We set the
	shininess of the model to a value other than 0 which is the default
	value. This enables specular highlights on the model if dynamic
	lighting is on. The value influences the size of the highlights.
	*/

	// load and display animated fairy mesh
	scene::IAnimatedMeshSceneNode* fairy = smgr->addAnimatedMeshSceneNode(
	            smgr->getMesh(mediaPath + 
#ifdef fairy
	"faerie.md2"));
#else
	"dwarf.x"));
#endif
	if (fairy)
	{
		        
		fairy->setMaterialFlag(video::EMF_LIGHTING, true); // enable dynamic lighting
		fairy->getMaterial(0).Shininess = 40.0f; // set size of specular highlights
		fairy->setPosition(core::vector3df(0 ,0,60));
		fairy->setAnimationSpeed(1);
		fairy->setFrameLoop(0, 10);
//		fairy->setA
#ifdef fairy
		fairy->setMD2Animation ( scene::EMAT_STAND );
		fairy->setMaterialTexture(0,
		        driver->getTexture(mediaPath + "faerie2.bmp")); // set diffuse texture
#endif
		f32 size = 55.0f;
		f32 size5 = size * 2.5f;
		for(int i = 0; i < 2; i ++)
			for(int j = 0; j < 6; j ++)
		{
			fairy->clone(smgr->getRootSceneNode(),smgr)->setPosition(core::vector3df(i*size - size5 ,0,j*size));
		}
	}



	// add white light
	smgr->addLightSceneNode(0, core::vector3df(-15,5,-105),
			video::SColorf(1.0f, 1.0f, 1.0f));

	// set ambient light
	smgr->setAmbientLight(video::SColor(0,60,60,60));

	// set window caption
	device->setWindowCaption(L"Irrlicht Engine 1.9 GLES2");


	// create render target
	video::IRenderTarget* renderTarget = 0;
	video::ITexture* renderTargetDepth = 0;
	video::ITexture* renderTargetTex = 0;
	scene::ICameraSceneNode* fixedCam = 0;
	ScreenNode *screenNode = new ScreenNode(smgr->getRootSceneNode(), smgr, -1);
	receiver->setScreenShader(screenNode->getShader());
#ifdef SAILFISH
	resolution = dynamic_cast<irr::CIrrDeviceSailfish*>(device)->getScreenResolution();
	resolution = core::dimension2du(resolution.Height*0.5, resolution.Width*0.5);
#endif
	
	screenNode->getShader()->m_resolution = core::dimension2df((f32)resolution.Width, (f32)resolution.Height);
	
	if (driver->queryFeature(video::EVDF_RENDER_TO_TARGET))
	{

		renderTargetTex = driver->addRenderTargetTexture(resolution, "RTT1", video::ECF_A8R8G8B8);
		renderTargetDepth = driver->addRenderTargetTexture(resolution, "DepthStencil", video::ECF_D16);

		renderTarget = driver->addRenderTarget();
		renderTarget->setTexture(renderTargetTex, renderTargetDepth);

		//test->setMaterialTexture(0, renderTargetTex); // set material of cube to render target
		screenNode->setMaterialTexture(0, renderTargetTex);
		screenNode->setMaterialTexture(1, renderTargetDepth);
		// add fixed camera
#ifdef fairy
		fixedCam = smgr->addCameraSceneNode(0, core::vector3df(0,35,15));
		fixedCam->setFarValue(300.0f);
		fixedCam->setTarget( fairy->getPosition() + core::vector3df(0,10,0) );
#else
		fixedCam = smgr->addCameraSceneNode(0, core::vector3df(0,80,-35));
		fixedCam->setFarValue(300.0f);
		fixedCam->setTarget( fairy->getPosition() + core::vector3df(0,70,-60) );
#endif
		//fixedCam->setNearValue(20.0f);

		{
			f32 width = (f32)resolution.Width;
			f32 height = (f32)resolution.Height;
			scene::ICameraSceneNode *cam = fixedCam;
			core::matrix4 m2;
			f32 wd = (f32)(width*0.003651);
			f32 hg = (f32)(height*0.003651);
			m2.buildProjectionMatrixPerspectiveLH(wd,hg,cam->getNearValue(),cam->getFarValue());
			cam->setProjectionMatrix(m2);
		}
		//fixedCam->set
	}
	else
	{
		// create problem text
		gui::IGUISkin* skin = env->getSkin();
		gui::IGUIFont* font = env->getFont(mediaPath + "fonthaettenschweiler.bmp");
		if (font)
			skin->setFont(font);

		gui::IGUIStaticText* text = env->addStaticText(
			L"Your hardware or this renderer is not able to use the "\
			L"render to texture feature. RTT Disabled.",
			core::rect<s32>(150,20,470,60));

		text->setOverrideColor(video::SColor(100,255,255,255));
	}
	
	// add fps camera
//	scene::ICameraSceneNode* fpsCamera = smgr->addCameraSceneNodeFPS(0,1.0f);
	scene::ICameraSceneNode* fpsCamera = smgr->addCameraSceneNode(0, core::vector3df(0,0,-1) );
//	fpsCamera->setPosition(core::vector3df(-50,50,-150));
	fpsCamera->setTarget(screenNode->getPosition());
	{
		f32 width = (f32)resolution.Width;
		f32 height = (f32)resolution.Height;
		scene::ICameraSceneNode *cam = fpsCamera;
		core::matrix4 m,m2;
		f32 wd = (f32)(width*4);
		f32 hg = (f32)(height*3);
		m2.buildProjectionMatrixOrthoLH(wd,hg,cam->getNearValue(),cam->getFarValue());
		cam->setProjectionMatrix(m2);
	}

	io::path fontPath = mediaPath + "bigfont.png";
	//gui::IGUISkin* skin = env->getSkin();
	gui::IGUIFont *font = env->getFont(fontPath);
	env->getSkin()->setFont(font);
	gui::IGUIStaticText *text = env->addStaticText( L"HelloWorld", core::recti(5,5,resolution.Width,60));
	/*
	Nearly finished. Now we need to draw everything. Every frame, we draw
	the scene twice. Once from the fixed camera into the render target
	texture and once as usual. When rendering into the render target, we
	need to disable the visibility of the test cube, because it has the
	render target texture applied to it. That's it, wasn't too complicated
	I hope. :)
	*/

	int lastFPS = -1;
	bool showPause = true;

	while(device->run())
	if (device->isWindowActive())
	{
		driver->beginScene(video::ECBF_COLOR | video::ECBF_DEPTH, video::SColor(0));

		if (renderTarget)
		{
			// draw scene into render target
			
			// set render target texture
			driver->setRenderTargetEx(renderTarget, video::ECBF_COLOR | video::ECBF_DEPTH, video::SColor(0,0,0,255));

			// make cube invisible and set fixed camera as active camera
			//test->setVisible(false);
			smgr->setActiveCamera(fixedCam);

			// draw whole scene into render buffer
			smgr->drawAll();
//			env->drawAll();
			// set back old render target
			// The buffer might have been distorted, so clear it
			driver->setRenderTargetEx(0, 0, video::SColor(0));

			// make the cube visible and set the user controlled camera as active one
			//test->setVisible(true);
			smgr->setActiveCamera(fpsCamera);
		}
		
		screenNode->draw(driver);
		env->drawAll();
//		smgr->drawAll();
//		driver->drawMeshBuffer();


		driver->endScene();

		// display frames per second in window title
		int fps = driver->getFPS();
		if (lastFPS != fps)
		{
			core::stringw str = L"FPS:";
			str += fps;
			text->setText(str.c_str());

			//device->setWindowCaption(str.c_str());
			lastFPS = fps;
		}
		showPause = true;
	}
	else if(showPause)
	{

	}

	device->drop(); // drop device
	return 0;
}

/*
**/
