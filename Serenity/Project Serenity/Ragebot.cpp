#include "RageBot.h"
#include "AutoWall.h"

#define TICK_INTERVAL			(g_pGlobalVars->interval_per_tick)

#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )

#define TICKS_TO_TIME( t )      ( g_pGlobalVars->interval_per_tick * ( t ) )

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


QAngle CalculateAngle(Vector vecOrigin, Vector vecOther)
{
	auto ret = Vector();
	Vector delta = vecOrigin - vecOther;
	double hyp = delta.Length2D();
	ret.y = (atan(delta.y / delta.x) * 57.295779513082f);
	ret.x = (atan(delta.z / hyp) * 57.295779513082f);
	ret.z = 0.f;

	if (delta.x >= 0.f)
		ret.y += 180.f;
	QAngle finalQAngle;
	finalQAngle.pitch = ret.x;
	finalQAngle.yaw = ret.y;
	finalQAngle.roll = ret.z;

	return finalQAngle;
}

float Normalize(float value)
{
	while (value > 180)
		value -= 360.f;

	while (value < -180)
		value += 360.f;
	return value;
}

float Rand_f(float min, float max)
{
	typedef float(*RandomFloat_t)(float, float);
	return ((RandomFloat_t)GetProcAddress(GetModuleHandle("vstdlib.dll"), "RandomFloat"))(min, max);
}

void AngleVectors(const Vector& angles, Vector* forward, Vector* right, Vector* up)
{
	auto SinCos = [](float flRadians, float* pflSine, float* pflCosine) {
		__asm
		{
			fld	DWORD PTR[flRadians]
			fsincos
			mov edx, DWORD PTR[pflCosine]
			mov eax, DWORD PTR[pflSine]
			fstp DWORD PTR[edx]
			fstp DWORD PTR[eax]
		}
	};
	float sr, sp, sy, cr, cp, cy;
	SinCos(DEG2RAD(angles[1]), &sy, &cy);
	SinCos(DEG2RAD(angles[0]), &sp, &cp);
	SinCos(DEG2RAD(angles[2]), &sr, &cr);

	if (forward) {
		forward->x = cp * cy;
		forward->y = cp * sy;
		forward->z = -sp;
	}

	if (right) {
		right->x = (-1 * sr * sp * cy + -1 * cr * -sy);
		right->y = (-1 * sr * sp * sy + -1 * cr * cy);
		right->z = -1 * sr * cp;
	}

	if (up) {
		up->x = (cr * sp * cy + -sr * -sy);
		up->y = (cr * sp * sy + -sr * cy);
		up->z = cr * cp;
	}
}

std::vector<int> GetHitboxesToScan(C_BaseEntity* pTarget)
{
	std::vector<int> HitBoxesToScan;

	bool hHead = g_pVars.rage.hitboxes.Head, hChest = g_pVars.rage.hitboxes.Chest, hStomach = g_pVars.rage.hitboxes.Stomach, hPelvis = g_pVars.rage.hitboxes.Pelvis, hArms = g_pVars.rage.hitboxes.Arms, hLegs = g_pVars.rage.hitboxes.Legs, hFeet = g_pVars.rage.hitboxes.Feet;

	if (hHead)
	{
		HitBoxesToScan.push_back(HITBOX_HEAD);
	}

	if (hChest)
	{
		HitBoxesToScan.push_back(HITBOX_THORAX);
		HitBoxesToScan.push_back(HITBOX_UPPER_CHEST);
		HitBoxesToScan.push_back(HITBOX_LOWER_CHEST);
	}
	
	if (hStomach)
	{
		HitBoxesToScan.push_back(HITBOX_BELLY);
	}

	if (hPelvis)
	{
		HitBoxesToScan.push_back(PELVIS);
	}

	if (hArms)
	{
		HitBoxesToScan.push_back(HITBOX_RIGHT_UPPER_ARM);
		HitBoxesToScan.push_back(HITBOX_RIGHT_FOREARM);
		HitBoxesToScan.push_back(HITBOX_RIGHT_HAND);
		HitBoxesToScan.push_back(HITBOX_LEFT_UPPER_ARM);
		HitBoxesToScan.push_back(HITBOX_LEFT_FOREARM);
		HitBoxesToScan.push_back(HITBOX_LEFT_HAND);
	}
		
	if (hLegs)
	{
		HitBoxesToScan.push_back(HITBOX_RIGHT_THIGH);
		HitBoxesToScan.push_back(HITBOX_LEFT_THIGH);
	}
		
	if(hFeet)
	{
		HitBoxesToScan.push_back(HITBOX_RIGHT_FOOT);
		HitBoxesToScan.push_back(HITBOX_LEFT_FOOT);
	}

	return HitBoxesToScan;
}

std::vector<Vector> GetMultiplePointsForHitbox(C_BaseEntity* pBaseEntity, int iHitbox, matrix3x4_t BoneMatrix[128])
{
	C_BaseEntity* pLocal = g_pEntityList->GetClientEntity(g_pEngine->GetLocalPlayer());

	auto VectorTransform_Wrapper = [](const Vector& in1, const matrix3x4_t& in2, Vector& out) {
		auto VectorTransform = [](const float* in1, const matrix3x4_t& in2, float* out) {
			auto DotProducts = [](const float* v1, const float* v2) {
				return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
			};
			out[0] = DotProducts(in1, in2[0]) + in2[0][3];
			out[1] = DotProducts(in1, in2[1]) + in2[1][3];
			out[2] = DotProducts(in1, in2[2]) + in2[2][3];
		};
		VectorTransform(&in1.x, in2, &out.x);
	};

	studiohdr_t* pStudioModel = g_pMdlInfo->GetStudioModel(pBaseEntity->GetModel());
	mstudiohitboxset_t* set = pStudioModel->pHitboxSet(0);
	mstudiobbox_t* hitbox = set->pHitbox(iHitbox);

	std::vector<Vector> vecArray;

	Vector max;
	Vector min;
	VectorTransform_Wrapper(hitbox->bbmax, BoneMatrix[hitbox->bone], max);
	VectorTransform_Wrapper(hitbox->bbmin, BoneMatrix[hitbox->bone], min);

	auto center = (min + max) * 0.5f;

	QAngle CurrentAngles = CalculateAngle(center, pLocal->GetEyePos());

	Vector Forward;
	Math::AngleVectors(CurrentAngles, Forward);

	Vector Right = Forward.Cross(Vector(0, 0, 1));
	Vector Left = Vector(-Right.x, -Right.y, Right.z);

	Vector Top = Vector(0, 0, 1);
	Vector Bot = Vector(0, 0, -1);

	switch (iHitbox) {
	case (int)HITBOX_HEAD:
		for (auto i = 0; i < 4; ++i) {
			vecArray.emplace_back(center);
		}
		vecArray[1] += Top * (hitbox->m_flRadius * g_pVars.rage.Pointscale);
		vecArray[2] += Right * (hitbox->m_flRadius * g_pVars.rage.Pointscale);
		vecArray[3] += Left * (hitbox->m_flRadius * g_pVars.rage.Pointscale);
		break;

	default:

		for (auto i = 0; i < 3; ++i) {
			vecArray.emplace_back(center);
		}
		vecArray[1] += Right * (hitbox->m_flRadius * g_pVars.rage.Pointscale);
		vecArray[2] += Left * (hitbox->m_flRadius * g_pVars.rage.Pointscale);
		break;
	}
	return vecArray;
}

std::vector<Vector> MainHitbox(C_BaseEntity* pTarget, matrix3x4_t BoneMatrix[128])
{
	std::vector<Vector> Points;
	int Aimspot = g_pVars.rage.hitboxes.Hitbox;

	switch (Aimspot) {
	case 0:
		Points = GetMultiplePointsForHitbox(pTarget, 0, BoneMatrix);
		break;
	case 1:
		Points = GetMultiplePointsForHitbox(pTarget, 1, BoneMatrix);
		break;
	case 2:
		Points = GetMultiplePointsForHitbox(pTarget, 6, BoneMatrix);
		break;
	case 3:
		Points = GetMultiplePointsForHitbox(pTarget, 3, BoneMatrix);
		break;
	case 4:
		Points = GetMultiplePointsForHitbox(pTarget, 4, BoneMatrix);
		break;
	case 5:
		Points = GetMultiplePointsForHitbox(pTarget, 0, BoneMatrix);
		break;
	}
	return Points;
}

float GetHitChance()
{
	C_BaseEntity* pLocal = g_pEntityList->GetClientEntity(g_pEngine->GetLocalPlayer());

	C_BaseWeapon* pWeapon = pLocal->GetActiveWeapon();

	if (!pLocal)
		return 0;

	if (!pWeapon)
		return 0;

	if (pWeapon->GetItemDefinitionIndex() == WEAPON_TASER)
		return 0;

	float hitchance = 101; //lol idk why, its pasted anyway so w/e
	float inaccuracy = pWeapon->GetInaccuracy();

	if (inaccuracy == 0)
		inaccuracy = 0.0000001;

	inaccuracy = 1 / inaccuracy;
	hitchance = inaccuracy;

	return hitchance;
}

void AutoRevolver(C_BaseEntity* pLocal, C_BaseWeapon* pWeapon)
{
	Globals::pCmd->buttons |= IN_ATTACK;
	float timeToReady = pWeapon->TimeToFireReady();
	
	if(timeToReady > 0 && timeToReady < g_pGlobalVars->curtime)
		Globals::pCmd->buttons &= ~IN_ATTACK;
}

float GetFov(const Vector &_from, const Vector &_to)
{
	return RAD2DEG(acos(_from.Dot(_to) / _from.LengthSqr()));
}

void RageBot::Run(CUserCmd* pCmd)
{
	//OPTIMISE THIS. THE AMMOUNT OF TIME I GET PLOCAL LOL
	C_BaseEntity* pLocal = g_pEntityList->GetClientEntity(g_pEngine->GetLocalPlayer());

	int pLocalTeam = pLocal->GetTeam();

	Vector localViewAngles = pCmd->viewangles + pLocal->GetPunchAngles() * 2.f;

	//Null check
	if (!pLocal || pLocal->GetHealth() <= 0)
		return;

	//Getting localplayer's weapon
	C_BaseWeapon* pWeapon = pLocal->GetActiveWeapon();

	if (g_pVars.rage.AutoFire && pWeapon->GetItemDefinitionIndex() == WEAPON_REVOLVER)
		AutoRevolver(pLocal, pWeapon);

	//Is the weapon valid. CAN OPTIMISE HERE
	if (!pWeapon || pWeapon->Clip1() == 0)
		return;


	//Add a max FOV here
	float bestFov = 180;

	//The selected enemy. = invalid rn
	int target = -1;

	//The position to aim at
	Vector bestAimPos;

	//Best simtime?? TO_CHECK
	float bestSimtime;

	//The best entity to target
	C_BaseEntity* bestEnt = pLocal;

	//Ent loop
	for (int i = 1; i < g_pGlobalVars->maxClients; i++)
	{
		//Setting the current entity
		 C_BaseEntity* pEnt = g_pEntityList->GetClientEntity(i);

		//Basic valid ent checks
		if (!pEnt || pEnt->GetHealth() <= 0 || pEnt->GetDormant() || pEnt == pLocal || pEnt->GetTeam() == pLocalTeam)
			continue;

		//Getting the FOV to the currently selected entity

		Vector targetsHeadPosition = pEnt->GetHitboxPos(0);
		Vector aimAngle = Math::CalculateAngle(pLocal->GetEyePos(), targetsHeadPosition);

		float fov = GetFov(localViewAngles, aimAngle);


		//Checking it is higher than the maximum / best found fov
		if (fov > bestFov)
			continue;

		//TO_DEL Possible crashes??????
		float simtime = 0;

		Vector minus_origin = Vector(0, 0, 0);

		//Getting the position to aiom at
		Vector aimPos = GetNewAimpoint(pLocal, pEnt, simtime, minus_origin);

		if (aimPos == Vector(0, 0, 0))
			continue;

		//Setting the best FOV as the fov to the current entity
		bestFov = fov;

		//Setting the target index? TO_DEL
		target = i;

		//Setting the best entity that we should aim at
		bestEnt = pEnt;

		//Setting the best position to aim at
		bestAimPos = aimPos;

		//Setting the best simtime
		bestSimtime = simtime;
	}

	if (target != -1 && bestAimPos != Vector(0, 0, 0) && bestEnt && bestEnt != pLocal)
	{
		//Getting the angle to aim at
		Vector aimAng = Math::CalculateAngle(pLocal->GetEyePos(), bestAimPos);

		//Normalising it, duh
		Math::NormalizeAngle(aimAng);

		//Setting the viewangs. ADD PSILENT OPTION HERE
		Globals::pCmd->viewangles = aimAng;

		//ADD AUTOSCOPE

		//Checking if the hitchance meets the requirement
		if (GetHitChance() >= g_pVars.rage.Hitchance)
		{
			//Shooting lol
			Globals::pCmd->buttons |= IN_ATTACK;
		}

		Globals::pCmd->tick_count = TIME_TO_TICKS(bestSimtime); // + TIME_TO_TICKS(baccTraccer.GetLerpTime()) BROKE DONT USE LERP

	}
}

void RageBot::RCS(CUserCmd* pCmd)
{
	//Getting pLocal again lolol
	C_BaseEntity* pLocal = g_pEntityList->GetClientEntity(g_pEngine->GetLocalPlayer());

	if (pLocal)
	{
		float AimPunch = g_pICvar->FindVar("weapon_recoil_scale")->GetFloat();

		if (pCmd->buttons & IN_ATTACK)
		{
			if (AimPunch)
			{
				Vector PunchAngles = pLocal->GetPunchAngles();
				Vector AimAngles = pCmd->viewangles;
				AimAngles -= PunchAngles * AimPunch;
				pCmd->viewangles = AimAngles;
			}
		}
	}
}


Vector RageBot::GetNewAimpoint(C_BaseEntity* pLocal, C_BaseEntity* pTarget, float& simtime, Vector& origin)
{
	Vector vEyePos = pLocal->GetEyePos();

	C_BaseWeapon* pWeapon = pLocal->GetActiveWeapon();

	if (!pWeapon)
		return Vector(0,0,0);

	CCSWeaponInfo* wpn_data = pWeapon->GetCSWeaponData();
	float minimum_damage = g_pVars.rage.MinimumDamage;

	bool found = false;

	matrix3x4_t BoneMatrix[128];

	if (g_pVars.rage.BaccTracc)
	{
		std::vector<TiccRecord> lby_records, trash_records;

		float latest_time = 0.0f;
		TiccRecord latest_record;

		for (int j = baccTraccer.PlayerRecord[pTarget->GetIndex()].records.size() - 1; j >= 0; j--)
		{
			TiccRecord record = baccTraccer.PlayerRecord[pTarget->GetIndex()].records.at(j);

			float lerptime = baccTraccer.GetLerpTime();
			float desired_time = record.m_flSimulationTime + lerptime;
			float estimated_time = baccTraccer.EstimateServerTime();
			float SV_MAXUNLAG = 1.0f;
			float latency = baccTraccer.GetNetworkLatency();
			float m_flLerpTime = baccTraccer.GetLerpTime();
			float correct = clamp<float>(latency + m_flLerpTime, 0.0f, SV_MAXUNLAG);
			float deltaTime = correct - (estimated_time + lerptime - desired_time);

			if (fabs(deltaTime) > 0.1f)
				continue;

			if (record.m_flSimulationTime == baccTraccer.PlayerRecord[pTarget->GetIndex()].records.back().m_flSimulationTime) 
			{
				latest_time = baccTraccer.PlayerRecord[pTarget->GetIndex()].records.at(j).m_flSimulationTime;
				latest_record = baccTraccer.PlayerRecord[pTarget->GetIndex()].records.at(j);
			}

			if (record.needs_extrapolation)
				continue;

			if (record.bLowerBodyYawUpdated)
				lby_records.emplace_back(record);
			else if (j != baccTraccer.PlayerRecord[pTarget->GetIndex()].records.size() - 1)
				trash_records.emplace_back(record);
		}

		bool was_latest_checked_in_lby = false;

		for (int i = 0; i < lby_records.size(); i++) 
		{
			TiccRecord record = lby_records.at(i);

			if (record.needs_extrapolation)
				continue;

			simtime = record.m_flSimulationTime;
			origin = record.m_vecOrigin;

			bool is_latest_record = record.m_flSimulationTime == latest_record.m_flSimulationTime;

			if (is_latest_record)
				was_latest_checked_in_lby = true;

			for (auto HitBox : MainHitbox(pTarget, record.boneMatrix))
			{
				int hitgroup = -1;

				if (is_latest_record)
				{
					static float damage_given = 0.f;

					if (autowall.CanHit(HitBox, &damage_given))
					{
						if (damage_given >= minimum_damage) 
						{
							if (HitBox != Vector(0, 0, 0))
								return HitBox;
						}
					}
				}
			}
		}
		
		if (!was_latest_checked_in_lby && !latest_record.needs_extrapolation)
		{
			was_latest_checked_in_lby = true;

			simtime = latest_record.m_flSimulationTime;
			origin = latest_record.m_vecOrigin;

			for (auto HitBox : MainHitbox(pTarget, latest_record.boneMatrix)) 
			{
				static float damage_given = 0.f;

				if (autowall.CanHit(HitBox, &damage_given)) 
				{
					if (damage_given >= minimum_damage) {
						if (HitBox != Vector(0, 0, 0))
							return HitBox;
					}
				}
			}
		}

		if (!latest_record.needs_extrapolation) {
			Vector vPoint;
			simtime = latest_record.m_flSimulationTime;
			origin = latest_record.m_vecOrigin;
			for (auto HitboxID : GetHitboxesToScan(pTarget)) {
				std::vector<Vector> Points = GetMultiplePointsForHitbox(pTarget, HitboxID, latest_record.boneMatrix);
				for (int k = 0; k < Points.size(); k++) {

					vPoint = Points.at(k);
					float damage = 0.f;

					if (k == 0) {
						static float damage_given = 0.f;

						if (g_pVars.rage.Hitscan && autowall.CanHit(vPoint, &damage_given)) {
							if (damage_given >= minimum_damage) {
								if (vPoint != Vector(0, 0, 0))
									return vPoint;
							}
						}
					}
				}
			}
		}

		if (latest_record.needs_extrapolation) {
			Vector position = latest_record.m_vecOrigin;
			float old_simtime = latest_record.m_flSimulationTime;
			;
			Vector extr_position = position;
			simtime = latest_record.m_flSimulationTime;
			baccTraccer.ExtrapolatePosition(pTarget, extr_position, simtime, latest_record.m_vecVelocity);

			for (auto HitBox : MainHitbox(pTarget, latest_record.boneMatrix)) {
				int hitgroup = -1;

				HitBox -= position;
				HitBox += extr_position;
			}
		}
	}

	return Vector(0, 0, 0);
}

RageBot* f_Rage;