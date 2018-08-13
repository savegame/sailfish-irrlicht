#ifndef CQGLFUNCTIONSRENDERTARGET_H
#define CQGLFUNCTIONSRENDERTARGET_H

#if defined(_IRR_COMPILE_WITH_QGLFUNCTIONS_)
#include <QOpenGLFunctions>
#endif

#include "IrrCompileConfig.h"

#if defined(_IRR_COMPILE_WITH_QGLFUNCTIONS_)

#include <IRenderTarget.h>
#include <ITexture.h>
#include <dimension2d.h>

namespace irr
{
namespace video
{

class CQGLFunctionsDriver;
class CQGLFunctionsTexture;

class CQGLFunctionsRenderTarget : public IRenderTarget
{
public:
	CQGLFunctionsRenderTarget(CQGLFunctionsDriver* driver);

	virtual ~CQGLFunctionsRenderTarget();

	virtual void setTexture(const core::array<ITexture*>& texture, ITexture* depthStencil) _IRR_OVERRIDE_;

	void update();

	GLuint getBufferID() const;

	const core::dimension2d<u32>& getSize() const
	{
		return Size;
	}

	ITexture* getTexture() const
	{
		for (u32 i = 0; i < Texture.size(); ++i)
		{
			if (Texture[i])
				return Texture[i];
		}

		return 0;
	}

protected:
	bool checkFBO(CQGLFunctionsDriver* driver);

	core::array<GLenum> AssignedTexture;
	bool AssignedDepth;
	bool AssignedStencil;

	bool RequestTextureUpdate;
	bool RequestDepthStencilUpdate;

	GLuint BufferID;

	core::dimension2d<u32> Size;

	u32 ColorAttachment;
	u32 MultipleRenderTarget;

	CQGLFunctionsDriver* Driver;
};

} //namespace video
} //namespace irr

#endif
#endif // CQGLFUNCTIONSRENDERTARGET_H
