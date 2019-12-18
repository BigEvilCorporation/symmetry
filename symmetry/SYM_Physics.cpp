#include <sstream>

#include "SYM_Physics.h"
#include "SYM_Maths.h"
#include "SYM_Engine.h"

namespace sym
{
	RaycastData RayHitData;

	// memory allocation for Newton
	void*  PhysicsAlloc(int sizeInBytes)
	{
		return malloc(sizeInBytes);
	}

	// memory de-allocation for Newton
	void  PhysicsFree(void *ptr, int sizeInBytes)
	{
		free (ptr);
	}

	//Transformation callback
	void  GlobalPhysicsSetTransform(const NewtonBody* body, const dFloat* matrix, int threadIndex)
    {
		//If this is a trigger volume, ignore
		if(NewtonCollisionIsTriggerVolume(NewtonBodyGetCollision(body))) return;

        //Convert dFloat matrix
        CMatrix4 Mat((float*)matrix);

        //Get geometry
        CGeometry *Geometry = 0;
        Geometry = (CGeometry*)NewtonBodyGetUserData(body);

        //Check geometry exists
        if(Geometry)
        {
            //Translate geometry
            Geometry->SetMatrix(Mat);
        }
        else
        {
            Engine->Logger.AddEntry("Error: PhysicsSetTransform called with no geometry set");
        }
    }

	//Collision callback
	void GlobalCollisionCallback(NewtonUserMeshCollisionCollideDesc* collideDescData)
	{
	    //Get geometry
        CGeometry *Geometry = 0;
        Geometry = (CGeometry*)collideDescData->m_userData;

        //Check geometry exists
        if(Geometry)
        {
            //Build argument list
            std::vector<const void*> Args;
            Args.push_back(Geometry->Name.c_str());

            //Call global script function
            //Engine->ScriptManager.CallGlobalFunction("OnCollision", "s", Args);
        }
        else
        {
            Engine->Logger.AddEntry("Error: LevelCollisionCallback called with no geometry set");
        }
	}

    //Brush collision callback
	void GlobalLevelCollisionCallback(const NewtonBody* bodyWithTreeCollision, const NewtonBody* body, const dFloat* vertex,
									int vertexstrideInBytes, int indexCount, const int* indexArray)
	{
	    //Get geometry
        CGeometry *Geometry = 0;
        Geometry = (CGeometry*)NewtonBodyGetUserData(body);

        CGeometry *Geometry2 = 0;
        Geometry2 = (CGeometry*)NewtonBodyGetUserData(bodyWithTreeCollision);

        //Check geometry exists
        if(Geometry && Geometry2)
        {
            //Build argument list
            std::vector<const void*> Args;
            Args.push_back(Geometry->Name.c_str());
            Args.push_back(Geometry2->Name.c_str());

            //Call global script function
            Engine->ScriptManager.CallGlobalFunction("OnCollision", "ss", Args);
        }
        else
        {
            Engine->Logger.AddEntry("Error: LevelCollisionCallback called with no geometry set");
        }
	}

	void GlobalContactCallback(const NewtonJoint* contactJoint, dFloat timestep, int threadIndex)
	{

	}

	int GlobalMaterialAABBCallback(const NewtonMaterial* material, const NewtonBody* body0, const NewtonBody* body1, int threadIndex)
	{
		CGeometry *Geom1 = static_cast<CGeometry*>(NewtonBodyGetUserData(body0));
		CGeometry *Geom2 = static_cast<CGeometry*>(NewtonBodyGetUserData(body1));

		CPhysicsBody *Body1 = dynamic_cast<CPhysicsBody*>(Geom1);
		CPhysicsBody *Body2 = dynamic_cast<CPhysicsBody*>(Geom2);

		Body1->MaterialAABBOverlapCallback(Body2);
		Body2->MaterialAABBOverlapCallback(Body1);

		return 0;
	}

	static void GlobalMaterialCollisionCallback(const NewtonJoint* contactJoint, dFloat timestep, int threadIndex)
	{
		NewtonBody *Body1 = NewtonJointGetBody0(contactJoint);
		NewtonBody *Body2 = NewtonJointGetBody1(contactJoint);

		CGeometry *Geom1 = static_cast<CGeometry*>(NewtonBodyGetUserData(Body1));
		CGeometry *Geom2 = static_cast<CGeometry*>(NewtonBodyGetUserData(Body2));

		CPhysicsBody *SymBody1 = dynamic_cast<CPhysicsBody*>(Geom1);
		CPhysicsBody *SymBody2 = dynamic_cast<CPhysicsBody*>(Geom2);

		CVector3 Position, Normal;
		void* Contact = NewtonContactJointGetFirstContact (contactJoint);
		NewtonMaterial* Material = NewtonContactGetMaterial (Contact);
		if(Material) NewtonMaterialGetContactPositionAndNormal(Material, &Position.x, &Normal.x);

		if(SymBody1) SymBody1->MaterialCollisionCallback(SymBody2, Position, Normal);
		if(SymBody2) SymBody2->MaterialCollisionCallback(SymBody1, Position, Normal);
	}

	void GlobalApplyForceAndTorque(const NewtonBody* Body, dFloat timestep, int threadIndex)
	{
		CGeometry *Geom = static_cast<CGeometry*>(NewtonBodyGetUserData(Body));

		CPhysicsBody *PhysicsBody = dynamic_cast<CPhysicsBody*>(Geom);

		PhysicsBody->ApplyForceAndTorque();
	}

	unsigned GetRunningTime()
	{
		return (unsigned)Engine->GetElapsedTime();
	}

	float RaycastCallback(const NewtonBody* body, const dFloat* hitNormal, int collisionID, void* userData, dFloat intersetParam)
	{
		RayHitData.Geometry = (CGeometry*)NewtonBodyGetUserData(body);
		RayHitData.Normal = CVector3(hitNormal[0], hitNormal[1], hitNormal[2]);
		RayHitData.Distance = intersetParam;

		return intersetParam;
	}

	int PhysicsIslandUpdate (const void* islandHandle, int bodyCount)
	{
		return 1;
	}

	CPhysicsWorld::CPhysicsWorld()
	{
		//Set default values
		IsRunning = false;

		TimeStep = 0.08f;
        PrevTime = 0.0f;
		Accumulator = 0.0f;
		AccumLimit = 0.25f;

		World = 0;
	}

	void CPhysicsWorld::SetWorldSize(CBoundingBox AABB)
	{
	    WorldAABB = AABB;

	    float WorldMin[3];
	    float WorldMax[3];

		WorldMin[0] = WorldAABB.Min.x - 10;
		WorldMin[1] = WorldAABB.Min.y - 10;
		WorldMin[2] = WorldAABB.Min.z - 10;

		WorldMax[0] = WorldAABB.Max.x + 10;
		WorldMax[1] = WorldAABB.Max.y + 10;
		WorldMax[2] = WorldAABB.Max.z + 10;

		std::stringstream Stream;
		Stream << "Setting physics world size to MIN(" << WorldAABB.Min.x << ", " << WorldAABB.Min.y << ", " << WorldAABB.Min.z
                                            << ") MAX(" << WorldAABB.Max.x << ", " << WorldAABB.Max.y << ", " << WorldAABB.Max.z << ")";

		Engine->Logger.AddEntry(Stream.str());

		NewtonSetWorldSize(World, WorldMin, WorldMax);
	}

	bool CPhysicsWorld::Init()
	{
		//Create the physics world
		World = NewtonCreate(PhysicsAlloc, PhysicsFree);

		//Check world was created
		if(!World)
		{
		    Engine->Logger.AddEntry("Error: Physics world creation failed");
		    return false;
		}

		NewtonSetPerformanceClock (World, GetRunningTime);
		NewtonSetIslandUpdateEvent(World, PhysicsIslandUpdate);

		// get the default material ID
		int defaultID;
		defaultID = NewtonMaterialGetDefaultGroupID(World);

		/*
		// set default material properties
		NewtonMaterialSetDefaultSoftness(World, defaultID, defaultID, 0.01f);
		NewtonMaterialSetDefaultElasticity(World, defaultID, defaultID, 0.01f);
		NewtonMaterialSetDefaultCollidable(World, defaultID, defaultID, 1);
		NewtonMaterialSetDefaultFriction(World, defaultID, defaultID, 1.0f, 0.5f);
		NewtonMaterialSetCollisionCallback(World, defaultID, defaultID, NULL, NULL, GlobalContactCallback); 
		NewtonMaterialSetSurfaceThickness(World, defaultID, defaultID, 1.0f);
		*/

		//Set default size
		CBoundingBox Bounds;
		Bounds.AddVertex(CVector3(-10000, -10000, -10000));
		Bounds.AddVertex(CVector3(10000, 10000, 10000));
		SetWorldSize(Bounds);

		return true;
	}

	void CPhysicsWorld::ClearWorld()
	{
	    //Destroy all bodies
	    NewtonDestroyAllBodies(World);
	}

	void CPhysicsWorld::Shutdown()
	{
	    //Clear world
	    ClearWorld();

        //Release Newton world
        NewtonDestroy(World);
        World = 0;
	}

	void CPhysicsWorld::Run()
	{
		IsRunning = true;
	}

	void CPhysicsWorld::Stop()
	{
		IsRunning = false;
	}

	void CPhysicsWorld::Update()
	{
	    //If the physics world is running
		if(IsRunning)
		{
			//Make sure the NewtonWorld exists
			if(World)
			{
				float CurrentTime = (float)Engine->GetElapsedTime();
				float DeltaTime = CurrentTime - PrevTime;
				PrevTime = CurrentTime;

				Accumulator += DeltaTime;
				if(Accumulator > AccumLimit) Accumulator = AccumLimit;

				
				while(Accumulator >= TimeStep)
				{
					NewtonUpdate(World, TimeStep);
					Accumulator -= TimeStep;
				}

				NewtonUpdate(World, Accumulator);
				Accumulator = 0.0f;
			}
            else Engine->Logger.AddEntry("Error: Physics update called while World doesn't exist");
		}
	}

	NewtonWorld * CPhysicsWorld::GetWorld()
	{
		return World;
	}

	RaycastData CPhysicsWorld::Raycast(CVector3 Start, CVector3 End)
	{
		RayHitData.Distance = 0.0f;
		RayHitData.Geometry = 0;
		RayHitData.Normal = CVector3();
		RayHitData.HitPosition = CVector3();

		NewtonWorldRayCast(World, &Start.x, &End.x, RaycastCallback, 0, NULL);

		RayHitData.HitPosition = Start.Lerp(End, RayHitData.Distance);

		return RayHitData;
	}

	CPhysicsBody::CPhysicsBody()
	{
		//Set default values
		Mass = 2.0f;
		RigidBody = 0;
	}

	CPhysicsBody::~CPhysicsBody()
	{
	    if(RigidBody)
		{
			NewtonDestroyBody(Engine->Physics.GetWorld(), RigidBody);
			RigidBody = 0;
		}
	}

	bool CPhysicsBody::CreateBox(CVector3 Dimentions)
	{
		//Create collision object
		Collision = NewtonCreateBox(Engine->Physics.GetWorld(), Dimentions.x, Dimentions.y, Dimentions.z, 0);

		//Create rigid body
		if(RigidBody) NewtonDestroyBody(Engine->Physics.GetWorld(), RigidBody);
		RigidBody = NewtonCreateBody(Engine->Physics.GetWorld(), Collision);

		//Set tranform callback
		NewtonBodySetTransformCallback(RigidBody, GlobalPhysicsSetTransform);

		//Set force and torque callback to rigid body
		OverrideCallbackForceAndTorque(PhysicsApplyForceAndTorque);

		//NewtonBodySetCollisionCollideCallback(CollisionCallback);

		//Calculate inertia
        float Origin[3];
        NewtonConvexCollisionCalculateInertialMatrix(Collision, Inertia, Origin);

		//Set the correct center of gravity
		NewtonBodySetCentreOfMass (RigidBody, Origin);

		//Set the mass matrix
		SetMass(Mass);

		//Get AABB
		NewtonBodyGetAABB(RigidBody, AABBMin, AABBMax);

		//Release collision object
		NewtonReleaseCollision(Engine->Physics.GetWorld(), Collision);
		Collision = 0;

		//Set material
		NewtonBodySetMaterialGroupID(RigidBody, NewtonMaterialGetDefaultGroupID(Engine->Physics.GetWorld()));

		return true;
	}

	bool CPhysicsBody::CreateSphere(CVector3 Dimentions)
	{
		//Create collision object
		Collision = NewtonCreateSphere(Engine->Physics.GetWorld(), Dimentions.x/2, Dimentions.y/2, Dimentions.z/2, 0);

		//Create rigid body
		if(RigidBody) NewtonDestroyBody(Engine->Physics.GetWorld(), RigidBody);
		RigidBody = NewtonCreateBody(Engine->Physics.GetWorld(), Collision);

		//Set tranform callback
		NewtonBodySetTransformCallback(RigidBody, GlobalPhysicsSetTransform);

		//Set force and torque callback to rigid body
		OverrideCallbackForceAndTorque(PhysicsApplyForceAndTorque);

		//NewtonBodySetCollisionCollideCallback(CollisionCallback);

		//Calculate inertia
        float Origin[3];
        NewtonConvexCollisionCalculateInertialMatrix(Collision, Inertia, Origin);

		//Set the correct center of gravity
		NewtonBodySetCentreOfMass (RigidBody, Origin);

		//Set initial mass
		SetMass(Mass);

		//Get AABB
		NewtonBodyGetAABB(RigidBody, AABBMin, AABBMax);

		//Release collision object
		NewtonReleaseCollision(Engine->Physics.GetWorld(), Collision);
		Collision = 0;

		//Set material
		NewtonBodySetMaterialGroupID(RigidBody, NewtonMaterialGetDefaultGroupID (Engine->Physics.GetWorld()));

		return true;
	}

	bool CPhysicsBody::CreateCylinder(CVector3 Dimentions)
	{
		//Create collision object
		Collision = NewtonCreateCylinder(Engine->Physics.GetWorld(), Dimentions.x/2, Dimentions.y, 0);

		//Create rigid body
		if(RigidBody) NewtonDestroyBody(Engine->Physics.GetWorld(), RigidBody);
		RigidBody = NewtonCreateBody(Engine->Physics.GetWorld(), Collision);

		//Set tranform callback
		NewtonBodySetTransformCallback(RigidBody, GlobalPhysicsSetTransform);

		//Set force and torque callback to rigid body
		OverrideCallbackForceAndTorque(PhysicsApplyForceAndTorque);

		//NewtonBodySetCollisionCollideCallback(CollisionCallback);

		//Calculate inertia
        float Origin[3];
        NewtonConvexCollisionCalculateInertialMatrix(Collision, Inertia, Origin);

		//Set the correct center of gravity
		NewtonBodySetCentreOfMass (RigidBody, Origin);

		//Set initial mass
		SetMass(Mass);

		//Get AABB
		NewtonBodyGetAABB(RigidBody, AABBMin, AABBMax);

		//Release collision object
		NewtonReleaseCollision(Engine->Physics.GetWorld(), Collision);
		Collision = 0;

		//Set material
		NewtonBodySetMaterialGroupID(RigidBody, NewtonMaterialGetDefaultGroupID (Engine->Physics.GetWorld()));

		return true;
	}

	bool CPhysicsBody::CreateCapsule(CVector3 Dimentions)
	{
		//Create collision object
		sym::CMatrix4 Offset;
		Offset.SetRotation(90.0f, sym::CVector3(0, 0, 1));
		Collision = NewtonCreateCapsule(Engine->Physics.GetWorld(), Dimentions.x * 0.5f, Dimentions.z * 2.0f, Offset.GetAsFloatArray());

		//Create rigid body
		if(RigidBody) NewtonDestroyBody(Engine->Physics.GetWorld(), RigidBody);
		RigidBody = NewtonCreateBody(Engine->Physics.GetWorld(), Collision);

		//Set tranform callback
		NewtonBodySetTransformCallback(RigidBody, GlobalPhysicsSetTransform);

		//Set force and torque callback to rigid body
		OverrideCallbackForceAndTorque(PhysicsApplyForceAndTorque);

		//NewtonBodySetCollisionCollideCallback(CollisionCallback);

		//Calculate inertia
        float Origin[3];
        NewtonConvexCollisionCalculateInertialMatrix(Collision, Inertia, Origin);

		//Set the correct center of gravity
		NewtonBodySetCentreOfMass (RigidBody, Origin);

		//Set initial mass
		SetMass(Mass);

		//Get AABB
		NewtonBodyGetAABB(RigidBody, AABBMin, AABBMax);

		//Release collision object
		NewtonReleaseCollision(Engine->Physics.GetWorld(), Collision);
		Collision = 0;

		//Set material
		NewtonBodySetMaterialGroupID(RigidBody, NewtonMaterialGetDefaultGroupID (Engine->Physics.GetWorld()));

		return true;
	}

	void CPhysicsBody::CreateCylinderTrigger(CVector3 Dimentions)
	{
		//Create collision object
		Collision = NewtonCreateCylinder(Engine->Physics.GetWorld(), Dimentions.x/2, Dimentions.y, 0);

		//Create dummy rigid body
		if(RigidBody) NewtonDestroyBody(Engine->Physics.GetWorld(), RigidBody);
		RigidBody = NewtonCreateBody(Engine->Physics.GetWorld(), Collision);

		//Set collision as trigger volume
		NewtonCollisionSetAsTriggerVolume(Collision, 1);

		//Create unique material
		CreateNewtonMaterial();

		//Zero mass
		SetMass(0.0f);
	}

	void CPhysicsBody::CreateSphericalTrigger(CVector3 Dimentions)
	{
		//Create collision object
		Collision = NewtonCreateSphere(Engine->Physics.GetWorld(), Dimentions.x, Dimentions.y, Dimentions.z, 0);

		//Create dummy rigid body
		if(RigidBody) NewtonDestroyBody(Engine->Physics.GetWorld(), RigidBody);
		RigidBody = NewtonCreateBody(Engine->Physics.GetWorld(), Collision);

		//Set collision as trigger volume
		NewtonCollisionSetAsTriggerVolume(Collision, 1);

		//Create unique material
		CreateNewtonMaterial();

		//Zero mass
		SetMass(0.0f);
	}

	void CPhysicsBody::CreateCustom(const std::vector<CVector3>& VertexList)
	{
		//Create collision object
		float *Vertices = new float[VertexList.size() * 3];
		int VertCount = 0;
		for(int i = 0; i < (int)VertexList.size(); i++)
		{
			Vertices[VertCount++] = VertexList[i].x;
			Vertices[VertCount++] = VertexList[i].y;
			Vertices[VertCount++] = VertexList[i].z;
		}

		Collision = NewtonCreateConvexHull(Engine->Physics.GetWorld(), (int)VertexList.size(), Vertices, 3 * sizeof (float), 0.0f, NULL);

		//Create rigid body
		if(RigidBody) NewtonDestroyBody(Engine->Physics.GetWorld(), RigidBody);
		RigidBody = NewtonCreateBody(Engine->Physics.GetWorld(), Collision);

		//Set tranform callback
		NewtonBodySetTransformCallback(RigidBody, GlobalPhysicsSetTransform);

		//Set force and torque callback to rigid body
		OverrideCallbackForceAndTorque(PhysicsApplyForceAndTorque);

		//NewtonBodySetCollisionCollideCallback(CollisionCallback);

		//Calculate inertia
        float Origin[3];
        NewtonConvexCollisionCalculateInertialMatrix(Collision, Inertia, Origin);

		//Set the correct center of gravity
		NewtonBodySetCentreOfMass (RigidBody, Origin);

		//Set initial mass
		SetMass(Mass);

		//Get AABB
		NewtonBodyGetAABB(RigidBody, AABBMin, AABBMax);

		//Release collision object
		NewtonReleaseCollision(Engine->Physics.GetWorld(), Collision);
		Collision = 0;

		//Set material
		NewtonBodySetMaterialGroupID(RigidBody, NewtonMaterialGetDefaultGroupID (Engine->Physics.GetWorld()));
	}

	void CPhysicsBody::CreateLevel(const std::vector<CVector3>& VertexList, const std::vector<CFace>& FaceList, bool BackFaceCull)
	{
		//New collision tree
		Collision = NewtonCreateTreeCollision(Engine->Physics.GetWorld());

		//Start building collision tree
		NewtonTreeCollisionBeginBuild(Collision);

			//Loop through faces
			for(int i = 0; i < (int)FaceList.size(); i++)
			{
				//The face in an array - 3x xyz
				float Face[9];

				Face[0] = VertexList[FaceList[i].Indices[0]].x;
				Face[1] = VertexList[FaceList[i].Indices[0]].y;
				Face[2] = VertexList[FaceList[i].Indices[0]].z;

				Face[3] = VertexList[FaceList[i].Indices[1]].x;
				Face[4] = VertexList[FaceList[i].Indices[1]].y;
				Face[5] = VertexList[FaceList[i].Indices[1]].z;

				Face[6] = VertexList[FaceList[i].Indices[2]].x;
				Face[7] = VertexList[FaceList[i].Indices[2]].y;
				Face[8] = VertexList[FaceList[i].Indices[2]].z;

				//Add the face
				NewtonTreeCollisionAddFace(Collision, 3, Face, 12, 0);

				/*
				if(!BackFaceCull)
				{
					Face[0] = VertexList[FaceList[i].Indices[2]].x;
					Face[1] = VertexList[FaceList[i].Indices[2]].y;
					Face[2] = VertexList[FaceList[i].Indices[2]].z;

					Face[3] = VertexList[FaceList[i].Indices[1]].x;
					Face[4] = VertexList[FaceList[i].Indices[1]].y;
					Face[5] = VertexList[FaceList[i].Indices[1]].z;

					Face[6] = VertexList[FaceList[i].Indices[0]].x;
					Face[7] = VertexList[FaceList[i].Indices[0]].y;
					Face[8] = VertexList[FaceList[i].Indices[0]].z;

					//Add the face
					NewtonTreeCollisionAddFace(Collision, 3, Face, 12, 0);
				}
				*/
			}

		//Finish building collision tree
		NewtonTreeCollisionEndBuild(Collision, 0);

		//Create the rigid body
		if(RigidBody) NewtonDestroyBody(Engine->Physics.GetWorld(), RigidBody);
		RigidBody = NewtonCreateBody(Engine->Physics.GetWorld(), Collision);

		//Set tranform callback
		//NewtonBodySetTransformCallback(RigidBody, GlobalPhysicsSetTransform);

		//Set force and torque callback to rigid body
		//OverrideCallbackForceAndTorque(PhysicsApplyForceAndTorque);

		//Release the collision object
		NewtonReleaseCollision(Engine->Physics.GetWorld(), Collision);
		Collision = 0;

		//Get AABB
		NewtonBodyGetAABB(RigidBody, AABBMin, AABBMax);

		//Set material
		NewtonBodySetMaterialGroupID(RigidBody, NewtonMaterialGetDefaultGroupID (Engine->Physics.GetWorld()));
	}

	void CPhysicsBody::DestroyNewtonBody()
	{
		if(RigidBody) NewtonDestroyBody(Engine->Physics.GetWorld(), RigidBody);
		RigidBody = 0;
	}

	void CPhysicsBody::SetGeometry(CGeometry *Geom)
	{
	    if(RigidBody) NewtonBodySetUserData(RigidBody, Geom);
	}

	void CPhysicsBody::SetMass(float M)
	{
		Mass = M;

		//Set the mass distribution matrix
		if(RigidBody) NewtonBodySetMassMatrix(RigidBody, Mass, Inertia[0], Inertia[1], Inertia[2]);
	}

	void CPhysicsBody::SetNewtonMatrix(CMatrix4 Matrix)
	{
		if(RigidBody) NewtonBodySetMatrix(RigidBody, Matrix.GetAsFloatArray());
	}

	int CPhysicsBody::CreateNewtonMaterial()
	{
		SetNewtonMaterial(NewtonMaterialCreateGroupID(sym::Engine->Physics.GetWorld()));
		return MaterialID;
	}

	void CPhysicsBody::SetNewtonMaterial(int ID)
	{
		NewtonBodySetMaterialGroupID(RigidBody, ID);
		MaterialID = ID;
	}

	void CPhysicsBody::SetVelocity(CVector3 Vel)
	{
		if(RigidBody)
		{
			float Velocity[3];
			Velocity[0] = Vel.x;
			Velocity[1] = Vel.y;
			Velocity[2] = Vel.z;

			NewtonBodySetVelocity(RigidBody, Velocity);
		}
	}

	CVector3 CPhysicsBody::GetVelocity()
	{
		if(RigidBody)
		{
			float Vel[3];
			NewtonBodyGetVelocity(RigidBody, Vel);
			return CVector3(Vel);
		}
		else
		{
			return CVector3();
		}
	}

	CVector3 CPhysicsBody::GetOmega()
	{
		if(RigidBody)
		{
			float Omega[3];
	    NewtonBodyGetOmega(RigidBody, Omega);
	    return CVector3(Omega);
		}
		else
		{
			return CVector3();
		}   
	}

	void CPhysicsBody::Impulse(CVector3 DeltaVelocity, CVector3 Centre)
	{
		if(RigidBody)
		{
			float Delta[3];
			float Point[3];

			Delta[0] = DeltaVelocity.x;
			Delta[1] = DeltaVelocity.y;
			Delta[2] = DeltaVelocity.z;

			Point[0] = Centre.x;
			Point[1] = Centre.y;
			Point[2] = Centre.z;

			NewtonBodyAddImpulse(RigidBody, Delta, Point);
		}
	}

	void CPhysicsBody::SetForce(CVector3 F)
	{
	    Force = F;
	}

    void CPhysicsBody::SetTorque(CVector3 T)
    {
	    Torque = T;
	}

    void CPhysicsBody::AddForce(CVector3 F)
    {
	    Force += F;
	}

    void CPhysicsBody::AddTorque(CVector3 T)
    {
	    Torque += T;
	}

	void CPhysicsBody::SetAutoFreeze(bool Freeze)
	{
		if(RigidBody)
		{
			NewtonBodySetAutoSleep(RigidBody, Freeze);
			NewtonBodySetFreezeState(RigidBody, Freeze);
		}
	}

	void CPhysicsBody::OverrideCallbackForceAndTorque(NewtonApplyForceAndTorque Callback)
	{
		//Set force and torque callback
		if(RigidBody) NewtonBodySetForceAndTorqueCallback(RigidBody, Callback);
	}

	void CPhysicsBody::OverrideCallbackSetTransform(NewtonSetTransform Callback)
	{
		//Set transform callback
		if(RigidBody) NewtonBodySetTransformCallback(RigidBody, Callback);
	}

	void CPhysicsBody::SetMaterialAABBOverlapCallback(int MatID)
	{
		//Set material AABB collision callback
		NewtonMaterialSetCollisionCallback(Engine->Physics.GetWorld(), MaterialID, MatID, 0, GlobalMaterialAABBCallback, 0);
	}

	void CPhysicsBody::SetMaterialCollisionCallback(int MatID)
	{
		//Set material collision callback
		NewtonMaterialSetCollisionCallback(Engine->Physics.GetWorld(), MaterialID, MatID, 0, 0, GlobalMaterialCollisionCallback); 
	}

	void CPhysicsBody::SetForceTorqueCallback()
	{
		if(RigidBody) NewtonBodySetForceAndTorqueCallback(RigidBody, GlobalApplyForceAndTorque);
	}

	void CPhysicsBody::PhysicsApplyForceAndTorque(const NewtonBody* Body, dFloat timestep, int threadIndex)
	{
		float Mass, x, y, z;

		//Get mass
		if(Body)
		{
			NewtonBodyGetMassMatrix(Body, &Mass, &x, &y, &z);

			//Get geom
			CGeometry *Geometry = (CGeometry*) NewtonBodyGetUserData(Body);

			//Check geometry exists
			if(Geometry)
			{
				//Set force and torque
				NewtonBodySetForce(Body, &Geometry->Force.x);
				NewtonBodySetTorque(Body, &Geometry->Torque.x);
	        
				//Build argument list
				std::vector<const void*> Args;
				Args.push_back(Geometry->Name.c_str());

				//Call global script function
				Engine->ScriptManager.CallGlobalFunction("OnApplyForceAndTorque", "s", Args);

				//Set gravitational force
				Geometry->SetForce(CVector3(0.0f, -Mass * 9.8f, 0.0f));

				//Reset torque
				Geometry->SetTorque(CVector3(0.0f, 0.0f, 0.0f));
			}
			else
			{
				Engine->Logger.AddEntry("Error: PhysicsApplyForceAndTorque called with no geometry set");
			}
		}
	}

	void CPhysicsBody::RenderCollision(CColour Colour)
	{
	    if(RigidBody)
		{
			glColor3ub(Colour.r, Colour.g, Colour.b);

			glBegin(GL_LINES);

			//NewtonBodyForEachPolygonDo(RigidBody, DrawCollision);
			CMatrix4 Matrix;
			NewtonBodyGetMatrix(RigidBody, Matrix.GetAsFloatArray());
			NewtonCollisionForEachPolygonDo (NewtonBodyGetCollision(RigidBody), Matrix.GetAsFloatArray(), DrawCollision, NewtonBodyGetUserData(RigidBody));

			glEnd();
		}
	}

	void DrawCollision(void* userData, int vertexCount, const dFloat* faceArray, int faceId)
    {
        int i = vertexCount - 1;
        CVector3 p0 (faceArray[i * 3 + 0], faceArray[i * 3 + 1], faceArray[i * 3 + 2]);

        for (i = 0; i < vertexCount; i ++)
        {
            CVector3 p1 (faceArray[i * 3 + 0], faceArray[i * 3 + 1], faceArray[i * 3 + 2]);
            glVertex3f (p0.x, p0.y, p0.z);
            glVertex3f (p1.x, p1.y, p1.z);
            p0 = p1;
        }
	}

} //Namespace
