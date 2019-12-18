#ifndef __SYM_SDL_GL_Utils
#define __SYM_SDL_GL_Utils

#ifdef SYM_DLL
#   define __declspec(dllexport)
#else
//#   define __declspec(dllimport)
#endif

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>	// Header File For The OpenGL32 Library
#include <GL/glu.h>	// Header File For The GLu32 Library
#include <GL/glext.h>

#include <SDL/SDL.h> // main SDL header

#include "SYM_Texture.h"

//OpenGL extensions
extern PFNGLACTIVETEXTUREARBPROC glActiveTextureARB;
extern PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB;
extern PFNGLGENFRAMEBUFFERSEXTPROC glGenFramebuffersEXT;
extern PFNGLBINDFRAMEBUFFEREXTPROC glBindFramebufferEXT;
extern PFNGLGENRENDERBUFFERSEXTPROC glGenRenderbuffersEXT;
extern PFNGLBINDRENDERBUFFEREXTPROC glBindRenderbufferEXT;
extern PFNGLRENDERBUFFERSTORAGEEXTPROC glRenderbufferStorageEXT;
extern PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC glFramebufferRenderbufferEXT;
extern PFNGLFRAMEBUFFERTEXTURE2DEXTPROC glFramebufferTexture2DEXT;
extern PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC glCheckFramebufferStatusEXT;
extern PFNGLDELETEFRAMEBUFFERSEXTPROC glDeleteFramebuffersEXT;
extern PFNGLDELETERENDERBUFFERSEXTPROC glDeleteRenderbuffersEXT;
extern PFNGLDRAWBUFFERSPROC glDrawBuffers;

namespace sym
{
	bool SDL_GL_InitSDL(int Width, int Height, bool Fullscreen, std::string Title);
	bool SDL_GL_InitGL(int Width, int Height);
	void GLResizeScene(int Width, int Height);

	void SDL_GL_InitGLExt();

	void glEnable2D();
	void glDisable2D();

	void StartRendering();
	void FinishRendering();

	bool CheckGLErrors();

	void CalculateFPS();
	int GetFPS();

	//Returns a pixel from an SDL surface
	Uint32 SDL_GetPixel(SDL_Surface *Surface, int x, int y);

	void Draw_Grid(float r, float g, float b, GLfloat size, GLfloat resolution);

	class CFramebuffer
	{
		public:
			CFramebuffer();
			~CFramebuffer();

			//Init/shutdown
			bool Init(int Width, int Height, bool Depth);
			void Shutdown();

			//Bind/unbind
			void Bind();
			void Unbind();

			//Get colour target
			sym::CTexture *GetColourTarget(int TargetNum);
			sym::CTexture *GetDepthTarget();

		private:
			sym::CTexture ColourTarget[4], DepthTarget;
			GLuint GLFrameBuffer, GLDepthBuffer;
			bool HasDepth;
	};

} //Namespace

#endif
