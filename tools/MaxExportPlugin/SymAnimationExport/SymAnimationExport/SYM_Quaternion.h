#ifndef __SYM_QUATERNION
#define __SYM_QUATERNION

#ifdef SYM_DLL
#   define __declspec(dllexport)
#else
//#   define __declspec(dllimport)
#endif

#include <math.h>

//#include "SYM_Maths.h"
#include "SYM_Vector.h"
#include "SYM_Matrix.h"

namespace SYM
{
	class SYM_QUATERNION
	{
		public:
			float W, X, Y, Z;      // components of a quaternion
		
			// default constructor
			SYM_QUATERNION();
			SYM_QUATERNION(const float, const float = 0.0, const float = 0.0, const float = 0.0);
			SYM_QUATERNION(float Degrees, SYM_VECTOR3 Axis);

			// quaternion multiplication
			friend SYM_QUATERNION operator * (const SYM_QUATERNION&, const SYM_QUATERNION&);
			const SYM_QUATERNION& operator *= (const SYM_QUATERNION&);

			// konjugiertes SYM_QUATERNION
			const SYM_QUATERNION& operator ~ ();

			// this function inverts the quaternion
			const SYM_QUATERNION& operator - ();

			// this normalizes a quaternion
			const SYM_QUATERNION& Normalize();
			void FromAxis(float Angle, SYM_VECTOR3 axis);
			SYM_MATRIX4 ToMatrix();
			void FromMatrix(const SYM_MATRIX4& Matrix);

			SYM_MATRIX4 GetMatrix();
			void FromEuler(SYM_VECTOR3 Euler);
			SYM_VECTOR3 ToEuler();

			void FromLookAt(SYM_VECTOR3 Forward, SYM_VECTOR3 Position, SYM_VECTOR3 Target, float Factor);

			void Slerp(const SYM_QUATERNION&, const SYM_QUATERNION&, const float);

			// some additional SYM_QUATERNION functions
			// getting the exponent of a quaternion
			const SYM_QUATERNION& exp();
			const SYM_QUATERNION& log();

			SYM_QUATERNION getConjugate() const
			{
				return SYM_QUATERNION(-X, -Y, -Z, W);
			}

			SYM_QUATERNION operator +(const SYM_QUATERNION& q) const
			{
				return SYM_QUATERNION(W + q.W, X + q.X, Y + q.Y, Z + q.Z);
			}

			SYM_VECTOR3 operator* (const SYM_VECTOR3 &vec) const
			{
				SYM_VECTOR3 vn = vec;
				vn.Normalise();
			 
				SYM_QUATERNION vecQuat, resQuat;
				vecQuat.X = vn.x;
				vecQuat.Y = vn.y;
				vecQuat.Z = vn.z;
				vecQuat.W = 0.0f;
			 
				resQuat = vecQuat * getConjugate();
				resQuat = *this * resQuat;
			 
				return (SYM_VECTOR3(resQuat.X, resQuat.Y, resQuat.Z));
			}
	};
}

#endif