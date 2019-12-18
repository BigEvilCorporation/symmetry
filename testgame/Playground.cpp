#include "Playground.h"
#include "Game.h"

#include "EnemyBase.h"
#include "WeaponTurretLauncher.h"
#include "WeaponGrappleGun.h"
#include "WeaponRocketLauncher.h"
#include "WeaponLaser.h"

int LuaLoadPlayground(lua_State *LuaState)
{
	if(lua_gettop(LuaState) == 0)
	{
		CPlaygroundState *State = new CPlaygroundState();

		Game->ChangeLevel(State);
	}
	else
	{
	    Game->Log.AddEntry("Error: 'loadplayground' takes no arguments");
		sym::Engine->Console.PrintLine("> Error: 'loadplayground' takes no arguments");
	}

	return 0;
}

CPlaygroundState::CPlaygroundState()
{

}

void CPlaygroundState::Init()
{
	//Call original init
	CLevelState::Init();

	LoadScene("level3.sym.scene");

	//Load crosshair
	Crosshair.Load("mouse_cursor.png");
	Crosshair.SetTransparency(0.5f);

	/*
	Test.Load("test", "test.sym.mesh", sym::NO_PHYSICS);
	TestAnim.Load("test.sym.animation");
	Test.Mesh->BindAnimation(&TestAnim);
	Test.Mesh->SetAnimSpeed(0.005f);
	Test.Mesh->StartAnimation();
	Test.SetPosition(sym::CVector3(100.0f, 0.0f, 100.0f));
	Scene.AddGeometry(&Test);
	*/

	//Player.SetPosition(sym::CVector3(100.0f, Player.GetLocalAABB().Size.y / 2.0f, -100.0f));
	//Test.SetPosition(sym::CVector3(600.0f, Test.GetLocalAABB().Size.y / 2.0f + 1000.0f, 100.0f));

	/*
	CEnemyBase *Enemy = new CEnemyBase();
	Enemy->Init(&Player);
	Enemy->SetPosition(sym::CVector3(200.0f, Enemy->GetLocalAABB().Size.y / 2.0f, -200.0f));
	AddEnemy(Enemy);
	*/

	//sym::CBrush *Floor = Scene.AddBox("Floor", sym::PHYSICS_OBJ_BOX, sym::CVector3(5000.0f, 1.0f, 5000.0f));
	//Floor->SetMaterial("floor.sym.material");

	/*
	for(std::map<std::string, sym::CGeometry*>::iterator i = Scene.Geometry.begin(); i != Scene.Geometry.end(); i++)
	{
		if((*i).second->Mesh)
		{
			for(std::vector<sym::CMeshSubset>::iterator j = (*i).second->Mesh->Subsets.begin(); j != (*i).second->Mesh->Subsets.end(); j++)
			{
				if((*j).Material)
					if((*j).Material->Texture)
						if((*j).Material->LoadShader("celshading.cgfx"))
							(*j).Material->Shader->SetTextureParam("DiffuseSampler", (*j).Material->Texture);
			}
		}
	}
	*/
}

void CPlaygroundState::Shutdown()
{
	AmbientMusic.Stop();
	CLevelState::Shutdown();
}

void CPlaygroundState::Pause()
{
	AmbientMusic.Stop();
	CLevelState::Pause();
}

void CPlaygroundState::Resume()
{
	AmbientMusic.Play();
	CLevelState::Resume();
}

void CPlaygroundState::HandleInput()
{
	CLevelState::HandleInput();
}

void CPlaygroundState::Update()
{
	sym::CVector3 Start = Player.GetPosition() + (Player.GetForward() * Player.GetLocalAABB().Size.z / 2.0f);
	sym::CVector3 End = Player.GetPosition() + (Player.GetAimVector() * 600.0f);
	AimRaycastData = sym::Engine->Physics.Raycast(Start, End);

	Crosshair.SetPosition(AimRaycastData.HitPosition);

	sym::CQuaternion Quat;
	Quat.FromLookAt(sym::CVector3(0.0f, 0.0f, -1.0f), Crosshair.GetPosition(), Player.GetPosition(), 1.0f);

	Crosshair.SetRotation(Quat);

	CLevelState::Update();
}

void CPlaygroundState::OnRender()
{
	/*
	glPushMatrix();
	glMultMatrixf(Player.GetMatrix().GetAsFloatArray());
	Player.Mesh->RenderBones(sym::CColour(255, 0, 0));
	glPopMatrix();
	*/

	//Render crosshair
	//if(AimRaycastData.Distance > 0.0f) Crosshair.Render();

	/*
	sym::CVector3 StartPos = Player.GetPosition() + (Player.GetForward() * ((Player.GetLocalAABB().Size.y / 2.0f)));
	sym::CVector3 EndPos = StartPos + (Player.GetAimVector() * 1000.0f);

	glBegin(GL_LINES);

	glColor3f(1.0f, 0.0f, 0.0f);
	
	glVertex3f(StartPos.x, StartPos.y, StartPos.z);
	glVertex3f(EndPos.x, EndPos.y, EndPos.z);

	glEnd();
	*/

	CLevelState::OnRender();
}

void CPlaygroundState::OnRenderOverlay()
{
	CLevelState::OnRenderOverlay();
}