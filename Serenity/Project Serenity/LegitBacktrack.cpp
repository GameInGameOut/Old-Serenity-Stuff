#include "LegitBacktrack.h"
#include "ESP.h"

#define TICK_INTERVAL			(g_pGlobalVars->interval_per_tick)
 
#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )

#define TICKS_TO_TIME( t )      ( g_pGlobalVars->interval_per_tick * ( t ) )

inline float DistPointToLine(Vector Point, Vector LineOrigin, Vector Dir)
{
	auto PointDir = Point - LineOrigin;

	auto TempOffset = PointDir.Dot(Dir) / (Dir.x*Dir.x + Dir.y*Dir.y + Dir.z*Dir.z);
	if (TempOffset < 0.000001f)
		return FLT_MAX;

	auto PerpendicularPoint = LineOrigin + (Dir * TempOffset);

	return (Point - PerpendicularPoint).Length();
}

bool Backtracking::Run(CUserCmd* _cmd, C_BaseEntity* _localPlayer, int _maxTick)
{
	if (_maxTick == 0)
		return false;

	if (this->bestTarget == -1)
		this->GetBestTarget(_cmd, _localPlayer, _maxTick);

	if (this->bestTarget != -1 && _cmd->buttons & IN_ATTACK)
		this->BacktrackBestTarget(_cmd, _localPlayer, _maxTick);

	this->bestTarget = -1;

	return true;
}

float GetFOV(const Vector &_from, const Vector &_to)
{
	return RAD2DEG(acos(_from.Dot(_to) / _from.LengthSqr()));
}

void Backtracking::GetBestTarget(CUserCmd* _cmd, C_BaseEntity* _localPlayer, int _maxTick)
{
	float bestFov = 180.f;

	Vector localViewAngles = _cmd->viewangles + _localPlayer->GetPunchAngles() * 2.f;

	for (int i = 0; i < g_pEngine->GetMaxClients(); ++i)
	{
		C_BaseEntity* entity = g_pEntityList->GetClientEntity(i);

		C_BaseEntity* pLocal = g_pEntityList->GetClientEntity(g_pEngine->GetLocalPlayer());

		if (!entity || !pLocal)
			continue;

		if (entity == pLocal)
			continue;

		if (entity->GetTeam() == pLocal->GetTeam())
			continue;


		Vector targetsHeadPosition = entity->GetHitboxPos(0);
		Vector aimAngle = Math::CalculateAngle(_localPlayer->GetEyePos(), targetsHeadPosition);

		float fov = GetFOV(localViewAngles, aimAngle);

		if (bestFov > fov)
		{
			this->bestTarget = i;
			bestFov = fov;
		}

		this->entityPostionData[i][_cmd->command_number % _maxTick] = { targetsHeadPosition, entity->SimTime() };
	}
}

bool Backtracking::BacktrackBestTarget(CUserCmd* _cmd, C_BaseEntity* _localPlayer, int _maxTick)
{
	float bestSimulationTime = 0.f;
	float bestFov = 180.f;

	Vector localViewAngles = _cmd->viewangles + _localPlayer->GetPunchAngles() * 2.f;

	for (int i = 0; i <= _maxTick; ++i)
	{
		if (!this->entityPostionData[this->bestTarget][i].simulationTime)
			continue;

		Vector aimAngle = Math::CalculateAngle(_localPlayer->GetEyePos(), this->entityPostionData[this->bestTarget][i].headPosition);
		float fov = GetFOV(localViewAngles, aimAngle);

		if (bestFov > fov)
		{
			bestSimulationTime = this->entityPostionData[this->bestTarget][i].simulationTime;
			bestFov = fov;
		}
	}

	if (!bestSimulationTime)
		return false;

	_cmd->tick_count = TIME_TO_TICKS(bestSimulationTime);

	return true;
}
Backtracking legitBacktrack;