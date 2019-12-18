#include <sstream>

#include "../SYM_Engine.h"
#include "../SYM_Character.h"
#include "../SYM_Rope.h"

class CCharTestState : public sym::CState
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
					Font.RenderText("[ Loading Character Demo ]", 
									sym::Engine->ScreenWidth / 2 - Font.GetTextWidth("[ Loading Character Demo ]") / 2, 
									sym::Engine->ScreenHeight / 2);

				//Disable 2D rendering
				sym::glDisable2D();

			//Finish drawing loading screen
			sym::FinishRendering();

			CursorHover = new sym::CBrush("Hover", sym::SYM_PLANE, sym::NO_PHYSICS, sym::CVector3(20, 20, 20));
			CursorHover->SetMaterial("crosshair_hover.sym.material");

			CursorTarget = new sym::CBrush("Target", sym::SYM_PLANE, sym::NO_PHYSICS, sym::CVector3(20, 20, 20));
			CursorTarget->SetMaterial("crosshair_target.sym.material");

			CursorLockOn = new sym::CBrush("Target", sym::SYM_PLANE, sym::NO_PHYSICS, sym::CVector3(20, 20, 20));
			CursorLockOn->SetMaterial("crosshair_lockon.sym.material");

			Hovered = false;
			Targeted = false;
			LockedOn = false;
			Hover = 0;
			LockOn = 0;

			Scene.SetActiveCamera(&Camera);
			sym::CBrush *Floor = Scene.AddPlane("Floor", sym::LEVEL_COLLISION, sym::CVector3(1000, 1000, 1000));
			Floor->SetMaterial("floor.sym.material");
			Floor->TextureTile = sym::CVector3(4.0f, 4.0f, 4.0f);

			sym::CBrush *Ramp1 = Scene.AddPlane("Ramp1", sym::LEVEL_COLLISION, sym::CVector3(50, 0, 100));
			sym::CBrush *Ramp2 = Scene.AddPlane("Ramp2", sym::LEVEL_COLLISION, sym::CVector3(50, 0, 100));
			sym::CBrush *Ramp3 = Scene.AddPlane("Ramp3", sym::LEVEL_COLLISION, sym::CVector3(50, 0, 100));

			Ramp1->Pitch(45.0f);
			Ramp2->Pitch(25.0f);
			Ramp3->Pitch(10.0f);

			Ramp1->SetPosition(sym::CVector3(-50.0f, 10.0f, -100.0f));
			Ramp2->SetPosition(sym::CVector3(0.0f, 10.0f, -100.0f));
			Ramp3->SetPosition(sym::CVector3(50.0f, 10.0f, -100.0f));

			Ramp1->SetMaterial("panel.sym.material");
			Ramp2->SetMaterial("panel.sym.material");
			Ramp3->SetMaterial("panel.sym.material");

			sym::CBrush *Box1 = Scene.AddBox("Box1", sym::LEVEL_COLLISION, sym::CVector3(50, 5, 50));
			sym::CBrush *Box2 = Scene.AddBox("Box2", sym::LEVEL_COLLISION, sym::CVector3(50, 10, 50));
			sym::CBrush *Box3 = Scene.AddBox("Box3", sym::LEVEL_COLLISION, sym::CVector3(50, 20, 50));
			sym::CBrush *Box4 = Scene.AddBox("Box4", sym::LEVEL_COLLISION, sym::CVector3(50, 50, 50));
			sym::CBrush *Box5 = Scene.AddBox("Box5", sym::PHYSICS_OBJ_BOX, sym::CVector3(20, 20, 20));

			Box1->SetPosition(sym::CVector3(-50.0f, 2.5f, 100.0f));
			Box2->SetPosition(sym::CVector3(0.0f, 5.0f, 100.0f));
			Box3->SetPosition(sym::CVector3(50.0f, 10.0f, 100.0f));
			Box4->SetPosition(sym::CVector3(100.0f, 300.0f, 100.0f));
			Box5->SetPosition(sym::CVector3(100.0f, 100.0f, 100.0f));

			Box1->SetMaterial("crate.sym.material");
			Box2->SetMaterial("crate.sym.material");
			Box3->SetMaterial("crate.sym.material");
			Box4->SetMaterial("crate.sym.material");
			Box5->SetMaterial("crate.sym.material");

			RopeMaterial.Load("rope.sym.material");

			if(!Character.Create("alien.sym.mesh"))
			{
				sym::Engine->Console.PrintLine("Could not load character mesh");
			}
			else
			{
				Character.SetMass(10.0f);
				Character.SetPosition(sym::CVector3(0, 60, 0));

				//Add to scene
				Scene.AddGeometry(&Character);
			}

			//Create some enemies
			for(int i = 0; i < 5; i++)
			{
				sym::CBrush *Enemy = Scene.AddBox("", sym::PHYSICS_OBJ_BOX, sym::CVector3(20, 50, 20));  
				Enemy->SetPosition(sym::CVector3(-200.0f, 25.0f, -50.0f * i));
				Enemy->SetMaterial("panel.sym.material");
				Enemies.push_back(Enemy);
			}

			//Set physics world size
			//sym::Engine->Physics.SetWorldSize(Scene.Octree.AABB);
			//Scene.ProcessScene();

			Scene.LoadSkyboxTextures("skyboxes\\reef\\reef_north.png", "skyboxes\\reef\\reef_south.png", "skyboxes\\reef\\reef_east.png", "skyboxes\\reef\\reef_west.png", "skyboxes\\reef\\reef_up.png", "skyboxes\\reef\\reef_down.png");
			Scene.ShowSkybox(true);

			//Start physics simulation
			sym::Engine->Physics.Run();

			//Turn the lights on
			glEnable(GL_LIGHT0);

			CharacterSpeed = 10.0f;
			CharacterTurnSpeed = 0.06f;
			CameraDistance = 100.0f;
			CameraHeight = 20.0f;
			CharacterPitch = 0.0f;

			CameraPitch = 0.0f;
			CameraYawOffset = 0.0f;
			MouseRelX = MouseRelY = 0.0f;

			MaxRopeLength = 200.0f;
			RopeWidth = 2.0f;
			LMB = false;

			DrawCollision = true;

			Rope = 0;

			Scene.ShowCollision(true);
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
			//Get mouse input
			MouseRelX = sym::Engine->Input.GetMouseRelativeX() * sym::Engine->Input.GetMouseResX();
			MouseRelY = sym::Engine->Input.GetMouseRelativeY() * sym::Engine->Input.GetMouseResY();

			//SHOOT ROPE
			if(sym::Engine->Input.IsLMBDown())
			{
				if(!LMB)
				{
					//If rope exists, delete it
					if(Rope)
					{
						delete Rope;
						Rope = 0;

						//Reset character up vector
						Character.SetUpVector(sym::CVector3(0, 1, 0));
					}
					else
					{
						//Raycast from character front to max distance
						sym::CVector3 Start = Character.GetPosition() + (Character.GetForward() * (Character.GetLocalAABB().Size.z));
						sym::CVector3 End = Character.GetPosition() + (Character.GetForward() * MaxRopeLength);
						sym::RaycastData HitData = sym::Engine->Physics.Raycast(Start, End);

						//Check raycast distance is valid
						if(HitData.Distance > 0.0f && HitData.Distance < 1.0f)
						{
							//Create new rope
							Rope = new sym::CRope;
							Rope->Create(Start, HitData.HitPosition, RopeWidth);

							//Attach rope
							Rope->AttachStart(&Character, Character.GetPosition());
							Rope->AttachEnd(HitData.Geometry, HitData.HitPosition);

							//Apply rope material
							Rope->SetMaterial(&RopeMaterial);

							//Disable collision with character
							Rope->DisableCollision(&Character);

							//Disable up vector
							Character.ReleaseUpVector();
						}
					}

					LMB = true;
				}
			}
			else LMB = false;

			//Get keyboard input+
			if(sym::Engine->Input.IsKeyDown('w')) Character.AddMovementVector(Character.GetForward());
			if(sym::Engine->Input.IsKeyDown('s')) Character.AddMovementVector(Character.GetForward() - (Character.GetForward() * 2));
			if(sym::Engine->Input.IsKeyDown('d')) Character.AddMovementVector(Character.GetRight());
			if(sym::Engine->Input.IsKeyDown('a')) Character.AddMovementVector(Character.GetRight() - (Character.GetRight() * 2));

			if(sym::Engine->Input.IsKeyPressed(SDLK_SPACE)) Character.Jump(3000.0f);

			if(sym::Engine->Input.IsKeyPressed('e'))
			{
				if(Targeted) Targeted = false;
				else if(Hovered)
				{
					Targeted = true;
				}
			}

			if(sym::Engine->Input.IsKeyPressed('f'))
			{
				if(LockedOn)
				{
					LockedOn = false;
					Targeted = true;
				}
				else if(Targeted)
				{
					LockedOn = true;
					Targeted = false;
				}
				else if(Hovered)
				{
					LockedOn = true;
					Hovered = false;
					LockOn = Hover;
				}
			}
			
			if(sym::Engine->Input.IsKeyPressed('c'))
			{
				DrawCollision = !DrawCollision;
				if(DrawCollision) Scene.ShowCollision(true);
				else Scene.ShowCollision(false);
			}

			//Quit key
			if(sym::Engine->Input.IsKeyPressed(27)) sym::Engine->StateManager.PopState();
		}

		void Update()
		{
			//Apply torque to character
			//Character.AddYawTorque(-MouseRelX * ((float)sym::Engine->GetDeltaTime() * CharacterTurnSpeed));
			sym::CPlaceable *Char = (sym::CPlaceable*)&Character;
			if(!LockedOn) Char->Yaw(-MouseRelX);// * (sym::Engine->GetDeltaTime() * CharacterTurnSpeed));

			//Set the cam matrix
			Camera.SetMatrix(Character.GetMatrix());

			//Apply mouse pitch to camera
			CameraPitch += MouseRelY;
			Camera.Pitch(-CameraPitch);

			//Pull back to camera distance
			Camera.Move(-CameraDistance);

			//Move camera up
			Camera.Fly(CameraHeight);

			if(!LockedOn)
			{
				if(!Targeted)
				{
					//Raycast forward, see if an enemy is in front
					sym::CVector3 Start = Character.GetPosition();// + (Character.GetForward() * (Character.GetLocalAABB().Size.z));
					sym::CVector3 End = Character.GetPosition() + (Character.GetForward() * 1000.0f);
					sym::RaycastData HitData = sym::Engine->Physics.Raycast(Start, End);

					Hovered = false;

					if(HitData.Distance > 0.0f && HitData.Distance < 1.0f)
					{
						//Loop through enemies
						for(int i = 0; i < Enemies.size(); i++)
						{
							//If the object is an enemy
							if(HitData.Geometry == Enemies[i])
							{
								//Set lock-on cursor position
								CursorHover->SetPosition(HitData.Geometry->GetPosition());
								CursorHover->LookAt(sym::CVector3(0, 1, 0), Camera.GetPosition(), 1.0f);
								Hover = HitData.Geometry;
								Hovered = true;

								break;
							}
						}
					}
				}
				else if(Hover)
				{
					LockOn = Hover;
					CursorTarget->SetPosition(LockOn->GetPosition());
					CursorTarget->LookAt(sym::CVector3(0, 1, 0), Camera.GetPosition(), 1.0f);
					Hovered = false;
				}
			}
			else if(LockOn && LockedOn)
			{
				//Create LookAt quaternion, facing forward
				sym::CQuaternion Quat;
				Quat.FromLookAt(sym::CVector3(0, 0, -1), Character.GetPosition(), LockOn->GetPosition(), 1.0f);

				//Zero roll
				Quat.X = 0.0f;

				Character.SetQuaternion(Quat);

				CursorLockOn->SetPosition(LockOn->GetPosition());
				CursorLockOn->LookAt(sym::CVector3(0, 1, 0), Camera.GetPosition(), 1.0f);
			}

			Stream1.str("");
			Stream2.str("");
			Stream1 << "Position: " << Character.GetPosition().x << " " << Character.GetPosition().y << " " << Character.GetPosition().z;
			Stream2 << "Quaternion: " << Character.GetQuaternion().X << " " << Character.GetQuaternion().Y << " " << Character.GetQuaternion().Z << " " << Character.GetQuaternion().W;
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

				if(Rope) Rope->Render();

				glDisable(GL_DEPTH_TEST);
				if(Hovered) CursorHover->Render();
				if(Targeted) CursorTarget->Render();
				if(LockedOn) CursorLockOn->Render();
				glEnable(GL_DEPTH_TEST);

				sym::glEnable2D();

					if(Targeted) Font.RenderText("TARGETED", 10, 60);
					if(LockedOn) Font.RenderText("LOCKED ON", 10, 60);

					Font.RenderText(Stream1.str(), 10, 40);
					Font.RenderText(Stream2.str(), 10, 20);

				sym::glDisable2D();

			sym::FinishRendering();
		}

	private:
		sym::CScene Scene;
		sym::CCamera Camera;
		sym::CCharacterController Character;

		sym::CFont Font;
		std::stringstream Stream1, Stream2;

		sym::CRope *Rope;
		sym::CMaterial RopeMaterial;
		float MaxRopeLength;
		float RopeWidth;
		bool LMB;

		std::vector<sym::CGeometry*> Enemies;
		sym::CBrush *CursorHover;
		sym::CBrush *CursorTarget;
		sym::CBrush *CursorLockOn;
		sym::CGeometry *Hover;
		sym::CGeometry *LockOn;
		bool Hovered;
		bool Targeted;
		bool LockedOn;

		bool DrawCollision;
		
		float CharacterSpeed;
		float CharacterTurnSpeed;
		float CameraDistance;
		float CameraHeight;
		float CameraPitch;
		float CameraYawOffset;
		float CharacterPitch;
		float MouseRelX, MouseRelY;
};