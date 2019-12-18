#pragma once

#include <map>
#include <vector>
#include <string>

#define S_ROOT_SECTION		0x0001	//No data, defines whether or not this is a .sym.scene file

#define S_CUSTOM_BRUSH      0xF300  //No data, header to a brush
#define     S_BRUSH_NAME    0xF301  //'\0' terminated CHAR string, name of model
#define     S_MATERIAL      0xF302  //'\0' terminated CHAR string, filename of material
#define     S_BRUSH_POS	    0xF303  //3x FLOAT, model position, X, Y and Z
#define     S_BRUSH_ROT	    0xF304  //3x FLOAT, model rotation, X, Y and Z

#define		S_MODEL_PHYSICS	0xC007	//int, physics type

#define 	NUM_VERTICES	0xF305 //UNSIGNED LONG INT, number of vertices in this object (also number of UV coords)
#define 	NUM_FACES		0xF306 //UNSIGNED LONG INT, number of faces in this object
#define 	VERTICES_LIST	0xF307 //4x FLOAT + INT (x NumVertices), vertex X, Y, Z
#define 	FACE_LIST		0xF308 //3x UNSIGNED LONG INT (x NumFaces), a face consisting of vertex number A, B and C

#define S_LIGHT			    0xD000  //No data, header to a light
#define     S_LIGHT_NAME	0xD001  //'\0' terminated CHAR string, name of light
#define     S_LIGHT_TYPE    0xD002  //UNSIGNED SHORT INT, light type
#define     S_LIGHT_COLOUR	0xD003  //3x UNSIGNED SHORT INT, colour R, G and B (0 - 255)
#define     S_LIGHT_POS	    0xD004  //3x FLOAT, light position X, Y and Z
#define     S_LIGHT_ROT	    0xD005  //3x FLOAT, light rotation, X, Y and Z
#define		S_LIGHT_ATT_NM	0xD006	//Float, Near Min attenuation
#define		S_LIGHT_ATT_NX	0xD006	//Float, Near Max attenuation
#define		S_LIGHT_ATT_FM	0xD006	//Float, Far Min attenuation
#define		S_LIGHT_ATT_FX	0xD006	//Float, Far Max attenuation

namespace sym
{
	class CUVCoord
	{
		public:
			CUVCoord() { u = v = 0.0f; }

			float u, v;
	};

	class CFace
	{
		public:
			CFace()
			{
				Indices[0] = Indices[1] = Indices[2] = 0;
			}

			CFace(int A, int B, int C)
			{
				Indices[0] = A;
				Indices[1] = B;
				Indices[2] = C;
			}

			CFace(int A, int B, int C, CUVCoord UVs[2], CUVCoord UVC)
			{
				Indices[0] = A;
				Indices[1] = B;
				Indices[2] = C;
				UVCoords[0] = UVs[0];
				UVCoords[1] = UVs[1];
				UVc = UVC;
			}

			unsigned long Indices[3];

			CUVCoord UVCoords[2], UVc;
	};

	class CQuaternion
	{
		public:
			CQuaternion()
			{
				x = 0.0f;
				y = 0.0f;
				z = 0.0f;
				w = 0.0f;
			}

			CQuaternion(float X, float Y, float Z, float W)
			{
				x = X;
				y = Y;
				z = Z;
				w = W;
			}

			float x, y, z, w;
	};

	class CVector3
	{
		public:
			CVector3() { x = y = z = -1;}
			CVector3(float X, float Y, float Z) { x = X; y = Y; z = Z; }

			float x, y, z;
	};

	class CCustomBrush
	{
		public:
			//Name
			std::string Name;

			//Vertex list
			std::vector<CVector3> Vertices;

			//List of faces
			std::vector<CFace> Faces;

			//Material name
			std::string MaterialName;

			//Position
			CVector3 Position;

			//Rotation
			CQuaternion Rotation;

			//Texture tile
			float TileU, TileV;

			//Texture offset
			float OffsetU, OffsetV;
	};

	class CScene
	{
		public:
            int Write(std::string Filename);

			//Container of geometry
			std::vector<CCustomBrush> Geometry;
	};
} //Namespace
