#include <math.h>

// TOLUA_BEGIN

class CVector3
{
    public:

        float x;
        float y;
        float z;

        CVector3() { x = y = z = 0.0f; }

        CVector3(float _x, float _y, float _z)
        {
            x = _x;
            y = _y;
            z = _z;
        }

        CVector3(const CVector3 & Vector)
        {
            x = Vector.x;
            y = Vector.y;
            z = Vector.z;
        }

        ~CVector3(void) {}

        /*
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
        */

        float Mul(CVector3 Vector)
        {
            return (x * Vector.x) + (y * Vector.y) + (z * Vector.z);
        }

        CVector3 operator *(float Scalar)
        {
            return CVector3(x * Scalar, y * Scalar, z * Scalar);
        }

        CVector3 operator /(float Scalar)
        {
            float inv = 1.0f/Scalar;
            return CVector3(x * inv, y * inv, z * inv);
        }

        CVector3 operator *(CVector3 Vector)
        {
            return CVector3(x * Vector.x, y * Vector.y, z * Vector.z);
        }

        CVector3 operator -(CVector3 Vector)
        {
            return CVector3(x - Vector.x, y - Vector.y, z - Vector.z);
        }

        CVector3 operator +(CVector3 Vector)
        {
            return CVector3(Vector.x + x, Vector.y + y, Vector.z + z);
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

            x /= Length;
            y /= Length;
            z /= Length;
        }
};
// TOLUA_END
