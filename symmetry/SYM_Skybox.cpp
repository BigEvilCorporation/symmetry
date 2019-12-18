#include "SYM_Skybox.h"
#include "SYM_Engine.h"

namespace sym
{
	CSkybox::CSkybox()
	{
		//Set dimentions
		xpos = 0;
		ypos = 0;
		zpos = 0;
		skywidth = 10;
		skyheight = 10;
		skylength = 10;

		//Center the Skybox around the given x, y, z position
		xpos = xpos - skywidth  / 2;
		ypos = ypos - skyheight / 2;
		zpos = zpos - skylength / 2;

		//Texture offset to remove seam
		Offset = 0.0005f;

		IsLoaded = false;

		for(int i = 0; i < 6; i++)
		{
			TextureNames[i] = "";
			TexNames[i] = "";
			SkyTexture[i] = 0;
		}
	}

	bool CSkybox::LoadSkyTextures(std::map<int, std::string> Textures)
	{
		TextureNames = Textures;

		for(std::map<int, std::string> ::iterator Texture = TextureNames.begin(); Texture != TextureNames.end(); Texture++)
		{
			if((*Texture).first == SKY_NORTH)
			{
				SkyTexture[0] = Engine->ResourceManager.Textures.Get((*Texture).second);
				if(!SkyTexture[0]) return false;
			}
			
			if((*Texture).first == SKY_SOUTH)
			{
				SkyTexture[1] = Engine->ResourceManager.Textures.Get((*Texture).second);
				if(!SkyTexture[1]) return false;
			}

			if((*Texture).first == SKY_WEST)
			{
				SkyTexture[2] = Engine->ResourceManager.Textures.Get((*Texture).second);
				if(!SkyTexture[2]) return false;
			}
			
			if((*Texture).first == SKY_EAST)
			{
				SkyTexture[3] = Engine->ResourceManager.Textures.Get((*Texture).second);
				if(!SkyTexture[3]) return false;
			}

			if((*Texture).first == SKY_TOP)
			{
				SkyTexture[4] = Engine->ResourceManager.Textures.Get((*Texture).second);
				if(!SkyTexture[4]) return false;
			}

			if((*Texture).first == SKY_BOTTOM)
			{
				SkyTexture[5] = Engine->ResourceManager.Textures.Get((*Texture).second);
				if(!SkyTexture[5]) return false;
			}
		}

		IsLoaded = true;
		return true;
	}

	bool CSkybox::LoadSkyTextures(std::string Front, std::string Back, std::string Left, std::string Right, std::string Up, std::string Down)
	{
		//Load skybox textures
		SkyTexture[0] = Engine->ResourceManager.Textures.Get(Front);
		if(!SkyTexture[0]) return false;
		TextureNames[SKY_NORTH] = Front;

		SkyTexture[1] = Engine->ResourceManager.Textures.Get(Back);
		if(!SkyTexture[1]) return false;
		TextureNames[SKY_SOUTH] = Back;

		SkyTexture[2] = Engine->ResourceManager.Textures.Get(Left);
		if(!SkyTexture[2]) return false;
		TextureNames[SKY_WEST] = Left;

		SkyTexture[3] = Engine->ResourceManager.Textures.Get(Right);
		if(!SkyTexture[3]) return false;
		TextureNames[SKY_EAST] = Right;

		SkyTexture[4] = Engine->ResourceManager.Textures.Get(Up);
		if(!SkyTexture[4]) return false;
		TextureNames[SKY_TOP] = Up;

		SkyTexture[5] = Engine->ResourceManager.Textures.Get(Down);
		if(!SkyTexture[5]) return false;
		TextureNames[SKY_BOTTOM] = Down;

		IsLoaded = true;
		return true;
	}

	bool CSkybox::LoadSkyTexture(int Face, std::string Filename)
	{
		SkyTexture[Face] = Engine->ResourceManager.Textures.Get(Filename);
		if(!SkyTexture[Face]) return false;
		TextureNames[Face] = Filename;

		return true;
	}

	std::string CSkybox::GetTextureFilename(int Face)
	{
		return TexNames[Face];
	}

	void CSkybox::Render()
	{
		glColor3f(1.0f, 1.0f, 1.0f);
		glEnable(GL_TEXTURE_2D);

		//Set texture matrix mode
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
		//glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
		//glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
		glScalef(-1.0f, 1.0f, 1.0f);

		//Back to modelview mode
		glMatrixMode(GL_MODELVIEW);

		// Draw Front side
		if(SkyTexture[0])
		{
			glBindTexture(GL_TEXTURE_2D, SkyTexture[0]->Texture);
			glBegin(GL_QUADS);
			glTexCoord2f(1.0f - Offset, 0.0f + Offset); glVertex3f(xpos,			ypos,			zpos+skylength);
			glTexCoord2f(1.0f - Offset, 1.0f - Offset); glVertex3f(xpos,			ypos+skyheight, zpos+skylength);
			glTexCoord2f(0.0f + Offset, 1.0f - Offset); glVertex3f(xpos+skywidth, ypos+skyheight, zpos+skylength);
			glTexCoord2f(0.0f + Offset, 0.0f + Offset); glVertex3f(xpos+skywidth, ypos,			zpos+skylength);
			glEnd();
		}

		// Draw Back side
		if(SkyTexture[1])
		{
			glBindTexture(GL_TEXTURE_2D, SkyTexture[1]->Texture);
			glBegin(GL_QUADS);
			glTexCoord2f(1.0f - Offset, 0.0f + Offset); glVertex3f(xpos+skywidth, ypos,			zpos);
			glTexCoord2f(1.0f - Offset, 1.0f - Offset); glVertex3f(xpos+skywidth, ypos+skyheight, zpos);
			glTexCoord2f(0.0f + Offset, 1.0f - Offset); glVertex3f(xpos,			ypos+skyheight,	zpos);
			glTexCoord2f(0.0f + Offset, 0.0f + Offset); glVertex3f(xpos,			ypos,			zpos);
			glEnd();
		}

		// Draw Left side
		if(SkyTexture[2])
		{
			glBindTexture(GL_TEXTURE_2D, SkyTexture[2]->Texture);
			glBegin(GL_QUADS);
			glTexCoord2f(1.0f - Offset, 1.0f - Offset); glVertex3f(xpos,		  ypos+skyheight,	zpos);
			glTexCoord2f(0.0f + Offset, 1.0f - Offset); glVertex3f(xpos,		  ypos+skyheight,	zpos+skylength);
			glTexCoord2f(0.0f + Offset, 0.0f + Offset); glVertex3f(xpos,		  ypos,				zpos+skylength);
			glTexCoord2f(1.0f - Offset, 0.0f + Offset); glVertex3f(xpos,		  ypos,				zpos);
			glEnd();
		}

		// Draw Right side
		if(SkyTexture[3])
		{
			glBindTexture(GL_TEXTURE_2D, SkyTexture[3]->Texture);
			glBegin(GL_QUADS);
			glTexCoord2f(0.0f + Offset, 0.0f + Offset); glVertex3f(xpos+skywidth, ypos,			zpos);
			glTexCoord2f(1.0f - Offset, 0.0f + Offset); glVertex3f(xpos+skywidth, ypos,			zpos+skylength);
			glTexCoord2f(1.0f - Offset, 1.0f - Offset); glVertex3f(xpos+skywidth, ypos+skyheight,	zpos+skylength);
			glTexCoord2f(0.0f + Offset, 1.0f - Offset); glVertex3f(xpos+skywidth, ypos+skyheight,	zpos);
			glEnd();
		}

		// Draw Up side
		if(SkyTexture[4])
		{
			glBindTexture(GL_TEXTURE_2D, SkyTexture[4]->Texture);
			glBegin(GL_QUADS);
			glTexCoord2f(1.0f - Offset, 0.0f + Offset); glVertex3f(xpos+skywidth, ypos+skyheight, zpos);
			glTexCoord2f(1.0f - Offset, 1.0f - Offset); glVertex3f(xpos+skywidth, ypos+skyheight, zpos+skylength);
			glTexCoord2f(0.0f + Offset, 1.0f - Offset); glVertex3f(xpos,			ypos+skyheight,	zpos+skylength);
			glTexCoord2f(0.0f + Offset, 0.0f + Offset); glVertex3f(xpos,			ypos+skyheight,	zpos);
			glEnd();
		}

		// Draw Down side
		if(SkyTexture[5])
		{
			glBindTexture(GL_TEXTURE_2D, SkyTexture[5]->Texture);
			glBegin(GL_QUADS);
			glTexCoord2f(0.0f + Offset, 1.0f - Offset); glVertex3f(xpos,			ypos,		zpos);
			glTexCoord2f(0.0f + Offset, 0.0f + Offset); glVertex3f(xpos,			ypos,		zpos+skylength);
			glTexCoord2f(1.0f - Offset, 0.0f + Offset); glVertex3f(xpos+skywidth, ypos,		zpos+skylength);
			glTexCoord2f(1.0f - Offset, 1.0f - Offset); glVertex3f(xpos+skywidth, ypos,		zpos);
			glEnd();
		}

		//Reset texture matrix
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
		glScalef(-1.0f, 1.0f, 1.0f);
		glMatrixMode(GL_MODELVIEW);

		glDisable(GL_TEXTURE_2D);
	}

} //Namespace
