#ifndef __SYM_3DMATHS
#define __SYM_3DMATHS

#ifdef SYM_DLL
#   define __declspec(dllexport)
#else
//#   define __declspec(dllimport)
#endif

#ifdef WIN32
#include <windows.h>
#endif

#include <cassert>
#include <iostream>
#include <vector>

#include <GL/gl.h>

#include "SYM_Matrix.h"
#include "SYM_Vector.h"

namespace sym
{
	namespace Maths
	{
		const float PI = 3.14159264f;
		const float TWO_PI = PI * 2.0f;
		const float HALF_PI = PI / 2.0f;
		const float PI_DIV_180 = PI / 180.0f;
		const float ONE80_DIV_PI = 180.0f / PI;

		const float DEGREES_TO_RADIANS = PI / 180.0f;
		const float RADIANS_TO_DEGREES = 180.0f / PI;

		int Round(double x);
		float RoundToNearest(float Value, int Nearest);
		int NextPowerOfTwo(int x);
	}

	//Forward declaration
	class CFace;
	class CTexCoord;

	class CFrustum
	{
		public:
			//Get and calculate frustum planes
			void Calculate();

			float* GetPlane(int Plane);

			//Collision tests
			bool PointInFrustum(CVector3 Point);
			bool SphereInFrustum(float x, float y, float z, float radius);
			bool CubeInFrustum(CVector3 Centre, float HalfLength);
			//bool AABBInfrustum(CBoundingBox AABB);

		private:

			//Planes
			float Frustum[6][4];

			//Normalise plane
			void NormalisePlane(int Plane);
	};

	//Normals functions
	CVector3 GetFaceNormal(CVector3 PointA, CVector3 PointB, CVector3 PointC);
	CVector3 CalculateTangent(CVector3 Vertices[3], CTexCoord UVCoords[3]);
	void CalculateNormals(std::vector<CVector3>& Vertex, std::vector<CFace>& Faces);

} //Namespace

#endif
