

// MQ2TargetInfo.cpp : Defines the entry point for the DLL application.
// by EqMule 2018

// PLUGIN_API is only to be used for callbacks.  All existing callbacks at this time
// are shown below. Remove the ones your plugin does not use.  Always use Initialize
// and Shutdown for setup and cleanup, do NOT do it in DllMain.



#include "../MQ2Plugin.h"
#include "resource.h"

PreSetup("MQ2TargetInfo");
HANDLE hLockphmap = 0;
CLabelWnd*InfoLabel = 0;
CLabelWnd*DistanceLabel = 0;
CLabelWnd*CanSeeLabel = 0;
CButtonWnd*PHButton = 0;
CSidlScreenWnd*Target_BuffWindow = 0;
CLabelWnd*Target_AggroPctPlayerLabel = 0;
CLabelWnd*Target_AggroNameSecondaryLabel = 0;
CLabelWnd*Target_AggroPctSecondaryLabel = 0;

typedef struct _phinfo
{
	std::string Expansion;
	std::string Zone;
	std::string Named;
	std::string Link;
}phinfo,*pphinfo;

std::map<std::string, phinfo> phmap;
bool GetPhMap(PSPAWNINFO pSpawn, phinfo *pinf);
class MyCTargetWnd
{
public:
	//we can safely use this as its always called when a user leftclicks the target window.
	//also the upside here is that we dont collide with the trade with target detour in mq2windows...
	void MyCTargetWnd::HandleBuffRemoveRequest_Tramp(CXWnd *);
	void MyCTargetWnd::HandleBuffRemoveRequest_Detour(CXWnd *pWnd)
	{
		if(PHButton && pWnd==PHButton)
		{
			if (pTarget) {
				phinfo pinf;
				if (GetPhMap((PSPAWNINFO)pTarget, &pinf)) {
					std::string url = "https://webproxy.to/browse.php?b=4&u=";
					url.append(pinf.Link);// https://eqresource.com&b=4";
					//std::string url = "https://www.google.com/search?q=";
					//std::string url = "http://everquest.allakhazam.com/search.html?q=";
					//url.append(pinf.Named);
					if (CHtmlWnd *ItemHtmlwnd = pCWebManager->CreateHtmlWnd(url.c_str(), pinf.Named.c_str(), NULL, true, pinf.Named.c_str()))
					{
						//Beep(1000, 100);
					}
				}
			}
		}
		HandleBuffRemoveRequest_Tramp(pWnd);
	}
};
DETOUR_TRAMPOLINE_EMPTY(void MyCTargetWnd::HandleBuffRemoveRequest_Tramp(CXWnd*));
void LoadPHs(char*szMyName) {
	//well we have it, lets fill in the map...
	//Chief Librarian Lars^a shissar arbiter, a shissar defiler^tds^kattacastrumdeluge^https://tds.eqresource.com/chieflibrarianlars.php
	phinfo phinf;
	std::string phs;
	int commapos = 0;
	CHAR szBuffer[2048] = { 0 };
	FILE *fp = 0;
	errno_t err = fopen_s(&fp, szMyName, "rb");
	if (!err) {
		while (fgets(szBuffer, 2048, fp) != 0) {
			if (char *pDest = strchr(szBuffer, '^')) {
				pDest[0] = '\0';
				phinf.Named = szBuffer;
				pDest++;
				if (char *pDest2 = strchr(pDest, '^')) {
					pDest2[0] = '\0';
					phs = pDest;
					*pDest2++;
					if (pDest = strchr(pDest2, '^')) {
						pDest[0] = '\0';
						phinf.Expansion = pDest2;
						pDest++;
						if (pDest2 = strchr(pDest, '^')) {
							pDest2[0] = '\0';
							phinf.Zone = pDest;
							pDest2++;
							if (pDest = strchr(pDest2, '\r')) {
								pDest[0] = '\0';
							}
							phinf.Link = pDest2;
						}
					}
				}
			}
			if (phs.find(",") != phs.npos && phs.find("Yikkarvi,") == phs.npos &&
				phs.find("Furg,") == phs.npos && phs.find("Tykronar,") == phs.npos &&
				phs.find("Ejarld,") == phs.npos && phs.find("Grald,") == phs.npos &&
				phs.find("Graluk,") == phs.npos) {
				while ((commapos = phs.find_last_of(",")) != phs.npos) {
					//more than one...
					std::string temp = phs.substr(commapos + 2, -1);
					phs.erase(commapos, -1);
					phmap[temp] = phinf;
				}
				phmap[phs] = phinf;
			}
			else {
				phmap[phs] = phinf;
			}
		}
		fclose(fp);
	}
}
/*#define Target_BuffWindow_TopOffset 62;
#define dTopOffset 46;
#define dBottomOffset 60;
#define InfoTopOffset 33;
#define dLeftOffset 50;
#define InfoBottomOffset 47;
*/
#define Target_BuffWindow_TopOffset 62+14;
#define dTopOffset 46+14;
#define dBottomOffset 60+14;
#define InfoTopOffset 33+14;
#define dLeftOffset 50;
#define InfoBottomOffset 47+14;

void Initialize()
{
	if (!DistanceLabel) {
		if (PCTARGETWND pTwnd = (PCTARGETWND)pTargetWnd) {

			Target_AggroPctPlayerLabel = (CLabelWnd*)((CXWnd*)pTwnd)->GetChildItem("Target_AggroPctPlayerLabel");
			Target_AggroNameSecondaryLabel = (CLabelWnd*)((CXWnd*)pTwnd)->GetChildItem("Target_AggroNameSecondaryLabel");
			Target_AggroPctSecondaryLabel = (CLabelWnd*)((CXWnd*)pTwnd)->GetChildItem("Target_AggroPctSecondaryLabel");
			Target_BuffWindow = (CSidlScreenWnd*)((CXWnd*)pTwnd)->GetChildItem("Target_BuffWindow");

			CControlTemplate *DistLabelTemplate = (CControlTemplate*)pSidlMgr->FindScreenPieceTemplate("Target_AggroPctSecondaryLabel");
			CControlTemplate *CanSeeLabelTemplate = (CControlTemplate*)pSidlMgr->FindScreenPieceTemplate("Target_AggroNameSecondaryLabel");
			CControlTemplate *PHButtonTemplate = (CControlTemplate*)pSidlMgr->FindScreenPieceTemplate("IDW_ModButton");//borrowing this...
			if (PHButtonTemplate && Target_BuffWindow && CanSeeLabelTemplate && Target_AggroNameSecondaryLabel && Target_AggroPctSecondaryLabel && Target_AggroPctPlayerLabel && DistLabelTemplate) {
				Target_BuffWindow->BGColor = 0xFF000000;
				Target_BuffWindow->TopOffset = Target_BuffWindow_TopOffset;
				Target_AggroPctPlayerLabel->BGColor = 0xFF00000;
				Target_AggroNameSecondaryLabel->BGColor = 0xFF00000;
				Target_AggroPctSecondaryLabel->BGColor = 0xFF00000;
				Target_AggroPctPlayerLabel->TopOffset = dTopOffset;
				Target_AggroPctPlayerLabel->BottomOffset = dBottomOffset;
				Target_AggroNameSecondaryLabel->TopOffset = dTopOffset;
				Target_AggroNameSecondaryLabel->BottomOffset = dBottomOffset;
				Target_AggroPctSecondaryLabel->TopOffset = dTopOffset;
				Target_AggroPctSecondaryLabel->BottomOffset = dBottomOffset;
				//CHAR szTemp[16];
				//sprintf_s(szTemp, "\xE2\x8C\x96");// , 0xE2, 0x8C, 0x96);

				SetCXStr(&DistLabelTemplate->Controller, "0");
				SetCXStr(&CanSeeLabelTemplate->Controller, "0");
				//create the info label
				SetCXStr(&DistLabelTemplate->Name, "Target_InfoLabel");
				SetCXStr(&DistLabelTemplate->ScreenID, "Target_InfoLabel");
				if (InfoLabel = (CLabelWnd *)pSidlMgr->CreateXWndFromTemplate((CXWnd*)pTwnd, DistLabelTemplate)) {
					InfoLabel->dShow = true;
					InfoLabel->bAlignCenter = false;
					InfoLabel->bAlignRight = false;
					InfoLabel->bLeftAnchoredToLeft = true;
					InfoLabel->TopOffset = 33;
					InfoLabel->BottomOffset = 47;
					InfoLabel->LeftOffset = 2;
					InfoLabel->RightOffset = 60;
					InfoLabel->CRNormal = 0xFF00FF00;//green
					InfoLabel->BGColor = 0xFFFFFFFF;
					SetCXStr(&InfoLabel->Tooltip, "Target Info");
				}
				//create the distance label
				SetCXStr(&DistLabelTemplate->Name, "Target_DistLabel");
				SetCXStr(&DistLabelTemplate->ScreenID, "Target_DistLabel");
				if (DistanceLabel = (CLabelWnd *)pSidlMgr->CreateXWndFromTemplate((CXWnd*)pTwnd, DistLabelTemplate)) {
					DistanceLabel->dShow = true;
					DistanceLabel->TopOffset = InfoTopOffset;
					DistanceLabel->BottomOffset = InfoBottomOffset;
					DistanceLabel->LeftOffset = dLeftOffset;
					DistanceLabel->RightOffset = 2;
					DistanceLabel->CRNormal = 0xFF00FF00;//green
					DistanceLabel->BGColor = 0xFFFFFFFF;
					SetCXStr(&DistanceLabel->Tooltip, "Target Distance");
				}
				//create can see label
				SetCXStr(&CanSeeLabelTemplate->Name, "Target_CanSeeLabel");
				SetCXStr(&CanSeeLabelTemplate->ScreenID, "Target_CanSeeLabel");
				if (CanSeeLabel = (CLabelWnd *)pSidlMgr->CreateXWndFromTemplate((CXWnd*)pTwnd, CanSeeLabelTemplate)) {
					CanSeeLabel->dShow = true;
					CanSeeLabel->TopOffset = InfoTopOffset;
					CanSeeLabel->BottomOffset = InfoBottomOffset;
					CanSeeLabel->LeftOffset = dLeftOffset;
					CanSeeLabel->RightOffset = dLeftOffset;
					CanSeeLabel->CRNormal = 0xFF00FF00;//green
					CanSeeLabel->BGColor = 0xFFFFFFFF;
					SetCXStr(&CanSeeLabel->Tooltip, "Can See Target");
				}
				//create PHButton
				PHButtonTemplate->Font = 0;
				if (PHButton = (CButtonWnd *)pSidlMgr->CreateXWndFromTemplate((CXWnd*)pTwnd, PHButtonTemplate)) {
					PHButton->dShow = true;
					PHButton->bBottomAnchoredToTop = Target_AggroPctPlayerLabel->bBottomAnchoredToTop;
					PHButton->bLeftAnchoredToLeft = Target_AggroPctPlayerLabel->bLeftAnchoredToLeft;
					PHButton->bRightAnchoredToLeft = Target_AggroPctPlayerLabel->bRightAnchoredToLeft;
					PHButton->bTopAnchoredToTop = Target_AggroPctPlayerLabel->bTopAnchoredToTop;
					PHButton->TopOffset = InfoTopOffset+1;
					PHButton->BottomOffset = dTopOffset-1;
					PHButton->LeftOffset = Target_AggroPctPlayerLabel->LeftOffset;
					PHButton->RightOffset = Target_AggroPctPlayerLabel->RightOffset;
					PHButton->Location.top = InfoTopOffset+1;
					PHButton->Location.bottom = PHButton->BottomOffset;
					PHButton->Location.left = 2;
					PHButton->Location.right = 20;
					PHButton->CRNormal = 0xFF00FFFF;//cyan
					PHButton->BGColor = 0xFFFFFFFF;
					SetCXStr(&PHButton->Tooltip, "Target is a Place Holder");
					SetCXStr(&PHButton->WindowText, "PH");
				}
				//
				//now set the template values back
				PHButtonTemplate->Font = 2;
				SetCXStr(&DistLabelTemplate->Name, "Target_AggroPctSecondaryLabel");
				SetCXStr(&DistLabelTemplate->ScreenID, "Target_AggroPctSecondaryLabel");
				SetCXStr(&DistLabelTemplate->Controller, "308");
				SetCXStr(&CanSeeLabelTemplate->Name, "Target_AggroNameSecondaryLabel");
				SetCXStr(&CanSeeLabelTemplate->ScreenID, "Target_AggroNameSecondaryLabel");
				SetCXStr(&CanSeeLabelTemplate->Controller, "304");
			}
		}
	}
}
// Called once, when the plugin is to initialize
PLUGIN_API VOID InitializePlugin(VOID)
{
	if (!hLockphmap)
		hLockphmap = CreateMutex(NULL, FALSE, NULL);

	HMODULE hMe = 0;
	CHAR szMyName[2048] = { 0 };
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCTSTR)InitializePlugin, &hMe);
	void* pMyBinaryData = 0;
	GetModuleFileName(hMe, szMyName, 2048);
	if (char *pDest = strrchr(szMyName, '.')) {
		pDest[0] = '\0';
		strcat_s(szMyName, ".txt");
	}
	WIN32_FIND_DATA FindFile = { 0 };
	HANDLE hSearch = FindFirstFile(szMyName, &FindFile);
	if (hSearch == INVALID_HANDLE_VALUE) {
		//need to unpack our resource.
		
		if (HRSRC hRes = FindResource(hMe, MAKEINTRESOURCE(IDR_DB1), "DB")) {
			if (HGLOBAL bin = LoadResource(hMe, hRes)) {
				BOOL bResult = 0;
				if (pMyBinaryData = LockResource(bin)) {
					//save it...
					DWORD ressize = SizeofResource(hMe, hRes);
					FILE *File = 0;
					errno_t err = fopen_s(&File, szMyName, "wb");
					if (!err) {
						fwrite(pMyBinaryData, ressize, 1, File);
						fclose(File);
					}
					bResult = UnlockResource(hRes);
				}
				bResult = FreeResource(hRes);
			}
		}
		LoadPHs(szMyName);
	}
	else {
		FindClose(hSearch);
		LoadPHs(szMyName);
	}
	EzDetourwName(CTargetWnd__HandleBuffRemoveRequest, &MyCTargetWnd::HandleBuffRemoveRequest_Detour, &MyCTargetWnd::HandleBuffRemoveRequest_Tramp, "CTargetWnd__HandleBuffRemoveRequest");
	DebugSpewAlways("Initializing MQ2TargetInfo");
	Initialize();
}
void CleanUp(bool bUnload)
{
	if (InfoLabel) {
		((CButtonWnd*)InfoLabel)->Destroy();
		InfoLabel = 0;
	}
	if (DistanceLabel) {
		((CButtonWnd*)DistanceLabel)->Destroy();
		DistanceLabel = 0;
	}
	if (CanSeeLabel) {
		((CButtonWnd*)CanSeeLabel)->Destroy();
		CanSeeLabel = 0;
	}
	if (PHButton) {
		((CButtonWnd*)PHButton)->Destroy();
		PHButton = 0;
	}
	if (GetGameState() == GAMESTATE_INGAME) {
		if (bUnload) {
			if (!IsBadReadPtr(Target_BuffWindow, 4)) {
				Target_BuffWindow->TopOffset = 50;
			}

			if (!IsBadReadPtr(Target_AggroPctPlayerLabel, 4)) {
				Target_AggroPctPlayerLabel->TopOffset = 33;
				Target_AggroPctPlayerLabel->BottomOffset = 47;
			}
			if (!IsBadReadPtr(Target_AggroNameSecondaryLabel, 4)) {
				Target_AggroNameSecondaryLabel->TopOffset = 33;
				Target_AggroNameSecondaryLabel->BottomOffset = 47;
			}
			if (!IsBadReadPtr(Target_AggroPctSecondaryLabel, 4)) {
				Target_AggroPctSecondaryLabel->TopOffset = 33;
				Target_AggroPctSecondaryLabel->BottomOffset = 47;
			}
		}
	}
}
// Called once, when the plugin is to shutdown
PLUGIN_API VOID ShutdownPlugin(VOID)
{
	DebugSpewAlways("Shutting down MQ2TargetInfo");
	if(CTargetWnd__HandleBuffRemoveRequest)
		RemoveDetour(CTargetWnd__HandleBuffRemoveRequest);
	CleanUp(true);
	if (hLockphmap) {
		ReleaseMutex(hLockphmap);
		CloseHandle(hLockphmap);
		hLockphmap = 0;
	}
}

// Called after entering a new zone
PLUGIN_API VOID OnZoned(VOID)
{
	DebugSpewAlways("MQ2TargetInfo::OnZoned()");
}

// Called once directly before shutdown of the new ui system, and also
// every time the game calls CDisplay::CleanGameUI()
PLUGIN_API VOID OnCleanUI(VOID)
{
	DebugSpewAlways("MQ2TargetInfo::OnCleanUI()");
	// destroy custom windows, etc
	CleanUp(false);
}

// Called once directly after the game ui is reloaded, after issuing /loadskin
PLUGIN_API VOID OnReloadUI(VOID)
{
	DebugSpewAlways("MQ2TargetInfo::OnReloadUI()");
	Initialize();
	// recreate custom windows, etc
}

bool IsPlaceHolder(PSPAWNINFO pSpawn)
{
	lockit lk(hLockphmap,"IsPlaceHolder");
	if (pSpawn && phmap.find(pSpawn->DisplayedName) != phmap.end()) {
		return true;
	}
	return false;
}
bool GetPhMap(PSPAWNINFO pSpawn, phinfo *pinf)
{
	lockit lk(hLockphmap,"IsPlaceHolder");
	if (pSpawn && phmap.find(pSpawn->DisplayedName) != phmap.end()) {
		*pinf = phmap[pSpawn->DisplayedName];
		return true;
	}
	return false;
}
// This is called every time MQ pulses
CHAR szTargetDist[64] = { 0 };
int looper = 0;
PSPAWNINFO oldspawn = 0;
PLUGIN_API VOID OnPulse(VOID)
{
	// DONT leave in this debugspew, even if you leave in all the others
	//DebugSpewAlways("MQ2TargetInfo::OnPulse()");
	looper++;
	if (looper > 20) {
		looper = 0;
		if (GetGameState() == GAMESTATE_INGAME) {
			Initialize();
			if (PCTARGETWND pTwnd = (PCTARGETWND)pTargetWnd) {
				if (InfoLabel && DistanceLabel && CanSeeLabel && PHButton) {
					if (pTarget && pCharSpawn) {
						if (oldspawn != (PSPAWNINFO)pTarget) {
							oldspawn = (PSPAWNINFO)pTarget;
							phinfo pinf;
							if (GetPhMap((PSPAWNINFO)pTarget, &pinf)) {
								SetCXStr(&PHButton->Tooltip, (char*)pinf.Named.c_str());
								PHButton->dShow = true;
							}
							else {
								PHButton->dShow = false;
							}
						}
						//set info
						PSPAWNINFO pInfo = (PSPAWNINFO)pTarget;
						switch (pInfo->Anon)
						{
						case 1:
							sprintf_s(szTargetDist, "Anonymous");
							break;
						case 2:
							sprintf_s(szTargetDist, "Roleplaying");
							break;
						default:
							sprintf_s(szTargetDist, "Lvl: %d %s %s", pInfo->Level, pEverQuest->GetRaceDesc(pInfo->mActorClient.Race), GetClassDesc(pInfo->mActorClient.Class));
							break;
						}
						SetCXStr(&InfoLabel->WindowText, szTargetDist);
						//then distance
						float dist = Distance3DToSpawn(pLocalPlayer, pTarget);
						sprintf_s(szTargetDist, "Dist: %.2f", dist);
						if (dist < 250) {
							DistanceLabel->CRNormal = 0xFF00FF00;//green
						}
						else {
							DistanceLabel->CRNormal = 0xFFFF0000;//red
						}
						SetCXStr(&DistanceLabel->WindowText, szTargetDist);
						//now do can see
						bool cansee = pCharSpawn->CanSee((EQPlayer*)pTarget);
						sprintf_s(szTargetDist, "%s", cansee ? "O" : "X");
						if (cansee) {
							CanSeeLabel->CRNormal = 0xFF00FF00;//green
						}
						else {
							CanSeeLabel->CRNormal = 0xFFFF0000;//red
						}
						SetCXStr(&CanSeeLabel->WindowText, szTargetDist);
					}
					else {
						SetCXStr(&InfoLabel->WindowText, "");
						SetCXStr(&DistanceLabel->WindowText, "");
						SetCXStr(&CanSeeLabel->WindowText, "");
						PHButton->dShow = false;
					}
				}
			}
		}
	}
}