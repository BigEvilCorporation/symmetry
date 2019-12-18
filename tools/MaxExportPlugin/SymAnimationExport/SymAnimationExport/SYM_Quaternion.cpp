#include "SYM_Quaternion.h"

namespace SYM
{
	const float PI = 3.14159264f;
	const float PI_DIV_180 = 0.0174532925f;
	const float MATHS_FLT_EPSILON = 1.19209290e-7f;

	SYM_QUATERNION::SYM_QUATERNION()
	{
        W = 1.0;
        X = 0.0;
        Y = 0.0;
        Z = 0.0;
	}

	SYM_QUATERNION::SYM_QUATERNION(const float w, const float x, const float y, const float z)
	{
        W = w;
        X = x;
        Y = y;
        Z = z;
	}

	SYM_QUATERNION::SYM_QUATERNION(float Degrees, SYM_VECTOR3 Axis)
	{
	    W = Degrees;
        X = Axis.x;
        Y = Axis.y;
        Z = Axis.z;
	}

	SYM_QUATERNION operator * (const SYM_QUATERNION &a, const SYM_QUATERNION &b)
	{
        float w,x,y,z;

        w = a.W*b.W - (a.X*b.X + a.Y*b.Y + a.Z*b.Z);

        x = a.W*b.X + b.W*a.X + a.Y*b.Z - a.Z*b.Y;
        y = a.W*b.Y + b.W*a.Y + a.Z*b.X - a.X*b.Z;
        z = a.W*b.Z + b.W*a.Z + a.X*b.Y - a.Y*b.X;

        return SYM_QUATERNION(w,x,y,z);
	}

	const SYM_QUATERNION& SYM_QUATERNION::operator *= (const SYM_QUATERNION &q)
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

	const SYM_QUATERNION& SYM_QUATERNION::operator ~ ()
	{
        X = -X;
        Y = -Y;
        Z = -Z;
        return *this;
	}

	const SYM_QUATERNION& SYM_QUATERNION::operator - ()
	{
        float norme = sqrt(W*W + X*X + Y*Y + Z*Z);
        if (norme == 0.0)
            norme = 1.0;

        float recip = 1.0f / norme;

        W =  W * recip;
        X = -X * recip;
        Y = -Y * recip;
        Z = -Z * recip;

        return *this;
	}

	const SYM_QUATERNION& SYM_QUATERNION::Normalize()
	{
		float norme = sqrt(W*W + X*X + Y*Y + Z*Z);
		if (norme == 0.0)
		{
			W = 1.0;
			X = Y = Z = 0.0;
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

	void SYM_QUATERNION::FromAxis(float Angle, SYM_VECTOR3 axis)
	{

			W = 0;
			X = axis.x;
			Y = axis.y;
			Z = axis.z;
			if (X == 0.0f && Y == 0.0f && Z == 0.0f)
				X = 1.0f;
			Normalize();

			Angle = Angle / 2;
			W = (float)cos(Angle);

			float scale = (float)sin(Angle);
			X *= scale;
			Y *= scale;
			Z *= scale;
	}

	void SYM_QUATERNION::FromLookAt(SYM_VECTOR3 Forward, SYM_VECTOR3 Position, SYM_VECTOR3 Target, float Factor)
	{
		SYM_VECTOR3 TMinusP = Target - Position;
		
		TMinusP.Normalise();

		float Theta = (float)acos(TMinusP.Dot(Forward));
		SYM_VECTOR3 Cross = Forward.Cross(TMinusP);

		SYM_QUATERNION TargetQuat;
		TargetQuat.FromAxis(Theta, Cross);

		//Slerp current quaternion using new quaternion, by factor
		Slerp((*this), TargetQuat, Factor);
	}

	SYM_MATRIX4 SYM_QUATERNION::GetMatrix()
	{
	    float Matrix[16];

	    Matrix[ 0] = 1.0f - 2.0f * ( Y * Y + Z * Z );
		Matrix[ 1] = 2.0f * (X * Y + Z * W);
		Matrix[ 2] = 2.0f * (X * Z - Y * W);
		Matrix[ 3] = 0.0f;
		// Second row
		Matrix[ 4] = 2.0f * ( X * Y - Z * W );
		Matrix[ 5] = 1.0f - 2.0f * ( X * X + Z * Z );
		Matrix[ 6] = 2.0f * (Z * Y + X * W );
		Matrix[ 7] = 0.0f;
		// Third row
		Matrix[ 8] = 2.0f * ( X * Z + Y * W );
		Matrix[ 9] = 2.0f * ( Y * Z - X * W );
		Matrix[10] = 1.0f - 2.0f * ( X * X + Y * Y );
		Matrix[11] = 0.0f;
		// Fourth row
		Matrix[12] = 0;
		Matrix[13] = 0;
		Matrix[14] = 0;
		Matrix[15] = 1.0f;

		SYM_MATRIX4 SMatrix(Matrix);

	    return SMatrix;
	}

	void SYM_QUATERNION::FromEuler(SYM_VECTOR3 Euler)
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

	SYM_VECTOR3 SYM_QUATERNION::ToEuler()
	{
	    SYM_VECTOR3 Euler;
        float testValue = X * Y + Z * W;
        if(testValue > 0.499f) // north pole singularity
        {
            Euler.y = 2 * atan2(X,W);
            Euler.z = PI / 2;
            Euler.x = 0.0f;
            return Euler;
        }
        if(testValue < -0.499f) // south pole singularity
        {
            Euler.y = -2 * atan2(X,W);
            Euler.z = -PI / 2;
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

	void SYM_QUATERNION::FromMatrix(const SYM_MATRIX4& Matrix)
    {
        float trace = Matrix.get(0, 0) + Matrix.get(1, 1) + Matrix.get(2, 2);
        if (trace > 0)
        {
            float root = (float)sqrt(trace + 1);
            W = root * 0.5f;

            root = 0.5f / root;
            X = (Matrix.get(2, 1) - Matrix.get(1, 2)) * root;
            Y = (Matrix.get(0, 2) - Matrix.get(2, 0)) * root;
            Z = (Matrix.get(1, 0) - Matrix.get(0, 1)) * root;
        }
        else
        {
            int next[3] = { 1, 2, 0 };
            SYM_VECTOR3 vec;

            int i = 0;
            if (Matrix.get(1, 1) > Matrix.get(0, 0)) i = 1;
            if (Matrix.get(2, 2) > Matrix.get(i, i)) i = 2;

            int j = next[i];
            int k = next[j];

            float root = sqrt(Matrix.get(i, i) - Matrix.get(j, j) - Matrix.get(k, k) + 1);

            vec.x = 0.5f * root;
            root = 0.5f / root;
            vec.y = (Matrix.get(j, i) + Matrix.get(i, j)) * root;
            vec.z = (Matrix.get(k, i) + Matrix.get(i, k)) * root;
            W      = (Matrix.get(k, j) - Matrix.get(j, k)) * root;

            X = vec.x;
            Y = vec.y;
            Z = vec.z;
        }
    }

	SYM_MATRIX4 SYM_QUATERNION::ToMatrix()
	{
		SYM_MATRIX4 Matrix;

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

	void SYM_QUATERNION::Slerp(const SYM_QUATERNION &a,const SYM_QUATERNION &b, const float t)
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

            sclp = (float)sin((1.0f - t) * PI * 0.5f);
            sclq = (float)sin(t * PI * 0.5f);

            X = sclp*a.X + sclq*b.X;
            Y = sclp*a.Y + sclq*b.Y;
            Z = sclp*a.Z + sclq*b.Z;
        }
    }

	const SYM_QUATERNION& SYM_QUATERNION::exp()
	{
        float Mul;
        float Length = sqrt(X*X + Y*Y + Z*Z);

        if (Length > 1.0e-4)
            Mul = sin(Length)/Length;
        else
            Mul = 1.0;

        W = cos(Length);

        X *= Mul;
        Y *= Mul;
        Z *= Mul;

        return *this;
	}

	const SYM_QUATERNION& SYM_QUATERNION::log()
	{
        float Length;

        Length = sqrt(X*X + Y*Y + Z*Z);
        Length = atan(Length/W);

        W = 0.0;

        X *= Length;
        Y *= Length;
        Z *= Length;

        return *this;
	}
}