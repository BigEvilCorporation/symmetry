#ifndef __SYM_OCTREE
#define __SYM_OCTREE

#ifdef SYM_DLL
#   define __declspec(dllexport)
#else
//#   define __declspec(dllimport)
#endif

#include <string>
#include <vector>
#include <list>
#include <map>

#include "SYM_Maths.h"
#include "SYM_Mesh.h"
#include "SYM_AABB.h"

namespace sym
{
    class CGeometry;
    class COctreeNode
    {
        public:
            //Constructors
            COctreeNode();
            COctreeNode(CBoundingBox Bounds, COctreeNode *ParentNode);
            ~COctreeNode();

            //Insert geometry
            void AddGeometry(CGeometry *Geom);

			//Remove geometry
			void RemoveGeometry(CGeometry *Geom);

            //Builds a list of renderable geometry
            void BuildRenderList(std::vector<CGeometry*> &GeomList, CFrustum *Frustum);

            //Clear octree
            void Clear();

             //Check if this node is in the view frustum
            bool InFrustum();

            //Render octree
            void Render(CFrustum *Frustum);

            //List of child nodes
            std::list<COctreeNode*> ChildNodes;

            //Parent node
            COctreeNode *Parent;

            //Vector of geometry in this node
            std::vector<CGeometry*> Geometry;

            //Object moved callback
            void GeomHasMoved(CGeometry* Geom);

            //Bounding box
            CBoundingBox AABB;

        protected:
            //Split this node
            void Split();

            //Distribute geometry
            void Distribute();

             //Maximum number of geom objects allowed in this node before it is split
            int MaxGeometry;

            //Node ID
            int NodeID;
    };

    class COctree : public COctreeNode
    {
        public:
            //Creates the tree using a geometry map
            void Create(std::map<std::string, CGeometry*>& GeomList);

            //Get node count
            int GetNodeCount();
    };

} //Namespace

#endif
