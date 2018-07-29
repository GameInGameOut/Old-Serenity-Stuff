#pragma once
#include "sdk.h"

struct BacktrackData
{
	Vector headPosition;
	float simulationTime;
};

class Backtracking
{
public:
	bool Run(CUserCmd* _cmd, C_BaseEntity* _localPlayer, int _maxTick);
	int bestTarget = -1;
	BacktrackData entityPostionData[64][12];

private:
	void GetBestTarget(CUserCmd* _cmd, C_BaseEntity* _localPlayer, int _maxTick);
	bool BacktrackBestTarget(CUserCmd* _cmd, C_BaseEntity* _localPlayer, int _maxTick);

}; extern Backtracking legitBacktrack;