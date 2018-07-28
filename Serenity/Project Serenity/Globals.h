#pragma once
#include "SDK.h"

namespace Globals
{
	extern bool bSendPacket;
	extern CUserCmd* pCmd;
	extern C_BaseEntity* pLocal;
	extern int resolverType[64];
	extern float lbyPredictTime;
	extern int previousChoked;
}