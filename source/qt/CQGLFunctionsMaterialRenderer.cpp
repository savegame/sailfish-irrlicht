// Copyright (C) 2014 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CQGLFunctionsMaterialRenderer.h"

#if defined(_IRR_COMPILE_WITH_OGLES2_) || defined(_IRR_COMPILE_WITH_QGLFUNCTIONS_)

#include "EVertexAttributes.h"
#include "IGPUProgrammingServices.h"
#include "IShaderConstantSetCallBack.h"
#include "IVideoDriver.h"
#include "os.h"

#include "CQGLFunctionsDriver.h"

#include "COpenGLCoreTexture.h"
#include "COpenGLCoreCacheHandler.h"

namespace irr
{
namespace video
{


CQGLFunctionsMaterialRenderer::CQGLFunctionsMaterialRenderer(CQGLFunctionsDriver* driver,
		s32& outMaterialTypeNr,
		const c8* vertexShaderProgram,
		const c8* pixelShaderProgram,
		IShaderConstantSetCallBack* callback,
		E_MATERIAL_TYPE baseMaterial,
		s32 userData)
	: Driver(driver), CallBack(callback), Alpha(false)
	, Blending(false), FixedBlending(false), Program(0), UserData(userData)
	, Functions(driver->m_functions)
{
#ifdef _DEBUG
	setDebugName("CQGLFunctionsMaterialRenderer");
#endif
	
	switch (baseMaterial)
	{
	case EMT_TRANSPARENT_VERTEX_ALPHA:
	case EMT_TRANSPARENT_ALPHA_CHANNEL:
	case EMT_NORMAL_MAP_TRANSPARENT_VERTEX_ALPHA:
	case EMT_PARALLAX_MAP_TRANSPARENT_VERTEX_ALPHA:
		Alpha = true;
		break;
	case EMT_TRANSPARENT_ADD_COLOR:
	case EMT_NORMAL_MAP_TRANSPARENT_ADD_COLOR:
	case EMT_PARALLAX_MAP_TRANSPARENT_ADD_COLOR:
		FixedBlending = true;
		break;
	case EMT_ONETEXTURE_BLEND:
		Blending = true;
		break;
	default:
		break;
	}
	
	if (CallBack)
		CallBack->grab();
	
	init(outMaterialTypeNr, vertexShaderProgram, pixelShaderProgram);
}


CQGLFunctionsMaterialRenderer::CQGLFunctionsMaterialRenderer(CQGLFunctionsDriver* driver,
                                                             IShaderConstantSetCallBack* callback,
                                                             E_MATERIAL_TYPE baseMaterial, s32 userData)
    : Driver(driver), CallBack(callback), Alpha(false), Blending(false), FixedBlending(false), Program(0), UserData(userData)
{
	switch (baseMaterial)
	{
	case EMT_TRANSPARENT_VERTEX_ALPHA:
	case EMT_TRANSPARENT_ALPHA_CHANNEL:
	case EMT_NORMAL_MAP_TRANSPARENT_VERTEX_ALPHA:
	case EMT_PARALLAX_MAP_TRANSPARENT_VERTEX_ALPHA:
		Alpha = true;
		break;
	case EMT_TRANSPARENT_ADD_COLOR:
	case EMT_NORMAL_MAP_TRANSPARENT_ADD_COLOR:
	case EMT_PARALLAX_MAP_TRANSPARENT_ADD_COLOR:
		FixedBlending = true;
		break;
	case EMT_ONETEXTURE_BLEND:
		Blending = true;
		break;
	default:
		break;
	}
	
	if (CallBack)
		CallBack->grab();
}


CQGLFunctionsMaterialRenderer::~CQGLFunctionsMaterialRenderer()
{
	if (CallBack)
		CallBack->drop();
	
	if (Program)
	{
		GLuint shaders[8];
		GLint count;
		Driver->m_functions->glGetAttachedShaders(Program, 8, &count, shaders);
		
		count=core::min_(count,8);
		for (GLint i=0; i<count; ++i)
			Driver->m_functions->glDeleteShader(shaders[i]);
		Driver->m_functions->glDeleteProgram(Program);
		Program = 0;
	}
	
	UniformInfo.clear();
}

GLuint CQGLFunctionsMaterialRenderer::getProgram() const
{
	return Program;
}

void CQGLFunctionsMaterialRenderer::init(s32& outMaterialTypeNr,
                                         const c8* vertexShaderProgram,
                                         const c8* pixelShaderProgram,
                                         bool addMaterial)
{
	outMaterialTypeNr = -1;
	
	Program = Driver->m_functions->glCreateProgram();
	
	if (!Program)
		return;
	
	if (vertexShaderProgram)
		if (!createShader(GL_VERTEX_SHADER, vertexShaderProgram))
			return;
	
	if (pixelShaderProgram)
		if (!createShader(GL_FRAGMENT_SHADER, pixelShaderProgram))
			return;
	
	for ( size_t i = 0; i < EVA_COUNT; ++i )
		Driver->m_functions->glBindAttribLocation( Program, i, sBuiltInVertexAttributeNames[i]);
	
	if (!linkProgram())
		return;
	
	if (addMaterial)
		outMaterialTypeNr = Driver->addMaterialRenderer(this);
}


bool CQGLFunctionsMaterialRenderer::OnRender(IMaterialRendererServices* service, E_VERTEX_TYPE vtxtype)
{
	if (CallBack && Program)
		CallBack->OnSetConstants(this, UserData);
	
	return true;
}


void CQGLFunctionsMaterialRenderer::OnSetMaterial(const video::SMaterial& material,
                                                  const video::SMaterial& lastMaterial,
                                                  bool resetAllRenderstates,
                                                  video::IMaterialRendererServices* services)
{
	CQGLFunctionsCacheHandler* cacheHandler = Driver->getCacheHandler();
	
	cacheHandler->setProgram(Program);
	
	Driver->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);
	
	if (Alpha)
	{
		cacheHandler->setBlend(true);
		cacheHandler->setBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else if (FixedBlending)
	{
		cacheHandler->setBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
		cacheHandler->setBlend(true);
	}
	else if (Blending)
	{
		E_BLEND_FACTOR srcRGBFact,dstRGBFact,srcAlphaFact,dstAlphaFact;
		E_MODULATE_FUNC modulate;
		u32 alphaSource;
		unpack_textureBlendFuncSeparate(srcRGBFact, dstRGBFact, srcAlphaFact, dstAlphaFact, modulate, alphaSource, material.MaterialTypeParam);
		
		cacheHandler->setBlendFuncSeparate(Driver->getGLBlend(srcRGBFact), Driver->getGLBlend(dstRGBFact),
		                                   Driver->getGLBlend(srcAlphaFact), Driver->getGLBlend(dstAlphaFact));
		
		cacheHandler->setBlend(true);
	}
	
	if (CallBack)
		CallBack->OnSetMaterial(material);
}


void CQGLFunctionsMaterialRenderer::OnUnsetMaterial()
{
}


bool CQGLFunctionsMaterialRenderer::isTransparent() const
{
	return (Alpha || Blending || FixedBlending);
}


s32 CQGLFunctionsMaterialRenderer::getRenderCapability() const
{
	return 0;
}


bool CQGLFunctionsMaterialRenderer::createShader(GLenum shaderType, const char* shader)
{
	if (Program)
	{
		GLuint shaderHandle = Driver->m_functions->glCreateShader(shaderType);
		Driver->m_functions->glShaderSource(shaderHandle, 1, &shader, NULL);
		Driver->m_functions->glCompileShader(shaderHandle);
		
		GLint status = 0;
		
		Driver->m_functions->glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &status);
		
		if (status != GL_TRUE)
		{
			os::Printer::log("GLSL shader failed to compile", ELL_ERROR);
			
			GLint maxLength=0;
			GLint length;
			
			Driver->m_functions->glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH,
			                                   &maxLength);
			
			if (maxLength)
			{
				GLchar *infoLog = new GLchar[maxLength];
				Driver->m_functions->glGetShaderInfoLog(shaderHandle, maxLength, &length, infoLog);
				os::Printer::log(reinterpret_cast<const c8*>(infoLog), ELL_ERROR);
				delete [] infoLog;
			}
			
			return false;
		}
		
		Driver->m_functions->glAttachShader(Program, shaderHandle);
	}
	
	return true;
}


bool CQGLFunctionsMaterialRenderer::linkProgram()
{
	if (Program)
	{
		Driver->m_functions->glLinkProgram(Program);
		
		GLint status = 0;
		
		Driver->m_functions->glGetProgramiv(Program, GL_LINK_STATUS, &status);
		
		if (!status)
		{
			os::Printer::log("GLSL shader program failed to link", ELL_ERROR);
			
			GLint maxLength=0;
			GLsizei length;
			
			Driver->m_functions->glGetProgramiv(Program, GL_INFO_LOG_LENGTH, &maxLength);
			
			if (maxLength)
			{
				GLchar *infoLog = new GLchar[maxLength];
				Driver->m_functions->glGetProgramInfoLog(Program, maxLength, &length, infoLog);
				os::Printer::log(reinterpret_cast<const c8*>(infoLog), ELL_ERROR);
				delete [] infoLog;
			}
			
			return false;
		}
		
		GLint num = 0;
		
		Driver->m_functions->glGetProgramiv(Program, GL_ACTIVE_UNIFORMS, &num);
		
		if (num == 0)
			return true;
		
		GLint maxlen = 0;
		
		Driver->m_functions->glGetProgramiv(Program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxlen);
		
		if (maxlen == 0)
		{
			os::Printer::log("GLSL: failed to retrieve uniform information", ELL_ERROR);
			return false;
		}
		
		// seems that some implementations use an extra null terminator.
		++maxlen;
		c8 *buf = new c8[maxlen];
		
		UniformInfo.clear();
		UniformInfo.reallocate(num);
		
		for (GLint i=0; i < num; ++i)
		{
			SUniformInfo ui;
			memset(buf, 0, maxlen);
			
			GLint size;
			Driver->m_functions->glGetActiveUniform(Program, i, maxlen, 0, &size, &ui.type, reinterpret_cast<GLchar*>(buf));
			
			core::stringc name = "";
			
			// array support, workaround for some bugged drivers.
			for (s32 i = 0; i < maxlen; ++i)
			{
				if (buf[i] == '[' || buf[i] == '\0')
					break;
				
				name += buf[i];
			}
			
			ui.name = name;
			ui.location = Driver->m_functions->glGetUniformLocation(Program, buf);
			
			UniformInfo.push_back(ui);
		}
		
		delete [] buf;
	}
	
	return true;
}


void CQGLFunctionsMaterialRenderer::setBasicRenderStates(const SMaterial& material,
                                                         const SMaterial& lastMaterial,
                                                         bool resetAllRenderstates)
{
	Driver->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);
}

s32 CQGLFunctionsMaterialRenderer::getVertexShaderConstantID(const c8* name)
{
	return getPixelShaderConstantID(name);
}

s32 CQGLFunctionsMaterialRenderer::getPixelShaderConstantID(const c8* name)
{
	for (u32 i = 0; i < UniformInfo.size(); ++i)
	{
		if (UniformInfo[i].name == name)
			return i;
	}
	
	return -1;
}

void CQGLFunctionsMaterialRenderer::setVertexShaderConstant(const f32* data, s32 startRegister, s32 constantAmount)
{
	os::Printer::log("Cannot set constant, please use high level shader call instead.", ELL_WARNING);
}

void CQGLFunctionsMaterialRenderer::setPixelShaderConstant(const f32* data, s32 startRegister, s32 constantAmount)
{
	os::Printer::log("Cannot set constant, use high level shader call.", ELL_WARNING);
}

bool CQGLFunctionsMaterialRenderer::setVertexShaderConstant(s32 index, const f32* floats, int count)
{
	return setPixelShaderConstant(index, floats, count);
}

bool CQGLFunctionsMaterialRenderer::setVertexShaderConstant(s32 index, const s32* ints, int count)
{
	return setPixelShaderConstant(index, ints, count);
}

bool CQGLFunctionsMaterialRenderer::setPixelShaderConstant(s32 index, const f32* floats, int count)
{
	if(index < 0 || UniformInfo[index].location < 0)
		return false;
	
	bool status = true;
	
	switch (UniformInfo[index].type)
	{
	case GL_FLOAT:
		Driver->m_functions->glUniform1fv(UniformInfo[index].location, count, floats);
		break;
	case GL_FLOAT_VEC2:
		Driver->m_functions->glUniform2fv(UniformInfo[index].location, count/2, floats);
		break;
	case GL_FLOAT_VEC3:
		Driver->m_functions->glUniform3fv(UniformInfo[index].location, count/3, floats);
		break;
	case GL_FLOAT_VEC4:
		Driver->m_functions->glUniform4fv(UniformInfo[index].location, count/4, floats);
		break;
	case GL_FLOAT_MAT2:
		Driver->m_functions->glUniformMatrix2fv(UniformInfo[index].location, count/4, false, floats);
		break;
	case GL_FLOAT_MAT3:
		Driver->m_functions->glUniformMatrix3fv(UniformInfo[index].location, count/9, false, floats);
		break;
	case GL_FLOAT_MAT4:
		Driver->m_functions->glUniformMatrix4fv(UniformInfo[index].location, count/16, false, floats);
		break;
	case GL_SAMPLER_2D:
	case GL_SAMPLER_CUBE:
	{
		if(floats)
		{
			const GLint id = (GLint)(*floats);
			Driver->m_functions->glUniform1iv(UniformInfo[index].location, 1, &id);
		}
		else
			status = false;
	}
		break;
	default:
		status = false;
		break;
	}
	
	return status;
}

bool CQGLFunctionsMaterialRenderer::setPixelShaderConstant(s32 index, const s32* ints, int count)
{
	if(index < 0 || UniformInfo[index].location < 0)
		return false;
	
	bool status = true;
	
	switch (UniformInfo[index].type)
	{
	case GL_INT:
	case GL_BOOL:
		Driver->m_functions->glUniform1iv(UniformInfo[index].location, count, ints);
		break;
	case GL_INT_VEC2:
	case GL_BOOL_VEC2:
		Driver->m_functions->glUniform2iv(UniformInfo[index].location, count/2, ints);
		break;
	case GL_INT_VEC3:
	case GL_BOOL_VEC3:
		Driver->m_functions->glUniform3iv(UniformInfo[index].location, count/3, ints);
		break;
	case GL_INT_VEC4:
	case GL_BOOL_VEC4:
		Driver->m_functions->glUniform4iv(UniformInfo[index].location, count/4, ints);
		break;
	case GL_SAMPLER_2D:
	case GL_SAMPLER_CUBE:
		Driver->m_functions->glUniform1iv(UniformInfo[index].location, 1, ints);
		break;
	default:
		status = false;
		break;
	}
	
	return status;
}

IVideoDriver* CQGLFunctionsMaterialRenderer::getVideoDriver()
{
	return Driver;
}

}
}


#endif

