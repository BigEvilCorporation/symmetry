#include <iostream>

#include "SYM_SDL_GL_Utils.h"
#include "SYM_Font.h"
#include "SYM_Engine.h"

int TickCounter = 0;
int FPSCounter = 0;
int CurrentFPS = 0;
long LastTick;

//OpenGL extensions
PFNGLACTIVETEXTUREARBPROC glActiveTextureARB;
PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB;
PFNGLGENFRAMEBUFFERSEXTPROC glGenFramebuffersEXT;
PFNGLBINDFRAMEBUFFEREXTPROC glBindFramebufferEXT;
PFNGLGENRENDERBUFFERSEXTPROC glGenRenderbuffersEXT;
PFNGLBINDRENDERBUFFEREXTPROC glBindRenderbufferEXT;
PFNGLRENDERBUFFERSTORAGEEXTPROC glRenderbufferStorageEXT;
PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC glFramebufferRenderbufferEXT;
PFNGLFRAMEBUFFERTEXTURE2DEXTPROC glFramebufferTexture2DEXT;
PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC glCheckFramebufferStatusEXT;
PFNGLDELETEFRAMEBUFFERSEXTPROC glDeleteFramebuffersEXT;
PFNGLDELETERENDERBUFFERSEXTPROC glDeleteRenderbuffersEXT;
PFNGLDRAWBUFFERSPROC glDrawBuffers;

namespace sym
{
	bool SDL_GL_InitSDL(int Width, int Height, bool Fullscreen, std::string Title)
	{
		//Fire up SDL, check for errors
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) != 0)
		{
			Engine->Logger.AddEntry("Error: Could not initialise SDL: " + (std::string)SDL_GetError());
			return false;
		}

		//Get video info
		const SDL_VideoInfo *VideoInfo = SDL_GetVideoInfo();
		if(!VideoInfo)
		{
			Engine->Logger.AddEntry("Error: Could not query video info: " + (std::string)SDL_GetError());
			return false;
		}

		//Get screen BPP
		int BPP = VideoInfo->vfmt->BitsPerPixel;

		//List of video flags
		int VideoFlags;

		//Add video flags
		VideoFlags  = SDL_OPENGL;
		VideoFlags |= SDL_HWPALETTE;
		VideoFlags |= SDL_HWSURFACE;
		VideoFlags |= SDL_HWACCEL;
		VideoFlags |= SDL_ANYFORMAT;
		VideoFlags |= SDL_OPENGLBLIT;

		//Add fullscreen flag if enabled
		if (Fullscreen) VideoFlags |= SDL_FULLSCREEN;

		//Use double buffer
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, true);

		//Set depth buffer
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

		//Framebuffer colour components
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);

		//Set video mode
		if (SDL_SetVideoMode(Width, Height, BPP, VideoFlags) == 0)
		{
			Engine->Logger.AddEntry("Error: Unable to set video mode: " + (std::string)SDL_GetError());
			return false;
		}

		//Show the cursor
		SDL_ShowCursor(SDL_DISABLE);

		//Set window caption
		SDL_WM_SetCaption(Title.c_str(), Title.c_str());

		return true;
	}

	void GLResizeScene(int Width, int Height)
	{
		//Protect against divide-by-zero error, make sure Height is at least 1
		if (Height== 0) Height = 1;

		//Set the viewport
		glViewport(0, 0, Width, Height);

		//Select projection matrix
		glMatrixMode(GL_PROJECTION);

		//Reset projection matrix
		glLoadIdentity();

		//Calculate aspect ratio
		gluPerspective(45.0f, (GLfloat)Width / (GLfloat)Height, 0.1f, 20000.0f);

		//Select the modelview matrix
		glMatrixMode(GL_MODELVIEW);

		//Reset the modelview matrix
		glLoadIdentity();
	}

	bool SDL_GL_InitGL(int Width, int Height)
	{
		//Background colour
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

		//Enable depth testing
		glEnable(GL_DEPTH_TEST);

		//Depth buffer setup
		glClearDepth(1.0f);
		glDepthFunc(GL_LEQUAL);

		//Enable back face culling
		glEnable(GL_CULL_FACE);

		//Shading and perspective
		glShadeModel(GL_SMOOTH);
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

		//Set blending mode
		glBlendFunc(GL_ONE, GL_ONE);

		//Calculate lighting for visible faces only
		glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

		//Make sure glColors still work with lighting
		glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
		glEnable(GL_COLOR_MATERIAL);

		//Flip all texture drawing (SDL textures are upside down to OpenGL)
		glMatrixMode(GL_TEXTURE);
		glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
		glScalef(-1.0f, 1.0f, 1.0f);
		glMatrixMode(GL_MODELVIEW);

		GLResizeScene(Width, Height);

		return true;
	}

	bool CheckGLErrors()
	{
		switch(glGetError())
		{
			case GL_NO_ERROR:
				return true;
				break;

			case GL_INVALID_ENUM:
				Engine->Logger.AddEntry("Error: OpenGL returned GL_INVALID_ENUM");
				return false;
				break;

			case GL_INVALID_VALUE:
				Engine->Logger.AddEntry("Error: OpenGL returned GL_INVALID_VALUE");
				return false;
				break;

			case GL_INVALID_OPERATION:
				Engine->Logger.AddEntry("Error: OpenGL returned GL_INVALID_OPERATION");
				return false;
				break;

			case GL_STACK_OVERFLOW:
				Engine->Logger.AddEntry("Error: OpenGL returned GL_STACK_OVERFLOW");
				return false;
				break;

			case GL_STACK_UNDERFLOW:
				Engine->Logger.AddEntry("Error: OpenGL returned GL_STACK_UNDERFLOW");
				return false;
				break;

			case GL_OUT_OF_MEMORY:
				Engine->Logger.AddEntry("Error: OpenGL returned GL_OUT_OF_MEMORY");
				return false;
				break;

			default:
				Engine->Logger.AddEntry("Error: OpenGL returned an unknown error");
				return false;
				break;
		}
	}

	void SDL_GL_InitGLExt()
	{
		Engine->Logger.AddEntry("Getting OpenGL extension addresses");
		glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC) wglGetProcAddress("glActiveTextureARB");
		glClientActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC) wglGetProcAddress("glClientActiveTextureARB");
		glGenFramebuffersEXT = (PFNGLGENFRAMEBUFFERSEXTPROC) wglGetProcAddress("glGenFramebuffersEXT");
		glBindFramebufferEXT = (PFNGLBINDFRAMEBUFFEREXTPROC) wglGetProcAddress("glBindFramebufferEXT");
		glGenRenderbuffersEXT = (PFNGLGENRENDERBUFFERSEXTPROC) wglGetProcAddress("glGenRenderbuffersEXT");
		glBindRenderbufferEXT = (PFNGLBINDRENDERBUFFEREXTPROC) wglGetProcAddress("glBindRenderbufferEXT");
		glRenderbufferStorageEXT = (PFNGLRENDERBUFFERSTORAGEEXTPROC) wglGetProcAddress("glRenderbufferStorageEXT");
		glFramebufferRenderbufferEXT = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC) wglGetProcAddress("glFramebufferRenderbufferEXT");
		glFramebufferTexture2DEXT = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC) wglGetProcAddress("glFramebufferTexture2DEXT");
		glCheckFramebufferStatusEXT = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC) wglGetProcAddress("glCheckFramebufferStatusEXT");
		glDeleteFramebuffersEXT = (PFNGLDELETEFRAMEBUFFERSEXTPROC) wglGetProcAddress("glDeleteFramebuffersEXT");
		glDeleteRenderbuffersEXT = (PFNGLDELETERENDERBUFFERSEXTPROC) wglGetProcAddress("glDeleteRenderbuffersEXT");
		glDrawBuffers = (PFNGLDRAWBUFFERSPROC) wglGetProcAddress("glDrawBuffers");

		if(!glActiveTextureARB) Engine->Logger.AddEntry("Error: glActiveTextureARB not available");
		if(!glClientActiveTextureARB) Engine->Logger.AddEntry("Error: glClientActiveTextureARB not available");
		if(!glGenFramebuffersEXT) Engine->Logger.AddEntry("Error: glGenFramebuffersEXT not available");
		if(!glBindFramebufferEXT) Engine->Logger.AddEntry("Error: glBindFramebufferEXT not available");
		if(!glGenRenderbuffersEXT) Engine->Logger.AddEntry("Error: glGenRenderbuffersEXT not available");
		if(!glBindRenderbufferEXT) Engine->Logger.AddEntry("Error: glBindRenderbufferEXT not available");
		if(!glRenderbufferStorageEXT) Engine->Logger.AddEntry("Error: glRenderbufferStorageEXT not available");
		if(!glFramebufferRenderbufferEXT) Engine->Logger.AddEntry("Error: glFramebufferRenderbufferEXT not available");
		if(!glFramebufferTexture2DEXT) Engine->Logger.AddEntry("Error: glFramebufferTexture2DEXT not available");
		if(!glCheckFramebufferStatusEXT) Engine->Logger.AddEntry("Error: glCheckFramebufferStatusEXT not available");
		if(!glDeleteFramebuffersEXT) Engine->Logger.AddEntry("Error: glDeleteFramebuffersEXT not available");
		if(!glDeleteRenderbuffersEXT) Engine->Logger.AddEntry("Error: glDeleteRenderbuffersEXT not available");
		if(!glDrawBuffers) Engine->Logger.AddEntry("Error: glDrawBuffers not available");
	}

	void glEnable2D()
	{
		int vPort[4];

		glDisable(GL_CULL_FACE);

		glGetIntegerv(GL_VIEWPORT, vPort);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();

		glOrtho(0, vPort[2], 0, vPort[3], -1, 1);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
	}

	void glDisable2D()
	{
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();

		glEnable(GL_CULL_FACE);
	}

	void StartRendering()
	{
		//Clear screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void FinishRendering()
	{
		SDL_GL_SwapBuffers();
	}

	void CalculateFPS()
	{
		long ElapsedTicks = SDL_GetTicks() - LastTick;
		LastTick = SDL_GetTicks();

		TickCounter += ElapsedTicks;

		FPSCounter++;
		if ( TickCounter >= 1000 )
		{
			CurrentFPS = FPSCounter;
			FPSCounter = 0;
			TickCounter = 0;
		}
	}

	int GetFPS()
	{
		return CurrentFPS;
	}

	//Returns a pixel from an SDL surface
	Uint32 SDL_GetPixel(SDL_Surface *Surface, int x, int y)
	{
		//Get BPP
		int BPP = Surface->format->BytesPerPixel;

		//Get pointer to pixel
		Uint8 *Pixel = (Uint8 *)Surface->pixels + y * Surface->pitch + x * BPP;

		switch(BPP)
		{
			case 1:
				return *Pixel;

			case 2:
				return *(Uint16 *)Pixel;

			case 3:
				if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
					return Pixel[0] << 16 | Pixel[1] << 8 | Pixel[2];
				else
					return Pixel[0] | Pixel[1] << 8 | Pixel[2] << 16;

			case 4:
				return *(Uint32 *)Pixel;

			default:
				return 0;
		}
	}

	void Draw_Grid(float r, float g, float b, GLfloat size, GLfloat resolution)
	{
		glColor3f(r,g,b);
		for(GLfloat i = -size; i <= size; i += resolution)
		{
			glBegin(GL_LINES);
			glVertex3f(-size, 0, i);
			glVertex3f(size, 0, i);
			glVertex3f(i, 0,-size);
			glVertex3f(i, 0, size);
			glEnd();
		}

		glColor3f(1.0,1.0,1.0);
	}

	CFramebuffer::CFramebuffer()
	{
		GLFrameBuffer = GLDepthBuffer = 0;
		HasDepth = false;
	}

	CFramebuffer::~CFramebuffer()
	{
	}

	bool CFramebuffer::Init(int Width, int Height, bool Depth)
	{
		//Keep depth choice for shutdown
		HasDepth = Depth;

		//Initialise FBO
		glGenFramebuffersEXT(1, &GLFrameBuffer);

		if(Depth)
		{
			//Setup depth buffer
			glGenRenderbuffersEXT(1, &GLDepthBuffer);
			glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, GLDepthBuffer);
			glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, Width, Height);

			//Attach depth buffer to FBO
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, GLFrameBuffer);
			glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, GLDepthBuffer);
		}

		//Create colour target textures
		for(int i = 0; i < 4; i++) ColourTarget[i].Create(Width, Height, GL_RGBA8);

		//Attach colour target textures to FBO
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, ColourTarget[0].Texture, 0);
		//glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_2D, ColourTarget[1].Texture, 0);
		//glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT2_EXT, GL_TEXTURE_2D, ColourTarget[2].Texture, 0);
		//glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT3_EXT, GL_TEXTURE_2D, ColourTarget[3].Texture, 0);

		//Check for error
		if(glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) != GL_FRAMEBUFFER_COMPLETE_EXT) return false;

		//Unbind FBO
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

		return true;
	}	

	void CFramebuffer::Shutdown()
	{
		glDeleteFramebuffersEXT(1, &GLFrameBuffer);
		if(HasDepth) glDeleteRenderbuffersEXT(1, &GLDepthBuffer);
	}

	void CFramebuffer::Bind()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, GLFrameBuffer);
		glPushAttrib(GL_VIEWPORT_BIT | GL_COLOR_BUFFER_BIT);

		//Set all 4 MRT's
		//GLenum Buffers[] = { GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_COLOR_ATTACHMENT2_EXT, GL_COLOR_ATTACHMENT3_EXT };
		GLenum Buffers[] = { GL_COLOR_ATTACHMENT0_EXT };
		glDrawBuffers(1, Buffers);
	}

	void CFramebuffer::Unbind()
	{
		glPopAttrib();
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	}

	sym::CTexture* CFramebuffer::GetColourTarget(int TargetNum)
	{
		if(TargetNum < 4) return &ColourTarget[TargetNum];
		else return 0;
	}

	sym::CTexture* CFramebuffer::GetDepthTarget()
	{
		if(HasDepth) return &DepthTarget;
		else return 0;
	}

} //Namespace
