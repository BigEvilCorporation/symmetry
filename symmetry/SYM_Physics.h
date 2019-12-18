#ifndef __SYM_PHYSICS
#define __SYM_PHYSICS

#ifdef SYM_DLL
#   define __declspec(dllexport)
#else
//#   define __declspec(dllimport)
#endif

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <vector>

#include <newton/Newton.h>

#include "SYM_Maths.h"
#include "SYM_AABB.h"
#include "SYM_Material.h"

namespace sym
{
    //Newton callbacks
	static void GlobalPhysicsSetTransform(const NewtonBody* body, const dFloat* matrix, int threadIndex);
	static void GlobalLevelCollisionCallback (const NewtonBody* bodyWithTreeCollision, const NewtonBody* body,
										   const dFloat* vertex, int vertexstrideInBytes, int indexCount, const int* indexArray);
	static void GlobalContactCallback(const NewtonJoint* contactJoint, dFloat timestep, int threadIndex);

	static int GlobalMaterialAABBCallback(const NewtonMaterial* material, const NewtonBody* body0, const NewtonBody* body1, int threadIndex);
	static void GlobalMaterialCollisionCallback(const NewtonJoint* contactJoint, dFloat timestep, int threadIndex);

	static void GlobalApplyForceAndTorque(const NewtonBody* Body, dFloat timestep, int threadIndex);

	static int PhysicsIslandUpdate (const void* islandHandle, int bodyCount);
	static unsigned GetRunningTime();

    void DrawCollision (void* userData, int vertexCount, const dFloat* faceArray, int faceId);

    class CGeometry;
	class CPhysicsBody;

	struct RaycastData
	{
		float Distance;
		CVector3 HitPosition;
		CVector3 Normal;
		CGeometry *Geometry;
	};

	struct ExplosionData
	{
		float Strength;
		CVector3 Position;
	};

	class CPhysicsWorld
	{
		public:
			//Constructor
			CPhysicsWorld();

			//Init physics world
			bool Init();

			//Set active physics world size
			void SetWorldSize(CBoundingBox AABB);

			//Clear all physics bodies
			void ClearWorld();

			//Destroy physics world
			void Shutdown();

			//Update world
			void Update();

			//Run physics simulation
			void Run();

			//Stop the physics simulation
			void Stop();

			//Override the physics timestep
			void SetTimeStep(float Time) { AccumLimit = Time; }

			//Return the physics world
			NewtonWorld *GetWorld();

			//Perform a raycast
			RaycastData Raycast(CVector3 Start, CVector3 End);

			//World size
			CBoundingBox WorldAABB;

		private:
			//Newton physics world
			NewtonWorld* World;

			//Simulation is running
			bool IsRunning;

			//Physics update timestep
			float TimeStep;

			float AccumLimit;

			//Previous time
            float PrevTime;

			//Time accumulator
            float Accumulator;
	};

	class CPhysicsBody
	{
		public:
			//Constructor
			CPhysicsBody();

			//Destructor
			~CPhysicsBody();

			//Create physics body and collision cage
			bool CreateBox(CVector3 Dimentions);
			bool CreateSphere(CVector3 Dimentions);
			bool CreateCylinder(CVector3 Dimentions);
			bool CreateCapsule(CVector3 Dimentions);
			void CreateCustom(const std::vector<CVector3>& VertexList);

			//Create trigger volumes
			void CreateCylinderTrigger(CVector3 Dimentions);
			void CreateSphericalTrigger(CVector3 Dimentions);

			//Setup level collision geometry from a vertex list and face list
			void CreateLevel(const std::vector<CVector3>& VertexList, const std::vector<CFace>& FaceList, bool BackFaceCull = true);

			//Destroy body
			void DestroyNewtonBody();

			//Set geometry object
			void SetGeometry(CGeometry *Geom);

			//Matrix
			void SetNewtonMatrix(CMatrix4 Matrix);

			//Set mass
			void SetMass(float M);

			//Physics movement
			void SetVelocity(CVector3 Vel);
			void Impulse(CVector3 DeltaVelocity, CVector3 Centre);
			void SetForce(CVector3 F);
			void SetTorque(CVector3 T);
			void AddForce(CVector3 F);
			void AddTorque(CVector3 T);

			void SetAutoFreeze(bool Freeze);

			CVector3 GetVelocity();
			CVector3 GetOmega();

			//Draw the AABB
			void RenderCollision(CColour Colour);

			//Mass
			float Mass;

			//Get Newton body
			NewtonBody* GetNewtonBody() { if(RigidBody) return RigidBody; else return 0; }

			//Override callbacks
			void OverrideCallbackForceAndTorque(NewtonApplyForceAndTorque Callback);
			void OverrideCallbackSetTransform(NewtonSetTransform Callback);
			
			void SetMaterialAABBOverlapCallback(int MatID);
			void SetMaterialCollisionCallback(int MatID);
			void SetForceTorqueCallback();
			virtual void MaterialAABBOverlapCallback(CPhysicsBody *HitBody) {}
			virtual void MaterialCollisionCallback(CPhysicsBody *HitBody, CVector3 HitPosition, CVector3 Normal) {}
			virtual void ApplyForceAndTorque() {}

			//Callbacks
			static void PhysicsApplyForceAndTorque(const NewtonBody* Body, dFloat timestep, int threadIndex);

			//Create a unique material
			int CreateNewtonMaterial();

			//Set material
			void SetNewtonMaterial(int ID);

			//Get material
			int GetNewtonMaterial() { return MaterialID; }

		//protected:

			//Newton rigid body
			NewtonBody* RigidBody;

			//Newton collision object
			NewtonCollision* Collision;

            //Inertia
            float Inertia[3];

            //Bounding box
			float AABBMin[3];
			float AABBMax[3];

			//Force and torque to be applied next callback
			CVector3 Force;
			CVector3 Torque;

			//Material
			int MaterialID;
	};

} //Namespace

#endif

