
#ifdef WIN32
#include <windows.h>
#endif

#include "SYM_Core.h"

#include "tests/MainMenuTest.h"

CMainMenu MainMenuState;

#ifdef WIN32
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
int main(int argc, char *argv[])
#endif
{
	//Fire up Symmetry engine
	if (!sym::Engine->Init()) return 0;

	//Push back states
	sym::Engine->StateManager.PushState(&MainMenuState);

	//MAIN LOOP
	while(sym::Engine->Update())
	{
        //Hardcoded console key, safety measure
		if(sym::Engine->Input.IsKeyPressed('`')) sym::Engine->Console.Toggle();
		if(sym::Engine->Input.IsKeyPressed('~')) sym::Engine->Console.Toggle();
	}

	return 0;
}

	//Load scene
	//sym::Engine->Scene.LoadScene("hoverplayground.sym.scene");

	//sym::Engine->Scene.DrawBones = true;


	/*
	sym::CShader Shader;
	Shader.Load("reliefmap.cgfx");
	Shader.AddTextureParam("DiffuseSampler", "rockwall.png");
	Shader.AddTextureParam("NormalMapSampler", "rockwallnmap.png");

	float Edge[3] = {0.0f, 0.0f, 1.0f};
	Shader.AddFloat3Param("EdgeColour", Edge);

	sym::CMaterial Material;
	Material.SetShader(&Shader);

	sym::CModel *Model = sym::Engine->Scene.AddModel("Model", "crate.sym.mesh", 1);
	//Model->SetMaterial(&Material);
	*/

		/*
		if(sym::Engine->Input.IsKeyDown('h')) Box2->AddForce(sym::CVector3(50, 0, 15) * sym::Engine->GetDeltaTime());
		if(sym::Engine->Input.IsKeyDown('f')) Box2->AddForce(sym::CVector3(-50, 0, -15) * sym::Engine->GetDeltaTime());
		if(sym::Engine->Input.IsKeyDown('t')) Box2->AddForce(sym::CVector3(0, 50, 0) * sym::Engine->GetDeltaTime());
		if(sym::Engine->Input.IsKeyDown('g')) Box2->AddForce(sym::CVector3(0, -50, 0) * sym::Engine->GetDeltaTime());
		*/

		//sym::CCamera *Camera = sym::Engine->Scene.GetActiveCamera();
		//float DeltaTime = sym::Engine->GetDeltaTime();
		//float MouseRes = sym::Engine->Input.GetMouseResX();

		//Camera->Yaw(-sym::Engine->Input.GetMouseRelativeX() * MouseRes);
		//Camera->Pitch(-sym::Engine->Input.GetMouseRelativeY() * MouseRes);

		//if(sym::Engine->Input.IsKeyDown('w')) Camera->Move(2.0f * DeltaTime);
		//if(sym::Engine->Input.IsKeyDown('s')) Camera->Move(-2.0f * DeltaTime);
		//if(sym::Engine->Input.IsKeyDown('d')) Camera->Strafe(2.0f * DeltaTime);
		//if(sym::Engine->Input.IsKeyDown('a')) Camera->Strafe(-2.0f * DeltaTime);

			/*
			sym::CModel *Model = sym::Engine->Scene.AddModel("Model", "beartrap.sym.mesh", 1);
			sym::CSkeletalAnimation *Animation = sym::Engine->Scene.LoadAnimation("Anim", "beartrap1.sym.animation");
			//sym::CSkeletalAnimation *Animation2 = sym::Engine->Scene.LoadAnimation("Anim2", "bip_dance.sym.animation");
			Model->Mesh->BindAnimation(Animation);
			Model->Mesh->SetAnimSpeed(0.001f);
			*/

		//if(sym::Engine->Input.IsKeyPressed('g')) Model->Mesh->StartAnimation();

		/*
		if(sym::Engine->Input.IsKeyPressed('1'))
		{
			Model->Mesh->ResetAnimation();
			Model->Mesh->SetAnimSpeed(0.001f);
			Model->Mesh->BindAnimation(Animation);
		}

		if(sym::Engine->Input.IsKeyPressed('2'))
		{
			Model->Mesh->ResetAnimation();
			Model->Mesh->SetAnimSpeed(0.0005f);
			Model->Mesh->BindAnimation(Animation2);
		}
		*/

