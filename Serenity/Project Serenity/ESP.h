#pragma once
#include "SDK.h"
class ESP {
public:
	void Start();
	bool WorldToScreen(const Vector& in, Vector& out);
private:
	void PlayerBox(C_BaseEntity* pEntity, RECT EspRect, Color BoxClr);
	void PlayerHealth(C_BaseEntity* pEntity, RECT EspRect);
	void PlayerWeapon(C_BaseEntity* pEntity, RECT EspRect);
	void PlayerMoney(C_BaseEntity* pEntity, RECT EspRect);
	void PlayerName(C_BaseEntity* pEntity, RECT EspRect, player_info_t info);
	void PlayerDistance(C_BaseEntity* pEntity, RECT EspRect,  C_BaseEntity* pLocal);
	void BacktrackTicks(C_BaseEntity* pEnt, C_BaseEntity* pLocal);
	void ResolverInfo(C_BaseEntity* pEnt, C_BaseEntity* pLocal);
	void Snaplines(C_BaseEntity* pEntity, RECT EspRect, Color BoxClr, int mode);
	RECT GetBox(C_BaseEntity* pEntity);
}; extern ESP* f_Esp;