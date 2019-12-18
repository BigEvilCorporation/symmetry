#ifndef __SYM_SHADER
#define __SYM_SHADER

#ifdef SYM_DLL
#   define __declspec(dllexport)
#else
//#   define __declspec(dllimport)
#endif

#include <Cg/cg.h>
#include <Cg/cgGL.h>
#include <string>
#include <map>

#include "SYM_Texture.h"
#pragma once

#include <Cg/cg.h>
#include <Cg/cgGL.h>
#include <string>

#include "SYM_Texture.h"

namespace sym
{
	void CgErrorCallback();

	class CShaderManager
	{
		public:
			//Init Cg context
			bool Init();

			//Get Cg context
			CGcontext GetContext();

			//Check for Cg errors
			bool CheckCgErrors();

		private:
			//Cg context
			CGcontext CgContext;
	};

	class CShader
	{
		public:
			CShader();

			//Load from file
			bool Load(std::string Filename);

			//Destroy effect
			void Destroy();

			//Set parameters
			bool SetBoolParam(std::string Semantic, bool Value);
			bool SetFloatParam(std::string Semantic, float Value);
			bool SetFloat2Param(std::string Semantic, float Values[2]);
			bool SetFloat3Param(std::string Semantic, float Values[3]);
			bool SetFloat4Param(std::string Semantic, float Values[4]);
			bool SetTextureParam(std::string Semantic, CTexture *Texture);

			//Rendering passes
			CGpass GetFirstPass();
			CGpass GetNextPass(CGpass CgPass);
			void SetPass(CGpass CgPass);
			void ResetPass(CGpass CgPass);

			CGeffect GetEffect() { return CgEffect; }

			//CGFX filename
			std::string CGFXFile;

		private:
			//Cg effect object
			CGeffect CgEffect;

			//Current Cg technique
			CGtechnique CgTechnique;

			//Matrix handles
			CGparameter ParamWorldMatrix;
			CGparameter ParamWorldViewProjMatrix;
			CGparameter ParamWorldInvTransMatrix;
			CGparameter ParamViewInverseMatrix;
			CGparameter ParamViewMatrix;
			CGparameter ParamWorldViewMatrix;

			//Other params
			CGparameter ParamEyePos;
			CGparameter ParamDeltaTime;
			CGparameter ParamRunningTime;
	};
}

#endif
