#ifndef __SYM_QUATERNION
#define __SYM_QUATERNION

#ifdef SYM_DLL
#   define __declspec(dllexport)
#else
//#   define __declspec(dllimport)
#endif

#include <math.h>

#include "SYM_Maths.h"
#include "SYM_Vector.h"
#include "SYM_Matrix.h"

namespace sym
{
	const float MATHS_FLT_EPSILON = 1.19209290e-7f;

	class CQuaternion
	{
		public:
			float W, X, Y, Z;      // components of a quaternion
		
			// default constructor
			CQuaternion();
			CQuaternion(const float, const float = 0.0, const float = 0.0, const float = 0.0);
			CQuaternion(float Degrees, CVector3 Axis);

			// quaternion multiplication
			friend CQuaternion operator * (const CQuaternion&, const CQuaternion&);
			const CQuaternion& operator *= (const CQuaternion&);

			CQuaternion GetInverse();
			CQuaternion GetToPowerOf(float n);
			float GetDotProduct(CQuaternion& q);

			// konjugiertes CQuaternion
			const CQuaternion& operator ~ ();

			// this function inverts the quaternion
			const CQuaternion& operator - ();

			void ConstrainYAxis(float factor);

			// this normalizes a quaternion
			const CQuaternion& Normalise();
			void FromAxis(float Angle, CVector3 axis);
			CMatrix4 ToMatrix();
			void FromMatrix(CMatrix4& Matrix);
			float ToAxis(CVector3& Axis);

			void FromEuler(CVector3 Euler);
			CVector3 ToEuler();

			void FromLookAt(CVector3 Forward, CVector3 Position, CVector3 Target, float Factor);

			void Slerp(CQuaternion&, CQuaternion&, float);

			// some additional CQuaternion functions
			// getting the exponent of a quaternion
			const CQuaternion& exp();
			const CQuaternion& log();

			CQuaternion getConjugate() const
			{
				return CQuaternion(-X, -Y, -Z, W);
			}

			CQuaternion operator +(const CQuaternion& q) const
			{
				return CQuaternion(W + q.W, X + q.X, Y + q.Y, Z + q.Z);
			}

			CVector3 operator* (const CVector3 &vec) const
			{
				CVector3 vn = vec;
				vn.Normalise();
			 
				CQuaternion vecQuat, resQuat;
				vecQuat.X = vn.x;
				vecQuat.Y = vn.y;
				vecQuat.Z = vn.z;
				vecQuat.W = 0.0f;
			 
				resQuat = vecQuat * getConjugate();
				resQuat = *this * resQuat;
			 
				return (CVector3(resQuat.X, resQuat.Y, resQuat.Z));
			}
	};
}

#endif