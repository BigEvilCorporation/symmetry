#ifndef __SYM_MATRIX
#define __SYM_MATRIX

#include <math.h>
#include <memory.h>

#include "SYM_Vector.h"

namespace SYM
{
	class SYM_MATRIX4
	{
		public:
			//Constructor
			SYM_MATRIX4() { Reset(); }

			SYM_MATRIX4(float *Matrix)
			{
				memcpy(_Matrix, Matrix, 16 * sizeof(float));
			}

			SYM_MATRIX4(const SYM_MATRIX4 & Mat)
			{
				memcpy(_Matrix, Mat._Matrix, 16 * sizeof(float));
			}

			~SYM_MATRIX4(void) {}

			//Transformation operations
			void SetTranslation(SYM_VECTOR3 Pos)
			{
				set(3, 0, Pos.x);
				set(3, 1, Pos.y);
				set(3, 2, Pos.z);
			}

			void SetRotation(float Angle, SYM_VECTOR3 Axis)
			{
				float s = sin(-Angle * 0.0174532925);
				float c = cos(-Angle * 0.0174532925);
				float t = 1 - c;

				SYM_VECTOR3 Ax;
				SYM_VECTOR3 AxNorm;

				Ax.x = Axis.x;
				Ax.y = Axis.y;
				Ax.z = Axis.z;

				SYM_VECTOR3 Ax2 = Ax / sqrt(Ax * Ax);

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

			SYM_VECTOR3 TransformVector(SYM_VECTOR3 Vector)
			{
				return SYM_VECTOR3( Vector.x * get(0, 0) + Vector.y * get(1, 0) + Vector.z * get(2, 0) + get(3, 0),
									Vector.x * get(0, 1) + Vector.y * get(1, 1) + Vector.z * get(2, 1) + get(3, 1),
									Vector.x * get(0, 2) + Vector.y * get(1, 2) + Vector.z * get(2, 2) + get(3, 2));
			}

			SYM_VECTOR3 RotateVector(SYM_VECTOR3 Vector)
			{
				return SYM_VECTOR3( Vector.x * get(0, 0) + Vector.y * get(1, 0) + Vector.z * get(2, 0),
									Vector.x * get(0, 1) + Vector.y * get(1, 1) + Vector.z * get(2, 1),
									Vector.x * get(0, 2) + Vector.y * get(1, 2) + Vector.z * get(2, 2));
			}

			SYM_VECTOR3 UnrotateVector(SYM_VECTOR3 Vector)
			{
				return SYM_VECTOR3( Vector.x * get(0, 0) + Vector.y * get(0, 1) + Vector.z * get(0, 2),
									Vector.x * get(1, 0) + Vector.y * get(1, 1) + Vector.z * get(1, 2),
									Vector.x * get(2, 0) + Vector.y * get(2, 1) + Vector.z * get(2, 2));
			}

			SYM_VECTOR3 GetTranslation()
			{
				return SYM_VECTOR3(get(3, 0), get(3, 1), get(3, 2));
			}

			//Reset matrix
			void Reset()
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

			void Identity() { Reset(); }

			//Inverse and transpose the matrix
			void Transpose()
			{
				//Loop through column
				for (int i = 0; i < 4; i++)
				{
					//Loop through rows
					for (int j = i + 1; j < 4; j++)
					{
						//Save value
						float save = get(i, j);

						//Transpose value
						set(i, j, get(j, i));

						//Restore saved value
						set(j, i, save);
					}
				}
			}

			SYM_MATRIX4 GetInverse()
			{
				SYM_MATRIX4 Matrix;

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
					Matrix.Identity();
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
			SYM_VECTOR3 operator*(SYM_VECTOR3 Vec)
			{
				float prod[4] = { 0,0,0,0 };

				for (int r = 0; r < 4; r++)
				{
					for (int c = 0; c < 3; c++)
						prod[r] += Vec[c] * (float)get(c, r);

					prod[r] += (float)get(3, r);
				}

				float div = 1.0f / prod[3];

				return SYM_VECTOR3(prod[0] * div, prod[1] * div, prod[2] * div);
			}
			*/

			SYM_MATRIX4 operator*(SYM_MATRIX4 Mat)
			{
				SYM_MATRIX4 Temp;

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

			//Return the matrix
			float * GetAsFloatArray() { return _Matrix; }


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
