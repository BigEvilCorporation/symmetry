#ifndef __SYM_MESH
#define __SYM_MESH

#ifdef SYM_DLL
#   define __declspec(dllexport)
#else
//#   define __declspec(dllimport)
#endif

#ifdef WIN32
#include <windows.h>
#endif

#include <string>
#include <vector>
#include <map>

#include <GL/gl.h>
#include <GL/glext.h>

#include "SYM_Placeable.h"
#include "SYM_Animation.h"
#include "SYM_Material.h"
#include "SYM_Maths.h"
#include "SYM_AABB.h"
#include "SYM_Shader.h"

#define ROOT_SECTION		0x0001 //No data, defines whether or not this is a .sym.mesh file

#define OBJECT				0xB000 //No data, header to an object
#define 	OBJ_NAME		0xB001 //'\0' terminated CHAR string, name of object
#define 	OBJ_TYPE		0xB002 //UNSIGNED SHORT INT, type of object (See above)
#define 	NUM_VERTICES	0xB003 //UNSIGNED LONG INT, number of vertices in this object (also number of UV coords)
#define 	NUM_FACES		0xB004 //UNSIGNED LONG INT, number of faces in this object
#define 	VERTICES_LIST	0xB006 //5x FLOAT (x NumVertices), vertex X, Y and Z plus UV Coords X and Y
#define 	FACE_LIST		0xB007 //3x UNSIGNED LONG INT (x NumFaces), a face consisting of vertex number A, B and C
#define		OBJ_MAT_NAME	0xB008 //'\0' terminated CHAR string, filename of material

#define	BONE				0xC000 //No data, header to a bone
#define		BONE_ID			0xC001 //Int, bone ID
#define		BONE_NAME		0xC002 //'\0' terminated string
#define		BONE_POS		0xC003 //3 x float
#define		BONE_ROT		0xC004 //3 x float
#define		BONE_PARENT		0xC005 //Int, ID of parent bone

namespace sym
{
	class CFont;

	class CTexCoord
	{
		public:
			//Constructor
			CTexCoord()
			{
				u = v = 0;
			}

			CTexCoord(float U, float V)
			{
				u = U;
				v = V;
			}

			//U and V coordinates
			float u, v;
	};

	class CFace
	{
		public:
			//Constructor
			CFace()
			{
				Indices[0] = 0;
				Indices[1] = 0;
				Indices[2] = 0;
			}

			CFace(int A, int B, int C)
			{
				Indices[0] = A;
				Indices[1] = B;
				Indices[2] = C;
			}

			CFace(int A, int B, int C, CTexCoord UVA, CTexCoord UVB, CTexCoord UVC)
			{
				Indices[0] = A;
				Indices[1] = B;
				Indices[2] = C;
				UVCoords[0] = UVA;
				UVCoords[1] = UVB;
				UVCoords[2] = UVC;
			}

			//Indices
			unsigned long Indices[3];

			//UV coords
			CTexCoord UVCoords[3];

			//Tangents
			CVector3 Tangents[3];

			//Binormals
			CVector3 Binormals[3];

			//Face normal
			CVector3 Normal;
	};

	class CBone : public CPlaceable
	{
		public:
			CBone()
			{
				ID = -1;
				ParentID = -1;
				ParentBone = 0;
			}

			//Bone ID
			int ID;

			//Parent bone
			int ParentID;
			CBone *ParentBone;
			std::vector<CBone*> ChildBones;
			
			//Pointers to vertices affected by this bone
			std::vector<CVector3*> Vertices;

			//Traverse the bone tree, transform each bone
			void Traverse();

			CMatrix4 OriginalMatrix;
			CMatrix4 CurrentKey;
			CMatrix4 UserMatrix;
	};

	class CMeshSubset : public CPlaceable
	{
		public:
			//Constructor
			CMeshSubset();
			~CMeshSubset();

			//Name
			std::string Name;

			//Render
			void Render();

			//Render wireframe
			void RenderWireframe(CColour Colour);

			//Process vertex/face list into GL arrays, calculate normals, fetch material
			void Process();

			//Vertex list
			std::vector <CVector3> Vertices;
			std::vector <CVector3> OrigVertices;

			//List of faces
			std::vector <CFace> Faces;

			//Pointer to bones vector
			std::vector <CBone> *OriginalBones;
			std::vector <CBone> *Bones;

			//Set material
			bool SetMaterial(std::string Filename);
			bool SetMaterial(CMaterial *Mat);

			//Material name
			std::string MaterialName;

			//Material
			CMaterial *Material;

			CShader AnimShader;

		private:
			//Internal draw
			void Draw();

			//Vertex array
			GLfloat *VertexArray;

			//Face index array
			GLuint *FaceIndexArray;

			//Vertex normals array
			GLfloat *VertexNormalArray;

			//UV coords array
			GLfloat *TexCoordArray;

			//Bone ID array
			GLfloat *BoneIDArray;

			//Vertex weights array
			GLfloat *VertexWeightArray;
	};

	class CMesh
	{
		public:
			CMesh();
			~CMesh();

			//Load from .sym.mesh file
			bool Load(std::string Filename);
			bool Write(std::string Filename);

			//Create and return a new subset
			CMeshSubset *NewSubset();

			//Scale vertices
			void Scale(CVector3 Scl);

			//Set global material
			bool SetMaterial(std::string Filename);
			bool SetMaterial(CMaterial *Mat);

			//Render
			void Render();

			//Render outline
			void RenderWireframe(CColour Colour);

			//Render bones
			void RenderBones(CColour Colour);

			//Transform all vertices
			void TranslateVertices(CVector3 Translation);

			//Mesh subsets
			std::vector<CMeshSubset> Subsets;

			//Original bone setup as loaded from .sym.mesh file
			std::vector<CBone> OriginalBones;

			//Current bone positions for animation
			std::vector<CBone> Bones;

			//Get bone by name
			CBone *GetBoneByName(std::string BoneName);

			//Get subset by name
			CMeshSubset *GetSubsetByName(std::string SubsetName);

			//Bounding box
			CBoundingBox AABB;
			
			//Animation functions
			void BindAnimation(CSkeletalAnimation *Anim);
			void NextFrame();
			void PrevFrame();
			void StartAnimation();
			void StopAnimation();
			void ResetAnimation();
			void SetAnimSpeed(float Speed);
			float GetAnimationTime();
			void SetAnimationTime(float Time);
			void SetAnimationLooping(bool Looping);

			//Set depth testing
			void SetDepthTest(bool Depth);

		private:

			//Current animation
			CSkeletalAnimation *CurrentAnimation;

			//Animation speed
			float AnimationSpeed;

			//Current animation time
			float CurrentKeyframeTime;

			//Is animation running
			bool AnimationRunning;

			//Is animation looping
			bool AnimationLoop;

			CFont *Font;

			//Render with depth testing
			bool DepthTest;
	};

} //Namespace

#endif

