#include "SYM_Sprite.h"
#include "SYM_Engine.h"

namespace sym
{
    CSpriteManager::CSpriteManager()
    {
		
    }

    CSpriteManager::~CSpriteManager()
    {
        for(std::map<std::string, CSprite*>::iterator s = Sprites.begin(); s != Sprites.end(); s++)
		{
		    delete (*s).second;
		}
    }

	bool CSpriteManager::LoadSprite(std::string Name, std::string Filename)
    {
        //Check if sprite already exists
        if(Sprites.find(Name) != Sprites.end())
		{
            Engine->Logger.AddEntry("Error: Sprite with name " + Name + " already exists");
		}
		else
		{
            //Load sprite
            CSprite *TempSprite = new CSprite;
            if (!TempSprite->Load(Filename))
            {
                Engine->Logger.AddEntry("Error: Could not load sprite " + Filename);
                return false;
            }

            //Add to map
            Sprites[Name] = TempSprite;
		}

		return true;
    }

    CSprite *CSpriteManager::GetSprite(std::string Name)
    {
        if(Sprites.find(Name) != Sprites.end())
		{
            return Sprites[Name];
		}
		else
		{
		    Engine->Logger.AddEntry("Error: Requested sprite '" + Name + "' doesn't exist");
		    return 0;
		}
    }

    CSprite::CSprite()
    {
        Scale = CVector3(1.0f, 1.0f, 1.0f);
        Texture = 0;
        Transparency = 1.0f;
		Tint = CColour(255, 255, 255);
		DoubleSided = false;
		DepthTest = true;
    }

    //CSprite::~CSprite()
    //{
        //if(Texture) delete Texture;
    //}

	void CSprite::SetDepthTest(bool Depth)
	{
		DepthTest = Depth;
	}

    bool CSprite::Load(std::string Filename)
    {
        //Load texture from resource manager
        Texture = Engine->ResourceManager.Textures.Get(Filename);

        //Check it loaded
        if (!Texture) return false;

        //Get dimentions
        Size.x = (float)Texture->Width;
        Size.y = (float)Texture->Height;

        return true;
    }

	bool CSprite::Load(CTexture *Tex)
	{
		//Set texture
        Texture = Tex;

        //Check it loaded
        if (!Texture) return false;

        //Get dimentions
        Size.x = (float)Texture->Width;
        Size.y = (float)Texture->Height;

        return true;
	}

    void CSprite::Render()
    {
        glPushMatrix();

        //Bind texture
        glEnable(GL_TEXTURE_2D);
        if(Texture) glBindTexture(GL_TEXTURE_2D, Texture->Texture);

        //Set blend colour
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(Tint.r / 255.0f, Tint.g / 255.0f, Tint.b / 255.0f, Transparency);

		//Transform
		sym::CMatrix4 Matrix = Rotation.ToMatrix();
		Matrix.SetTranslation(Position);
		glMultMatrixf(Matrix.GetAsFloatArray());

		//Scale
        glScalef(Scale.x, Scale.y, Scale.z);

		//Set depth
		if(!DepthTest) glDisable(GL_DEPTH_TEST);

		//Draw
        glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f); glVertex2f(-Size.x * 0.5f, -Size.y * 0.5f);
            glTexCoord2f(1.0f, 0.0f); glVertex2f(Size.x * 0.5f, -Size.y * 0.5f);
            glTexCoord2f(1.0f, 1.0f); glVertex2f(Size.x * 0.5f, Size.y * 0.5f);
            glTexCoord2f(0.0f, 1.0f); glVertex2f(-Size.x* 0.5f, Size.y * 0.5f);

			if(DoubleSided)
			{
				glTexCoord2f(0.0f, 1.0f); glVertex2f(-Size.x* 0.5f, Size.y * 0.5f);
				glTexCoord2f(1.0f, 1.0f); glVertex2f(Size.x * 0.5f, Size.y * 0.5f);
				glTexCoord2f(1.0f, 0.0f); glVertex2f(Size.x * 0.5f, -Size.y * 0.5f);
				glTexCoord2f(0.0f, 0.0f); glVertex2f(-Size.x * 0.5f, -Size.y * 0.5f);
			}
        glEnd();

		if(!DepthTest) glEnable(GL_DEPTH_TEST);

        //Finish
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);

        glPopMatrix();
    }

    void CSprite::SetPosition(CVector3 Pos)
    {
        Position = Pos;
		UpdateBoundingBox();
    }

    void CSprite::SetRotation(CQuaternion Rot)
    {
        Rotation = Rot;
    }

    void CSprite::SetScale(CVector3 Scl)
    {
        Scale = Scl;
		UpdateBoundingBox();
    }

    void CSprite::SetTransparency(float Trans)
    {
        Transparency = Trans;
    }

	void CSprite::UpdateBoundingBox()
	{
		BoundingBox.top = Position.y + (Size.y * 0.5f);
		BoundingBox.left = Position.x - (Size.x * 0.5f);
		BoundingBox.right = Position.x + (Size.x * 0.5f);
		BoundingBox.bottom = Position.y - (Size.y * 0.5f);
	}

	bool CSprite::CheckCollision(CVector3 Point)
	{
		if ((Point.x >= BoundingBox.left) && 
			(Point.x <= BoundingBox.right) && 
			(Point.y <= BoundingBox.top) && 
			(Point.y >= BoundingBox.bottom)) return true;

		return false;
	}

	bool CSprite::CheckCollision(CSprite *Sprite)
	{
		if (BoundingBox.bottom > Sprite->GetBoundingBox().top) return false;
		if (BoundingBox.top < Sprite->GetBoundingBox().bottom) return false;

		if (BoundingBox.right < Sprite->GetBoundingBox().left) return false;
		if (BoundingBox.left > Sprite->GetBoundingBox().right) return false;

		return true;
	}

} //Namespace
