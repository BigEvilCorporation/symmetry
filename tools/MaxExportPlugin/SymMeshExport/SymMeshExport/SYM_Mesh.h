#ifndef __SYM_MESH
#define __SYM_MESH

#include <string>
#include <vector>
#include <map>

//#include "SYM_Material.h"

#define ROOT_SECTION		0x0001 //No data, defines whether or not this is a .sym.mesh file

#define OBJECT				0xB000 //No data, header to an object
#define 	OBJ_NAME		0xB001 //'\0' terminated CHAR string, name of object
#define 	OBJ_TYPE		0xB002 //UNSIGNED SHORT INT, type of object (See above)
#define 	NUM_VERTICES	0xB003 //UNSIGNED LONG INT, number of vertices in this object (also number of UV coords)
#define 	NUM_FACES		0xB004 //UNSIGNED LONG INT, number of faces in this object
#define 	VERTICES_LIST	0xB006 //4x FLOAT + INT (x NumVertices), vertex X, Y, Z, Weight, BoneID
#define 	FACE_LIST		0xB007 //3x UNSIGNED LONG INT (x NumFaces), a face consisting of vertex number A, B and C
#define		OBJ_MAT_NAME	0xB008 //'\0' terminated CHAR string, filename of material

#define	BONE				0xC000 //No data, header to a bone
#define		BONE_ID			0xC001 //Int, bone ID
#define		BONE_NAME		0xC002 //'\0' terminated string
#define		BONE_POS		0xC003 //3 x float
#define		BONE_ROT		0xC004 //4 x float - quaternion
#define		BONE_PARENT		0xC005 //Int, ID of parent bone

namespace SYM
{
	class SYM_UVCOORD
	{
		public:
			SYM_UVCOORD() { u = v = 0.0f; }

			float u, v;
	};

	class SYM_FACE
	{
		public:
			SYM_FACE()
			{
				Indices[0] = Indices[1] = Indices[2] = 0;
			}

			SYM_FACE(int A, int B, int C)
			{
				Indices[0] = A;
				Indices[1] = B;
				Indices[2] = C;
			}

			SYM_FACE(int A, int B, int C, SYM_UVCOORD UVs[2], SYM_UVCOORD UVC)
			{
				Indices[0] = A;
				Indices[1] = B;
				Indices[2] = C;
				UVCoords[0] = UVs[0];
				UVCoords[1] = UVs[1];
				UVc = UVC;
			}

			unsigned long Indices[3];

			SYM_UVCOORD UVCoords[2], UVc;
	};

	class SYM_QUATERNION
	{
		public:
			SYM_QUATERNION()
			{
				x = 0.0f;
				y = 0.0f;
				z = 0.0f;
				w = 0.0f;
			}

			SYM_QUATERNION(float X, float Y, float Z, float W)
			{
				x = X;
				y = Y;
				z = Z;
				w = W;
			}

			float x, y, z, w;
	};

	class SYM_VECTOR3
	{
		public:
			SYM_VECTOR3()
			{
				x = y = z = 1.0f;

				for(int i = 0; i < 4; i++)
				{
					BoneIDs[i] = -1;
					Weights[i] = 0.0f;
				}
			}

			SYM_VECTOR3(float X, float Y, float Z)
			{
				x = X; y = Y; z = Z;

				for(int i = 0; i < 4; i++)
				{
					BoneIDs[i] = -1;
					Weights[i] = 0.0f;
				}
			}

			float x, y, z;
			int BoneIDs[4];
			float Weights[4];
	};

	class SYM_BONE
	{
		public:
			SYM_BONE()
			{
				ParentID = -1;
			}

			//Bone ID
			int ID;

			//Bone name
			std::string Name;

			//Position/rotation
			SYM_VECTOR3 Position;
			SYM_QUATERNION Rotation;

			//Parent/child bones
			int ParentID;
			std::vector<SYM_BONE*> Children;
	};

	class SYM_MESHSUBSET
	{
		public:
			//Constructor
			//SYM_MESHSUBSET() { Material = 0; }

			//Name
			std::string Name;

			//Vertex list
			std::vector<SYM_VECTOR3> Vertices;

			//List of faces
			std::vector<SYM_FACE> Faces;

			//Material name
			std::string MaterialName;

			//Material
			//SYM_MATERIAL *Material;
	};

	class SYM_MESH
	{
		public:
			//Load from .sym.mesh file
			int Write(std::string Filename);

			//Create and return a new subset
			SYM_MESHSUBSET *NewSubset();

			//Mesh subsets
			std::vector<SYM_MESHSUBSET> Subsets;

			//Bones
			std::map<int, SYM_BONE> Bones;
	};

} //Namespace

#endif

