#include "ImMenu.h"

namespace ImMenu
{
	int iTab;
	void GuiInit(HWND window, IDirect3DDevice9 *pDevice)
	{
		ImGui_ImplDX9_Init(window, pDevice);
		auto& style = ImGui::GetStyle();

		style.ChildWindowRounding = 0.f;
		style.GrabRounding = 0.f;
		style.WindowRounding = 0.f;
		style.ScrollbarRounding = 0.f;
		style.FrameRounding = 0.f;
		style.WindowTitleAlign = ImVec2(0.5f, 0.5f);

		style.Colors[ImGuiCol_Text] = ImVec4(0.86f, 0.93f, 0.89f, 0.78f);
		style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.86f, 0.93f, 0.89f, 0.78f);
		style.Colors[ImGuiCol_WindowBg] = ImColor(15, 15, 15, 255);
		style.Colors[ImGuiCol_ChildWindowBg] = ImColor(10, 10, 10, 255);
		style.Colors[ImGuiCol_Border] = ImColor(15, 15, 15, 255);
		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.09f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.04f, 0.04f, 0.04f, 0.88f);
		style.Colors[ImGuiCol_TitleBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.20f, 0.22f, 0.27f, 0.75f);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.60f, 0.78f, 1.00f);
		style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.09f, 0.15f, 0.16f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.15f, 0.60f, 0.78f, 0.78f);
		style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.15f, 0.60f, 0.78f, 1.00f);
		style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.15f, 0.60f, 0.78f, 1.00f);
		style.Colors[ImGuiCol_Button] = ImVec4(0.24f, 0.40f, 0.95f, 1.00f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.40f, 0.95f, 0.59f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
		style.Colors[ImGuiCol_Header] = ImVec4(0.24f, 0.40f, 0.95f, 1.00f);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.24f, 0.40f, 0.95f, 0.59f);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
		style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.70f, 0.02f, 0.60f, 0.22f);
		style.Colors[ImGuiCol_CloseButton] = ImVec4(0.24f, 0.40f, 0.95f, 1.00f);
		style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.24f, 0.40f, 0.95f, 0.59f);

		D3Init = true;
	}

	ImFont* mainText;

	void Open() 
	{
		static bool bDown = false;
		static bool bClicked = false;
		static bool bPrevMenuState = g_pVars.menu.OpenedMenu;

		if (pressedKey[VK_INSERT])
		{
			bClicked = false;
			bDown = true;
		}
		else if (!pressedKey[VK_INSERT] && bDown)
		{
			bClicked = true;
			bDown = false;
		}
		else
		{
			bClicked = false;
			bDown = false;
		}

		if (bClicked)
		{
			g_pVars.menu.OpenedMenu = !g_pVars.menu.OpenedMenu;
		}

		if (bPrevMenuState != g_pVars.menu.OpenedMenu)
		{
			std::string msg = "cl_mouseenable " + std::to_string(!g_pVars.menu.OpenedMenu);
			g_pEngine->ExecuteClientCmd(msg.c_str());
		}

		bPrevMenuState = g_pVars.menu.OpenedMenu;
	}

	void ButtonColor(int r, int g, int b)
	{
		ImGuiStyle& style = ImGui::GetStyle();

		style.Colors[ImGuiCol_Button] = ImColor(r, g, b);
		style.Colors[ImGuiCol_ButtonHovered] = ImColor(r, g, b);
		style.Colors[ImGuiCol_ButtonActive] = ImColor(r, g, b);
	}

	void styled()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowRounding = 0.f;
		style.FramePadding = ImVec2(4, 0);
		style.WindowPadding = ImVec2(0, 0);
		style.ItemSpacing = ImVec2(0, 0);
		style.ScrollbarSize = 10.f;
		style.ScrollbarRounding = 0.f;
		style.GrabMinSize = 5.f;
	}

	void color()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		style.Colors[ImGuiCol_Text] = ImVec4(0.86f, 0.93f, 0.89f, 0.78f);
		style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.86f, 0.93f, 0.89f, 0.78f);
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 0.95f);
		style.Colors[ImGuiCol_CheckMark] = ImColor(1, 1, 1, 255);

		style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 0.95f);
		style.Colors[ImGuiCol_Border] = ImColor(0, 0, 0, 255);
		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.184f, 0.310f, 0.310f, 0.95f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.184f, 0.310f, 0.310f, 0.95f);
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.184f, 0.310f, 0.310f, 0.95f);
		style.Colors[ImGuiCol_TitleBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.20f, 0.22f, 0.27f, 0.75f);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.60f, 0.78f, 1.00f);
		style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.09f, 0.15f, 0.16f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.15f, 0.60f, 0.78f, 0.78f);
		style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.15f, 0.60f, 0.78f, 1.00f);

		style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.15f, 0.60f, 0.78f, 1.00f);
		style.Colors[ImGuiCol_Button] = ImVec4(0.1f, 0.1f, 0.1f, 0.95f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.1f, 0.1f, 0.1f, 0.95f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.1f, 0.1f, 0.1f, 0.95f);
		style.Colors[ImGuiCol_Header] = ImVec4(0.24f, 0.40f, 0.95f, 1.00f);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.24f, 0.40f, 0.95f, 0.59f);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
		style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.70f, 0.02f, 0.60f, 0.22f);
		style.Colors[ImGuiCol_CloseButton] = ImVec4(0.24f, 0.40f, 0.95f, 1.00f);
		style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.24f, 0.40f, 0.95f, 0.59f);
	}
	
	void Legit()
	{
		ImGui::BeginChild("Misc", ImVec2(700 / 1, 400), true);
		{
			ImGui::Spacing();
			ImGui::Checkbox("Backtracking", &g_pVars.legit.Backtrack);

			if(g_pVars.misc.FakeLatencyEnable)
				ImGui::SliderInt("Ticks", &g_pVars.legit.BacktrackTicks, 1, 64);

			else
				ImGui::SliderInt("Ticks", &g_pVars.legit.BacktrackTicks, 1, 12);

			ImGui::Spacing();
			ImGui::Spacing();
		} ImGui::EndChild();
	}
	
	void Rage()
	{
		ImGui::BeginChild("Main", ImVec2(700 / 3, 400), true);
		{
			ImGui::Spacing();
			ImGui::Checkbox("Enabled", &g_pVars.rage.Enabled);

			ImGui::Spacing();
			ImGui::Spacing();

			ImGui::Checkbox("Recoil", &g_pVars.rage.RCS);
			ImGui::Checkbox("Multipoint", &g_pVars.rage.Multipoint);
			ImGui::Checkbox("Backtracking", &g_pVars.rage.BaccTracc);
			ImGui::Checkbox("Animation Fix", &g_pVars.rage.AnimationFix);
			ImGui::SliderFloat("Pointscale", &g_pVars.rage.Pointscale, 0, 1);

			ImGui::Spacing();
			ImGui::Spacing();

			ImGui::SliderInt("Min Damage", &g_pVars.rage.MinimumDamage, 0, 100);
			ImGui::SliderInt("Hitchance", &g_pVars.rage.Hitchance, 0, 100);
		}
		ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginChild("AA", ImVec2(700 / 3, 400), true);
		{
			static const char* pitch[] = { "Down", "Fake Down", "Up" };
			static const char* real[] = { "Arrow Key", "180 Treehouse" };

			ImGui::Checkbox("Enabled", &g_pVars.rage.antiaim.Enabled);

			ImGui::Spacing();

			ImGui::Combo("Pitch", &g_pVars.rage.antiaim.Pitch, pitch, IM_ARRAYSIZE(pitch));
			ImGui::Combo("Real Yaw", &g_pVars.rage.antiaim.Real, real, IM_ARRAYSIZE(real));

			ImGui::Spacing();

			ImGui::Checkbox("Lby Breaker", &g_pVars.rage.antiaim.LbyBreak);
			ImGui::SliderInt("Lby Delta", &g_pVars.rage.antiaim.LbyDelta, 10, 180);
		}
		ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginChild("Hitboxes", ImVec2(700 / 3, 400), true);
		{
			ImGui::Checkbox("Head", &g_pVars.rage.hitboxes.Head);
			ImGui::Checkbox("Chest", &g_pVars.rage.hitboxes.Chest);
			ImGui::Checkbox("Stomach", &g_pVars.rage.hitboxes.Stomach);
			ImGui::Checkbox("Pelvis", &g_pVars.rage.hitboxes.Pelvis);
			ImGui::Checkbox("Arms", &g_pVars.rage.hitboxes.Arms);
			ImGui::Checkbox("Legs", &g_pVars.rage.hitboxes.Feet);
			ImGui::Checkbox("Feet", &g_pVars.rage.hitboxes.Feet);
		}
		ImGui::EndChild();
	}
	
	void Visuals()
	{
		ImGui::BeginChild("ESP", ImVec2(700 / 3, 400), true);
		{
			ImGui::Spacing();
			static const char* snaplines[] = { "Off", "Center", "Bottom", "Top" };

			ImGui::Checkbox("Enabled", &g_pVars.visuals.Enabled);
			ImGui::Checkbox("Enemy Only", &g_pVars.visuals.EnemyOnly);

			ImGui::Spacing();
			ImGui::Spacing();

			ImGui::Checkbox("Box", &g_pVars.visuals.Box);

			ImGui::Spacing();
			ImGui::Text("Snaplines");
			ImGui::Combo("Snaplines", &g_pVars.visuals.Snaplines, snaplines, IM_ARRAYSIZE(snaplines));
			ImGui::Spacing();

			ImGui::Checkbox("Name", &g_pVars.visuals.Name);
			ImGui::Checkbox("Health", &g_pVars.visuals.Health);
			ImGui::Checkbox("Weapon", &g_pVars.visuals.Weapon);
			ImGui::Checkbox("Distance", &g_pVars.visuals.Distance);
			ImGui::Checkbox("Money", &g_pVars.visuals.Money);
		}
		ImGui::EndChild();

		ImGui::SameLine();

		static const char* chams[] = { "Off", "Textured", "Flat", "Skeet" };

		ImGui::BeginChild("Misc", ImVec2(700 / 3, 400), true);
		{
			ImGui::Spacing();
			ImGui::Spacing();

			ImGui::Checkbox("Glow", &g_pVars.visuals.Glow);
			ImGui::Checkbox("Backtrack", &g_pVars.visuals.Backtrack);

			ImGui::Spacing();
			ImGui::Spacing();

			ImGui::Text("Chams");
			ImGui::Combo("Chams Style", &g_pVars.visuals.ChamsStyle, chams, IM_ARRAYSIZE(chams));
			ImGui::Checkbox("Vis Only", &g_pVars.visuals.ChamsVisOnly);
		}
		ImGui::EndChild();
	}
	
	void Misc()
	{
		ImGui::BeginChild("Fake Latency", ImVec2(700 / 3, 400), true);
		{
			ImGui::Spacing();
			ImGui::Checkbox("Fake Latency", &g_pVars.misc.FakeLatencyEnable);
			ImGui::Spacing();
			ImGui::Text("Fake Latency Ammount");
			ImGui::SliderFloat("", &g_pVars.misc.FakeLatencyAmmount, 0, 1);

			ImGui::Spacing();
		} ImGui::EndChild();
		
		ImGui::SameLine();

		ImGui::BeginChild("Movement", ImVec2(700 / 3, 400), true);
		{
			ImGui::Checkbox("Bhop", &g_pVars.misc.Bhop);
			ImGui::Spacing();
			ImGui::Checkbox("Meme Walk", &g_pVars.misc.MemeWalk);
		} ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginChild("Other", ImVec2(700 / 3, 400), true);
		{
			static const char* clantags[] = { "Off", "Serenity", "Custom" };

			ImGui::Combo("Clan Tag", &g_pVars.misc.ClanTag, clantags, IM_ARRAYSIZE(clantags));

			if (g_pVars.misc.ClanTag == 2)
			{
				ImGui::InputText("Text", &g_pVars.misc.ClanTagText, 16);
			}

			ImGui::Spacing();
			ImGui::Checkbox("Reveal Ranks", &g_pVars.misc.RevealRanks);
			ImGui::Spacing();
		} ImGui::EndChild();
	}

	void Draw() 
	{
		ImGuiStyle& style = ImGui::GetStyle();

		style.Alpha = 1.f;

		ImGui::SetNextWindowSize(ImVec2(700, 500));
		ImGui::SetNextWindowPosCenter(ImGuiSetCond_Always);

		color();

		ImGuiIO& io = ImGui::GetIO();

		

		io.MouseDrawCursor = g_pVars.menu.OpenedMenu;

		ImGui::Begin("Project Serenity", &g_pVars.menu.OpenedMenu, ImVec2(700, 500), 0.4f, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_ShowBorders | ImGuiWindowFlags_NoScrollbar);
		{
			style.ItemSpacing = ImVec2(0, 0);
			style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);				
			ImGui::Button("S E R E N I T Y", ImVec2(700, 30));

			int TabCount = 4;

			if (ImGui::Button("Rage", ImVec2(700 / TabCount, 70))) iTab = 0;
			ImGui::SameLine();
			style.Colors[ImGuiCol_Text] = iTab == 1 ? ImVec4(0.961f, 0.961f, 0.961f, 1.00f) : ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
			if (ImGui::Button("Legit", ImVec2(700 / TabCount, 70))) iTab = 1;
			ImGui::SameLine();
			style.Colors[ImGuiCol_Text] = iTab == 2 ? ImVec4(0.961f, 0.961f, 0.961f, 1.00f) : ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
			if (ImGui::Button("Visuals", ImVec2(700 / TabCount, 70))) iTab = 2;
			style.Colors[ImGuiCol_Text] = iTab == 3 ? ImVec4(0.961f, 0.961f, 0.961f, 1.00f) : ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
			ImGui::SameLine();
			if (ImGui::Button("Misc", ImVec2(700 / TabCount, 70))) iTab = 3;

			style.ItemSpacing = ImVec2(0, 5);

			switch (iTab)
			{
			case 0: //aimbot
				Rage();
				break;
			case 1: //rage
				Legit();
				break;
			case 2: //rage
				Visuals();
				break;
			case 3: //rage
				Misc();
				break;
			}
		}

		ImGui::End();

		/*if (ImGui::Begin("Project Serenity", &g_Vars.menu.OpenedMenu, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_ShowBorders))
		{
			ImGui::Checkbox("HvH", &g_Vars.menu.OpenedRage);
			ImGui::Checkbox("Legit", &g_Vars.menu.OpenedLegit);
			ImGui::Checkbox("Visuals", &g_Vars.menu.OpenedVisuals);
			ImGui::Checkbox("Misc", &g_Vars.menu.OpenedMisc);
		}
		ImGui::End();

		if (g_Vars.menu.OpenedRage)
		{
			if (ImGui::Begin("Rage", &g_Vars.menu.OpenedRage, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_ShowBorders))
			{
				ImGui::Checkbox("Enabled", &g_Vars.rage.Enabled);

				ImGui::Spacing();
				ImGui::Spacing();

				ImGui::Checkbox("Recoil", &g_Vars.rage.RCS);
				ImGui::Checkbox("Multipoint", &g_Vars.rage.Multipoint);
				ImGui::Checkbox("Anti-Lagcomp", &g_Vars.rage.AntiLagcomp);
				ImGui::SliderFloat("Pointscale", &g_Vars.rage.Pointscale, 0, 1);

				ImGui::Spacing();
				ImGui::Spacing();

				ImGui::SliderInt("Min Damage", &g_Vars.rage.MinimumDamage, 0, 100);
				ImGui::SliderInt("Hitchance", &g_Vars.rage.Hitchance, 0, 100);

				ImGui::End();
			}

			if (ImGui::Begin("Hitboxes", &g_Vars.menu.OpenedRage, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_ShowBorders))
			{
				ImGui::Checkbox("Head", &g_Vars.rage.hitboxes.Head);
				ImGui::Checkbox("Chest", &g_Vars.rage.hitboxes.Chest);
				ImGui::Checkbox("Stomach", &g_Vars.rage.hitboxes.Stomach);
				ImGui::Checkbox("Pelvis", &g_Vars.rage.hitboxes.Pelvis);
				ImGui::Checkbox("Arms", &g_Vars.rage.hitboxes.Arms);
				ImGui::Checkbox("Feet", &g_Vars.rage.hitboxes.Feet);
			}

			if (ImGui::Begin("AntiAims", &g_Vars.menu.OpenedRage, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_ShowBorders))
			{
				static const char* pitch[] = { "Down", "Fake Down", "Up" };
				static const char* real[] = { "Arrow Key", "180 Treehouse" };

				ImGui::Checkbox("Enabled", &g_Vars.rage.antiaim.Enabled);

				ImGui::Spacing();
				ImGui::Spacing();

				ImGui::Combo("Pitch", &g_Vars.rage.antiaim.Pitch, pitch, IM_ARRAYSIZE(pitch));
				ImGui::Combo("Real Yaw", &g_Vars.rage.antiaim.Real, real, IM_ARRAYSIZE(real));
			}
		}

		if (g_Vars.menu.OpenedLegit)
		{
			if (ImGui::Begin("Legit", &g_Vars.menu.OpenedLegit, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_ShowBorders))
			{
				ImGui::Checkbox("Backtracking", &g_Vars.legit.Backtrack);
				ImGui::SliderInt("Ticks", &g_Vars.legit.BacktrackTicks, 1, 12);

				ImGui::Spacing();
				ImGui::Spacing();


			}
			ImGui::End();
		}

		if (g_Vars.menu.OpenedVisuals)
		{
			if (ImGui::Begin("Visuals", &g_Vars.menu.OpenedVisuals, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_ShowBorders))
			{
				static const char* chams[] = { "Off", "Textured", "Flat", "Skeet" };
				static const char* snaplines[] = { "Off", "Center", "Bottom", "Top" };

				ImGui::Checkbox("Enabled", &g_Vars.visuals.Enabled);
				ImGui::Checkbox("Enemy Only", &g_Vars.visuals.EnemyOnly);

				ImGui::Spacing();
				ImGui::Spacing();

				ImGui::Checkbox("ESP Box", &g_Vars.visuals.Box);
				ImGui::Text("Snaplines");
				ImGui::Combo("Snaplines", &g_Vars.visuals.Snaplines, snaplines, IM_ARRAYSIZE(snaplines));
				ImGui::Checkbox("ESP Name", &g_Vars.visuals.Name);
				ImGui::Checkbox("ESP Health", &g_Vars.visuals.Health);
				ImGui::Checkbox("ESP Weapon", &g_Vars.visuals.Weapon);
				ImGui::Checkbox("ESP Distance", &g_Vars.visuals.Distance);
				ImGui::Checkbox("ESP Money", &g_Vars.visuals.Money);

				ImGui::Spacing();
				ImGui::Spacing();

				ImGui::Checkbox("Glow", &g_Vars.visuals.Glow);
				ImGui::Checkbox("Backtrack", &g_Vars.visuals.Backtrack);

				ImGui::Spacing();
				ImGui::Spacing();

				ImGui::Text("Chams");
				ImGui::Combo("Chams Style", &g_Vars.visuals.ChamsStyle, chams, IM_ARRAYSIZE(chams));
				ImGui::Checkbox("Vis Only", &g_Vars.visuals.ChamsVisOnly);
			}
			ImGui::End();
		}


		if (g_Vars.menu.OpenedMisc)
		{
			if (ImGui::Begin("Misc", &g_Vars.menu.OpenedMisc, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_ShowBorders))
			{
				ImGui::Checkbox("Bhop", &g_Vars.misc.Bhop);
			}
			ImGui::End();
		}*/

	}
}
