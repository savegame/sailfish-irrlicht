// Copyright (C) 2014 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#include "CQGLFunctionsRenderer2D.h"

#if defined(_IRR_COMPILE_WITH_OGLES2_) || defined(_IRR_COMPILE_WITH_QGLFUNCTIONS_)

#include "IGPUProgrammingServices.h"
#include "os.h"

#include "CQGLFunctionsDriver.h"

#include "COpenGLCoreFeature.h"
#include "COpenGLCoreTexture.h"
#include "COpenGLCoreCacheHandler.h"

namespace irr
{
namespace video
{

CQGLFunctionsRenderer2D::CQGLFunctionsRenderer2D(const c8* vertexShaderProgram, const c8* pixelShaderProgram, CQGLFunctionsDriver* driver, bool withTexture) :
	CQGLFunctionsMaterialRenderer(driver, 0, EMT_SOLID),
	WithTexture(withTexture)
{
#ifdef _DEBUG
	setDebugName("CQGLFunctionsRenderer2D");
#endif

	int Temp = 0;

	init(Temp, vertexShaderProgram, pixelShaderProgram, false);

	CQGLFunctionsCacheHandler* cacheHandler = Driver->getCacheHandler();

	cacheHandler->setProgram(Program);

	// These states don't change later.

	ThicknessID = getPixelShaderConstantID("uThickness");
	if ( WithTexture )
	{
		TextureUsageID = getPixelShaderConstantID("uTextureUsage");
		s32 TextureUnitID = getPixelShaderConstantID("uTextureUnit");

		s32 TextureUnit = 0;
		setPixelShaderConstant(TextureUnitID, &TextureUnit, 1);

		s32 TextureUsage = 0;
		setPixelShaderConstant(TextureUsageID, &TextureUsage, 1);
	}

	cacheHandler->setProgram(0);
}

CQGLFunctionsRenderer2D::~CQGLFunctionsRenderer2D()
{
}

void CQGLFunctionsRenderer2D::OnSetMaterial(const video::SMaterial& material,
				const video::SMaterial& lastMaterial,
				bool resetAllRenderstates,
				video::IMaterialRendererServices* services)
{
	Driver->getCacheHandler()->setProgram(Program);
	Driver->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);

	f32 Thickness = (material.Thickness > 0.f) ? material.Thickness : 1.f;
	setPixelShaderConstant(ThicknessID, &Thickness, 1);

	if ( WithTexture )
	{
		s32 TextureUsage = material.TextureLayer[0].Texture ? 1 : 0;
		setPixelShaderConstant(TextureUsageID, &TextureUsage, 1);
	}
}

bool CQGLFunctionsRenderer2D::OnRender(IMaterialRendererServices* service, E_VERTEX_TYPE vtxtype)
{
	return true;
}

}
}

#endif
