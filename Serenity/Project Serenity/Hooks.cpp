#include "Hooks.h"
#include "FakeLatency.h"
#include "VMT.h"
#include "LegitBacktrack.h"

//reso hooks
CreateMove oCreateMove;
DoPostScreenEffects oDoPostScreenEffects;
PaintTraverse oPaintTraverse;
OverrideView oOverrideView;
SceneEnd oSceneEnd;
FrameStageNotify oFrameStageNotify;
WriteUsercmdDeltaToBuffer oWriteUsercmdDeltaToBuffer;
PlaySoundT oPlaySound;
EndScene oEndScene;
Reset oReset;
DrawModelExecute oDrawModelExecute;
SendDatagramFn oSendDatagram;
extern LRESULT ImGui_ImplDX9_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);



void __fastcall  Hooks::PaintTraverseHook(void *thisptr, void * _EDX,vgui::VPANEL panel, bool forceRepaint, bool allowForce) 
{
	oPaintTraverse = VGuiHook.get_original<PaintTraverse>(41);
	static uint32_t FocusOverlayPanel;
	if (!FocusOverlayPanel)
	{
		const char* szName = g_pVGuiPanel->GetName(panel);

		if (lstrcmpA(szName, "MatSystemTopPanel") == 0)
		{
			FocusOverlayPanel = panel;
			g_pSDK->InitFonts();
			g_pEngine->ExecuteClientCmd("clear");
		}
	}

	if (FocusOverlayPanel == panel)
	{
		auto local = g_pEntityList->GetClientEntity(g_pEngine->GetLocalPlayer());
		time_t _tm = time(NULL);
		struct tm * curtime = localtime(&_tm);
		std::string time = asctime(curtime);

		Draw.text(5, 3, "Project Serenity", F_Arial, Color(255, 255, 255, 255));
		Draw.text(5, 18, time.c_str(), F_Arial, Color(255, 0, 0, 255));

		if (g_pVars.visuals.Enabled && local)
		{
			f_Esp->Start();
		}
	}

	return oPaintTraverse(thisptr, panel, forceRepaint, allowForce);
}


bool __stdcall Hooks::CreateMoveHook(float flInputSampleTime, CUserCmd* cmd) 
{
	oCreateMove = ClientModeHook.get_original<CreateMove>(24);


	if (!cmd->command_number || !g_pEngine->IsConnected() && !g_pEngine->IsInGame()) 
		return oCreateMove(flInputSampleTime, cmd);
	
	Globals::pLocal = g_pEntityList->GetClientEntity(g_pEngine->GetLocalPlayer());

	Globals::pCmd = cmd;

	if (g_pVars.rage.Enabled)
	{
		/*f_Rage->Run(cmd);

		if (g_pVars.rage.RCS)
			f_Rage->RCS(cmd);*/
	}

	if(g_pVars.legit.Backtrack)
		legitBacktrack.Run(cmd, Globals::pLocal, g_pVars.legit.BacktrackTicks);

	if (g_pVars.misc.MemeWalk)
		f_MoveHacks->MemeWalk();

	if (g_pVars.misc.RevealRanks && pressedKey[VK_TAB])
		f_Misc->ShowRanks();

	if (g_pVars.misc.ClanTag == 1)
		f_Misc->SetClantag("S E R E N I T Y");

	else if (g_pVars.misc.ClanTag == 2 && g_pVars.misc.ClanTagText)
	{
		f_Misc->SetClantag((const char*)g_pVars.misc.ClanTagText);
	}

	else
		f_Misc->SetClantag("");

	return false;
}

int __fastcall Hooks::SendDatagram(INetChannel* netchan, void*, bf_write* datagram)
{
	if (datagram)
	{
		return oSendDatagram(netchan, datagram);
	}

	int instate = netchan->m_nInReliableState;
	int insequencenr = netchan->m_nInSequenceNr;

	f_FakeLatency->AddLatencyToNetchan(netchan, g_pVars.misc.FakeLatencyAmmount);

	int ret = oSendDatagram(netchan, datagram);

	netchan->m_nInReliableState = instate;
	netchan->m_nInSequenceNr = insequencenr;
	return ret;
}

int  __fastcall Hooks::DoPostScreenEffectsHook(void *thisptr, void * _EDX,int a1) 
{
	oDoPostScreenEffects = ClientModeHook.get_original<DoPostScreenEffects>(41);

	if(g_pEngine->GetLocalPlayer() && g_pVars.visuals.Glow) 
		f_Glow->Start();

	return oDoPostScreenEffects(thisptr,a1);
}

void __stdcall Hooks::OverrideViewHook(CViewSetup* pSetup) {
	
	oOverrideView = ClientModeHook.get_original<OverrideView>(18);

	 oOverrideView(pSetup);
}

void __stdcall Hooks::FrameStageNotifyHook(ClientFrameStage_t stage)
{
	oFrameStageNotify = ClientHook.get_original<FrameStageNotify>(36);

	

	return oFrameStageNotify(stage);
}

void __fastcall Hooks::SceneEndHook(void* thisptr)
{
	
	oSceneEnd = RenderViewHook.get_original<SceneEnd>(9);

	//oSceneEnd(thisptr);

	C_BaseEntity* pLocal = g_pEntityList->GetClientEntity(g_pEngine->GetLocalPlayer());

	if(g_pVars.visuals.ChamsStyle > 0)
		f_Chams->Start();
}

bool __fastcall Hooks::WriteUsercmdDeltaToBufferHook(IBaseClientDLL* this0, void * _EDX, int nSlot, void* buf, int from, int to, bool isNewCmd) 
{
	return true;
 }

void __stdcall Hooks::PlaySoundHook(const char *folderIme) 
{
	oPlaySound = SurfaceHook.get_original<PlaySoundT>(82);
	return oPlaySound(g_pVGuiSurface, folderIme);
 }

HRESULT __stdcall Hooks::EndSceneHook(IDirect3DDevice9 *pDevice)
{
	oEndScene = D3D.get_original<EndScene>(42);

	pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xFFFFFFFF);

	if (!ImMenu::D3Init)
		ImMenu::GuiInit(m_hwndWindow, pDevice);

	ImGui::GetIO().MouseDrawCursor = g_pVars.menu.OpenedMenu;

	ImGui_ImplDX9_NewFrame();

	if (g_pVars.menu.OpenedMenu)
		ImMenu::Draw();

	ImGui::Render();


	return oEndScene(pDevice);
}

HRESULT __stdcall Hooks::ResetHook(IDirect3DDevice9 *pDevice, D3DPRESENT_PARAMETERS *pPresentationParameters)
{
	oReset = D3D.get_original<Reset>(16);
	if (!ImMenu::D3Init)
		return oReset(pDevice, pPresentationParameters);

	ImGui_ImplDX9_InvalidateDeviceObjects();

	auto hr = oReset(pDevice, pPresentationParameters);

	if (hr >= 0)
		ImGui_ImplDX9_CreateDeviceObjects();

	return hr;
}

LRESULT __stdcall Hooks::WndProcHook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_LBUTTONDOWN:

		pressedKey[VK_LBUTTON] = true;
		break;

	case WM_LBUTTONUP:

		pressedKey[VK_LBUTTON] = false;
		break;

	case WM_RBUTTONDOWN:

		pressedKey[VK_RBUTTON] = true;
		break;

	case WM_RBUTTONUP:

		pressedKey[VK_RBUTTON] = false;
		break;

	case WM_KEYDOWN:

		pressedKey[wParam] = true;
		break;

	case WM_KEYUP:

		pressedKey[wParam] = false;
		break;

	default: break;
	}
	ImMenu::Open();

	if (ImMenu::D3Init && g_pVars.menu.OpenedMenu && ImGui_ImplDX9_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	return CallWindowProc(oldWindowProc, hWnd, uMsg, wParam, lParam);
}

void __stdcall Hooks::DrawModelExecuteHook(IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld) 
{
	oDrawModelExecute = RenderViewHook.get_original<DrawModelExecute>(21);
	
	return;
}

