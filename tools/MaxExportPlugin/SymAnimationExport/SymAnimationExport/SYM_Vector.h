#ifndef __SYM_VECTOR
#define __SYM_VECTOR

#include <math.h>

namespace SYM
{
	class SYM_VECTOR3
	{
		public:

			float x, y, z, Weight;
			int BoneID;
			SYM_VECTOR3 *Normal;

			SYM_VECTOR3()
			{ 
				x = y = z = 0.0f;
				Normal = 0;
				Weight = 1.0f;
				BoneID = -1;
			}

			SYM_VECTOR3(float _x, float _y, float _z)
			{
				x = _x;
				y = _y;
				z = _z;
				Weight = 1.0f;
				BoneID = -1;
				Normal = 0;
			}

			SYM_VECTOR3(float _x, float _y, float _z, float _w, int _bone)
			{
				x = _x;
				y = _y;
				z = _z;
				Weight = _w;
				BoneID = _bone;
				Normal = 0;
			}

			SYM_VECTOR3(float *Floats)
			{
				x = Floats[0];
				y = Floats[1];
				z = Floats[2];
				Normal = 0;
				Weight = 1.0f;
				BoneID = -1;
			}

			SYM_VECTOR3(const SYM_VECTOR3 & Vector)
			{
				x = Vector.x;
				y = Vector.y;
				z = Vector.z;
				Normal = Vector.Normal;
				Weight = Vector.Weight;
				BoneID = Vector.BoneID;
			}

			~SYM_VECTOR3() {}

			float &operator [](int Index)
			{
				switch(Index)
				{
					case 0:
						return x;
						break;

					case 1:
						return y;
						break;

					case 2:
						return z;
						break;

					default:
						return x;
				}
			}

			SYM_VECTOR3 operator *(float Scalar)
			{
				return SYM_VECTOR3(x * Scalar, y * Scalar, z * Scalar);
			}

			float operator *(SYM_VECTOR3 Vector)
			{
				return (x * Vector.x) + (y * Vector.y) + (z * Vector.z);
			}

			SYM_VECTOR3 Multiply(SYM_VECTOR3 Vector)
			{
				return SYM_VECTOR3(x * Vector.x, y * Vector.y, z * Vector.z);
			}

			SYM_VECTOR3 operator /(float Scalar)
			{
				float inv = 1.0f/Scalar;
				return SYM_VECTOR3(x * inv, y * inv, z * inv);
			}

			SYM_VECTOR3 operator -(SYM_VECTOR3 Vector)
			{
				return SYM_VECTOR3(x - Vector.x, y - Vector.y, z - Vector.z);
			}

			SYM_VECTOR3 operator +(SYM_VECTOR3 Vector)
			{
				return SYM_VECTOR3(Vector.x + x, Vector.y + y, Vector.z + z);
			}

			void operator+=(SYM_VECTOR3 Vector)
			{
				*this = *this + Vector;
			}

			void operator*=(SYM_VECTOR3 Vector)
			{
				x = x * Vector.x;
				y = y * Vector.y;
				z = z * Vector.z;
			}

			void operator *=(float Scalar)
			{
				*this = *this * Scalar;
			}

			bool operator==(SYM_VECTOR3 Vector)
			{
				return	x == Vector.x && y == Vector.y && z == Vector.z;
			}

			SYM_VECTOR3 Cross(SYM_VECTOR3& b)
			{
				return SYM_VECTOR3((y * b.z - b.y * z), (z * b.x - b.z * x), (x * b.y - b.x * y));
			}

			float Dot(SYM_VECTOR3 Vector)
			{
				return (x * Vector.x + y * Vector.y + z * Vector.z);
			}

			void Normalise()
			{
				float Length = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));

				x /= Length;
				y /= Length;
				z /= Length;
			}
	};

}

#endif