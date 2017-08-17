#include "irrapp.h"
#include "driverChoice.h"
#include "exampleHelper.h"


using namespace irr;
#include <QTimer>
#include <QObject>

IrrApp::IrrApp(QObject *parent) : QObject(parent)
{
	// ask user for driver
	video::E_DRIVER_TYPE driverType=video::EDT_OGLES2;//driverChoiceConsole();
	if (driverType==video::EDT_COUNT)
		return;

	// create device

	device = createDevice(driverType, core::dimension2d<u32>(640, 480), 16, false);

	if (device == 0)
		return; // could not create selected driver.

	const bool separateMeshBuffers = true;

	driver = device->getVideoDriver();
	smgr = device->getSceneManager();

	const io::path mediaPath = getExampleMediaPath();

	device->getFileSystem()->addFileArchive(mediaPath + "map-20kdm2.pk3");

	scene::IAnimatedMesh* q3levelmesh = smgr->getMesh("20kdm2.bsp");
	scene::IMeshSceneNode* q3node = 0;

	// The Quake mesh is pickable, but doesn't get highlighted.
	if (q3levelmesh)
		q3node = smgr->addOctreeSceneNode(q3levelmesh->getMesh(0), 0, IDFlag_IsPickable);

	scene::ITriangleSelector* selector = 0;

	if (q3node)
	{
		q3node->setPosition(core::vector3df(-1350,-130,-1400));

		if ( separateMeshBuffers && q3node->getMesh()->getMeshBufferCount() > 1)
		{
			scene::IMetaTriangleSelector * metaSelector = smgr->createMetaTriangleSelector();
			for ( irr::u32 m=0; m < q3node->getMesh()->getMeshBufferCount(); ++m )
			{
				scene::ITriangleSelector*
				    bufferSelector = smgr->createOctreeTriangleSelector(
				    q3node->getMesh()->getMeshBuffer(m), m, q3node);
				if ( bufferSelector )
				{
					metaSelector->addTriangleSelector( bufferSelector );
					bufferSelector->drop();
				}
			}
			selector = metaSelector;
		}
		else
		{
			// If you don't need material infos just create one octree for the
			// whole mesh.
			selector = smgr->createOctreeTriangleSelector(
			        q3node->getMesh(), q3node, 128);
		}
		q3node->setTriangleSelector(selector);
		// We're not done with this selector yet, so don't drop it.
	}

	// when used with the gravity of (0, -1000, 0) in the collision response animator.
	camera =
	    smgr->addCameraSceneNodeFPS(0, 100.0f, .3f, ID_IsNotPickable, 0, 0, true, 300.f);
	camera->setPosition(core::vector3df(50,50,-90));
	camera->setTarget(core::vector3df(-70,30,-60));

	if (selector)
	{
		scene::ISceneNodeAnimatorCollisionResponse * anim = smgr->createCollisionResponseAnimator(
		    selector, camera, core::vector3df(30,50,30),
		    core::vector3df(0,-1000,0), core::vector3df(0,30,0));
		selector->drop(); // As soon as we're done with the selector, drop it.
		camera->addAnimator(anim);
		anim->drop();  // And likewise, drop the animator when we're done referring to it.
	}

	device->getCursorControl()->setVisible(false);

	// Add the billboard.
	bill = smgr->addBillboardSceneNode();
	bill->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR );
	bill->setMaterialTexture(0, driver->getTexture(mediaPath + "particle.bmp"));
	bill->setMaterialFlag(video::EMF_LIGHTING, false);
	bill->setMaterialFlag(video::EMF_ZBUFFER, false);
	bill->setSize(core::dimension2d<f32>(20.0f, 20.0f));
	bill->setID(ID_IsNotPickable); // This ensures that we don't accidentally ray-pick it

	scene::IAnimatedMeshSceneNode* node = 0;

	// Add an MD2 node, which uses vertex-based animation.
	node = smgr->addAnimatedMeshSceneNode(smgr->getMesh(mediaPath + "faerie.md2"),
	                    0, IDFlag_IsPickable | IDFlag_IsHighlightable);
	node->setPosition(core::vector3df(-90,-15,-140)); // Put its feet on the floor.
	node->setScale(core::vector3df(1.6f)); // Make it appear realistically scaled
	node->setMD2Animation(scene::EMAT_POINT);
	node->setAnimationSpeed(20.f);
	material.setTexture(0, driver->getTexture(mediaPath + "faerie2.bmp"));
	material.Lighting = true;
	material.NormalizeNormals = true;
	node->getMaterial(0) = material;

	// Now create a triangle selector for it.  The selector will know that it
	// is associated with an animated node, and will update itself as necessary.
	selector = smgr->createTriangleSelector(node, separateMeshBuffers);
	node->setTriangleSelector(selector);
	selector->drop(); // We're done with this selector, so drop it now.

	// And this B3D file uses skinned skeletal animation.
	node = smgr->addAnimatedMeshSceneNode(smgr->getMesh(mediaPath + "ninja.b3d"),
	                    0, IDFlag_IsPickable | IDFlag_IsHighlightable);
	node->setScale(core::vector3df(10));
	node->setPosition(core::vector3df(-75,-66,-80));
	node->setRotation(core::vector3df(0,90,0));
	node->setAnimationSpeed(8.f);
	node->getMaterial(0).NormalizeNormals = true;
	node->getMaterial(0).Lighting = true;
	// Just do the same as we did above.
	selector = smgr->createTriangleSelector(node, separateMeshBuffers);
	node->setTriangleSelector(selector);
	selector->drop();

	// This X files uses skeletal animation, but without skinning.
	node = smgr->addAnimatedMeshSceneNode(smgr->getMesh(mediaPath + "dwarf.x"),
	                    0, IDFlag_IsPickable | IDFlag_IsHighlightable);
	node->setPosition(core::vector3df(-70,-66,-30)); // Put its feet on the floor.
	node->setRotation(core::vector3df(0,-90,0)); // And turn it towards the camera.
	node->setAnimationSpeed(20.f);
	node->getMaterial(0).Lighting = true;
	selector = smgr->createTriangleSelector(node, separateMeshBuffers);
	node->setTriangleSelector(selector);
	selector->drop();

	// And this mdl file uses skinned skeletal animation.
	node = smgr->addAnimatedMeshSceneNode(smgr->getMesh(mediaPath + "yodan.mdl"),
	                    0, IDFlag_IsPickable | IDFlag_IsHighlightable);
	node->setPosition(core::vector3df(-90,-25,20));
	node->setScale(core::vector3df(0.8f));
	node->getMaterial(0).Lighting = true;
	node->setAnimationSpeed(20.f);

	// Just do the same as we did above.
	selector = smgr->createTriangleSelector(node, separateMeshBuffers);
	node->setTriangleSelector(selector);
	selector->drop();

	material.setTexture(0, 0);
	material.Lighting = false;

	// Add a light, so that the unselected nodes aren't completely dark.
	scene::ILightSceneNode * light = smgr->addLightSceneNode(0, core::vector3df(-60,100,400),
	    video::SColorf(1.0f,1.0f,1.0f,1.0f), 600.0f);
	light->setID(ID_IsNotPickable); // Make it an invalid target for selection.

	// Remember which scene node is highlighted
	highlightedSceneNode = 0;
	collMan = smgr->getSceneCollisionManager();

	// draw the selection triangle only as wireframe
	material.Wireframe=true;

	timer = new QTimer(this);
	timer->setInterval(0);
	timer->setSingleShot(false);
	timer->start();
	connect(timer, SIGNAL(timeout()), SLOT(onTimer()), Qt::DirectConnection );
}

IrrApp::~IrrApp()
{
	delete timer;
	device->drop();
}

void IrrApp::onTimer()
{
	if(device->run())
	{
		driver->beginScene(video::ECBF_COLOR | video::ECBF_DEPTH, video::SColor(0));
		smgr->drawAll();

		// Unlight any currently highlighted scene node
		if (highlightedSceneNode)
		{
			highlightedSceneNode->setMaterialFlag(video::EMF_LIGHTING, true);
			highlightedSceneNode = 0;
		}

		core::line3d<f32> ray;
		ray.start = camera->getPosition();
		ray.end = ray.start + (camera->getTarget() - ray.start).normalize() * 1000.0f;

		irr::io::SNamedPath hitTextureName;
		scene::SCollisionHit hitResult;
		scene::ISceneNode * selectedSceneNode =collMan->getSceneNodeAndCollisionPointFromRay(
		            hitResult,	// Returns all kind of info about the collision
		            ray,
		            IDFlag_IsPickable, // This ensures that only nodes that we have
		            // set up to be pickable are considered
		            0); // Check the entire scene (this is actually the implicit default)


		// If the ray hit anything, move the billboard to the collision position
		// and draw the triangle that was hit.
		if(selectedSceneNode)
		{
			bill->setPosition(hitResult.Intersection);	// Show the current intersection point with the level or a mesh

			// We need to reset the transform before doing our own rendering.
			driver->setTransform(video::ETS_WORLD, core::matrix4());
			driver->setMaterial(material);
			driver->draw3DTriangle(hitResult.Triangle, video::SColor(0,255,0,0));	// Show which triangle has been hit

			// We can check the flags for the scene node that was hit to see if it should be
			// highlighted. The animated nodes can be highlighted, but not the Quake level mesh
			if((selectedSceneNode->getID() & IDFlag_IsHighlightable) == IDFlag_IsHighlightable)
			{
				highlightedSceneNode = selectedSceneNode;

				// Highlighting in this case means turning lighting OFF for this node,
				// which means that it will be drawn with full brightness.
				highlightedSceneNode->setMaterialFlag(video::EMF_LIGHTING, false);
			}

			// When separateMeshBuffers is set to true we can now find out which material was hit
			if ( hitResult.MeshBuffer && hitResult.Node && hitResult.Node->getMaterial(hitResult.MaterialIndex).TextureLayer[0].Texture )
			{
				// Note we are interested in the node material and not in the meshbuffer material.
				// Otherwise we wouldn't get the fairy2 texture which is only set on the node.
				hitTextureName = hitResult.Node->getMaterial(hitResult.MaterialIndex).TextureLayer[0].Texture->getName();
			}
		}

		// We're all done drawing, so end the scene.
		driver->endScene();

		// Show some info in title-bar
		int fps = driver->getFPS();
		static core::stringw lastString;
		core::stringw str = L"Collision detection example - Irrlicht Engine [";
		str += driver->getName();
		str += "] FPS:";
		str += fps;
		if ( !hitTextureName.getInternalName().empty() )
		{
			str += " ";
			irr::io::path texName(hitTextureName.getInternalName());
			str += core::deletePathFromFilename(texName);
		}
		if ( str != lastString )	// changing caption is somewhat expensive, so don't when nothing changed
		{
			device->setWindowCaption(str.c_str());
			lastString = str;
		}
	}
	timer->start();
}
