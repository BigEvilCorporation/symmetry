#include "SYM_PhysicsObject.h"

namespace sym
{
    CPhysicsObject::CPhysicsObject()
    {
		PhysicsType = PHYSICS_OBJ_BOX;
    }

    CPhysicsObject::~CPhysicsObject()
    {

    }

    void CPhysicsObject::SetPosition(CVector3 Pos)
	{
		CPlaceable::SetPosition(Pos);
		SetNewtonMatrix(Matrix);
	}

	void CPhysicsObject::SetRotation(CQuaternion Rot)
	{
		CPlaceable::SetRotation(Rot);
		SetNewtonMatrix(Matrix);
	}

	void CPhysicsObject::LookAt(CVector3 Forward, CVector3 Target, float Factor)
	{
		CPlaceable::LookAt(Forward, Target, Factor);
		SetNewtonMatrix(Matrix);
	}

    void CPhysicsObject::Move(float Distance)
    {
        CPlaceable::Move(Distance);
		SetNewtonMatrix(Matrix);
    }

    void CPhysicsObject::Strafe(float Distance)
    {
        CPlaceable::Strafe(Distance);
		SetNewtonMatrix(Matrix);
    }

    void CPhysicsObject::Fly(float Distance)
    {
        CPlaceable::Fly(Distance);
		SetNewtonMatrix(Matrix);
    }

    void CPhysicsObject::Pitch(float Degrees)
    {
        CPlaceable::Pitch(Degrees);
        SetNewtonMatrix(Matrix);
    }

    void CPhysicsObject::Yaw(float Degrees)
    {
        CPlaceable::Yaw(Degrees);
        SetNewtonMatrix(Matrix);
    }

    void CPhysicsObject::Roll(float Degrees)
    {
        CPlaceable::Roll(Degrees);
        SetNewtonMatrix(Matrix);
    }

	void CPhysicsObject::RotateAxis(float Degrees, CVector3 Axis)
	{
		CPlaceable::RotateAxis(Degrees, Axis);
        SetNewtonMatrix(Matrix);
	}

} //Namespace
