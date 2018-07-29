pragma once
#include "SDK.h"
#include <list>

enum ResolveMode
{
	None,
	LastMoving,
	LbyUpdate,
	Freestanding,

};

class ResolverInfo
{
public:
	bool isFakewalking, shouldResolve, useLastMovingLby, useMovingDelta, isLbyUpdating, isLbyFlicking, isLbyPredicted;
	float lby, lastMovingLby, lastMovingTime, lastMovingDelta, lastUpdateTime, nextPredictedUpdate;

	float resolvedPitch, resolvedYaw, networkedYaw;
	int shotsHit[20], shotsFired[20], shotsMissedMoving, shotsMissedMovingDelta;



	Vector vecVelocity, vecOrigin;
};

Succ_pener