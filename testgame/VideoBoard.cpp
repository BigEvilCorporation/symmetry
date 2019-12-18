#include "VideoBoard.h"

void CVideoBoard::OnSpawn(sym::CVector3 Position, sym::CQuaternion Rotation)
{
	//Load mesh
	Load("VideoDisplay", "billboard.sym.mesh", sym::PHYSICS_OBJ_BOX);

	//Set position
	SetPosition(sym::CVector3(Position.x, GetLocalAABB().Size.y / 2, Position.z));
	SetRotation(Rotation);

	//Get display
	Display = Mesh->GetSubsetByName("display");

	//Set material
	Display->SetMaterial(&VideoMaterial);

	//Load video
	Video.Load("robotica.mpg");

	//Set video texture
	VideoMaterial.Texture = Video.Texture;
	
	TextureRotation = sym::CVector3(0.0f, 0.0f, 90.0f);
	TextureTile = sym::CVector3(-1.0f, 1.0f, 1.0f);

	//Play video
	Video.Play();
}

void CVideoBoard::OnUpdate()
{
	if(!Video.ReadFrame())
	{
		Video.SeekBackward(0);
		Video.Play();
	}
}

void CVideoBoard::OnRender()
{
}

void CVideoBoard::OnDestroy()
{
}