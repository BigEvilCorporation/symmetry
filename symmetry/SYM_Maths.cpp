#ifdef WIN32
#include <windows.h>
#endif

#include <iostream>
#include <math.h>
#include <float.h>
#include <stdlib.h>

#include "SYM_Engine.h"
#include "SYM_Maths.h"

enum FrustumPlanes
{
	RIGHT	= 0,
	LEFT	= 1,
	BOTTOM	= 2,
	TOP		= 3,
	BACK	= 4,
	FRONT	= 5
};

namespace sym
{
	namespace Maths
	{
		int Round(double x)
		{
			return (int)(x + 0.5);
		}

		float RoundToNearest(float Value, int Nearest)
		{
			return floor(Value / Nearest) * Nearest;
		}

		int NextPowerOfTwo(int x)
		{
			double logbase2 = log((double)x) / log((double)2);
			return Round(pow((double)2, (double)ceil(logbase2)));
		}
	}

	CVector3 GetFaceNormal(CVector3 PointA, CVector3 PointB, CVector3 PointC)
	{
		CVector3 Normal;

		CVector3 Edge1 = PointA - PointB;
		CVector3 Edge2 = PointB - PointC;

		Normal = Edge1.Cross(Edge2);

		return Normal;
	}

	void CalculateNormals(std::vector<CVector3>& Vertex, std::vector<CFace>& Faces)
	{
		//For calculating face normals
		CVector3 Point[3];

		//Loop through faces
		for (int f = 0; f < (int)Faces.size(); f++)
		{
			//Calculate face normal
			Faces[f].Normal = GetFaceNormal(Vertex[Faces[f].Indices[0]], Vertex[Faces[f].Indices[1]], Vertex[Faces[f].Indices[2]]);

			//New vertex normals
			Vertex[ Faces[f].Indices[0] ].Normal = new CVector3;
            Vertex[ Faces[f].Indices[1] ].Normal = new CVector3;
			Vertex[ Faces[f].Indices[2] ].Normal = new CVector3;

			//Add the face normal to the vertex normal of each vertex in this face
			*Vertex[ Faces[f].Indices[0] ].Normal += Faces[f].Normal;
			*Vertex[ Faces[f].Indices[1] ].Normal += Faces[f].Normal;
			*Vertex[ Faces[f].Indices[2] ].Normal += Faces[f].Normal;
		}

		//Loop through vertices
		for (int v = 0; v < (int)Vertex.size(); v++)
		{
			//Normalise the summed normals
			if(Vertex[v].Normal) Vertex[v].Normal->Normalise();
		}
	}

	CVector3 CalculateTangent(CVector3 Vertices[3], CTexCoord UVCoords[3])
	{
		if(!Vertices[0].Normal || !Vertices[1].Normal || !Vertices[2].Normal) return CVector3(0,0,0);

		CVector3 vAB = Vertices[1] - Vertices[0];
		CVector3 vAC = Vertices[2] - Vertices[0];
		CVector3 nA  = *Vertices[0].Normal;

		// Components of vectors to neighboring vertices that are orthogonal to the
		// vertex normal
		CVector3 vProjAB = vAB - (nA * nA.Dot(vAB));
		CVector3 vProjAC = vAC - (nA * nA.Dot(vAC));

		// tu texture coordinate differences
		float duAB = UVCoords[1].u - UVCoords[0].u;
		float duAC = UVCoords[2].u - UVCoords[0].u;

		// tv texture coordinate differences
		float dvAB = UVCoords[1].v - UVCoords[0].v;
		float dvAC = UVCoords[2].v - UVCoords[0].v;

		if( (duAC * dvAB) > (duAB * dvAC) )
		{
			duAC = -duAC;
			duAB = -duAB;
		}
	    
		CVector3 vTangent = (vProjAB * duAC) - (vProjAC * duAB);
		vTangent.Normalise();
		return vTangent;
	}

	void CFrustum::NormalisePlane(int Plane)
	{
        float magnitude = (float)sqrt( Frustum[Plane][0] * Frustum[Plane][0] +
                                       Frustum[Plane][1] * Frustum[Plane][1] +
                                       Frustum[Plane][2] * Frustum[Plane][2] );

        Frustum[Plane][0] /= magnitude;
        Frustum[Plane][1] /= magnitude;
        Frustum[Plane][2] /= magnitude;
        Frustum[Plane][3] /= magnitude;
    }

	float* CFrustum::GetPlane(int Plane)
	{
		return Frustum[Plane];
	}

	void CFrustum::Calculate()
	{
        float   proj[16];
        float   modl[16];
        float   clip[16];

        glGetFloatv( GL_PROJECTION_MATRIX, proj );
        glGetFloatv( GL_MODELVIEW_MATRIX, modl );

        //calcula o clipping plane pela multiplicacao as duas matrizes
        //TODO usar algebra.h para representacao das matrizes e multiplicacao
        clip[ 0] = modl[ 0] * proj[ 0] + modl[ 1] * proj[ 4] + modl[ 2] * proj[ 8] + modl[ 3] * proj[12];
        clip[ 1] = modl[ 0] * proj[ 1] + modl[ 1] * proj[ 5] + modl[ 2] * proj[ 9] + modl[ 3] * proj[13];
        clip[ 2] = modl[ 0] * proj[ 2] + modl[ 1] * proj[ 6] + modl[ 2] * proj[10] + modl[ 3] * proj[14];
        clip[ 3] = modl[ 0] * proj[ 3] + modl[ 1] * proj[ 7] + modl[ 2] * proj[11] + modl[ 3] * proj[15];

        clip[ 4] = modl[ 4] * proj[ 0] + modl[ 5] * proj[ 4] + modl[ 6] * proj[ 8] + modl[ 7] * proj[12];
        clip[ 5] = modl[ 4] * proj[ 1] + modl[ 5] * proj[ 5] + modl[ 6] * proj[ 9] + modl[ 7] * proj[13];
        clip[ 6] = modl[ 4] * proj[ 2] + modl[ 5] * proj[ 6] + modl[ 6] * proj[10] + modl[ 7] * proj[14];
        clip[ 7] = modl[ 4] * proj[ 3] + modl[ 5] * proj[ 7] + modl[ 6] * proj[11] + modl[ 7] * proj[15];

        clip[ 8] = modl[ 8] * proj[ 0] + modl[ 9] * proj[ 4] + modl[10] * proj[ 8] + modl[11] * proj[12];
        clip[ 9] = modl[ 8] * proj[ 1] + modl[ 9] * proj[ 5] + modl[10] * proj[ 9] + modl[11] * proj[13];
        clip[10] = modl[ 8] * proj[ 2] + modl[ 9] * proj[ 6] + modl[10] * proj[10] + modl[11] * proj[14];
        clip[11] = modl[ 8] * proj[ 3] + modl[ 9] * proj[ 7] + modl[10] * proj[11] + modl[11] * proj[15];

        clip[12] = modl[12] * proj[ 0] + modl[13] * proj[ 4] + modl[14] * proj[ 8] + modl[15] * proj[12];
        clip[13] = modl[12] * proj[ 1] + modl[13] * proj[ 5] + modl[14] * proj[ 9] + modl[15] * proj[13];
        clip[14] = modl[12] * proj[ 2] + modl[13] * proj[ 6] + modl[14] * proj[10] + modl[15] * proj[14];
        clip[15] = modl[12] * proj[ 3] + modl[13] * proj[ 7] + modl[14] * proj[11] + modl[15] * proj[15];

        //hora de extrair os lados:
        Frustum[RIGHT][0] = clip[ 3] - clip[ 0];
        Frustum[RIGHT][1] = clip[ 7] - clip[ 4];
        Frustum[RIGHT][2] = clip[11] - clip[ 8];
        Frustum[RIGHT][3] = clip[15] - clip[12];
        NormalisePlane( RIGHT);

        Frustum[LEFT][0] = clip[ 3] + clip[ 0];
        Frustum[LEFT][1] = clip[ 7] + clip[ 4];
        Frustum[LEFT][2] = clip[11] + clip[ 8];
        Frustum[LEFT][3] = clip[15] + clip[12];
        NormalisePlane( LEFT);

        Frustum[BOTTOM][0] = clip[ 3] + clip[ 1];
        Frustum[BOTTOM][1] = clip[ 7] + clip[ 5];
        Frustum[BOTTOM][2] = clip[11] + clip[ 9];
        Frustum[BOTTOM][3] = clip[15] + clip[13];
        NormalisePlane( BOTTOM);

        Frustum[TOP][0] = clip[ 3] - clip[ 1];
        Frustum[TOP][1] = clip[ 7] - clip[ 5];
        Frustum[TOP][2] = clip[11] - clip[ 9];
        Frustum[TOP][3] = clip[15] - clip[13];
        NormalisePlane(TOP);

        Frustum[BACK][0] = clip[ 3] - clip[ 2];
        Frustum[BACK][1] = clip[ 7] - clip[ 6];
        Frustum[BACK][2] = clip[11] - clip[10];
        Frustum[BACK][3] = clip[15] - clip[14];
        NormalisePlane(BACK);

        Frustum[FRONT][0] = clip[ 3] + clip[ 2];
        Frustum[FRONT][1] = clip[ 7] + clip[ 6];
        Frustum[FRONT][2] = clip[11] + clip[10];
        Frustum[FRONT][3] = clip[15] + clip[14];
        NormalisePlane(FRONT);
	}

	bool CFrustum::CubeInFrustum(CVector3 Centre, float HalfLength)
	{
	    for(int i = 0; i < 6; i++ )
	    {
            if(Frustum[i][0] * (Centre.x - HalfLength) + Frustum[i][1] * (Centre.y - HalfLength) + Frustum[i][2] * (Centre.z - HalfLength) + Frustum[i][3] > 0) continue;
            if(Frustum[i][0] * (Centre.x + HalfLength) + Frustum[i][1] * (Centre.y - HalfLength) + Frustum[i][2] * (Centre.z - HalfLength) + Frustum[i][3] > 0) continue;
            if(Frustum[i][0] * (Centre.x - HalfLength) + Frustum[i][1] * (Centre.y + HalfLength) + Frustum[i][2] * (Centre.z - HalfLength) + Frustum[i][3] > 0) continue;
            if(Frustum[i][0] * (Centre.x + HalfLength) + Frustum[i][1] * (Centre.y + HalfLength) + Frustum[i][2] * (Centre.z - HalfLength) + Frustum[i][3] > 0) continue;
            if(Frustum[i][0] * (Centre.x - HalfLength) + Frustum[i][1] * (Centre.y - HalfLength) + Frustum[i][2] * (Centre.z + HalfLength) + Frustum[i][3] > 0) continue;
            if(Frustum[i][0] * (Centre.x + HalfLength) + Frustum[i][1] * (Centre.y - HalfLength) + Frustum[i][2] * (Centre.z + HalfLength) + Frustum[i][3] > 0) continue;
            if(Frustum[i][0] * (Centre.x - HalfLength) + Frustum[i][1] * (Centre.y + HalfLength) + Frustum[i][2] * (Centre.z + HalfLength) + Frustum[i][3] > 0) continue;
            if(Frustum[i][0] * (Centre.x + HalfLength) + Frustum[i][1] * (Centre.y + HalfLength) + Frustum[i][2] * (Centre.z + HalfLength) + Frustum[i][3] > 0) continue;

            return false;
        }

        return true;
	}

	bool CFrustum::PointInFrustum(sym::CVector3 Point)
	{
		for(int i = 0; i < 6; i++ )
	    {
            if(Frustum[i][0] * (Point.x) + Frustum[i][1] * (Point.y) + Frustum[i][2] * (Point.z) + Frustum[i][3] > 0) continue;

            return false;
        }

        return true;
	}

} //Namespace
