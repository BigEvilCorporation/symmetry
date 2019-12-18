#include "../SYM_Engine.h"

class CVisionTestState : public sym::CState
{
	public:
		void Init()
		{
			//Load font for loading screen
			Font.LoadTTFFont("arial.ttf", 24);

			//Start drawing loading screen
			sym::StartRendering();

				//Enable 2D rendering
				sym::glEnable2D();

					//Render loading text
					Font.RenderText("[ Loading Vision Modes Demo ]", 
									sym::Engine->ScreenWidth / 2 - Font.GetTextWidth("[ Loading Vision Modes Demo ]") / 2, 
									sym::Engine->ScreenHeight / 2);

				//Disable 2D rendering
				sym::glDisable2D();

			//Finish drawing loading screen
			sym::FinishRendering();

			CameraPitch = 0.0f;
			CameraYaw = 0.0f;
			CameraDistance = 200.0f;
			CameraHeight = 30.0f;

			BallSpeed = 2.0f;

			BulletTime = false;

			EnhancedVision = VisionButton = false;

			//Camera.Init();
			Scene.SetActiveCamera(&Camera);

			Ball = Scene.AddModel("", "ball.sym.mesh", sym::PHYSICS_OBJ_SPHERE);
			Ball->SetPosition(sym::CVector3(0, 20, 0));
			Ball->SetMass(5.0f);

			sym::CBrush *Floor = Scene.AddPlane("Floor", sym::LEVEL_COLLISION, sym::CVector3(5000, 5000, 5000));
			Floor->SetMaterial("floor.sym.material");
			Floor->TextureTile = sym::CVector3(20.0f, 20.0f, 20.0f);

			//Ad some real geometry
			for(int x = 0; x < 2; x++)
			{
				for(int y = 0; y < 2; y++)
				{
					sym::CBrush *Box = Scene.AddBox("", sym::PHYSICS_OBJ_BOX, sym::CVector3(20, 20, 20));
					Box->SetMaterial("panel.sym.material");
					Box->SetPosition(sym::CVector3(x * 20, 10 + (y * 20), -800));
					Box->SetMass(2.0f);
					Box->SetAutoFreeze(false);
				}
			}

			//Vision mode materials
			sym::CMaterial *FakeMaterial = new sym::CMaterial();
			FakeMaterial->LoadTexture("vm_fake.png");

			sym::CMaterial *RealMaterial = new sym::CMaterial();
			RealMaterial->LoadTexture("vm_bioeye.png");

			sym::CMaterial *EnhancedMaterial = new sym::CMaterial();
			EnhancedMaterial->LoadTexture("vm_enhanced.png");

			//Add some fake geometry
			sym::CBrush *FakeBox = new sym::CBrush("FakeBox", sym::SYM_BOX, sym::NO_PHYSICS, sym::CVector3(50, 50, 50));
			FakeBox->SetMaterial(FakeMaterial);
			FakeBox->Roll(180.0f);
			FakeBox->SetPosition(sym::CVector3(100, 25, -100));
			FakeGeometry.push_back(FakeBox);

			/*
			sym::CBrush *FakeWall = new sym::CBrush("FakeWall", sym::SYM_PLANE, sym::NO_PHYSICS, sym::CVector3(200, 200, 200));
			FakeWall->SetMaterial(FakeMaterial);
			FakeWall->Pitch(-90.0f);
			FakeWall->SetPosition(sym::CVector3(1000, 100, -1000));
			//FakeGeometry.push_back(FakeWall);
			*/

			//Add some real (normal) geometry
			sym::CBrush *RealBox = new sym::CBrush("RealBox", sym::SYM_BOX, sym::PHYSICS_OBJ_BOX, sym::CVector3(50, 50, 50));
			RealBox->SetMaterial(RealMaterial);
			RealBox->Roll(180.0f);
			RealBox->SetPosition(sym::CVector3(0, 25, -100));
			VMToHide.push_back(RealBox);

			/*
			sym::CBrush *RealWall = new sym::CBrush("RealWall", sym::SYM_PLANE, sym::LEVEL_COLLISION, sym::CVector3(200, 200, 200));
			RealWall->SetMaterial(RealMaterial);
			RealWall->Pitch(-90.0f);
			RealWall->SetPosition(sym::CVector3(0, 25, -100));
			//VMToHide.push_back(RealWall);
			*/

			//Add some real (enhanced) geometry
			sym::CBrush *EnhancedBox = new sym::CBrush("FakeBox", sym::SYM_BOX, sym::PHYSICS_OBJ_BOX, sym::CVector3(50, 50, 50));
			EnhancedBox->SetMaterial(EnhancedMaterial);
			EnhancedBox->Roll(180.0f);
			EnhancedBox->SetPosition(sym::CVector3(-100, 25, -100));
			VMToShow.push_back(EnhancedBox);

			/*
			sym::CBrush *EnhancedWall = new sym::CBrush("EnhancedWall", sym::SYM_PLANE, sym::LEVEL_COLLISION, sym::CVector3(200, 200, 200));
			EnhancedWall->SetMaterial(EnhancedMaterial);
			EnhancedWall->Pitch(-90.0f);
			EnhancedWall->SetPosition(sym::CVector3(-1000, 25, -1000));
			//VMToShow.push_back(EnhancedWall);
			*/

			Scene.ProcessScene();

			Scene.LoadSkyboxTextures("skyboxes\\reef\\reef_north.png", "skyboxes\\reef\\reef_south.png", "skyboxes\\reef\\reef_east.png", "skyboxes\\reef\\reef_west.png", "skyboxes\\reef\\reef_up.png", "skyboxes\\reef\\reef_down.png");
			Scene.ShowSkybox(true);

			//Start physics simulation
			sym::Engine->Physics.Run();

			//Turn the lights on
			glEnable(GL_LIGHT0);

			//Scene.ShowCollision(true);
			//Scene.ShowOctree(true);
			
		}

		void Shutdown()
		{
			Scene.ClearScene();
			sym::Engine->Physics.Stop();
		}

		void Pause()
		{
			sym::Engine->Physics.Stop();
		}

		void Resume()
		{
			sym::Engine->Physics.Run();
		}

		void HandleInput()
		{
			MouseRelX = MouseRelY = 0.0f;

			if(sym::Engine->Input.JoystickAvailable())
			{
				if(sym::Engine->Input.GetNumJoystickAxis() > 1)
				{
					float Axis0 = (float)sym::Engine->Input.GetJoystickAxis(0) * 0.0005f;
					float Axis1 = (float)sym::Engine->Input.GetJoystickAxis(1) * 0.0005f;
					if(Axis1 > 0.1f || Axis1 < -0.1f) Ball->AddForce(sym::CVector3(0, 0, 1) * (Ball->Mass * BallSpeed * Axis1));
					if(Axis0 > 0.1f || Axis0 < -0.1f) Ball->AddForce(sym::CVector3(1, 0, 0) * (Ball->Mass * BallSpeed * Axis0));
				}

				if(sym::Engine->Input.GetNumJoystickAxis() > 2)
				{
					float Axis2 = (float)sym::Engine->Input.GetJoystickAxis(4) * 0.00001f;
					float Axis4 = (float)sym::Engine->Input.GetJoystickAxis(3) * 0.00001f;
					if(Axis2 > 0.1f || Axis2 < -0.1f) MouseRelX = (-Axis2);
					if(Axis4 > 0.1f || Axis4 < -0.1f) MouseRelY = (-Axis4);
				}

				if(sym::Engine->Input.GetNumJoystickButtons() > 7)
				{
					if(sym::Engine->Input.GetJoystickButton(9) != 0)
					{
						if(!ResetButton)
						{
							Ball->SetPosition(sym::CVector3(0, 20, 0));
							ResetButton = true;
						}
					}
					else ResetButton = false;

					if(sym::Engine->Input.GetJoystickButton(5) != 0)
					{
						if(!VisionButton)
						{
							EnhancedVision = !EnhancedVision;
							VisionButton = true;
						}
					}
					else VisionButton = false;
				}
			}

			//Get mouse input
			//MouseRelX = sym::Engine->Input.GetMouseRelativeX() * sym::Engine->Input.GetMouseResX();
			//MouseRelY = sym::Engine->Input.GetMouseRelativeY() * sym::Engine->Input.GetMouseResY();

			//if(sym::Engine->Input.IsKeyDown('w')) Hovercraft.SetSpeed(5.0f * sym::Engine->GetDeltaTime());
			//if(sym::Engine->Input.IsKeyDown('s')) Hovercraft.SetSpeed(-3.0f * sym::Engine->GetDeltaTime());
			//if(sym::Engine->Input.IsKeyDown('d')) Hovercraft.SetSteerTorque(-2.0f * sym::Engine->GetDeltaTime());
			//if(sym::Engine->Input.IsKeyDown('a')) Hovercraft.SetSteerTorque(2.0f * sym::Engine->GetDeltaTime());

			if(sym::Engine->Input.IsKeyPressed('b'))
			{
				BulletTime = !BulletTime;
				if(BulletTime) sym::Engine->Physics.SetTimeStep(0.05);
				else  sym::Engine->Physics.SetTimeStep(0.25);
			}

			//ESC key
			if(sym::Engine->Input.IsKeyPressed(SDLK_ESCAPE)) sym::Engine->StateManager.PopState();
		}

		void Update()
		{
			//Set the cam matrix
			//Camera.SetMatrix(Ball->GetMatrix());
			Camera.SetPosition(Ball->GetPosition());

			//Apply mouse pitch/yaw to camera
			CameraPitch = MouseRelY;
			CameraYaw = Ball->GetRotation().y;
			
			//Camera.Yaw(-CameraYaw);
			//Camera.Pitch(-CameraPitch);

			//Pull back to camera distance
			Camera.Move(-CameraDistance);

			//Move camera up
			Camera.Fly(CameraHeight);
		}

		void Render()
		{
			//Light colours
			GLfloat ambientLight[] = { 0.8f, 0.8f, 0.8f, 1.0f };
			GLfloat diffuseLight[] = { 0.8f, 0.8f, 0.8f, 1.0f };
			GLfloat specularLight[] = { 0.5f, 0.5f, 0.5f, 1.0f };
			GLfloat position[] = { 100.0f, 100.0f, 100.0f, 1.0f };

			//Position lights
			glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
			glLightfv(GL_LIGHT0, GL_POSITION, position);
			glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
			glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
			
			sym::StartRendering();

				//Render scene
				Scene.Render();

				if(EnhancedVision)
				{
					for(int i = 0; i < VMToShow.size(); i++)
					{
						VMToShow[i]->Render();
					}
				}
				else
				{
					for(int i = 0; i < FakeGeometry.size(); i++)
					{
						FakeGeometry[i]->Render();
					}

					for(int i = 0; i < VMToHide.size(); i++)
					{
						VMToHide[i]->Render();
					}
				}

				sym::glEnable2D();

					if(EnhancedVision) Font.RenderText("Enhanced Reality Vision mode", 20, sym::Engine->ScreenHeight - 30);
					else Font.RenderText("Bionic Eye (Normal) Vision mode", 20, sym::Engine->ScreenHeight - 30);

				sym::glDisable2D();
			
			sym::FinishRendering();
			
		}

	private:
		sym::CScene Scene;
		sym::CCamera Camera;

		sym::CModel *Ball;

		sym::CFont Font;

		float CameraDistance;
		float CameraHeight;
		float CameraPitch;
		float CameraYaw;
		float MouseRelX, MouseRelY;

		float BallSpeed;

		bool ResetButton;

		bool BulletTime;

		bool EnhancedVision, VisionButton;

		std::vector<sym::CGeometry*> FakeGeometry;
		std::vector<sym::CGeometry*> VMToShow;
		std::vector<sym::CGeometry*> VMToHide;
};