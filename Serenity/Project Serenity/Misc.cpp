#include "Misc.h"

void Movement::Bhop()
{
	if (Globals::pCmd->buttons & IN_JUMP) {
		if (Globals::pLocal->GetFlags() & FL_ONGROUND || Globals::pLocal->GetMoveType() & MOVETYPE_LADDER)
		{
			Globals::pCmd->buttons |= IN_JUMP;
		}
		else
		{
			Globals::pCmd->buttons &= ~IN_JUMP;
		}
	}
}

void Movement::MemeWalk()
{
	CUserCmd* cmd = Globals::pCmd;

	if (cmd->forwardmove > 0)
	{
		cmd->buttons |= IN_BACK;
		cmd->buttons &= ~IN_FORWARD;
	}

	if (cmd->forwardmove < 0)
	{
		cmd->buttons |= IN_FORWARD;
		cmd->buttons &= ~IN_BACK;
	}

	if (cmd->sidemove < 0)
	{
		cmd->buttons |= IN_MOVERIGHT;
		cmd->buttons &= ~IN_MOVELEFT;
	}

	if (cmd->sidemove > 0)
	{
		cmd->buttons |= IN_MOVELEFT;
		cmd->buttons &= ~IN_MOVERIGHT;
	}
}

void Misc::SetClantag(const char* clantag)
{
	static auto pattern = (int(__fastcall*)(const char*))g_pUtils->FindPatternIDA("engine.dll", "53 56 57 8B DA 8B F9 FF 15");
	pattern(clantag);
}

void Misc::ShowRanks()
{
	typedef char(__cdecl* revealranksfn)(int*);
	static auto pattern = g_pUtils->FindPatternIDA("client.dll", "55 8B EC 8B 0D ? ? ? ? 68");
	int array[3] = { 0, 0, 0 };
	reinterpret_cast<revealranksfn>(pattern)(array);
}