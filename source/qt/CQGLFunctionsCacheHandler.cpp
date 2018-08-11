#include "CQGLFunctionsCacheHandler.h"


#if defined(_IRR_COMPILE_WITH_QGLFUNCTIONS_)
#include <QOpenGLFunctions>
#include "SMaterial.h"
#include "ITexture.h"
#include "CQGLFunctionsTexture.h"
#include "CQGLFunctionsDriver.h"

namespace irr {
namespace video {

CQGLFunctionsCacheHandler::STextureCache::STextureCache(irr::video::CQGLFunctionsCacheHandler &cacheHandler, irr::video::E_DRIVER_TYPE driverType, irr::u32 textureCount) :
    CacheHandler(cacheHandler), DriverType(driverType), TextureCount(textureCount)
{
	for (u32 i = 0; i < MATERIAL_MAX_TEXTURES; ++i)
	{
		Texture[i] = 0;
	}
}

CQGLFunctionsCacheHandler::STextureCache::~STextureCache()
{
	clear();
}

const CQGLFunctionsTexture *CQGLFunctionsCacheHandler::STextureCache::operator[](int index) const
{
	if (static_cast<u32>(index) < MATERIAL_MAX_TEXTURES)
		return Texture[static_cast<u32>(index)];
	
	return 0;
}

const CQGLFunctionsTexture *CQGLFunctionsCacheHandler::STextureCache::get(u32 index) const
{
	if (index < MATERIAL_MAX_TEXTURES)
		return Texture[index];
	
	return 0;
}

bool CQGLFunctionsCacheHandler::STextureCache::set(u32 index, const ITexture *texture)
{
	bool status = false;

	E_DRIVER_TYPE type = DriverType;

	if (index < MATERIAL_MAX_TEXTURES && index < TextureCount)
	{
		CacheHandler.setActiveTexture(GL_TEXTURE0 + index);

		const CQGLFunctionsTexture* prevTexture = Texture[index];

		if (texture != prevTexture)
		{
			if (texture)
			{
				type = texture->getDriverType();

				if (type == DriverType)
				{
					texture->grab();

					const CQGLFunctionsTexture* curTexture = static_cast<const CQGLFunctionsTexture*>(texture);
					const GLenum curTextureType = curTexture->getOpenGLTextureType();
					const GLenum prevTextureType = (prevTexture) ? prevTexture->getOpenGLTextureType() : curTextureType;

					if (curTextureType != prevTextureType)
					{
						CacheHandler.Driver->m_functions->glBindTexture(prevTextureType, 0);

#if (defined(IRR_OPENGL_VERSION) && IRR_OPENGL_VERSION < 20) || (defined(IRR_OPENGL_ES_VERSION) && IRR_OPENGL_ES_VERSION < 20)
						CacheHandler.Driver->m_functions->glDisable(prevTextureType);
						CacheHandler.Driver->m_functions->glEnable(curTextureType);
#endif
					}
#if (defined(IRR_OPENGL_VERSION) && IRR_OPENGL_VERSION < 20) || (defined(IRR_OPENGL_ES_VERSION) && IRR_OPENGL_ES_VERSION < 20)
					else if (!prevTexture)
						CacheHandler.Driver->m_functions->glEnable(curTextureType);
#endif

					CacheHandler.Driver->m_functions->glBindTexture(curTextureType, static_cast<const CQGLFunctionsTexture*>(texture)->getOpenGLTextureName());
				}
				else
				{
					texture = 0;

					os::Printer::log("Fatal Error: Tried to set a texture not owned by this driver.", ELL_ERROR);
					os::Printer::log("Texture type", irr::core::stringc((int)type), ELL_ERROR);
					os::Printer::log("Driver (or cache handler) type", irr::core::stringc((int)DriverType), ELL_ERROR);
				}
			}

			if (!texture && prevTexture)
			{
				const GLenum prevTextureType = prevTexture->getOpenGLTextureType();

				CacheHandler.Driver->m_functions->glBindTexture(prevTextureType, 0);

#if (defined(IRR_OPENGL_VERSION) && IRR_OPENGL_VERSION < 20) || (defined(IRR_OPENGL_ES_VERSION) && IRR_OPENGL_ES_VERSION < 20)
				Driver->m_functions->glDisable(prevTextureType);
#endif
			}

			Texture[index] = static_cast<const CQGLFunctionsTexture*>(texture);

			if (prevTexture)
				prevTexture->drop();
		}

		status = true;
	}

	return (status && type == DriverType);
}

void CQGLFunctionsCacheHandler::STextureCache::remove(ITexture *texture)
{
	if (!texture)
		return;
	
	for (u32 i = 0; i < MATERIAL_MAX_TEXTURES; ++i)
	{
		if (Texture[i] == texture)
		{
			Texture[i] = 0;
			
			texture->drop();
		}
	}
}

void CQGLFunctionsCacheHandler::STextureCache::clear()
{
	for (u32 i = 0; i < MATERIAL_MAX_TEXTURES; ++i)
	{
		if (Texture[i])
		{
			const CQGLFunctionsTexture* prevTexture = Texture[i];
			
			Texture[i] = 0;
			
			prevTexture->drop();
		}
	}
}

QOpenGLFunctions *CQGLFunctionsCacheHandler::functions() {return Driver->m_functions;}

CQGLFunctionsCacheHandler::CQGLFunctionsCacheHandler(CQGLFunctionsDriver *driver) :
    Driver(driver), 
    #if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable: 4355)	// Warning: "'this' : used in base member initializer list. ". It's OK, we don't use the reference in STextureCache constructor.
    #endif
    TextureCache(STextureCache(*this, driver->getDriverType(), driver->getFeature().TextureUnit)), 
    #if defined(_MSC_VER)
    #pragma warning(pop)
    #endif
    FrameBufferCount(0), BlendEquation(0), BlendSourceRGB(0),
    BlendDestinationRGB(0), BlendSourceAlpha(0), BlendDestinationAlpha(0), Blend(0), BlendEquationInvalid(false), BlendFuncInvalid(false), BlendInvalid(false),
    ColorMask(0), ColorMaskInvalid(false), CullFaceMode(GL_BACK), CullFace(false), DepthFunc(GL_LESS), DepthMask(true), DepthTest(false), FrameBufferID(0),
    ProgramID(0), ActiveTexture(GL_TEXTURE0), ViewportX(0), ViewportY(0)
{
	const COpenGLCoreFeature& feature = Driver->getFeature();
	
	FrameBufferCount = core::max_(static_cast<GLuint>(1), static_cast<GLuint>(feature.MultipleRenderTarget));
	
	BlendEquation = new GLenum[FrameBufferCount];
	BlendSourceRGB = new GLenum[FrameBufferCount];
	BlendDestinationRGB = new GLenum[FrameBufferCount];
	BlendSourceAlpha = new GLenum[FrameBufferCount];
	BlendDestinationAlpha = new GLenum[FrameBufferCount];
	Blend = new bool[FrameBufferCount];
	ColorMask = new u8[FrameBufferCount];
	
	// Initial OpenGL values from specification.
	
	if (feature.BlendOperation)
	{
		Driver->irrGlBlendEquation(GL_FUNC_ADD);
	}
	
	for (u32 i = 0; i < FrameBufferCount; ++i)
	{
		BlendEquation[i] = GL_FUNC_ADD;
		
		BlendSourceRGB[i] = GL_ONE;
		BlendDestinationRGB[i] = GL_ZERO;
		BlendSourceAlpha[i] = GL_ONE;
		BlendDestinationAlpha[i] = GL_ZERO;
		
		Blend[i] = false;
		ColorMask[i] = ECP_ALL;
	}
	
	Driver->m_functions->glBlendFunc(GL_ONE, GL_ZERO);
	Driver->m_functions->glDisable(GL_BLEND);
	
	Driver->m_functions->glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	
	Driver->m_functions->glCullFace(CullFaceMode);
	Driver->m_functions->glDisable(GL_CULL_FACE);
	
	Driver->m_functions->glDepthFunc(DepthFunc);
	Driver->m_functions->glDepthMask(GL_TRUE);
	Driver->m_functions->glDisable(GL_DEPTH_TEST);
	
	Driver->irrGlActiveTexture(ActiveTexture);
	
#if (defined(IRR_OPENGL_VERSION) && IRR_OPENGL_VERSION < 20) || (defined(IRR_OPENGL_ES_VERSION) && IRR_OPENGL_ES_VERSION < 20)
	Driver->m_functions->glDisable(GL_TEXTURE_2D);
#endif
	
	const core::dimension2d<u32> ScreenSize = Driver->getScreenSize();
	ViewportWidth = ScreenSize.Width;
	ViewportHeight = ScreenSize.Height;
	Driver->m_functions->glViewport(ViewportX, ViewportY, ViewportWidth, ViewportHeight);
}

irr::video::CQGLFunctionsCacheHandler::~CQGLFunctionsCacheHandler()
{
	delete[] BlendEquation;
	delete[] BlendSourceRGB;
	delete[] BlendDestinationRGB;
	delete[] BlendSourceAlpha;
	delete[] BlendDestinationAlpha;
	delete[] Blend;
	
	delete[] ColorMask;
}

E_DRIVER_TYPE irr::video::CQGLFunctionsCacheHandler::getDriverType() const
{
	return Driver->getDriverType();
}

irr::video::CQGLFunctionsCacheHandler::STextureCache &irr::video::CQGLFunctionsCacheHandler::getTextureCache()
{
	return TextureCache;
}

void irr::video::CQGLFunctionsCacheHandler::setBlendEquation(GLenum mode)
{
	if (BlendEquation[0] != mode || BlendEquationInvalid)
	{
		Driver->irrGlBlendEquation(mode);
		
		for (GLuint i = 0; i < FrameBufferCount; ++i)
			BlendEquation[i] = mode;
		
		BlendEquationInvalid = false;
	}
}

void irr::video::CQGLFunctionsCacheHandler::setBlendEquationIndexed(GLuint index, GLenum mode)
{
	if (index < FrameBufferCount && BlendEquation[index] != mode)
	{
		Driver->irrGlBlendEquationIndexed(index, mode);
		
		BlendEquation[index] = mode;
		BlendEquationInvalid = true;
	}
}

void irr::video::CQGLFunctionsCacheHandler::setBlendFunc(GLenum source, GLenum destination)
{
	if (BlendSourceRGB[0] != source || BlendDestinationRGB[0] != destination ||
	        BlendSourceAlpha[0] != source || BlendDestinationAlpha[0] != destination ||
	        BlendFuncInvalid)
	{
		Driver->m_functions->glBlendFunc(source, destination);
		
		for (GLuint i = 0; i < FrameBufferCount; ++i)
		{
			BlendSourceRGB[i] = source;
			BlendDestinationRGB[i] = destination;
			BlendSourceAlpha[i] = source;
			BlendDestinationAlpha[i] = destination;
		}
		
		BlendFuncInvalid = false;
	}
}

void irr::video::CQGLFunctionsCacheHandler::setBlendFuncSeparate(GLenum sourceRGB, GLenum destinationRGB, GLenum sourceAlpha, GLenum destinationAlpha)
{
	if (sourceRGB != sourceAlpha || destinationRGB != destinationAlpha)
	{
		if (BlendSourceRGB[0] != sourceRGB || BlendDestinationRGB[0] != destinationRGB ||
		        BlendSourceAlpha[0] != sourceAlpha || BlendDestinationAlpha[0] != destinationAlpha ||
		        BlendFuncInvalid)
		{
			Driver->irrGlBlendFuncSeparate(sourceRGB, destinationRGB, sourceAlpha, destinationAlpha);
			
			for (GLuint i = 0; i < FrameBufferCount; ++i)
			{
				BlendSourceRGB[i] = sourceRGB;
				BlendDestinationRGB[i] = destinationRGB;
				BlendSourceAlpha[i] = sourceAlpha;
				BlendDestinationAlpha[i] = destinationAlpha;
			}
			
			BlendFuncInvalid = false;
		}
	}
	else
	{
		setBlendFunc(sourceRGB, destinationRGB);
	}
}

void irr::video::CQGLFunctionsCacheHandler::setBlendFuncIndexed(GLuint index, GLenum source, GLenum destination)
{
	if (index < FrameBufferCount && (BlendSourceRGB[index] != source || BlendDestinationRGB[index] != destination ||
	                                 BlendSourceAlpha[index] != source || BlendDestinationAlpha[index] != destination))
	{
		Driver->irrGlBlendFuncIndexed(index, source, destination);
		
		BlendSourceRGB[index] = source;
		BlendDestinationRGB[index] = destination;
		BlendSourceAlpha[index] = source;
		BlendDestinationAlpha[index] = destination;
		BlendFuncInvalid = true;
	}
}

void irr::video::CQGLFunctionsCacheHandler::setBlendFuncSeparateIndexed(GLuint index, GLenum sourceRGB, GLenum destinationRGB, GLenum sourceAlpha, GLenum destinationAlpha)
{
	if (sourceRGB != sourceAlpha || destinationRGB != destinationAlpha)
	{
		if (index < FrameBufferCount && (BlendSourceRGB[index] != sourceRGB || BlendDestinationRGB[index] != destinationRGB ||
		                                 BlendSourceAlpha[index] != sourceAlpha || BlendDestinationAlpha[index] != destinationAlpha))
		{
			Driver->irrGlBlendFuncSeparateIndexed(index, sourceRGB, destinationRGB, sourceAlpha, destinationAlpha);
			
			BlendSourceRGB[index] = sourceRGB;
			BlendDestinationRGB[index] = destinationRGB;
			BlendSourceAlpha[index] = sourceAlpha;
			BlendDestinationAlpha[index] = destinationAlpha;
			BlendFuncInvalid = true;
		}
	}
	else
	{
		setBlendFuncIndexed(index, sourceRGB, destinationRGB);
	}
}

void irr::video::CQGLFunctionsCacheHandler::setBlend(bool enable)
{
	if (Blend[0] != enable || BlendInvalid)
	{
		if (enable)
			Driver->m_functions->glEnable(GL_BLEND);
		else
			Driver->m_functions->glDisable(GL_BLEND);
		
		for (GLuint i = 0; i < FrameBufferCount; ++i)
			Blend[i] = enable;
		
		BlendInvalid = false;
	}
}

void irr::video::CQGLFunctionsCacheHandler::setBlendIndexed(GLuint index, bool enable)
{
	if (index < FrameBufferCount && Blend[index] != enable)
	{
		if (enable)
			Driver->irrGlEnableIndexed(GL_BLEND, index);
		else
			Driver->irrGlDisableIndexed(GL_BLEND, index);
		
		Blend[index] = enable;
		BlendInvalid = true;
	}
}

void irr::video::CQGLFunctionsCacheHandler::getColorMask(u8 &mask)
{
	mask = ColorMask[0];
}

void irr::video::CQGLFunctionsCacheHandler::setColorMask(u8 mask)
{
	if (ColorMask[0] != mask || ColorMaskInvalid)
	{
		Driver->m_functions->glColorMask((mask & ECP_RED) ? GL_TRUE : GL_FALSE, (mask & ECP_GREEN) ? GL_TRUE : GL_FALSE, (mask & ECP_BLUE) ? GL_TRUE : GL_FALSE, (mask & ECP_ALPHA) ? GL_TRUE : GL_FALSE);
		
		for (GLuint i = 0; i < FrameBufferCount; ++i)
			ColorMask[i] = mask;
		
		ColorMaskInvalid = false;
	}
}

void irr::video::CQGLFunctionsCacheHandler::setColorMaskIndexed(GLuint index, u8 mask)
{
	if (index < FrameBufferCount && ColorMask[index] != mask)
	{
		Driver->irrGlColorMaskIndexed(index, (mask & ECP_RED) ? GL_TRUE : GL_FALSE, (mask & ECP_GREEN) ? GL_TRUE : GL_FALSE, (mask & ECP_BLUE) ? GL_TRUE : GL_FALSE, (mask & ECP_ALPHA) ? GL_TRUE : GL_FALSE);
		
		ColorMask[index] = mask;
		ColorMaskInvalid = true;
	}
}

void irr::video::CQGLFunctionsCacheHandler::setCullFaceFunc(GLenum mode)
{
	if (CullFaceMode != mode)
	{
		Driver->m_functions->glCullFace(mode);
		CullFaceMode = mode;
	}
}

void irr::video::CQGLFunctionsCacheHandler::setCullFace(bool enable)
{
	if (CullFace != enable)
	{
		if (enable)
			Driver->m_functions->glEnable(GL_CULL_FACE);
		else
			Driver->m_functions->glDisable(GL_CULL_FACE);
		
		CullFace = enable;
	}
}

void irr::video::CQGLFunctionsCacheHandler::setDepthFunc(GLenum mode)
{
	if (DepthFunc != mode)
	{
		Driver->m_functions->glDepthFunc(mode);
		DepthFunc = mode;
	}
}

void irr::video::CQGLFunctionsCacheHandler::getDepthMask(bool &depth)
{
	depth = DepthMask;
}

void irr::video::CQGLFunctionsCacheHandler::setDepthMask(bool enable)
{
	if (DepthMask != enable)
	{
		if (enable)
			Driver->m_functions->glDepthMask(GL_TRUE);
		else
			Driver->m_functions->glDepthMask(GL_FALSE);
		
		DepthMask = enable;
	}
}

void irr::video::CQGLFunctionsCacheHandler::setDepthTest(bool enable)
{
	if (DepthTest != enable)
	{
		if (enable)
			Driver->m_functions->glEnable(GL_DEPTH_TEST);
		else
			Driver->m_functions->glDisable(GL_DEPTH_TEST);
		
		DepthTest = enable;
	}
}

void irr::video::CQGLFunctionsCacheHandler::getFBO(GLuint &frameBufferID) const
{
	frameBufferID = FrameBufferID;
}

void irr::video::CQGLFunctionsCacheHandler::setFBO(GLuint frameBufferID)
{
	if (FrameBufferID != frameBufferID)
	{
		Driver->irrGlBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
		FrameBufferID = frameBufferID;
	}
}

void irr::video::CQGLFunctionsCacheHandler::getProgram(GLuint &programID) const
{
	programID = ProgramID;
}

void irr::video::CQGLFunctionsCacheHandler::setProgram(GLuint programID)
{
//	if (ProgramID != programID)
	{
		Driver->irrGlUseProgram(programID);
		ProgramID = programID;
	}
}

void irr::video::CQGLFunctionsCacheHandler::getActiveTexture(GLenum &texture) const
{
	texture = ActiveTexture;
}

void irr::video::CQGLFunctionsCacheHandler::setActiveTexture(GLenum texture)
{
	if (ActiveTexture != texture)
	{
		Driver->irrGlActiveTexture(texture);
		ActiveTexture = texture;
	}
}

void irr::video::CQGLFunctionsCacheHandler::getViewport(GLint &viewportX, GLint &viewportY, GLsizei &viewportWidth, GLsizei &viewportHeight) const
{
	viewportX = ViewportX;
	viewportY = ViewportY;
	viewportWidth = ViewportWidth;
	viewportHeight = ViewportHeight;
}

void irr::video::CQGLFunctionsCacheHandler::setViewport(GLint viewportX, GLint viewportY, GLsizei viewportWidth, GLsizei viewportHeight)
{
	if (ViewportX != viewportX || ViewportY != viewportY || ViewportWidth != viewportWidth || ViewportHeight != viewportHeight)
	{
		Driver->m_functions->glViewport(viewportX, viewportY, viewportWidth, viewportHeight);
		ViewportX = viewportX;
		ViewportY = viewportY;
		ViewportWidth = viewportWidth;
		ViewportHeight = viewportHeight;
	}
}


}// namespcae video
}//namespace irr

#endif // defined(_IRR_COMPILE_WITH_QGLFUNCTIONS_)
