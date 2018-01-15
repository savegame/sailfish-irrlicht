/** Example 013 Render To Texture

This tutorial shows how to render to a texture using Irrlicht. Render to
texture is a feature with which it is possible to create nice special effects.
In addition, this tutorial shows how to enable specular highlights.

In the beginning, everything as usual. Include the needed headers, ask the user
for the rendering driver, create the Irrlicht Device:
*/

#include <irrlicht.h>
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
	ScreenShaderCB() : WorldViewProjID(-1), TransWorldID(-1), InvWorldID(-1), PositionID(-1),
	                    ColorID(-1), TextureID0(-1), TextureID1(-1), FirstUpdate(true),isFlipped(true),
	                    isFlippedID(-1)
	{
	}

	virtual void OnSetConstants(video::IMaterialRendererServices* services,
	        s32 userData)
	{
		bool UseHighLevelShaders = true;
		video::IVideoDriver* driver = services->getVideoDriver();

		// get shader constants id.

		if (FirstUpdate)
		{
			WorldViewProjID = services->getVertexShaderConstantID("mWorldViewProj");
//			TransWorldID = services->getVertexShaderConstantID("mTransWorld");
//			InvWorldID = services->getVertexShaderConstantID("mInvWorld");
//			PositionID = services->getVertexShaderConstantID("mLightPos");
//			ColorID = services->getVertexShaderConstantID("mLightColor");

			// Textures ID are important only for OpenGL interface.
#ifdef SAILFISH
			//if(driver->getDriverType() == video::EDT_OGLES2)
#else
			if(driver->getDriverType() == video::EDT_OPENGL)
#endif
			{
				TextureID0 = services->getPixelShaderConstantID("Texture0");
				TextureID1 = services->getPixelShaderConstantID("Texture1");
				isFlippedID = services->getPixelShaderConstantID("isFlipped");
			}

			FirstUpdate = false;
		}

		// set inverted world matrix
		// if we are using highlevel shaders (the user can select this when
		// starting the program), we must set the constants by name.

		core::matrix4 invWorld = driver->getTransform(video::ETS_WORLD);
		invWorld.makeInverse();

//		if (UseHighLevelShaders)
//			services->setVertexShaderConstant(InvWorldID, invWorld.pointer(), 16);
//		else
//			services->setVertexShaderConstant(invWorld.pointer(), 0, 4);

		// set clip matrix

		core::matrix4 worldViewProj;
		worldViewProj = driver->getTransform(video::ETS_PROJECTION);
		worldViewProj *= driver->getTransform(video::ETS_VIEW);
		worldViewProj *= driver->getTransform(video::ETS_WORLD);

//		if (UseHighLevelShaders)
			services->setVertexShaderConstant(WorldViewProjID, worldViewProj.pointer(), 16);
//		else
//			services->setVertexShaderConstant(worldViewProj.pointer(), 4, 4);

		// set camera position

//		core::vector3df pos = device->getSceneManager()->
//		    getActiveCamera()->getAbsolutePosition();

//		if (UseHighLevelShaders)
//			services->setVertexShaderConstant(PositionID, reinterpret_cast<f32*>(&pos), 3);
//		else
//			services->setVertexShaderConstant(reinterpret_cast<f32*>(&pos), 8, 1);

		// set light color

		video::SColorf col(0.0f,1.0f,1.0f,0.0f);

//		if (UseHighLevelShaders)
//			services->setVertexShaderConstant(ColorID,
//			        reinterpret_cast<f32*>(&col), 4);
//		else
//			services->setVertexShaderConstant(reinterpret_cast<f32*>(&col), 9, 1);

		// set transposed world matrix

		core::matrix4 world = driver->getTransform(video::ETS_WORLD);
		world = world.getTransposed();

//		if (UseHighLevelShaders)
		{
//			services->setVertexShaderConstant(TransWorldID, world.pointer(), 16);

			// set texture, for textures you can use both an int and a float setPixelShaderConstant interfaces (You need it only for an OpenGL driver).
			s32 TextureLayerID = 0;
			services->setPixelShaderConstant(TextureID0, &TextureLayerID, 1);
			s32 TextureLayerID1 = 1;
			services->setPixelShaderConstant(TextureID1, &TextureLayerID1, 1);
			s32 isFlippedValue = (isFlipped)?1:0;
			services->setPixelShaderConstant(isFlippedID,&isFlippedValue, 1);
		}
//		else
//			services->setVertexShaderConstant(world.pointer(), 10, 4);
	}

	void setIsFlipped(bool val)
	{
		isFlipped = val;
	}

private:
	s32 WorldViewProjID;
	s32 TransWorldID;
	s32 InvWorldID;
	s32 PositionID;
	s32 ColorID;
	s32 TextureID0;
	s32 TextureID1;
	s32 isFlippedID;

	bool FirstUpdate;
	bool isFlipped;
};


class EventReseiver : public irr::IEventReceiver
{
public:

	EventReseiver()
	    : IEventReceiver()
	{
		m_device = NULL;
		m_shader = NULL;
		m_isFlipLandscape = false;
	}

	//! Destructor
//	virtual ~EventReceiver() {}

	//! Called if an event happened.
	/** Please take care that you should only return 'true' when you want to _prevent_ Irrlicht
	* from processing the event any further. So 'true' does mean that an event is completely done.
	* Therefore your return value for all unprocessed events should be 'false'.
	\return True if the event was processed.
	*/
	virtual bool OnEvent(const SEvent& event)
	{
		switch( event.EventType )
		{
		case irr::EET_ORITENTATION_EVENT:
			eventOrientation(event.OrientationEvent);
			break;
		case irr::EET_TOUCH_INPUT_EVENT:
			eventTouch(event.TouchInput);
			break;
		}
	}


	void setSailfishDevice(irr::CIrrDeviceSailfish *device)
	{
		m_device = device;
		m_device->setQESOrientation(irr::EOET_TRANSFORM_270);
		m_isFlipLandscape = false;
	}

	void setScreenShader(ScreenShaderCB *shader)
	{
		m_shader = shader;
		m_shader->setIsFlipped(m_isFlipLandscape);
	}

	bool isFlipLandscape() const
	{
		return m_isFlipLandscape;
	}

protected:
	void eventOrientation(const SEvent::SOrientationEvent &event)
	{
		if(!m_device || !m_shader)
			return;
		switch (event.EventType) {
		case irr::EOET_TRANSFORM_90:
			m_device->setQESOrientation(irr::EOET_TRANSFORM_90);
			m_isFlipLandscape = true;
			m_shader->setIsFlipped(m_isFlipLandscape);
			break;
		case irr::EOET_TRANSFORM_270:
			m_device->setQESOrientation(irr::EOET_TRANSFORM_270);
			m_isFlipLandscape = false;
			m_shader->setIsFlipped(m_isFlipLandscape);
			break;
		}
	}

	void eventTouch(const SEvent::STouchInput &event)
	{

	}

private:

	irr::CIrrDeviceSailfish *m_device;
	ScreenShaderCB *m_shader;
	bool m_isFlipLandscape;
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
#ifndef _DEBUG
		io::path mediaPath = getExampleMediaPath();
#else
		io::path mediaPath = "/home/src1/OpenGL/harbour-irrlicht/irrlicht/media/";
#endif
		io::path psFileName = mediaPath + "Shaders/DFGLES2Screen.fsh";
		io::path vsFileName = mediaPath + "Shaders/DFGLES2Screen.vsh";

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

	virtual void OnRegisterSceneNode()
	{
		if (IsVisible)
			SceneManager->registerNodeForRendering(this);

		ISceneNode::OnRegisterSceneNode();
	}

	virtual void render()
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

	virtual const core::aabbox3d<f32>& getBoundingBox() const
	{
		return Box;
	}

	virtual u32 getMaterialCount() const
	{
		return 1;
	}

	virtual video::SMaterial& getMaterial(u32 i)
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



int main()
{

	// ask user for driver
	video::E_DRIVER_TYPE driverType=video::EDT_OGLES2;
	if (driverType==video::EDT_COUNT)
		return 1;

	// create device and exit if creation failed

	IrrlichtDevice *device =
	    createDevice(driverType, core::dimension2d<u32>(540, 960),
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
#endif
	device->setEventReceiver(receiver);

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
	            smgr->getMesh(mediaPath + "dwarf.x"/*"faerie.md2"*/));
	if (fairy)
	{
		//		fairy->setMaterialTexture(0,
		//				driver->getTexture(mediaPath + "faerie2.bmp")); // set diffuse texture
		fairy->setMaterialFlag(video::EMF_LIGHTING, true); // enable dynamic lighting
		fairy->getMaterial(0).Shininess = 20.0f; // set size of specular highlights
		fairy->setPosition(core::vector3df(0 ,0,60));
		//fairy->setMD2Animation ( scene::EMAT_STAND );
		f32 size = 35.0f;
		f32 size5 = size * 2.5f;
		for(int i = 0; i < 5; i ++)
			for(int j = 0; j < 5; j ++)
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
	core::dimension2du dim = core::dimension2d<u32>(400, 240);
#ifdef SAILFISH
	dim = dynamic_cast<irr::CIrrDeviceSailfish*>(device)->getScreenResolution();
	dim = core::dimension2du(dim.Height*0.5, dim.Width*0.5);
#endif
	if (driver->queryFeature(video::EVDF_RENDER_TO_TARGET))
	{

		renderTargetTex = driver->addRenderTargetTexture(dim, "RTT1", video::ECF_A8R8G8B8);
		renderTargetDepth = driver->addRenderTargetTexture(dim, "DepthStencil", video::ECF_D16);

		renderTarget = driver->addRenderTarget();
		renderTarget->setTexture(renderTargetTex, renderTargetDepth);

		//test->setMaterialTexture(0, renderTargetTex); // set material of cube to render target
		screenNode->setMaterialTexture(0, renderTargetTex);
		screenNode->setMaterialTexture(1, renderTargetDepth);
		// add fixed camera
		fixedCam = smgr->addCameraSceneNode(0, core::vector3df(0,50,-80));
		fixedCam->setFarValue(300.0f);
		fixedCam->setTarget( fairy->getPosition() + core::vector3df(0,60,0) );
		//fixedCam->setNearValue(20.0f);

		{
			f32 width = (f32)dim.Width;
			f32 height = (f32)dim.Height;
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
		f32 width = (f32)dim.Width;
		f32 height = (f32)dim.Height;
		scene::ICameraSceneNode *cam = fpsCamera;
		core::matrix4 m,m2;
		f32 wd = (f32)(width*4);
		f32 hg = (f32)(height*3);
		m2.buildProjectionMatrixOrthoLH(wd,hg,cam->getNearValue(),cam->getFarValue());
		cam->setProjectionMatrix(m2);
	}

	// disable mouse cursor
	device->getCursorControl()->setVisible(false);
	io::path fontPath = mediaPath + "bigfont.png";
	//gui::IGUISkin* skin = env->getSkin();
	gui::IGUIFont *font = env->getFont(fontPath);
	env->getSkin()->setFont(font);
	gui::IGUIStaticText *text = env->addStaticText( L"HelloWorld", core::recti(5,5,dim.Width,60));
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
			env->drawAll();
			// set back old render target
			// The buffer might have been distorted, so clear it
			driver->setRenderTargetEx(0, 0, video::SColor(0));

			// make the cube visible and set the user controlled camera as active one
			//test->setVisible(true);
			smgr->setActiveCamera(fpsCamera);
		}
		
		screenNode->draw(driver);

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
