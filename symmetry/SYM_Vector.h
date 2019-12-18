#ifndef __SYM_VECTOR
#define __SYM_VECTOR

#ifdef SYM_DLL
#   define __declspec(dllexport)
#else
//#   define __declspec(dllimport)
#endif

#include <math.h>

namespace sym
{
	class CVector3
	{
		public:

			float x, y, z;
			CVector3 *Normal;

			int MaxBoneIDs[4];
			int BoneIDs[4];
			float Weights[4];
			
			//CVector3 *Tangent;

			CVector3()
			{ 
				x = y = z = 0.0f;
				Normal = 0;
				//Tangent = 0;

				for(int i = 0; i < 4; i++)
				{
					MaxBoneIDs[i] = -1;
					BoneIDs[i] = -1;
					Weights[i] = 1.0f;
				}
			}

			CVector3(float _x, float _y, float _z)
			{
				x = _x;
				y = _y;
				z = _z;
				Normal = 0;
				//Tangent = 0;

				for(int i = 0; i < 4; i++)
				{
					MaxBoneIDs[i] = -1;
					BoneIDs[i] = -1;
					Weights[i] = 1.0f;
				}
			}

			CVector3(float *Floats)
			{
				x = Floats[0];
				y = Floats[1];
				z = Floats[2];
				Normal = 0;
				//Tangent = 0;

				for(int i = 0; i < 4; i++)
				{
					MaxBoneIDs[i] = -1;
					BoneIDs[i] = -1;
					Weights[i] = 1.0f;
				}
			}

			/*
			CVector3(const CVector3 & Vector)
			{
				x = Vector.x;
				y = Vector.y;
				z = Vector.z;
				Normal = Vector.Normal;
				//Tangent = Vector.Tangent;
			}
			*/

			~CVector3() {}

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

			CVector3 operator *(float Scalar)
			{
				return CVector3(x * Scalar, y * Scalar, z * Scalar);
			}

			float operator *(CVector3 Vector)
			{
				return (x * Vector.x) + (y * Vector.y) + (z * Vector.z);
			}

			CVector3 Multiply(CVector3 Vector)
			{
				return CVector3(x * Vector.x, y * Vector.y, z * Vector.z);
			}

			CVector3 operator /(float Scalar)
			{
				float inv = 1.0f/Scalar;
				return CVector3(x * inv, y * inv, z * inv);
			}

			CVector3 operator -(CVector3 Vector)
			{
				return CVector3(x - Vector.x, y - Vector.y, z - Vector.z);
			}

			CVector3 operator +(CVector3 Vector)
			{
				return CVector3(Vector.x + x, Vector.y + y, Vector.z + z);
			}

			void operator+=(CVector3 Vector)
			{
				*this = *this + Vector;
			}

			void operator*=(CVector3 Vector)
			{
				x = x * Vector.x;
				y = y * Vector.y;
				z = z * Vector.z;
			}

			void operator *=(float Scalar)
			{
				*this = *this * Scalar;
			}

			bool operator==(CVector3 Vector)
			{
				return	x == Vector.x && y == Vector.y && z == Vector.z;
			}

			CVector3 Cross(CVector3& b)
			{
				return CVector3((y * b.z - b.y * z), (z * b.x - b.z * x), (x * b.y - b.x * y));
			}

			float Dot(CVector3 Vector)
			{
				return (x * Vector.x + y * Vector.y + z * Vector.z);
			}

			void Normalise()
			{
				float Length = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));

				if(Length != 0)
				{
					if(x != 0) x /= Length;
					if(y != 0) y /= Length;
					if(z != 0) z /= Length;
				}
			}

			CVector3 GetNormalised()
			{
				CVector3 Normalised(*this);
				Normalised.Normalise();

				return Normalised;
			}

			float GetLength()
			{
				return sqrt(x*x + y*y + z*z);
			}

			float GetDistance(CVector3& v)
			{
				float dx, dy, dz;
				dx = x - v.x;
				dy = y - v.y;
				dz = z - v.z;
				return sqrt(dx*dx + dy*dy + dz*dz);
			}

			//Interpolation with another vector, with a given weight
			CVector3 Lerp(CVector3& Vector, float Weight)
			{
				CVector3 Result;

				if (Weight <= 0.0f)
				{
					Result = (*this);
				}
				else if(Weight >= 1.0f)
				{
					Result = Vector;
				}
				else
				{
					Result.x = x + Weight * (Vector.x - x);
					Result.y = y + Weight * (Vector.y - y);
					Result.z = z + Weight * (Vector.z - z);
				}

				return Result;
			}
	};
}

#endif