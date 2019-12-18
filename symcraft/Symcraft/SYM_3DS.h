
#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>

#include "../../symmetry/SYM_Maths.h"
#include "../../symmetry/SYM_Mesh.h"

#define MAX_VERTICES	10000
#define MAX_FACES		10000
#define MAX_KEYFRAMES	32
#define MAX_KEYS		32

//3DS Chunks
#define MAIN3DS			0x4D4D
#define MAIN_VERS		0x0002
#define EDIT3DS			0x3D3D
#define MESH_VERS		0x3D3E
#define MASTER_SCALE	0x0100

#define MATERIAL3DS		0xAFFF
#define MAT_NAME3DS		0xA000
#define MAT_AMBIENT		0xA010
#define MAT_DIFFUSE		0xA020
#define MAT_SPECULAR	0xA030
#define SHINY_PERC		0xA040
#define SHINY_STR_PERC	0xA041
#define TRANS_PERC		0xA050
#define TRANS_FOFF_PERC	0xA052
#define REF_BLUR_PERC	0xA053
#define RENDER_TYPE		0xA100
#define SELF_ILLUM		0xA084
#define MAT_SELF_ILPCT	0xA08A
#define WIRE_THICKNESS	0xA087
	
#define MAT_TEXMAP		0xA200
#define MAT_MAPNAME	0xA300
#define MAT_UTILE	0xA354
#define MAT_VTILE	0xA356
#define MAT_UOFFSET	0xA358
#define MAT_VOFFSET	0xA35A
		
#define MAT_BUMPMAP		0xA230
#define MAT_MAPNAME	0xA300
#define MAT_UTILE	0xA354
#define MAT_VTILE	0xA356
#define MAT_UOFFSET	0xA358
#define MAT_VOFFSET	0xA35A

#define OBJECT3DS		0x4000
#define TRIG_MESH		0x4100
#define VERT_LIST		0x4110
#define FACE_DESC		0x4120
#define FACE_MAT		0x4130
#define TEX_VERTS		0x4140
#define SMOOTH_GROUP	0x4150
#define LOCAL_COORDS	0x4160

#define KEYFRAME3DS		0xB000
#define FRAMES			0xB008
#define MESH_INFO		0xB002
#define OBJNAME		0xB010
#define PIVOT_PT	0xB013
#define TRACK_POS	0xB020
#define TRACK_ROT	0xB021
#define TRACK_SCL	0xB022
#define NODE_ID		0xB030

#define	COLOR_RGB		0x0010
#define COLOR_TRU		0x0011
#define COLOR_TRUG		0x0012
#define COLOR_RGBG		0x0013
#define PERC_INT		0x0030
#define PERC_FLOAT		0x0031

//TODO: Add *UNKNOWN* chunk types

//Colour/light types
#define COLOUR_AMBIENT 1
#define COLOUR_DIFFUSE 2
#define COLOUR_SPECULAR 3

//Map types
#define TEXMAP 1
#define BUMPMAP 2

using namespace std;

class SYM_3DS_UVCOORD
{
	public:
		float u, v;
};

class SYM_3DS_VERTEX
{
	public:
		float x, y, z;

		SYM_3DS_UVCOORD UVCoord;
};

class SYM_3DS_POLYGON
{
	public:
		int a, b, c;
};

class SYM_3DS_FACE
{
	public:
		SYM_3DS_VERTEX Vertex[3];
		float Normal[3];
};

class SYM_3DS_COLOUR
{
	public:
		int r, g, b;

		int rgb[3];
};

class SYM_3DS_MATERIAL
{
	public:
		string Name;
		string TextureFile;
		string BumpMapFile;
		int Texture;
		int BumpMap;

		//Texture tiling
		float TileU, TileV;

		bool HasTexture;

		SYM_3DS_COLOUR Ambient;
		SYM_3DS_COLOUR Diffuse;
		SYM_3DS_COLOUR Specular;

		int Shininess;
		int Transparency;
};

class SYM_3DS_MODEL
{
	public:
		//Model name
		string Name;

		//Material name
		string MaterialName;

		//Material number
		int MaterialNumber;

		//The material face index list, max 10, 000
		int MaterialFaces[MAX_FACES];

		//Number of model polygons, vertices and materials
		short int NumPolygons;
		short int NumVertices;
		short int NumMappedFaces;

		//Model has material
		bool HasMaterial;
	
		//Model has texture
		bool HasTexture;

		int Texture;

		//Vertex list, max 32,000
		SYM::SYM_VECTOR3 Vertex[MAX_VERTICES];

		//Polygon list, max 10,000
		SYM_3DS_POLYGON Polygon[MAX_FACES];

		//Organised triangle faces, max 10,000
		SYM::SYM_FACE Face[MAX_FACES];

		//Material UVCoords, max 32,000
		SYM_3DS_UVCOORD UVCoord[MAX_VERTICES];

		//Model position
		float xpos;
		float ypos;
		float zpos;
	
		//Model rotation
		float xrot;
		float yrot;
		float zrot;
};

class SYM_3DS_KEY_POS
{
	public:
		unsigned short FrameNum;
		long int Unknown_Data;
		float PosX, PosY, PosZ;
};

class SYM_3DS_TRACK_POS
{
	public:
		unsigned short Flags;
		unsigned short Unknown_Data[4];
		unsigned short NumKeys;
		unsigned short Unknown_Data2;

		SYM_3DS_KEY_POS Keys[MAX_KEYS];
};

class SYM_3DS_KEY_ROT
{
	public:
		unsigned short FrameNum;
		long int Unknown_Data;
		float RotationRadian;
		float AxisX, AxisY, AxisZ; 
};

class SYM_3DS_TRACK_ROT
{
	public:
		unsigned short Flags;
		unsigned short Unknown_Data[4];
		unsigned short NumKeys;
		unsigned short Unknown_Data2;

		SYM_3DS_KEY_ROT Keys[MAX_KEYS];
};


class SYM_3DS_KEY_SCL
{
	public:
		unsigned short FrameNum;
		long int Unknown_Data;
		float ScaleX, ScaleY, ScaleZ;
};

class SYM_3DS_TRACK_SCL
{
	public:
		unsigned short Flags;
		unsigned short Unknown_Data[4];
		unsigned short NumKeys;
		unsigned short Unknown_Data2;

		SYM_3DS_KEY_SCL Keys[MAX_KEYS];
};

class SYM_3DS_OBJINFO
{
	public:
		string Name;
		float PivotX, PivotY, PivotZ;
		SYM_3DS_TRACK_POS Track_Position;
		SYM_3DS_TRACK_ROT Track_Rotation;
		SYM_3DS_TRACK_SCL Track_Scale;

		unsigned short Node_ID;
};

class SYM_3DS_KEYFRAME
{
	public:
		
		unsigned short Unknown_Data[3];
		unsigned int StartFrame, Endframe;
		unsigned short Flags;
		
		int NumObjInfo;

		SYM_3DS_OBJINFO ObjInfo[256];
};

class SYM_3DS
{
	public:
		//Model name
		string Name;
		
		//Number of models/materials loaded
		int NumModels;
		int NumMaterials;
		int NumKeyframes;
		
		//3D Model list, max 32
		SYM_3DS_MODEL Model[16];

		//Material list, max 32
		SYM_3DS_MATERIAL Material[8];

		SYM_3DS_KEYFRAME Keyframe[MAX_KEYFRAMES];
	
		//Functions
		bool Load3DSFile(string Filename);// Loads in a 3DS file, returns 0 if failed

	private:
		//unsigned int GLlistNumber;
};
