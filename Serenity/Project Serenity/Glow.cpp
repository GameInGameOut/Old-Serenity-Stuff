#include "Glow.h"

void Glow::Start() 
{
	auto GlowObjectManager = g_pGlowObjectManager;
	GlowObjectManager::GlowObjectDefinition_t* glowEntity;
	auto local = g_pEntityList->GetClientEntity(g_pEngine->GetLocalPlayer());

	for (int i = 0; i < GlowObjectManager->size; i++)
	{
		glowEntity = &GlowObjectManager->m_GlowObjectDefinitions[i];
		auto Entity = glowEntity->getEntity();
		if (!Entity) continue;
		if (g_pVars.visuals.EnemyOnly && Entity->GetTeam() == local->GetTeam()) continue;
		auto EntityClass = Entity->GetClientClass();
		if (EntityClass->m_ClassID == ClassId_CCSPlayer) {
			Color cglow;
			cglow = Entity->GetTeam() == local->GetTeam() ? Color(0, 255, 0, 180) : Color(255, 0, 0, 180);
			glowEntity->set(cglow);
		}
	}
}