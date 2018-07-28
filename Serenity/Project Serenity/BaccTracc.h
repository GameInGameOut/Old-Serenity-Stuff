#pragma once
#include "SDK.h"

struct TiccRecord 
{
	Vector m_vecOrigin;
	bool bLowerBodyYawUpdated;
	bool bFakelagging;
	float m_flSimulationTime;
	float m_flAnimTime;
	int m_nSequence;
	float m_flCycle;
	QAngle* m_angEyeAngles;


	float m_flUpdateTime;
	Vector m_vecVelocity;
	float m_flPoseParameter[24];
	float backtrack_time;

	bool needs_extrapolation = false;
	matrix3x4_t boneMatrix[128];
};

struct plAyyerReccord
{
	std::vector<TiccRecord> records;
	Vector EyeAngles;
	float LowerBodyYawTarget;
	int Flags;
	float unknown;
};

struct SimRecord 
{
	Vector origin;
	Vector velocity;
	Vector acceleration;
	float simulation_time;
	float simulation_time_increasment_per_tick;
	float update_time;
};

class BaccTracc
{
public:
	plAyyerReccord PlayerRecord[64];
	SimRecord simRecord[64][7];
	float GetLerpTime();

	float GetNetworkLatency();
	void AnimationFix(ClientFrameStage_t stage);
	int GetLatency();
	bool IsTickValid(int tick);
	float EstimateServerTime();
	void UpdateBacktrackRecords(C_BaseEntity* pPlayer);
	void UpdateExtrapolationRecords(C_BaseEntity* pPlayer);
	void ExtrapolatePosition(C_BaseEntity* pPlayer, Vector& position, float& simtime, Vector velocity);
}; extern BaccTracc baccTraccer;

struct lbyRecords 
{
	int tick_count;
	float lby;
	Vector headPosition;
};

struct BaccTraccData 
{
	float simtime;
	Vector hitboxPos;
};

//class BaccTracc
//{
//public:
//	lbyRecords records[64];
//	bool RunLBYBackTrack(int i, CUserCmd* cmd, QAngle& aimPoint);
//	void Update(int tick_count);
//private:
//	int latest_tick;
//	bool IsTickValid(int tick);
//	void UpdateRecord(int i);
//}; extern BaccTracc baccTraccer;