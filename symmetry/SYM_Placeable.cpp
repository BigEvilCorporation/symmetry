#include <math.h>

#include "SYM_Placeable.h"

namespace sym
{
    CPlaceable::CPlaceable()
    {

    }

    void CPlaceable::SetPosition(CVector3 Pos)
    {
        Position = Pos;
        Matrix.SetTranslation(Position);
        HasMoved();
    }

    void CPlaceable::SetRotation(CQuaternion Rot)
    {
        Quat = Rot;
        Matrix = Quat.ToMatrix();
        Matrix.SetTranslation(Position);
        HasMoved();
    }

    void CPlaceable::SetMatrix(CMatrix4 Mat)
    {
        Position = Mat.TransformVector(CVector3(0.0f, 0.0f, 0.0f));
        Quat.FromMatrix(Mat);
        Matrix = Mat;
        HasMoved();
    }

    void CPlaceable::Move(float Distance)
    {
        Position += GetForward() * Distance;
        Matrix.SetTranslation(Position);
        HasMoved();
    }

    void CPlaceable::Strafe(float Distance)
    {
        Position += GetRight() * Distance;
        Matrix.SetTranslation(Position);
        HasMoved();
    }

    void CPlaceable::Fly(float Distance)
    {
        Position += GetUp() * Distance;
        Matrix.SetTranslation(Position);
        HasMoved();
    }

    void CPlaceable::Pitch(float Degrees)
    {
		CMatrix4 PitchMatrix;
		PitchMatrix.SetRotation(Degrees, GetRight());
		Matrix = Matrix * PitchMatrix;
		Matrix.SetTranslation(Position);

		Quat.FromMatrix(Matrix);
        HasMoved();
    }

    void CPlaceable::Yaw(float Degrees)
    {
		CMatrix4 PitchMatrix;
		PitchMatrix.SetRotation(Degrees, GetUp());
		Matrix = Matrix * PitchMatrix;
		Matrix.SetTranslation(Position);

		Quat.FromMatrix(Matrix);
        HasMoved();
    }

    void CPlaceable::Roll(float Degrees)
    {
		CMatrix4 PitchMatrix;
		PitchMatrix.SetRotation(Degrees, GetForward());
		Matrix = Matrix * PitchMatrix;
		Matrix.SetTranslation(Position);

		Quat.FromMatrix(Matrix);
        HasMoved();
    }

	void CPlaceable::RotateAxis(float Degrees, CVector3 Axis)
	{
		CMatrix4 RotMatrix;
		RotMatrix.SetRotation(Degrees, Axis);
		Matrix = Matrix * RotMatrix;

        Quat.FromMatrix(Matrix);
        HasMoved();
	}

	void CPlaceable::LookAt(CVector3 Forward, CVector3 Target, float Factor)
	{
		/*
		CVector3 Direction = Target - Position;
		Direction.Normalise();

		CVector3 Right = Forward.Cross(Direction);
		Right.Normalise();

		CVector3 Up = Direction.Cross(Right);
		Up.Normalise();

		CMatrix4 LookAtMatrix;

		LookAtMatrix.set(0, 0, Right.x);
		LookAtMatrix.set(1, 0, Right.y);
		LookAtMatrix.set(2, 0, Right.z);

		LookAtMatrix.set(0, 1, Up.x);
		LookAtMatrix.set(1, 1, Up.y);
		LookAtMatrix.set(2, 1, Up.z);

		LookAtMatrix.set(0, 2, Direction.x);
		LookAtMatrix.set(1, 2, Direction.y);
		LookAtMatrix.set(2, 2, Direction.z);

		CQuaternion TargetQuat;
		TargetQuat.FromMatrix(LookAtMatrix);
		*/
		CVector3 ForwardVec = Target - Position;
		ForwardVec.Normalise();

		float Theta = (float)acos(ForwardVec.Dot(Forward));

		CVector3 UpVec = Forward.Cross(ForwardVec);
		UpVec.Normalise();

		CQuaternion TargetQuat;
		TargetQuat.FromAxis(Theta, UpVec);

		//Slerp current quaternion using new quaternion, by factor
		Quat.Slerp(Quat, TargetQuat, Factor);

		Matrix = Quat.ToMatrix();
		Matrix.SetTranslation(Position);
		HasMoved();
	}

    CVector3 CPlaceable::GetForward()
    {
		CVector3 Forward( -Matrix.get(2, 0),
                            -Matrix.get(2, 1),
                            -Matrix.get(2, 2));
		
        return Forward.GetNormalised();
    }

    CVector3 CPlaceable::GetRight()
    {
		CVector3 Right( Matrix.get(0, 0),
                            Matrix.get(0, 1),
                            Matrix.get(0, 2));

        return Right.GetNormalised();
    }

    CVector3 CPlaceable::GetUp()
    {
		CVector3 Up( Matrix.get(1, 0),
                            Matrix.get(1, 1),
                            Matrix.get(1, 2));

        return Up.GetNormalised();
    }

} //Namespace
