#pragma once

#include <sstream>

#include "SYM_Core.h"

#include "CharacterPlayer.h"
#include "EnemyBase.h"
#include "Spawn.h"
#include "HUD.h"

class CLevelState : public sym::CState
{
	public:
		CLevelState();

		//State functions
		void Init();
		void Shutdown();
		void Pause();
		void Resume();
		void HandleInput();
		void Update();
		void Render();

		//Post render
		void OnRender();
		void OnRenderOverlay();

		//Register all spawnable objects, preload media
		void RegisterSpawnables();

		//Load scene
		bool LoadScene(std::string Filename);

		//Get current scene
		sym::CScene *GetScene();

		//Spawn an object
		template<class Type>
		Type *Spawn(sym::CVector3 Position, sym::CQuaternion Rotation);

		//Destroy a spawned object
		void DestroySpawnedObject(CSpawnable *Object);

		//Get list of spawned objects
		std::vector<CSpawnable*> *GetSpawnedObjects();

		//Add an enemy
		void AddEnemy(CEnemyBase* Enemy);

		//Return player
		CCharacterPlayer *GetPlayer();

		//Set vision mode
		void SetVisionMode(int Mode);

	protected:
		//Scene
		sym::CScene Scene;

		//Post processing
		sym::CFramebuffer Framebuffer;
		GLuint QuadDisplayList;
		sym::CShader *CelShader;
		sym::CShader *PostShader;
		CGparameter BloomParam;
		float BloomLevels[4];
		int BloomIndex;

		//Third person player camera
		sym::CCamera Camera;

		//HUD
		CHUD Hud;

		//The player
		CCharacterPlayer Player;

		float RunSpeed;
		float TurnSpeed;
		float SidestepSpeed;
		float PlayerHeading;

		//All spawned objects
		std::vector<CSpawnable*> SpawnedObjects;

		//Enemies
		std::vector<CEnemyBase*> Enemies;

		//Font
		sym::CFont Font;

		//Relative mouse coords
		float MouseRelX, MouseRelY;
		bool LMB, RMB;
		bool Jump;

		bool BulletTime;
		bool EnhancedVision;

		//Camera position and orientation
		float CameraDistance;
		float CameraHeight;
		float CameraPitch;
		float CameraYawOffset;

		//Special level geometry
		std::vector<sym::CGeometry*> GrappleObjects;
		std::vector<sym::CGeometry*> VisionObjects1;
		std::vector<sym::CGeometry*> VisionObjects2;

		//Vision mode transparency
		float VisibleAlpha;
		float InvisibleAlpha;

		//Skydome
		sym::CModel SkyDome;
		sym::CMaterial SkyMaterial;
		sym::CShader SkyShader;
		sym::CTexture SkyTextureExternal, SkyTextureInternal1, SkyTextureInternal2;

		//Audio
		sym::CSound AmbientMusic, BattleMusic;
};

template<class Type>
Type *CLevelState::Spawn(sym::CVector3 Position, sym::CQuaternion Rotation)
{
	//Create object
	Type *Object = new Type();

	//Call OnSpawn
	Object->OnSpawn(Position, Rotation);

	//Add to spawned list
	SpawnedObjects.push_back(Object);

	//Add to scene manager
	Scene.AddGeometry(Object);

	//Return object
	return Object;
}

//Custom geometry data flags
enum
{
	GRAPPLE1 = 1,
	GRAPPLE2 = 2,
	VISION1 = 4,
	VISION2 = 8,
	VISION3 = 16,
	SPAWN = 32
};