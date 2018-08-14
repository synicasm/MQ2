

/*
// MQ2Radar by Odessa (eqplugins@gmail.com)
// 1.0 July 5/2007
// 2.0 July 8/2018 - eqmule updated to work again, removed dependency on DSurface.exe, there is no need for it anymore.
*/

#pragma warning(disable : 4244)
#pragma warning(disable : 4996)

#include "../MQ2Plugin.h"
#include "resource.h"
#include "DSurface.h"
#include <list>
using namespace std;
PreSetup("MQ2Radar");
HMODULE hDSurface = 0;
PLUGIN_VERSION(2.0);

//vDInput__MouseWheelScrolled was 0x6E3E10 08-18-09 I think it is actually __HandleMouseWheel

int vDInput__MouseWheelScrolled = __HandleMouseWheel;
int vCEverQuest__LeftClickedOnPlayer = CEverQuest__LeftClickedOnPlayer;

// Debug       DebugSpewNoFile("EndWhile - Macro was ended before we could handle the end of while command");
// DebugSpew("New element '%s' in color %X",pElement->Text,pElement->Color);

//define all global variables
int igSkipPulses = 1; // how many pulses to skip
int igSkipPulse = 0; // pulse counter
int igMode = 0; // radar mode
surface igSurface; // surface from DSurface
int igRadarButton = VK_F11; // toggle button
float fgZoom = 1.0f; // radar zoom
float fgScale = 1.0f; // radar scale
float fgZDepth = 20.0f; // radar ZDepth
float fgSizeUnit = 1.0f; // radar size unit
float fgAlertSpeed = 5.0f; // radar alert speed
float fgViewDistance; // radar view distance
point gCenter; // radar center
bool bgKeyPressed = false;
bool bgShowRadar = true;
bool bgDrawMap = false;
bool bgDrawTargetLine = false;
bool bgPluginInactive = false;
bool bgShowSize = false;
bool bgInGame = false;
bool bgDrawNames;
bool bgFilters[2] = { true,true };
sprite sItem; // Item sprite
sprite sSpawn; // Spawn sprite
sprite sRunning; // Running Spawn sprite
sprite sTarget; // Target sprite
sprite sCompass; // Compass sprite

				 //HWND EQhWnd; // EQ Window
FILE *pFile = NULL; // use to load maps
list<line> gMap; // list of map lines

				 // sloppy, sloppy, will change later
VOID LoadMap(char* pZone);

// Detours click on player
class CEQClick_Detour {
public:
	void EQClick_Trampoline(EQPlayer*);
	void EQClick_Detour(EQPlayer* pPlayer)
	{
		// If a CTRLClick happened within radar circle do not let EQ know about a click
		if (!GetAsyncKeyState(VK_LCONTROL) || !DSPointInCircle(gCenter.x, gCenter.y, fgViewDistance*fgScale, (float)EQADDR_MOUSE->X, (float)EQADDR_MOUSE->Y))
			EQClick_Trampoline(pPlayer);
	}
};
// Detours scroll wheel// Detours scroll wheel
void MouseScroll_Trampoline(int);
void MouseScroll_Detour(int scroll)
{
	// If we are holding CTRL and scrolling the mouse within a radar circle do zooming
	if (!GetAsyncKeyState(VK_LCONTROL) || !DSPointInCircle(gCenter.x, gCenter.y, fgViewDistance*fgScale, (float)EQADDR_MOUSE->X, (float)EQADDR_MOUSE->Y))
		MouseScroll_Trampoline(scroll);
	else {
		if (scroll<0)
			fgZoom += 0.1f;
		else
		{
			if (fgZoom >= 0.1f) fgZoom -= 0.1f;
		}
	}
}
DETOUR_TRAMPOLINE_EMPTY(void CEQClick_Detour::EQClick_Trampoline(EQPlayer*));
DETOUR_TRAMPOLINE_EMPTY(void MouseScroll_Trampoline(int));

// Checking if we have all proper variables initialized
bool InGame()
{
	return(gGameState == GAMESTATE_INGAME && GetCharInfo2() && GetCharInfo());
}

// A bunch of command processing functions, should be self-explanatory

void RadarCenter(float x, float y)
{
	gCenter.x = x;
	gCenter.y = y;
}

void RadarZoom(float zoom)
{
	fgZoom = zoom;
}

void RadarScale(float scale)
{
	fgScale = scale;
	DSChangeScale(igSurface, fgScale);
}

void RadarNames(bool drawNames)
{
	bgDrawNames = drawNames;
}

void RadarView(float viewDistance)
{
	fgViewDistance = viewDistance;
}

void RadarDelay(int skipPulses)
{
	igSkipPulse = 0;
	igSkipPulses = skipPulses;
}

void RadarMap(bool drawMap)
{
	bgDrawMap = drawMap;
	if (!bgDrawMap) DSClearLines(igSurface);
}

void RadarSpawnSize(bool showSize, float sizeUnit)
{
	bgShowSize = showSize;
	if (bgShowSize) {
		if (sizeUnit != 0)
			fgSizeUnit = sizeUnit;
		else
			fgSizeUnit = GetCharInfo()->pSpawn->AvatarHeight;
	}
}

void RadarTargetLine(bool drawTargetLine)
{
	bgDrawTargetLine = drawTargetLine;
	if (!bgDrawTargetLine) DSClearLines(igSurface);
}

void RadarAlertSpeed(float alertSpeed)
{
	fgAlertSpeed = alertSpeed;
}

void RadarMode(int iMode)
{
	if (iMode == 0 || iMode == 1)
		igMode = iMode;
}

void RadarZDepth(float zDepth)
{
	fgZDepth = zDepth;
}

void RadarHelp() {
	WriteChatColor("MQ2Radar:\n/radar COMMAND\nCOMMANDS:\ncenter x y\nview z\nscale z\nzoom z\nzdepth z\nalertspeed z\nmode 0/1\nnames on/off\nmap on/off\ntargetline on/off\ndelay z\nspawnsize on/off (z)\nsave\nreload\ntoggle\noptions");
}

// Check if the mouse is over a radar element
bool MouseOn(float x, float y, float degangle)
{
	return DSPointDegInCircle(gCenter.x - EQADDR_MOUSE->X, gCenter.y - EQADDR_MOUSE->Y, 8 * fgScale, x*fgScale, y*fgScale, degangle + 180.0f);
}

// Initialize radar
VOID RadarInitialize()
{
	igSkipPulse = 0;
	DSChangeScale(igSurface, fgScale);
	DSChangeCenter(igSurface, gCenter.x, gCenter.y);
	if (InGame())
		LoadMap(GetShortZone(GetCharInfo()->pSpawn->Zone));
}

VOID RadarFilter(char* filter, bool onoff)
{
	if (strncmp(filter, "PC", 2) == 0) {
		if (onoff) bgFilters[0] = true;
		else bgFilters[0] = false;
	}
	else if (strncmp(filter, "NPC", 3) == 0) {
		if (onoff) bgFilters[1] = true;
		else bgFilters[1] = false;
	}
}

// Get proper ini file name
VOID Update_INIFileName() {
	if (InGame()) {
		sprintf_s(INIFileName, "%s\\MQ2Radar_%s.ini", gszINIPath, GetCharInfo()->Name);
	}
	else {
		sprintf_s(INIFileName, "%s\\MQ2Radar.ini", gszINIPath);
	}
}
template <unsigned int _Size>LPSTR Safe_itoa_s(int _Value,char(&_Buffer)[_Size], int _Radix)
{
	errno_t err = _itoa_s(_Value, _Buffer, _Radix);
	if (!err) {
		return _Buffer;
	}
	return "";
}
VOID Load_MQ2SettingsRadar_INI()
{
	CHAR szTemp[MAX_STRING] = { 0 };
	CHAR szTemp2[MAX_STRING] = { 0 };
	sprintf_s(INIFileName, "%s\\MQ2Radar.ini", gszINIPath);

	/*GetPrivateProfileString("MQ2Radar", "CEverQuest__LeftClickedOnPlayer", Safe_itoa_s(vCEverQuest__LeftClickedOnPlayer, szTemp2, 16), &szTemp[0], sizeof(szTemp), INIFileName);
	vCEverQuest__LeftClickedOnPlayer = (int)strtol(&szTemp[0], NULL, 16);
	sprintf_s(szTemp, "%X", vCEverQuest__LeftClickedOnPlayer);
	WritePrivateProfileString("MQ2Radar", "CEverQuest__LeftClickedOnPlayer", szTemp, INIFileName);

	GetPrivateProfileString("MQ2Radar", "DInput__MouseWheelScrolled", Safe_itoa_s(vDInput__MouseWheelScrolled, szTemp2, 16), &szTemp[0], sizeof(szTemp), INIFileName);
	vDInput__MouseWheelScrolled = (int)strtol(&szTemp[0], NULL, 16);
	sprintf_s(szTemp, "%X", vDInput__MouseWheelScrolled);
	WritePrivateProfileString("MQ2Radar", "DInput__MouseWheelScrolled", szTemp, INIFileName);
	*/
}

// Load ini file and initialize with missing information
VOID Load_MQ2CharRadar_INI()
{
	CHAR szTemp[MAX_STRING] = { 0 };
	CHAR szSection[MAX_STRING] = { 0 };
	Update_INIFileName();

	GetPrivateProfileString("MQ2Radar", "CenterX", "110", &szTemp[0], sizeof(szTemp), INIFileName);
	gCenter.x = atoi(&szTemp[0]);
	sprintf_s(szTemp, "%d", (int)gCenter.x);
	WritePrivateProfileString("MQ2Radar", "CenterX", szTemp, INIFileName);

	GetPrivateProfileString("MQ2Radar", "CenterY", "110", &szTemp[0], sizeof(szTemp), INIFileName);
	gCenter.y = atoi(&szTemp[0]);
	sprintf_s(szTemp, "%d", (int)gCenter.y);
	WritePrivateProfileString("MQ2Radar", "CenterY", szTemp, INIFileName);

	GetPrivateProfileString("MQ2Radar", "View", "100", &szTemp[0], sizeof(szTemp), INIFileName);
	fgViewDistance = (float)atof(&szTemp[0]);
	sprintf_s(szTemp, "%f", fgViewDistance);
	WritePrivateProfileString("MQ2Radar", "View", szTemp, INIFileName);

	GetPrivateProfileString("MQ2Radar", "Scale", "1", &szTemp[0], sizeof(szTemp), INIFileName);
	fgScale = (float)atof(&szTemp[0]);
	sprintf_s(szTemp, "%f", fgScale);
	WritePrivateProfileString("MQ2Radar", "Scale", szTemp, INIFileName);

	GetPrivateProfileString("MQ2Radar", "Zoom", "0.7", &szTemp[0], sizeof(szTemp), INIFileName);
	fgZoom = (float)atof(&szTemp[0]);
	sprintf_s(szTemp, "%f", fgZoom);
	WritePrivateProfileString("MQ2Radar", "Zoom", szTemp, INIFileName);

	GetPrivateProfileString("MQ2Radar", "ZDepth", "5", &szTemp[0], sizeof(szTemp), INIFileName);
	fgZDepth = (float)atof(&szTemp[0]);
	sprintf_s(szTemp, "%f", fgZDepth);
	WritePrivateProfileString("MQ2Radar", "ZDepth", szTemp, INIFileName);

	GetPrivateProfileString("MQ2Radar", "Mode", "0", &szTemp[0], sizeof(szTemp), INIFileName);
	igMode = atoi(&szTemp[0]);
	sprintf_s(szTemp, "%d", igMode);
	WritePrivateProfileString("MQ2Radar", "Mode", szTemp, INIFileName);

	GetPrivateProfileString("MQ2Radar", "AlertSpeed", "5", &szTemp[0], sizeof(szTemp), INIFileName);
	fgAlertSpeed = (float)atof(&szTemp[0]);
	sprintf_s(szTemp, "%f", fgAlertSpeed);
	WritePrivateProfileString("MQ2Radar", "AlertSpeed", szTemp, INIFileName);

	GetPrivateProfileString("MQ2Radar", "Delay", "1", &szTemp[0], sizeof(szTemp), INIFileName);
	igSkipPulses = atoi(&szTemp[0]);
	sprintf_s(szTemp, "%d", igSkipPulses);
	WritePrivateProfileString("MQ2Radar", "Delay", szTemp, INIFileName);

	GetPrivateProfileString("MQ2Radar", "Names", "1", &szTemp[0], sizeof(szTemp), INIFileName);
	bgDrawNames = atoi(&szTemp[0]) != 0;
	sprintf_s(szTemp, "%d", bgDrawNames);
	WritePrivateProfileString("MQ2Radar", "Names", szTemp, INIFileName);

	GetPrivateProfileString("MQ2Radar", "Map", "1", &szTemp[0], sizeof(szTemp), INIFileName);
	bgDrawMap = atoi(&szTemp[0]) != 0;;
	sprintf_s(szTemp, "%d", bgDrawMap);
	WritePrivateProfileString("MQ2Radar", "Map", szTemp, INIFileName);

	GetPrivateProfileString("MQ2Radar", "TargetLine", "1", &szTemp[0], sizeof(szTemp), INIFileName);
	bgDrawTargetLine = atoi(&szTemp[0]) != 0;;
	sprintf_s(szTemp, "%d", bgDrawTargetLine);
	WritePrivateProfileString("MQ2Radar", "TargetLine", szTemp, INIFileName);

	GetPrivateProfileString("MQ2Radar", "ShowSize", "1", &szTemp[0], sizeof(szTemp), INIFileName);
	bgShowSize = atoi(&szTemp[0]) != 0;;
	sprintf_s(szTemp, "%d", bgShowSize);
	WritePrivateProfileString("MQ2Radar", "ShowSize", szTemp, INIFileName);

	GetPrivateProfileStringA("MQ2Radar", "SizeUnit", "7", &szTemp[0], sizeof(szTemp), INIFileName);
	fgSizeUnit = atof(&szTemp[0]);
	sprintf_s(szTemp, "%f", fgSizeUnit);
	WritePrivateProfileString("MQ2Radar", "SizeUnit", szTemp, INIFileName);

	RadarInitialize();
}

// Save ini file
void RadarSave()
{
	CHAR szTemp[MAX_STRING] = { 0 };
	CHAR szSection[MAX_STRING] = { 0 };

	Update_INIFileName();

	sprintf_s(szTemp, "%d", (int)gCenter.x);
	WritePrivateProfileString("MQ2Radar", "CenterX", szTemp, INIFileName);

	sprintf_s(szTemp, "%d", (int)gCenter.y);
	WritePrivateProfileString("MQ2Radar", "CenterY", szTemp, INIFileName);

	sprintf_s(szTemp, "%f", fgViewDistance);
	WritePrivateProfileString("MQ2Radar", "View", szTemp, INIFileName);

	sprintf_s(szTemp, "%f", fgScale);
	WritePrivateProfileString("MQ2Radar", "Scale", szTemp, INIFileName);

	sprintf_s(szTemp, "%f", fgZoom);
	WritePrivateProfileString("MQ2Radar", "Zoom", szTemp, INIFileName);

	sprintf_s(szTemp, "%f", fgZDepth);
	WritePrivateProfileString("MQ2Radar", "ZDepth", szTemp, INIFileName);

	sprintf_s(szTemp, "%d", igMode);
	WritePrivateProfileString("MQ2Radar", "Mode", szTemp, INIFileName);

	sprintf_s(szTemp, "%f", fgAlertSpeed);
	WritePrivateProfileString("MQ2Radar", "AlertSpeed", szTemp, INIFileName);

	sprintf_s(szTemp, "%d", igSkipPulses);
	WritePrivateProfileString("MQ2Radar", "Delay", szTemp, INIFileName);

	sprintf_s(szTemp, "%d", (int)bgDrawNames);
	WritePrivateProfileString("MQ2Radar", "Names", szTemp, INIFileName);

	sprintf_s(szTemp, "%d", (int)bgDrawMap);
	WritePrivateProfileString("MQ2Radar", "Map", szTemp, INIFileName);

	sprintf_s(szTemp, "%d", (int)bgDrawTargetLine);
	WritePrivateProfileString("MQ2Radar", "TargetLine", szTemp, INIFileName);

	sprintf_s(szTemp, "%d", (int)bgShowSize);
	WritePrivateProfileString("MQ2Radar", "ShowSize", szTemp, INIFileName);

	sprintf_s(szTemp, "%f", fgSizeUnit);
	WritePrivateProfileString("MQ2Radar", "SizeUnit", szTemp, INIFileName);

	WriteChatColor("MQ2Radar INI Saved...", CONCOLOR_GREEN);
}

VOID RadarShowOptions()
{
	CHAR szTemp[MAX_STRING] = { 0 };
	sprintf_s(szTemp, "Radar Options\nCenter = %d,%d \n ViewDistance = %f \nScale = %f \nZoom = %f \nZDepth = %f \nMode = %d \nAlertSpeed = %f \nSkipPulses = %d \nDrawNames = %d \nDrawMap = %d \nDrawTargetLine = %d \nShowSize = %d \nSizeUnit = %f",
		(int)gCenter.x, (int)gCenter.y, fgViewDistance, fgScale, fgZoom, fgZDepth, igMode, fgAlertSpeed, igSkipPulses, bgDrawNames, bgDrawMap, bgDrawTargetLine, bgShowSize, fgSizeUnit);
	WriteChatColor(szTemp);
}

// Load map information
void LoadMap(char* pZone) {
	char cBuffer[128];
	char* pTokens = { 0 };
	char cFileName[2048];
	line tempLine;
	gMap.clear();
	sprintf_s(cFileName, "%s\\maps\\%s_1.txt", gszEQPath, pZone);
	errno_t err = fopen_s(&pFile,cFileName, "rt");
	if (!err) {
		while (fgets(cBuffer, 128, pFile) != NULL) {
			pTokens = strtok(cBuffer, ", ");
			if (strcmp(pTokens, "L") == 0) {
				pTokens = strtok(NULL, " ,");
				tempLine.x1 = -atof(pTokens);
				pTokens = strtok(NULL, " ,");
				tempLine.y1 = -atof(pTokens);
				pTokens = strtok(NULL, " ,");
				tempLine.z1 = atof(pTokens);
				pTokens = strtok(NULL, " ,");
				tempLine.x2 = -atof(pTokens);
				pTokens = strtok(NULL, " ,");
				tempLine.y2 = -atof(pTokens);
				pTokens = strtok(NULL, " ,");
				tempLine.z2 = atof(pTokens);
				gMap.push_back(tempLine);
			}
		}
	}
	if (pFile) fclose(pFile);
}

// Show map on radar
void ShowMap(float x, float y, float z, float degangle)
{
	PSPAWNINFO pChar = GetCharInfo()->pSpawn;
	line tempLine;
	iline tempPoint;
	line finalLine;
	float x1, x2, y1, y2, z1, z2;
	float t1, t2;

	list<line>::iterator it;
	for (it = gMap.begin(); it != gMap.end(); it++) {
		tempLine = *it;

		x1 = (tempLine.x1 - x) / fgZoom;
		y1 = (tempLine.y1 - y) / fgZoom;
		x2 = (tempLine.x2 - x) / fgZoom;
		y2 = (tempLine.y2 - y) / fgZoom;
		z1 = tempLine.z1 - z;
		z2 = tempLine.z2 - z;
		z1 = abs(tempLine.z1 - pChar->Z);
		z2 = abs(tempLine.z2 - pChar->Z);

		float dist1 = GetDistance(tempLine.x1, tempLine.y1, x, y);
		float dist2 = GetDistance(tempLine.x2, tempLine.y2, x, y);

		if (dist1 <= (fgZoom*fgViewDistance) && dist2 <= (fgZoom*fgViewDistance)) {
			DSAddLine(igSurface, x1, y1, x2, y2, 0xffffffff);
		}
		else if ((abs(x2 - x1) >= 2 || abs(y2 - y1) >= 2))
		{
			// start assuming the points are within the circle, then correct
			finalLine.x1 = x1;
			finalLine.x2 = x2;
			finalLine.y1 = y1;
			finalLine.y2 = y2;

			// find the intersection points (only once)
			tempPoint = DSCLIntersection(0, 0, fgViewDistance, x1, y1, z1, x2, y2, z2);
			// in general t1,t2 should be identical regardless of if branch, but numerically
			// one may be more stable use that one.
			if (tempPoint.intersections == 2) {
				if (abs(x2 - x1) > abs(y2 - y1)) {
					t1 = (tempPoint.x1 - x1) / (x2 - x1);
					t2 = (tempPoint.x2 - x1) / (x2 - x1);
				}
				else {
					t1 = (tempPoint.y1 - y1) / (y2 - y1);
					t2 = (tempPoint.y2 - y1) / (y2 - y1);
				}
			}
			else {
				t1 = 2.0f;
				t2 = 2.0f;
			}

			if ((t1 < 0 && t2 < 0) || (t1 > 1 && t2 > 1)) {
				// Line segment doesn't cross circle 
			}
			else {
				// Line segment may cross circle

				// is point one outside the circle?  if so, clip this first end point to the circle
				// boundry using the closest intersection point 
				if (dist1 > (fgZoom*fgViewDistance)) {
					if (t1 < t2) {
						finalLine.x1 = tempPoint.x1;
						finalLine.y1 = tempPoint.y1;
					}
					else {
						finalLine.x1 = tempPoint.x2;
						finalLine.y1 = tempPoint.y2;
					}
				}
				// is point one outside the circle?  if so, clip this second end point to the circle 
				// boundry using the closest intersection point  (ie furthest from first end point)            
				if (dist2 > (fgZoom*fgViewDistance)) {
					if (t1 > t2) {
						finalLine.x2 = tempPoint.x1;
						finalLine.y2 = tempPoint.y1;
					}
					else {
						finalLine.x2 = tempPoint.x2;
						finalLine.y2 = tempPoint.y2;
					}
				}
				// draw the clipped line
				DSAddLine(igSurface, finalLine.x1, finalLine.y1, finalLine.x2, finalLine.y2, 0xffffffff);
			}
		}
	}
}

// Shows/Hides radar
void ShowHide() {
	if (bgShowRadar) {
		bgShowRadar = false;
		bgKeyPressed = true;
	}
	else {
		bgShowRadar = true;
		bgKeyPressed = true;
	}
}

// Draws all the ground spawns on radar
void DrawItems(PSPAWNINFO pChar, float heading) {
	PGROUNDITEM pItem = *(PGROUNDITEM*)pItemList;
	char name[64] = { 0 };
	DWORD color;
	while (pItem)
	{
		if (GetDistance(pChar->X, pChar->Y, pItem->X, pItem->Y)<(fgZoom*fgViewDistance) && abs(pItem->Z - pChar->Z)<fgZDepth) {
			color = 0xef9EBF19;
			// add a sprite to radar for every ground spawn
			DSAddSprite(igSurface, (pItem->X - pChar->X) / fgZoom, (pItem->Y - pChar->Y) / fgZoom, color, sItem, 0, 1);
			// if we are hovering over the radar ground spawn element show its name
			if (MouseOn((pItem->X - pChar->X) / fgZoom, (pItem->Y - pChar->Y) / fgZoom, heading))
			{
				GetFriendlyNameForGroundItem(pItem, name, sizeof(name));
				DSAddText(igSurface, EQADDR_MOUSE->X, EQADDR_MOUSE->Y - 20, EQADDR_MOUSE->X + 300, EQADDR_MOUSE->Y, name, 0x0ffffff00);
			}
		}
		pItem = pItem->pNext;
	}
}

// Draws all the PC/NPC spawns on the radar
void DrawSpawns(PSPAWNINFO pChar, float heading) {
	PSPAWNINFO pSpawn = (PSPAWNINFO)pSpawnList;
	iline tempCoord;
	DWORD color;
	float Size;
	while (pSpawn)
	{
		// Initialize spawn size depending on the /spawnsize
		if (bgShowSize) Size = pSpawn->AvatarHeight / fgSizeUnit;
		else Size = 1.0f;

		// If we have a target and its over the radar range draw target line
		if (GetDistance(pChar, pSpawn)>(fgZoom*fgViewDistance) && pSpawn == (PSPAWNINFO)pTarget && pSpawn != pChar && bgDrawTargetLine)
		{
			tempCoord = DSCLIntersection(0, 0, fgViewDistance, 0, 0, 0, (pSpawn->X - pChar->X) / fgZoom, (pSpawn->Y - pChar->Y) / fgZoom, 0);
			if ((pSpawn->Y - pChar->Y) / fgZoom>0) DSAddLine(igSurface, 0, 0, tempCoord.x1, tempCoord.y1, 0xffFFB400);
			else DSAddLine(igSurface, 0, 0, tempCoord.x2, tempCoord.y2, 0xffFFB400);
			// If the target is within radar range but outside ZDepth range draw target line
		}
		else if (GetDistance(pChar, pSpawn)<(fgZoom*fgViewDistance) && pSpawn == (PSPAWNINFO)pTarget && pSpawn != pChar && bgDrawTargetLine && abs(pSpawn->Z - pChar->Z) >= fgZDepth) {
			DSAddLine(igSurface, 0, 0, (pSpawn->X - pChar->X) / fgZoom, (pSpawn->Y - pChar->Y) / fgZoom, 0xffFFB400);
		}

		// If the spawn lies within radar range draw it
		if (GetDistance(pChar, pSpawn)<(fgZoom*fgViewDistance) && abs(pSpawn->Z - pChar->Z)<fgZDepth) {
			// Target spawn on radar click
			if (MouseOn((pSpawn->X - pChar->X) / fgZoom, (pSpawn->Y - pChar->Y) / fgZoom, heading))
				if (EQADDR_MOUSECLICK->Click[0] && pSpawn != (PSPAWNINFO)pTarget) pTarget = (EQPlayer*)pSpawn;
			//set up proper colors for our spawns
			color = 0xefff00ff;
			if (GetSpawnType(pSpawn) == NPC) {
				DWORD visibility; // Transparency variable for ZDepth sorting
				if (abs(pSpawn->Z - pChar->Z) >= 180)
					visibility = 0x1E000000;
				else
					visibility = ((DWORD)(0xFF - abs(pSpawn->Z - pChar->Z)*1.275f)) << 24;
				color = (ConColorToARGB(ConColor(pSpawn)) & 0x00FFFFFF) | (DWORD)visibility;
			}
			// If we are drawing a target spawn draw both spawn sprite and target sprite
			if (pSpawn == (PSPAWNINFO)pTarget && pSpawn != pChar) {
				DSAddSprite(igSurface, (pSpawn->X - pChar->X) / fgZoom, (pSpawn->Y - pChar->Y) / fgZoom, color, sSpawn, 180 - pSpawn->Heading*0.703125f, Size);
				DSAddSprite(igSurface, (pSpawn->X - pChar->X) / fgZoom, (pSpawn->Y - pChar->Y) / fgZoom, 0xffffffff, sTarget, 0, Size);
				if (bgDrawTargetLine) DSAddLine(igSurface, 0, 0, (pSpawn->X - pChar->X) / fgZoom, (pSpawn->Y - pChar->Y) / fgZoom, 0xffFFB400);
			}
			// If we are drawing ourselves just draw a proper white sprite
			else if (pSpawn == pChar)
				DSAddSprite(igSurface, 0, 0, 0xffffff00, sSpawn, 180.0f - pChar->Heading*0.703125f, 0.7f);
			// Draw a normal PC/NPC spawn
			else if (GetSpawnType(pSpawn) == NPC && bgFilters[1]) {
				DSAddSprite(igSurface, (pSpawn->X - pChar->X) / fgZoom, (pSpawn->Y - pChar->Y) / fgZoom, color, sSpawn, 180 - pSpawn->Heading*0.703125f, Size);
				// If our alertspeed is enabled and the target is running draw alertspeed sprite
				if (abs(pSpawn->SpeedRun) >= fgAlertSpeed)
					DSAddSprite(igSurface, (pSpawn->X - pChar->X) / fgZoom, (pSpawn->Y - pChar->Y) / fgZoom, 0xffB1FFFF, sRunning, 180 - pSpawn->Heading*0.703125f, Size);
			}
			else if (GetSpawnType(pSpawn) == PC && bgFilters[0]) {
				DSAddSprite(igSurface, (pSpawn->X - pChar->X) / fgZoom, (pSpawn->Y - pChar->Y) / fgZoom, color, sSpawn, 180 - pSpawn->Heading*0.703125f, Size);
				// If our alertspeed is enabled and the target is running draw alertspeed sprite
				if (abs(pSpawn->SpeedRun) >= fgAlertSpeed)
					DSAddSprite(igSurface, (pSpawn->X - pChar->X) / fgZoom, (pSpawn->Y - pChar->Y) / fgZoom, 0xffB1FFFF, sRunning, 180 - pSpawn->Heading*0.703125f, Size);
			}
			// If /rnames is on draw names on radar
			if (bgDrawNames && (GetSpawnType(pSpawn) == NPC || GetSpawnType(pSpawn) == PC)) {
				point newCoord;
				newCoord = DSPointDeg((pSpawn->X - pChar->X) / fgZoom, (pSpawn->Y - pChar->Y) / fgZoom, heading);
				if (pSpawn != pChar)
					DSAddText(igSurface, newCoord.x*fgScale + gCenter.x, newCoord.y*fgScale + gCenter.y - 20, newCoord.x*fgScale + gCenter.x + 300, newCoord.y*fgScale + gCenter.y, CleanupName(pSpawn->Name, sizeof(pSpawn->Name),0,0), 0xff00ffff);
			}
			else {
				// If mouse is over a radar spawn show the name
				if (MouseOn((pSpawn->X - pChar->X) / fgZoom, (pSpawn->Y - pChar->Y) / fgZoom, heading)) {
					DSAddText(igSurface, EQADDR_MOUSE->X, EQADDR_MOUSE->Y - 20, EQADDR_MOUSE->X + 300, EQADDR_MOUSE->Y, CleanupName(pSpawn->Name,sizeof(pSpawn->Name),0,0), 0xff00ffff);
				}
			}
		}
		pSpawn = pSpawn->pNext;
	}
}

PLUGIN_API VOID OnPulse(VOID)
{
	// Check if the plugin is active, if we are in foreground, if we are in game and if the radar is on-screen
	if (!bgPluginInactive) {
		HWND EQhWnd = *(HWND*)EQADDR_HWND;
		if (GetForegroundWindow() == EQhWnd) {
			if (InGame()) {
				if (GetKeyState(igRadarButton) & 0x80) {
					if (!bgKeyPressed) ShowHide();
				}
				else {
					bgKeyPressed = false;
				}
				if (bgShowRadar) {
					PSPAWNINFO pChar = GetCharInfo()->pSpawn;
					float Heading;
					if (igSkipPulse == igSkipPulses) {
						igSkipPulse = 0;
						// Clear everything
						DSClearSprites(igSurface);
						DSClearText(igSurface);
						DSClearLines(igSurface);
						// Draw map
						if (bgDrawMap) ShowMap(pChar->X, pChar->Y, pChar->Z, pChar->Heading*0.703125f);

						// Change center of radar
						DSChangeCenter(igSurface, gCenter.x, gCenter.y);

						// Depending on radar mode set Heading
						if (igMode == 0) Heading = pChar->Heading*0.703125f - 180.0f;
						else if (igMode == 1) Heading = 180.0f;

						// Change radar angle depending on Heading
						DSChangeAngle(igSurface, Heading);

						// Add Compass sprite
						//DSAddSprite(igSurface, 0, 0, 0x0f0FF6600, sCompass, 180.0f, fgViewDistance / 120.0f);
						DSAddSprite(igSurface, 0, 0, 0xF0FF0606, sCompass, 180.0f, fgViewDistance / 120.0f);

						// Draw Ground Spawns
						DrawItems(pChar, Heading);
						// Draw Spawns
						DrawSpawns(pChar, Heading);
					}
					igSkipPulse++;
				}
				else {
					// Clear everything
					DSClearSprites(igSurface);
					DSClearText(igSurface);
					DSClearLines(igSurface);
				}
			}
		}
	}
}

// Command parser
void Radar(PSPAWNINFO pChar, PCHAR szLine)
{
	CHAR szSwitch[MAX_STRING] = { 0 };
	GetArg(szSwitch, szLine, 1);
	if (_stricmp(szSwitch, "center") == 0) RadarCenter((float)atof(GetNextArg(szLine, 1)), (float)atof(GetNextArg(szLine, 2)));
	else if (_stricmp(szSwitch, "zoom") == 0) RadarZoom((float)atof(GetNextArg(szLine, 1)));
	else if (_stricmp(szSwitch, "scale") == 0) RadarScale((float)atof(GetNextArg(szLine, 1)));
	else if (_stricmp(szSwitch, "names") == 0) RadarNames(_stricmp(GetNextArg(szLine, 1), "on") ? false : true);
	else if (_stricmp(szSwitch, "save") == 0) RadarSave();
	else if (_stricmp(szSwitch, "view") == 0) RadarView((float)atof(GetNextArg(szLine, 1)));
	else if (strcmp(szSwitch, "delay") == 0) RadarDelay(atoi(GetNextArg(szLine, 1)));
	else if (_stricmp(szSwitch, "map") == 0) RadarMap(_stricmp(GetNextArg(szLine, 1), "on") ? false : true);
	else if (strcmp(szSwitch, "zdepth") == 0) RadarZDepth((float)atof(GetNextArg(szLine, 1)));
	else if (_stricmp(szSwitch, "targetline") == 0) RadarTargetLine(_stricmp(GetNextArg(szLine, 1), "on") ? false : true);
	else if (strcmp(szSwitch, "alertspeed") == 0) RadarAlertSpeed((float)atof(GetNextArg(szLine, 1)));
	else if (strcmp(szSwitch, "mode") == 0) RadarMode(atoi(GetNextArg(szLine, 1)));
	else if (strcmp(szSwitch, "spawnsize") == 0) RadarSpawnSize(strncmp(GetNextArg(szLine, 1), "on", 2) ? false : true, (float)atof(GetNextArg(szLine, 2)));
	else if (strcmp(szSwitch, "reload") == 0) Load_MQ2CharRadar_INI();
	else if (strcmp(szSwitch, "options") == 0) RadarShowOptions();
	else if (_stricmp(szSwitch, "filter") == 0) RadarFilter(GetNextArg(szLine, 1), _stricmp(GetNextArg(szLine, 2), "on") ? false : true);
	else if (strcmp(szSwitch, "toggle") == 0) ShowHide();
	else RadarHelp();
}

PLUGIN_API VOID InitializePlugin()
{
	DebugSpewAlways("MQ2Radar:: Starting.");
	//MessageBox(NULL, "inject", "", MB_SYSTEMMODAL | MB_OK);
	HMODULE hMQ2Radar = 0;
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCTSTR)InitializePlugin, &hMQ2Radar);
	CHAR szPath[MAX_STRING] = { 0 };
	GetModuleFileNameA(hMQ2Radar, szPath, MAX_STRING);
	if (szPath[0]) {
		if (char *pDest = strrchr(szPath, '\\')) {
			pDest[0] = '\0';
			strcat_s(szPath, "\\DSurface.dll");
		}
	}
	void* pMyBinaryData = 0;
	WIN32_FIND_DATA FindFile = { 0 };
	HANDLE hSearch = FindFirstFile(szPath, &FindFile);
	if (hSearch == INVALID_HANDLE_VALUE) {
		//need to unpack our resource.
		if (HRSRC hRes = FindResource(hMQ2Radar, MAKEINTRESOURCE(IDR_DLL), "DLL")) {
			if (HGLOBAL bin = LoadResource(hMQ2Radar, hRes)) {
				BOOL bResult = 0;
				if (pMyBinaryData = LockResource(bin)) {
					//save it...
					DWORD ressize = SizeofResource(hMQ2Radar, hRes);
					FILE *File = 0;
					errno_t err = fopen_s(&File, szPath, "wb");
					if (!err) {
						fwrite(pMyBinaryData, ressize, 1, File);
						fclose(File);
					}
					bResult = UnlockResource(hRes);
				}
				bResult = FreeResource(hRes);
			}
		}
	} else {
		FindClose(hSearch);
	}


	hDSurface = GetModuleHandle("DSurface.dll");
	if (!hDSurface) {
		hDSurface = LoadLibrary(szPath);
	}
	// Check if DirectSurface is present
	if (InitializeDS()) {
		WriteChatColor("MQ2Radar (July 5/2007) by Odessa (eqplugins@gmail.com)", CONCOLOR_YELLOW);
		// Check if we have a proper DSurface version
		if (atof(DSGetVersion()) >= 0.110) {
			//EQhWnd = *(HWND*)EQADDR_HWND;
			igSurface = DSCreateSurface(); // get our Surface
			DebugSpewAlways("MQ2Radar::Got Surface - %d", igSurface);

			// Create our sprites to be used in radar
			sCompass = DSCreateSprite(igSurface, hMQ2Radar, MAKEINTRESOURCE(IDB_Compass), 256, 256, 256, 256, 1, 0, 2, 1, 0xFF000000);
			sItem = DSCreateSprite(igSurface, hMQ2Radar, MAKEINTRESOURCE(IDB_Item), 8, 8, 8, 8, 1, 0, 2, 1, 0xFF000000);
			sRunning = DSCreateSprite(igSurface, hMQ2Radar, MAKEINTRESOURCE(IDB_Running), 160, 32, 32, 32, 5, 0, 1, 2, 0xFF000000);
			sSpawn = DSCreateSprite(igSurface, hMQ2Radar, MAKEINTRESOURCE(IDB_Spawn), 16, 16, 16, 16, 1, 0, 2, 1, 0xFF000000);
			sTarget = DSCreateSprite(igSurface, hMQ2Radar, MAKEINTRESOURCE(IDB_Target), 32, 32, 32, 32, 9, 10, 2, 1, 0xFF000000);
			/*DSAddSprite(igSurface,100, 100, 0xffFF6600,sCompass,180.0f,1.0f);
			DSAddSprite(igSurface,200, 200, 0xffFF6600,sItem,180.0f,1.0f);
			DSAddSprite(igSurface,300, 300, 0xffFF6600,sRunning,180.0f,1.0f);
			DSAddSprite(igSurface,400, 400, 0xffFF6600,sSpawn,180.0f,1.0f);
			DSAddSprite(igSurface,500, 500, 0xffFF6600,sTarget,180.0f,1.0f);*/

			// If we are in game load map and ini files
			/*if (InGame()) {
			LoadMap(GetShortZone(GetCharInfo()->pSpawn->Zone));
			Load_MQ2Radar_INI();
			}*/
			Load_MQ2SettingsRadar_INI();
			AddCommand("/radar", Radar);
			if (vCEverQuest__LeftClickedOnPlayer != 0)
				EzDetourwName(vCEverQuest__LeftClickedOnPlayer, &CEQClick_Detour::EQClick_Detour, &CEQClick_Detour::EQClick_Trampoline,"CEverQuest__LeftClickedOnPlayer");
			if (vDInput__MouseWheelScrolled != 0)
				EzDetourwName(vDInput__MouseWheelScrolled, &MouseScroll_Detour, &MouseScroll_Trampoline,"__HandleMouseWheel");
		}
		else {
			WriteChatColor("MQ2Radar (July 5/2007) requires DirectSurface 0.110 or newer", CONCOLOR_RED);
			DebugSpewAlways("MQ2Radar Failed.");
			bgPluginInactive = true;
		}
	}
	else {
		WriteChatColor("MQ2Radar (July 5/2007) requires DirectSurface 0.110 or newer", CONCOLOR_RED);
		DebugSpewAlways("MQ2Radar::DirectSurface not present.");
		DebugSpewAlways("MQ2Radar Failed.");
		bgPluginInactive = true;
	}
}

PLUGIN_API VOID ShutdownPlugin(VOID)
{
	DebugSpewAlways("MQ2Radar:: Shutting Down.");
	if (!bgPluginInactive) {
		RemoveCommand("/radar");
		if (vCEverQuest__LeftClickedOnPlayer != 0)
			RemoveDetour(CEverQuest__LeftClickedOnPlayer);
		if (vDInput__MouseWheelScrolled != 0)
			RemoveDetour(vDInput__MouseWheelScrolled);
		// MAKE SURE to destroy a surface upon exit
		DSDestroySurface(igSurface);
		//cant do this we crash...
		//if (hDSurface)
		//	FreeLibrary(hDSurface);
	}
}

PLUGIN_API VOID SetGameState(DWORD GameState)
{
	if (!bgPluginInactive) {
		if (!bgInGame && InGame())
		{
			bgInGame = true;
			Load_MQ2CharRadar_INI();
		}
		else {
			DSClearSprites(igSurface);
			DSClearText(igSurface);
			DSClearLines(igSurface);
		}
	}
}

PLUGIN_API VOID OnBeginZone(VOID)
{
	if (!bgPluginInactive) {
		//clear everything on zone
		DebugSpewAlways("MQ2Radar - begin zone");
		DSClearSprites(igSurface);
		DSClearText(igSurface);
		DSClearLines(igSurface);
	}
}

PLUGIN_API VOID OnEndZone(VOID)
{
	if (!bgPluginInactive) {
		// if we just zoned reload map
		//if (InGame()) LoadMap(GetShortZone(GetCharInfo()->pSpawn->Zone));
		LoadMap(GetShortZone(GetCharInfo()->pSpawn->Zone));
	}
}
