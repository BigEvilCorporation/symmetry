#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include <vector>

#include "SYM_AABB.h"

namespace sym
{
    CBoundingBox::CBoundingBox()
	{
        //A very large float
		const float LargeFloat = 1e37f;

		//Fill the MIN with the large float
		Min.x = Min.y = Min.z = LargeFloat;

		//Fill the MAX with the opposite
		Max.x = Max.y = Max.z = -LargeFloat;
	}

	CBoundingBox::CBoundingBox(CVector3 Mn, CVector3 Mx)
	{
	    Min = Mn;
	    Max = Mx;

	    //Set dimentions
		Size = Max - Min;
		Centre = Min + (Size * 0.5f);
	}

	void CBoundingBox::AddVertex(CVector3 Vertex)
	{
		//See if the vertex is smaller than the MIN
		if (Vertex.x < Min.x) Min.x = Vertex.x;
		if (Vertex.y < Min.y) Min.y = Vertex.y;
		if (Vertex.z < Min.z) Min.z = Vertex.z;

		//See if the vertex is larger than the MAX
		if (Vertex.x > Max.x) Max.x = Vertex.x;
		if (Vertex.y > Max.y) Max.y = Vertex.y;
		if (Vertex.z > Max.z) Max.z = Vertex.z;

		//Set dimentions
		Size = Max - Min;
		Centre = Min + (Size * 0.5f);
	}

	void CBoundingBox::Merge(CBoundingBox *AABB)
	{
	    //Add the new AABB's vertices
	    AddVertex(AABB->Min);
	    AddVertex(AABB->Max);

	    //Set dimentions
		Size = Max - Min;
		Centre = Min + (Size * 0.5f);
	}

	void CBoundingBox::MakeCube()
	{
	    //Get side lengths
        float LengthX = CVector3(1.0f, 0.0f, 0.0f) * (Size);
        float LengthY = CVector3(0.0f, 1.0f, 0.0f) * (Size);
        float LengthZ = CVector3(0.0f, 0.0f, 1.0f) * (Size);

        //Find longest side
        float Longest = LengthX;
        if (LengthY > Longest) Longest = LengthY;
        if (LengthZ > Longest) Longest = LengthZ;

        //Set size
        Size.x = Longest;
        Size.y = Longest;
        Size.z = Longest;

        Max.x = Min.x + Longest;
        Max.y = Min.y + Longest;
        Max.z = Min.z + Longest;

        Max = Min + Size;

		Centre = Min + (Size * 0.5f);
	}

	bool CBoundingBox::Contains(CVector3 Vector)
	{
	    if( Vector.x >= Min.x && Vector.x <= Max.x &&
            Vector.y >= Min.y && Vector.y <= Max.y &&
            Vector.z >= Min.z && Vector.z <= Max.z ) return true;

        return false;
	}

	bool CBoundingBox::Contains(CBoundingBox *AABB)
	{
	    //If the second AABB's min is GREATER OR EQUAL TO than this min, AND the second AABB's max is LESS THAN OR EQUAL TO this max
	    if( AABB->Min.x >= Min.x && AABB->Max.x <= Max.x &&
            AABB->Min.y >= Min.y && AABB->Max.y <= Max.y &&
            AABB->Min.z >= Min.z && AABB->Max.z <= Max.z ) return true;

	    return false;
	}

	void CBoundingBox::Render()
	{
        glPushMatrix();

        glDisable(GL_LIGHTING);
        glDisable(GL_COLOR_MATERIAL);

        glColor3f(1.0f, 0.0f, 0.0f);

        CVector3 SizeX = CVector3(1, 0, 0).Multiply(Size);
        CVector3 SizeY = CVector3(0, 1, 0).Multiply(Size);
        CVector3 SizeZ = CVector3(0, 0, 1).Multiply(Size);

		std::vector<CVector3> Verts;

        //Top
        Verts.push_back(Min);
        Verts.push_back((Min + SizeZ));
        Verts.push_back((Min + SizeX + SizeZ));
        Verts.push_back((Min + SizeX));

        //Bottom
        Verts.push_back((Max - SizeX - SizeZ));
        Verts.push_back((Max - SizeX));
        Verts.push_back(Max);
        Verts.push_back((Max - SizeZ));

        glBegin(GL_LINES);

            glVertex3f(Verts[0].x, Verts[0].y, Verts[0].z);
            glVertex3f(Verts[1].x, Verts[1].y, Verts[1].z);
            glVertex3f(Verts[2].x, Verts[2].y, Verts[2].z);
            glVertex3f(Verts[3].x, Verts[3].y, Verts[3].z);

            glVertex3f(Verts[4].x, Verts[4].y, Verts[4].z);
            glVertex3f(Verts[5].x, Verts[5].y, Verts[5].z);
            glVertex3f(Verts[6].x, Verts[6].y, Verts[6].z);
            glVertex3f(Verts[7].x, Verts[7].y, Verts[7].z);

            glVertex3f(Verts[0].x, Verts[0].y, Verts[0].z);
            glVertex3f(Verts[1].x, Verts[1].y, Verts[1].z);
            glVertex3f(Verts[5].x, Verts[5].y, Verts[5].z);
            glVertex3f(Verts[4].x, Verts[4].y, Verts[4].z);

            glVertex3f(Verts[3].x, Verts[3].y, Verts[3].z);
            glVertex3f(Verts[2].x, Verts[2].y, Verts[2].z);
            glVertex3f(Verts[6].x, Verts[6].y, Verts[6].z);
            glVertex3f(Verts[7].x, Verts[7].y, Verts[7].z);

            glVertex3f(Verts[0].x, Verts[0].y, Verts[0].z);
            glVertex3f(Verts[3].x, Verts[3].y, Verts[3].z);
            glVertex3f(Verts[7].x, Verts[7].y, Verts[7].z);
            glVertex3f(Verts[4].x, Verts[4].y, Verts[4].z);

            glVertex3f(Verts[1].x, Verts[1].y, Verts[1].z);
            glVertex3f(Verts[2].x, Verts[2].y, Verts[2].z);
            glVertex3f(Verts[6].x, Verts[6].y, Verts[6].z);
            glVertex3f(Verts[5].x, Verts[5].y, Verts[5].z);

            glVertex3f(Verts[0].x, Verts[0].y, Verts[0].z);
            glVertex3f(Verts[4].x, Verts[4].y, Verts[4].z);

            glVertex3f(Verts[1].x, Verts[1].y, Verts[1].z);
            glVertex3f(Verts[5].x, Verts[5].y, Verts[5].z);

            glVertex3f(Verts[2].x, Verts[2].y, Verts[2].z);
            glVertex3f(Verts[6].x, Verts[6].y, Verts[6].z);

            glVertex3f(Verts[3].x, Verts[3].y, Verts[3].z);
            glVertex3f(Verts[7].x, Verts[7].y, Verts[7].z);

        glEnd();

        glColor3f(1.0f, 1.0f, 1.0f);

        glPopMatrix();
	}

} //Namespace
