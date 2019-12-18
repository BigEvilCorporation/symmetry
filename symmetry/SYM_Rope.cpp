#include "SYM_Rope.h"

namespace sym
{
	CRope::CRope()
	{
		StartJoint = 0;
		EndJoint = 0;
	}

	CRope::~CRope()
	{
		DetachStart();
		DetachEnd();

		for(int i = 0; i < (int)Joints.size(); i++)
		{
			if(Joints[i]) NewtonDestroyJoint(Engine->Physics.GetWorld(), Joints[i]);
		}

		for(int i = 0; i < (int)Links.size(); i++)
		{
			if(Links[i]) Links[i]->DestroyNewtonBody();
		}
	}

	void CRope::Create(CVector3 Start, CVector3 End, float Width)
	{
		//Height of rope links
		float Height = Width * 4.0f;

		//Get length between points
		float Length = Start.GetDistance(End);

		//Calculate how many links are needed
		int NumLinks = (int)Length / (int)(Height * 2);

		Create(Start, End, Width, NumLinks);
	}

	void CRope::Create(CVector3 Start, CVector3 End, float Width, int NumLinks)
	{
		//Height of rope links
		float Height = Width * 4.0f;

		//Get length between points
		float Length = Start.GetDistance(End);

		//Set physics defaults
		ConeRestrictionDeg = 360.0f;
		TwistRestrictionDeg = 45.0f;
		JointStiffness = 0.9f;

		//Current and previous links
        CBrush *Link0 = 0;
        CBrush *Link1 = 0;

		//Newton joint
        NewtonJoint* Joint = 0;

		//Get length of joint
		CVector3 SecondLink = Start.Lerp(End, (float)1.0f / NumLinks);
		float JointLength = Start.GetDistance(SecondLink);

		//Current link position
        CVector3 Position = Start;

		//Previous link position
		CVector3 PrevPosition;

		float Mass = 1.0f;
        float Ixx = 0.7f * Mass * (Height * Height + Width * Width) / 12.0f;
        float Iyy = 0.7f * Mass * (Width * Width + Width * Width) / 12.0f;
        float Izz = 0.7f * Mass * (Width * Width + Height * Height) / 12.0f;

		//Create material
		MaterialID = NewtonMaterialCreateGroupID(Engine->Physics.GetWorld());
		NewtonMaterialSetDefaultElasticity(Engine->Physics.GetWorld(), NewtonMaterialGetDefaultGroupID(Engine->Physics.GetWorld()), MaterialID, 0.0f);
		NewtonMaterialSetDefaultSoftness(Engine->Physics.GetWorld(), NewtonMaterialGetDefaultGroupID(Engine->Physics.GetWorld()), MaterialID, 0.1f);
		NewtonMaterialSetDefaultCollidable(Engine->Physics.GetWorld(), MaterialID, MaterialID, 0);

		for (int i = 0; i < NumLinks; i++)
        {
			//Create link
			Link1 = new CBrush("Link", SYM_BOX, PHYSICS_OBJ_CAPSULE, CVector3(Width, Height, Width));

			//Set material ID
			Link1->SetNewtonMaterial(MaterialID);

			//Set position
            Link1->SetPosition(Position);

			//Set rotation (Point bottom towards next link)
			if(i != NumLinks) Link1->LookAt(CVector3(0, -1, 0), Start.Lerp(End, (float)(i+1) / NumLinks), 1.0f);
			else Link1->LookAt(CVector3(0, 1, 0), PrevPosition, 1.0f);

            if(Link0 != 0)
            {
				//Calculate pivot position
				CVector3 Pivot = PrevPosition.Lerp(Position, 0.5f);

				//Get newton bodies from links
                NewtonBody *Body0 = Link0->GetNewtonBody();
                NewtonBody *Body1 = Link1->GetNewtonBody();

				//Create joint
                Joint = NewtonConstraintCreateBall(Engine->Physics.GetWorld(), &Pivot.x, Body1, Body0);

				//Turn off collision between these two joints
				NewtonJointSetCollisionState(Joint, 0);

				//Set linear and angular damping
                CVector3 AngularDamp (0.1f, 0.1f, 0.1f);
				//NewtonBodySetLinearDamping (Body1, 0.4f);
                //NewtonBodySetAngularDamping (Body1, &AngularDamp.x);

				//Set cone restriction
                CVector3 Pin(Link1->GetUp() * (-1.0f));
                NewtonBallSetConeLimits(Joint, &Pin.x, ConeRestrictionDeg  * Maths::PI_DIV_180, TwistRestrictionDeg * Maths::PI_DIV_180);

				//Set joint stiffness
                NewtonJointSetStiffness(Joint, JointStiffness);

				//Turn off autofreeze
				Link1->SetAutoFreeze(false);

                //Set mass and inertia
                NewtonBodySetMassMatrix(Body1, Mass, Ixx, Iyy, Izz);

				Joints.push_back(Joint);
            }

            //Add to links vector
            Links.push_back(Link1);

            Link0 = Link1;

			PrevPosition = Position;
            
			//Interpolate position
			Position = Start.Lerp(End, (float)(i+1) / NumLinks);
        }
	}

	CBrush *CRope::GetStartLink()
	{
		if(!Links.empty())
        {
			return Links.front();
		}

		return 0;
	}

	CBrush *CRope::GetEndLink()
	{
		if(!Links.empty())
        {
			return Links.back();
		}

		return 0;
	}

	int CRope::GetNumLinks()
	{
		return (int)Joints.size();
	}

	float CRope::GetJointForce(int Joint)
	{
		if(Joint > GetNumLinks()) return 0.0f;

		CVector3 Force;
		if(Joints[Joint]) NewtonBallGetJointForce(Joints[Joint], &Force.x);

		return Force.GetLength();
	}

	void CRope::BreakJoint(int Joint)
	{
		if(Joint > GetNumLinks()) return;
		if(Joints[Joint])
		{
			NewtonDestroyJoint(Engine->Physics.GetWorld(), Joints[Joint]);
			Joints[Joint] = 0;
		}
	}

	void CRope::AttachStart(CGeometry *Geom, CVector3 Position)
    {
        //Check there is at least one link
        if(!Links.empty())
        {
            //If geometry exists
            if(Geom)
            {
                NewtonBody *Body0 = Geom->GetNewtonBody();
                NewtonBody *Body1 = Links[0]->GetNewtonBody();
                StartJoint = NewtonConstraintCreateBall(Engine->Physics.GetWorld(), &Position.x, Body1, Body0);
				NewtonJointSetCollisionState(StartJoint, 0);
                
                CVector3 AngularDamp (0.1f, 0.1f, 0.1f);
				//NewtonBodySetLinearDamping (Body0, 0.1f);
                //NewtonBodySetAngularDamping (Body0, &AngularDamp.x);

                CVector3 Pin(Geom->GetUp() * (-1.0f));
                NewtonBallSetConeLimits(StartJoint, &Pin.x, 180 * Maths::PI_DIV_180, 180 * Maths::PI_DIV_180);

                NewtonJointSetStiffness (StartJoint, JointStiffness);
            }
        }

		Start = Position;
    }

    void CRope::AttachEnd(CGeometry *Geom, CVector3 Position)
    {
        //Check there is at least one link
        if(!Links.empty())
        {
            //If geometry exists
            if(Geom)
            {
                NewtonBody *Body0 = Geom->GetNewtonBody();
                NewtonBody *Body1 = Links.back()->GetNewtonBody();
                EndJoint = NewtonConstraintCreateBall(Engine->Physics.GetWorld(), &Position.x, Body0, Body1);
				NewtonJointSetCollisionState(EndJoint, 0);

                CVector3 AngularDamp (0.1f, 0.1f, 0.1f);
				//NewtonBodySetLinearDamping (Body0, 0.1f);
                //NewtonBodySetAngularDamping (Body0, &AngularDamp.x);

                CVector3 Pin(Geom->GetUp() * (-1.0f));
                NewtonBallSetConeLimits(EndJoint, &Pin.x, 180 * Maths::PI_DIV_180, 180 * Maths::PI_DIV_180);

                NewtonJointSetStiffness (EndJoint, JointStiffness);
            }
        }

		End = Position;
    }

	void CRope::DetachStart()
	{
		if(StartJoint) NewtonDestroyJoint(Engine->Physics.GetWorld(), StartJoint);
	}
	
	void CRope::DetachEnd()
	{
		if(EndJoint) NewtonDestroyJoint(Engine->Physics.GetWorld(), EndJoint);
	}

    void CRope::Move(CVector3 Vector)
    {
        for(std::vector<CBrush*>::iterator Link = Links.begin(); Link != Links.end(); Link++)
        {
            (*Link)->SetPosition((*Link)->GetPosition() + Vector);
        }
    }

    void CRope::SetMaterial(CMaterial *Material)
    {
        for(std::vector<CBrush*>::iterator Link = Links.begin(); Link != Links.end(); Link++)
        {
            (*Link)->SetMaterial(Material);
        }
    }

	void CRope::DisableCollision(CGeometry *Geom)
	{
		NewtonMaterialSetDefaultCollidable(Engine->Physics.GetWorld(), Geom->GetNewtonMaterial(), MaterialID, 0);
	}

    CVector3 CRope::GetStartPosition()
    {
        return (*Links.begin())->GetPosition() + CVector3(0, Links.back()->GetLocalAABB().Size.y, 0);
    }

    CVector3 CRope::GetEndPosition()
    {
		return Links.back()->GetPosition() - CVector3(0, Links.back()->GetLocalAABB().Size.y, 0);
    }

	void CRope::Render()
	{
		/*

		//Render normally
		for(int i = 0; i < (int)Links.size(); i++)
		{
			Links[i]->Render();
		}
		
		//Loop through links
		if(Links.size() > 2)
		{
			//Apply material
			if(Links.front()->Mesh->Subsets[0].Material) Links.front()->Mesh->Subsets[0].Material->Apply();

			//Set texture matrix mode
			glMatrixMode(GL_TEXTURE);
			glLoadIdentity();
			glRotatef(Links.front()->TextureRotation.x, 1.0f, 0.0f, 0.0f);
			glRotatef(Links.front()->TextureRotation.y, 0.0f, 1.0f, 0.0f);
			glRotatef(Links.front()->TextureRotation.z, 0.0f, 0.0f, 1.0f);
			glTranslatef(Links.front()->TextureOffset.x, Links.front()->TextureOffset.y, Links.front()->TextureOffset.z);
			glScalef(-Links.front()->TextureTile.x, Links.front()->TextureTile.y, Links.front()->TextureTile.z);

			//Back to modelview mode
			glMatrixMode(GL_MODELVIEW);

			CVector3 Vert1, Vert2, Vert3;

			for(int i = 0; i < (int)Joints.size(); i++)
			{
				if(Joints[i])
				{
					CMeshSubset *CurrentLink = &Links[i]->Mesh->Subsets[0];
					CMeshSubset *NextLink = &Links[i + 1]->Mesh->Subsets[0];
					CMatrix4 CurrMatrix = Links[i]->GetMatrix();
					CMatrix4 NextMatrix = Links[i + 1]->GetMatrix();

					glBegin(GL_TRIANGLES);
					
						Vert1 = CurrMatrix.TransformVector(CurrentLink->Vertices[0]);
						Vert2 = NextMatrix.TransformVector(NextLink->Vertices[4]);
						Vert3 = NextMatrix.TransformVector(NextLink->Vertices[5]);
						glTexCoord2f(1.0f, 0.0f);	glNormal3f(0.0f, -1.0f, 0.0f);	glVertex3fv(&Vert1.x);
						glTexCoord2f(1.0f, 1.0f);	glNormal3f(0.0f, -1.0f, 0.0f);	glVertex3fv(&Vert2.x);
						glTexCoord2f(0.0f, 1.0f);	glNormal3f(0.0f, -1.0f, 0.0f);	glVertex3fv(&Vert3.x);

						Vert1 = CurrMatrix.TransformVector(CurrentLink->Vertices[0]);
						Vert2 = NextMatrix.TransformVector(NextLink->Vertices[5]);
						Vert3 = CurrMatrix.TransformVector(CurrentLink->Vertices[1]);
						glTexCoord2f(1.0f, 0.0f);	glNormal3f(0.0f, -1.0f, 0.0f);	glVertex3fv(&Vert1.x);
						glTexCoord2f(0.0f, 1.0f);	glNormal3f(0.0f, -1.0f, 0.0f);	glVertex3fv(&Vert2.x);
						glTexCoord2f(0.0f, 0.0f);	glNormal3f(0.0f, -1.0f, 0.0f);	glVertex3fv(&Vert3.x);

						Vert1 = CurrMatrix.TransformVector(CurrentLink->Vertices[1]);
						Vert2 = NextMatrix.TransformVector(NextLink->Vertices[5]);
						Vert3 = NextMatrix.TransformVector(NextLink->Vertices[6]);
						glTexCoord2f(1.0f, 0.0f);	glNormal3f(0.0f, 0.0f, 1.0f);	glVertex3fv(&Vert1.x);
						glTexCoord2f(1.0f, 1.0f);	glNormal3f(0.0f, 0.0f, 1.0f);	glVertex3fv(&Vert2.x);
						glTexCoord2f(0.0f, 1.0f);	glNormal3f(0.0f, 0.0f, 1.0f);	glVertex3fv(&Vert3.x);

						Vert1 = CurrMatrix.TransformVector(CurrentLink->Vertices[1]);
						Vert2 = NextMatrix.TransformVector(NextLink->Vertices[6]);
						Vert3 = CurrMatrix.TransformVector(CurrentLink->Vertices[2]);
						glTexCoord2f(1.0f, 0.0f);	glNormal3f(0.0f, 0.0f, 1.0f);	glVertex3fv(&Vert1.x);
						glTexCoord2f(0.0f, 1.0f);	glNormal3f(0.0f, 0.0f, 1.0f);	glVertex3fv(&Vert2.x);
						glTexCoord2f(0.0f, 0.0f);	glNormal3f(0.0f, 0.0f, 1.0f);	glVertex3fv(&Vert3.x);

						Vert1 = CurrMatrix.TransformVector(CurrentLink->Vertices[2]);
						Vert2 = NextMatrix.TransformVector(NextLink->Vertices[6]);
						Vert3 = NextMatrix.TransformVector(NextLink->Vertices[7]);
						glTexCoord2f(1.0f, 0.0f);	glNormal3f(-1.0f, 0.0f, 0.0f);	glVertex3fv(&Vert1.x);
						glTexCoord2f(1.0f, 1.0f);	glNormal3f(-1.0f, 0.0f, 0.0f);	glVertex3fv(&Vert2.x);
						glTexCoord2f(0.0f, 1.0f);	glNormal3f(-1.0f, 0.0f, 0.0f);	glVertex3fv(&Vert3.x);

						Vert1 = CurrMatrix.TransformVector(CurrentLink->Vertices[2]);
						Vert2 = NextMatrix.TransformVector(NextLink->Vertices[7]);
						Vert3 = CurrMatrix.TransformVector(CurrentLink->Vertices[3]);
						glTexCoord2f(1.0f, 0.0f);	glNormal3f(-1.0f, 0.0f, 0.0f);	glVertex3fv(&Vert1.x);
						glTexCoord2f(0.0f, 1.0f);	glNormal3f(-1.0f, 0.0f, 0.0f);	glVertex3fv(&Vert2.x);
						glTexCoord2f(0.0f, 0.0f);	glNormal3f(-1.0f, 0.0f, 0.0f);	glVertex3fv(&Vert3.x);

						Vert1 = CurrMatrix.TransformVector(CurrentLink->Vertices[3]);
						Vert2 = NextMatrix.TransformVector(NextLink->Vertices[7]);
						Vert3 = NextMatrix.TransformVector(NextLink->Vertices[4]);
						glTexCoord2f(1.0f, 0.0f);	glNormal3f(0.0f, 0.0f, -1.0f);	glVertex3fv(&Vert1.x);
						glTexCoord2f(1.0f, 1.0f);	glNormal3f(0.0f, 0.0f, -1.0f);	glVertex3fv(&Vert2.x);
						glTexCoord2f(0.0f, 1.0f);	glNormal3f(0.0f, 0.0f, -1.0f);	glVertex3fv(&Vert3.x);

						Vert1 = CurrMatrix.TransformVector(CurrentLink->Vertices[3]);
						Vert2 = NextMatrix.TransformVector(NextLink->Vertices[4]);
						Vert3 = CurrMatrix.TransformVector(CurrentLink->Vertices[0]);
						glTexCoord2f(1.0f, 0.0f);	glNormal3f(0.0f, 0.0f, -1.0f);	glVertex3fv(&Vert1.x);
						glTexCoord2f(0.0f, 1.0f);	glNormal3f(0.0f, 0.0f, -1.0f);	glVertex3fv(&Vert2.x);
						glTexCoord2f(0.0f, 0.0f);	glNormal3f(0.0f, 0.0f, -1.0f);	glVertex3fv(&Vert3.x);

					glEnd();
				}

			}

			//Reset texture matrix
			glMatrixMode(GL_TEXTURE);
			glLoadIdentity();
			glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
			glScalef(-1.0f, 1.0f, 1.0f);
			glMatrixMode(GL_MODELVIEW);
		}
		*/

		glColor3f(1.0f, 0.0f, 0.0f);
		glBegin(GL_LINES);
		glVertex3f(GetStartPosition().x, GetStartPosition().y, GetStartPosition().z);
		glVertex3f(End.x, End.y, End.z);
		glEnd();
	}
}
