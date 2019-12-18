#ifdef WIN32
#include <windows.h>
#endif

#include <stdlib.h>
#include <algorithm>

#include <GL/gl.h>

#include "SYM_Geometry.h"
#include "SYM_Mesh.h"

namespace sym
{
	CGeometry::CGeometry()
	{
	    Name = "None";
		Visible = true;
		CastShadows = true;
		ReceiveShadows = true;
		Collidable = true;
		Physics = false;
		Dimentions = CVector3(1.0f, 1.0f, 1.0f);
		TextureRotation = CVector3(180.0f, 180.0f, 0.0f);
		TextureTile = CVector3(1.0f, 1.0f, 1.0f);
		DoubleSided = false;
		CustomData = 0;

		Mesh = 0;
	}

	CGeometry::~CGeometry()
	{
	    //if(Mesh) delete Mesh;
	}

	void CGeometry::Render()
	{
		//If geometry is visible
		if(Visible)
		{
		    //Enable lighting
		    glEnable(GL_LIGHTING);

			//Push matrix to stack
			glPushMatrix();

            //Apply the matrix
            glMultMatrixf(Matrix.GetAsFloatArray());

			//Set texture matrix mode
			glMatrixMode(GL_TEXTURE);
			glLoadIdentity();
			glRotatef(TextureRotation.x, 1.0f, 0.0f, 0.0f);
			glRotatef(TextureRotation.y, 0.0f, 1.0f, 0.0f);
			glRotatef(TextureRotation.z, 0.0f, 0.0f, 1.0f);
			glTranslatef(TextureOffset.x, TextureOffset.y, TextureOffset.z);
			glScalef(-TextureTile.x, TextureTile.y, TextureTile.z);

			//Back to modelview mode
			glMatrixMode(GL_MODELVIEW);

			if(DoubleSided) glDisable(GL_CULL_FACE);

			//Render mesh
			if(Mesh) Mesh->Render();

			glEnable(GL_CULL_FACE);

			//Reset texture matrix
			glMatrixMode(GL_TEXTURE);
			glLoadIdentity();
			glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
			glScalef(-1.0f, 1.0f, 1.0f);
			glMatrixMode(GL_MODELVIEW);

			//Retrieve matrix
			glPopMatrix();

			//Disable lighting
			glDisable(GL_LIGHTING);
		}
	}

	void CGeometry::RenderWireframe(CColour Colour)
	{
		//Push matrix to stack
		glPushMatrix();

        //Apply the matrix
        glMultMatrixf(Matrix.GetAsFloatArray());

		Mesh->RenderWireframe(Colour);

		//Retrieve matrix
		glPopMatrix();
	}

	void CGeometry::RenderCollision(CColour Colour)
	{
			//Set the colour
			glColor3ub(Colour.r, Colour.g, Colour.b);

			//Render collision faces
			CPhysicsBody::RenderCollision(Colour);
	}

	CVector3 CGeometry::GetDimentions()
	{
	    return Dimentions;
	}

	void CGeometry::Scale(CVector3 Scl)
	{
	    //Resize mesh vertices
		if(Mesh) Mesh->Scale(Scl);

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
		if(PhysicsType != NO_PHYSICS)
		{
			SetGeometry(dynamic_cast<CGeometry*>(this));
			SetNewtonMatrix(GetMatrix());
		}

		//Scale dimentions
		Dimentions *= Scl;
		

		HasMoved();
	}

	bool CGeometry::SetMaterial(std::string Filename)
	{
		if(Mesh) if (!Mesh->SetMaterial(Filename)) return false;

		//Set filename
		MatFilename = Filename;

		return true;
	}

	bool CGeometry::SetMaterial(CMaterial *Mat)
	{
		if(Mesh)
		{
			if(!Mat) return false;
			if (!Mesh->SetMaterial(Mat)) return false;
		}
		else return false;

		//Set filename
		MatFilename = Mat->Name;

		return true;
	}

	void CGeometry::SetMass(float M)
	{
		CPhysicsObject::SetMass(M);
	}

	void CGeometry::SetVelocity(CVector3 Vel)
	{
		CPhysicsObject::SetVelocity(Vel);
	}

	CVector3 CGeometry::GetVelocity()
	{
		return CPhysicsObject::GetVelocity();
	}

	CVector3 CGeometry::GetOmega()
	{
		return CPhysicsObject::GetOmega();
	}

	CBoundingBox CGeometry::GetLocalAABB()
	{
	    return Mesh->AABB;
	}

	void CGeometry::RecalculateAABB()
	{
		Mesh->AABB = CBoundingBox();

		for(int i = 0; i < (int)Mesh->Subsets.size(); i++)
		{
			for(int j = 0; j < (int)Mesh->Subsets[i].Vertices.size(); j++)
			{
				Mesh->AABB.AddVertex(Mesh->Subsets[i].Vertices[j]);
			}
		}
	}

	CBoundingBox CGeometry::GetWorldAABB()
	{
		//New AABB
	    CBoundingBox WorldAABB = Mesh->AABB;

	    //Translate each boundary by position
	    WorldAABB.Min += Position;
	    WorldAABB.Max += Position;

	    //Return
	    return WorldAABB;
	}

	void CGeometry::HasMoved()
    {
        //Notify all octree nodes
        for(int i = 0; i < (int)OctreeNodes.size(); i++)
        {
            OctreeNodes[i]->GeomHasMoved(this);
        }
    }

    void CGeometry::AddOctreeNode(COctreeNode *Node)
    {
        if(find(OctreeNodes.begin(), OctreeNodes.end(), Node) == OctreeNodes.end()) OctreeNodes.push_back(Node);
    }

	void CGeometry::DeleteOctreeNode(COctreeNode *Node)
	{
	    OctreeNodes.erase(find(OctreeNodes.begin(), OctreeNodes.end(), Node));
	}

	void CGeometry::ClearOctreeNodes()
	{
	    OctreeNodes.clear();
	}

	void CGeometry::RemoveFromOctree()
	{
		for(int i = 0; i < (int)OctreeNodes.size(); i++)
        {
            OctreeNodes[i]->RemoveGeometry(this);
        }
	}

} //Namespace
