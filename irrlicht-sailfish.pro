TEMPLATE = lib

CONFIG += link_pkgconfig warn_off sailfishapp

TARGET = irrlicht
DESTDIR = lib

PKGCONFIG += wayland-client wayland-egl zlib libpng egl
LIBS += -ljpeg -lbz2
DEFINES += SAILFISH

INCLUDEPATH += include
INCLUDEPATH += source/Irrlicht

removespec.target = rpm/irrlicht.spec
removespec.command = rm -f rpm/irrlicht.spec
QMAKE_EXTRA_TARGETS += removespec

HEADERS += \
    include/aabbox3d.h \
    include/CDynamicMeshBuffer.h \
    include/CIndexBuffer.h \
    include/CMeshBuffer.h \
    include/coreutil.h \
    include/CVertexBuffer.h \
    include/dimension2d.h \
    include/driverChoice.h \
    include/EAttributes.h \
    include/ECullingTypes.h \
    include/EDebugSceneTypes.h \
    include/EDeviceTypes.h \
    include/EDriverFeatures.h \
    include/EDriverTypes.h \
    include/EFocusFlags.h \
    include/EGUIAlignment.h \
    include/EGUIElementTypes.h \
    include/EHardwareBufferFlags.h \
    include/EMaterialFlags.h \
    include/EMaterialTypes.h \
    include/EMeshWriterEnums.h \
    include/EMessageBoxFlags.h \
    include/EPrimitiveTypes.h \
    include/ESceneNodeAnimatorTypes.h \
    include/ESceneNodeTypes.h \
    include/EShaderTypes.h \
    include/ETerrainElements.h \
    include/EVertexAttributes.h \
    include/exampleHelper.h \
    include/fast_atof.h \
    include/heapsort.h \
    include/IAnimatedMesh.h \
    include/IAnimatedMeshMD2.h \
    include/IAnimatedMeshMD3.h \
    include/IAnimatedMeshSceneNode.h \
    include/IAttributeExchangingObject.h \
    include/IAttributes.h \
    include/IBillboardSceneNode.h \
    include/IBillboardTextSceneNode.h \
    include/IBoneSceneNode.h \
    include/ICameraSceneNode.h \
    include/IColladaMeshWriter.h \
    include/IContextManager.h \
    include/ICursorControl.h \
    include/IDummyTransformationSceneNode.h \
    include/IDynamicMeshBuffer.h \
    include/IEventReceiver.h \
    include/IFileArchive.h \
    include/IFileList.h \
    include/IFileSystem.h \
    include/IGeometryCreator.h \
    include/IGPUProgrammingServices.h \
    include/IGUIButton.h \
    include/IGUICheckBox.h \
    include/IGUIColorSelectDialog.h \
    include/IGUIComboBox.h \
    include/IGUIContextMenu.h \
    include/IGUIEditBox.h \
    include/IGUIElement.h \
    include/IGUIElementFactory.h \
    include/IGUIEnvironment.h \
    include/IGUIFileOpenDialog.h \
    include/IGUIFont.h \
    include/IGUIFontBitmap.h \
    include/IGUIImage.h \
    include/IGUIImageList.h \
    include/IGUIInOutFader.h \
    include/IGUIListBox.h \
    include/IGUIMeshViewer.h \
    include/IGUIProfiler.h \
    include/IGUIScrollBar.h \
    include/IGUISkin.h \
    include/IGUISpinBox.h \
    include/IGUISpriteBank.h \
    include/IGUIStaticText.h \
    include/IGUITabControl.h \
    include/IGUITable.h \
    include/IGUIToolbar.h \
    include/IGUITreeView.h \
    include/IGUIWindow.h \
    include/IImage.h \
    include/IImageLoader.h \
    include/IImageWriter.h \
    include/IIndexBuffer.h \
    include/ILightManager.h \
    include/ILightSceneNode.h \
    include/ILogger.h \
    include/IMaterialRenderer.h \
    include/IMaterialRendererServices.h \
    include/IMesh.h \
    include/IMeshBuffer.h \
    include/IMeshCache.h \
    include/IMeshLoader.h \
    include/IMeshManipulator.h \
    include/IMeshSceneNode.h \
    include/IMeshTextureLoader.h \
    include/IMeshWriter.h \
    include/IMetaTriangleSelector.h \
    include/IOctreeSceneNode.h \
    include/IOSOperator.h \
    include/IParticleAffector.h \
    include/IParticleAnimatedMeshSceneNodeEmitter.h \
    include/IParticleAttractionAffector.h \
    include/IParticleBoxEmitter.h \
    include/IParticleCylinderEmitter.h \
    include/IParticleEmitter.h \
    include/IParticleFadeOutAffector.h \
    include/IParticleGravityAffector.h \
    include/IParticleMeshEmitter.h \
    include/IParticleRingEmitter.h \
    include/IParticleRotationAffector.h \
    include/IParticleSphereEmitter.h \
    include/IParticleSystemSceneNode.h \
    include/IProfiler.h \
    include/IQ3LevelMesh.h \
    include/IQ3Shader.h \
    include/IRandomizer.h \
    include/IReadFile.h \
    include/IReferenceCounted.h \
    include/IRenderTarget.h \
    include/irrAllocator.h \
    include/irrArray.h \
    include/IrrCompileConfig.h \
    include/irrlicht.h \
    include/IrrlichtDevice.h \
    include/irrList.h \
    include/irrMap.h \
    include/irrMath.h \
    include/irrpack.h \
    include/irrString.h \
    include/irrTypes.h \
    include/irrunpack.h \
    include/irrXML.h \
    include/ISceneCollisionManager.h \
    include/ISceneLoader.h \
    include/ISceneManager.h \
    include/ISceneNode.h \
    include/ISceneNodeAnimator.h \
    include/ISceneNodeAnimatorCameraFPS.h \
    include/ISceneNodeAnimatorCameraMaya.h \
    include/ISceneNodeAnimatorCollisionResponse.h \
    include/ISceneNodeAnimatorFactory.h \
    include/ISceneNodeFactory.h \
    include/ISceneUserDataSerializer.h \
    include/IShaderConstantSetCallBack.h \
    include/IShadowVolumeSceneNode.h \
    include/ISkinnedMesh.h \
    include/ITerrainSceneNode.h \
    include/ITextSceneNode.h \
    include/ITexture.h \
    include/ITimer.h \
    include/ITriangleSelector.h \
    include/IVertexBuffer.h \
    include/IVideoDriver.h \
    include/IVideoModeList.h \
    include/IVolumeLightSceneNode.h \
    include/IWriteFile.h \
    include/IXMLReader.h \
    include/IXMLWriter.h \
    include/Keycodes.h \
    include/leakHunter.h \
    include/line2d.h \
    include/line3d.h \
    include/matrix4.h \
    include/path.h \
    include/plane3d.h \
    include/position2d.h \
    include/quaternion.h \
    include/rect.h \
    include/S3DVertex.h \
    include/SAnimatedMesh.h \
    include/SceneParameters.h \
    include/SColor.h \
    include/SExposedVideoData.h \
    include/SIrrCreationParameters.h \
    include/SKeyMap.h \
    include/SLight.h \
    include/SMaterial.h \
    include/SMaterialLayer.h \
    include/SMesh.h \
    include/SMeshBuffer.h \
    include/SMeshBufferLightMap.h \
    include/SMeshBufferTangents.h \
    include/SParticle.h \
    include/SSharedMeshBuffer.h \
    include/SSkinMeshBuffer.h \
    include/SVertexIndex.h \
    include/SVertexManipulator.h \
    include/SViewFrustum.h \
    include/triangle3d.h \
    include/vector2d.h \
    include/vector3d.h \
    source/Irrlicht/aesGladman/aes.h \
    source/Irrlicht/aesGladman/aesopt.h \
    source/Irrlicht/aesGladman/fileenc.h \
    source/Irrlicht/aesGladman/hmac.h \
    source/Irrlicht/aesGladman/prng.h \
    source/Irrlicht/aesGladman/pwd2key.h \
    source/Irrlicht/aesGladman/sha1.h \
    source/Irrlicht/aesGladman/sha2.h \
    source/Irrlicht/lzma/LzmaDec.h \
    source/Irrlicht/lzma/Types.h \
    source/Irrlicht/BuiltInFont.h \
    source/Irrlicht/C3DSMeshFileLoader.h \
    source/Irrlicht/CAnimatedMeshHalfLife.h \
    source/Irrlicht/CAnimatedMeshMD2.h \
    source/Irrlicht/CAnimatedMeshMD3.h \
    source/Irrlicht/CAnimatedMeshSceneNode.h \
    source/Irrlicht/CAttributeImpl.h \
    source/Irrlicht/CAttributes.h \
    source/Irrlicht/CB3DMeshFileLoader.h \
    source/Irrlicht/CB3DMeshWriter.h \
    source/Irrlicht/CBillboardSceneNode.h \
    source/Irrlicht/CBlit.h \
    source/Irrlicht/CBoneSceneNode.h \
    source/Irrlicht/CBSPMeshFileLoader.h \
    source/Irrlicht/CCameraSceneNode.h \
    source/Irrlicht/CColladaFileLoader.h \
    source/Irrlicht/CColladaMeshWriter.h \
    source/Irrlicht/CColorConverter.h \
    source/Irrlicht/CCSMLoader.h \
    source/Irrlicht/CCubeSceneNode.h \
    source/Irrlicht/CD3D9Driver.h \
    source/Irrlicht/CD3D9HLSLMaterialRenderer.h \
    source/Irrlicht/CD3D9MaterialRenderer.h \
    source/Irrlicht/CD3D9NormalMapRenderer.h \
    source/Irrlicht/CD3D9ParallaxMapRenderer.h \
    source/Irrlicht/CD3D9RenderTarget.h \
    source/Irrlicht/CD3D9ShaderMaterialRenderer.h \
    source/Irrlicht/CD3D9Texture.h \
    source/Irrlicht/CDefaultGUIElementFactory.h \
    source/Irrlicht/CDefaultSceneNodeAnimatorFactory.h \
    source/Irrlicht/CDefaultSceneNodeFactory.h \
    source/Irrlicht/CDepthBuffer.h \
    source/Irrlicht/CDMFLoader.h \
    source/Irrlicht/CDummyTransformationSceneNode.h \
    source/Irrlicht/CEAGLManager.h \
    source/Irrlicht/CEGLManager.h \
    source/Irrlicht/CEmptySceneNode.h \
    source/Irrlicht/CFileList.h \
    source/Irrlicht/CFileSystem.h \
    source/Irrlicht/CFPSCounter.h \
    source/Irrlicht/CGeometryCreator.h \
    source/Irrlicht/CGLXManager.h \
    source/Irrlicht/CGUIButton.h \
    source/Irrlicht/CGUICheckBox.h \
    source/Irrlicht/CGUIColorSelectDialog.h \
    source/Irrlicht/CGUIComboBox.h \
    source/Irrlicht/CGUIContextMenu.h \
    source/Irrlicht/CGUIEditBox.h \
    source/Irrlicht/CGUIEnvironment.h \
    source/Irrlicht/CGUIFileOpenDialog.h \
    source/Irrlicht/CGUIFont.h \
    source/Irrlicht/CGUIImage.h \
    source/Irrlicht/CGUIImageList.h \
    source/Irrlicht/CGUIInOutFader.h \
    source/Irrlicht/CGUIListBox.h \
    source/Irrlicht/CGUIMenu.h \
    source/Irrlicht/CGUIMeshViewer.h \
    source/Irrlicht/CGUIMessageBox.h \
    source/Irrlicht/CGUIModalScreen.h \
    source/Irrlicht/CGUIProfiler.h \
    source/Irrlicht/CGUIScrollBar.h \
    source/Irrlicht/CGUISkin.h \
    source/Irrlicht/CGUISpinBox.h \
    source/Irrlicht/CGUISpriteBank.h \
    source/Irrlicht/CGUIStaticText.h \
    source/Irrlicht/CGUITabControl.h \
    source/Irrlicht/CGUITable.h \
    source/Irrlicht/CGUIToolBar.h \
    source/Irrlicht/CGUITreeView.h \
    source/Irrlicht/CGUIWindow.h \
    source/Irrlicht/CImage.h \
    source/Irrlicht/CImageLoaderBMP.h \
    source/Irrlicht/CImageLoaderDDS.h \
    source/Irrlicht/CImageLoaderJPG.h \
    source/Irrlicht/CImageLoaderPCX.h \
    source/Irrlicht/CImageLoaderPNG.h \
    source/Irrlicht/CImageLoaderPPM.h \
    source/Irrlicht/CImageLoaderPSD.h \
    source/Irrlicht/CImageLoaderPVR.h \
    source/Irrlicht/CImageLoaderRGB.h \
    source/Irrlicht/CImageLoaderTGA.h \
    source/Irrlicht/CImageLoaderWAL.h \
    source/Irrlicht/CImageWriterBMP.h \
    source/Irrlicht/CImageWriterJPG.h \
    source/Irrlicht/CImageWriterPCX.h \
    source/Irrlicht/CImageWriterPNG.h \
    source/Irrlicht/CImageWriterPPM.h \
    source/Irrlicht/CImageWriterPSD.h \
    source/Irrlicht/CImageWriterTGA.h \
    source/Irrlicht/CIrrDeviceConsole.h \
    source/Irrlicht/CIrrDeviceFB.h \
    source/Irrlicht/CIrrDeviceiOS.h \
    source/Irrlicht/CIrrDeviceLinux.h \
    source/Irrlicht/CIrrDeviceOSX.h \
    source/Irrlicht/CIrrDeviceSDL.h \
    source/Irrlicht/CIrrDeviceStub.h \
    source/Irrlicht/CIrrDeviceWin32.h \
    source/Irrlicht/CIrrMeshFileLoader.h \
    source/Irrlicht/CIrrMeshWriter.h \
    source/Irrlicht/CLightSceneNode.h \
    source/Irrlicht/CLimitReadFile.h \
    source/Irrlicht/CLMTSMeshFileLoader.h \
    source/Irrlicht/CLogger.h \
    source/Irrlicht/CLWOMeshFileLoader.h \
    source/Irrlicht/CMD2MeshFileLoader.h \
    source/Irrlicht/CMD3MeshFileLoader.h \
    source/Irrlicht/CMemoryFile.h \
    source/Irrlicht/CMeshCache.h \
    source/Irrlicht/CMeshManipulator.h \
    source/Irrlicht/CMeshSceneNode.h \
    source/Irrlicht/CMeshTextureLoader.h \
    source/Irrlicht/CMetaTriangleSelector.h \
    source/Irrlicht/CMountPointReader.h \
    source/Irrlicht/CMS3DMeshFileLoader.h \
    source/Irrlicht/CMY3DHelper.h \
    source/Irrlicht/CMY3DMeshFileLoader.h \
    source/Irrlicht/CNPKReader.h \
    source/Irrlicht/CNSOGLManager.h \
    source/Irrlicht/CNullDriver.h \
    source/Irrlicht/COBJMeshFileLoader.h \
    source/Irrlicht/COBJMeshWriter.h \
    source/Irrlicht/COCTLoader.h \
    source/Irrlicht/COctreeSceneNode.h \
    source/Irrlicht/COctreeTriangleSelector.h \
    source/Irrlicht/COGLES2Common.h \
    source/Irrlicht/COGLES2Driver.h \
    source/Irrlicht/COGLES2ExtensionHandler.h \
    source/Irrlicht/COGLES2FixedPipelineRenderer.h \
    source/Irrlicht/COGLES2MaterialRenderer.h \
    source/Irrlicht/COGLES2NormalMapRenderer.h \
    source/Irrlicht/COGLES2ParallaxMapRenderer.h \
    source/Irrlicht/COGLES2Renderer2D.h \
    source/Irrlicht/COGLESCommon.h \
    source/Irrlicht/COGLESDriver.h \
    source/Irrlicht/COGLESExtensionHandler.h \
    source/Irrlicht/COGLESMaterialRenderer.h \
    source/Irrlicht/COgreMeshFileLoader.h \
    source/Irrlicht/COpenGLCacheHandler.h \
    source/Irrlicht/COpenGLCommon.h \
    source/Irrlicht/COpenGLCoreCacheHandler.h \
    source/Irrlicht/COpenGLCoreFeature.h \
    source/Irrlicht/COpenGLCoreRenderTarget.h \
    source/Irrlicht/COpenGLCoreTexture.h \
    source/Irrlicht/COpenGLDriver.h \
    source/Irrlicht/COpenGLExtensionHandler.h \
    source/Irrlicht/COpenGLMaterialRenderer.h \
    source/Irrlicht/COpenGLNormalMapRenderer.h \
    source/Irrlicht/COpenGLParallaxMapRenderer.h \
    source/Irrlicht/COpenGLShaderMaterialRenderer.h \
    source/Irrlicht/COpenGLSLMaterialRenderer.h \
    source/Irrlicht/COSOperator.h \
    source/Irrlicht/CPakReader.h \
    source/Irrlicht/CParticleAnimatedMeshSceneNodeEmitter.h \
    source/Irrlicht/CParticleAttractionAffector.h \
    source/Irrlicht/CParticleBoxEmitter.h \
    source/Irrlicht/CParticleCylinderEmitter.h \
    source/Irrlicht/CParticleFadeOutAffector.h \
    source/Irrlicht/CParticleGravityAffector.h \
    source/Irrlicht/CParticleMeshEmitter.h \
    source/Irrlicht/CParticlePointEmitter.h \
    source/Irrlicht/CParticleRingEmitter.h \
    source/Irrlicht/CParticleRotationAffector.h \
    source/Irrlicht/CParticleScaleAffector.h \
    source/Irrlicht/CParticleSphereEmitter.h \
    source/Irrlicht/CParticleSystemSceneNode.h \
    source/Irrlicht/CPLYMeshFileLoader.h \
    source/Irrlicht/CPLYMeshWriter.h \
    source/Irrlicht/CProfiler.h \
    source/Irrlicht/CQ3LevelMesh.h \
    source/Irrlicht/CQuake3ShaderSceneNode.h \
    source/Irrlicht/CReadFile.h \
    source/Irrlicht/CSceneCollisionManager.h \
    source/Irrlicht/CSceneLoaderIrr.h \
    source/Irrlicht/CSceneManager.h \
    source/Irrlicht/CSceneNodeAnimatorCameraFPS.h \
    source/Irrlicht/CSceneNodeAnimatorCameraMaya.h \
    source/Irrlicht/CSceneNodeAnimatorCollisionResponse.h \
    source/Irrlicht/CSceneNodeAnimatorDelete.h \
    source/Irrlicht/CSceneNodeAnimatorFlyCircle.h \
    source/Irrlicht/CSceneNodeAnimatorFlyStraight.h \
    source/Irrlicht/CSceneNodeAnimatorFollowSpline.h \
    source/Irrlicht/CSceneNodeAnimatorRotation.h \
    source/Irrlicht/CSceneNodeAnimatorTexture.h \
    source/Irrlicht/CShadowVolumeSceneNode.h \
    source/Irrlicht/CSkinnedMesh.h \
    source/Irrlicht/CSkyBoxSceneNode.h \
    source/Irrlicht/CSkyDomeSceneNode.h \
    source/Irrlicht/CSMFMeshFileLoader.h \
    source/Irrlicht/CSoftware2MaterialRenderer.h \
    source/Irrlicht/CSoftwareDriver.h \
    source/Irrlicht/CSoftwareDriver2.h \
    source/Irrlicht/CSoftwareTexture.h \
    source/Irrlicht/CSoftwareTexture2.h \
    source/Irrlicht/CSphereSceneNode.h \
    source/Irrlicht/CSTLMeshFileLoader.h \
    source/Irrlicht/CSTLMeshWriter.h \
    source/Irrlicht/CTarReader.h \
    source/Irrlicht/CTerrainSceneNode.h \
    source/Irrlicht/CTerrainTriangleSelector.h \
    source/Irrlicht/CTextSceneNode.h \
    source/Irrlicht/CTimer.h \
    source/Irrlicht/CTriangleBBSelector.h \
    source/Irrlicht/CTriangleSelector.h \
    source/Irrlicht/CTRTextureGouraud.h \
    source/Irrlicht/CVideoModeList.h \
    source/Irrlicht/CVolumeLightSceneNode.h \
    source/Irrlicht/CWADReader.h \
    source/Irrlicht/CWaterSurfaceSceneNode.h \
    source/Irrlicht/CWebGL1Driver.h \
    source/Irrlicht/CWGLManager.h \
    source/Irrlicht/CWriteFile.h \
    source/Irrlicht/CXMeshFileLoader.h \
    source/Irrlicht/CXMLReader.h \
    source/Irrlicht/CXMLReaderImpl.h \
    source/Irrlicht/CXMLWriter.h \
    source/Irrlicht/CZBuffer.h \
    source/Irrlicht/CZipReader.h \
    source/Irrlicht/dmfsupport.h \
    source/Irrlicht/eglext.h \
    source/Irrlicht/EProfileIDs.h \
    source/Irrlicht/gles-ext.h \
    source/Irrlicht/gles2-ext.h \
    source/Irrlicht/gles3-ext.h \
    source/Irrlicht/glext.h \
    source/Irrlicht/glxext.h \
    source/Irrlicht/IAttribute.h \
    source/Irrlicht/IBurningShader.h \
    source/Irrlicht/IDepthBuffer.h \
    source/Irrlicht/IImagePresenter.h \
    source/Irrlicht/Irrlicht.aps \
    source/Irrlicht/ISceneNodeAnimatorFinishing.h \
    source/Irrlicht/ITriangleRenderer.h \
    source/Irrlicht/IZBuffer.h \
    source/Irrlicht/Octree.h \
    source/Irrlicht/os.h \
    source/Irrlicht/resource.h \
    source/Irrlicht/S2DVertex.h \
    source/Irrlicht/S4DVertex.h \
    source/Irrlicht/SB3DStructs.h \
    source/Irrlicht/SoftwareDriver2_compile_config.h \
    source/Irrlicht/SoftwareDriver2_helper.h \
    source/Irrlicht/wglext.h
#    source/Irrlicht/jpeglib/cderror.h \
#    source/Irrlicht/jpeglib/cdjpeg.h \
#    source/Irrlicht/jpeglib/jconfig.h \
#    source/Irrlicht/jpeglib/jdct.h \
#    source/Irrlicht/jpeglib/jerror.h \
#    source/Irrlicht/jpeglib/jinclude.h \
#    source/Irrlicht/jpeglib/jmemsys.h \
#    source/Irrlicht/jpeglib/jmorecfg.h \
#    source/Irrlicht/jpeglib/jpegint.h \
#    source/Irrlicht/jpeglib/jpeglib.h \
#    source/Irrlicht/jpeglib/jversion.h \
#    source/Irrlicht/jpeglib/transupp.h

SOURCES += \
    source/Irrlicht/aesGladman/aescrypt.cpp \
    source/Irrlicht/aesGladman/aeskey.cpp \
    source/Irrlicht/aesGladman/aestab.cpp \
    source/Irrlicht/aesGladman/fileenc.cpp \
    source/Irrlicht/aesGladman/hmac.cpp \
    source/Irrlicht/aesGladman/prng.cpp \
    source/Irrlicht/aesGladman/pwd2key.cpp \
    source/Irrlicht/aesGladman/sha1.cpp \
    source/Irrlicht/aesGladman/sha2.cpp \
    source/Irrlicht/C3DSMeshFileLoader.cpp \
    source/Irrlicht/CAnimatedMeshHalfLife.cpp \
    source/Irrlicht/CAnimatedMeshMD2.cpp \
    source/Irrlicht/CAnimatedMeshMD3.cpp \
    source/Irrlicht/CAnimatedMeshSceneNode.cpp \
    source/Irrlicht/CAttributes.cpp \
    source/Irrlicht/CB3DMeshFileLoader.cpp \
    source/Irrlicht/CB3DMeshWriter.cpp \
    source/Irrlicht/CBillboardSceneNode.cpp \
    source/Irrlicht/CBoneSceneNode.cpp \
    source/Irrlicht/CBSPMeshFileLoader.cpp \
    source/Irrlicht/CBurningShader_Raster_Reference.cpp \
    source/Irrlicht/CCameraSceneNode.cpp \
    source/Irrlicht/CColladaFileLoader.cpp \
    source/Irrlicht/CColladaMeshWriter.cpp \
    source/Irrlicht/CColorConverter.cpp \
    source/Irrlicht/CCSMLoader.cpp \
    source/Irrlicht/CCubeSceneNode.cpp \
#    source/Irrlicht/CD3D9Driver.cpp \
#    source/Irrlicht/CD3D9HLSLMaterialRenderer.cpp \
#    source/Irrlicht/CD3D9NormalMapRenderer.cpp \
#    source/Irrlicht/CD3D9ParallaxMapRenderer.cpp \
#    source/Irrlicht/CD3D9RenderTarget.cpp \
#    source/Irrlicht/CD3D9ShaderMaterialRenderer.cpp \
#    source/Irrlicht/CD3D9Texture.cpp \
    source/Irrlicht/CDefaultGUIElementFactory.cpp \
    source/Irrlicht/CDefaultSceneNodeAnimatorFactory.cpp \
    source/Irrlicht/CDefaultSceneNodeFactory.cpp \
    source/Irrlicht/CDepthBuffer.cpp \
    source/Irrlicht/CDMFLoader.cpp \
    source/Irrlicht/CDummyTransformationSceneNode.cpp \
    source/Irrlicht/CEGLManager.cpp \
    source/Irrlicht/CEmptySceneNode.cpp \
    source/Irrlicht/CFileList.cpp \
    source/Irrlicht/CFileSystem.cpp \
    source/Irrlicht/CFPSCounter.cpp \
    source/Irrlicht/CGeometryCreator.cpp \
#    source/Irrlicht/CGLXManager.cpp \
    source/Irrlicht/CGUIButton.cpp \
    source/Irrlicht/CGUICheckBox.cpp \
    source/Irrlicht/CGUIColorSelectDialog.cpp \
    source/Irrlicht/CGUIComboBox.cpp \
    source/Irrlicht/CGUIContextMenu.cpp \
    source/Irrlicht/CGUIEditBox.cpp \
    source/Irrlicht/CGUIEnvironment.cpp \
    source/Irrlicht/CGUIFileOpenDialog.cpp \
    source/Irrlicht/CGUIFont.cpp \
    source/Irrlicht/CGUIImage.cpp \
    source/Irrlicht/CGUIImageList.cpp \
    source/Irrlicht/CGUIInOutFader.cpp \
    source/Irrlicht/CGUIListBox.cpp \
    source/Irrlicht/CGUIMenu.cpp \
    source/Irrlicht/CGUIMeshViewer.cpp \
    source/Irrlicht/CGUIMessageBox.cpp \
    source/Irrlicht/CGUIModalScreen.cpp \
    source/Irrlicht/CGUIProfiler.cpp \
    source/Irrlicht/CGUIScrollBar.cpp \
    source/Irrlicht/CGUISkin.cpp \
    source/Irrlicht/CGUISpinBox.cpp \
    source/Irrlicht/CGUISpriteBank.cpp \
    source/Irrlicht/CGUIStaticText.cpp \
    source/Irrlicht/CGUITabControl.cpp \
    source/Irrlicht/CGUITable.cpp \
    source/Irrlicht/CGUIToolBar.cpp \
    source/Irrlicht/CGUITreeView.cpp \
    source/Irrlicht/CGUIWindow.cpp \
    source/Irrlicht/CImage.cpp \
    source/Irrlicht/CImageLoaderBMP.cpp \
    source/Irrlicht/CImageLoaderDDS.cpp \
    source/Irrlicht/CImageLoaderJPG.cpp \
    source/Irrlicht/CImageLoaderPCX.cpp \
    source/Irrlicht/CImageLoaderPNG.cpp \
    source/Irrlicht/CImageLoaderPPM.cpp \
    source/Irrlicht/CImageLoaderPSD.cpp \
    source/Irrlicht/CImageLoaderPVR.cpp \
    source/Irrlicht/CImageLoaderRGB.cpp \
    source/Irrlicht/CImageLoaderTGA.cpp \
    source/Irrlicht/CImageLoaderWAL.cpp \
    source/Irrlicht/CImageWriterBMP.cpp \
    source/Irrlicht/CImageWriterJPG.cpp \
    source/Irrlicht/CImageWriterPCX.cpp \
    source/Irrlicht/CImageWriterPNG.cpp \
    source/Irrlicht/CImageWriterPPM.cpp \
    source/Irrlicht/CImageWriterPSD.cpp \
    source/Irrlicht/CImageWriterTGA.cpp \
    source/Irrlicht/CIrrDeviceConsole.cpp \
    source/Irrlicht/CIrrDeviceFB.cpp \
    source/Irrlicht/CIrrDeviceLinux.cpp \
    source/Irrlicht/CIrrDeviceSDL.cpp \
    source/Irrlicht/CIrrDeviceStub.cpp \
    source/Irrlicht/CIrrDeviceWin32.cpp \
    source/Irrlicht/CIrrMeshFileLoader.cpp \
    source/Irrlicht/CIrrMeshWriter.cpp \
    source/Irrlicht/CLightSceneNode.cpp \
    source/Irrlicht/CLimitReadFile.cpp \
    source/Irrlicht/CLMTSMeshFileLoader.cpp \
    source/Irrlicht/CLogger.cpp \
    source/Irrlicht/CLWOMeshFileLoader.cpp \
    source/Irrlicht/CMD2MeshFileLoader.cpp \
    source/Irrlicht/CMD3MeshFileLoader.cpp \
    source/Irrlicht/CMemoryFile.cpp \
    source/Irrlicht/CMeshCache.cpp \
    source/Irrlicht/CMeshManipulator.cpp \
    source/Irrlicht/CMeshSceneNode.cpp \
    source/Irrlicht/CMeshTextureLoader.cpp \
    source/Irrlicht/CMetaTriangleSelector.cpp \
    source/Irrlicht/CMountPointReader.cpp \
    source/Irrlicht/CMS3DMeshFileLoader.cpp \
    source/Irrlicht/CMY3DMeshFileLoader.cpp \
    source/Irrlicht/CNPKReader.cpp \
    source/Irrlicht/CNullDriver.cpp \
    source/Irrlicht/COBJMeshFileLoader.cpp \
    source/Irrlicht/COBJMeshWriter.cpp \
    source/Irrlicht/COCTLoader.cpp \
    source/Irrlicht/COctreeSceneNode.cpp \
    source/Irrlicht/COctreeTriangleSelector.cpp \
    source/Irrlicht/COGLES2Driver.cpp \
    source/Irrlicht/COGLES2ExtensionHandler.cpp \
    source/Irrlicht/COGLES2FixedPipelineRenderer.cpp \
    source/Irrlicht/COGLES2MaterialRenderer.cpp \
    source/Irrlicht/COGLES2NormalMapRenderer.cpp \
    source/Irrlicht/COGLES2ParallaxMapRenderer.cpp \
    source/Irrlicht/COGLES2Renderer2D.cpp \
    source/Irrlicht/COGLESDriver.cpp \
    source/Irrlicht/COGLESExtensionHandler.cpp \
    source/Irrlicht/COgreMeshFileLoader.cpp \
    source/Irrlicht/COpenGLCacheHandler.cpp \
    source/Irrlicht/COpenGLDriver.cpp \
    source/Irrlicht/COpenGLExtensionHandler.cpp \
    source/Irrlicht/COpenGLNormalMapRenderer.cpp \
    source/Irrlicht/COpenGLParallaxMapRenderer.cpp \
    source/Irrlicht/COpenGLShaderMaterialRenderer.cpp \
    source/Irrlicht/COpenGLSLMaterialRenderer.cpp \
    source/Irrlicht/COSOperator.cpp \
    source/Irrlicht/CPakReader.cpp \
    source/Irrlicht/CParticleAnimatedMeshSceneNodeEmitter.cpp \
    source/Irrlicht/CParticleAttractionAffector.cpp \
    source/Irrlicht/CParticleBoxEmitter.cpp \
    source/Irrlicht/CParticleCylinderEmitter.cpp \
    source/Irrlicht/CParticleFadeOutAffector.cpp \
    source/Irrlicht/CParticleGravityAffector.cpp \
    source/Irrlicht/CParticleMeshEmitter.cpp \
    source/Irrlicht/CParticlePointEmitter.cpp \
    source/Irrlicht/CParticleRingEmitter.cpp \
    source/Irrlicht/CParticleRotationAffector.cpp \
    source/Irrlicht/CParticleScaleAffector.cpp \
    source/Irrlicht/CParticleSphereEmitter.cpp \
    source/Irrlicht/CParticleSystemSceneNode.cpp \
    source/Irrlicht/CPLYMeshFileLoader.cpp \
    source/Irrlicht/CPLYMeshWriter.cpp \
    source/Irrlicht/CProfiler.cpp \
    source/Irrlicht/CQ3LevelMesh.cpp \
    source/Irrlicht/CQuake3ShaderSceneNode.cpp \
    source/Irrlicht/CReadFile.cpp \
    source/Irrlicht/CSceneCollisionManager.cpp \
    source/Irrlicht/CSceneLoaderIrr.cpp \
    source/Irrlicht/CSceneManager.cpp \
    source/Irrlicht/CSceneNodeAnimatorCameraFPS.cpp \
    source/Irrlicht/CSceneNodeAnimatorCameraMaya.cpp \
    source/Irrlicht/CSceneNodeAnimatorCollisionResponse.cpp \
    source/Irrlicht/CSceneNodeAnimatorDelete.cpp \
    source/Irrlicht/CSceneNodeAnimatorFlyCircle.cpp \
    source/Irrlicht/CSceneNodeAnimatorFlyStraight.cpp \
    source/Irrlicht/CSceneNodeAnimatorFollowSpline.cpp \
    source/Irrlicht/CSceneNodeAnimatorRotation.cpp \
    source/Irrlicht/CSceneNodeAnimatorTexture.cpp \
    source/Irrlicht/CShadowVolumeSceneNode.cpp \
    source/Irrlicht/CSkinnedMesh.cpp \
    source/Irrlicht/CSkyBoxSceneNode.cpp \
    source/Irrlicht/CSkyDomeSceneNode.cpp \
    source/Irrlicht/CSMFMeshFileLoader.cpp \
    source/Irrlicht/CSoftwareDriver.cpp \
    source/Irrlicht/CSoftwareDriver2.cpp \
    source/Irrlicht/CSoftwareTexture.cpp \
    source/Irrlicht/CSoftwareTexture2.cpp \
    source/Irrlicht/CSphereSceneNode.cpp \
    source/Irrlicht/CSTLMeshFileLoader.cpp \
    source/Irrlicht/CSTLMeshWriter.cpp \
    source/Irrlicht/CTarReader.cpp \
    source/Irrlicht/CTerrainSceneNode.cpp \
    source/Irrlicht/CTerrainTriangleSelector.cpp \
    source/Irrlicht/CTextSceneNode.cpp \
    source/Irrlicht/CTRFlat.cpp \
    source/Irrlicht/CTRFlatWire.cpp \
    source/Irrlicht/CTRGouraud.cpp \
    source/Irrlicht/CTRGouraud2.cpp \
    source/Irrlicht/CTRGouraudAlpha2.cpp \
    source/Irrlicht/CTRGouraudAlphaNoZ2.cpp \
    source/Irrlicht/CTRGouraudWire.cpp \
    source/Irrlicht/CTriangleBBSelector.cpp \
    source/Irrlicht/CTriangleSelector.cpp \
    source/Irrlicht/CTRNormalMap.cpp \
    source/Irrlicht/CTRStencilShadow.cpp \
    source/Irrlicht/CTRTextureBlend.cpp \
    source/Irrlicht/CTRTextureDetailMap2.cpp \
    source/Irrlicht/CTRTextureFlat.cpp \
    source/Irrlicht/CTRTextureFlatWire.cpp \
    source/Irrlicht/CTRTextureGouraud.cpp \
    source/Irrlicht/CTRTextureGouraud2.cpp \
    source/Irrlicht/CTRTextureGouraudAdd.cpp \
    source/Irrlicht/CTRTextureGouraudAdd2.cpp \
    source/Irrlicht/CTRTextureGouraudAddNoZ2.cpp \
    source/Irrlicht/CTRTextureGouraudAlpha.cpp \
    source/Irrlicht/CTRTextureGouraudAlphaNoZ.cpp \
    source/Irrlicht/CTRTextureGouraudNoZ.cpp \
    source/Irrlicht/CTRTextureGouraudNoZ2.cpp \
    source/Irrlicht/CTRTextureGouraudVertexAlpha2.cpp \
    source/Irrlicht/CTRTextureGouraudWire.cpp \
    source/Irrlicht/CTRTextureLightMap2_Add.cpp \
    source/Irrlicht/CTRTextureLightMap2_M1.cpp \
    source/Irrlicht/CTRTextureLightMap2_M2.cpp \
    source/Irrlicht/CTRTextureLightMap2_M4.cpp \
    source/Irrlicht/CTRTextureLightMapGouraud2_M4.cpp \
    source/Irrlicht/CTRTextureWire2.cpp \
    source/Irrlicht/CVideoModeList.cpp \
    source/Irrlicht/CVolumeLightSceneNode.cpp \
    source/Irrlicht/CWADReader.cpp \
    source/Irrlicht/CWaterSurfaceSceneNode.cpp \
    source/Irrlicht/CWebGL1Driver.cpp \
    source/Irrlicht/CWGLManager.cpp \
    source/Irrlicht/CWriteFile.cpp \
    source/Irrlicht/CXMeshFileLoader.cpp \
    source/Irrlicht/CXMLReader.cpp \
    source/Irrlicht/CXMLWriter.cpp \
    source/Irrlicht/CZBuffer.cpp \
    source/Irrlicht/CZipReader.cpp \
    source/Irrlicht/IBurningShader.cpp \
    source/Irrlicht/Irrlicht.cpp \
    source/Irrlicht/irrXML.cpp \
    source/Irrlicht/leakHunter.cpp \
    source/Irrlicht/os.cpp \
    source/Irrlicht/utf8.cpp \
    source/Irrlicht/lzma/LzmaDec.c

DISTFILES += \
    rpm/irrlicht-sailfish.yaml
