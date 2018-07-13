/** Example 013 Render To Texture

This tutorial shows how to render to a texture using Irrlicht. Render to
texture is a feature with which it is possible to create nice special effects.
In addition, this tutorial shows how to enable specular highlights.

In the beginning, everything as usual. Include the needed headers, ask the user
for the rendering driver, create the Irrlicht Device:
*/

//#define /*NO_XREADER_DEBUG*/

#include <map>
#include <irrlicht.h>
#include <source/Irrlicht/CGUIButton.h>
#include <os.h>
#include "driverChoice.h"
#include <exampleHelper.h>

#ifdef _IRR_COMPILE_WITH_SAILFISH_DEVICE_
#include <source/Irrlicht/CIrrDeviceSailfish.h>
#endif

using namespace irr;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

enum GUI_ID : s32
{
	id_Button0 = 100,
	id_ButtonDown,
	id_scrollFar1,
};

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
#ifdef _IRR_COMPILE_WITH_SAILFISH_DEVICE_
	    m_screenOrientation(Rotate270),
#else
	    m_screenOrientation(Normal),
#endif
	    OrientationID(-1),
	    ResolutionID(-1)
	{
		m_resolution = core::dimension2df(640.0,480.0);
		m_depth_far = core::vector2df(0.999f,0.95f);
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
			IsUseDepthID  = services->getPixelShaderConstantID("inIsUseDepth");
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

	void setOrientation(s32 val)
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
	s32 IsUseDepthID;
	
public:	
	s32  m_screenOrientation;
	core::vector2df  m_depth_near;
	core::vector2df  m_depth_far;
	core::dimension2df m_resolution;
	bool FirstUpdate;
};


class CGUIJoystic : public gui::IGUIElement
{
	friend class EventReseiver;
public:
	CGUIJoystic(gui::IGUIEnvironment* environment, IGUIElement* parent,
	            s32 id, const core::rect<s32>& rectangle)
	    : IGUIElement(gui::EGUIET_BUTTON, environment, parent, id, rectangle)
	{
//		RelativeRect.UpperLeftCorner = core::position2di(0,0);
//		RelativeRect.LowerRightCorner = core::position2di(0,0);
	}

	//! Draws the element and its children.
	virtual void draw() _IRR_OVERRIDE_
	{
		if ( isVisible() )
		{
			core::list<IGUIElement*>::Iterator it = Children.begin();
			for (; it != Children.end(); ++it)
				(*it)->draw();
		}

	}

	//! Called if an event happened.
	virtual bool OnEvent(const SEvent& event)
	{
		return false;
		bool result = false;
		if( event.EventType != EET_TOUCH_INPUT_EVENT )
			return Parent ? Parent->OnEvent(event) : true;

		SEvent::STouchInput te = event.TouchInput;
		switch(te.Event)
		{
		case ETIE_PRESSED_DOWN:
			if( !AbsoluteRect.isPointInside( core::vector2di(te.X, te.Y) ) )
				break;
			TouchPressPosition.X = te.X;
			TouchPressPosition.Y = te.Y;
			Velocity.X = Velocity.Y = 0.0f;
			IsTouchPressed = true;
			TouchID = te.ID;
			return true;
			break;
		case ETIE_MOVED:
			if(!IsTouchPressed || te.ID != TouchID)
				break;
			Velocity.X = te.X - TouchPressPosition.X;
			Velocity.X = (Velocity.X > 0) ? (Velocity.X > Radius.X ? Radius.X : Velocity.X) : (Velocity.X < Radius.X ? -Radius.X : Velocity.X);
			Velocity.X = Velocity.X / Radius.X;
			Velocity.Y = te.Y - TouchPressPosition.Y;
			Velocity.Y = (Velocity.Y > 0) ? (Velocity.Y > Radius.Y ? Radius.Y : Velocity.Y) : (Velocity.Y < Radius.Y ? -Radius.Y : Velocity.Y);
			Velocity.Y = Velocity.Y / Radius.Y;
			return true;
			break;
		case ETIE_LEFT_UP:
			if(!IsTouchPressed || te.ID != TouchID)
				break;
			IsTouchPressed = false;
			Velocity.X = Velocity.Y = 0.0f;
			return true;
			break;
		}

		return/* Parent && !result  ? Parent->OnEvent(event) : */result;
	}



public:
	size_t             TouchID;
	bool               IsTouchPressed;
	core::position2di  TouchPressPosition;
	core::vector2df    Velocity;
	core::vector2df    Radius;
};

class EventReseiver : public irr::IEventReceiver
{
public:

	EventReseiver()
	    : IEventReceiver()
	{
#ifdef _IRR_COMPILE_WITH_SAILFISH_DEVICE_
		m_device = NULL;
		m_touchCount = 0;
#endif
		m_shader = NULL;
#ifdef _IRR_COMPILE_WITH_SAILFISH_DEVICE_
		m_orientation = ScreenShaderCB::ScreenOrientation::Rotate270;
#else
		m_isFlipLandscape = ScreenShaderCB::ScreenOrientation::Normal;
#endif
		fpsCamera = nullptr;
		Joystic = nullptr;
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
#ifdef _IRR_COMPILE_WITH_SAILFISH_DEVICE_
		case irr::EET_ORITENTATION_EVENT:
			return eventOrientation(event.OrientationEvent);
			break;
#endif
		case irr::EET_TOUCH_INPUT_EVENT:
			if(!m_device->getGUIEnvironment()->postEventFromUser(event))
				return eventTouch(event.TouchInput);
			break;
		case irr::EET_MOUSE_INPUT_EVENT:
			return eventMouse(event.MouseInput);
			break;
		case irr::EET_LOG_TEXT_EVENT:
			eventLog(event.LogEvent);
			break;
//		case irr::EET_KEY_INPUT_EVENT:
//			if(fpsCamera)
//				return fpsCamera->OnEvent(event);
			break;
		case irr::EET_GUI_EVENT:
			switch( event.GUIEvent.EventType)
			{
			case irr::gui::EGET_BUTTON_CLICKED:
			case irr::gui::EGET_BUTTON_PRESSED:
				{
					if( event.GUIEvent.Caller->getID() == id_Button0 )
					{
						gui::IGUIButton *btn = static_cast<gui::IGUIButton *>(event.GUIEvent.Caller);
						if(fpsCamera && btn)
						{
							SEvent keyEvent;
							keyEvent.EventType = EET_KEY_INPUT_EVENT;
							keyEvent.KeyInput.PressedDown = btn->isPressed() ;
							keyEvent.KeyInput.Key =  KEY_UP;
							return fpsCamera->OnEvent(keyEvent);
						}
					}
					else if(event.GUIEvent.Caller->getID() == id_ButtonDown )
					{
						gui::IGUIButton *btn = static_cast<gui::IGUIButton *>(event.GUIEvent.Caller);
						if(fpsCamera && btn)
						{
							SEvent keyEvent;
							keyEvent.EventType = EET_KEY_INPUT_EVENT;
							keyEvent.KeyInput.PressedDown = btn->isPressed() ;
							keyEvent.KeyInput.Key =  KEY_DOWN;
							return fpsCamera->OnEvent(keyEvent);
						}
					}
				}
				break;
			}
			break;
		default:
//			IEventReceiver::OnEvent(event);
			break;
		}
		return false;
	}

#ifdef _IRR_COMPILE_WITH_SAILFISH_DEVICE_
	void setSailfishDevice(irr::CIrrDeviceSailfish *device)
	{
		m_device = device;
		m_device->setQESOrientation(irr::EOET_TRANSFORM_270);
		m_orientation = ScreenShaderCB::ScreenOrientation::Rotate270;

//		Joystic = new CGUIJoystic(
//		            m_device->getGUIEnvironment(),
//		            nullptr, 0,
//		            core::recti(0,0, m_device->getScreenResolution().Height*0.5f, m_device->getScreenResolution().Width) );
//		Joystic->Radius.X = Joystic->Radius.Y = m_device->getScreenResolution().Width * 0.33f;
	}
#endif

	void setScreenShader(ScreenShaderCB *shader)
	{
		m_shader = shader;
		m_shader->setOrientation(m_orientation);
	}

	s32 isFlipLandscape() const
	{
		return m_orientation;
	}

protected:
#ifdef _IRR_COMPILE_WITH_SAILFISH_DEVICE_
	bool eventOrientation(const SEvent::SOrientationEvent &event)
	{
		if(!m_device || !m_shader)
			return false;
		switch (event.EventType) {
		case irr::EOET_TRANSFORM_90:
			m_device->setQESOrientation(irr::EOET_TRANSFORM_90);
			m_orientation = ScreenShaderCB::ScreenOrientation::Rotate90;
			m_shader->setOrientation(m_orientation);
			return true;
			break;
		case irr::EOET_TRANSFORM_270:
			m_device->setQESOrientation(irr::EOET_TRANSFORM_270);
			m_orientation = ScreenShaderCB::ScreenOrientation::Rotate270;
			m_shader->setOrientation(m_orientation);
			return true;
			break;
		}
		return false;
	}
#endif
	bool eventTouch(const SEvent::STouchInput &event)
	{

//		switch(event.Event)
//		{
//		case irr::ETIE_MOVED:
//			break;
//		case irr::ETIE_PRESSED_DOWN:
//			for( int i = 0; i < 10; i++)
//			{
//				if( m_touches[i].id == -1 )
//				{
//					m_touches[i].id = event.ID;
//					m_touches[i].pos.X = event.X;
//					m_touches[i].pos.Y = event.Y;
//					m_touches[i].pressed = true;
//					break;
//				}
//			}
//			break;
//		case irr::ETIE_LEFT_UP:
//			for( int i = 0; i < 10; i++)
//			{
//				if( m_touches[i].id == event.ID )
//				{
//					m_touches[i].id = -1;
//					m_touches[i].pos.X = event.X;
//					m_touches[i].pos.Y = event.Y;
//					m_touches[i].pressed = false;
//					break;
//				}
//			}
//			break;
//		}

		SEvent cameraEvent;
		cameraEvent.EventType = EET_TOUCH_INPUT_EVENT;
		cameraEvent.TouchInput = event;
#ifdef _IRR_COMPILE_WITH_SAILFISH_DEVICE_
//		if(m_isFlipLandscape == ScreenShaderCB::ScreenOrientation::Rotate270)
//		{
//			cameraEvent.TouchInput.X = event.Y;
//			cameraEvent.TouchInput.Y = (m_device->getScreenResolution().Width - event.X);
//		}
//		else if(m_isFlipLandscape == ScreenShaderCB::ScreenOrientation::Rotate90)
//		{
//			cameraEvent.TouchInput.X = (m_device->getScreenResolution().Height - event.Y);
//			cameraEvent.TouchInput.Y = event.X;
//		}
#endif
		bool isJoyEvent = (Joystic)?Joystic->IsTouchPressed:false;
		bool isJoystic = (Joystic)?Joystic->OnEvent(cameraEvent):false;

		if( !isJoystic )
			return fpsCamera->OnEvent(cameraEvent);

		if(isJoystic && false)
		{
			SEvent keyEvent;
			keyEvent.EventType = EET_KEY_INPUT_EVENT;
			keyEvent.KeyInput.PressedDown = true;

			if( Joystic->Velocity.Y > 0.2 )
				keyEvent.KeyInput.Key = KEY_DOWN;
			else if( Joystic->Velocity.Y < -0.2 )
				keyEvent.KeyInput.Key = KEY_UP;

			return fpsCamera->OnEvent(keyEvent);

//			if( Joystic->Velocity.Y > 0.2 )
//				keyEvent.KeyInput.Key = KEY_RIGHT;
//			else if( Joystic->Velocity.Y < -0.2 )
//				keyEvent.KeyInput.Key = KEY_LEFT;

//			fpsCamera->OnEvent(keyEvent);
		}
	}
	
	void eventLog(const SEvent::SLogEvent &event)
	{
		printf("%s\n", event.Text);
	}
	
	bool eventMouse(const SEvent::SMouseInput &event)
	{
		if(fpsCamera){
			SEvent cameraEvent;
			cameraEvent.EventType = EET_MOUSE_INPUT_EVENT;
			cameraEvent.MouseInput = event;
			
			return fpsCamera->OnEvent(cameraEvent);
		}
	}
private:
#ifdef _IRR_COMPILE_WITH_SAILFISH_DEVICE_
	irr::CIrrDeviceSailfish *m_device;
	//std::map<int,int> m_touch;
#endif

	struct touch {
		size_t id;
		bool pressed;
		core::vector2di pos;

		touch()
		{
			id = -1;
			pressed = false;
		}
	};

	touch m_touches[10];
	int m_touchCount;
	ScreenShaderCB *m_shader;
	s32 m_orientation;
public:

	scene::ICameraSceneNode *fpsCamera;
	CGUIJoystic *Joystic;
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
#elif defined(_IRR_COMPILE_WITH_SAILFISH_DEVICE_)
        io::path mediaPath = getExampleMediaPath();// "/home/src1/OpenGL/harbour-irrlicht/irrlicht/media/";
#else
		io::path mediaPath = getExampleMediaPath();
#endif
        io::path psFileName = mediaPath + "Shaders/DFGLES2Screen.fsh";
		io::path vsFileName = mediaPath + "Shaders/DFGLES2Screen.vsh";
#if defined(_OUT_PWD_PATH) && !defined(_IRR_COMPILE_WITH_SAILFISH_DEVICE_)
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


void create_ui(gui::IGUIEnvironment* env, const core::dimension2du &resolution)
{
	//env->addButton( core::recti(10,10,128,128), nullptr, id_Button0, L"", L"" );
//	Button *btn = new Button(env, env->getRootGUIElement(), id_Button0, core::recti(10,10,256,256));
	core::dimension2du btnSize = core::dimension2du(128,128);
	core::dimension2du margin = core::dimension2du(10,10);
	gui::IGUIButton *btn = env->addButton(
	            core::recti(core::position2di(10,resolution.Height - btnSize.Height*2 - margin.Height*2)
	                        ,btnSize)
	            ,nullptr,id_Button0,L"UP"
	            );
	env->addButton(
	            core::recti(
	                btn->getRelativePosition().UpperLeftCorner + core::position2di(0,btn->getRelativePosition().getHeight() + margin.Height),
	                btnSize
	                ),
	            nullptr,id_ButtonDown,L"DOWN"
	            );

	gui::IGUIScrollBar *scroll = env->addScrollBar(true, core::recti(10, resolution.Height - 30, (resolution.Width < resolution.Height)?resolution.Width:resolution.Height, resolution.Height - 20 ), 0, id_scrollFar1 );
	scroll->setMin(0);
	scroll->setMin(100);
}

#define IRRLICHT_QUAKE3_ARENA
#ifdef IRRLICHT_QUAKE3_ARENA
    #define QUAKE3_STORAGE_FORMAT   addFileArchive
    #define QUAKE3_STORAGE_1        getExampleMediaPath() + "map-20kdm2.pk3"//"q3maps/q3ctfp22_final.pk3"
    #define QUAKE3_MAP_NAME         "maps/20kdm2.bsp"
#endif

void load_q3map(io::path path, IrrlichtDevice *device/*, scene::ICameraSceneNode *camera = nullptr*/)
{
	if( path.empty() )
		path = QUAKE3_MAP_NAME;

	if( !device->getFileSystem()->addFileArchive(QUAKE3_STORAGE_1) )
	{
		core::stringc m = "ERROR: Cant open \"";
		m += QUAKE3_STORAGE_1;
		m += "\" file";
		device->getLogger()->log(m.c_str(),ELL_ERROR);
		return;
	}
	device->getSceneManager()->getParameters()->setAttribute(scene::ALLOW_ZWRITE_ON_TRANSPARENT, true);

	scene::IQ3LevelMesh* const mesh =
	    (scene::IQ3LevelMesh*) device->getSceneManager()->getMesh(path);

	scene::ISceneNode* node = 0;
	if (mesh)
	{
		scene::IMesh * const geometry = mesh->getMesh(scene::quake3::E_Q3_MESH_GEOMETRY);
		node = device->getSceneManager()->addOctreeSceneNode(geometry, 0, -1, 4096);
	}

	if ( mesh )
	{
		// the additional mesh can be quite huge and is unoptimized
		const scene::IMesh * const additional_mesh = mesh->getMesh(scene::quake3::E_Q3_MESH_ITEMS);

#ifdef SHOW_SHADER_NAME
		gui::IGUIFont *font = device->getGUIEnvironment()->getFont(mediaPath + "fontlucida.png");
		u32 count = 0;
#endif

		for ( u32 i = 0; i!= additional_mesh->getMeshBufferCount(); ++i )
		{
			const scene::IMeshBuffer* meshBuffer = additional_mesh->getMeshBuffer(i);
			const video::SMaterial& material = meshBuffer->getMaterial();

			// The ShaderIndex is stored in the material parameter
			const s32 shaderIndex = (s32) material.MaterialTypeParam2;

			// the meshbuffer can be rendered without additional support, or it has no shader
			const scene::quake3::IShader *shader = mesh->getShader(shaderIndex);
			if (0 == shader)
			{
				continue;
			}

			// we can dump the shader to the console in its
			// original but already parsed layout in a pretty
			// printers way.. commented out, because the console
			// would be full...
			// quake3::dumpShader ( Shader );

			node = device->getSceneManager()->addQuake3SceneNode(meshBuffer, shader);

#ifdef SHOW_SHADER_NAME
			count += 1;
			core::stringw name( node->getName() );
			node = device->getSceneManager()->addBillboardTextSceneNode(
			        font, name.c_str(), node,
			        core::dimension2d<f32>(80.0f, 8.0f),
			        core::vector3df(0, 10, 0));
#endif
		}
	}

	/*
	Now we only need a Camera to look at the Quake 3 map. And we want to
	create a user controlled camera. There are some different cameras
	available in the Irrlicht engine. For example the Maya Camera which can
	be controlled comparable to the camera in Maya: Rotate with left mouse
	button pressed, Zoom with both buttons pressed, translate with right
	mouse button pressed. This could be created with
	addCameraSceneNodeMaya(). But for this example, we want to create a
	camera which behaves like the ones in first person shooter games (FPS).
	*/

	/*if(!camera)
		camera = device->getSceneManager()->getActiveCamera();

//	camera->setUpVector( core::vector3df(1,0,0) );

	if ( mesh )
	{
		scene::quake3::tQ3EntityList &entityList = mesh->getEntityList();

		scene::quake3::IEntity search;
		search.name = "info_player_deathmatch";

		s32 index = entityList.binary_search(search);
		if (index >= 0)
		{
			s32 notEndList;
			do
			{
				const scene::quake3::SVarGroup *group = entityList[index].getGroup(1);

				u32 parsepos = 0;
				const core::vector3df pos =
					scene::quake3::getAsVector3df(group->get("origin"), parsepos);

				parsepos = 0;
				const f32 angle = scene::quake3::getAsFloat(group->get("angle"), parsepos);

				core::vector3df target(0.f, 0.f, 1.f);
				target.rotateXZBy(angle);

				camera->setPosition(pos);
				camera->setTarget(pos + target);

				++index;
/*
				notEndList = (	index < (s32) entityList.size () &&
								entityList[index].name == search.name &&
								(device->getTimer()->getRealTime() >> 3 ) & 1
							);
 * /
				notEndList = index == 2;
			} while ( notEndList );
		}
	}*/
//	camera->setPosition(core::vector3df(1300,144,1249));
}

void fixCamera(scene::ICameraSceneNode *cam, core::dimension2du resolution)
{
	cam->setAspectRatio((f32)resolution.Width/(f32)resolution.Height);
	return;
}

int main()
{

	// ask user for driver
#ifdef __APPLE__
	video::E_DRIVER_TYPE driverType=video::EDT_OPENGL;
#else
	video::E_DRIVER_TYPE driverType=video::EDT_OGLES2;
#endif
	if (driverType==video::EDT_COUNT)
		return 1;

	// create device and exit if creation failed
	core::dimension2du resolution =
#ifdef _IRR_COMPILE_WITH_SAILFISH_DEVICE_
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
#ifdef _IRR_COMPILE_WITH_SAILFISH_DEVICE_
	receiver->setSailfishDevice( reinterpret_cast<irr::CIrrDeviceSailfish*>(device) );
	device->getCursorControl()->setVisible(false);
	resolution.Width = device->getVideoDriver()->getScreenSize().Height;
	resolution.Height = device->getVideoDriver()->getScreenSize().Width;
#else
	device->getCursorControl()->setVisible(true);
#endif
	device->setEventReceiver(receiver);

	create_ui(env, resolution);
	
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
    scene::ICameraSceneNode* fpsCamera = 0;
	ScreenNode *screenNode = new ScreenNode(smgr->getRootSceneNode(), smgr, -1);
	receiver->setScreenShader(screenNode->getShader());
#ifdef _IRR_COMPILE_WITH_SAILFISH_DEVICE_
//	resolution = dynamic_cast<irr::CIrrDeviceSailfish*>(device)->getScreenResolution();
	resolution.Height = driver->getScreenSize().Width;
	resolution.Width = driver->getScreenSize().Height;
//	resolution = core::dimension2du(resolution.Height/**0.15*/, resolution.Width/**0.15*/);
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
        fpsCamera = smgr->addCameraSceneNode(0, core::vector3df(0,35,15));
        fpsCamera->setFarValue(300.0f);
        fpsCamera->setTarget( fairy->getPosition() + core::vector3df(0,10,0) );
#else
		fpsCamera = smgr->addCameraSceneNodeFPS(0,170.0f,0.15f);
//        fpsCamera->setFarValue(300.0f);
        fpsCamera->setPosition(core::vector3df(0,80,-35));

//        fpsCamera->setTarget( fairy->getPosition() + core::vector3df(0,70,-60) );
#endif
		receiver->fpsCamera = fpsCamera;
		//fixedCam->setNearValue(20.0f);

		fixCamera(fpsCamera, resolution);
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
    scene::ICameraSceneNode* screenCamera = smgr->addCameraSceneNode(0, core::vector3df(0,0,-1) );
//	fpsCamera->setPosition(core::vector3df(-50,50,-150));
    screenCamera->setTarget(screenNode->getPosition());
	fixCamera(screenCamera, resolution);

//	io::path fontPath = mediaPath + "bigfont.png";
	//gui::IGUISkin* skin = env->getSkin();
//	gui::IGUIFont *font = env->getFont(fontPath);
//	env->getSkin()->setFont(font);
	gui::IGUIStaticText *text = env->addStaticText( L"HelloWorld", core::recti(5,5,resolution.Width,60));
	/*
	Nearly finished. Now we need to draw everything. Every frame, we draw
	the scene twice. Once from the fixed camera into the render target
	texture and once as usual. When rendering into the render target, we
	need to disable the visibility of the test cube, because it has the
	render target texture applied to it. That's it, wasn't too complicated
	I hope. :)
	*/

	load_q3map("", device/*, fpsCamera*/);

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
            smgr->setActiveCamera(fpsCamera);

			// draw whole scene into render buffer
			smgr->drawAll();
			env->drawAll();
			// set back old render target
			// The buffer might have been distorted, so clear it
			driver->setRenderTargetEx(0, 0, video::SColor(0));

			// make the cube visible and set the user controlled camera as active one
			//test->setVisible(true);
            smgr->setActiveCamera(screenCamera);
		}
		
		screenNode->draw(driver);
//		env->drawAll();
//		smgr->drawAll();
//		driver->drawMeshBuffer();


		driver->endScene();

		// display frames per second in window title
		int fps = driver->getFPS();
		if (lastFPS != fps)
		{
			core::stringw str = L"FPS:";
			str += fps;
			str += "\n Touch: ";

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
