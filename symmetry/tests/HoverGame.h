#include "../SYM_Engine.h"

class CHovercraft : public sym::CModel
{
	public:
		bool Create(std::string ModelFilename);

		//Movement
		void SetSpeed(float Speed);
		void SetSteerTorque(float Torque);

		//Tweakables
		void SetHoverHeight(float Height);
		void SetDamping(float Damp) { Damping = Damp; }
		void SetMass(float M);

	private:
		//Newton callbacks
		static void OnApplyForceAndTorque(const NewtonBody* Body, dFloat timestep, int threadIndex);
		static float OnRayCastHit(const NewtonBody* Body, const float* Normal, int CollisionID, void* UserData, float IntersetParam);
		float FindFloor(sym::CVector3 Centre, float MaxDist);

		float HoverHeight, Damping, Gravity, ThrusterForce, MaxHoverHeight, Steering, CurrentSpeed;

		sym::CVector3 Thrusters[6], GravityForce;

		//Up vector joint
		NewtonJoint* UpVectorJoint;
};

class CHoverGameState : public sym::CState
{
	public:
		void Init()
		{
			sym::CSprite Loading;
			Loading.Load("hcraft_loading.png");
			Loading.SetPosition(sym::CVector3(sym::Engine->ScreenWidth / 2, sym::Engine->ScreenHeight / 2, 0));

			sym::StartRendering();

			//Enable 2D rendering
			sym::glEnable2D();

				//Render sprites
				Loading.Render();

			//Disable 2D rendering
			sym::glDisable2D();

			sym::FinishRendering();

			CameraPitch = 0.0f;
			CameraYaw = 0.0f;
			CameraDistance = 200.0f;
			CameraHeight = 50.0f;
			PrevViewButton = false;
			BulletTimeButton = false;
			BulletTime = false;

			//ThirdPersonCam.Init();
			Scene.SetActiveCamera(&ThirdPersonCam);

			Floor = Scene.AddPlane("Floor", sym::LEVEL_COLLISION, sym::CVector3(5000, 5000, 5000));
			Floor->SetMaterial("floor.sym.material");
			Floor->TextureTile = sym::CVector3(20.0f, 20.0f, 20.0f);

			Ramp = Scene.AddPlane("Ramp", sym::LEVEL_COLLISION, sym::CVector3(500, 500, 500));
			Ramp->SetMaterial("panel.sym.material");
			Ramp->TextureTile = sym::CVector3(2.0f, 2.0f, 2.0f);
			Ramp->Pitch(10.0f);
			Ramp->SetPosition(sym::CVector3(0, 40, -400));

			for(int x = 0; x < 2; x++)
			{
				for(int y = 0; y < 3; y++)
				{
					sym::CBrush *Box = Scene.AddBox("", sym::PHYSICS_OBJ_BOX, sym::CVector3(50, 50, 50));
					Box->SetMaterial("panel.sym.material");
					Box->SetPosition(sym::CVector3(x * 50, 25 + (y * 50), 800));
					Box->SetMass(2.0f);
					//Box->SetAutoFreeze(false);
				}
			}

			for(int x = 0; x < 2; x++)
			{
				for(int y = 0; y < 3; y++)
				{
					sym::CBrush *Box = Scene.AddBox("", sym::PHYSICS_OBJ_BOX, sym::CVector3(50, 50, 50));
					Box->SetMaterial("crate.sym.material");
					Box->SetPosition(sym::CVector3(x * 50, 25 + (y * 50), -800));
					Box->SetMass(2.0f);
					//Box->SetAutoFreeze(false);
				}
			}

			MoveBox = Scene.AddBox("", sym::PHYSICS_OBJ_BOX, sym::CVector3(100, 100, 100));
			MoveBox->SetMaterial("crate.sym.material");
			MoveBox->SetPosition(sym::CVector3(200, 50, 200));
			MoveBox->SetMass(5.0f);
			
			if(!Hovercraft.Create("gunship.sym.mesh")) sym::Engine->Console.PrintLine("Could not create hovercraft");
			Hovercraft.SetPosition(sym::CVector3(0, 40, 0));

			Scene.AddGeometry(&Hovercraft);
			//Scene.ProcessScene();

			Scene.LoadSkyboxTextures("skyboxes\\reef\\reef_north.png", "skyboxes\\reef\\reef_south.png", "skyboxes\\reef\\reef_east.png", "skyboxes\\reef\\reef_west.png", "skyboxes\\reef\\reef_up.png", "skyboxes\\reef\\reef_down.png");
			Scene.ShowSkybox(true);

			//Set physics world size
			//sym::Engine->Physics.SetWorldSize(Scene.Octree.AABB);

			//Start physics simulation
			sym::Engine->Physics.Run();

			//Turn the lights on
			glEnable(GL_LIGHT0);

			//Scene.ShowCollision(true);
			//Scene.ShowOctree(true);

			Font.LoadTTFFont("arial", 12);

			//Music.Load("barracks.ogg", sym::SOUND_3D);
			Music.Play(sym::SOUND_LOOP);
			Music.SetPosition(MoveBox->GetPosition());
			Music.SetVolume(1.0f);
			Music.SetConeDegrees(90.0f, 360.0f, 1.0f);
			Music.SetMinMaxDistance(1000.0f, 10000.0f);
			
		}

		void Shutdown()
		{
			Scene.ClearScene();
			sym::Engine->Physics.Stop();
			Music.Stop();
		}

		void Pause()
		{
			sym::Engine->Physics.Stop();
			Music.Stop();
		}

		void Resume()
		{
			sym::Engine->Physics.Run();
			Music.Play(sym::SOUND_LOOP);
		}

		void HandleInput()
		{
			Hovercraft.SetSpeed(0.0f);
			Hovercraft.SetSteerTorque(0.0f);

			//Get mouse input
			MouseRelX = sym::Engine->Input.GetMouseRelativeX() * sym::Engine->Input.GetMouseResX();
			MouseRelY = sym::Engine->Input.GetMouseRelativeY() * sym::Engine->Input.GetMouseResY();

			if(sym::Engine->Input.JoystickAvailable())
			{
				if(sym::Engine->Input.GetNumJoystickAxis() > 1)
				{
					float Axis0 = (float)sym::Engine->Input.GetJoystickAxis(0) * 0.0005f;
					float Axis1 = (float)sym::Engine->Input.GetJoystickAxis(1) * 0.001f;
					if(Axis1 > 0.1f || Axis1 < -0.1f) Hovercraft.SetSpeed(-Axis1);
					if(Axis0 > 0.1f || Axis0 < -0.1f) Hovercraft.SetSteerTorque(-Axis0);
				}

				if(sym::Engine->Input.GetNumJoystickAxis() > 2)
				{
					float Axis2 = (float)sym::Engine->Input.GetJoystickAxis(4) * 0.00005f;
					float Axis4 = (float)sym::Engine->Input.GetJoystickAxis(3) * 0.00005f;
					if(Axis2 > 0.1f || Axis2 < -0.1f) MouseRelX = (Axis2);
					if(Axis4 > 0.1f || Axis4 < -0.1f) MouseRelY = (Axis4);
				}

				if(sym::Engine->Input.GetNumJoystickButtons() > 8)
				{
					if(sym::Engine->Input.GetJoystickButton(9) != 0)
					{
						if(!PrevViewButton)
						{
							CameraDistance += 50.0f;
							if(CameraDistance > 400.0f) CameraDistance = 100.0f;
							PrevViewButton = true;
						}
					}
					else PrevViewButton = false;

					if(sym::Engine->Input.GetJoystickButton(4) != 0)
					{
						if(!BulletTimeButton)
						{
							BulletTime = !BulletTime;
							if(BulletTime) sym::Engine->Physics.SetTimeStep(0.0025f);
							else sym::Engine->Physics.SetTimeStep(0.25f);
							BulletTimeButton = true;
						}
					}
					else BulletTimeButton = false;
				}
			}

			if(sym::Engine->Input.IsKeyPressed('f'))
			{
				BulletTime = !BulletTime;
				if(BulletTime) sym::Engine->Physics.SetTimeStep(0.0025f);
							else sym::Engine->Physics.SetTimeStep(0.25f);
			}
			

			if(sym::Engine->Input.IsKeyDown('w')) Hovercraft.SetSpeed(1.0f * sym::Engine->GetDeltaTime());
			if(sym::Engine->Input.IsKeyDown('s')) Hovercraft.SetSpeed(-1.0f * sym::Engine->GetDeltaTime());
			if(sym::Engine->Input.IsKeyDown('d')) Hovercraft.SetSteerTorque(-0.5f * sym::Engine->GetDeltaTime());
			if(sym::Engine->Input.IsKeyDown('a')) Hovercraft.SetSteerTorque(0.5f * sym::Engine->GetDeltaTime());

			//ESC key
			if(sym::Engine->Input.IsKeyPressed(SDLK_ESCAPE)) sym::Engine->StateManager.PopState();
		}

		void Update()
		{

			//Set the cam matrix
			ThirdPersonCam.SetMatrix(Hovercraft.GetMatrix());

			//Apply mouse pitch/yaw to camera
			CameraPitch += MouseRelY;
			CameraYaw += MouseRelX;
			
			ThirdPersonCam.Yaw(-CameraYaw);
			ThirdPersonCam.Pitch(-CameraPitch);

			//Pull back to camera distance
			ThirdPersonCam.Move(-CameraDistance);

			//Move camera up
			ThirdPersonCam.Fly(CameraHeight);

			sym::Engine->Audio.SetListenerPosition(ThirdPersonCam.GetPosition(), ThirdPersonCam.GetForward(), ThirdPersonCam.GetUp());
			
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

				//sym::glEnable2D();


				//sym::glDisable2D();
			
			sym::FinishRendering();
			
		}

	private:
		sym::CScene Scene;
		sym::CCamera ThirdPersonCam;
		sym::CBrush *Floor;
		sym::CBrush *Ramp;
		sym::CBrush *MoveBox;
		CHovercraft Hovercraft;

		sym::CFont Font;

		float CameraDistance;
		float CameraHeight;
		float CameraPitch;
		float CameraYaw;
		float MouseRelX, MouseRelY;

		sym::CSound Music;

		bool PrevViewButton;
		bool BulletTimeButton;
		bool BulletTime;
};