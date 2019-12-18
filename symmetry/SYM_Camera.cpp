#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <math.h>

#include "SYM_Camera.h"
#include "SYM_Engine.h"

namespace sym
{
	void CCamera::Init()
	{
		CreateSphere(CVector3(2.0f, 2.0f, 2.0f));
		SetGeometry(dynamic_cast<CGeometry*>(this));
	}

    void CCamera::ApplyMatrix()
    {
        glMultMatrixf(Matrix.GetInverse().GetAsFloatArray());
    }

	bool CCamera::VectorInFront(CVector3 Vector)
	{
		/*
		Function WorldToObjectTransform(Vec As Vector3D) As Vector3D
		  //me is the object (or camera) that we're transforming _to_

		   dim temp as vector3D

		   temp = me.position.minus(vec)
		   me.orientation.transform(temp)

		   return temp
		End Function
		*/

		//Translate vector to camera's object space
		Vector = GetPosition() - Vector;
		Vector = GetMatrix().RotateVector(Vector);

		//If Z is positive, vector is in front of camera
		if(Vector.z > 0.0f) return true;
		else return false;
	}

	void CCamera::Update()
	{
		if(IsShaking)
		{
			SetPosition(GetPosition() + (ShakeDirection * (sin(ShakeSin) * (ShakeTimer.GetTime() - ShakeTimer.GetElapsed()) * ShakeStrength)));

			ShakeSin += Engine->GetDeltaTime();

			if(ShakeTimer.HasTriggered()) IsShaking = false;
		}
	}

	void CCamera::Shake(CVector3 Direction, float Strength, float CalmTime)
	{
		ShakeDirection = Direction.GetNormalised();
		ShakeStrength = Strength;
		ShakeSin = 0.0f;
		ShakeTimer.SetTime(CalmTime * 1000.0f);
		ShakeTimer.Reset();
		ShakeTimer.Start();
		IsShaking = true;
	}

} //Namespace
