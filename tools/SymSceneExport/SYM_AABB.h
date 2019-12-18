#ifndef __SYM_BOUNDINGBOX
#define __SYM_BOUNDINGBOX

#include "SYM_Vector.h"

namespace sym
{
    class CBoundingBox
	{
	    public:
            //Constructors
            CBoundingBox();
            CBoundingBox(CVector3 Mn, CVector3 Mx);

            //Add a vertex
			void AddVertex(CVector3 Vertex);

			//Merge with another bounding box
			void Merge(CBoundingBox *AABB);

			//Make cuboid
			void MakeCube();

			//Checks if objects are contained with this AABB
			bool Contains(CVector3 Vector);
			bool Contains(CBoundingBox *AABB);

			//Draw
			void Render();

			//AABB
			CVector3 Centre;
			CVector3 Min;
			CVector3 Max;
			CVector3 Size;
	};

} //Namespace

#endif //__CBoundingBox
