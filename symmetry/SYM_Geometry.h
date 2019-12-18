#ifndef __SYM_GEOMETRY
#define __SYM_GEOMETRY

#ifdef SYM_DLL
#   define __declspec(dllexport)
#else
//#   define __declspec(dllimport)
#endif

#include <vector>

#include "SYM_PhysicsObject.h"
#include "SYM_Maths.h"
#include "SYM_Mesh.h"
#include "SYM_Octree.h"

namespace sym
{
	const int SYM_MDL		= 0;
	const int SYM_BOX		= 1;
	const int SYM_PLANE		= 2;
	const int SYM_CYLINDER	= 3;
	const int SYM_SPHERE	= 4;
	const int SYM_TERR		= 5;
	const int SYM_CUST_BRUSH = 6;

	//Base class for all renderable geometry
	class CGeometry : public CPhysicsObject
	{
		public:
			//Constructor
			CGeometry();
			~CGeometry();

			//Render()
			void Render();
			void RenderWireframe(CColour Colour);
			void RenderCollision(CColour Colour);

			//Scale
			void Scale(CVector3 Scl);

			//Physics
			void SetVelocity(CVector3 Vel);
			void SetMass(float M);
			void SetFriction(float F);
			void SetBounciness(float B);

			CVector3 GetVelocity();
			CVector3 GetOmega();

			//Material
			bool SetMaterial(std::string Filename);
			bool SetMaterial(CMaterial *Mat);

			//Bounding boxes
			CBoundingBox GetLocalAABB();
			CBoundingBox GetWorldAABB();
			void RecalculateAABB();

			//Rendering options
			bool Visible;
			bool CastShadows;
			bool ReceiveShadows;
			bool Collidable;
			bool Physics;

			//Get dimentions
			CVector3 GetDimentions();

			//Add and remove nodes from octree node list
			void AddOctreeNode(COctreeNode *Node);
			void DeleteOctreeNode(COctreeNode *Node);
			void ClearOctreeNodes();
			void RemoveFromOctree();

			//Notify octree nodes that this object has moved
			void HasMoved();

			//Geometry type
			int Type;

			//Filenames
			std::string ModelFilename;
			std::string MatFilename;
			std::string HMapFilename;

			//Texture tiling/offset/rotation
			CVector3 TextureTile;
			CVector3 TextureOffset;
			CVector3 TextureRotation;

			//Don't use backface culling
			bool DoubleSided;

			//Mesh
			CMesh *Mesh;

			//Application-specific custom data
			int CustomData;

		protected:
			//Brush dimentions
			CVector3 Dimentions;

			//Pointers to octree nodes
			std::vector<COctreeNode *> OctreeNodes;
	};

} //Namespace

#endif
