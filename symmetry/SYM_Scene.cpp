#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <fstream>

#include "SYM_Scene.h"
#include "SYM_Level.h"
#include "SYM_Engine.h"

static int NameSuffix = 0;

namespace sym
{
	CScene::CScene()
	{
		ActiveCamera = &DefaultCamera;
		PostShader = 0;
		CgTime = 0;
		FBOInitialised = false;
		DrawGrid = false;
		DrawCollision = false;
		DrawDebugInfo = false;
		DrawOctree = false;
		DrawBones = false;
		UseOctree = false;

		AmbientLight = CColour(100, 100, 100);
		Sunlight = CColour(200, 200, 200);
		SunPosition = CVector3(100, 100, 100);
	}

	CScene::~CScene()
	{
	    for(std::map<std::string, CGeometry*> ::iterator g = Geometry.begin(); g != Geometry.end(); g++)
        {
            delete (*g).second;
        }
	}

	void CScene::Init()
	{
		//Initialise depth buffer
		glGenRenderbuffersEXT(1, &DepthBuffer);

		//Bind depth buffer
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, DepthBuffer);

		//Set depth buffer size
		glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, Engine->ScreenWidth, Engine->ScreenHeight);

		//Initialise FBO
		glGenFramebuffersEXT(1, &FBO);

		//Attach depth buffer to FBO
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FBO);
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, DepthBuffer);

		//Create texture
		FBOTexture.Create(Engine->ScreenWidth, Engine->ScreenHeight, GL_RGBA8);

		//Attach FBO texture to FBO
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, FBOTexture.Texture, 0);
	
		if(glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) != GL_FRAMEBUFFER_COMPLETE_EXT)
		{
			FBOInitialised = false;
			Engine->Logger.AddEntry("Error: Could not initialise FBO framebuffer object");
			return;
		}

		//Unbind FBO
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		
		glGenFramebuffersEXT(1, &RTTFBO);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, RTTFBO);
		//glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, DepthBuffer);
		RTTTexture.Create(Engine->ScreenWidth, Engine->ScreenHeight, GL_RGBA8);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, RTTTexture.Texture, 0);

		if(glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) != GL_FRAMEBUFFER_COMPLETE_EXT)
		{
			FBOInitialised = false;
			Engine->Logger.AddEntry("Error: Could not initialise RTT framebuffer object");
			return;
		}

		//Unbind FBO
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

		FBOInitialised = true;

		//Create RTT rendering display list
		RTTDisplayList = glGenLists(1);

		glNewList(RTTDisplayList, GL_COMPILE);
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f,						(float)Engine->ScreenHeight);
			glTexCoord2f(1.0f, 0.0f); glVertex2f((float)Engine->ScreenWidth,(float)Engine->ScreenHeight);
			glTexCoord2f(1.0f, 1.0f); glVertex2f((float)Engine->ScreenWidth,0.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f,						0.0f);
		glEndList();

		if(!CheckGLErrors()) Engine->Logger.AddEntry("Error: OpenGL returned an error during CScene::Initialise()");
	}

	void CScene::Shutdown()
	{
		if(FBOInitialised)
		{
			glDeleteFramebuffersEXT(1, &FBO);
			glDeleteRenderbuffersEXT(1, &DepthBuffer);
		}
	}

	CBoundingBox CScene::GetWorldSize()
	{
	    return Octree.AABB;
	}

	void CScene::ProcessScene()
	{
	    //Build octree
	    Octree.Create(Geometry);

	    //Set physics world size
	    Engine->Physics.SetWorldSize(Octree.AABB);
	}

	void CScene::ClearScene()
	{
		//Clear geometry map
		Geometry.clear();

		//Clear render list
		RenderList.clear();

		//Clear octree
		Octree.Clear();

		//Clear physics bodies
		Engine->Physics.ClearWorld();
	}

	void CScene::Draw()
	{
		//Load identity matrix
		glLoadIdentity();

		//If skybox is loaded and set to render
		if((int)Skyboxes.size() && DrawSky)
		{
		    //Push matrix
		    glPushMatrix();

		    //get camera's inverse matrix
		    CMatrix4 RotMatrix = ActiveCamera->GetMatrix().GetInverse();

		    //Remove translation
		    RotMatrix.set(3, 0, 0);
		    RotMatrix.set(3, 1, 0);
		    RotMatrix.set(3, 2, 0);

		    //Apply matrix
		    glMultMatrixf(RotMatrix.GetAsFloatArray());

			//Disable depth testing
			glDisable(GL_DEPTH_TEST);

			//Draw skybox models
			for(int i = 0; i < (int)Skyboxes.size(); i++)
			{
				Skyboxes[i]->Render();
			}

			//Enable depth testing
			glEnable(GL_DEPTH_TEST);

            //Pop matrix
			glPopMatrix();
		}

		//Set camera matrix
		ActiveCamera->ApplyMatrix();

		//Update camera's frustum
		ActiveCamera->Frustum.Calculate();

		//Call PreRender on all scripts
		Engine->ScriptManager.CallGlobalFunction("OnPreRender");

		//Draw grid, if switched on
		if (DrawGrid) Draw_Grid(0.0, 1.0f, 0.0, 50, 5);

		//Clear render list
		RenderList.clear();

		//Build render list from octree
		if(UseOctree)
		{
			Octree.BuildRenderList(RenderList, &ActiveCamera->Frustum);

			//Render geometry in render list
			for(std::vector<CGeometry*> ::iterator g = RenderList.begin(); g != RenderList.end(); g++)
			{
				if((*g)->Mesh)
				{
					(*g)->Mesh->NextFrame();
					(*g)->Render();
					if (DrawCollision) (*g)->RenderCollision(CColour(255, 0, 0));
					if (DrawBones) (*g)->Mesh->RenderBones(CColour(255, 0, 0));
				}
			}

			//Render octree
			if (DrawOctree) Octree.Render(&ActiveCamera->Frustum);
		}
		else
		{
			//List of transparent objects
			std::vector<CGeometry*> TransparentBatch;

			//Iterate through geometry
			for(std::map<std::string, CGeometry*> ::iterator g = Geometry.begin(); g != Geometry.end(); g++)
			{
				if((*g).second->Mesh)
				{
					bool IsTransparent = false;

					//Loop through submeshes
					for(int i = 0; i < (int)(*g).second->Mesh->Subsets.size(); i++)
					{
						//Check is transparent or has alpha channel
						if((*g).second->Mesh->Subsets[i].Material)
						{
							if((*g).second->Mesh->Subsets[i].Material->Transparency < 1.0f)
							{
								IsTransparent = true;
							}
							
							if((*g).second->Mesh->Subsets[i].Material->Texture)
								if((*g).second->Mesh->Subsets[i].Material->Texture->HasAlphaChannel())
								{
									IsTransparent = true;
								}
						}

					}

					if(IsTransparent)
					{
						TransparentBatch.push_back((*g).second);
					}
					else
					{
						(*g).second->Mesh->NextFrame();
						(*g).second->Render();
						if (DrawCollision) (*g).second->RenderCollision(CColour(255, 0, 0));
						if (DrawBones) (*g).second->Mesh->RenderBones(CColour(255, 0, 0));
					}
				}
			}

			//Draw transparent objects last
			for(int i = 0; i < (int)TransparentBatch.size(); i++)
			{
				if(TransparentBatch[i]->Mesh)
				{
					TransparentBatch[i]->Mesh->NextFrame();
					TransparentBatch[i]->Render();
					if (DrawCollision) TransparentBatch[i]->RenderCollision(CColour(255, 0, 0));
					if (DrawBones) TransparentBatch[i]->Mesh->RenderBones(CColour(255, 0, 0));
				}
			}
		}
	}

	void CScene::Render()
	{
		if(!ActiveCamera) return;

		/*
		//If a post shader has been loaded
		if(PostShader && FBOInitialised)
		{
			//Bind FBO
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FBO);

			//Clear
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//Setup viewport
			glPushAttrib(GL_VIEWPORT_BIT);
			glViewport(0, 0, Engine->ScreenWidth, Engine->ScreenHeight);

			//Render to FBO
			Draw();

			//Stop rendering to FBO
			glPopAttrib();
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

			//Bind RTT FBO
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, RTTFBO);

			//Clear
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//Setup viewport
			glPushAttrib(GL_VIEWPORT_BIT);
			glViewport(0, 0, Engine->ScreenWidth, Engine->ScreenHeight);

			CgTime += (float)Engine->GetDeltaTime() / 1000.0f;

			float BloomTime = (float)cos(CgTime) * 0.5f + 0.5f;
			if(BloomAlpha) cgSetParameter1fv(BloomAlpha, &BloomTime);

			//Get the first render pass
			CGpass Pass = PostShader->GetFirstPass();

			//Loop until all passes have finished
			while(Pass)
			{
				//Set current pass
				PostShader->SetPass(Pass);

				//Draw FBO texture
				glBindTexture(GL_TEXTURE_2D, FBOTexture.Texture);

				glEnable2D();
				glBegin(GL_QUADS);
					glCallList(RTTDisplayList);
				glEnd();
				glDisable2D();

				//Reset current pass
				PostShader->ResetPass(Pass);

				//Get the next pass
				Pass = PostShader->GetNextPass(Pass);
			}

			//Stop rendering to RTT FBO
			glPopAttrib();
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

			//Blend together
			glEnable2D();

			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, FBOTexture.Texture);
			glBegin(GL_QUADS);
				glCallList(RTTDisplayList);
			glEnd();

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			//glBlendFunc(GL_ONE, GL_ONE);

			glBindTexture(GL_TEXTURE_2D, RTTTexture.Texture);
			glBegin(GL_QUADS);
				glCallList(RTTDisplayList);
			glEnd();

			glDisable(GL_TEXTURE_2D);
			glDisable(GL_BLEND);
			glDisable2D();
		}
		else
		{
			*/
			//Draw without shader
			Draw();
		//}

		//Call PostRender on all scripts
		Engine->ScriptManager.CallGlobalFunction("OnPostRender");

		//Enable 2D rendering
		glEnable2D();

		//Call RenderOverlay on all scripts
		Engine->ScriptManager.CallGlobalFunction("OnRenderOverlay");

		//Render MPEG if playing
		if(Engine->MPEGPlayback.IsPlaying())
		{
			//Get next frame
			Engine->MPEGPlayback.ReadFrame();

			//Render frame
			Engine->MPEGPlayback.RenderFrame();
		}

		//Render console if open
		if (Engine->Console.IsOpen) Engine->Console.Render();

		//Disable 2D rendering
		glDisable2D();

		if(!CheckGLErrors()) Engine->Logger.AddEntry("Error: OpenGL returned an error during CScene::Render()");
	}

	void CScene::ForceRender()
	{
		Render();
	}

	CShader *CScene::LoadPostShader(std::string Filename)
	{
		Filename = Engine->ShadersPath + Filename;
		PostShader = Engine->ResourceManager.Shaders.Get(Filename);

		if(!PostShader) Engine->Logger.AddEntry("Error: Could not load scene post shader " + Filename);
		else if(FBOInitialised)
		{
			PostShader->SetTextureParam("SceneTexture", &RTTTexture);
			//BloomAlpha = cgGetEffectParameterBySemantic(PostShader->GetEffect(), "BloomAlpha");

			//if(!BloomAlpha) Engine->Logger.AddEntry("Warning: No BloomAlpha parameter in post shader " + Filename);
		}

		return PostShader;
	}

	CShader *CScene::GetPostShader()
	{
		return PostShader;
	}

	bool CScene::GeometryExists(std::string Name)
	{
		if(Geometry.find(Name) != Geometry.end()) return true;
		else return false;
	}

	std::string CScene::GetUniqueName()
	{
		//Generate name
		std::stringstream AutoName;
		AutoName << "Geometry" << NameSuffix;

		while(GeometryExists(AutoName.str()))
		{
			AutoName.str("");
			AutoName << "Geometry" << ++NameSuffix;
		}

		return AutoName.str();
	}

	CBrush* CScene::AddPlane(std::string Name, int PhysType, CVector3 Dim)
	{
	    if(GeometryExists(Name))
		{
		    Engine->Logger.AddEntry("Warning: Geometry with name '" + Name + "' already exists");
		    Name = GetUniqueName();
			Engine->Logger.AddEntry("Renamed as " + Name);
		}

        //New plane brush
        CBrush *Brush = new CBrush(Name, SYM_PLANE, PhysType, Dim);

        //Add to map
        Geometry[Name] = Brush;

		//Add to octree
		if(UseOctree) Octree.AddGeometry(Brush);

        //Return brush
        return Brush;
    }

	CBrush* CScene::AddBox(std::string Name, int PhysType, CVector3 Dim)
	{
	    if(GeometryExists(Name))
		{
		    Engine->Logger.AddEntry("Warning: Geometry with name '" + Name + "' already exists");
		    Name = GetUniqueName();
			Engine->Logger.AddEntry("Renamed as " + Name);
		}

        //New box brush
        CBrush *Brush = new CBrush(Name, SYM_BOX, PhysType, Dim);

        //Add to map
        Geometry[Name] = Brush;

		//Add to octree
		if(UseOctree) Octree.AddGeometry(Brush);

        //Return brush
        return Brush;
	}

	CBrush* CScene::AddCylinder(std::string Name, int PhysType, CVector3 Dim)
	{
	    if(GeometryExists(Name))
		{
		    Engine->Logger.AddEntry("Warning: Geometry with name '" + Name + "' already exists");
		    Name = GetUniqueName();
			Engine->Logger.AddEntry("Renamed as " + Name);
		}

        //New cylinder brush
        CBrush *Brush = new CBrush(Name, SYM_CYLINDER, PhysType, Dim);

        //Add to map
        Geometry[Name] = Brush;

		//Add to octree
		if(UseOctree) Octree.AddGeometry(Brush);

        //Return brush
        return Brush;
	}

	CBrush* CScene::AddSphere(std::string Name, int PhysType, CVector3 Dim)
	{
	    if(GeometryExists(Name))
		{
		    Engine->Logger.AddEntry("Warning: Geometry with name '" + Name + "' already exists");
		    Name = GetUniqueName();
			Engine->Logger.AddEntry("Renamed as " + Name);
		}

        //New sphere brush
        CBrush *Brush = new CBrush(Name, SYM_SPHERE, PhysType, Dim);

        //Add to map
        Geometry[Name] = Brush;

		//Add to octree
		if(UseOctree) Octree.AddGeometry(Brush);

        //Return brush
        return Brush;
	}

	CModel* CScene::AddModel(std::string Name, std::string Filename, int PhysType)
	{
		if(GeometryExists(Name))
		{
		    Engine->Logger.AddEntry("Warning: Geometry with name '" + Name + "' already exists");
		    Name = GetUniqueName();
			Engine->Logger.AddEntry("Renamed as " + Name);
		}

        //New model
        CModel *Model = new CModel;

        //Load model
        if(!Model->Load(Name, Filename, PhysType)) return 0;

        //Add to map
        Geometry[Name] = Model;

		//Add to octree
		if(UseOctree) Octree.AddGeometry(Model);

		//Return model
		return Model;
	}

	void CScene::AddGeometry(CGeometry *Geom)
	{
		std::string Name = Geom->Name;

		if(GeometryExists(Name))
		{
		    Engine->Logger.AddEntry("Warning: Geometry with name '" + Name + "' already exists");
		    Name = GetUniqueName();
			Engine->Logger.AddEntry("Renamed as " + Name);
		}

		Geometry[Name] = Geom;
		if(UseOctree) Octree.AddGeometry(Geom);
	}

	bool CScene::AddTerrain(std::string Name, std::string Filename)
	{
		//Convert filename to upper case
		//transform(Filename.begin(), Filename.end(), Filename.begin(), toupper);

        if(GeometryExists(Name))
		{
		    Engine->Logger.AddEntry("Warning: Geometry with name '" + Name + "' already exists");
		    Name = GetUniqueName();
			Engine->Logger.AddEntry("Renamed as " + Name);
		}

        //New terrain
        CTerrain *Terrain = new CTerrain;

        //Load heightmap
        if(!Terrain->Load(Name, Filename)) return false;

        //Add to map
        Geometry[Name] = Terrain;

		//Add to octree
		if(UseOctree) Octree.AddGeometry(Terrain);

		return true;
	}

	void CScene::DeleteGeometry(std::string Name)
	{
		//Remove the model from the map
		Geometry.erase(Name);

		//TODO: remove from octree
	}

	void CScene::DeleteGeometry(CGeometry *Geom)
	{
		for(std::map<std::string, CGeometry*>::iterator i = Geometry.begin(); i != Geometry.end();)
		{
			if((*i).second == Geom)
			{
				//Remove from octree
				Geom->RemoveFromOctree();

				i = Geometry.erase(i);
				return;
			}

			i++;
		}
	}

	CGeometry *CScene::GetGeometry(std::string Name)
	{
	    if(Geometry.find(Name) != Geometry.end())
		{
            return Geometry[Name];
		}
		else
		{
		    Engine->Logger.AddEntry("Error: Requested geometry '" + Name + "' doesn't exist");
		    return 0;
		}
	}

	bool CScene::RenameGeometry(CGeometry *Geom, std::string Name)
	{
		if(GetGeometry(Name) == 0)
		{
			std::map<std::string, CGeometry*>::iterator i = Geometry.find(Geom->Name);
			if(i != Geometry.end())
			{
				Geometry.erase(i);
				Geom->Name = Name;
				Geometry[Name] = Geom;
				return true;
			}
			else
			{
				Engine->Logger.AddEntry("Error: Requested geometry '" + Name + "' doesn't exist");
				return false;
			}
		}
		else
		{
			Engine->Logger.AddEntry("Error: Cannot rename geometry, geometry with name '" + Name + "' already exists");
			return false;
		}
	}

	CSkeletalAnimation *CScene::LoadAnimation(std::string Name, std::string Filename)
	{
		if(Animations.find(Name) != Animations.end())
		{
		    Engine->Logger.AddEntry("Error: Animation with name '" + Name + "' already exists");
		    return 0;
		}
		else
		{
            //Get animation from resource manager
			CSkeletalAnimation *Anim = Engine->ResourceManager.Animations.Get(Filename);

			if(Anim)
			{
				Animations[Name] = Anim;

				//Return model
				return Anim;
			}
			else
			{
				return 0;
			}
		}
	}

	CSkeletalAnimation *CScene::GetAnimation(std::string Name)
	{
		if(Animations.find(Name) != Animations.end())
		{
            return Animations[Name];
		}
		else
		{
		    Engine->Logger.AddEntry("Error: Requested animation '" + Name + "' doesn't exist");
		    return 0;
		}
	}

	void CScene::LoadSkyboxTextures(std::string Front, std::string Back, std::string Left, std::string Right, std::string Up, std::string Down)
	{
		/*
		if (!Skybox.LoadSkyTextures(Front, Back, Left, Right, Up, Down))
		{
		    Engine->Logger.AddEntry("Error: Could not load skybox textures");
			Engine->Console.PrintLine("> Error: Could not load skybox textures");
		}
		*/
	}

	void CScene::ShowSkybox(bool Visible)
	{
		DrawSky = Visible;
	}

	void CScene::ShowGrid(bool Visible)
	{
		DrawGrid = Visible;
	}

	void CScene::ShowCollision(bool Visible)
	{
		DrawCollision = Visible;
	}

	void CScene::ShowDebugInfo(bool Visible)
	{
		DrawDebugInfo = Visible;
	}

	void CScene::ShowOctree(bool Visible)
	{
		DrawOctree = Visible;
	}

	void CScene::SetUseOctree(bool UseOct)
	{
		UseOctree = UseOct;
	}

	bool CScene::Write(std::string Filename)
    {
        //Current MAIN section ID and length
        int MainSection_ID = 0;
        int MainSection_Length = 0;

        //Current SUB section ID and length
        int SubSection_ID = 0;
        int SubSection_Length = 0;
        int SubSection_Start = 0;

        //Current CHILD section ID and length
        int ChildSection_ID = 0;
        int ChildSection_Length = 0;
        int ChildSection_Start = 0;

        //Prefix with scenes path from engine config
		Filename = Engine->ScenePath + Filename;

        Engine->Logger.AddEntry("Writing scene to disk: " + Filename);

        int TempInt = 0;
        float TempFloat = 0;

        //Open file for writing
        FILE *File = fopen ( Filename.c_str(), "wb" );

        //Check for errors
        if ( !File )
        {
            Engine->Logger.AddEntry("Error: Could not write scene file " + Filename);
            return false;
        }

        //////////////////////////////////////////////////////////////////
        //ROOT_SECTION
        MainSection_ID = S_ROOT_SECTION;
        MainSection_Length = 6;

        //Write ROOT_SECTION header
        fwrite ( &MainSection_ID, 2, 1, File );

        //Write root section length
        fwrite ( &MainSection_Length, 4, 1, File );

        //////////////////////////////////////////////////////////////////
        //S_GLOBALS
        MainSection_ID = S_GLOBALS;

        ///DEFAULT VALUES
		std::string TerrainFile = "default.sym.terrain";
        std::string SkyboxFile = "default.sym.sky";
        std::string ScriptFile = "default.lua";

        int Amb_r = GetAmbientLight().r;
		int Amb_g = GetAmbientLight().g;
		int Amb_b = GetAmbientLight().b;

        float Gravity = 9.8f;

        //Append '\0' to all strings
        TerrainFile += '\0';
        SkyboxFile += '\0';
        ScriptFile += '\0';


        ///Calculate section length

        ///        HEADER	(6)
        /// + 6 + AMB_LIGHT	(12)
        /// + 6 + TERRAIN	(NumChars)
        /// + 6 + SKY_FILE	(NumChars)
        /// + 6 + SCRIPT	(NumChars)
        /// + 6 + GRAVITY	(4)

        MainSection_Length = 	  36			    //Headers
                                + 12			    //Ambient light
                                + (int)TerrainFile.size()//Terrain file
                                + (int)SkyboxFile.size()	//Skybox
                                + (int)ScriptFile.size()	//Script
                                + 4;			    //Gravity

        //Write header
        fwrite ( &MainSection_ID, 2, 1, File );

        //Write length
        fwrite ( &MainSection_Length, 4, 1, File );

            //////////////////////////////////////////////////////////////////
            //S_AMBIENT_LIGHT
            Engine->Logger.AddEntry("Writing scene ambient light");
            SubSection_ID = S_AMBIENT_LIGHT;
            SubSection_Length = 18;

            //Write header
            fwrite(&SubSection_ID, 2, 1, File);

            //Write length
            fwrite(&SubSection_Length, 4, 1, File);

            //Write R
            fwrite(&Amb_r, 4, 1, File);

            //Write G
            fwrite(&Amb_g, 4, 1, File);

            //Write B
            fwrite(&Amb_b, 4, 1, File);

            //////////////////////////////////////////////////////////////////
            //S_TERRAIN_FILE
            Engine->Logger.AddEntry("Writing scene terrain file");
            SubSection_ID = S_TERRAIN_FILE;
            SubSection_Length = (int)TerrainFile.size() + 6;

            //Write header
            fwrite ( &SubSection_ID, 2, 1, File );

            //Write length
            fwrite ( &SubSection_Length, 4, 1, File );

            //Write object name
            fwrite ( TerrainFile.c_str(), 1, TerrainFile.size(), File );

            //////////////////////////////////////////////////////////////////
            //S_SKY_FILE
            Engine->Logger.AddEntry("Writing scene sky file");
            SubSection_ID = S_SKY_FILE;
            SubSection_Length = (int)SkyboxFile.size() + 6;

            //Write header
            fwrite ( &SubSection_ID, 2, 1, File );

            //Write length
            fwrite ( &SubSection_Length, 4, 1, File );

            //Write object name
            fwrite ( SkyboxFile.c_str(), 1, SkyboxFile.size(), File );

            //////////////////////////////////////////////////////////////////
            //S_SCRIPT_FILE
            Engine->Logger.AddEntry("Writing scene script file");
            SubSection_ID = S_SCRIPT_FILE;
            SubSection_Length = (int)ScriptFile.size() + 6;

            //Write header
            fwrite ( &SubSection_ID, 2, 1, File );

            //Write length
            fwrite ( &SubSection_Length, 4, 1, File );

            //Write object name
            fwrite ( ScriptFile.c_str(), 1, ScriptFile.size(), File );

            //////////////////////////////////////////////////////////////////
            //S_GRAVITY
            Engine->Logger.AddEntry("Writing scene gravity");
            SubSection_ID = S_GRAVITY;
            SubSection_Length = 10;

            //Write header
            fwrite(&SubSection_ID, 2, 1, File);

            //Write length
            fwrite(&SubSection_Length, 4, 1, File);

            //Write R
            fwrite(&Gravity, 4, 1, File);

        //////////////////////////////////////////////////////////////
        //GEOMETRY

        //For all geometry
        for(std::map<std::string, CGeometry*> ::iterator g = Geometry.begin(); g != Geometry.end(); g++)
        {
            //If this geometry is a model
            if((*g).second->Type == SYM_MDL)
            {
                MainSection_ID = S_MODEL;

                Engine->Logger.AddEntry("Writing model " + (*g).second->Name);

                //Append '\0' to names
                std::string ModelName = (*g).second->Name + '\0';
                std::string ModelFilename = (*g).second->ModelFilename + '\0';

                //Calculate S_MODEL section length
                ///       Header 		(6)
                /// + 6 + Name 			(NumChars)
                /// + 6 + Filename		(NumChars)
                /// + 6 + OBJ_POS		(3 x float)
                /// + 6 + OBJ_ROT		(4 x float)
                /// + 6 + OBJ_SCL		(3 x float)
                /// + 6 + MASS          (float)
				/// + 6 + PHYSICS       (int)
				/// + 6 + CUST_DATA		(int)

                MainSection_Length = 	  54		            //All headers
                                        + (int)ModelName.size()		//Name
                                        + (int)ModelFilename.size()	//Filename
                                        + (11 * sizeof(float))	//POS, ROT, SCL, MASS
										+ 8; //Physicstype, customdata

                //Write header
                fwrite(&MainSection_ID, 2, 1, File);

                //Write length
                fwrite(&MainSection_Length, 4, 1, File);

                    //////////////////////////////////////////////////////////////
                    //S_MODEL_NAME
                    SubSection_ID = S_MODEL_NAME;

                    SubSection_Length = (int)ModelName.size() + 6;

                    //Write header
                    fwrite(&SubSection_ID, 2, 1, File);

                    //Write length
                    fwrite(&SubSection_Length, 4, 1, File);

                    //Write object name
                    fwrite(ModelName.c_str(), 1, ModelName.size(), File);

                    //////////////////////////////////////////////////////////////
                    //S_MDS_FILENAME
                    SubSection_ID = S_MDL_FILENAME;

                    SubSection_Length = (int)ModelFilename.size() + 6;

                    //Write header
                    fwrite(&SubSection_ID, 2, 1, File);

                    //Write length
                    fwrite(&SubSection_Length, 4, 1, File);

                    //Write object name
                    fwrite(ModelFilename.c_str(), 1, ModelFilename.size(), File);

                    //////////////////////////////////////////////////////////////////
                    //S_MODEL_POS
                    SubSection_ID = S_MODEL_POS;
                    SubSection_Length = 3 * sizeof(float) + 6;

                    //Write header
                    fwrite(&SubSection_ID, 2, 1, File);

                    //Write length
                    fwrite(&SubSection_Length, 4, 1, File);

                    //Write POS
                    TempFloat = (*g).second->GetPosition().x;
                    fwrite(&TempFloat, sizeof(float), 1, File);

                    TempFloat = (*g).second->GetPosition().y;
                    fwrite(&TempFloat, sizeof(float), 1, File);

                    TempFloat = (*g).second->GetPosition().z;
                    fwrite(&TempFloat, sizeof(float), 1, File);

                    //////////////////////////////////////////////////////////////////
                    //S_MODEL_ROT
                    SubSection_ID = S_MODEL_ROT;
                    SubSection_Length = 4 * sizeof(float) + 6;

                    //Write header
                    fwrite(&SubSection_ID, 2, 1, File);

                    //Write length
                    fwrite(&SubSection_Length, 4, 1, File);

                    //Write ROT
                    TempFloat = (*g).second->GetRotation().X;
                    fwrite(&TempFloat, sizeof(float), 1, File);

                    TempFloat = (*g).second->GetRotation().Y;
                    fwrite(&TempFloat, sizeof(float), 1, File);

                    TempFloat = (*g).second->GetRotation().Z;
                    fwrite(&TempFloat, sizeof(float), 1, File);

					TempFloat = (*g).second->GetRotation().W;
                    fwrite(&TempFloat, sizeof(float), 1, File);

					std::stringstream Stream;
					Stream << "Written rotation: " << (*g).second->GetRotation().X
													<< "\t" << (*g).second->GetRotation().Y
													<< "\t" << (*g).second->GetRotation().Z
													<< "\t" << (*g).second->GetRotation().W;

					sym::Engine->Logger.AddEntry(Stream.str());

                    //////////////////////////////////////////////////////////////////
                    //S_MODEL_SCL
                    SubSection_ID = S_MODEL_SCL;
                    SubSection_Length = 3 * sizeof(float) + 6;

                    //Write header
                    fwrite(&SubSection_ID, 2, 1, File);

                    //Write length
                    fwrite(&SubSection_Length, 4, 1, File);

                    //Write SCALE
                    TempFloat = (*g).second->GetDimentions().x;
                    fwrite(&TempFloat, sizeof(float), 1, File);

                    TempFloat = (*g).second->GetDimentions().y;
                    fwrite(&TempFloat, sizeof(float), 1, File);

                    TempFloat = (*g).second->GetDimentions().z;
                    fwrite(&TempFloat, sizeof(float), 1, File);

                    //////////////////////////////////////////////////////////////////
                    //S_MODEL_MASS
                    SubSection_ID = S_MODEL_MASS;
                    SubSection_Length = 10;

                    //Write header
                    fwrite(&SubSection_ID, 2, 1, File);

                    //Write length
                    fwrite(&SubSection_Length, 4, 1, File);

                    //Write MASS
                    TempFloat = (*g).second->Mass;
                    fwrite(&TempFloat, 4, 1, File);

					//////////////////////////////////////////////////////////////////
                    //S_MODEL_PHYSICS
                    SubSection_ID = S_MODEL_PHYSICS;
                    SubSection_Length = 10;

                    //Write header
                    fwrite(&SubSection_ID, 2, 1, File);

                    //Write length
                    fwrite(&SubSection_Length, 4, 1, File);

                    //Write PHYSICS
                    int TempInt = (*g).second->PhysicsType;
                    fwrite(&TempInt, 4, 1, File);

					//////////////////////////////////////////////////////////////////
                    //S_CUSTOM_DATA
                    SubSection_ID = S_CUSTOM_DATA;
                    SubSection_Length = 10;

                    //Write header
                    fwrite(&SubSection_ID, 2, 1, File);

                    //Write length
                    fwrite(&SubSection_Length, 4, 1, File);

                    //Write S_CUSTOM_DATA
                    TempInt = (*g).second->CustomData;
                    fwrite(&TempInt, 4, 1, File);
            }
			else if((*g).second->Type == SYM_CUST_BRUSH)
			{
				MainSection_ID = S_CUSTOM_BRUSH;

				//Engine->Logger.AddEntry("Writing brush " + (*g).Name);

				//Append '\0' to names
				std::string BrushName = (*g).second->Name + '\0';
				std::string MatFilename = (*g).second->MatFilename + '\0';

				//Calculate S_BRUSH section length
				///       Header 		(6)
				/// + 6 + Name 			(NumChars)
				/// + 6 + MatFilename	(NumChars)
				/// + 6 + BRUSH_POS		(3 x float)
				/// + 6 + BRUSH_ROT		(4 x float)
				/// + 6 + NUM_VERTS		(int)
				/// + 6 + NUM_FACE		(int)
				/// + 6 + VERTS			3x float (x NumVertices), vertex X, Y, Z
				/// + 6 + FACES			3x int + 6x float (x NumFaces)
				/// + 6 + PHYSICS		int
				/// + 6 + MASS			float

				MainSection_Length = 	  72		            //All headers
										+ (int)BrushName.size()		//Name
										+ (int)MatFilename.size()	//Material
										+ (7 * 4)	//POS, ROT
										+ 8						//NUM_VERT + FACES
										+ (3 * 4) * (int)(*g).second->Mesh->Subsets[0].Vertices.size()
										+ (9 * 4) * (int)(*g).second->Mesh->Subsets[0].Faces.size()
										+ 12; //Cust data, physics, mass

				//Write header
				fwrite(&MainSection_ID, 2, 1, File);

				//Write length
				fwrite(&MainSection_Length, 4, 1, File);

					//////////////////////////////////////////////////////////////
					//S_BRUSH_NAME
					SubSection_ID = CB_BRUSH_NAME;

					SubSection_Length = (int)BrushName.size() + 6;

					//Write header
					fwrite(&SubSection_ID, 2, 1, File);

					//Write length
					fwrite(&SubSection_Length, 4, 1, File);

					//Write object name
					fwrite(BrushName.c_str(), 1, BrushName.size(), File);

					//////////////////////////////////////////////////////////////
					//S_MATERIAL
					SubSection_ID = CB_MATERIAL;

					SubSection_Length = (int)MatFilename.size() + 6;

					//Write header
					fwrite(&SubSection_ID, 2, 1, File);

					//Write length
					fwrite(&SubSection_Length, 4, 1, File);

					//Write object name
					fwrite(MatFilename.c_str(), 1, MatFilename.size(), File);

					//////////////////////////////////////////////////////////////////
					//S_BRUSH_POS
					SubSection_ID = CB_BRUSH_POS;
					SubSection_Length = 3 * sizeof(float) + 6;

					//Write header
					fwrite(&SubSection_ID, 2, 1, File);

					//Write length
					fwrite(&SubSection_Length, 4, 1, File);

					//Write POS
					TempFloat = (*g).second->GetPosition().x;
					fwrite(&TempFloat, sizeof(float), 1, File);

					TempFloat = (*g).second->GetPosition().y;
					fwrite(&TempFloat, sizeof(float), 1, File);

					TempFloat = (*g).second->GetPosition().z;
					fwrite(&TempFloat, sizeof(float), 1, File);

					//////////////////////////////////////////////////////////////////
					//S_BRUSH_ROT
					SubSection_ID = CB_BRUSH_ROT;
					SubSection_Length = 4 * sizeof(float) + 6;

					//Write header
					fwrite(&SubSection_ID, 2, 1, File);

					//Write length
					fwrite(&SubSection_Length, 4, 1, File);

					//Write POS
					TempFloat = (*g).second->GetRotation().X;
					fwrite(&TempFloat, sizeof(float), 1, File);

					TempFloat = (*g).second->GetRotation().Y;
					fwrite(&TempFloat, sizeof(float), 1, File);

					TempFloat = (*g).second->GetRotation().Z;
					fwrite(&TempFloat, sizeof(float), 1, File);

					TempFloat = (*g).second->GetRotation().W;
					fwrite(&TempFloat, sizeof(float), 1, File);

				//////////////////////////////////////////////////////////////////
				//NUM_VERTICES
				SubSection_ID = CB_NUM_VERTICES;
				SubSection_Length = 10;

				//Write header
				fwrite(&SubSection_ID, 2, 1, File);

				//Write length
				fwrite(&SubSection_Length, 4, 1, File);

				int Size = (int)(*g).second->Mesh->Subsets[0].Vertices.size();

				//Write numvertices
				fwrite(&Size, 4, 1, File);

				//////////////////////////////////////////////////////////////////
				//NUM_FACES
				SubSection_ID = CB_NUM_FACES;
				SubSection_Length = 10;

				//Write header
				fwrite(&SubSection_ID, 2, 1, File);

				//Write length
				fwrite(&SubSection_Length, 4, 1, File);

				Size = (int)(*g).second->Mesh->Subsets[0].Faces.size();

				//Write numfaces
				fwrite(&Size, 4, 1, File);

				//////////////////////////////////////////////////////////////////
				//VERTICES_LIST
				SubSection_ID = CB_VERTICES_LIST;

				//Calculate section size
				SubSection_Length = (int)(12 * (*g).second->Mesh->Subsets[0].Vertices.size()) + 6;

				//Write header
				fwrite(&SubSection_ID, 2, 1, File);

				//Write length
				fwrite(&SubSection_Length, 4, 1, File);

				//For all vertices
				for (int j = 0; j < (int)(*g).second->Mesh->Subsets[0].Vertices.size(); j++)
				{

					//Write X
					fwrite(&(*g).second->Mesh->Subsets[0].Vertices[j].x, 4, 1, File);

					//Write Y
					fwrite(&(*g).second->Mesh->Subsets[0].Vertices[j].y, 4, 1, File);

					//Write Z
					fwrite(&(*g).second->Mesh->Subsets[0].Vertices[j].z, 4, 1, File);
				}

				//////////////////////////////////////////////////////////////////
				//FACE_LIST
				SubSection_ID = CB_FACE_LIST;

				//Calculate section size
				SubSection_Length = (36 * (int)(*g).second->Mesh->Subsets[0].Faces.size()) + 6;

				//Write header
				fwrite(&SubSection_ID, 2, 1, File);

				//Write length
				fwrite(&SubSection_Length, 4, 1, File);

				//For all faces
				for (int j = 0; j < (int)(*g).second->Mesh->Subsets[0].Faces.size(); j++)
				{
					//Write A
					fwrite(&(*g).second->Mesh->Subsets[0].Faces[j].Indices[0], 4, 1, File);

					//Write B
					fwrite(&(*g).second->Mesh->Subsets[0].Faces[j].Indices[1], 4, 1, File);

					//Write C
					fwrite(&(*g).second->Mesh->Subsets[0].Faces[j].Indices[2], 4, 1, File);

					//Write UV coord A
					fwrite(&(*g).second->Mesh->Subsets[0].Faces[j].UVCoords[0].u, 4, 1, File);
					fwrite(&(*g).second->Mesh->Subsets[0].Faces[j].UVCoords[0].v, 4, 1, File);

					//Write UV coord B
					fwrite(&(*g).second->Mesh->Subsets[0].Faces[j].UVCoords[1].u, 4, 1, File);
					fwrite(&(*g).second->Mesh->Subsets[0].Faces[j].UVCoords[1].v, 4, 1, File);

					//Write UV coord C
					fwrite(&(*g).second->Mesh->Subsets[0].Faces[j].UVCoords[2].u, 4, 1, File);
					fwrite(&(*g).second->Mesh->Subsets[0].Faces[j].UVCoords[2].v, 4, 1, File);
				}

				//////////////////////////////////////////////////////////////////
                //S_MODEL_PHYSICS
                SubSection_ID = S_MODEL_PHYSICS;
                SubSection_Length = 10;

                //Write header
                fwrite(&SubSection_ID, 2, 1, File);

                //Write length
                fwrite(&SubSection_Length, 4, 1, File);

                //Write PHYSICS
                int TempInt = (*g).second->PhysicsType;
                fwrite(&TempInt, 4, 1, File);

				//////////////////////////////////////////////////////////////////
                //S_MODEL_MASS
                SubSection_ID = S_MODEL_MASS;
                SubSection_Length = 10;

                //Write header
                fwrite(&SubSection_ID, 2, 1, File);

                //Write length
                fwrite(&SubSection_Length, 4, 1, File);

                //Write PHYSICS
				TempInt = (*g).second->Mass;
                fwrite(&TempInt, 4, 1, File);

				//////////////////////////////////////////////////////////////////
                //S_CUSTOM_DATA
                SubSection_ID = S_CUSTOM_DATA;
                SubSection_Length = 10;

                //Write header
                fwrite(&SubSection_ID, 2, 1, File);

                //Write length
                fwrite(&SubSection_Length, 4, 1, File);

                //Write S_CUSTOM_DATA
                TempInt = (*g).second->CustomData;
                fwrite(&TempInt, 4, 1, File);
			}
			else if((*g).second->Type == SYM_TERR)
			{
				MainSection_ID = S_TERRAIN;

                Engine->Logger.AddEntry("Writing terrain " + (*g).second->Name);

                //Append '\0' to names
                std::string TerrName = (*g).second->Name + '\0';
                std::string TerrHMapFilename = (*g).second->HMapFilename + '\0';
				std::string TerrMatFilename = (*g).second->MatFilename + '\0';

                //Calculate S_BRUSH section length
                ///       Header 		(6)
                /// + 6 + Name 			(NumChars)
                /// + 6 + HMapFilename	(NumChars)
				/// + 6 + MatFilename	(NumChars)
                /// + 6 + POS			(3 x float)
                /// + 6 + ROT			(4 x float)
                /// + 6 + SCALE			(3 x float)


                MainSection_Length = 	  48		            //All headers
                                        + (int)TerrName.size()		//Name
										+ (int)TerrHMapFilename.size()	//Heightmap
                                        + (int)TerrMatFilename.size()	//Material
                                        + (10 * sizeof(float)),	//POS, ROT, SIZE
										+ 4; //Cust data

                //Write header
                fwrite(&MainSection_ID, 2, 1, File);

                //Write length
                fwrite(&MainSection_Length, 4, 1, File);

                    //////////////////////////////////////////////////////////////
                    //S_TERR_NAME
                    SubSection_ID = S_TERR_NAME;

                    SubSection_Length = (int)TerrName.size() + 6;

                    //Write header
                    fwrite(&SubSection_ID, 2, 1, File);

                    //Write length
                    fwrite(&SubSection_Length, 4, 1, File);

                    //Write object name
                    fwrite(TerrName.c_str(), 1, TerrName.size(), File);

					//////////////////////////////////////////////////////////////
                    //S_HMAP_FILENAME
                    SubSection_ID = S_HMAP_FILENAME;

                    SubSection_Length = (int)TerrHMapFilename.size() + 6;

                    //Write header
                    fwrite(&SubSection_ID, 2, 1, File);

                    //Write length
                    fwrite(&SubSection_Length, 4, 1, File);

                    //Write object name
                    fwrite(TerrHMapFilename.c_str(), 1, TerrHMapFilename.size(), File);

					//////////////////////////////////////////////////////////////
                    //S_TERR_MATERIAL
                    SubSection_ID = S_TERR_MATERIAL;

                    SubSection_Length = (int)TerrMatFilename.size() + 6;

                    //Write header
                    fwrite(&SubSection_ID, 2, 1, File);

                    //Write length
                    fwrite(&SubSection_Length, 4, 1, File);

                    //Write object name
                    fwrite(TerrMatFilename.c_str(), 1, TerrMatFilename.size(), File);

					//////////////////////////////////////////////////////////////////
                    //S_TERR_POS
                    SubSection_ID = S_TERR_POS;
                    SubSection_Length = 3 * sizeof(float) + 6;

                    //Write header
                    fwrite(&SubSection_ID, 2, 1, File);

                    //Write length
                    fwrite(&SubSection_Length, 4, 1, File);

                    //Write POS
                    TempFloat = (*g).second->GetPosition().x;
                    fwrite(&TempFloat, sizeof(float), 1, File);

                    TempFloat = (*g).second->GetPosition().y;
                    fwrite(&TempFloat, sizeof(float), 1, File);

                    TempFloat = (*g).second->GetPosition().z;
                    fwrite(&TempFloat, sizeof(float), 1, File);

                    //////////////////////////////////////////////////////////////////
                    //S_TERR_ROT
                    SubSection_ID = S_TERR_ROT;
                    SubSection_Length = 4 * sizeof(float) + 6;

                    //Write header
                    fwrite(&SubSection_ID, 2, 1, File);

                    //Write length
                    fwrite(&SubSection_Length, 4, 1, File);

                    //Write POS
                    TempFloat = (*g).second->GetRotation().X;
                    fwrite(&TempFloat, sizeof(float), 1, File);

                    TempFloat = (*g).second->GetRotation().Y;
                    fwrite(&TempFloat, sizeof(float), 1, File);

                    TempFloat = (*g).second->GetRotation().Z;
                    fwrite(&TempFloat, sizeof(float), 1, File);

					TempFloat = (*g).second->GetRotation().W;
                    fwrite(&TempFloat, sizeof(float), 1, File);

                    //////////////////////////////////////////////////////////////////
                    //S_TERR_SCALE
                    SubSection_ID = S_TERR_SCALE;
                    SubSection_Length = 3 * sizeof(float) + 6;

                    //Write header
                    fwrite(&SubSection_ID, 2, 1, File);

                    //Write length
                    fwrite(&SubSection_Length, 4, 1, File);

                    //Write SCALE
                    TempFloat = (*g).second->GetDimentions().x;
                    fwrite(&TempFloat, sizeof(float), 1, File);

                    TempFloat = (*g).second->GetDimentions().y;
                    fwrite(&TempFloat, sizeof(float), 1, File);

                    TempFloat = (*g).second->GetDimentions().z;
                    fwrite(&TempFloat, sizeof(float), 1, File);

					//////////////////////////////////////////////////////////////////
                    //S_CUSTOM_DATA
                    SubSection_ID = S_CUSTOM_DATA;
                    SubSection_Length = 10;

                    //Write header
                    fwrite(&SubSection_ID, 2, 1, File);

                    //Write length
                    fwrite(&SubSection_Length, 4, 1, File);

                    //Write S_CUSTOM_DATA
                    int TempInt = (*g).second->CustomData;
                    fwrite(&TempInt, 4, 1, File);

			}
            else
            {
                //It's a brush

                MainSection_ID = S_BRUSH;

                Engine->Logger.AddEntry("Writing brush " + (*g).second->Name);

                //Append '\0' to names
                std::string BrushName = (*g).second->Name + '\0';
                std::string MatFilename = (*g).second->MatFilename + '\0';

                //Calculate S_BRUSH section length
                ///       Header 		(6)
                /// + 6 + Name 			(NumChars)
                /// + 6 + MatFilename	(NumChars)
                /// + 6 + TYPE          (Int)
                /// + 6 + BRUSH_POS		(3 x float)
                /// + 6 + BRUSH_ROT		(4 x float)
                /// + 6 + BRUSH_SZE		(3 x float)
				/// + 6 + UV_TILE		(2 x float)
				/// + 6 + UV_OFFSET		(2 x float)
				/// + 6 + UV_ROTATE		(float)
				/// + 6 + PHYSICS		(int)

                MainSection_Length = 	  78		            //All headers
                                        + (int)BrushName.size()		//Name
                                        + (int)MatFilename.size()	//Material
                                        + 4                     //Type
                                        + (16 * sizeof(float))	//POS, ROT, SIZE, MASS, UV
										+ 8; //Physics, cust data

                //Write header
                fwrite(&MainSection_ID, 2, 1, File);

                //Write length
                fwrite(&MainSection_Length, 4, 1, File);

                    //////////////////////////////////////////////////////////////
                    //S_BRUSH_NAME
                    SubSection_ID = S_BRUSH_NAME;

                    SubSection_Length = (int)BrushName.size() + 6;

                    //Write header
                    fwrite(&SubSection_ID, 2, 1, File);

                    //Write length
                    fwrite(&SubSection_Length, 4, 1, File);

                    //Write object name
                    fwrite(BrushName.c_str(), 1, BrushName.size(), File);

                    //////////////////////////////////////////////////////////////////
                    //S_TYPE
                    SubSection_ID = S_TYPE;
                    SubSection_Length = 10;

                    //Write header
                    fwrite(&SubSection_ID, 2, 1, File);

                    //Write length
                    fwrite(&SubSection_Length, 4, 1, File);

                    //Write R
                    fwrite(&(*g).second->Type, 4, 1, File);

                    //////////////////////////////////////////////////////////////
                    //S_MATERIAL
                    SubSection_ID = S_MATERIAL;

                    SubSection_Length = (int)MatFilename.size() + 6;

                    //Write header
                    fwrite(&SubSection_ID, 2, 1, File);

                    //Write length
                    fwrite(&SubSection_Length, 4, 1, File);

                    //Write object name
                    fwrite(MatFilename.c_str(), 1, MatFilename.size(), File);

                    //////////////////////////////////////////////////////////////////
                    //S_BRUSH_SIZE
                    SubSection_ID = S_BRUSH_SIZE;
                    SubSection_Length = 3 * sizeof(float) + 6;

                    //Write header
                    fwrite(&SubSection_ID, 2, 1, File);

                    //Write length
                    fwrite(&SubSection_Length, 4, 1, File);

                    //Write POS
                    TempFloat = (*g).second->GetDimentions().x;
                    fwrite(&TempFloat, sizeof(float), 1, File);

                    TempFloat = (*g).second->GetDimentions().y;
                    fwrite(&TempFloat, sizeof(float), 1, File);

                    TempFloat = (*g).second->GetDimentions().z;
                    fwrite(&TempFloat, sizeof(float), 1, File);

                    //////////////////////////////////////////////////////////////////
                    //S_BRUSH_POS
                    SubSection_ID = S_BRUSH_POS;
                    SubSection_Length = 3 * sizeof(float) + 6;

                    //Write header
                    fwrite(&SubSection_ID, 2, 1, File);

                    //Write length
                    fwrite(&SubSection_Length, 4, 1, File);

                    //Write POS
                    TempFloat = (*g).second->GetPosition().x;
                    fwrite(&TempFloat, sizeof(float), 1, File);

                    TempFloat = (*g).second->GetPosition().y;
                    fwrite(&TempFloat, sizeof(float), 1, File);

                    TempFloat = (*g).second->GetPosition().z;
                    fwrite(&TempFloat, sizeof(float), 1, File);

                    //////////////////////////////////////////////////////////////////
                    //S_BRUSH_ROT
                    SubSection_ID = S_BRUSH_ROT;
                    SubSection_Length = 4 * sizeof(float) + 6;

                    //Write header
                    fwrite(&SubSection_ID, 2, 1, File);

                    //Write length
                    fwrite(&SubSection_Length, 4, 1, File);

                    //Write POS
                    TempFloat = (*g).second->GetRotation().X;
                    fwrite(&TempFloat, sizeof(float), 1, File);

                    TempFloat = (*g).second->GetRotation().Y;
                    fwrite(&TempFloat, sizeof(float), 1, File);

                    TempFloat = (*g).second->GetRotation().Z;
                    fwrite(&TempFloat, sizeof(float), 1, File);

					TempFloat = (*g).second->GetRotation().W;
                    fwrite(&TempFloat, sizeof(float), 1, File);

                    //////////////////////////////////////////////////////////////////
                    //S_BRUSH_MASS
                    SubSection_ID = S_BRUSH_MASS;
                    SubSection_Length = 10;

                    //Write header
                    fwrite(&SubSection_ID, 2, 1, File);

                    //Write length
                    fwrite(&SubSection_Length, 4, 1, File);

                    //Write MASS
                    TempFloat = (*g).second->Mass;
                    fwrite(&TempFloat, sizeof(float), 1, File);

					//////////////////////////////////////////////////////////////////
                    //S_BRUSH_PHYSICS
                    SubSection_ID = S_BRUSH_PHYSICS;
                    SubSection_Length = 10;

                    //Write header
                    fwrite(&SubSection_ID, 2, 1, File);

                    //Write length
                    fwrite(&SubSection_Length, 4, 1, File);

                    //Write PHYSICS
					TempInt = (*g).second->PhysicsType;
                    fwrite(&TempInt, 4, 1, File);

					//////////////////////////////////////////////////////////////////
                    //S_UV_TILE
                    SubSection_ID = S_UV_TILE;
                    SubSection_Length = 2 * sizeof(float) + 6;

                    //Write header
                    fwrite(&SubSection_ID, 2, 1, File);

                    //Write length
                    fwrite(&SubSection_Length, 4, 1, File);

                    //Write UV_TILE
					TempFloat = (*g).second->TextureTile.x;
                    fwrite(&TempFloat, sizeof(float), 1, File);

					TempFloat = (*g).second->TextureTile.y;
					fwrite(&TempFloat, sizeof(float), 1, File);

					//////////////////////////////////////////////////////////////////
                    //S_UV_OFFSET
                    SubSection_ID = S_UV_OFFSET;
                    SubSection_Length = 2 * sizeof(float) + 6;

                    //Write header
                    fwrite(&SubSection_ID, 2, 1, File);

                    //Write length
                    fwrite(&SubSection_Length, 4, 1, File);

                    //Write UV_OFFSET
                    TempFloat = (*g).second->TextureOffset.x;
                    fwrite(&TempFloat, sizeof(float), 1, File);

					TempFloat = (*g).second->TextureOffset.y;
					fwrite(&TempFloat, sizeof(float), 1, File);

					//////////////////////////////////////////////////////////////////
                    //S_UV_ROTATE
                    SubSection_ID = S_UV_ROTATE;
                    SubSection_Length = 10;

                    //Write header
                    fwrite(&SubSection_ID, 2, 1, File);

                    //Write length
                    fwrite(&SubSection_Length, 4, 1, File);

                    //Write UV_ROTATION
                    TempFloat = (*g).second->TextureRotation.z;
                    fwrite(&TempFloat, 4, 1, File);

					//////////////////////////////////////////////////////////////////
                    //S_CUSTOM_DATA
                    SubSection_ID = S_CUSTOM_DATA;
                    SubSection_Length = 10;

                    //Write header
                    fwrite(&SubSection_ID, 2, 1, File);

                    //Write length
                    fwrite(&SubSection_Length, 4, 1, File);

                    //Write S_CUSTOM_DATA
                    int TempInt = (*g).second->CustomData;
                    fwrite(&TempInt, 4, 1, File);
            }
		}

		//////////////////////////////////////////////////////////////////
		//S_SKYBOX
		MainSection_ID = S_SKYBOX;

		std::map<int, std::string> TexNames;// = Skybox.TextureNames;

		int NameSize = 0;

		for(std::map<int, std::string> ::iterator Texture = TexNames.begin(); Texture != TexNames.end(); Texture++)
		{
			//Add \0 to string
			(*Texture).second += '\0';

			//Add string size to count
			NameSize += (int)(*Texture).second.size();

			//Add integer (type) and header to count
			NameSize += 10;
		}

		MainSection_Length = 6 + NameSize;

		//Write header
		fwrite ( &MainSection_ID, 2, 1, File );

		//Write length
		fwrite ( &MainSection_Length, 4, 1, File );

		for(std::map<int, std::string> ::iterator Texture = TexNames.begin(); Texture != TexNames.end(); Texture++)
		{
			//////////////////////////////////////////////////////////////////
			//S_SKY_TEXTURE
			Engine->Logger.AddEntry("Writing skybox texture");
			SubSection_ID = S_SKY_TEXTURE;
			SubSection_Length = (int)(*Texture).second.size() + 10;

			//Write header
			fwrite ( &SubSection_ID, 2, 1, File );

			//Write length
			fwrite ( &SubSection_Length, 4, 1, File );

			//Write sky texture face
			int SkyFace = (*Texture).first;
			fwrite ( &SkyFace, 1, 4, File );

			//Write object name
			std::string TexName = (*Texture).second;
			fwrite (TexName.c_str(), 1, TexName.size(), File);
		}

        //Close the file
        fclose ( File );

        Engine->Logger.AddEntry("Scene " + Filename + " successfully written");

        return true;
    }

	 bool CScene::Load(std::string Filename)
    {
        //Current MAIN section ID and length
        int MainSection_ID = 0;
        int MainSection_Length = 0;

        //Current SUB section ID and length
        int SubSection_ID = 0;
        int SubSection_Length = 0;
        int SubSection_Start = 0;

        //Current CHILD section ID and length
        int ChildSection_ID = 0;
        int ChildSection_Length = 0;
        int ChildSection_Start = 0;

        //Prefix with scenes path from engine config
		Filename = Engine->ScenePath + Filename;

		Engine->Logger.AddEntry("Loading scene file " + Filename);

        //Open file for reading
        FILE *File = fopen ( Filename.c_str(), "rb" );

        //Check for errors
        if ( !File )
        {
            Engine->Logger.AddEntry("Error: Could not open scene file " + Filename);
            return false;
        }

        //Determine filesize (Seek to end, get current position, rewind to start)
        fseek ( File, 0, SEEK_END );
        int FileSize = ftell ( File );
        rewind ( File );

        //Loop through file until end
        while ( ftell ( File ) < FileSize )
        {
            //Read the section header
            fread ( &MainSection_ID, 2, 1, File );

            //Read the length of the section
            fread ( &MainSection_Length, 4, 1, File );

            //Determine section type
            switch ( MainSection_ID )
            {
                case S_ROOT_SECTION:
                    ////cout << "\n+S_ROOT_SECTION \tPosition:" << ftell ( File ) - 6 << "\tLength: " << MainSection_Length << "\n|\n";

                    break;

                case S_GLOBALS:
                    //cout << "+--S_GLOBALS \tPosition:" << ftell ( File ) - 6 << "\tLength: " << MainSection_Length << "\n|  |\n";

                    //Store where the subsection was found, so we can return later
                    SubSection_Start = ftell ( File ) - 6;

                    //Zero the subsection ID and length, in case they were used previously
                    SubSection_ID = 0;
                    SubSection_Length = 0;

                    //Loop from subsection Start to subsection End (Mainsection length, + the position it was found)
                    while ( ftell ( File ) < ( MainSection_Length + SubSection_Start ) )
                    {
                        //Read the subchunk ID
                        fread ( &SubSection_ID, 2, 1, File );

                        //Read the length of the subchunk
                        fread ( &SubSection_Length, 4, 1, File );

                        //Determine SubChunk type using its ID
                        switch ( SubSection_ID )
                        {
                            case S_AMBIENT_LIGHT:
							{
                                //cout << "|  +S_AMBIENT_LIGHT \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

								CColour Ambient;

                                fread ( &Ambient.r, 4, 1, File );
                                fread ( &Ambient.g, 4, 1, File );
                                fread ( &Ambient.b, 4, 1, File );

								SetAmbientLight(Ambient);

                                break;
							}

                            case S_TERRAIN_FILE:
							{
                                //cout << "|  +S_TERRAIN_FILE \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

                                //Clear current name
								std::string Heightmap;

                                int CharCount = 0;
								char TempChar;
                                do
                                {
                                    fread ( &TempChar, 1, 1, File );
                                    Heightmap += TempChar;
                                    CharCount++;
                                }
                                while ( TempChar != '\0' );

                                //Remove '\0' or things get messy
                                Heightmap = Heightmap.substr(0, Heightmap.size() - 1);

                                break;
							}

                            case S_SKY_FILE:
							{
                                //cout << "|  +S_SKY_FILE \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

                                //Clear current name
								std::string SkyFile;

                                int CharCount = 0;
								char TempChar;
                                do
                                {
                                    fread ( &TempChar, 1, 1, File );
                                    SkyFile += TempChar;
                                    CharCount++;
                                }
                                while ( TempChar != '\0' );

                                //Remove '\0' or things get messy
                                SkyFile = SkyFile.substr(0, SkyFile.size() - 1);

                                break;
							}

                            case S_SCRIPT_FILE:
							{
                                //cout << "|  +S_SCRIPT_FILE \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

                                //Clear current name
								std::string ScriptFile;

                                int CharCount = 0;
								char TempChar;
                                do
                                {
                                    fread ( &TempChar, 1, 1, File );
                                    ScriptFile += TempChar;
                                    CharCount++;
                                }
                                while ( TempChar != '\0' );

                                //Remove '\0' or things get messy
                                ScriptFile = ScriptFile.substr(0, ScriptFile.size());

                                break;
							}

                            case S_GRAVITY:
							{
                                //cout << "|  +S_GRAVITY \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

								float Gravity;

                                fread ( &Gravity, 4, 1, File );

								//TODO: Apply gravity

                                break;
							}

                            default:
                                //cout << "|  +**UNKNOWN \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\tID: 0x" << hex << SubSection_ID << dec << "\n|\n";

                                //Advance to next subchunk (Current Position, + SubChunk length, - the header (6))
                                fseek ( File, SubSection_Length - 6, SEEK_CUR );

                                break;
                        }
                    }

                    //Move to end of subsection before returning
                    //(where we found the subsection, + section length, - the header (6))
                    fseek ( File, SubSection_Start + MainSection_Length, SEEK_SET );

                    break;

                case S_MODEL:
				{
                    //cout << "+--S_MODEL \tPosition:" << ftell ( File ) - 6 << "\tLength: " << MainSection_Length << "\n|  |\n";

					std::string ModelName;
					std::string ModelFile;
					CVector3 ModelPos;
					CQuaternion ModelRot;
					CVector3 ModelScale(1.0f, 1.0f, 1.0f);
					float ModelMass = 0.0f;
					int ModelPhysics = 0;
					int CustomData = 0;

                    //Store where the subsection was found, so we can return later
                    SubSection_Start = ftell ( File ) - 6;

                    //Zero the subsection ID and length, in case they were used previously
                    SubSection_ID = 0;
                    SubSection_Length = 0;

                    //Loop from subsection Start to subsection End (Mainsection length, + the position it was found)
                    while ( ftell ( File ) < ( MainSection_Length + SubSection_Start ) )
                    {
                        //Read the subchunk ID
                        fread ( &SubSection_ID, 2, 1, File );

                        //Read the length of the subchunk
                        fread ( &SubSection_Length, 4, 1, File );

                        //Determine SubChunk type using its ID
                        switch ( SubSection_ID )
                        {
                            case S_MODEL_NAME:
							{
                                //cout << "|  +S_MODEL_NAME \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

                                int CharCount = 0;
								char TempChar;
                                do
                                {
                                    fread ( &TempChar, 1, 1, File );
                                    ModelName += TempChar;
                                    CharCount++;
                                }
                                while ( TempChar != '\0' );

                                //Remove '\0' or things get messy
                                ModelName = ModelName.substr(0, ModelName.size() - 1);

                                break;
							}

                            case S_MDL_FILENAME:
							{
                                //cout << "|  +S_MDL_FILENAME \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

                                int CharCount = 0;
								char TempChar;
                                do
                                {
                                    fread ( &TempChar, 1, 1, File );
                                    ModelFile += TempChar;
                                    CharCount++;
                                }
                                while ( TempChar != '\0' );

                                //Remove '\0' or things get messy
                                ModelFile = ModelFile.substr(0, ModelFile.size() - 1);

                                break;
							}

                            case S_MODEL_POS:
							{
                                //cout << "|  +S_MODEL_POS \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

                                fread ( &ModelPos.x, sizeof(float), 1, File );
                                fread ( &ModelPos.y, sizeof(float), 1, File );
                                fread ( &ModelPos.z, sizeof(float), 1, File );
								                                break;
							}

                            case S_MODEL_ROT:
							{
                                //cout << "|  +S_MODEL_ROT \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";
                                fread ( &ModelRot.X, sizeof(float), 1, File );
                                fread ( &ModelRot.Y, sizeof(float), 1, File );
                                fread ( &ModelRot.Z, sizeof(float), 1, File );
								fread ( &ModelRot.W, sizeof(float), 1, File );

                                break;
							}

                            case S_MODEL_SCL:
							{
                                //cout << "|  +S_MODEL_SCL \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";
                                fread ( &ModelScale.x, sizeof(float), 1, File );
                                fread ( &ModelScale.y, sizeof(float), 1, File );
                                fread ( &ModelScale.z, sizeof(float), 1, File );

                                break;
							}

                            case S_MODEL_MASS:
							{
                                //cout << "|  +S_MODEL_MASS \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

                                fread ( &ModelMass, sizeof(float), 1, File );

                                break;
							}

							case S_MODEL_PHYSICS:
							{
                                //cout << "|  +S_MODEL_MASS \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

                                fread ( &ModelPhysics, 4, 1, File );

                                break;
							}

							case S_CUSTOM_DATA:
							{
								//Read byte
								fread(&CustomData, 4, 1, File);
							}

                            default:
                                //cout << "|  +**UNKNOWN \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\tID: 0x" << hex << SubSection_ID << dec << "\n|\n";

                                //Advance to next subchunk (Current Position, + SubChunk length, - the header (6))
                                fseek ( File, SubSection_Length - 6, SEEK_CUR );

                                break;
                        }
                    }

                    //Create model
					sym::CModel *Model = AddModel(ModelName, ModelFile, ModelPhysics);

					if(Model)
					{
						Model->SetPosition(ModelPos);
						Model->SetRotation(ModelRot);
						Model->Scale(ModelScale);
						Model->SetMass(ModelMass);
						Model->CustomData = CustomData;
					}
					else
					{
						Engine->Logger.AddEntry("Error: Could not process model section " + ModelName + " in scene file " + Filename);
					}


                    //Move to end of subsection before returning
                    //(where we found the subsection, + section length, - the header (6))
                    fseek ( File, SubSection_Start + MainSection_Length, SEEK_SET );

                    break;
				}

				case S_CUSTOM_BRUSH:
				{
                    //cout << "+--S_MODEL \tPosition:" << ftell ( File ) - 6 << "\tLength: " << MainSection_Length << "\n|  |\n";

					CModel *Model = new CModel;
					CMesh *Mesh = new CMesh;
					CMeshSubset Subset;

					float ModelMass = 0.0f;
					int ModelPhysics = sym::LEVEL_COLLISION;
					int CustomData = 0;

                    //Store where the subsection was found, so we can return later
                    SubSection_Start = ftell ( File ) - 6;

                    //Zero the subsection ID and length, in case they were used previously
                    SubSection_ID = 0;
                    SubSection_Length = 0;

					int NumFaces = 0;
					int NumVerts = 0;

					CVector3 ModelPos;
					CQuaternion ModelRot;

                    //Loop from subsection Start to subsection End (Mainsection length, + the position it was found)
                    while ( ftell ( File ) < ( MainSection_Length + SubSection_Start ) )
                    {
                        //Read the subchunk ID
                        fread ( &SubSection_ID, 2, 1, File );

                        //Read the length of the subchunk
                        fread ( &SubSection_Length, 4, 1, File );

                        //Determine SubChunk type using its ID
                        switch ( SubSection_ID )
                        {
                            case CB_BRUSH_NAME:
							{
                                //cout << "|  +S_MODEL_NAME \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

								std::string ModelName;
                                int CharCount = 0;
								char TempChar;
                                do
                                {
                                    fread ( &TempChar, 1, 1, File );
                                    ModelName += TempChar;
                                    CharCount++;
                                }
                                while ( TempChar != '\0' );

                                //Remove '\0' or things get messy
                                Model->Name = ModelName.substr(0, ModelName.size() - 1);

                                break;
							}

							case CB_MATERIAL:
							{
								//Clear current name
								std::string MatFilename;
								int CharCount = 0;
								char TempChar;
								do
								{
									fread (&TempChar, 1, 1, File);
									MatFilename += TempChar;
									CharCount++;
								}while(TempChar != '\0');

								//Remove '\0'
								Model->MatFilename = MatFilename.substr(0, MatFilename.size() - 1);

								break;
							}

                            case CB_BRUSH_POS:
							{
                                //cout << "|  +S_MODEL_POS \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

                                fread ( &ModelPos.x, sizeof(float), 1, File );
                                fread ( &ModelPos.y, sizeof(float), 1, File );
                                fread ( &ModelPos.z, sizeof(float), 1, File );

                                break;
							}

                            case CB_BRUSH_ROT:
							{
                                //cout << "|  +S_MODEL_ROT \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

                                fread ( &ModelRot.X, sizeof(float), 1, File );
                                fread ( &ModelRot.Y, sizeof(float), 1, File );
                                fread ( &ModelRot.Z, sizeof(float), 1, File );
								fread ( &ModelRot.W, sizeof(float), 1, File );

                                break;
							}

							case CB_NUM_VERTICES:
								fread (&NumVerts, 4, 1, File);
								break;

							case CB_NUM_FACES:
								fread (&NumFaces, 4, 1, File);
								break;

							case CB_VERTICES_LIST:
							{
								//For all vertices
								for (int i = 0; i < NumVerts; i++)
								{
									//Create temp vertex
									CVector3 TempVertex;

									//Read X
									fread (&TempVertex.x, sizeof(float), 1, File);

									//Read Y
									fread (&TempVertex.y, sizeof(float), 1, File);

									//Read Z
									fread (&TempVertex.z, sizeof(float), 1, File);

									//Push back to vector
									Subset.Vertices.push_back(TempVertex);

									//Add vertex to AABB
									Mesh->AABB.AddVertex(CVector3(TempVertex.x, TempVertex.y, TempVertex.z));
								}

								break;
							}

							case CB_FACE_LIST:
							{
								//For all faces
								for (int i = 0; i < NumFaces; i++)
								{
									//Create new temp face
									CFace TempFace;

									//Read A
									fread (&TempFace.Indices[0], 4, 1, File);

									//Read B
									fread (&TempFace.Indices[1], 4, 1, File);

									//Read C
									fread (&TempFace.Indices[2], 4, 1, File);

									//Read UVCoords[0]
									fread (&TempFace.UVCoords[0].u, 4, 1, File);
									fread (&TempFace.UVCoords[0].v, 4, 1, File);

									//Read UVCoords[1]
									fread (&TempFace.UVCoords[1].u, 4, 1, File);
									fread (&TempFace.UVCoords[1].v, 4, 1, File);

									//Read UVc
									fread (&TempFace.UVCoords[2].u, 4, 1, File);
									fread (&TempFace.UVCoords[2].v, 4, 1, File);

									//Push back the vector
									Subset.Faces.push_back(TempFace);
								}

								break;
							}

							case S_MODEL_MASS:
							{
                                //cout << "|  +S_MODEL_MASS \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

                                fread ( &ModelMass, sizeof(float), 1, File );
								
								if(ModelMass > 0.0f)
								{
									int Nothing = 0;
								}

                                break;
							}

							case S_MODEL_PHYSICS:
							{
                                //cout << "|  +S_MODEL_MASS \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

                                fread ( &ModelPhysics, 4, 1, File );

                                break;
							}

							case S_CUSTOM_DATA:
							{
								//Read byte
								fread(&CustomData, 4, 1, File);
							}

                            default:
                                //cout << "|  +**UNKNOWN \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\tID: 0x" << hex << SubSection_ID << dec << "\n|\n";

                                //Advance to next subchunk (Current Position, + SubChunk length, - the header (6))
                                fseek ( File, SubSection_Length - 6, SEEK_CUR );

                                break;
                        }
                    }

                    Model->Mesh = Mesh;
					Model->Type = SYM_CUST_BRUSH;
					Subset.Process();
					Mesh->Subsets.push_back(Subset);
					Model->SetMaterial(Model->MatFilename);
					//Model->CreateLevel(Mesh->Subsets[0].Vertices, Mesh->Subsets[0].Faces);
					Model->PhysicsType = ModelPhysics;

					//Setup collision body
					switch(ModelPhysics)
					{
						case PHYSICS_OBJ_BOX:
							Model->CreateBox(Mesh->AABB.Size);
							break;

						case PHYSICS_OBJ_SPHERE:
							Model->CreateSphere(Mesh->AABB.Size);
							break;

						case PHYSICS_OBJ_CAPSULE:
							Model->CreateCapsule(Mesh->AABB.Size);
							break;

						case PHYSICS_OBJ_CUSTOM:
						{
							std::vector<CVector3> Vertices;

							for(std::vector<CMeshSubset>::iterator i = Mesh->Subsets.begin(); i != Mesh->Subsets.end(); i++)
							{
								Vertices.insert(Vertices.end(), i->Vertices.begin(), i->Vertices.end());
							}

							Model->CreateCustom(Vertices);
							break;
						}

						case LEVEL_COLLISION:
						{
							Model->CreateLevel(Mesh->Subsets[0].Vertices, Mesh->Subsets[0].Faces);
							break;
						}

						case NO_PHYSICS:
							break;

						default: 
							Model->CreateBox(Mesh->AABB.Size);
							break;
					}

					//Set physics geometry
					Model->SetGeometry(dynamic_cast<CGeometry*>(Model));
					Model->CustomData = CustomData;
					Model->SetMass(ModelMass);
					AddGeometry(Model);

					Model->SetPosition(ModelPos);
					Model->SetRotation(ModelRot);

                    //Move to end of subsection before returning
                    //(where we found the subsection, + section length, - the header (6))
                    fseek ( File, SubSection_Start + MainSection_Length, SEEK_SET );

                    break;
				}

				case S_TERRAIN:
				{
                    //cout << "+--S_TERRAIN \tPosition:" << ftell ( File ) - 6 << "\tLength: " << MainSection_Length << "\n|  |\n";

                    CTerrain *Terrain = new CTerrain;

					int CustomData = 0;

                    //Store where the subsection was found, so we can return later
                    SubSection_Start = ftell ( File ) - 6;

                    //Zero the subsection ID and length, in case they were used previously
                    SubSection_ID = 0;
                    SubSection_Length = 0;

                    //Loop from subsection Start to subsection End (Mainsection length, + the position it was found)
                    while ( ftell ( File ) < ( MainSection_Length + SubSection_Start ) )
                    {
                        //Read the subchunk ID
                        fread ( &SubSection_ID, 2, 1, File );

                        //Read the length of the subchunk
                        fread ( &SubSection_Length, 4, 1, File );

                        //Determine SubChunk type using its ID
                        switch ( SubSection_ID )
                        {
                            case S_TERR_NAME:
							{
                                //cout << "|  +S_TERR_NAME \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

                                //Clear current name
                                Terrain->Name.clear();

                                int CharCount = 0;
								char TempChar;
                                do
                                {
                                    fread ( &TempChar, 1, 1, File );
                                    Terrain->Name += TempChar;
                                    CharCount++;
                                }
                                while ( TempChar != '\0' );

                                //Remove '\0' or things get messy
                                Terrain->Name = Terrain->Name.substr(0, Terrain->Name.size() - 1);

                                break;
							}

                            case S_HMAP_FILENAME:
							{
                                //cout << "|  +S_HMAP_FILENAME \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

                                //Clear current name
								std::string HeightMap;

                                int CharCount = 0;
								char TempChar;
                                do
                                {
                                    fread ( &TempChar, 1, 1, File );
                                    HeightMap += TempChar;
                                    CharCount++;
                                }
                                while ( TempChar != '\0' );

                                //Remove '\0' or things get messy
                                HeightMap = HeightMap.substr(0, HeightMap.size() - 1);

								Terrain->Load(Terrain->Name, HeightMap);

                                break;
							}

							case S_TERR_MATERIAL:
							{
                                //cout << "|  +S_TERR_MATERIAL \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

                                //Clear current name
								std::string Material;

                                int CharCount = 0;
								char TempChar;
                                do
                                {
                                    fread ( &TempChar, 1, 1, File );
                                    Material += TempChar;
                                    CharCount++;
                                }
                                while ( TempChar != '\0' );

                                //Remove '\0' or things get messy when using the string
                                Material = Material.substr(0, Material.size() - 1);

								Terrain->SetMaterial(Material);

                                break;
							}

                            case S_TERR_POS:
							{
                                //cout << "|  +S_TERR_POS \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

								CVector3 Position;

                                fread ( &Position.x, sizeof(float), 1, File );
                                fread ( &Position.y, sizeof(float), 1, File );
                                fread ( &Position.z, sizeof(float), 1, File );

								Terrain->SetPosition(Position);

                                break;
							}

                            case S_TERR_ROT:
							{
                                //cout << "|  +S_TERR_ROT \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

								CQuaternion Rotation;

                                fread ( &Rotation.X, sizeof(float), 1, File );
                                fread ( &Rotation.Y, sizeof(float), 1, File );
                                fread ( &Rotation.Z, sizeof(float), 1, File );
								fread ( &Rotation.W, sizeof(float), 1, File );

								Terrain->SetRotation(Rotation);

                                break;
							}

                            case S_TERR_SCALE:
							{
                                //cout << "|  +S_TERR_SCALE \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

								CVector3 Scale;

                                fread ( &Scale.x, sizeof(float), 1, File );
                                fread ( &Scale.y, sizeof(float), 1, File );
                                fread ( &Scale.z, sizeof(float), 1, File );

								Terrain->Scale(Scale);

                                break;
							}

							case S_CUSTOM_DATA:
							{
								//Read byte
								fread(&CustomData, 4, 1, File);
							}

                            default:
                                //cout << "|  +**UNKNOWN \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\tID: 0x" << hex << SubSection_ID << dec << "\n|\n";

                                //Advance to next subchunk (Current Position, + SubChunk length, - the header (6))
                                fseek ( File, SubSection_Length - 6, SEEK_CUR );

                                break;
                        }
                    }

					Terrain->CustomData = CustomData;
                    AddGeometry(Terrain);

                    //Move to end of subsection before returning
                    //(where we found the subsection, + section length, - the header (6))
                    fseek ( File, SubSection_Start + MainSection_Length, SEEK_SET );

                    break;
				}

                case S_BRUSH:
				{
                    //cout << "+--S_BRUSH \tPosition:" << ftell ( File ) - 6 << "\tLength: " << MainSection_Length << "\n|  |\n";

                    std::string BrushName;
					int BrushType = 0;
					std::string BrushMaterial;
					CVector3 BrushPos;
					CQuaternion BrushRot;
					CVector3 BrushDimentions(16.0f, 16.0f, 16.0f);
					float BrushMass = 0.0f;
					int BrushPhysics = 0;
					CVector3 UVTile(1.0f, 1.0f, 1.0f);
					CVector3 UVOffset;
					CVector3 UVRotate;
					int CustomData = 0;

                    //Store where the subsection was found, so we can return later
                    SubSection_Start = ftell ( File ) - 6;

                    //Zero the subsection ID and length, in case they were used previously
                    SubSection_ID = 0;
                    SubSection_Length = 0;

                    //Loop from subsection Start to subsection End (Mainsection length, + the position it was found)
                    while ( ftell ( File ) < ( MainSection_Length + SubSection_Start ) )
                    {
                        //Read the subchunk ID
                        fread ( &SubSection_ID, 2, 1, File );

                        //Read the length of the subchunk
                        fread ( &SubSection_Length, 4, 1, File );

                        //Determine SubChunk type using its ID
                        switch ( SubSection_ID )
                        {
                            case S_BRUSH_NAME:
							{
                                //cout << "|  +S_BRUSH_NAME \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

                                int CharCount = 0;
								char TempChar;
                                do
                                {
                                    fread ( &TempChar, 1, 1, File );
                                    BrushName += TempChar;
                                    CharCount++;
                                }
                                while ( TempChar != '\0' );

                                //Remove '\0' or things get messy when using the string
                                BrushName = BrushName.substr(0, BrushName.size() - 1);

                                break;
							}

                            case S_TYPE:
							{
                                //cout << "|  +S_TYPE \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

                                fread ( &BrushType, 4, 1, File );

                                break;
							}

                            case S_MATERIAL:
							{
                                //cout << "|  +S_MATERIAL \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

                                int CharCount = 0;
								char TempChar;
                                do
                                {
                                    fread ( &TempChar, 1, 1, File );
                                    BrushMaterial += TempChar;
                                    CharCount++;
                                }
                                while ( TempChar != '\0' );

                                //Remove '\0' or things get messy when using the string
                                BrushMaterial = BrushMaterial.substr(0, BrushMaterial.size() - 1);

                                break;
							}

                            case S_BRUSH_SIZE:
							{
                                //cout << "|  +S_BRUSH_SIZE \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

                                fread ( &BrushDimentions.x, sizeof(float), 1, File );
                                fread ( &BrushDimentions.y, sizeof(float), 1, File );
                                fread ( &BrushDimentions.z, sizeof(float), 1, File );

                                break;
							}

                            case S_BRUSH_POS:
							{
                                //cout << "|  +S_BRUSH_POS \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";
                                fread ( &BrushPos.x, sizeof(float), 1, File );
                                fread ( &BrushPos.y, sizeof(float), 1, File );
                                fread ( &BrushPos.z, sizeof(float), 1, File );

                                break;
							}

                            case S_BRUSH_ROT:
							{
                                //cout << "|  +S_BRUSH_ROT \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

                                fread ( &BrushRot.X, sizeof(float), 1, File );
                                fread ( &BrushRot.Y, sizeof(float), 1, File );
                                fread ( &BrushRot.Z, sizeof(float), 1, File );
								fread ( &BrushRot.W, sizeof(float), 1, File );

                                break;
							}

                            case S_BRUSH_MASS:
							{
                                //cout << "|  +S_BRUSH_MASS \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

                                fread(&BrushMass, sizeof(float), 1, File);

                                break;
							}

							case S_BRUSH_PHYSICS:
							{
                                //cout << "|  +S_BRUSH_MASS \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

                                fread(&BrushPhysics, 4, 1, File);

                                break;
							}

							case S_UV_TILE:
							{
                                //cout << "|  +S_UV_TILE \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";
                                fread ( &UVTile.x, sizeof(float), 1, File );
                                fread ( &UVTile.y, sizeof(float), 1, File );

                                break;
							}

							case S_UV_OFFSET:
							{
                                //cout << "|  +S_UV_OFFSET \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

                                fread ( &UVOffset.x, sizeof(float), 1, File );
                                fread ( &UVOffset.y, sizeof(float), 1, File );

                                break;
							}

							case S_UV_ROTATE:
							{
                                //cout << "|  +S_UV_ROTATION \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

                                fread ( &UVRotate.z, sizeof(float), 1, File );

                                break;
							}

							case S_CUSTOM_DATA:
							{
								//Read byte
								fread(&CustomData, 4, 1, File);
							}

                            default:
                                //cout << "|  +**UNKNOWN \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\tID: 0x" << hex << SubSection_ID << dec << "\n|\n";

                                //Advance to next subchunk (Current Position, + SubChunk length, - the header (6))
                                fseek ( File, SubSection_Length - 6, SEEK_CUR );

                                break;
                        }
                    }

                    CBrush *Brush = new CBrush(BrushName, BrushType, BrushPhysics, BrushDimentions);
					Brush->SetMaterial(BrushMaterial);
					Brush->SetMass(BrushMass);
					Brush->SetPosition(BrushPos);
					Brush->SetRotation(BrushRot);
					Brush->TextureOffset = UVOffset;
					Brush->TextureTile = UVTile;
					Brush->TextureRotation = UVRotate;
					Brush->CustomData = CustomData;

					AddGeometry(Brush);

                    //Move to end of subsection before returning
                    //(where we found the subsection, + section length, - the header (6))
                    fseek ( File, SubSection_Start + MainSection_Length, SEEK_SET );

                    break;
				}

				/*
				case S_SKYBOX:
                    //cout << "+--S_SKYBOX \tPosition:" << ftell ( File ) - 6 << "\tLength: " << MainSection_Length << "\n|  |\n";

					//Store where the subsection was found, so we can return later
                    SubSection_Start = ftell ( File ) - 6;

                    //Zero the subsection ID and length, in case they were used previously
                    SubSection_ID = 0;
                    SubSection_Length = 0;

                    //Loop from subsection Start to subsection End (Mainsection length, + the position it was found)
                    while ( ftell ( File ) < ( MainSection_Length + SubSection_Start ) )
                    {
                        //Read the subchunk ID
                        fread ( &SubSection_ID, 2, 1, File );

                        //Read the length of the subchunk
                        fread ( &SubSection_Length, 4, 1, File );

                        //Determine SubChunk type using its ID
                        switch ( SubSection_ID )
                        {
                            case S_SKY_TEXTURE:
							{
                                //cout << "|  +S_SKY_TEXTURE \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

								std::string Texture;
								int Face;

								fread ( &Face, 4, 1, File );

                                int CharCount = 0;
								char TempChar;
                                do
                                {
                                    fread ( &TempChar, 1, 1, File );
                                    Texture += TempChar;
                                    CharCount++;
                                }
                                while ( TempChar != '\0' );

                                //Remove '\0' or things get messy
                                Texture = Texture.substr(0, Texture.size() - 1);

								//Load texture
								Skybox.LoadSkyTexture(Face, Texture);

                                break;
							}

							default:
                                //cout << "|  +**UNKNOWN \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\tID: 0x" << hex << SubSection_ID << dec << "\n|\n";

                                //Advance to next subchunk (Current Position, + SubChunk length, - the header (6))
                                fseek ( File, SubSection_Length - 6, SEEK_CUR );

                                break;
						}

						//Move to end of subsection before returning
						//(where we found the subsection, + section length, - the header (6))
						fseek ( File, SubSection_Start + MainSection_Length, SEEK_SET );

						break;
					}
					*/

                default:
                    //cout << "+**UNKNOWN \tPosition:" << ftell ( File ) - 6 << "\tLength: " << MainSection_Length << "\tID: 0x" << hex << MainSection_ID << dec << "\n";

                    //Go to next section (Advance to the end of this section, minus 6)
                    fseek ( File, MainSection_Length - 6, SEEK_CUR );

                    break;
            }
        }

        //Close the file
        fclose(File);

        return true;
    }

} //Namespace
