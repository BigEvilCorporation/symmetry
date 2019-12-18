#include "SYM_Core.h"

class CHUD
{
	public:
		void Init();

		void Update();
		void Render();

		void SelectWeapon(int Weapon);
		void CloseWeaponSelector();

		void SetHealth(int Health);

		bool IsOpen();

	private:
		int CurrentWeapon;
		int CurrentHealth;
		bool Open;

		sym::CFont Font;
		sym::CSprite	WeaponSelectedMinigun,
						WeaponUnSelectedMinigun,
						WeaponSelectedLaser,
						WeaponUnselectedLaser,
						WeaponSelectedTurret,
						WeaponUnselectedTurret,
						WeaponSelectedRocket,
						WeaponUnselectedRocket;

		sym::CSprite HealthIcon;
};

