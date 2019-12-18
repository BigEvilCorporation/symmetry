#include <math.h>

#include "SYM_Brush.h"
#include "SYM_AABB.h"
#include "SYM_Geometry.h"
#include "SYM_Engine.h"

namespace sym
{
	CBrush::CBrush()
	{
		MeshSubset = 0;
	}

	CBrush::~CBrush()
	{
	    if(MeshSubset) delete MeshSubset;
	}

	CBrush::CBrush(std::string BrushName, int GeomType, int PhysType, CVector3 Dim)
	{
		Create(BrushName, GeomType, PhysType, Dim);
	}

	void CBrush::Create(std::string BrushName, int GeomType, int PhysType, CVector3 Dim)
	{
		//Create new mesh
		Mesh = new CMesh;

		//Create new mesh subset
		MeshSubset = new CMeshSubset;

		//Set dimentions
		Dimentions = Dim;

		//Set physics type
		PhysicsType = PhysType;

		//Switch type
		switch(GeomType)
		{
			case SYM_BOX:
				//Create a box
				CreateBoxGeometry(Dimentions);

				//Set type
                Type = SYM_BOX;

				break;

			case SYM_PLANE:
				//Create geometry
				CreatePlaneGeometry(Dimentions);

				//Set type
                Type = SYM_PLANE;

				break;

			case SYM_CYLINDER:
				//Create a cylinder
				CreateCylinderGeometry(Dimentions);

				//Set type
                Type = SYM_CYLINDER;

				break;

			case SYM_SPHERE:
				//Create a sphere
				CreateSphereGeometry(Dimentions);

				//Set type
                Type = SYM_SPHERE;

				break;

			default:
				return;
		}

		//Calculate AABB
		//RecalculateAABB();

		//Process vertex/face data
		MeshSubset->Process();

		//Add subset to mesh
		Mesh->Subsets.push_back(*MeshSubset);

		//Set name
		Name = BrushName;

		//Setup collision body
		switch(PhysicsType)
		{
			case PHYSICS_OBJ_BOX:
				CreateBox(Mesh->AABB.Size);
				break;

			case PHYSICS_OBJ_SPHERE:
				CreateSphere(Mesh->AABB.Size);
				break;

			case PHYSICS_OBJ_CAPSULE:
				CreateCapsule(Mesh->AABB.Size);
				break;

			case PHYSICS_OBJ_CUSTOM:
				CreateCustom(Mesh->Subsets[0].Vertices);
				break;

			case LEVEL_COLLISION:
				CreateLevel(Mesh->Subsets[0].Vertices, Mesh->Subsets[0].Faces);
				break;

			case NO_PHYSICS:
				break;

			default: 
				CreateBox(Mesh->AABB.Size);
				break;
		}

		//Set physics geometry
		if(PhysicsType != NO_PHYSICS) SetGeometry(dynamic_cast<CGeometry*>(this));
	}

	void CBrush::Resize(CVector3 Dim)
	{
		Create(Name, Type, PhysicsType, Dim);
	}

	void CBrush::CreatePlaneGeometry(CVector3 Size)
	{
		MeshSubset->Vertices.push_back(CVector3(-Size.x * 0.5f, 0.0f, -Size.z * 0.5f));
		Mesh->AABB.AddVertex(MeshSubset->Vertices.back());

		MeshSubset->Vertices.push_back(CVector3(-Size.x * 0.5f, 0.0f, Size.z * 0.5f));
		Mesh->AABB.AddVertex(MeshSubset->Vertices.back());

		MeshSubset->Vertices.push_back(CVector3(Size.x * 0.5f, 0.0f, Size.z * 0.5f));
		Mesh->AABB.AddVertex(MeshSubset->Vertices.back());

		MeshSubset->Vertices.push_back(CVector3(Size.x * 0.5f, 0.0f, -Size.z * 0.5f));
		Mesh->AABB.AddVertex(MeshSubset->Vertices.back());

		MeshSubset->Faces.push_back(CFace(0, 1, 2, CTexCoord(0.0f, 0.0f), CTexCoord(0.0f, 1.0f), CTexCoord(1.0f, 1.0f)));
		MeshSubset->Faces.push_back(CFace(2, 3, 0, CTexCoord(1.0f, 1.0f), CTexCoord(1.0f, 0.0f), CTexCoord(0.0f, 0.0f)));
	}

	void CBrush::CreateBoxGeometry(CVector3 Size)
	{
		CVector3 TVertex;
		CFace TFace;

		TVertex.x = -Size.x * 0.5f;
		TVertex.y = -Size.y * 0.5f;
		TVertex.z = -Size.z * 0.5f;
		MeshSubset->Vertices.push_back(TVertex);
		Mesh->AABB.AddVertex(TVertex);

		TVertex.x = -Size.x * 0.5f;
		TVertex.y = -Size.y * 0.5f;
		TVertex.z = Size.z * 0.5f;
		MeshSubset->Vertices.push_back(TVertex);
		Mesh->AABB.AddVertex(TVertex);

		TVertex.x = Size.x * 0.5f;
		TVertex.y = -Size.y * 0.5f;
		TVertex.z = Size.z * 0.5f;
		MeshSubset->Vertices.push_back(TVertex);
		Mesh->AABB.AddVertex(TVertex);

		TVertex.x = Size.x * 0.5f;
		TVertex.y = -Size.y * 0.5f;
		TVertex.z = -Size.z * 0.5f;
		MeshSubset->Vertices.push_back(TVertex);
		Mesh->AABB.AddVertex(TVertex);

		TVertex.x = -Size.x * 0.5f;
		TVertex.y = Size.y * 0.5f;
		TVertex.z = -Size.z * 0.5f;
		MeshSubset->Vertices.push_back(TVertex);
		Mesh->AABB.AddVertex(TVertex);

		TVertex.x = -Size.x * 0.5f;
		TVertex.y = Size.y * 0.5f;
		TVertex.z = Size.z * 0.5f;
		MeshSubset->Vertices.push_back(TVertex);
		Mesh->AABB.AddVertex(TVertex);

		TVertex.x = Size.x * 0.5f;
		TVertex.y = Size.y * 0.5f;
		TVertex.z = Size.z * 0.5f;
		MeshSubset->Vertices.push_back(TVertex);
		Mesh->AABB.AddVertex(TVertex);

		TVertex.x = Size.x * 0.5f;
		TVertex.y = Size.y * 0.5f;
		TVertex.z = -Size.z * 0.5f;
		MeshSubset->Vertices.push_back(TVertex);
		Mesh->AABB.AddVertex(TVertex);

		//TOP
		TVertex.x = -Size.x * 0.5f;
		TVertex.y = Size.y * 0.5f;
		TVertex.z = -Size.z * 0.5f;
		MeshSubset->Vertices.push_back(TVertex);
		Mesh->AABB.AddVertex(TVertex);

		TVertex.x = -Size.x * 0.5f;
		TVertex.y = Size.y * 0.5f;
		TVertex.z = Size.z * 0.5f;
		MeshSubset->Vertices.push_back(TVertex);
		Mesh->AABB.AddVertex(TVertex);

		TVertex.x = Size.x * 0.5f;
		TVertex.y = Size.y * 0.5f;
		TVertex.z = Size.z * 0.5f;
		MeshSubset->Vertices.push_back(TVertex);
		Mesh->AABB.AddVertex(TVertex);

		TVertex.x = Size.x * 0.5f;
		TVertex.y = Size.y * 0.5f;
		TVertex.z = -Size.z * 0.5f;
		MeshSubset->Vertices.push_back(TVertex);
		Mesh->AABB.AddVertex(TVertex);

		//BOTTOM
		TVertex.x = -Size.x * 0.5f;
		TVertex.y = -Size.y * 0.5f;
		TVertex.z = -Size.z * 0.5f;
		MeshSubset->Vertices.push_back(TVertex);
		Mesh->AABB.AddVertex(TVertex);

		TVertex.x = -Size.x * 0.5f;
		TVertex.y = -Size.y * 0.5f;
		TVertex.z = Size.z * 0.5f;
		MeshSubset->Vertices.push_back(TVertex);
		Mesh->AABB.AddVertex(TVertex);

		TVertex.x = Size.x * 0.5f;
		TVertex.y = -Size.y * 0.5f;
		TVertex.z = Size.z * 0.5f;
		MeshSubset->Vertices.push_back(TVertex);
		Mesh->AABB.AddVertex(TVertex);

		TVertex.x = Size.x * 0.5f;
		TVertex.y = -Size.y * 0.5f;
		TVertex.z = -Size.z * 0.5f;
		MeshSubset->Vertices.push_back(TVertex);
		Mesh->AABB.AddVertex(TVertex);

        //Left
		MeshSubset->Faces.push_back(CFace(5, 4, 0, CTexCoord(0.0f, 0.0f), CTexCoord(1.0f, 0.0f), CTexCoord(1.0f, 1.0f)));
		MeshSubset->Faces.push_back(CFace(0, 1, 5, CTexCoord(1.0f, 1.0f), CTexCoord(0.0f, 1.0f), CTexCoord(0.0f, 0.0f)));

        //Back
		MeshSubset->Faces.push_back(CFace(6, 5, 1, CTexCoord(0.0f, 0.0f), CTexCoord(1.0f, 0.0f), CTexCoord(1.0f, 1.0f)));
		MeshSubset->Faces.push_back(CFace(1, 2, 6, CTexCoord(1.0f, 1.0f), CTexCoord(0.0f, 1.0f), CTexCoord(0.0f, 0.0f)));

        //Right
		MeshSubset->Faces.push_back(CFace(7, 6, 2, CTexCoord(0.0f, 0.0f), CTexCoord(1.0f, 0.0f), CTexCoord(1.0f, 1.0f)));
		MeshSubset->Faces.push_back(CFace(2, 3, 7, CTexCoord(1.0f, 1.0f), CTexCoord(0.0f, 1.0f), CTexCoord(0.0f, 0.0f)));

        //Front
		MeshSubset->Faces.push_back(CFace(4, 7, 3, CTexCoord(0.0f, 0.0f), CTexCoord(1.0f, 0.0f), CTexCoord(1.0f, 1.0f)));
		MeshSubset->Faces.push_back(CFace(3, 0, 4, CTexCoord(1.0f, 1.0f), CTexCoord(0.0f, 1.0f), CTexCoord(0.0f, 0.0f)));

		//Top
		MeshSubset->Faces.push_back(CFace(8, 9, 11, CTexCoord(0.0f, 1.0f), CTexCoord(0.0f, 0.0f), CTexCoord(1.0f, 1.0f)));
		MeshSubset->Faces.push_back(CFace(11, 9, 10, CTexCoord(1.0f, 1.0f), CTexCoord(0.0f, 0.0f), CTexCoord(1.0f, 0.0f)));

        //Bottom
		MeshSubset->Faces.push_back(CFace(15, 13, 12, CTexCoord(1.0f, 1.0f), CTexCoord(1.0f, 0.0f), CTexCoord(0.0f, 0.0f)));
		MeshSubset->Faces.push_back(CFace(14, 13, 15, CTexCoord(0.0f, 0.0f), CTexCoord(0.0f, 1.0f), CTexCoord(1.0f, 1.0f)));
	}

	void CBrush::CreateCylinderGeometry(CVector3 Dimentions)
	{
		//create mesh here.
		//create along the y-axis, then rotate it.
		//hz is the radius.
		//hy is both the height,and the number of segments.
		//hx is the number of times around.

		//center vertices for the caps.
		//Vertex * tv = createVertex( 0, hy / 2, 0 );
		//Vertex *bv = createVertex( 0, -hy / 2, 0 );

		float HeightSegments = 2.0f * 3.142f / Dimentions.y;

		//std::vector < VertexList > segs;


		//segs.reserve( ( int ) hx + 1 );
		//create the vertices
		for ( float i = 0; i < Dimentions.x; i++ )
		{
			float Radius = HeightSegments * i;
			//VertexList vlist;
			//vlist.reserve( ( int ) hy );

			for ( float j = -Dimentions.y / 2; j <= Dimentions.y / 2; j++ )
			{
				//rotate about the y axis;
				float x = cos( Radius ) * Dimentions.z;
				float z = sin( Radius ) * Dimentions.z;
				//Vertex *v1 = createVertex( x, j, z );
				//vlist.push_back( v1 );

				MeshSubset->Vertices.push_back(CVector3(x, j, z));
				Mesh->AABB.AddVertex(CVector3(x, j, z));
			}

			//segs.push_back( vlist );
		}

		//segs.push_back( segs[ 0 ] );



		for ( int i = 0; i < Dimentions.x; i++ )
		{
			//x + x * y

			//createFace( bv->getParentIndex(),
						//segs[ i ][ 0 ] ->getParentIndex(),
						//segs[ i + 1 ][ 0 ] ->getParentIndex() );

			//MeshSubset->Faces.push_back(CFace(0, i * i, i + 1 * i));

			for ( int j = 0; j < Dimentions.y; j++ )
			{
				//createFace( segs[ i + 1 ][ j ] ->getParentIndex(),
							//segs[ i ][ j ] ->getParentIndex(),
							//segs[ i ][ j + 1 ] ->getParentIndex(),
							//segs[ i + 1 ][ j + 1 ] ->getParentIndex() );

				MeshSubset->Faces.push_back(CFace(i * i + j, i * i + j + 1, i + 1 * i + j + 1));
				MeshSubset->Faces.push_back(CFace(i + 1 * i + j + 1, i + 1 * i + j, i * i + j));
			}

			//createFace( tv->getParentIndex(),
						//segs[ i + 1 ][ ( int ) hy ] ->getParentIndex(),
						//segs[ i ][ ( int ) hy ] ->getParentIndex() );

			//MeshSubset->Faces.push_back(CFace(0, i + 1 * i + j + Dimentions.y, i * i + j + Dimentions.y));
		}


		//normalize();
	}

	void CBrush::CreateSphereGeometry(CVector3 Size)
	{
		int Precision = 50;

		for(int i = 0; i < Precision / 2; i++)
		{
			float Theta1 = i * Maths::TWO_PI / (float)Precision - Maths::HALF_PI;
			float Theta2 = (i + 1) * Maths::TWO_PI / (float)Precision - Maths::HALF_PI;

			for(int j = 0; j < Precision; j += 2)
			{
				float Theta3 = j * Maths::TWO_PI / (float)Precision;
				float Theta4 = (j + 1) * Maths::TWO_PI / (float)Precision;

				CVector3 Vertex1(cos(Theta2) * cos(Theta3), sin(Theta2), cos(Theta2) * sin(Theta3));
				CVector3 Vertex2(cos(Theta1) * cos(Theta3), sin(Theta1), cos(Theta1) * sin(Theta3));

				CVector3 Vertex3(cos(Theta2) * cos(Theta4), sin(Theta2), cos(Theta2) * sin(Theta4));
				CVector3 Vertex4(cos(Theta1) * cos(Theta4), sin(Theta1), cos(Theta1) * sin(Theta4));

				MeshSubset->Vertices.push_back(Vertex1);
				MeshSubset->Vertices.push_back(Vertex2);
				MeshSubset->Vertices.push_back(Vertex3);
				MeshSubset->Vertices.push_back(Vertex4);

				MeshSubset->Faces.push_back(CFace(j,		j + 1,	j + 2, CTexCoord(0.0f, 0.0f), CTexCoord(1.0f, 0.0f), CTexCoord(1.0f, 1.0f)));
				MeshSubset->Faces.push_back(CFace(j + 1,	j + 3,	j + 2, CTexCoord(0.0f, 0.0f), CTexCoord(1.0f, 0.0f), CTexCoord(1.0f, 1.0f)));
			}
		}

		Scale(Size);
	}

} //Namespace

