#include "SYM_Quaternion.h"

namespace sym
{
	CQuaternion::CQuaternion()
	{
        W = 1.0f;
        X = 0.0f;
        Y = 0.0f;
        Z = 0.0f;
	}

	CQuaternion::CQuaternion(const float w, const float x, const float y, const float z)
	{
        W = w;
        X = x;
        Y = y;
        Z = z;
	}

	CQuaternion::CQuaternion(float Degrees, CVector3 Axis)
	{
		FromAxis(Degrees * Maths::DEGREES_TO_RADIANS, Axis);
	}

	CQuaternion operator * (const CQuaternion &a, const CQuaternion &b)
	{
        float w,x,y,z;

        w = a.W*b.W - (a.X*b.X + a.Y*b.Y + a.Z*b.Z);

        x = a.W*b.X + b.W*a.X + a.Y*b.Z - a.Z*b.Y;
        y = a.W*b.Y + b.W*a.Y + a.Z*b.X - a.X*b.Z;
        z = a.W*b.Z + b.W*a.Z + a.X*b.Y - a.Y*b.X;

        return CQuaternion(w,x,y,z);
	}

	const CQuaternion& CQuaternion::operator *= (const CQuaternion &q)
	{
        float w = W*q.W - (X*q.X + Y*q.Y + Z*q.Z);

        float x = W*q.X + q.W*X + Y*q.Z - Z*q.Y;
        float y = W*q.Y + q.W*Y + Z*q.X - X*q.Z;
        float z = W*q.Z + q.W*Z + X*q.Y - Y*q.X;

        W = w;
        X = x;
        Y = y;
        Z = z;

        return *this;
	}

	const CQuaternion& CQuaternion::operator ~ ()
	{
        X = -X;
        Y = -Y;
        Z = -Z;
        return *this;
	}

	const CQuaternion& CQuaternion::operator - ()
	{
        float norme = sqrt(W*W + X*X + Y*Y + Z*Z);
        if (norme == 0.0f)
            norme = 1.0f;

        float recip = 1.0f / norme;

        W =  W * recip;
        X = -X * recip;
        Y = -Y * recip;
        Z = -Z * recip;

        return *this;
	}

	const CQuaternion& CQuaternion::Normalise()
	{
		float norme = sqrt(W*W + X*X + Y*Y + Z*Z);
		if (norme == 0.0f)
		{
			W = 1.0f;
			X = Y = Z = 0.0f;
		}
		else
		{
			float recip = 1.0f/norme;

			W *= recip;
			X *= recip;
			Y *= recip;
			Z *= recip;
		}

		return *this;
	}

	float CQuaternion::GetDotProduct(CQuaternion& q)
	{
		return (X * q.X) + (Y * q.Y) + (Z * q.Z) + (W * q.W);
	}

	void CQuaternion::FromAxis(float Angle, CVector3 axis)
	{
		W = 0.0f;
		X = axis.x;
		Y = axis.y;
		Z = axis.z;
		if (X == 0.0f && Y == 0.0f && Z == 0.0f)
			X = 1.0f;
		Normalise();

		Angle = Angle / 2;
		W = (float)cos(Angle);

		float scale = (float)sin(Angle);
		X *= scale;
		Y *= scale;
		Z *= scale;
	}

	float CQuaternion::ToAxis(CVector3& Axis)
	{
		CQuaternion Quat = (*this);
		if (Quat.W > 1.0f) Quat.Normalise();

		float Angle = 2 * acos(Quat.W);

		float s = sqrt(1.0f - Quat.W * Quat.W);

		if (s < 0.001f)
		{
			Axis.x = Quat.X;
			Axis.y = Quat.Y;
			Axis.z = Quat.Z;
		}
		else
		{
			Axis.x = Quat.X / s;
			Axis.y = Quat.Y / s;
			Axis.z = Quat.Z / s;
		}

		return Angle * Maths::RADIANS_TO_DEGREES;
	}

	void CQuaternion::ConstrainYAxis(float factor)
	{
		CQuaternion Q(W, 0, Y, 0);
		Slerp(*this, Q, factor);
	}

	void CQuaternion::FromLookAt(CVector3 Forward, CVector3 Position, CVector3 Target, float Factor)
	{
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

		//Slerp current quaternion using new quaternion, by factor
		//Slerp((*this), TargetQuat, Factor);

		*this = TargetQuat;
	}

	void CQuaternion::FromEuler(CVector3 Euler)
	{
        float cosYaw = cos(Euler.y / 2);
        float sinYaw = sin(Euler.y / 2);
        float cosPitch = cos(Euler.x / 2);
        float sinPitch = sin(Euler.x / 2);
        float cosRoll = cos(Euler.z / 2);
        float sinRoll = sin(Euler.z / 2);
        X = cosRoll * sinPitch * cosYaw + sinRoll * cosPitch * sinYaw;
        Y = cosRoll * cosPitch * sinYaw - sinRoll * sinPitch * cosYaw;
        Z = sinRoll * cosPitch * cosYaw - cosRoll * sinPitch * sinYaw;
        W = cosRoll * cosPitch * cosYaw + sinRoll * sinPitch * sinYaw;
	}

	CVector3 CQuaternion::ToEuler()
	{
	    CVector3 Euler;
        float testValue = X * Y + Z * W;
        if(testValue > 0.499f) // north pole singularity
        {
            Euler.y = 2 * atan2(X,W);
            Euler.z = Maths::PI / 2;
            Euler.x = 0.0f;
            return Euler;
        }
        if(testValue < -0.499f) // south pole singularity
        {
            Euler.y = -2 * atan2(X,W);
            Euler.z = -Maths::PI / 2;
            Euler.x = 0.0f;
            return Euler;
        }

        float sqx = X*X;
        float sqy = Y*Y;
        float sqz = Z*Z;
        float sqw = W*W;
        float unit = sqx + sqy + sqz + sqw;

        Euler.y = atan2( (float)2.0f*Y*W - 2.0f*X*Z, (float)sqx - sqy - sqz + sqw );
        Euler.z = asin ( (float)2.0f * testValue / unit );
        Euler.x = atan2( (float)2.0f*X*W - 2.0f*Y*Z, (float)-sqx + sqy - sqz + sqw );

        return Euler;
	}

	void CQuaternion::FromMatrix(CMatrix4& Matrix)
    {
		float tr = Matrix.get(0, 0) + Matrix.get(1, 1) + Matrix.get(2, 2);
		if ( tr > 0 )
		{
			float s = (float)sqrt(tr + 1);
			W = s * 0.5f;
			s = 0.5f / s;
			X = (Matrix.get(1, 2) - Matrix.get(2, 1)) * s;
			Y = (Matrix.get(2, 0) - Matrix.get(0, 2)) * s;
			Z = (Matrix.get(0, 1) - Matrix.get(1, 0)) * s;
		}
		else
		{
			int i = 0;
			if ( Matrix.get(1, 1) > Matrix.get(0, 0) ) i = 1;
			if ( Matrix.get(2, 2) > Matrix.get(i, i) ) i = 2;

			static int nxt[] = {1,2,0};
			int j = nxt[i];
			int k = nxt[j];

			float s = (float)sqrt(Matrix.get(i, i) - Matrix.get(j, j) - Matrix.get(k, k) + 1);
			if ( s < MATHS_FLT_EPSILON )
			{
				CQuaternion();
			}
			else
			{
				float pq[4];

				pq[i] = s * 0.5f;
				s = 0.5f / s;
				pq[3] = (Matrix.get(j, k) - Matrix.get(k, j)) * s;
				pq[j] = (Matrix.get(i, j) + Matrix.get(j, i)) * s;
				pq[k] = (Matrix.get(i, k) + Matrix.get(k, i)) * s;

				W = pq[3];
				X = pq[0];
				Y = pq[1];
				Z = pq[2];
			}
		}
    }

	CMatrix4 CQuaternion::ToMatrix()
	{
		CMatrix4 Matrix;

		// First row
		Matrix._Matrix[ 0] = 1.0f - 2.0f * ( Y * Y + Z * Z );
		Matrix._Matrix[ 1] = 2.0f * (X * Y + Z * W);
		Matrix._Matrix[ 2] = 2.0f * (X * Z - Y * W);
		Matrix._Matrix[ 3] = 0.0f;
		// Second row
		Matrix._Matrix[ 4] = 2.0f * ( X * Y - Z * W );
		Matrix._Matrix[ 5] = 1.0f - 2.0f * ( X * X + Z * Z );
		Matrix._Matrix[ 6] = 2.0f * (Z * Y + X * W );
		Matrix._Matrix[ 7] = 0.0f;
		// Third row
		Matrix._Matrix[ 8] = 2.0f * ( X * Z + Y * W );
		Matrix._Matrix[ 9] = 2.0f * ( Y * Z - X * W );
		Matrix._Matrix[10] = 1.0f - 2.0f * ( X * X + Y * Y );
		Matrix._Matrix[11] = 0.0f;
		// Fourth row
		Matrix._Matrix[12] = 0;
		Matrix._Matrix[13] = 0;
		Matrix._Matrix[14] = 0;
		Matrix._Matrix[15] = 1.0f;
		
		return Matrix;
	}

	void CQuaternion::Slerp(CQuaternion &a, CQuaternion &b, float t)
	{
        float omega, cosom, sinom, sclp, sclq;

        cosom = a.X*b.X + a.Y*b.Y + a.Z*b.Z + a.W*b.W;


        if ((1.0f+cosom) > MATHS_FLT_EPSILON)
        {
            if ((1.0f-cosom) > MATHS_FLT_EPSILON)
            {
                omega = acos(cosom);
                sinom = sin(omega);
                sclp = sin((1.0f-t)*omega) / sinom;
                sclq = sin(t*omega) / sinom;
            }
            else
            {
                sclp = 1.0f - t;
                sclq = t;
            }

            X = sclp*a.X + sclq*b.X;
            Y = sclp*a.Y + sclq*b.Y;
            Z = sclp*a.Z + sclq*b.Z;
            W = sclp*a.W + sclq*b.W;

        }
        else
        {
            X =-a.Y;
            Y = a.X;
            Z =-a.W;
            W = a.Z;

            sclp = (float)sin((1.0f - t) * Maths::PI * 0.5f);
            sclq = (float)sin(t * Maths::PI * 0.5f);

            X = sclp*a.X + sclq*b.X;
            Y = sclp*a.Y + sclq*b.Y;
            Z = sclp*a.Z + sclq*b.Z;
        }
    }

	/*
	void CQuaternion::Slerp(CQuaternion &a, CQuaternion &b, float t)
	{
		//slerp(q1,q2,lambda) = q1*(q1^-1*q2)^lambda
	    
		if (t <= 0.0f) *this = a;
		else if (t >= 1.0f) *this = b;
		else
		{
			*this = a * (a.GetInverse() * b).GetToPowerOf(t);
		}
	}
	*/

	CQuaternion CQuaternion::GetToPowerOf(float n)
	{
		// q = w + (x,y,z)
		// q = cos(theta) + u*sin(theta)
		// q^n = cos(n*theta) + u*sin(n*theta)

		float ntheta = n * Z;
		float costheta = cos(ntheta);
		float sintheta = sin(ntheta);
		return CQuaternion(costheta, X * sintheta, Y * sintheta, Z * sintheta);
	}

	CQuaternion CQuaternion::GetInverse()
	{
		return CQuaternion(W, -X, -Y, -Z);
	}

	const CQuaternion& CQuaternion::exp()
	{
        float Mul;
        float Length = sqrt(X*X + Y*Y + Z*Z);

        if (Length > 1.0e-4f)
            Mul = sin(Length)/Length;
        else
            Mul = 1.0f;

        W = cos(Length);

        X *= Mul;
        Y *= Mul;
        Z *= Mul;

        return *this;
	}

	const CQuaternion& CQuaternion::log()
	{
        float Length;

        Length = sqrt(X*X + Y*Y + Z*Z);
        Length = atan(Length/W);

        W = 0.0f;

        X *= Length;
        Y *= Length;
        Z *= Length;

        return *this;
	}
}