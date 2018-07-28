#include "AntiAim.h"
#define clamp(val, min, max) (((val) > (max)) ? (max) : (((val) < (min)) ? (min) : (val)))
#define TICKS_TO_TIME( t )      ( g_pGlobalVars->interval_per_tick * ( t ) )
#define TICK_INTERVAL			(g_pGlobalVars->interval_per_tick)
#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )

int GetFPS()
{
	static int fps = 0;
	static int count = 0;
	using namespace std::chrono;
	auto now = high_resolution_clock::now();
	static auto last = high_resolution_clock::now();
	count++;

	if (duration_cast<milliseconds>(now - last).count() > 1000)
	{
		fps = count;
		count = 0;
		last = now;
	}

	return fps;
}

namespace AA
{
	void BreakLby(CUserCmd* pCmd)
	{
		static int choked = 0;
		static bool wasMoving = true;
		static bool preBreak = false;
		static bool shouldBreak = false;
		static bool brokeThisTick = false;
		static float oldCurtime = g_pGlobalVars->curtime;

		C_BaseEntity* pLocal = g_pEntityList->GetClientEntity(g_pEngine->GetLocalPlayer());

		if (Globals::bSendPacket)
		{
			brokeThisTick = false;
			choked = g_pClientState->chokedcommands;

			if (pLocal->GetVelocity().Length2D() >= 0.1f && (pLocal->GetFlags() & FL_ONGROUND))
			{
				oldCurtime = g_pGlobalVars->curtime;
				wasMoving = true;
			}
			else
			{
				if (wasMoving && g_pGlobalVars->curtime - oldCurtime > 0.22f)
				{
					wasMoving = false;
					shouldBreak = true;
					Globals::lbyPredictTime = g_pGlobalVars->curtime;
				}
				else if (g_pGlobalVars->curtime - oldCurtime > 1.1f)
				{
					shouldBreak = true;
					Globals::lbyPredictTime = g_pGlobalVars->curtime;
				}
				else if (g_pGlobalVars->curtime - oldCurtime > 1.1f - TICKS_TO_TIME(choked) - TICKS_TO_TIME(2))
					preBreak = true;
			}
		}
		else if (shouldBreak)
		{
			shouldBreak = false;
			brokeThisTick = true;
			oldCurtime = g_pGlobalVars->curtime;
			pCmd->viewangles.y += g_pVars.rage.antiaim.LbyDelta;
		}
		else if (preBreak)
		{
			preBreak = false;
			brokeThisTick = true;
			float addAngle = GetFPS() >= (TIME_TO_TICKS(1.f) * 0.5f) ? (2.9 * max(choked, Globals::previousChoked) + 100) : 145.f;
			pCmd->viewangles.y += g_pVars.rage.antiaim.LbyDelta + addAngle;
		}

		if (brokeThisTick)
		{
			if (pLocal->GetActiveWeapon()->GetItemDefinitionIndex() == WEAPON_REVOLVER)
			{
				pCmd->buttons &= ~IN_ATTACK;
				pCmd->buttons &= ~IN_ATTACK2;
			}
		}
	}

	void Treehouse(CUserCmd* pCmd)
	{
		pCmd->viewangles.y = rand() % 180;
	}
}

void VectorAngles(const Vector& vecForward, Vector& vecAngles)
{
	Vector vecView;
	if (vecForward[1] == 0.f && vecForward[0] == 0.f) {
		vecView[0] = 0.f;
		vecView[1] = 0.f;
	}
	else {
		vecView[1] = atan2(vecForward[1], vecForward[0]) * 180.f / M_PI;

		if (vecView[1] < 0.f)
			vecView[1] += 360;

		vecView[2] = sqrt(vecForward[0] * vecForward[0] + vecForward[1] * vecForward[1]);

		vecView[0] = atan2(vecForward[2], vecView[2]) * 180.f / M_PI;
	}

	vecAngles[0] = -vecView[0];
	vecAngles[1] = vecView[1];
	vecAngles[2] = 0.f;
}

void AntiAim::Fix(CUserCmd* cmd)
{
	auto local_player = g_pEntityList->GetClientEntity(g_pEngine->GetLocalPlayer());

	if (!local_player)
		return;

	cmd->forwardmove = clamp(cmd->forwardmove, -450.f, 450.f);
	cmd->sidemove = clamp(cmd->sidemove, -450.f, 450.f);
	cmd->upmove = clamp(cmd->upmove, -320.f, 320.f);

	cmd->viewangles.x = clamp(cmd->viewangles.x, -89.f, 89.f);

	QAngle real_viewangles;
	g_pEngine->GetViewAngles(real_viewangles);

	Vector vecMove(cmd->forwardmove, cmd->sidemove, cmd->upmove);
	float speed = sqrt(vecMove.x * vecMove.x + vecMove.y * vecMove.y);

	QAngle angMove;
	Math::VectorAngles(vecMove, angMove);

	float yaw = DEG2RAD(cmd->viewangles.y - real_viewangles.yaw + angMove.yaw);

	cmd->forwardmove = cos(yaw) * speed;
	cmd->sidemove = sin(yaw) * speed;

	cmd->viewangles = Math::NormalizeAngle(cmd->viewangles);

	if (cmd->viewangles.x < -89.f || cmd->viewangles.x > 89.f) cmd->forwardmove *= -1;
}

void AntiAim::Run(CUserCmd* pCmd)
{
	C_BaseEntity* pLocal = g_pEntityList->GetClientEntity(g_pEngine->GetLocalPlayer());

	if (!pLocal || pLocal->GetHealth() <= 0) return;

	if (pCmd->buttons & IN_USE || pCmd->buttons & IN_ATTACK) return;

	if (pLocal->GetMoveType() == MOVETYPE_LADDER)
		return;
		
	C_BaseWeapon* pLocalWeapon = pLocal->GetActiveWeapon();

	switch (g_pVars.rage.antiaim.Pitch)
	{
	case 0:
		pCmd->viewangles.x = 89.f;
		break;
	case 1:
		pCmd->viewangles.x = -180.f;
		break;
	case 2:
		pCmd->viewangles.x = 0.f;
	}

	if (!Globals::bSendPacket)
	{
		pCmd->viewangles.y = rand() % 180;
	}

	else
	{
		switch (g_pVars.rage.antiaim.Real)
		{
		case 1:
			AA::Treehouse(pCmd);
		}
	}

	if (g_pVars.rage.antiaim.LbyBreak)
		AA::BreakLby(pCmd);
}

AntiAim antiaim;