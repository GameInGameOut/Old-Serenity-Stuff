#pragma once

class Variables
{
public:

	class Menu
	{
	public:
		bool OpenedMenu;
		bool OpenedVisuals;
		bool OpenedMisc;
		bool OpenedLegit;
		bool OpenedRage;
		bool OpenedHitboxes;
	}; Menu menu;

	class Visuals
	{
	public:
		bool Enabled;
		bool EnemyOnly;

		bool Box;
		int Snaplines;
		bool Name;
		bool Health;
		bool Weapon;
		bool Distance;
		bool Money;
		bool ResolverInfo;

		bool Glow;
		bool Backtrack;

		int ChamsStyle;
		bool ChamsVisOnly;
	}; Visuals visuals;

	class Misc
	{
	public:
		bool Bhop;
		bool FakeLatencyEnable;
		float FakeLatencyAmmount;
		bool MemeWalk;
		bool RevealRanks;
		int ClanTag;
		char ClanTagText;
	}; Misc misc;

	class Legit
	{
	public:
		bool Backtrack;
		int BacktrackTicks;
	}; Legit legit;

	class Rage
	{
	public:
		bool Enabled = false;
		bool AutoFire = true;
		bool BaccTracc = true;
		bool AnimationFix = true;
		bool Hitscan = true;
		bool Multipoint = true;
		bool RCS = true;
		float Pointscale = 7.f;
		int MinimumDamage = 30;
		int Hitchance = 30;

		class Hitboxes
		{
		public:
			bool Hitbox = 0;

			bool Head = true;
			bool Stomach = true;
			bool Pelvis = true;
			bool Arms = true;
			bool Chest = true;
			bool Legs = true;
			bool Feet = true;
		}; Hitboxes hitboxes;

		class AntiAim
		{
		public:
			bool Enabled;
			int Pitch;
			int Real;
			int Fake;
			bool LbyBreak;
			int LbyDelta;
		}; AntiAim antiaim;

	}; Rage rage;

}; extern Variables g_pVars;