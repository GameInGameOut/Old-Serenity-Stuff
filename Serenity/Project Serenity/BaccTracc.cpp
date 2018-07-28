#include "BaccTracc.h"

#define TICK_INTERVAL (g_pGlobalVars->interval_per_tick)
#define TIME_TO_TICKS(dt) (floorf((0.5f + (float)(dt) / TICK_INTERVAL)))
#define TICKS_TO_TIME(t) (TICK_INTERVAL * (t))

template <class T, class U>
inline T clamp(T in, U low, U high)
{
	if (in <= low)
		return low;
	else if (in >= high)
		return high;
	else
		return in;
}



void BaccTracc::AnimationFix(ClientFrameStage_t stage)
{
	C_BaseEntity* pLocal = g_pEntityList->GetClientEntity(g_pEngine->GetLocalPlayer());

	if (!pLocal)
		return;

	if (pLocal->GetHealth() <= 0)
		return;

	static int userId[64];
	static AnimationLayer
		backupLayersUpdate[64][15],
		backupLayersInterp[64][15];

	for (int i = 1; i < g_pGlobalVars->maxClients; i++)
	{
		C_BaseEntity* player = g_pEntityList->GetClientEntity(i);

		if (!player || 	player == pLocal || player->GetTeam() == pLocal->GetTeam() || !player->IsPlayer() || player->GetDormant())
			continue;

		player_info_t player_info;
		g_pEngine->GetPlayerInfo(i, &player_info);

		switch (stage)
		{
		case ClientFrameStage_t::FRAME_NET_UPDATE_START:
			userId[i] = player_info.userId;
			memcpy(&backupLayersUpdate[i], player->GetAnimOverlays(), (sizeof AnimationLayer) * player->GetNumAnimOverlays());
			break;
		case ClientFrameStage_t::FRAME_RENDER_START:
			if (userId[i] != player_info.userId) continue;
			memcpy(&backupLayersInterp[i], player->GetAnimOverlays(), (sizeof AnimationLayer) * player->GetNumAnimOverlays());
			memcpy(player->GetAnimOverlays(), &backupLayersUpdate[i], (sizeof AnimationLayer) * player->GetNumAnimOverlays());
			break;
		case ClientFrameStage_t::FRAME_RENDER_END:
			if (userId[i] != player_info.userId) continue;
			memcpy(player->GetAnimOverlays(), &backupLayersInterp[i], (sizeof AnimationLayer) * player->GetNumAnimOverlays());
			break;
		default:
			return;
		}
	}
}

int BaccTracc::GetLatency()
{
	double v0;
	INetChannelInfo* v1;
	INetChannelInfo* v2;
	float v3;
	float v4;

	v1 = (INetChannelInfo*)g_pEngine->GetNetChannelInfo();
	v2 = (INetChannelInfo*)g_pEngine->GetNetChannelInfo();

	v3 = v1->GetAvgLatency(1);
	v4 = v2->GetAvgLatency(0);

	float interval_per_tick = 1.0f / g_pGlobalVars->interval_per_tick;

	return floorf(((v3 + v4) * interval_per_tick) + 0.5f);
}

bool BaccTracc::IsTickValid(int tick)
{
	INetChannelInfo *nci = g_pEngine->GetNetChannelInfo();

	if (!nci)
		return false;

	float correct = clamp(nci->GetLatency(FLOW_OUTGOING) + GetLerpTime(), 0.f, 1.f);

	float deltaTime = correct - (g_pGlobalVars->curtime - TICKS_TO_TIME(tick));

	return fabsf(deltaTime) < 0.2f;
}

float BaccTracc::EstimateServerTime()
{
	double v0;
	INetChannelInfo* v1;
	INetChannelInfo* v2;
	float v3;
	float v4;

	v1 = (INetChannelInfo*)g_pEngine->GetNetChannelInfo();
	v2 = (INetChannelInfo*)g_pEngine->GetNetChannelInfo();

	v3 = v1->GetAvgLatency(1);
	v4 = v2->GetAvgLatency(0);

	return v3 + v4 + TICKS_TO_TIME(1) + TICKS_TO_TIME(Globals::pCmd->tick_count);
}

float BaccTracc::GetNetworkLatency()
{
	INetChannelInfo* nci = g_pEngine->GetNetChannelInfo();
	if (nci) {
		float OutgoingLatency = nci->GetLatency(0);
		return OutgoingLatency;
	}
	return 0.0f;
}

ConVar* minupdate;
ConVar* maxupdate;
ConVar* updaterate;
ConVar* interprate;
ConVar* cmin;
ConVar* cmax;
ConVar* interp;

float BaccTracc::GetLerpTime()
{
	if (!minupdate)
		minupdate = g_pICvar->FindVar(("sv_minupdaterate"));
	if (!maxupdate)
		maxupdate = g_pICvar->FindVar(("sv_maxupdaterate"));
	if (!updaterate)
		updaterate = g_pICvar->FindVar(("cl_updaterate"));
	if (!interprate)
		interprate = g_pICvar->FindVar(("cl_interp_ratio"));
	if (!cmin)
		cmin = g_pICvar->FindVar(("sv_client_min_interp_ratio"));
	if (!cmax)
		cmax = g_pICvar->FindVar(("sv_client_max_interp_ratio"));
	if (!interp)
		interp = g_pICvar->FindVar(("cl_interp"));

	float UpdateRate = updaterate->GetFloat();
	float LerpRatio = interprate->GetFloat();

	return max(LerpRatio / UpdateRate, interp->GetFloat());
}

void BaccTracc::UpdateBacktrackRecords(C_BaseEntity* pEnt)
{
	int i = pEnt->GetIndex();

	for (int j = baccTraccer.PlayerRecord[i].records.size() - 1; j >= 0; j--)
	{
		float lerptime = baccTraccer.GetLerpTime();
		float desired_time = baccTraccer.PlayerRecord[i].records.at(j).m_flSimulationTime + lerptime;
		float estimated_time = baccTraccer.EstimateServerTime();
		float SV_MAXUNLAG = 1.0f;
		float latency = baccTraccer.GetNetworkLatency();
		float m_flLerpTime = baccTraccer.GetLerpTime();
		float correct = clamp<float>(latency + m_flLerpTime, 0.0f, SV_MAXUNLAG);
		float deltaTime = correct - (estimated_time + lerptime - desired_time);

		if (fabs(deltaTime) > 0.2f)
			baccTraccer.PlayerRecord[i].records.erase(baccTraccer.PlayerRecord[i].records.begin() + j);
	}

	static Vector old_origin[64];

	if (PlayerRecord[i].records.size() > 0 && pEnt->SimTime() == PlayerRecord[i].records.back().m_flSimulationTime)
		return;

	if (PlayerRecord[i].records.size() > 12 && PlayerRecord[i].records.back().m_flSimulationTime > pEnt->SimTime())
	{
		PlayerRecord[i].records.clear();
		return;
	}

	Vector cur_origin = pEnt->GetOrigin();
	Vector v = cur_origin - old_origin[i];
	bool breaks_lagcomp = v.LengthSqr() > 4096.f;
	old_origin[i] = cur_origin;
	TiccRecord new_record;

	new_record.needs_extrapolation = breaks_lagcomp;
	static float OldLower[64];

	PlayerRecord[i].LowerBodyYawTarget = pEnt->Lby();
	new_record.m_angEyeAngles = pEnt->EyeAngs();
	new_record.m_flCycle = pEnt->GetCycle();
	new_record.m_flSimulationTime = pEnt->SimTime();
	new_record.m_flAnimTime = pEnt->GetAnimationTime();
	new_record.bLowerBodyYawUpdated = false;
	new_record.m_nSequence = pEnt->GetSequence();
	new_record.m_vecOrigin = pEnt->GetOrigin();
	new_record.m_vecVelocity = pEnt->GetVelocity();
	new_record.m_flUpdateTime = g_pGlobalVars->curtime;
	new_record.backtrack_time = new_record.m_flSimulationTime + GetLerpTime();

	if (PlayerRecord[i].LowerBodyYawTarget != OldLower[i] || (pEnt->GetFlags() & FL_ONGROUND && pEnt->GetVelocity().Length() > 29.f))
		new_record.bLowerBodyYawUpdated = true;

	for (int i = 0; i < 24; i++)
		new_record.m_flPoseParameter[i] = *(float*)((DWORD)pEnt + NetVars.m_flPoseParameter + sizeof(float) * i);

	pEnt->SetupBones(new_record.boneMatrix, 128, 0x00000100, g_pGlobalVars->curtime);

	OldLower[i] = PlayerRecord[i].LowerBodyYawTarget;
	PlayerRecord[i].records.push_back(new_record);
}

void BaccTracc::UpdateExtrapolationRecords(C_BaseEntity* pEnt)
{
	int index = pEnt->GetIndex();
	if (pEnt->SimTime() == this->simRecord[index][0].simulation_time)
		return;

	for (int i = 7; i > 0; i--) {
		this->simRecord[index][i].acceleration = this->simRecord[index][i - 1].acceleration;
		this->simRecord[index][i].origin = this->simRecord[index][i - 1].origin;
		this->simRecord[index][i].simulation_time = this->simRecord[index][i - 1].simulation_time;
		this->simRecord[index][i].update_time = this->simRecord[index][i - 1].update_time;
		this->simRecord[index][i].velocity = this->simRecord[index][i - 1].velocity;
	}

	this->simRecord[index][0].simulation_time = pEnt->SimTime();
	this->simRecord[index][0].update_time = g_pGlobalVars->curtime;
	this->simRecord[index][0].origin = pEnt->GetOrigin();

	int lost_ticks = TIME_TO_TICKS(this->simRecord[index][0].simulation_time) - TIME_TO_TICKS(this->simRecord[index][1].simulation_time);

	this->simRecord[index][0].simulation_time_increasment_per_tick = (this->simRecord[index][0].simulation_time - this->simRecord[index][1].simulation_time) / lost_ticks;

	Vector velocity = this->simRecord[index][0].origin - this->simRecord[index][1].origin;

	velocity /= lost_ticks;

	this->simRecord[index][0].velocity = pEnt->GetVelocity();
}

void BaccTracc::ExtrapolatePosition(C_BaseEntity* pPlayer, Vector& position, float& simtime, Vector velocity)
{
	C_BaseEntity* pLocal = g_pEntityList->GetClientEntity(g_pEngine->GetLocalPlayer());

	int index = pPlayer->GetIndex();
	SimRecord latest_record = this->simRecord[index][0];
	SimRecord pre_latest_record = this->simRecord[index][1];
	int latency_ticks = GetLatency();
	Vector vel = velocity;
	float s_time = simtime;
	INetChannelInfo* nci = g_pEngine->GetNetChannelInfo();
	float latency = nci->GetAvgLatency(0) + nci->GetAvgLatency(1);
	float ticks_per_second = 1.0f / g_pGlobalVars->interval_per_tick;
	float server_time = ((floorf(((latency)*ticks_per_second) + 0.5) + Globals::pCmd->tick_count + 1) * g_pGlobalVars->interval_per_tick);
	float server_time_difference = server_time - s_time;

	if (server_time_difference > 1.0f)
		server_time_difference = 1.0f;

	float sim_time_difference = g_pGlobalVars->curtime - latest_record.update_time;

	int ticks_choked = TIME_TO_TICKS(sim_time_difference);

	if (ticks_choked <= 15) {
		latency_ticks = 1;
		if (ticks_choked < 1)
			ticks_choked = 1;
	}
	else
		ticks_choked = 15;

	int total_difference = floorf((server_time_difference * ticks_per_second) + 0.5) - ticks_choked;

	vel.z -= g_pICvar->FindVar(("sv_gravity"))->GetFloat() * g_pGlobalVars->interval_per_tick;

	Ray_t ray;
	trace_t tr;
	CTraceFilterWorldAndPropsOnly filter;

	if (total_difference < 0) {
		return;
	}
	else {
		do {
			int chokes = ticks_choked;
			do {

				ray.Init(position, position + (vel * g_pGlobalVars->interval_per_tick), pLocal->GetCollideable()->OBBMins(), pLocal->GetCollideable()->OBBMaxs());

				g_pEngineTrace->TraceRay(ray, MASK_SOLID, &filter, &tr);

				if (tr.fraction > 0) {
					position += vel * (g_pGlobalVars->interval_per_tick);
					simtime += g_pGlobalVars->interval_per_tick;
				}
				else {

					return;
				}

				chokes--;

			}

			while (chokes);

			total_difference -= ticks_choked;
		}

		while (total_difference >= 0);
	}
}


BaccTracc baccTraccer;