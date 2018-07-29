#include "SDK.h"

HWND m_hwndWindow = NULL;
WNDPROC oldWindowProc = nullptr;
//reso externals
Utils* g_pUtils;
Glow* f_Glow;
GlowObjectManager* g_pGlowObjectManager;
SDK* g_pSDK;
ESP* f_Esp;
Misc* f_Misc;
GameUI* g_pUi;
Movement* f_MoveHacks;
Chams* f_Chams;
DrawManager Draw;
vgui::HFont F_Arial;
vgui::HFont F_ESP;
Offsets NetVars;
COffsets g_Offsets;
bool pressedKey[256] = {};
bool menuOpen = true;
bool ImMenu::D3Init;
Variables g_pVars;

VMT_Hook VGuiHook;
VMT_Hook ClientModeHook;
VMT_Hook ClientHook;
VMT_Hook SurfaceHook;
VMT_Hook RenderViewHook;
VMT_Hook D3D;
VMT_Hook ModelRenderHook;

IBaseClientDLL* g_pCHLClient = nullptr;
IClientMode* g_pClientMode = nullptr;
IPanel* g_pVGuiPanel = nullptr;
IPhysicsSurfaceProps* g_pSurfaceProps = nullptr;
ISurface* g_pVGuiSurface = nullptr;
IVEngineClient* g_pEngine = nullptr;
ICvar* g_pICvar = nullptr;
IClientEntityList* g_pEntityList = nullptr;
CGlobalVarsBase* g_pGlobalVars = nullptr;
IEngineTrace* g_pEngineTrace = nullptr;
IVDebugOverlay* g_pDebugOverlay = nullptr;
CInput* g_pCInput = nullptr;
IVModelInfoClient* g_pMdlInfo = nullptr;
IVModelRender* g_pMdlRender = nullptr;
IVRenderView* g_pRenderView = nullptr;
IMaterialSystem* g_pMatSystem = nullptr;
IGameEventManager2* g_pGameEvents = nullptr;
IMoveHelper* g_pMoveHelper = nullptr;
IMDLCache* g_pMDLCache = nullptr;
IPrediction* g_pIPrediction = nullptr;
CGameMovement* g_pGameMovement = nullptr;
CClientState* g_pClientState = nullptr;
IInputSystem* g_pInputSystem = nullptr;


template< typename T >
T* GetInterface(CreateInterfaceFn f, const char* szInterfaceVersion)
    {
        auto result = reinterpret_cast<T*>(f(szInterfaceVersion, nullptr));

        if(!result) {
            throw std::runtime_error(std::string("[get_interface] Failed to GetOffset interface: ") + szInterfaceVersion);
        }

        return result;
    }

CreateInterfaceFn get_module_factory(HMODULE module)
{
	return reinterpret_cast<CreateInterfaceFn>(GetProcAddress(module, "CreateInterface"));
}

void InstallQotq(void)
{
	while (!(m_hwndWindow = FindWindowA("Valve001", NULL)))
		std::this_thread::sleep_for(std::chrono::milliseconds(500));

	while (!GetModuleHandleA("serverbrowser.dll"))
		std::this_thread::sleep_for(std::chrono::milliseconds(500));

	g_pUtils->Print("-------------------------------------------------------------------------------");
	g_pUtils->Print("-------------------------------QotqCheetos-------------------------------------");
	g_pUtils->Print("-------------------------------------------------------------------------------");
	g_pUtils->Print("Got serverbrowser.dll(game is read)");
	g_pUtils->Print(" "); //don't ask dude
	g_pUtils->Print("-------------------------------------------------------------------------------");
	//-----------------------------------------------------------------------------
	// Gettig interfaces.
	//-----------------------------------------------------------------------------
	auto engineFactory    = get_module_factory(GetModuleHandleW(L"engine.dll"));
	auto inputFactory    = get_module_factory(GetModuleHandleW(L"inputsystem.dll"));
        auto clientFactory    = get_module_factory(GetModuleHandleW(L"client.dll"));
        auto valveStdFactory  = get_module_factory(GetModuleHandleW(L"vstdlib.dll"));
        auto vguiFactory      = get_module_factory(GetModuleHandleW(L"vguimatsurface.dll"));
        auto vgui2Factory     = get_module_factory(GetModuleHandleW(L"vgui2.dll"));
        auto matSysFactory    = get_module_factory(GetModuleHandleW(L"materialsystem.dll"));
        auto dataCacheFactory = get_module_factory(GetModuleHandleW(L"datacache.dll"));
        auto vphysicsFactory  = get_module_factory(GetModuleHandleW(L"vphysics.dll"));

	g_pUtils->Print("Getting Interfaces:");

	g_pCHLClient = GetInterface<IBaseClientDLL>(clientFactory, "VClient018");
	g_pClientMode = **reinterpret_cast<IClientMode***>((*(uintptr_t**)g_pCHLClient)[10] + 0x5);

	g_pUtils->Print("ClientMode -> 0x%x", g_pClientMode);

	g_pVGuiPanel = GetInterface<IPanel>(vgui2Factory    , "VGUI_Panel009");
	g_pVGuiSurface = GetInterface<ISurface>(vguiFactory     , "VGUI_Surface031");

	g_pEngine = GetInterface<IVEngineClient>(engineFactory, "VEngineClient014");

	g_pICvar = GetInterface<ICvar>(valveStdFactory , "VEngineCvar007");

	g_pSurfaceProps = GetInterface<IPhysicsSurfaceProps>(vphysicsFactory, "VPhysicsSurfaceProps001");

	g_pGlowObjectManager = *(GlowObjectManager**)(g_pUtils->FindPatternIDA("client.dll", "0F 11 05 ? ? ? ? 83 C8 01") + 3);
	g_pUtils->Print("GlowObjectManager -> 0x%x", g_pGlowObjectManager);

	g_pEntityList = GetInterface<IClientEntityList>(clientFactory   , "VClientEntityList003");

	g_pGlobalVars = **(CGlobalVarsBase***)((*(DWORD**)(g_pCHLClient))[0] + 0x1B);
	g_pUtils->Print("GlobalVarsBase -> 0x%x", g_pGlobalVars);

	g_pEngineTrace = GetInterface<IEngineTrace>(engineFactory   , "EngineTraceClient004");

	g_pDebugOverlay = GetInterface<IVDebugOverlay>(engineFactory   , "VDebugOverlay004");;

	g_pCInput = *(CInput**)((*(DWORD**)g_pCHLClient)[15] + 0x1);;
	g_pUtils->Print("Input -> 0x%x", g_pCInput);

	g_pMdlRender = GetInterface<IVModelRender>(engineFactory   , "VEngineModel016");

	g_pMdlInfo = GetInterface<IVModelInfoClient>(engineFactory   , "VModelInfoClient004");

	g_pRenderView = GetInterface<IVRenderView>(engineFactory   , "VEngineRenderView014");

	g_pMatSystem = GetInterface<IMaterialSystem>(matSysFactory   , "VMaterialSystem080");

	g_pGameEvents = GetInterface<IGameEventManager2>(engineFactory   , "GAMEEVENTSMANAGER002");

	g_pMoveHelper = **(IMoveHelper***)(g_pUtils->FindPatternIDA("client.dll", "8B 0D ? ? ? ? 8B 45 ? 51 8B D4 89 02 8B 01") + 2);
	g_pUtils->Print("IMoveHelper -> 0x%x", g_pMoveHelper);

	g_pMDLCache = GetInterface<IMDLCache>(dataCacheFactory, "MDLCache004");

	g_pIPrediction = GetInterface<IPrediction>(clientFactory   , "VClientPrediction001");

	g_pGameMovement = GetInterface<CGameMovement>(clientFactory   , "GameMovement001");

	g_pClientState = **(CClientState***)(g_pUtils->FindPatternIDA("engine.dll", "A1 ? ? ? ? 8B 80 ? ? ? ? C3") + 1);
	g_pUtils->Print("CClientState -> 0x%x", g_pClientState);

	g_pInputSystem = GetInterface<IInputSystem>(inputFactory, "InputSystemVersion001");

	auto D3DDevice9 = **(IDirect3DDevice9***)(g_pUtils->FindPatternIDA("shaderapidx9.dll", "A1 ? ? ? ? 50 8B 08 FF 51 0C") + 1);
	g_pUtils->Print("IDirect3DDevice9 -> 0x%x", D3DDevice9);

	g_pUtils->Print("-------------------------------------------------------------------------------");



	g_pUtils->Print("-------------------------------------------------------------------------------");
	g_pUtils->Print("Dumping Netvars:");
	g_pUtils->Print("Initialising Cham Mats:");

	std::ofstream("csgo\\materials\\reflective_ignorez.vmt") << R"#("VertexLitGeneric"
	{

	  "$basetexture" "vgui/white_additive"
	  "$ignorez"      "1"
	  "$envmap"       "env_cubemap"
	  "$normalmapalphaenvmapmask"  "1"
	  "$envmapcontrast"             "1"
	  "$nofog"        "1"
	  "$model"        "1"
	  "$nocull"       "0"
	  "$znearer"      "0"
	  "$flat"         "1"
	}
	)#";

	std::ofstream("csgo\\materials\\reflective_normal.vmt") << R"#("VertexLitGeneric"
	{

	  "$basetexture" "vgui/white_additive"
	  "$ignorez"      "0"
	  "$envmap"       "env_cubemap"
	  "$normalmapalphaenvmapmask"  "1"
	  "$envmapcontrast"             "1"
	  "$nofog"        "1"
	  "$model"        "1"
	  "$nocull"       "0"
	  "$znearer"      "0"
	  "$flat"         "1"
	}
	)#";

	std::ofstream("csgo\\materials\\textured_ignorez.vmt") << R"#("VertexLitGeneric"
	{
	  "$basetexture" "vgui/white_additive"
	  "$ignorez"      "1"
	  "$envmap"       ""
	  "$nofog"        "1"
	  "$model"        "1"
	  "$nocull"       "0"
	  "$selfillum"    "1"
	  "$halflambert"  "1"
	  "$znearer"      "0"
	  "$flat"         "1"
	}
	)#";

	std::ofstream("csgo\\materials\\textured_normal.vmt") << R"#("VertexLitGeneric"
	{
	  "$basetexture" "vgui/white_additive"
	  "$ignorez"      "0"
	  "$envmap"       ""
	  "$nofog"        "1"
	  "$model"        "1"
	  "$nocull"       "0"
	  "$selfillum"    "1"
	  "$halflambert"  "1"
	  "$znearer"      "0"
	  "$flat"         "1"
	}
	)#";

	std::ofstream("csgo\\materials\\flat_normal.vmt") << R"#("UnlitGeneric"
	{
	  "$basetexture" "vgui/white_additive"
	  "$ignorez"      "0"
	  "$envmap"       ""
	  "$nofog"        "1"
	  "$model"        "1"
	  "$nocull"       "0"
	  "$selfillum"    "1"
	  "$halflambert"  "1"
	  "$znearer"      "0"
	  "$flat"         "1"
	}
	)#";

	std::ofstream("csgo\\materials\\flat_ignorez.vmt") << R"#("UnlitGeneric"
	{
	  "$basetexture" "vgui/white_additive"
	  "$ignorez"      "1"
	  "$envmap"       ""
	  "$nofog"        "1"
	  "$model"        "1"
	  "$nocull"       "0"
	  "$selfillum"    "1"
	  "$halflambert"  "1"
	  "$znearer"      "0"
	  "$flat"         "1"
	}
	)#";

	g_pUtils->Print("-------------------------------------------------------------------------------");



	g_pUtils->Print("-------------------------------------------------------------------------------");
	g_pUtils->Print("Dumping Netvars:");


	//-----------------------------------------------------------------------------
	// Netvars.
	//-----------------------------------------------------------------------------

	//pasted from my old paste which i pasted from somewhere, dont remember.
	g_Offsets.NetVars = std::shared_ptr<CNetVars>(new CNetVars());
	g_Offsets.NetVars->GetOffset(("DT_BaseEntity"), ("m_CollisionGroup"), &NetVars.m_rgflCoordinateFrame, -0x30);
	g_Offsets.NetVars->GetOffset(("DT_BaseEntity"), ("m_Collision"), &NetVars.m_Collision);
	g_Offsets.NetVars->GetOffset(("DT_BaseEntity"), ("m_vecOrigin"), &NetVars.m_vecOrigin);
	g_Offsets.NetVars->GetOffset(("DT_BaseEntity"), ("m_iTeamNum"), &NetVars.m_iTeamNum);
	g_Offsets.NetVars->GetOffset(("DT_BasePlayer"), ("m_aimPunchAngle"), &NetVars.m_vecPunchAngles);
	g_Offsets.NetVars->GetOffset(("DT_BasePlayer"), ("m_viewPunchAngle"), &NetVars.m_vecViewPunchAngles);
	g_Offsets.NetVars->GetOffset(("DT_CSPlayer"), ("m_vecViewOffset[0]"), &NetVars.m_vecViewOffset);
	g_Offsets.NetVars->GetOffset(("DT_CSPlayer"), ("m_ArmorValue"), &NetVars.m_ArmorValue);
	g_Offsets.NetVars->GetOffset(("DT_BasePlayer"), ("m_vecVelocity[0]"), &NetVars.m_vecVelocity);
	g_Offsets.NetVars->GetOffset(("DT_BasePlayer"), ("m_lifeState"), &NetVars.m_lifeState);
	g_Offsets.NetVars->GetOffset(("DT_BasePlayer"), ("m_fFlags"), &NetVars.m_fFlags);
	g_Offsets.NetVars->GetOffset(("DT_BasePlayer"), ("m_iHealth"), &NetVars.m_iHealth);
	g_Offsets.NetVars->GetOffset(("DT_BasePlayer"), ("m_hActiveWeapon"), &NetVars.m_hActiveWeapon);
	g_Offsets.NetVars->GetOffset(("DT_BasePlayer"), ("m_hMyWeapons"), &NetVars.m_hMyWeapons);
	g_Offsets.NetVars->GetOffset(("DT_BasePlayer"), ("m_hViewModel[0]"), &NetVars.m_hViewModel);
	g_Offsets.NetVars->GetOffset(("DT_BasePlayer"), ("m_nTickBase"), &NetVars.m_nTickBase);
	g_Offsets.NetVars->GetOffset(("DT_BasePlayer"), ("m_hObserverTarget"), &NetVars.m_hObserverTarget);
	g_Offsets.NetVars->GetOffset(("DT_BasePlayer"), ("m_flSimulationTime"), &NetVars.m_flSimulationTime);
	g_Offsets.NetVars->GetOffset(("DT_CSPlayer"), ("m_iAccount"), &NetVars.m_iAccountID);
	g_Offsets.NetVars->GetOffset(("DT_BaseAttributableItem"), ("m_iItemDefinitionIndex"), &NetVars.m_iItemDefinitionIndex);
	g_Offsets.NetVars->GetOffset(("DT_BaseAttributableItem"), ("m_iItemIDHigh"), &NetVars.m_iItemIDHigh);
	g_Offsets.NetVars->GetOffset(("DT_BaseAttributableItem"), ("m_iEntityQuality"), &NetVars.m_iEntityQuality);
	g_Offsets.NetVars->GetOffset(("DT_BaseAttributableItem"), ("m_szCustomName"), &NetVars.m_szCustomName);
	g_Offsets.NetVars->GetOffset(("DT_BaseAttributableItem"), ("m_nFallbackPaintKit"), &NetVars.m_nFallbackPaintKit);
	g_Offsets.NetVars->GetOffset(("DT_BaseAttributableItem"), ("m_flFallbackWear"), &NetVars.m_flFallbackWear);
	g_Offsets.NetVars->GetOffset(("DT_BaseAttributableItem"), ("m_nFallbackSeed"), &NetVars.m_nFallbackSeed);
	g_Offsets.NetVars->GetOffset(("DT_BaseAttributableItem"), ("m_nFallbackStatTrak"), &NetVars.m_nFallbackStatTrak);
	g_Offsets.NetVars->GetOffset(("DT_BaseAttributableItem"), ("m_OriginalOwnerXuidLow"), &NetVars.m_OriginalOwnerXuidLow);
	g_Offsets.NetVars->GetOffset(("DT_BaseAttributableItem"), ("m_OriginalOwnerXuidHigh"), &NetVars.m_OriginalOwnerXuidHigh);
	g_Offsets.NetVars->GetOffset(("DT_BaseViewModel"), ("m_nModelIndex"), &NetVars.m_nModelIndex);
	g_Offsets.NetVars->GetOffset(("DT_BaseViewModel"), ("m_hOwner"), &NetVars.m_hOwner);
	g_Offsets.NetVars->GetOffset(("DT_BaseViewModel"), ("m_hWeapon"), &NetVars.m_hWeapon);
	g_Offsets.NetVars->GetOffset(("DT_BaseCombatWeapon"), ("m_flNextPrimaryAttack"), &NetVars.m_flNextPrimaryAttack);
	g_Offsets.NetVars->GetOffset(("DT_BaseCombatWeapon"), ("m_flNextSecondaryAttack"), &NetVars.m_flNextSecondaryAttack);
	g_Offsets.NetVars->GetOffset(("DT_BaseCombatWeapon"), ("m_iClip1"), &NetVars.m_iClip1);
	g_Offsets.NetVars->GetOffset(("DT_BaseCombatWeapon"), ("m_iPrimaryReserveAmmoCount"), &NetVars.m_iClip2);
	g_Offsets.NetVars->GetOffset(("DT_CSPlayer"), ("m_bIsScoped"), &NetVars.m_bIsScoped);
	g_Offsets.NetVars->GetOffset(("DT_CSPlayer"), ("m_iShotsFired"), &NetVars.m_iShotsFired);
	g_Offsets.NetVars->GetOffset(("DT_CSPlayer"), ("m_flFlashMaxAlpha"), &NetVars.m_flFlashMaxAlpha);
	g_Offsets.NetVars->GetOffset(("DT_CSPlayer"), ("#"), &NetVars.m_hMyWearables);
	g_Offsets.NetVars->GetOffset(("DT_PlantedC4"), ("m_flC4Blow"), &NetVars.m_flC4Blow);
	g_Offsets.NetVars->GetOffset(("DT_PlantedC4"), ("m_flTimerLength"), &NetVars.m_flTimerLength);
	g_Offsets.NetVars->GetOffset(("DT_CSPlayer"), ("m_flLowerBodyYawTarget"), &NetVars.m_flLowerBodyYawTarget);
	g_Offsets.NetVars->GetOffset(("DT_CSPlayer"), ("m_angEyeAngles"), &NetVars.m_angEyeAngles);
	g_Offsets.NetVars->GetOffset(("DT_CSPlayer"), ("m_flCycle"), &NetVars.m_flCycle);
	g_Offsets.NetVars->GetOffset(("DT_CSPlayer"), ("m_nSequence"), &NetVars.m_nSequence);
	g_Offsets.NetVars->GetOffset(("DT_CSPlayer"), ("m_flPoseParameter"), &NetVars.m_flPoseParameter);
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	//-----------------------------------------------------------------------------
	// Hooking.
	//-----------------------------------------------------------------------------

	oldWindowProc = (WNDPROC)SetWindowLongPtr(m_hwndWindow, GWLP_WNDPROC, (LONG_PTR)Hooks::WndProcHook);

	/*g_pUtils->Print("Hooking:");
	vfunc_hook* VGuiHook = new vfunc_hook((PDWORD*)g_pVGuiPanel);
	oPaintTraverse = (PaintTraverse)(VGuiHook->HookMethod((DWORD)Hooks::PaintTraverseHook, 41));
	g_pUtils->Print("PaintTraverse(41) -> 0x%x", oPaintTraverse);
	vfunc_hook* ClientModeHook = new vfunc_hook((PDWORD*)g_pClientMode);
	oCreateMove = (CreateMove)(ClientModeHook->HookMethod((DWORD)Hooks::CreateMoveHook, 24));
	g_pUtils->Print("CreateMove(24) -> 0x%x", oCreateMove);
	oDoPostScreenEffects = (DoPostScreenEffects)(ClientModeHook->HookMethod((DWORD)Hooks::DoPostScreenEffectsHook, 44));
	g_pUtils->Print("DoPostScreenEffects(44) -> 0x%x", oDoPostScreenEffects);
	oOverrideView = (OverrideView)(ClientModeHook->HookMethod((DWORD)Hooks::OverrideViewHook, 18));
	g_pUtils->Print("OverrideView(18) -> 0x%x", oOverrideView);
	vfunc_hook* ClientHook = new vfunc_hook((PDWORD*)g_pCHLClient);
	oFrameStageNotify = (FrameStageNotify)(ClientHook->HookMethod((DWORD)Hooks::FrameStageNotifyHook, 36));
	g_pUtils->Print("FrameStageNotify(36) -> 0x%x", oFrameStageNotify);
	vfunc_hook* SurfaceHook = new vfunc_hook((PDWORD*)g_pVGuiSurface);
	oPlaySound = (PlaySoundT)(SurfaceHook->HookMethod((DWORD)Hooks::PlaySoundHook, 82));
	g_pUtils->Print("PlaySound(82) -> 0x%x", oPlaySound);
	vfunc_hook* RenderViewHook = new vfunc_hook((PDWORD*)g_pRenderView);
	oSceneEnd = (SceneEnd)(RenderViewHook->HookMethod((DWORD)Hooks::SceneEndHook, 9));
	g_pUtils->Print("SceneEnd(9) -> 0x%x", oSceneEnd);
	vfunc_hook* D3DDevHook = new vfunc_hook((PDWORD*)D3DDevice9);
	oEndScene = (EndScene)(D3DDevHook->HookMethod((DWORD)Hooks::EndSceneHook, 42));
	g_pUtils->Print("EndScene(42) -> 0x%x", oEndScene);
	oReset = (Reset)(D3DDevHook->HookMethod((DWORD)Hooks::ResetHook, 16));
	g_pUtils->Print("Reset(16) -> 0x%x", oReset);
	vfunc_hook* MdlRenderHook = new vfunc_hook((PDWORD*)g_pMdlRender);
	oDrawModelExecute = (DrawModelExecute)(MdlRenderHook->HookMethod((DWORD)Hooks::DrawModelExecuteHook, 21));
	g_pUtils->Print("DrawModelExecute(21) -> 0x%x", oDrawModelExecute);*/

	g_pUtils->Print("Hooking");

	VGuiHook.setup(g_pVGuiPanel);
	g_pUtils->Print("VGUI");

	ClientModeHook.setup(g_pClientMode, "client.dll");
	g_pUtils->Print("Client mode");

	ClientHook.setup(g_pCHLClient, "client.dll");
	g_pUtils->Print("client");

	SurfaceHook.setup(g_pVGuiSurface);

	RenderViewHook.setup(g_pRenderView);
	g_pUtils->Print("rv");

	D3D.setup(D3DDevice9, "shaderapidx9.dll");
	g_pUtils->Print("dx9");

	ModelRenderHook.setup(g_pMdlRender, "engine.dll");
	g_pUtils->Print("mdlrender");

	const char* const buildTime = "Last Build Date Was On " __DATE__ " @ " __TIME__;
	
	//g_pUtils->ShowMessagebox(buildTime, "Injected!");

	VGuiHook.hook_index(Hooks::PaintTraverseHook, 41);

	//ClientModeHook.hook_index(Hooks::CreateMoveHook, 24);
	//ClientModeHook.hook_index(Hooks::DoPostScreenEffectsHook, 44);
	//ClientModeHook.hook_index(Hooks::OverrideViewHook, 18);

	//ClientHook.hook_index(Hooks::FrameStageNotifyHook, 36);

	//SurfaceHook->hook_index(Hooks::PlaySoundHook, 82);

	//RenderViewHook.hook_index(Hooks::SceneEndHook, 9);

	D3D.hook_index(Hooks::EndSceneHook, 42);
	D3D.hook_index(Hooks::ResetHook, 16);

	//RenderViewHook.hook_index(Hooks::DrawModelExecuteHook, 21);

	
}



void SDK::InitFonts() {
	F_Arial = g_pVGuiSurface->CreateFont_();
	F_ESP = g_pVGuiSurface->CreateFont_();

	g_pVGuiSurface->SetFontGlyphSet(F_Arial, "Arial", 16, 600, 0, 0, FONTFLAG_DROPSHADOW);
	g_pVGuiSurface->SetFontGlyphSet(F_ESP, "Arial", 12, 450, 0, 0, FONTFLAG_OUTLINE);
}

