#pragma once

#include "SYM_Core.h"

int LuaLoadHackLevel(lua_State *LuaState);

class CHackingPlayer : public sym::CModel
{
	public:
		void Init();
		void SetSpeed(float S);
		void SetSteerSpeed(float S);

	private:
		void ApplyForceAndTorque();
		float Speed, SteerSpeed;
};

class CHackingState : public sym::CState
{
	public:
		CHackingState();
		void Init();
		void Shutdown();
		void Pause();
		void Resume();
		void HandleInput();
		void Update();
		void Render();

		void SetDifficulty(int Value);

	private:
		sym::CScene Scene;
		sym::CCamera Camera;
		sym::CFont Font;

		CHackingPlayer Player;

		//Tweakable difficulty
		int Difficulty;

		float Length;
		float Width;
		float Acceleration;
		float CurrentSpeed;
		float TopSpeed;
		float SteerSpeed;
		float Penalty;

		//Input
		float MouseRelX, MouseRelY;
		float CameraPitch, CameraDistance, CameraHeight;
		float CameraSpeed;
};