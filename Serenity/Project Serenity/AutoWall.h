#pragma once

#include "SDK.h"

struct FireBulletData
{
	Vector           src;
	trace_t          enter_trace;
	Vector           direction;
	CTraceFilter    filter;
	float           trace_length;
	float           trace_length_remaining;
	float           current_damage;
	int             penetrate_count;
};

class Autowall
{
public:
	bool CanHit(const Vector &point, float *damage_given);
	void ScaleDamage(int hitgroup, C_BaseEntity *enemy, float weapon_armor_ratio, float &current_damage);
}; extern Autowall autowall;