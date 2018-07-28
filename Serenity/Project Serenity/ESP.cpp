#include "ESP.h"
#include "LegitBacktrack.h"

void ESP::Start() 
{
	for (int i = 0; i <= g_pEngine->GetMaxClients(); i++) 
	{
		auto pLocal = g_pEntityList->GetClientEntity(g_pEngine->GetLocalPlayer());
		auto pEntity = g_pEntityList->GetClientEntity(i);

		if (!pEntity) continue;
		if (pEntity == pLocal)continue;
		if (pEntity->GetDormant()) continue;
		if (pEntity->GetHealth() < 1) continue;
		if (g_pVars.visuals.EnemyOnly && pEntity->GetTeam() == pLocal->GetTeam()) continue;

		RECT EspRect;
		player_info_t info;
		EspRect = GetBox(pEntity);
		if (IsRectEmpty(&EspRect)) continue;
		Color BoxClr = pEntity->GetTeam() == pLocal->GetTeam() ? Color(0, 255, 0, 255) : Color(255, 0, 0, 255);
		g_pEngine->GetPlayerInfo(pEntity->GetIndex(), &info);

		if (g_pVars.visuals.Box)
			PlayerBox(pEntity, EspRect, BoxClr);

		if (g_pVars.visuals.Name)
			PlayerName(pEntity, EspRect, info);

		if (g_pVars.visuals.Weapon)
			PlayerWeapon(pEntity, EspRect);

		if (g_pVars.visuals.Distance)
			PlayerDistance(pEntity, EspRect, pLocal);

		if (g_pVars.visuals.Health)
			PlayerHealth(pEntity, EspRect);

		if (g_pVars.visuals.Money)
			PlayerMoney(pEntity, EspRect);

		if (g_pVars.visuals.Backtrack && g_pVars.legit.Backtrack)
			BacktrackTicks(pEntity, pLocal);

		if (g_pVars.visuals.ResolverInfo)
			ResolverInfo(pEntity, pLocal);

		int snapMode = g_pVars.visuals.Snaplines;

		if(snapMode > 0)
			Snaplines(pEntity, EspRect, BoxClr, snapMode);
	}
}

void ESP::ResolverInfo(C_BaseEntity* pEnt, C_BaseEntity* pLocal)
{
	/*Draw.text(EspRect.right + 3, EspRect.top + 10, ("%d$", money), F_ESP, Color(0, 222, 0, 255));*/
}

void ESP::BacktrackTicks(C_BaseEntity* pEnt, C_BaseEntity* pLocal)
{
	int index = pEnt->GetIndex();

	if (g_pVars.misc.FakeLatencyEnable)
	{
		for (int i = 52; i < 65; i++)
		{
			Vector thisTick = legitBacktrack.entityPostionData[pEnt->GetIndex()][i].headPosition;

			Vector screenThisTick;

			if (WorldToScreen(thisTick, screenThisTick))
			{
				Draw.rect(screenThisTick.x, screenThisTick.y, 3, 3, Color(255, 255, 255, 255));
			}
		}
	}

	else
	{
		for (int i = 2; i < g_pVars.legit.BacktrackTicks; ++i)
		{
			if (legitBacktrack.entityPostionData[index][i].simulationTime && legitBacktrack.entityPostionData[index][i].simulationTime + 1 > pLocal->SimTime())
			{
				Vector thisTick = legitBacktrack.entityPostionData[pEnt->GetIndex()][i].headPosition;
				Vector nextTick = legitBacktrack.entityPostionData[pEnt->GetIndex()][i - 1].headPosition;

				Vector screenThisTick, screenNextTick;

				if (WorldToScreen(thisTick, screenThisTick) && WorldToScreen(nextTick, screenNextTick))
				{
					Draw.line(screenThisTick.x, screenThisTick.y, screenNextTick.x, screenNextTick.y, Color(255, 255, 255, 255));
				}
			}
		}

	}
}

void ESP::Snaplines(C_BaseEntity* pEntity, RECT EspRect, Color BoxClr, int mode)
{
	int width = 0;
	int height = 0;

	Vector to = Vector(EspRect.left + (EspRect.right - EspRect.left) / 2, EspRect.top + (EspRect.bottom - EspRect.top), 0);

	g_pEngine->GetScreenSize(width, height);

	Vector From(0,0,0);

	if(mode == 1)
		From = Vector((width / 2), (height / 2), 0);

	else if (mode == 2)
		From = Vector((width / 2), height, 0);

	else
		From = Vector((width / 2), 0, 0);
		
	Draw.line(From.x, From.y, to.x, to.y, BoxClr);
}

void ESP::PlayerBox(C_BaseEntity* pEntity, RECT EspRect, Color BoxClr)
{	
	Draw.outlined_rect(EspRect.left, EspRect.top, EspRect.right - EspRect.left, EspRect.bottom - EspRect.top, BoxClr, Color(255,255,255,0));/*
	Draw.outlined_rect(EspRect.left - 1, EspRect.top - 1, EspRect.right - EspRect.left + 2, EspRect.bottom - EspRect.top + 2, Color(255, 255, 255, 255), Color(0, 0, 0, 255));
	Draw.outlined_rect(EspRect.left + 1, EspRect.top + 1, EspRect.right - EspRect.left - 2, EspRect.bottom - EspRect.top - 2, Color(255, 255, 255, 255), Color(0, 0, 0, 255));*/
}

void ESP::PlayerHealth(C_BaseEntity* pEntity, RECT EspRect) 
{
	std::string s = std::to_string(pEntity->GetHealth());
	char const *health = s.c_str();

	Draw.text(EspRect.left - 7, EspRect.top - 2, ("HP:%d", health), F_ESP, Color(255, 255, 255, 255));
	auto hp = pEntity->GetHealth();
	auto green = int(hp * 2.55f);
	auto red = 255 - green;
	auto height = ((EspRect.bottom - EspRect.top) * hp) / 100;
	Draw.outlined_rect(EspRect.left - 4, EspRect.bottom - height, 1, height, Color(255, 255, 255, 255), Color(red, green, 0, 255));
}
void ESP::PlayerWeapon(C_BaseEntity* pEntity, RECT EspRect)
{
	auto weapon = pEntity->GetActiveWeapon();
	auto weapondata = weapon->GetCSWeaponData();


	std::string a = std::to_string(weapon->Clip1());
	std::string aa = std::to_string(weapondata->iMaxClip1);
	char const *inClip = a.c_str();
	char const *max = aa.c_str();

	Draw.text((EspRect.left + EspRect.right) / 2, EspRect.bottom + 1, ("%s[%d/%d]", weapon->GetWeaponName().c_str(), inClip, max), F_ESP, Color(255, 255, 255, 255));

}
void ESP::PlayerMoney(C_BaseEntity* pEntity, RECT EspRect) 
{
	std::string s = std::to_string(pEntity->GetMoney());
	char const *money = s.c_str();

	Draw.text(EspRect.right + 3, EspRect.top + 10, ("%d$", money), F_ESP, Color(0, 222, 0, 255));
}

void ESP::PlayerName(C_BaseEntity* pEntity, RECT EspRect,player_info_t info) 
{
	std::string name = info.szName;
	Draw.text((EspRect.left + EspRect.right) / 2, EspRect.top - 12, name.c_str(), F_ESP, Color(255, 255, 255, 255));
}

void ESP::PlayerDistance(C_BaseEntity* pEntity, RECT EspRect, C_BaseEntity* pLocal) {
	auto a = pLocal->GetOrigin();
	auto b = pEntity->GetOrigin();
	int dist = sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2) + pow(a.z - b.z, 2)) * 0.0254f;
	std::string s = std::to_string(dist);
	char const *distance = s.c_str();

	Draw.text(EspRect.right + 3, EspRect.top, ("%dM", distance), F_ESP, Color(255, 255, 255, 255));
}

bool ESP::WorldToScreen(const Vector& in, Vector& out) 
{
	static ptrdiff_t ptrViewMatrix;
	if (!ptrViewMatrix)
	{
		ptrViewMatrix = static_cast<ptrdiff_t>(g_pUtils->FindPatternIDA("client.dll", "0F 10 05 ? ? ? ? 8D 85 ? ? ? ? B9"));
		ptrViewMatrix += 0x3;
		ptrViewMatrix = *reinterpret_cast<uintptr_t*>(ptrViewMatrix);
		ptrViewMatrix += 176;
	}
	const matrix3x4_t& worldToScreen = *(matrix3x4_t*)ptrViewMatrix;

	int ScrW, ScrH;

	g_pEngine->GetScreenSize(ScrW, ScrH);

	float w = worldToScreen[3][0] * in[0] + worldToScreen[3][1] * in[1] + worldToScreen[3][2] * in[2] + worldToScreen[3][3];
	out.z = 0;
	if (w > 0.01)
	{
		float inverseWidth = 1 / w;
		out.x = (ScrW / 2) + (0.5 * ((worldToScreen[0][0] * in[0] + worldToScreen[0][1] * in[1] + worldToScreen[0][2] * in[2] + worldToScreen[0][3]) * inverseWidth) * ScrW + 0.5);
		out.y = (ScrH / 2) - (0.5 * ((worldToScreen[1][0] * in[0] + worldToScreen[1][1] * in[1] + worldToScreen[1][2] * in[2] + worldToScreen[1][3]) * inverseWidth) * ScrH + 0.5);
		return true;
	}
	return false;
}
RECT ESP::GetBox(C_BaseEntity* pEntity) {
	RECT rect{};
	auto collideable = pEntity->GetCollideable();

	if (!collideable)
		return rect;

	auto min = collideable->OBBMins();
	auto max = collideable->OBBMaxs();

	auto trans = pEntity->GetCoordinateFrame();

	Vector points[] = {
		Vector(min.x, min.y, min.z),
		Vector(min.x, max.y, min.z),
		Vector(max.x, max.y, min.z),
		Vector(max.x, min.y, min.z),
		Vector(max.x, max.y, max.z),
		Vector(min.x, max.y, max.z),
		Vector(min.x, min.y, max.z),
		Vector(max.x, min.y, max.z)
	};

	Vector pointsTransformed[8];
	for (int i = 0; i < 8; i++) {
		Math::VectorTransform(points[i], trans, pointsTransformed[i]);
	}

	Vector screen_points[8] = {};

	for (int i = 0; i < 8; i++) {
		if (!WorldToScreen(pointsTransformed[i], screen_points[i]))
			return rect;
	}

	auto left = screen_points[0].x;
	auto top = screen_points[0].y;
	auto right = screen_points[0].x;
	auto bottom = screen_points[0].y;

	for (int i = 1; i < 8; i++) {
		if (left > screen_points[i].x)
			left = screen_points[i].x;
		if (top < screen_points[i].y)
			top = screen_points[i].y;
		if (right < screen_points[i].x)
			right = screen_points[i].x;
		if (bottom > screen_points[i].y)
			bottom = screen_points[i].y;
	}
	return RECT{ (long)left, (long)bottom, (long)right, (long)top };

}
