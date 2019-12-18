#include "Level.h"
#include "Game.h"

#include "WeaponTurretLauncher.h"
#include "WeaponTurretGun.h"
#include "WeaponGrappleGun.h"
#include "WeaponRocketLauncher.h"
#include "WeaponLaser.h"

CLevelState::CLevelState()
{
	BulletTime = false;
	EnhancedVision = false;
	VisibleAlpha = 0.95f;
	InvisibleAlpha = 0.05f;
}

void CLevelState::Init()
{
	//Setup camera and mouse
	MouseRelX = MouseRelY = 0.0f;
	CameraDistance = 100.0f;
	CameraHeight = 25.0f;
	CameraPitch = 0.0f;
	CameraYawOffset = 0.0f;
	LMB = RMB = false;
	Jump = false;

	RunSpeed = 50.0f;
	TurnSpeed= 0.3f;
	SidestepSpeed = 75.0f;
	PlayerHeading = 0.0f;

	PostShader = 0;
	BloomLevels[0] = 0.0f;
	BloomLevels[1] = 0.004f;
	BloomLevels[2] = 0.008f;
	BloomLevels[3] = 0.1f;
	BloomIndex = 0;

	BulletTime = false;

	//Initialise scene
	Scene.Init();

	//Initialise framebuffer
	Framebuffer.Init(sym::Engine->ScreenWidth, sym::Engine->ScreenHeight, true);

	//Create RTT rendering display list
	QuadDisplayList = glGenLists(1);

	glNewList(QuadDisplayList, GL_COMPILE);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f,								(float)sym::Engine->ScreenHeight);
		glTexCoord2f(1.0f, 0.0f); glVertex2f((float)sym::Engine->ScreenWidth,	(float)sym::Engine->ScreenHeight);
		glTexCoord2f(1.0f, 1.0f); glVertex2f((float)sym::Engine->ScreenWidth,	0.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f,								0.0f);
	glEndList();

	//Load post-process shaders
	//PostShader = Scene.LoadPostShader("post_edge_toon.cgfx");
	CelShader = sym::Engine->ResourceManager.Shaders.Get("post_edge_toon.cgfx");

	if(CelShader)
	{
		CelShader->SetTextureParam("SceneSampler", Framebuffer.GetColourTarget(0));
		CelShader->SetTextureParam("DepthSampler", Framebuffer.GetDepthTarget());
	}

	if(PostShader)
	{
		BloomParam = cgGetNamedEffectParameter(PostShader->GetEffect(), "Magnitude");
		cgSetParameter1fv(BloomParam, &BloomLevels[0]);
	}

	Scene.SetActiveCamera(&Camera);
	Camera.SetPosition(sym::CVector3(0.0f, 10.0f, 0.0f));

	//Load font
	Font.LoadTTFFont("arial.ttf", 12);

	//Init HUD
	Hud.Init();

	//Initialise player character
	if(!Player.Init())
	{
		Game->Log.AddEntry("Error: Could not initialise player character");

		//TODO: State init failures
	}

	//Set initial position (TODO: Spawn points)
	Player.SetPosition(sym::CVector3(0, Player.GetLocalAABB().Size.y / 2, 0));

	//Add player to scene manager
	Scene.AddGeometry(&Player);

	//Don't let mouse leave window
	sym::Engine->Input.StealMouse();

	//Start physics simulation
	sym::Engine->Physics.Run();

	//Turn the lights on
	glEnable(GL_LIGHT0);
}

void CLevelState::Shutdown()
{
	AmbientMusic.Stop();

	//Shutdown scene
	Scene.Shutdown();
}

void CLevelState::Pause()
{
	AmbientMusic.Stop();
}

void CLevelState::Resume()
{
	//Don't let mouse leave window
	sym::Engine->Input.StealMouse();
}

void CLevelState::HandleInput()
{
	MouseRelX = sym::Engine->Input.GetMouseRelativeX() * sym::Engine->Input.GetMouseResolution() * sym::Engine->GetDeltaTime() * 0.1f;
	MouseRelY = sym::Engine->Input.GetMouseRelativeY() * sym::Engine->Input.GetMouseResolution() * sym::Engine->GetDeltaTime() * 0.1f;

	float Axis0 = (float)sym::Engine->Input.GetJoystickAxis(0) * sym::Engine->Input.GetJoystickSensitivity();
	float Axis1 = (float)sym::Engine->Input.GetJoystickAxis(1) * sym::Engine->Input.GetJoystickSensitivity();
	//if(Axis1 > 1.0f) Player.AddMovementVector(Player.GetForward() - (Player.GetForward() * 2) * Axis1);
	//if(Axis1 < -1.0f) Player.AddMovementVector(Player.GetForward() * -Axis1);
	//if(Axis0 > 1.0f) Player.AddMovementVector(Player.GetRight() * Axis0);
	//if(Axis0 < -1.0f) Player.AddMovementVector(Player.GetRight() - (Player.GetRight() * 2) * -Axis0);

	float Axis2 = (float)sym::Engine->Input.GetJoystickAxis(4) * sym::Engine->Input.GetJoystickSensitivity();
	float Axis3 = (float)sym::Engine->Input.GetJoystickAxis(3) * sym::Engine->Input.GetJoystickSensitivity();
	if(Axis3 > 1.0f) MouseRelY = -Axis3;
	if(Axis3 < -1.0f) MouseRelY =  -Axis3;
	if(Axis2 > 1.0f) MouseRelX =  Axis2;
	if(Axis2 < -1.0f) MouseRelX =  Axis2;
	

	//static sym::CPlaceable Placeable;
	//Placeable.Pitch(MouseRelY * 100.0f);

	//Player.AimBone->UserMatrix = Placeable.GetMatrix();

	PlayerHeading += MouseRelX * TurnSpeed;
	if(PlayerHeading > 360.0f) PlayerHeading = 0.0f;
	if(PlayerHeading < 0.0f) PlayerHeading = 360.0f;

	float Forward = 0.0f;
	float Side = 0.0f;

	sym::CVector3 MovementVector;

	//Find floor
	sym::CVector3 RayStart = Player.GetPosition();
	sym::CVector3 RayEnd = RayStart - sym::CVector3(0.0f, (Player.GetLocalAABB().Size.y / 2.0f) + 5.0f, 0.0f);
	sym::RaycastData Data = sym::Engine->Physics.Raycast(RayStart, RayEnd);

	/*
	if(!Data.Geometry)
	{
		Player.Mesh->BindAnimation(&Player.JumpAnim);
		Player.Mesh->SetAnimSpeed(0.001f);
		Player.Mesh->ResetAnimation();
		Player.Mesh->SetAnimationLooping(false);
	}
	else
	*/
	{
		Player.Mesh->BindAnimation(&Player.IdleAnim);
		Player.Mesh->SetAnimSpeed(0.01f);
		Player.Mesh->SetAnimationLooping(true);

		if(sym::Engine->Input.IsKeyDown('w'))
		{
			MovementVector += Player.GetForward();
			Player.Mesh->BindAnimation(&Player.WalkAnim);
			Player.Mesh->SetAnimSpeed(0.01f);
			Player.Mesh->SetAnimationLooping(true);
		}

		if(sym::Engine->Input.IsKeyDown('s'))
		{
			MovementVector += Player.GetForward() - (Player.GetForward() * 2);
			Player.Mesh->BindAnimation(&Player.WalkBackAnim);
			Player.Mesh->SetAnimSpeed(0.005f);
			Player.Mesh->SetAnimationLooping(true);
		}

		if(sym::Engine->Input.IsKeyDown('d'))
		{
			MovementVector += Player.GetRight();
			Player.Mesh->BindAnimation(&Player.StepRightAnim);
			Player.Mesh->SetAnimSpeed(0.01f);
			Player.Mesh->SetAnimationLooping(true);
		}

		if(sym::Engine->Input.IsKeyDown('a'))
		{
			MovementVector += Player.GetRight() - (Player.GetRight() * 2);
			Player.Mesh->BindAnimation(&Player.StepLeftAnim);
			Player.Mesh->SetAnimSpeed(0.01f);
			Player.Mesh->SetAnimationLooping(true);
		}
	}

	if(sym::Engine->Input.IsKeyPressed(SDLK_SPACE))
	{
		Player.Jump(30.0f);
	}

	//If weapon select button is down
	if(sym::Engine->Input.IsMMBDown())
	{
		//Select weapon using mouse direction
		if(MouseRelY > 1.0f)
		{
			Hud.SelectWeapon(0);
			//Player.SelectWeapon1("MiniGun");
		}

		if(MouseRelX > 1.0f)
		{
			Hud.SelectWeapon(1);
			Player.SelectWeapon1("Laser");
		}

		if(MouseRelY < -1.0f)
		{
			Hud.SelectWeapon(2);
			Player.SelectWeapon1("TurretGun");
		}

		if(MouseRelX < -1.0f)
		{
			Hud.SelectWeapon(3);
			Player.SelectWeapon1("RocketLauncher");
		}
	}
	else Hud.CloseWeaponSelector();
	
	if(!Hud.IsOpen()) Player.SetHeading(PlayerHeading);
	Player.SetMovementVector(MovementVector);

	if(sym::Engine->Input.IsKeyPressed('z')) Camera.Shake(sym::CVector3(0.0f, 1.0f, 0.0f), 0.002f, 0.5f);
	if(sym::Engine->Input.IsKeyPressed('x')) Camera.Shake(sym::CVector3(0.0f, 1.0f, 0.0f), 0.004f, 0.5f);
	if(sym::Engine->Input.IsKeyPressed('c')) Camera.Shake(sym::CVector3(0.0f, 1.0f, 0.0f), 0.01f, 1.0f);

	if(sym::Engine->Input.GetJoystickButton(0))
	{
		if(!Jump)
		{
			Player.Jump(1500.0f);
			Jump = true;
		}
	}
	else Jump = false;

	if(sym::Engine->Input.IsLMBDown() || (float)sym::Engine->Input.GetJoystickAxis(2) > 5.0f)
	{
		if(!LMB)
		{
			Player.ShootWeapon1(FIRE_PRIMARY);
			LMB = true;
		}
	}
	else LMB = false;

	if(sym::Engine->Input.IsRMBDown() || (float)sym::Engine->Input.GetJoystickAxis(2) < -5.0f)
	{
		if(!RMB)
		{
			Player.ShootWeapon2(FIRE_PRIMARY);
			RMB = true;
		}
	}
	else RMB = false;

	if(sym::Engine->Input.IsKeyPressed('f'))
	{
		BulletTime = !BulletTime;
		if(BulletTime) sym::Engine->Physics.SetTimeStep(0.0025f);
					else sym::Engine->Physics.SetTimeStep(0.25f);
	}

	if(sym::Engine->Input.IsKeyPressed('e'))
	{
		if(PostShader)
		{
			BloomIndex++;
			if(BloomIndex > 4) BloomIndex = 0;
			cgSetParameter1fv(BloomParam, &BloomLevels[BloomIndex]);
		}
	}

	if(sym::Engine->Input.IsKeyPressed(SDLK_1)) SetVisionMode(0);
	if(sym::Engine->Input.IsKeyPressed(SDLK_2)) SetVisionMode(1);

	if(sym::Engine->Input.IsKeyPressed(SDLK_ESCAPE)) Game->CloseLevel();
}

void CLevelState::Update()
{
	//Apply torque to character
	sym::CPlaceable *Char = static_cast<sym::CPlaceable*>(&Player);
	//Char->Yaw(-MouseRelX);

	//Set the cam matrix
	Camera.SetMatrix(Player.GetMatrix());

	//Apply mouse pitch to camera
	CameraPitch += MouseRelY;
	if(!Hud.IsOpen()) Camera.Pitch(-CameraPitch);

	//Pull back to camera distance
	Camera.Move(-CameraDistance);

	//Move camera up
	Camera.Fly(CameraHeight);

	//Raycast from player to camera
	sym::RaycastData HitData = sym::Engine->Physics.Raycast(Char->GetPosition(), Camera.GetPosition());

	//If geometry is hit
	if(HitData.Geometry)
	{
		//Position camera at hit position, lerped back a little
		Camera.SetPosition(Char->GetPosition().Lerp(HitData.HitPosition, 0.9f));
	}

	//Set player aiming vector
	sym::CPlaceable Aiming;
	Aiming.SetMatrix(Player.GetMatrix());
	Aiming.Pitch(-CameraPitch + (90.0f * sym::Maths::DEGREES_TO_RADIANS));
	Player.SetAimVector(Aiming.GetForward());

	//Update all spawned objects
	for(int i = 0; i < (int)SpawnedObjects.size(); i++)
	{
		if(SpawnedObjects[i]) SpawnedObjects[i]->OnUpdate();
	}

	//Update all enemies
	for(int i = 0; i < (int)Enemies.size(); i++)
	{
		if(Enemies[i]) Enemies[i]->Update();
	}

	//Update camera
	Camera.Update();

	//Update HUD
	Hud.Update();
}

void CLevelState::Render()
{
	//Light colours
	GLfloat ambientLight[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat diffuseLight[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	GLfloat specularLight[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat position[] = { 100.0f, 100.0f, 100.0f, 1.0f };

	//Position lights
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);

	//Start rendering
	sym::StartRendering();

		//Bind framebuffer
		//Framebuffer.Bind();

		//Clear
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Render scene
		Scene.Render();

		//Call render for all spawned objects
		for(int i = 0; i < (int)SpawnedObjects.size(); i++)
		{
			if(SpawnedObjects[i]) SpawnedObjects[i]->OnRender();
		}

		//Call render for all enemies
		for(int i = 0; i < (int)Enemies.size(); i++)
		{
			if(Enemies[i]) Enemies[i]->Render();
		}

		//Call post render
		OnRender();

		//Finished with framebuffer
		//Framebuffer.Unbind();

		//Start 2D drawing
		sym::glEnable2D();

		/*
			//Get the first render pass
			CGpass Pass = CelShader->GetFirstPass();

			//Loop until all passes have finished
			while(Pass)
			{
				//Set current pass
				CelShader->SetPass(Pass);

				//Render framebuffer
				glBegin(GL_QUADS);
					glCallList(QuadDisplayList);
				glEnd();

				//Reset current pass
				CelShader->ResetPass(Pass);

				//Get the next pass
				Pass = CelShader->GetNextPass(Pass);
			}
		*/

			//Call overlay render
			OnRenderOverlay();

			//Render HUD
			Hud.Render();

			if(Game->GetDebugMode())
			{
				std::stringstream Stream;
				Stream << "Frames per second: " << sym::GetFPS();
				Font.RenderText(Stream.str(), 20.0f, (float)sym::Engine->ScreenHeight - 20.0f);
			}

		//Finish 2D rendering
		sym::glDisable2D();

	//Finish rendering
	sym::FinishRendering();
}

void CLevelState::OnRender()
{
}

void CLevelState::OnRenderOverlay()
{
	if(EnhancedVision) Font.RenderText("Enhanced Reality Vision mode", 20, sym::Engine->ScreenHeight - 30);
	else Font.RenderText("Bionic Eye (Normal) Vision mode", 20, sym::Engine->ScreenHeight - 30);
}

bool CLevelState::LoadScene(std::string Filename)
{
	//Show loading screen
	sym::CSprite Loading;
	Loading.Load("LoadingScreen.jpg");
	Loading.SetPosition(sym::CVector3((float)sym::Engine->ScreenWidth / 2, (float)sym::Engine->ScreenHeight / 2, 0.0f));

	sym::StartRendering();
	sym::glEnable2D();
	Loading.Render();
	sym::glDisable2D();
	sym::FinishRendering();

	//Load scene file
	if(!Scene.Load(Filename)) return false;

	//List of geometry for removal
	std::vector<sym::CGeometry*> GeomToDelete;

	//Create lists of custom level geometry (grappleable objects and vision mode geometry)
	for(std::map<std::string, sym::CGeometry*>::iterator i = Scene.Geometry.begin(); i != Scene.Geometry.end(); i++)
	{
		//Grapple points
		if((GRAPPLE1 & (*i).second->CustomData) == GRAPPLE1)
		{
			GrappleObjects.push_back((*i).second);
		}

		//Vision modes
		if((VISION1 & (*i).second->CustomData) == VISION1)
		{
			VisionObjects1.push_back((*i).second);
		}

		if((VISION2 & (*i).second->CustomData) == VISION2)
		{
			VisionObjects2.push_back((*i).second);

			//Turn off collision
			(*i).second->DestroyNewtonBody();
		}

		//Enemy spawn points
		if((SPAWN & (*i).second->CustomData) == SPAWN)
		{
			//Create new enemy
			CEnemyBase *Enemy = new CEnemyBase();
			Enemy->Init(&Player);

			//Set position/rotation
			Enemy->SetPosition((*i).second->GetPosition());
			Enemy->SetRotation((*i).second->GetRotation());

			//Give weapon

			//Add to enemies list
			Enemies.push_back(Enemy);

			//Mark for deletion
			GeomToDelete.push_back((*i).second);
		}
	}

	for(int i = 0; i < (int)GeomToDelete.size(); i++)
	{
		Scene.DeleteGeometry(GeomToDelete[i]);
	}

	std::stringstream Stream;
	Stream << Filename << "\tGrapple geometry: " << GrappleObjects.size() << "\tCloaked geometry: " << VisionObjects1.size() << "\tFake geometry: " << VisionObjects2.size();
	Game->Log.AddEntry(Stream.str());

	//Set initial vision mode
	SetVisionMode(0);

	//Load skydome assets
	SkyDome.Load("SkyDome", "skydome.sym.mesh", sym::NO_PHYSICS, false);
	SkyShader.Load("skydome_multilayer_glitchy.cgfx");

	//Setup skydome shader and material
	SkyTextureExternal.Load("skydome_external.png");
	SkyTextureInternal1.Load("skydome_internal1.png");
	SkyTextureInternal2.Load("skydome_internal2.png");

	SkyShader.SetTextureParam("ExternalSkySampler", &SkyTextureExternal);
	SkyShader.SetTextureParam("InternalSky1Sampler", &SkyTextureInternal1);
	SkyShader.SetTextureParam("InternalSky2Sampler", &SkyTextureInternal2);
	SkyMaterial.SetShader(&SkyShader);
	SkyDome.SetMaterial(&SkyMaterial);

	//Add skydome to scene skyboxes
	Scene.Skyboxes.push_back(&SkyDome);

	//Spawn weapons
	CGrappleGun *GrappleGun = Spawn<CGrappleGun>(sym::CVector3(), sym::CQuaternion());
	GrappleGun->SetOwner(&Player);
	GrappleGun->SetMaxLength(600.0f);
	Player.AddInventoryItem((CInventoryItemBase*)GrappleGun);

	CRocketLauncher *RocketLauncher = Spawn<CRocketLauncher>(sym::CVector3(), sym::CQuaternion());
	RocketLauncher->SetOwner(&Player);
	Player.AddInventoryItem((CInventoryItemBase*)RocketLauncher);

	std::vector<sym::CGeometry*> EnemyGeoms;
	for(int i = 0; i < (int)Enemies.size(); i++) EnemyGeoms.push_back((sym::CGeometry*) Enemies[i]);

	CTurretGun *TurretLauncher = Spawn<CTurretGun>(sym::CVector3(), sym::CQuaternion());
	TurretLauncher->SetOwner(&Player);
	TurretLauncher->SetTargets(EnemyGeoms);
	Player.AddInventoryItem((CInventoryItemBase*)TurretLauncher);

	CLaser *Laser = Spawn<CLaser>(sym::CVector3(), sym::CQuaternion());
	Laser->SetOwner(&Player);
	Player.AddInventoryItem((CInventoryItemBase*)Laser);

	//Set player weapons
	Player.SelectWeapon1("TurretGun");
	Player.SelectWeapon2("GrappleGun");

	//Send grapple list to grapple gun
	GrappleGun->SetGrappleObjectList(GrappleObjects);

	//load music
	if(Filename == "level1.sym.scene") AmbientMusic.Load("ambience3.ogg");
	else if(Filename == "level2.sym.scene") AmbientMusic.Load("level1.ogg");
	else if(Filename == "level3.sym.scene") AmbientMusic.Load("ambience2.ogg");
	else if(Filename == "level4.sym.scene") AmbientMusic.Load("ambience.ogg");

	//Play music
	AmbientMusic.Play();

	//Reset all input
	sym::Engine->Input.Reset();

	return true;
}

sym::CScene *CLevelState::GetScene()
{
	return &Scene;
}

void CLevelState::DestroySpawnedObject(CSpawnable *Object)
{
	//Find object in list
	for(std::vector<CSpawnable*>::iterator i = SpawnedObjects.begin(); i != SpawnedObjects.end();)
	{
		if((*i) == Object)
		{
			//Found, call OnDestroy
			Object->OnDestroy();

			//Destroy physics body
			Object->DestroyNewtonBody();
			
			//Remove from scene manager
			Scene.DeleteGeometry(Object);

			//Remove from spawned objects list
			i = SpawnedObjects.erase(i);

			//Call destructor
			delete Object;
			Object = 0;

			//Stop searching
			return;
		}

		 ++i;
	}

	//We should never reach here
	Game->Log.AddEntry("CRITICAL: Cannot destroy spawnable " + Object->Name + ", not found in spawn list");
}

void CLevelState::AddEnemy(CEnemyBase* Enemy)
{
	Enemies.push_back(Enemy);
}

std::vector<CSpawnable*> *CLevelState::GetSpawnedObjects()
{
	return &SpawnedObjects;
}

CCharacterPlayer *CLevelState::GetPlayer()
{
	return &Player;
}

void CLevelState::SetVisionMode(int Mode)
{
	//Loop through 'cloaked' objects
	for(int i = 0; i < VisionObjects1.size(); i++)
	{
		for(int j = 0; j < VisionObjects1[i]->Mesh->Subsets.size(); j++)
		{
			if(VisionObjects1[i]->Mesh->Subsets[j].Material)
			{
				if(Mode == 0)
				{
					VisionObjects1[i]->Mesh->Subsets[j].Material->SetTransparency(InvisibleAlpha);
				}
				else VisionObjects1[i]->Mesh->Subsets[j].Material->SetTransparency(VisibleAlpha);
			}
		}
	}

	//Loop through 'fake' objects
	for(int i = 0; i < VisionObjects2.size(); i++)
	{
		for(int j = 0; j < VisionObjects2[i]->Mesh->Subsets.size(); j++)
		{
			if(VisionObjects2[i]->Mesh->Subsets[j].Material)
			{
				if(Mode == 0)
				{
					VisionObjects2[i]->Mesh->Subsets[j].Material->SetTransparency(VisibleAlpha);
				}
				else VisionObjects2[i]->Mesh->Subsets[j].Material->SetTransparency(InvisibleAlpha);
			}
		}
	}

	EnhancedVision = (bool)Mode;
}