#include "Chams.h"

void Chams::Start()
{
	int chamsType = g_pVars.visuals.ChamsStyle;

	static IMaterial* ignorez = g_pMatSystem->FindMaterial("ignorez_ignorez", TEXTURE_GROUP_MODEL);;
	static IMaterial* normal = g_pMatSystem->FindMaterial("textured_normal", TEXTURE_GROUP_MODEL);

	if (chamsType == 2)
	{
		ignorez = g_pMatSystem->FindMaterial("flat_ignorez", TEXTURE_GROUP_MODEL);
		normal = g_pMatSystem->FindMaterial("flat_normal", TEXTURE_GROUP_MODEL);
	}

	else if (chamsType == 3)
	{
		ignorez = g_pMatSystem->FindMaterial("reflective_ignorez", TEXTURE_GROUP_MODEL);
		normal = g_pMatSystem->FindMaterial("reflective_normal", TEXTURE_GROUP_MODEL);
	}

	auto pLocal = g_pEntityList->GetClientEntity(g_pEngine->GetLocalPlayer());

	for (int i = 1; i <= g_pEngine->GetMaxClients(); i++)
	{
		C_BaseEntity* pEntity = g_pEntityList->GetClientEntity(i);
		if (!pEntity) continue;
		if (pEntity == pLocal) continue;
		if (pEntity->GetDormant()) continue;
		if (pEntity->GetHealth() < 1) continue;
		if (g_pVars.visuals.EnemyOnly && pEntity->GetTeam() == pLocal->GetTeam()) continue;

		if (!g_pVars.visuals.ChamsVisOnly)
		{
			g_pRenderView->SetColorModulation(1, 0, 0);
			g_pMdlRender->ForcedMaterialOverride(ignorez);
			pEntity->DrawModel(0x1/*STUDIO_RENDER*/, 255);
		}

		g_pRenderView->SetColorModulation(0, 1, 1);
		g_pMdlRender->ForcedMaterialOverride(normal);
		pEntity->DrawModel(0x1/*STUDIO_RENDER*/, 255);
		g_pMdlRender->ForcedMaterialOverride(nullptr);
	}

	/*if (g_Vars.rage.antiaim.Enabled && pLocal && pLocal->GetHealth() > 0)
	{
		QAngle* OrigAng = pLocal->EyeAngs();

		ignorez = g_pMatSystem->FindMaterial("reflective_ignorez", TEXTURE_GROUP_MODEL);
		normal = g_pMatSystem->FindMaterial("reflective_normal", TEXTURE_GROUP_MODEL);

		pLocal->SetAngle2(Vector(0, Globals::fakeYaw, 0));

		float col[3] = { 1, 0, 0 };

		g_pRenderView->SetColorModulation(1, 0, 0);
		g_pMdlRender->ForcedMaterialOverride(normal);
		pLocal->DrawModel(0x1, 255);
		g_pMdlRender->ForcedMaterialOverride(nullptr);
		pLocal->SetAngle2(Vector(OrigAng->pitch, OrigAng->yaw, OrigAng->roll));
	}*/
}