#ifndef CQGLFUNCTIONSCACHEHANDLER_H
#define CQGLFUNCTIONSCACHEHANDLER_H
// 2018 sashikknox

#include "IrrCompileConfig.h"

#if defined(_IRR_COMPILE_WITH_QGLFUNCTIONS_)

#include <QOpenGLFunctions>
#include <ITexture.h>
#include <SMaterial.h>

class QOpenGLFunctions;

namespace irr
{
namespace video
{

class CQGLFunctionsTexture;
class CQGLFunctionsDriver;
        
class CQGLFunctionsCacheHandler
{
	class STextureCache
	{
	public:
		STextureCache(CQGLFunctionsCacheHandler& cacheHandler, E_DRIVER_TYPE driverType, u32 textureCount);

		~STextureCache();

		const CQGLFunctionsTexture* operator[](int index) const;

		const CQGLFunctionsTexture* get(u32 index) const;

		bool set(u32 index, const ITexture* texture);

		void remove(ITexture* texture);

		void clear();

	private:
		CQGLFunctionsCacheHandler& CacheHandler;

		E_DRIVER_TYPE DriverType;

		const CQGLFunctionsTexture* Texture[MATERIAL_MAX_TEXTURES];
		u32 TextureCount;
	};

	inline QOpenGLFunctions *functions();
public:
	CQGLFunctionsCacheHandler(CQGLFunctionsDriver* driver);

	virtual ~CQGLFunctionsCacheHandler();

	E_DRIVER_TYPE getDriverType() const;

	STextureCache& getTextureCache();

	// Blending calls.

	void setBlendEquation(GLenum mode);

	void setBlendEquationIndexed(GLuint index, GLenum mode);

	void setBlendFunc(GLenum source, GLenum destination);

	void setBlendFuncSeparate(GLenum sourceRGB, GLenum destinationRGB, GLenum sourceAlpha, GLenum destinationAlpha);

	void setBlendFuncIndexed(GLuint index, GLenum source, GLenum destination);

	void setBlendFuncSeparateIndexed(GLuint index, GLenum sourceRGB, GLenum destinationRGB, GLenum sourceAlpha, GLenum destinationAlpha);

	void setBlend(bool enable);

	void setBlendIndexed(GLuint index, bool enable);

	// Color Mask.

	void getColorMask(u8& mask);

	void setColorMask(u8 mask);

	void setColorMaskIndexed(GLuint index, u8 mask);

	// Cull face calls.

	void setCullFaceFunc(GLenum mode);

	void setCullFace(bool enable);

	// Depth calls.

	void setDepthFunc(GLenum mode);

	void getDepthMask(bool& depth);

	void setDepthMask(bool enable);

	void setDepthTest(bool enable);

	// FBO calls.

	void getFBO(GLuint& frameBufferID) const;

	void setFBO(GLuint frameBufferID);

	// Shaders calls.

	void getProgram(GLuint& programID) const;

	void setProgram(GLuint programID);

	// Texture calls.

	void getActiveTexture(GLenum& texture) const;

	void setActiveTexture(GLenum texture);

	// Viewport calls.

	void getViewport(GLint& viewportX, GLint& viewportY, GLsizei& viewportWidth, GLsizei& viewportHeight) const;

	void setViewport(GLint viewportX, GLint viewportY, GLsizei viewportWidth, GLsizei viewportHeight);

protected:
	CQGLFunctionsDriver* Driver;

	STextureCache TextureCache;

	GLuint FrameBufferCount;

	GLenum* BlendEquation;
	GLenum* BlendSourceRGB;
	GLenum* BlendDestinationRGB;
	GLenum* BlendSourceAlpha;
	GLenum* BlendDestinationAlpha;
	bool* Blend;
	bool BlendEquationInvalid;
	bool BlendFuncInvalid;
	bool BlendInvalid;
	

	u8* ColorMask;
	bool ColorMaskInvalid;

	GLenum CullFaceMode;
	bool CullFace;

	GLenum DepthFunc;
	bool DepthMask;
	bool DepthTest;

	GLuint FrameBufferID;

	GLuint ProgramID;

	GLenum ActiveTexture;

	GLint ViewportX;
	GLint ViewportY;
	GLsizei ViewportWidth;
	GLsizei ViewportHeight;
};

}
}

#endif

#endif // CQGLFUNCTIONSCACHEHANDLER_H
