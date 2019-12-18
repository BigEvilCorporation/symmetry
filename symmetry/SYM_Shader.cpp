#include <iostream>

#include "SYM_Shader.h"
#include "SYM_Engine.h"

namespace sym
{
	void CgErrorCallback()
	{
		CGerror Error = cgGetError();

		if(Error)
		{
			std::string ErrorString = cgGetErrorString(Error);
			Engine->Logger.AddEntry(ErrorString);

			if (Error == CG_COMPILER_ERROR)
			{
				std::string LastListing = cgGetLastListing(Engine->ShaderManager.GetContext());
				Engine->Logger.AddEntry(LastListing);
			}
		}
	}

	bool CShaderManager::Init()
	{

		//Create Cg context
		CgContext = cgCreateContext();

		//Check for errors
		if (!CheckCgErrors() || !CgContext)
		{
            Engine->Logger.AddEntry("Error: Could not create Cg context");
			return false;
		}

		//Set error callback
		cgSetErrorCallback(CgErrorCallback);

		cgGLSetDebugMode( CG_FALSE );

		//Set deferred parameters
		cgSetParameterSettingMode(CgContext, CG_DEFERRED_PARAMETER_SETTING);

		//Set register states
		cgGLRegisterStates(CgContext);

		//Manage texture parameters
		cgGLSetManageTextureParameters(CgContext, CG_TRUE);

		return true;
	}

	bool CShaderManager::CheckCgErrors()
	{
		/*
		CGerror CgError;
		const char *ErrorString = cgGetLastErrorString(&CgError);

		bool Return = true;

		if (CgError != CG_NO_ERROR)
		{
			Engine->Logger.AddEntry(ErrorString);

			if (CgError == CG_COMPILER_ERROR) Engine->Logger.AddEntry(cgGetLastListing(CgContext));

			ErrorString = cgGetLastErrorString(&CgError);
			
			Return = false;
		}
		*/

		return true;
	}

	CGcontext CShaderManager::GetContext()
	{
		return CgContext;
	}

	CShader::CShader()
	{
	}

	bool CShader::Load(std::string Filename)
	{
		CGFXFile = Filename;

		//Prefix filename with path from engine config
		Filename = Engine->ShadersPath + Filename;

		//Set Cg include directory to shader path
		std::string IncludeArg = "-I" + Engine->ShadersPath;
		const char* CgCompilerArgs[2] = { IncludeArg.c_str(), 0 };

		//Compile Cg program from file
		CgEffect = cgCreateEffectFromFile(Engine->ShaderManager.GetContext(), Filename.c_str(), CgCompilerArgs);

		//Check for errors
		if (!Engine->ShaderManager.CheckCgErrors())
		{
			Engine->Logger.AddEntry("Error creating Cg effect for " + CGFXFile);
			return false;
		}

		//Get first technique
		CgTechnique = cgGetFirstTechnique(CgEffect);

		if(cgValidateTechnique(CgTechnique) == CG_FALSE)
		{
			Engine->Logger.AddEntry("Could not validate Cg technique for " + CGFXFile);
			return false;
		}

		if (!Engine->ShaderManager.CheckCgErrors())
		{
			Engine->Logger.AddEntry("Error getting Cg technique for " + CGFXFile);
			return false;
		}

		//Get matrix handles
		ParamWorldMatrix = cgGetEffectParameterBySemantic(CgEffect, "WORLD");
		Engine->ShaderManager.CheckCgErrors();
		if(ParamWorldMatrix == 0) std::cout << "No parameter: World" << std::endl;

		ParamWorldViewProjMatrix = cgGetEffectParameterBySemantic(CgEffect, "WORLDVIEWPROJECTION");
		Engine->ShaderManager.CheckCgErrors();
		if(ParamWorldViewProjMatrix == 0) std::cout << "No parameter: WorldViewProjection" << std::endl;

		ParamWorldInvTransMatrix = cgGetEffectParameterBySemantic(CgEffect, "WORLDINVERSETRANSPOSE");
		Engine->ShaderManager.CheckCgErrors();
		if(ParamWorldInvTransMatrix == 0) std::cout << "No parameter: WorldInverseTranspose" << std::endl;

		ParamViewMatrix = cgGetEffectParameterBySemantic(CgEffect, "VIEW");
		Engine->ShaderManager.CheckCgErrors();
		if(ParamViewMatrix == 0) std::cout << "No parameter: View" << std::endl;

		ParamViewInverseMatrix = cgGetEffectParameterBySemantic(CgEffect, "VIEWINVERSE");
		Engine->ShaderManager.CheckCgErrors();
		if(ParamViewInverseMatrix == 0) std::cout << "No parameter: ViewInverse" << std::endl;

		ParamWorldViewMatrix = cgGetEffectParameterBySemantic(CgEffect, "WORLDVIEW");
		Engine->ShaderManager.CheckCgErrors();
		if(ParamWorldViewMatrix == 0) std::cout << "No parameter: WorldView" << std::endl;

		//Get other params
		ParamEyePos = cgGetEffectParameterBySemantic(CgEffect, "CAMERAPOSITION");
		Engine->ShaderManager.CheckCgErrors();
		if(ParamEyePos == 0) std::cout << "No parameter: CameraPosition" << std::endl;

		ParamDeltaTime = cgGetEffectParameterBySemantic(CgEffect, "DELTATIME");
		Engine->ShaderManager.CheckCgErrors();
		if(ParamDeltaTime == 0) std::cout << "No parameter: DeltaTime" << std::endl;

		ParamRunningTime = cgGetEffectParameterBySemantic(CgEffect, "RUNNINGTIME");
		Engine->ShaderManager.CheckCgErrors();
		if(ParamRunningTime == 0) std::cout << "No parameter: RunningTime" << std::endl;

		return true;
	}

	CGpass CShader::GetFirstPass()
	{
		//Get pass before setting params
		CGpass Pass = cgGetFirstPass(CgTechnique);

		//World
		if(ParamWorldMatrix != 0) cgGLSetStateMatrixParameter(ParamWorldMatrix, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);

		//World view projection
		if(ParamWorldViewProjMatrix != 0) cgGLSetStateMatrixParameter(ParamWorldViewProjMatrix, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);

		//World inverse transpose
		if(ParamWorldInvTransMatrix != 0) cgGLSetStateMatrixParameter(ParamWorldInvTransMatrix, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);

		//View
		if(ParamViewMatrix != 0) cgGLSetStateMatrixParameter(ParamViewMatrix, CG_GL_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);

		//View inverse
		if(ParamViewInverseMatrix != 0) cgGLSetStateMatrixParameter(ParamViewInverseMatrix, CG_GL_PROJECTION_MATRIX, CG_GL_MATRIX_INVERSE);

		//Worldview
		if(ParamWorldViewMatrix != 0) cgGLSetStateMatrixParameter(ParamWorldViewMatrix, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);

		//CamPos
		float EyePos[3];
		EyePos[0] = 0.0f;
		EyePos[1] = 0.0f;
		EyePos[2] = 0.0f;
		if(ParamEyePos != 0) cgSetParameter3fv(ParamEyePos, EyePos);

		//Set time
		float CgTime = (float)Engine->GetElapsedTime() / 1000.0f;
		if(ParamRunningTime != 0) cgSetParameter1fv(ParamRunningTime, &CgTime);

		CgTime = (float)Engine->GetDeltaTime() / 1000.0f;
		if(ParamDeltaTime != 0) cgSetParameter1fv(ParamDeltaTime, &CgTime);

		return Pass;
	}

	CGpass CShader::GetNextPass(CGpass CgPass)
	{
		return cgGetNextPass(CgPass);
	}

	void CShader::SetPass(CGpass CgPass)
	{
		cgSetPassState(CgPass);
	}

	void CShader::ResetPass(CGpass CgPass)
	{
		cgResetPassState(CgPass);
	}

	void CShader::Destroy()
	{
		cgDestroyEffect(CgEffect);
	}

	bool CShader::SetBoolParam(std::string Semantic, bool Value)
	{
		CGparameter Param = cgGetNamedEffectParameter(CgEffect, Semantic.c_str());

		if (!Engine->ShaderManager.CheckCgErrors())
		{
			Engine->Logger.AddEntry("Error setting parameter " + Semantic + " in effect file " + CGFXFile);
			return false;
		}

		if (!Param) return false;

		if(Value) cgSetParameter1f(Param, 1.0f);
		else cgSetParameter1f(Param, 0.0f);

		return true;
	}

	bool CShader::SetFloatParam(std::string Semantic, float Value)
	{
		CGparameter Param = cgGetNamedEffectParameter(CgEffect, Semantic.c_str());

		if (!Engine->ShaderManager.CheckCgErrors())
		{
			Engine->Logger.AddEntry("Error setting parameter " + Semantic + " in effect file " + CGFXFile);
			return false;
		}

		if (!Param) return false;

		cgSetParameter1fv(Param, &Value);

		return true;
	}

	bool CShader::SetFloat2Param(std::string Semantic, float Values[2])
	{
		CGparameter Param = cgGetNamedEffectParameter(CgEffect, Semantic.c_str());

		if (!Engine->ShaderManager.CheckCgErrors())
		{
			Engine->Logger.AddEntry("Error setting parameter " + Semantic + " in effect file " + CGFXFile);
			return false;
		}

		if (!Param) return false;

		cgSetParameter2fv(Param, Values);

		return true;
	}

	bool CShader::SetFloat3Param(std::string Semantic, float Values[3])
	{
		CGparameter Param = cgGetNamedEffectParameter(CgEffect, Semantic.c_str());

		if (!Engine->ShaderManager.CheckCgErrors())
		{
			Engine->Logger.AddEntry("Error setting parameter " + Semantic + " in effect file " + CGFXFile);
			return false;
		}

		if (!Param) return false;

		cgSetParameter3fv(Param, Values);

		return true;
	}

	bool CShader::SetFloat4Param(std::string Semantic, float Values[4])
	{
		CGparameter Param = cgGetNamedEffectParameter(CgEffect, Semantic.c_str());

		if (!Engine->ShaderManager.CheckCgErrors())
		{
			Engine->Logger.AddEntry("Error setting parameter " + Semantic + " in effect file " + CGFXFile);
			return false;
		}

		if (!Param) return false;

		cgSetParameter4fv(Param, Values);

		return true;
	}

	bool CShader::SetTextureParam(std::string Semantic, CTexture *Texture)
	{
		CGparameter Param = cgGetNamedEffectParameter(CgEffect, Semantic.c_str());

		if (!Engine->ShaderManager.CheckCgErrors())
		{
			Engine->Logger.AddEntry("Error setting parameter " + Semantic + " in effect file " + CGFXFile);
			return false;
		}

		if (!Param) return false;
		if (!Texture) return false;

		cgGLSetTextureParameter(Param, Texture->Texture);

		return true;
	}
}

