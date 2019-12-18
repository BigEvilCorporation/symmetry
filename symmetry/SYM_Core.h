#ifndef __SYM_CORE
#define __SYM_CORE

#ifdef WIN32
#include <windows.h>
#endif

#pragma comment(lib, "SDLmain.lib")
#pragma comment(lib, "sdl.lib")
#pragma comment(lib, "sdl_image.lib")
#pragma comment(lib, "sdl_ttf.lib")
#pragma comment(lib, "lua5.1.lib")
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "Newton.lib")
#pragma comment(lib, "jointLibrary.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "swscale.lib")
#pragma comment(lib, "cg.lib")
#pragma comment(lib, "cgGL.lib")
#pragma comment(lib, "tolua++.lib")
#pragma comment(lib, "fmodex_vc.lib")

#include "SYM_AABB.h"
#include "SYM_Animation.h"
#include "SYM_Audio.h"
#include "SYM_Brush.h"
#include "SYM_Camera.h"
#include "SYM_Character.h"
#include "SYM_Console.h"
#include "SYM_Debug.h"
#include "SYM_Engine.h"
#include "SYM_Font.h"
#include "SYM_Geometry.h"
#include "SYM_Input.h"
#include "SYM_Level.h"
#include "SYM_Logger.h"
#include "SYM_Material.h"
#include "SYM_Maths.h"
#include "SYM_Matrix.h"
#include "SYM_Mesh.h"
#include "SYM_Model.h"
#include "SYM_MPEG.h"
#include "SYM_Octree.h"
#include "SYM_Physics.h"
#include "SYM_PhysicsObject.h"
#include "SYM_Placeable.h"
#include "SYM_Quaternion.h"
#include "SYM_Resource.h"
#include "SYM_Rope.h"
#include "SYM_Scene.h"
#include "SYM_Script.h"
#include "SYM_SDL_GL_Utils.h"
#include "SYM_Shader.h"
#include "SYM_Skybox.h"
#include "SYM_Sprite.h"
#include "SYM_State.h"
#include "SYM_Terrain.h"
#include "SYM_Texture.h"
#include "SYM_Timer.h"
#include "SYM_Vector.h"

#endif