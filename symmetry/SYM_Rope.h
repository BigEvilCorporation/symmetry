#ifndef __SYM_ROPE
#define __SYM_ROPE

#ifdef SYM_DLL
#   define __declspec(dllexport)
#else
//#   define __declspec(dllimport)
#endif

#include <vector>
#include <string>
#include <sstream>
#include <newton/Newton.h>

#include "SYM_Engine.h"
#include "SYM_Geometry.h"

namespace sym
{
    class CRope
    {
        public:
			CRope();
			~CRope();

            //Create rope links and attach joints
			void Create(CVector3 Start, CVector3 End, float Width);
			void Create(CVector3 Start, CVector3 End, float Width, int NumLinks);

            //Attach start/end of rope to another geometry
            void AttachStart(CGeometry *Geom, CVector3 Position);
            void AttachEnd(CGeometry *Geom, CVector3 Position);

            //Detach rope
            void DetachStart();
            void DetachEnd();

            //Set rope material
            void SetMaterial(CMaterial *Material);

            //Move rope
            void Move(CVector3 Vector);

            //Get start/end position
            CVector3 GetStartPosition();
            CVector3 GetEndPosition();

			CBrush *GetStartLink();
			CBrush *GetEndLink();

			//Disable collision with geometry
			void DisableCollision(CGeometry *Geom);

			//Render
			void Render();

			//Get number of links
			int GetNumLinks();

			//Get force applied to joint
			float GetJointForce(int Joint);

			//Break a joint
			void BreakJoint(int Joint);

			 std::vector<CBrush*> Links;

        private:

			//Vector of links
           
			std::vector<NewtonJoint*> Joints;

            //Start and end joints
            NewtonJoint *StartJoint, *EndJoint;

			//Material
			int MaterialID;

			//Cone restriction
			float ConeRestrictionDeg;

			//Twist restriction
			float TwistRestrictionDeg;

			//Joint Stiffness
			float JointStiffness;

			CVector3 Start, End;
    };
}

#endif // __SYM_ROPE
