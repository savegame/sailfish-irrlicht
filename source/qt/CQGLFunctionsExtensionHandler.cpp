// Copyright (C) 2015 Patryk Nadrowski
// Copyright (C) 2009-2010 Amundis
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#include "CQGLFunctionsExtensionHandler.h"

#if defined(_IRR_COMPILE_WITH_QGLFUNCTIONS_)

#include "irrString.h"
#include "SMaterial.h"
#include "fast_atof.h"

namespace irr
{
namespace video
{


	CQGLFunctionsExtensionHandler::CQGLFunctionsExtensionHandler(QOpenGLFunctions *f) :
			Version(0), MaxAnisotropy(1), MaxIndices(0xffff),
			MaxTextureSize(1), MaxTextureLODBias(0.f),
			StencilBuffer(false)
	{
		for (u32 i = 0; i < IRR_OGLES2_Feature_Count; ++i)
			FeatureAvailable[i] = false;

		DimAliasedLine[0] = 1.f;
		DimAliasedLine[1] = 1.f;
		DimAliasedPoint[0] = 1.f;
		DimAliasedPoint[1] = 1.f;
	}


//	void CQGLFunctionsExtensionHandler::dump() const
//	{
		
//	}


//	void CQGLFunctionsExtensionHandler::initExtensions()
//	{
		
//	}

	const COpenGLCoreFeature& CQGLFunctionsExtensionHandler::getFeature() const
	{
		return Feature;
	}

} // end namespace video
} // end namespace irr


#endif // _IRR_COMPILE_WITH_OGLES2_
