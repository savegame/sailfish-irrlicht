#ifndef CQGLFUNCTIONSTEXTURE_H
#define CQGLFUNCTIONSTEXTURE_H

#if defined(_IRR_COMPILE_WITH_QGLFUNCTIONS_)

#if defined(_IRR_COMPILE_WITH_QGLFUNCTIONS_)
#include <QOpenGLFunctions>
#endif

#include "IrrCompileConfig.h"
#include <ITexture.h>
#include <irrArray.h>
#include <SMaterialLayer.h>
#include "EDriverFeatures.h"
#include "os.h"
#include "CImage.h"
#include "CColorConverter.h"


namespace irr {
namespace video {

class CQGLFunctionsDriver;

class CQGLFunctionsTexture : public ITexture
{
public:
	struct SStatesCache
	{
		SStatesCache() : WrapU(ETC_REPEAT), WrapV(ETC_REPEAT), WrapW(ETC_REPEAT),
			LODBias(0), AnisotropicFilter(0), BilinearFilter(false), TrilinearFilter(false),
			MipMapStatus(false), IsCached(false)
		{
		}

		u8 WrapU;
		u8 WrapV;
		u8 WrapW;
		s8 LODBias;
		u8 AnisotropicFilter;
		bool BilinearFilter;
		bool TrilinearFilter;
		bool MipMapStatus;
		bool IsCached;
	};

	CQGLFunctionsTexture(const io::path& name, const core::array<IImage*>& image, E_TEXTURE_TYPE type, CQGLFunctionsDriver* driver);

	CQGLFunctionsTexture(const io::path& name, const core::dimension2d<u32>& size, ECOLOR_FORMAT format, CQGLFunctionsDriver* driver);

	virtual ~CQGLFunctionsTexture();

	virtual void* lock(E_TEXTURE_LOCK_MODE mode = ETLM_READ_WRITE, u32 layer = 0) _IRR_OVERRIDE_;

	virtual void unlock() _IRR_OVERRIDE_;

	virtual void regenerateMipMapLevels(void* data = 0, u32 layer = 0) _IRR_OVERRIDE_;

	GLenum getOpenGLTextureType() const;

	GLuint getOpenGLTextureName() const;

	SStatesCache& getStatesCache() const;

protected:
	ECOLOR_FORMAT getBestColorFormat(ECOLOR_FORMAT format);

	void getImageValues(const IImage* image);
	void uploadTexture(bool initTexture, u32 layer, u32 level, void* data);

	CQGLFunctionsDriver* Driver;

	GLenum TextureType;
	GLuint TextureName;
	GLint InternalFormat;
	GLenum PixelFormat;
	GLenum PixelType;
	void (*Converter)(const void*, s32, void*);

	bool LockReadOnly;
	IImage* LockImage;
	u32 LockLayer;

	bool KeepImage;
	core::array<IImage*> Image;

	bool AutoGenerateMipMaps;

	mutable SStatesCache StatesCache;
};

}
}

#endif // _IRR_COMPILE_WITH_QGLFUNCTIONS_
#endif // CQGLFUNCTIONSTEXTURE_H
