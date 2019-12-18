#include "Hacking.h"
#include "Game.h"

#include <sstream>

int LuaLoadHackLevel(lua_State *LuaState)
{
	if(lua_gettop(LuaState) == 1)
	{
		CHackingState *State = new CHackingState();
		State->SetDifficulty(lua_tonumber(LuaState, 1));
		Game->ChangeLevel((CLevelState*)State);
	}
	else
	{
	    Game->Log.AddEntry("Error: 'loadhacklevel' takes 1 argument");
		sym::Engine->Console.PrintLine("> Error: 'loadhacklevel' takes 1 argument");
	}

	return 0;
}

void CHackingPlayer::Init()
{
	Speed = 0.0f;
	SteerSpeed = 0.0f;
	Load("HackingAvatar", "olaf.sym.mesh", sym::PHYSICS_OBJ_SPHERE);
	CreateNewtonMaterial();
	NewtonMaterialSetDefaultFriction(sym::Engine->Physics.GetWorld(), 
		NewtonMaterialGetDefaultGroupID(sym::Engine->Physics.GetWorld()), GetNewtonMaterial(), 2.0f, 1.0f);
	SetForceTorqueCallback();
	SetMass(20.0f);
}

void CHackingPlayer::SetSpeed(float S)
{
	Speed = S;
}

void CHackingPlayer::SetSteerSpeed(float S)
{
	SteerSpeed = S;
}

void CHackingPlayer::ApplyForceAndTorque()
{
	Force = sym::CVector3(SteerSpeed * Mass, -9.8f * Mass, -Speed * Mass);
	NewtonBodyAddForce(GetNewtonBody(), &Force.x);
}

CHackingState::CHackingState()
{
	Difficulty = 5;
	CurrentSpeed = 0.0f;
	SteerSpeed = 2000000.0f;

	MouseRelX = MouseRelY = 0.0f;
	CameraDistance = 200.0f;
	CameraHeight = 50.0f;
	CameraPitch = -10.0f;
	CameraSpeed = 10.0f;
}

void CHackingState::SetDifficulty(int Value)
{
	if(Value < 1) Difficulty = 1;
	else if(Value > 10) Difficulty = 10;
	else Difficulty = Value;
}

void CHackingState::Init()
{
	//Calculate variables using difficulty
	Length = 5000.0f * Difficulty;
	Width = 2000.0f / Difficulty;
	Acceleration = 0.001f * Difficulty;
	TopSpeed = 30.0f * Difficulty;
	Penalty = 2.0f * Difficulty;

	//Initialise scene
	Scene.Init();

	//Set camera
	Scene.SetActiveCamera(&Camera);
	Camera.Pitch(CameraPitch);

	//Load font
	Font.LoadTTFFont("arial.ttf", 12);

	//Create player
	Player.Init();
	Player.SetPosition(sym::CVector3(0.0f, Player.GetLocalAABB().Size.y, (Length / 2.0f) - 50.0f));
	Scene.AddGeometry(&Player);

	//Create floor and barriers
	sym::CBrush *Floor = Scene.AddPlane("Floor", sym::LEVEL_COLLISION, sym::CVector3(Width, 0.0f, Length));
	sym::CBrush *LeftWall = Scene.AddBox("LeftWall", sym::LEVEL_COLLISION, sym::CVector3(10.0f, 50.0f, Length));
	sym::CBrush *RightWall = Scene.AddBox("RightWall", sym::LEVEL_COLLISION, sym::CVector3(10.0f, 50.0f, Length));

	Floor->SetMaterial("floor.sym.material");
	LeftWall->SetMaterial("panel.sym.material");
	RightWall->SetMaterial("panel.sym.material");

	Floor->TextureTile = sym::CVector3(Width * 0.01f, Length * 0.01f, Length * 0.01f);
	LeftWall->TextureTile = sym::CVector3(Length * 0.01f, 1.0f, Length * 0.01f);
	RightWall->TextureTile = sym::CVector3(Length * 0.01f, 1.0f, Length * 0.01f);

	LeftWall->SetPosition(sym::CVector3(-Width / 2.0f, 25.0f, 0.0f));
	RightWall->SetPosition(sym::CVector3(Width / 2.0f, 25.0f, 0.0f));

	//Don't let mouse leave window
	sym::Engine->Input.StealMouse();

	//Set physics world size
	sym::CBoundingBox AABB;
	AABB.AddVertex(sym::CVector3(Length / 2.0f, Length / 2.0f, Length / 2.0f));
	AABB.AddVertex(sym::CVector3(-Length / 2.0f, -Length / 2.0f, -Length / 2.0f));

	sym::Engine->Physics.SetWorldSize(AABB);

	//Start physics simulation
	sym::Engine->Physics.Run();

	//Turn the lights on
	glEnable(GL_LIGHT0);
}

void CHackingState::Shutdown()
{
	//Shutdown scene
	Scene.ClearScene();
	Scene.Shutdown();
}

void CHackingState::Pause()
{
}

void CHackingState::Resume()
{
	//Don't let mouse leave window
	sym::Engine->Input.StealMouse();
}

void CHackingState::HandleInput()
{
	MouseRelX = sym::Engine->Input.GetMouseRelativeX() * sym::Engine->Input.GetMouseResolution() * sym::Engine->GetDeltaTime() * 0.1f;
	MouseRelY = sym::Engine->Input.GetMouseRelativeY() * sym::Engine->Input.GetMouseResolution() * sym::Engine->GetDeltaTime() * 0.1f;

	if(sym::Engine->Input.IsKeyPressed(SDLK_ESCAPE)) Game->CloseLevel();
}

void CHackingState::Update()
{
	CurrentSpeed += Acceleration * sym::Engine->GetDeltaTime();
	if(CurrentSpeed > TopSpeed) CurrentSpeed = TopSpeed;
	Player.SetSpeed(CurrentSpeed);
	Player.SetSteerSpeed(MouseRelX);

	//Set the cam position
	Camera.SetPosition(Player.GetPosition() + sym::CVector3(0.0f, CameraHeight, CameraDistance));

	//Update camera
	Camera.Update();
}

void CHackingState::Render()
{
	sym::StartRendering();
		Scene.Render();

		sym::glEnable2D();
		std::stringstream Stream;
		Stream << "Position: " << Player.GetPosition().x << " " << Player.GetPosition().y << " " << Player.GetPosition().z;
		Font.RenderText(Stream.str(), 20, 20);

		Stream.str("");
		Stream << "Speed: " << CurrentSpeed << "  Top speed: " << TopSpeed << "  Accel: " << Acceleration;
		Font.RenderText(Stream.str(), 20, 40);
		sym::glDisable2D();
	sym::FinishRendering();
}