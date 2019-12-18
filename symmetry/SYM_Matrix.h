#ifndef __SYM_MATRIX
#define __SYM_MATRIX

#ifdef SYM_DLL
#   define __declspec(dllexport)
#else
//#   define __declspec(dllimport)
#endif

#include <math.h>
#include <memory.h>

#include "SYM_Vector.h"

namespace sym
{
	class CMatrix4
	{
		public:
			//Constructor
			CMatrix4() { SetIdentity(); }

			CMatrix4(float *Matrix)
			{
				memcpy(_Matrix, Matrix, 16 * sizeof(float));
			}

			CMatrix4(const CMatrix4 & Mat)
			{
				memcpy(_Matrix, Mat._Matrix, 16 * sizeof(float));
			}

			//lookat matrix constructor
			CMatrix4(CVector3 Position, CVector3 Up, CVector3 Forward)
			{
				CVector3 AxisZ = Forward - Position;
				AxisZ.Normalise();

				CVector3 AxisX = Up.Cross(AxisZ);
				AxisX.Normalise();

				CVector3 AxisY = AxisZ.Cross(AxisX);
				_Matrix[0] = AxisX.x;
				_Matrix[1] = AxisY.x;
				_Matrix[2] = AxisZ.x;
				_Matrix[3] = 0;

				_Matrix[4] = AxisX.y;
				_Matrix[5] = AxisY.y;
				_Matrix[6] = AxisZ.y;
				_Matrix[7] = 0;

				_Matrix[8] = AxisX.z;
				_Matrix[9] = AxisY.z;
				_Matrix[10] = AxisZ.z;
				_Matrix[11] = 0;

				_Matrix[12] = -AxisX.Dot(Position);
				_Matrix[13] = -AxisY.Dot(Position);
				_Matrix[14] = -AxisZ.Dot(Position);
				_Matrix[15] = 1.0f;
			}

			~CMatrix4(void) {}

			//Transformation operations
			void SetTranslation(CVector3 Pos)
			{
				set(3, 0, Pos.x);
				set(3, 1, Pos.y);
				set(3, 2, Pos.z);
			}

			void SetRotation(float Angle, CVector3 Axis)
			{
				float s = (float)sin(-Angle * 0.0174532925);
				float c = (float)cos(-Angle * 0.0174532925);
				float t = 1 - c;

				CVector3 Ax;
				CVector3 AxNorm;

				Ax.x = Axis.x;
				Ax.y = Axis.y;
				Ax.z = Axis.z;

				CVector3 Ax2 = Ax / sqrt(Ax * Ax);

				float x = Ax2.x;
				float y = Ax2.y;
				float z = Ax2.z;

				set(0,0,t*x*x+c);
				set(1,0,t*y*x+s*z);
				set(2,0,t*z*x-s*y);

				set(0,1,t*x*y-s*z);
				set(1,1,t*y*y+c);
				set(2,1,t*z*y+s*x);

				set(0,2,t*x*z+s*y);
				set(1,2,t*y*z-s*x);
				set(2,2,t*z*z+c);
			}

			CVector3 TransformVector(CVector3 Vector)
			{
				return CVector3(	Vector.x * get(0, 0) + Vector.y * get(1, 0) + Vector.z * get(2, 0) + get(3, 0),
									Vector.x * get(0, 1) + Vector.y * get(1, 1) + Vector.z * get(2, 1) + get(3, 1),
									Vector.x * get(0, 2) + Vector.y * get(1, 2) + Vector.z * get(2, 2) + get(3, 2));
			}

			CVector3 TranslateVector(CVector3 Vector)
			{
				return CVector3(	Vector.x + get(3, 0),
									Vector.y + get(3, 1),
									Vector.z + get(3, 2));
			}

			CVector3 UnTranslateVector(CVector3 Vector)
			{
				return CVector3(	Vector.x - get(3, 0),
									Vector.y - get(3, 1),
									Vector.z - get(3, 2));
			}

			CVector3 RotateVector(CVector3 Vector)
			{
				return CVector3(	Vector.x * get(0, 0) + Vector.y * get(1, 0) + Vector.z * get(2, 0),
									Vector.x * get(0, 1) + Vector.y * get(1, 1) + Vector.z * get(2, 1),
									Vector.x * get(0, 2) + Vector.y * get(1, 2) + Vector.z * get(2, 2));
			}

			CVector3 UnRotateVector(CVector3 Vector)
			{
				return CVector3(	Vector.x * get(0, 0) + Vector.y * get(0, 1) + Vector.z * get(0, 2),
									Vector.x * get(1, 0) + Vector.y * get(1, 1) + Vector.z * get(1, 2),
									Vector.x * get(2, 0) + Vector.y * get(2, 1) + Vector.z * get(2, 2));
			}

			CVector3 GetTranslation()
			{
				return CVector3(get(3, 0), get(3, 1), get(3, 2));
			}

			void SetIdentity()
			{
				float _Identity[16] =
				{
					1.0, 0.0, 0.0, 0.0,
					0.0, 1.0, 0.0, 0.0,
					0.0, 0.0, 1.0, 0.0,
					0.0, 0.0, 0.0, 1.0
				};

				memcpy(_Matrix, _Identity, 16 * sizeof(float));
			}

			CMatrix4 GetTransposeMatrix()
			{
				CMatrix4 Result;

				//Loop through column
				for (int i = 0; i < 4; i++)
				{
					//Loop through rows
					for (int j = i + 1; j < 4; j++)
					{
						//Save value
						float save = get(i, j);

						//Transpose value
						Result.set(i, j, get(j, i));

						//Restore saved value
						Result.set(j, i, save);
					}
				}

				return Result;
			}

			CMatrix4 GetInverse()
			{
				CMatrix4 Matrix;

				float d =   (get(0, 0) * get(1, 1) - get(0, 1) * get(1, 0)) * (get(2, 2) * get(3, 3) - get(2, 3) * get(3, 2)) -
				(get(0, 0) * get(1, 2) - get(0, 2) * get(1, 0)) * (get(2, 1) * get(3, 3) - get(2, 3) * get(3, 1)) +
				(get(0, 0) * get(1, 3) - get(0, 3) * get(1, 0)) * (get(2, 1) * get(3, 2) - get(2, 2) * get(3, 1)) +
				(get(0, 1) * get(1, 2) - get(0, 2) * get(1, 1)) * (get(2, 0) * get(3, 3) - get(2, 3) * get(3, 0)) -
				(get(0, 1) * get(1, 3) - get(0, 3) * get(1, 1)) * (get(2, 0) * get(3, 2) - get(2, 2) * get(3, 0)) +
				(get(0, 2) * get(1, 3) - get(0, 3) * get(1, 2)) * (get(2, 0) * get(3, 1) - get(2, 1) * get(3, 0));

				const float PrecisionLimit = 1.0e-4f;

				d = 1.0f / d;

				if(fabsf(d) < PrecisionLimit)
				{
					Matrix.SetIdentity();
					return Matrix;
				}

				Matrix.set(0, 0, (d * (get(1, 1) * (get(2, 2) * get(3, 3) - get(2, 3) * get(3, 2)) + get(1, 2) * (get(2, 3) * get(3, 1) - get(2, 1) * get(3, 3)) + get(1, 3) * (get(2, 1) * get(3, 2) - get(2, 2) * get(3, 1)))));
				Matrix.set(0, 1, (d * (get(2, 1) * (get(0, 2) * get(3, 3) - get(0, 3) * get(3, 2)) + get(2, 2) * (get(0, 3) * get(3, 1) - get(0, 1) * get(3, 3)) + get(2, 3) * (get(0, 1) * get(3, 2) - get(0, 2) * get(3, 1)))));
				Matrix.set(0, 2, (d * (get(3, 1) * (get(0, 2) * get(1, 3) - get(0, 3) * get(1, 2)) + get(3, 2) * (get(0, 3) * get(1, 1) - get(0, 1) * get(1, 3)) + get(3, 3) * (get(0, 1) * get(1, 2) - get(0, 2) * get(1, 1)))));
				Matrix.set(0, 3, (d * (get(0, 1) * (get(1, 3) * get(2, 2) - get(1, 2) * get(2, 3)) + get(0, 2) * (get(1, 1) * get(2, 3) - get(1, 3) * get(2, 1)) + get(0, 3) * (get(1, 2) * get(2, 1) - get(1, 1) * get(2, 2)))));
				Matrix.set(1, 0, (d * (get(1, 2) * (get(2, 0) * get(3, 3) - get(2, 3) * get(3, 0)) + get(1, 3) * (get(2, 2) * get(3, 0) - get(2, 0) * get(3, 2)) + get(1, 0) * (get(2, 3) * get(3, 2) - get(2, 2) * get(3, 3)))));
				Matrix.set(1, 1, (d * (get(2, 2) * (get(0, 0) * get(3, 3) - get(0, 3) * get(3, 0)) + get(2, 3) * (get(0, 2) * get(3, 0) - get(0, 0) * get(3, 2)) + get(2, 0) * (get(0, 3) * get(3, 2) - get(0, 2) * get(3, 3)))));
				Matrix.set(1, 2, (d * (get(3, 2) * (get(0, 0) * get(1, 3) - get(0, 3) * get(1, 0)) + get(3, 3) * (get(0, 2) * get(1, 0) - get(0, 0) * get(1, 2)) + get(3, 0) * (get(0, 3) * get(1, 2) - get(0, 2) * get(1, 3)))));
				Matrix.set(1, 3, (d * (get(0, 2) * (get(1, 3) * get(2, 0) - get(1, 0) * get(2, 3)) + get(0, 3) * (get(1, 0) * get(2, 2) - get(1, 2) * get(2, 0)) + get(0, 0) * (get(1, 2) * get(2, 3) - get(1, 3) * get(2, 2)))));
				Matrix.set(2, 0, (d * (get(1, 3) * (get(2, 0) * get(3, 1) - get(2, 1) * get(3, 0)) + get(1, 0) * (get(2, 1) * get(3, 3) - get(2, 3) * get(3, 1)) + get(1, 1) * (get(2, 3) * get(3, 0) - get(2, 0) * get(3, 3)))));
				Matrix.set(2, 1, (d * (get(2, 3) * (get(0, 0) * get(3, 1) - get(0, 1) * get(3, 0)) + get(2, 0) * (get(0, 1) * get(3, 3) - get(0, 3) * get(3, 1)) + get(2, 1) * (get(0, 3) * get(3, 0) - get(0, 0) * get(3, 3)))));
				Matrix.set(2, 2, (d * (get(3, 3) * (get(0, 0) * get(1, 1) - get(0, 1) * get(1, 0)) + get(3, 0) * (get(0, 1) * get(1, 3) - get(0, 3) * get(1, 1)) + get(3, 1) * (get(0, 3) * get(1, 0) - get(0, 0) * get(1, 3)))));
				Matrix.set(2, 3, (d * (get(0, 3) * (get(1, 1) * get(2, 0) - get(1, 0) * get(2, 1)) + get(0, 0) * (get(1, 3) * get(2, 1) - get(1, 1) * get(2, 3)) + get(0, 1) * (get(1, 0) * get(2, 3) - get(1, 3) * get(2, 0)))));
				Matrix.set(3, 0, (d * (get(1, 0) * (get(2, 2) * get(3, 1) - get(2, 1) * get(3, 2)) + get(1, 1) * (get(2, 0) * get(3, 2) - get(2, 2) * get(3, 0)) + get(1, 2) * (get(2, 1) * get(3, 0) - get(2, 0) * get(3, 1)))));
				Matrix.set(3, 1, (d * (get(2, 0) * (get(0, 2) * get(3, 1) - get(0, 1) * get(3, 2)) + get(2, 1) * (get(0, 0) * get(3, 2) - get(0, 2) * get(3, 0)) + get(2, 2) * (get(0, 1) * get(3, 0) - get(0, 0) * get(3, 1)))));
				Matrix.set(3, 2, (d * (get(3, 0) * (get(0, 2) * get(1, 1) - get(0, 1) * get(1, 2)) + get(3, 1) * (get(0, 0) * get(1, 2) - get(0, 2) * get(1, 0)) + get(3, 2) * (get(0, 1) * get(1, 0) - get(0, 0) * get(1, 1)))));
				Matrix.set(3, 3, (d * (get(0, 0) * (get(1, 1) * get(2, 2) - get(1, 2) * get(2, 1)) + get(0, 1) * (get(1, 2) * get(2, 0) - get(1, 0) * get(2, 2)) + get(0, 2) * (get(1, 0) * get(2, 1) - get(1, 1) * get(2, 0)))));

				return Matrix;
			}

			//Operators
			/*
			CVector3 operator*(CVector3 Vec)
			{
				float prod[4] = { 0,0,0,0 };

				for (int r = 0; r < 4; r++)
				{
					for (int c = 0; c < 3; c++)
						prod[r] += Vec[c] * (float)get(c, r);

					prod[r] += (float)get(3, r);
				}

				float div = 1.0f / prod[3];

				return CVector3(prod[0] * div, prod[1] * div, prod[2] * div);
			}
			*/

			CMatrix4 operator*(CMatrix4 Mat)
			{
				CMatrix4 Temp;

				Temp._Matrix[ 0] = _Matrix[ 0] * Mat._Matrix[ 0] + _Matrix[ 1] * Mat._Matrix[ 4] + _Matrix[ 2] * Mat._Matrix[ 8] + _Matrix[ 3] * Mat._Matrix[12];
				Temp._Matrix[ 1] = _Matrix[ 0] * Mat._Matrix[ 1] + _Matrix[ 1] * Mat._Matrix[ 5] + _Matrix[ 2] * Mat._Matrix[ 9] + _Matrix[ 3] * Mat._Matrix[13];
				Temp._Matrix[ 2] = _Matrix[ 0] * Mat._Matrix[ 2] + _Matrix[ 1] * Mat._Matrix[ 6] + _Matrix[ 2] * Mat._Matrix[10] + _Matrix[ 3] * Mat._Matrix[14];
				Temp._Matrix[ 3] = _Matrix[ 0] * Mat._Matrix[ 3] + _Matrix[ 1] * Mat._Matrix[ 7] + _Matrix[ 2] * Mat._Matrix[11] + _Matrix[ 3] * Mat._Matrix[15];

				Temp._Matrix[ 4] = _Matrix[ 4] * Mat._Matrix[ 0] + _Matrix[ 5] * Mat._Matrix[ 4] + _Matrix[ 6] * Mat._Matrix[ 8] + _Matrix[ 7] * Mat._Matrix[12];
				Temp._Matrix[ 5] = _Matrix[ 4] * Mat._Matrix[ 1] + _Matrix[ 5] * Mat._Matrix[ 5] + _Matrix[ 6] * Mat._Matrix[ 9] + _Matrix[ 7] * Mat._Matrix[13];
				Temp._Matrix[ 6] = _Matrix[ 4] * Mat._Matrix[ 2] + _Matrix[ 5] * Mat._Matrix[ 6] + _Matrix[ 6] * Mat._Matrix[10] + _Matrix[ 7] * Mat._Matrix[14];
				Temp._Matrix[ 7] = _Matrix[ 4] * Mat._Matrix[ 3] + _Matrix[ 5] * Mat._Matrix[ 7] + _Matrix[ 6] * Mat._Matrix[11] + _Matrix[ 7] * Mat._Matrix[15];

				Temp._Matrix[ 8] = _Matrix[ 8] * Mat._Matrix[ 0] + _Matrix[ 9] * Mat._Matrix[ 4] + _Matrix[10] * Mat._Matrix[ 8] + _Matrix[11] * Mat._Matrix[12];
				Temp._Matrix[ 9] = _Matrix[ 8] * Mat._Matrix[ 1] + _Matrix[ 9] * Mat._Matrix[ 5] + _Matrix[10] * Mat._Matrix[ 9] + _Matrix[11] * Mat._Matrix[13];
				Temp._Matrix[10] = _Matrix[ 8] * Mat._Matrix[ 2] + _Matrix[ 9] * Mat._Matrix[ 6] + _Matrix[10] * Mat._Matrix[10] + _Matrix[11] * Mat._Matrix[14];
				Temp._Matrix[11] = _Matrix[ 8] * Mat._Matrix[ 3] + _Matrix[ 9] * Mat._Matrix[ 7] + _Matrix[10] * Mat._Matrix[11] + _Matrix[11] * Mat._Matrix[15];

				Temp._Matrix[12] = _Matrix[12] * Mat._Matrix[ 0] + _Matrix[13] * Mat._Matrix[ 4] + _Matrix[14] * Mat._Matrix[ 8] + _Matrix[15] * Mat._Matrix[12];
				Temp._Matrix[13] = _Matrix[12] * Mat._Matrix[ 1] + _Matrix[13] * Mat._Matrix[ 5] + _Matrix[14] * Mat._Matrix[ 9] + _Matrix[15] * Mat._Matrix[13];
				Temp._Matrix[14] = _Matrix[12] * Mat._Matrix[ 2] + _Matrix[13] * Mat._Matrix[ 6] + _Matrix[14] * Mat._Matrix[10] + _Matrix[15] * Mat._Matrix[14];
				Temp._Matrix[15] = _Matrix[12] * Mat._Matrix[ 3] + _Matrix[13] * Mat._Matrix[ 7] + _Matrix[14] * Mat._Matrix[11] + _Matrix[15] * Mat._Matrix[15];

				return Temp;
			}

			CMatrix4 operator*(float Scalar)
			{
				float Result[16];

				for(int i = 0; i < 16; i++)
				{
					Result[i] = _Matrix[i] * Scalar;
				}

				return CMatrix4(Result);
			}

			CMatrix4 operator+(CMatrix4 Mat)
			{
				float Result[16];
				float *Input = Mat.GetAsFloatArray();

				for(int i = 0; i < 16; i++)
				{
					Result[i] = _Matrix[i] + Input[i];
				}

				return CMatrix4(Result);
			}

			CMatrix4 operator-(CMatrix4 Mat)
			{
				CMatrix4 Result;

				for(int i = 0; i < 16; i++)
				{
					Result._Matrix[i] = _Matrix[i] - Mat._Matrix[i];
				}

				return Result;
			}

			//Return the matrix
			float * GetAsFloatArray() { return _Matrix; }

			CMatrix4 GetProductMatrix(CMatrix4 &Mat)
			{
				CMatrix4 Result;

				Result._Matrix[0] = _Matrix[0]*Mat._Matrix[0] + _Matrix[4]*Mat._Matrix[1] + _Matrix[8]*Mat._Matrix[2] + _Matrix[12]*Mat._Matrix[3];
				Result._Matrix[1] = _Matrix[1]*Mat._Matrix[0] + _Matrix[5]*Mat._Matrix[1] + _Matrix[9]*Mat._Matrix[2] + _Matrix[13]*Mat._Matrix[3];
				Result._Matrix[2] = _Matrix[2]*Mat._Matrix[0] + _Matrix[6]*Mat._Matrix[1] + _Matrix[10]*Mat._Matrix[2] + _Matrix[14]*Mat._Matrix[3];
				Result._Matrix[3] = _Matrix[3]*Mat._Matrix[0] + _Matrix[7]*Mat._Matrix[1] + _Matrix[11]*Mat._Matrix[2] + _Matrix[15]*Mat._Matrix[3];

				Result._Matrix[4] = _Matrix[0]*Mat._Matrix[4] + _Matrix[4]*Mat._Matrix[5] + _Matrix[8]*Mat._Matrix[6] + _Matrix[12]*Mat._Matrix[7];
				Result._Matrix[5] = _Matrix[1]*Mat._Matrix[4] + _Matrix[5]*Mat._Matrix[5] + _Matrix[9]*Mat._Matrix[6] + _Matrix[13]*Mat._Matrix[7];
				Result._Matrix[6] = _Matrix[2]*Mat._Matrix[4] + _Matrix[6]*Mat._Matrix[5] + _Matrix[10]*Mat._Matrix[6] + _Matrix[14]*Mat._Matrix[7];
				Result._Matrix[7] = _Matrix[3]*Mat._Matrix[4] + _Matrix[7]*Mat._Matrix[5] + _Matrix[11]*Mat._Matrix[6] + _Matrix[15]*Mat._Matrix[7];

				Result._Matrix[8] = _Matrix[0]*Mat._Matrix[8] + _Matrix[4]*Mat._Matrix[9] + _Matrix[8]*Mat._Matrix[10] + _Matrix[12]*Mat._Matrix[11];
				Result._Matrix[9] = _Matrix[1]*Mat._Matrix[8] + _Matrix[5]*Mat._Matrix[9] + _Matrix[9]*Mat._Matrix[10] + _Matrix[13]*Mat._Matrix[11];
				Result._Matrix[10] = _Matrix[2]*Mat._Matrix[8] + _Matrix[6]*Mat._Matrix[9] + _Matrix[10]*Mat._Matrix[10] + _Matrix[14]*Mat._Matrix[11];
				Result._Matrix[11] = _Matrix[3]*Mat._Matrix[8] + _Matrix[7]*Mat._Matrix[9] + _Matrix[11]*Mat._Matrix[10] + _Matrix[15]*Mat._Matrix[11];

				Result._Matrix[12] = _Matrix[0]*Mat._Matrix[12] + _Matrix[4]*Mat._Matrix[13] + _Matrix[8]*Mat._Matrix[14] + _Matrix[12]*Mat._Matrix[15];
				Result._Matrix[13] = _Matrix[1]*Mat._Matrix[12] + _Matrix[5]*Mat._Matrix[13] + _Matrix[9]*Mat._Matrix[14] + _Matrix[13]*Mat._Matrix[15];
				Result._Matrix[14] = _Matrix[2]*Mat._Matrix[12] + _Matrix[6]*Mat._Matrix[13] + _Matrix[10]*Mat._Matrix[14] + _Matrix[14]*Mat._Matrix[15];
				Result._Matrix[15] = _Matrix[3]*Mat._Matrix[12] + _Matrix[7]*Mat._Matrix[13] + _Matrix[11]*Mat._Matrix[14] + _Matrix[15]*Mat._Matrix[15];

				return Result;
			}

			CMatrix4 GetInterpolatedMatrix(CMatrix4 &Mat, float Time) const
			{
				CMatrix4 Result;

				for (int i = 0; i < 16; i += 4)
				{
					Result._Matrix[i+0] = _Matrix[i+0] + ( Mat._Matrix[i+0] - _Matrix[i+0] ) * Time;
					Result._Matrix[i+1] = _Matrix[i+1] + ( Mat._Matrix[i+1] - _Matrix[i+1] ) * Time;
					Result._Matrix[i+2] = _Matrix[i+2] + ( Mat._Matrix[i+2] - _Matrix[i+2] ) * Time;
					Result._Matrix[i+3] = _Matrix[i+3] + ( Mat._Matrix[i+3] - _Matrix[i+3] ) * Time;
				}

				return Result;
			}

			float Tr()
			{
				return get(1, 1) + get(2, 2) + get(3, 3) + get(4, 4);
			}

			inline float get(const int col, const int row) const
			{
				return _Matrix[col * 4 + row];
			}

			inline void set(const int col, const int row, const float val)
			{
				_Matrix[col * 4 + row] = val;
			}

			//The 4x4 matrix
			float _Matrix[16];
	};

}

#endif
