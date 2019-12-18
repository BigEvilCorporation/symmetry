#include "../SYM_Engine.h"
#include "../SYM_Rope.h"

class CRopeDemoState : public sym::CState
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
					Font.RenderText("[ Loading Rope Demo ]", 
									sym::Engine->ScreenWidth / 2 - Font.GetTextWidth("[ Loading Rope Demo ]") / 2, 
									sym::Engine->ScreenHeight / 2);

				//Disable 2D rendering
				sym::glDisable2D();

			//Finish drawing loading screen
			sym::FinishRendering();

			//Load mouse cursor
			MouseCursor.Load("MouseCursor.png");
			MouseAbsX = MouseAbsY = MouseRelX = MouseRelY = 0.0f;

			//Add floor
			sym::CBrush *Floor = Scene.AddPlane("Floor", sym::LEVEL_COLLISION, sym::CVector3(2000, 2000, 2000));
			Floor->SetMaterial("floor.sym.material");
			Floor->TextureTile = sym::CVector3(4.0f, 4.0f, 4.0f);

			//Add restraint box
			sym::CBrush *Restraint = Scene.AddBox("Restraint", sym::LEVEL_COLLISION, sym::CVector3(2, 2, 200));
			Restraint->SetPosition(sym::CVector3(4, 30, 0));
			Restraint->SetMaterial("panel.sym.material");

			//Add rope
			//Rope.Create(Scene, 60.0f, sym::CVector3(1.0f, 4.0f, 1.0f), sym::CVector3(0, 80, 0));
			Rope.Create(sym::CVector3(-50, 80, 0), sym::CVector3(50, 10, 0), 1.0f);
			//Rope.SetMaterial("rope.sym.material");

			//Add box
			sym::CBrush *Box = Scene.AddBox("Box2", sym::PHYSICS_OBJ_BOX, sym::CVector3(5, 5, 5));
			Box->SetPosition(Rope.GetEndPosition());
			Box->SetMaterial("crate.sym.material");
			Box->SetMass(20.0f);

			//Attach end of rope to box
			Rope.AttachEnd(Box, sym::CVector3(0, Box->GetDimentions().y / 2, 0) + Box->GetPosition());

			Scene.ProcessScene();

			//Start physics simulation
			//sym::Engine->Physics.Run();

			//Scene.ShowCollision(true);

			//Turn the lights on
			glEnable(GL_LIGHT0);

			//Pull camera back
			Camera.SetPosition(sym::CVector3(0, 30, 120));

			//Set current camera
			Scene.SetActiveCamera(&Camera);
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
			//Get mouse absolute coords
			MouseAbsX = (float)sym::Engine->Input.GetMouseAbsoluteX();
			MouseAbsY = (float)sym::Engine->Input.GetMouseAbsoluteY();

			if(sym::Engine->Input.IsLMBDown())
			{
				MouseRelX += sym::Engine->Input.GetMouseRelativeX();
				MouseRelY += sym::Engine->Input.GetMouseRelativeY();
			}
			else
			{
				MouseRelX = MouseRelY = 0.0f;
			}

			if(sym::Engine->Input.IsKeyPressed(SDLK_SPACE)) Rope.DetachEnd();

			if(sym::Engine->Input.IsKeyPressed('p')) sym::Engine->Physics.Run();
			
			//ESC key
			if(sym::Engine->Input.IsKeyPressed(SDLK_ESCAPE)) sym::Engine->StateManager.PopState();
		}

		void Update()
		{
			Rope.GetStartLink()->AddForce(sym::CVector3(0, -MouseRelY * sym::Engine->GetDeltaTime() / 4, 0));
			Rope.GetStartLink()->AddForce(sym::CVector3(MouseRelX * sym::Engine->GetDeltaTime() / 4, 0, 0));

			//Set mouse cursor position
			MouseCursor.SetPosition(sym::CVector3(MouseAbsX, sym::Engine->ScreenHeight - MouseAbsY, 0.0f));
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

				//Render rope
				Rope.Render();

				sym::glEnable2D();

					Font.RenderText("Left click + drag: Move rope", 20, sym::Engine->ScreenHeight - 30);
					Font.RenderText("Space: Detach box", 20, sym::Engine->ScreenHeight - 55);

					std::stringstream Stream;
					Stream << "Number of links: " << Rope.Links.size();
					Font.RenderText(Stream.str(), 20, sym::Engine->ScreenHeight - 80);

					//Render mouse cursor
					MouseCursor.Render();

				sym::glDisable2D();

			sym::FinishRendering();
		}

	private:
		sym::CScene Scene;
		sym::CCamera Camera;
		sym::CFont Font;
		sym::CSprite MouseCursor;

		float MouseAbsX, MouseAbsY, MouseRelX, MouseRelY;

		sym::CRope Rope;
};