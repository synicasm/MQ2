

// MQ2FamiliarKiller.cpp :
//Kills Wizard Flappies on zone and checks now and then if another one is up.
//Those pesky Familiars keep showing up all the time :)

#include "../MQ2Plugin.h"

PreSetup("MQ2FamKiller");
PLUGIN_VERSION(1.32);

ULONG Check_TS = 0, RI = 0;
BOOL Have_Fam, WizOnly = true, MeZoning;
ULONG Check_OnPulse = 0;

void ToggleWizOnly(PSPAWNINFO pChar, PCHAR szLine)
{
	WizOnly = !WizOnly;
	WriteChatf("\ayMQ2FamKiller\aw: \atWizOnly\ax now %s\ax.", WizOnly?"\agENABLED":"\arDISABLED");
	WritePrivateProfileString("Settings","WizOnly",WizOnly?"on":"off",INIFileName);
}

void KillFam()
{
	if(Have_Fam) {
		DoCommand(NULL,"/familiar leave");
		WriteChatf("\ar[\ayMQ2FamKiller\ar] \agRest in peace flappy.");
		Have_Fam = false;
	}
}

bool WizClass() {
	if(!WizOnly)
		return true;
	if(GetCharInfo2())
		return (strncmp(pEverQuest->GetClassDesc(GetCharInfo()->pSpawn->mActorClient.Class & 0xFF),"Wizard",6))?false:true;
	return false;
}

PLUGIN_API VOID InitializePlugin(VOID)
{
	char szTemp[MAX_STRING];
	DebugSpewAlways("Initializing MQ2FamKiller");
	Check_OnPulse = 0;
	Have_Fam = false;
	if(gGameState != GAMESTATE_INGAME)
		MeZoning = true;
	else
		MeZoning = false;
	if (GetPrivateProfileString("Settings","WizOnly",NULL,szTemp,MAX_STRING,INIFileName)) {
		if (!_stricmp(szTemp,"off"))
			WizOnly = false;
		else
			WizOnly = true;
	}
	else {
		WritePrivateProfileString("Settings","WizOnly", "on",INIFileName); 
		WizOnly = true;
	}
	AddCommand("/wizonly", ToggleWizOnly);
}

PLUGIN_API VOID ShutdownPlugin(VOID)
{
	DebugSpewAlways("Shutting down MQ2FamKiller");
	RemoveCommand("/wizonly");
}

PLUGIN_API VOID OnAddSpawn(PSPAWNINFO pNewSpawn)
{
	if(!pNewSpawn || !GetCharInfo())
		return;
	if (strstr(pNewSpawn->DisplayedName, "familiar")) {
		if(GetSpawnType(pNewSpawn)==PET && WizClass() && pNewSpawn->Level > 1) {
			if(GetCharInfo()->pSpawn) {
				if(strstr(pNewSpawn->DisplayedName,GetCharInfo()->pSpawn->DisplayedName)) {
					Check_TS = GetTickCount();
					if(MeZoning)
						RI = 10000000UL;
					else {
						srand((unsigned)time(NULL));
						RI = (ULONG)((double)rand() / (RAND_MAX + 1) * 5000 + 5000);
					}
					Have_Fam = true;
				}
			}
		}
	}
} 

PLUGIN_API VOID OnBeginZone(VOID)
{
	MeZoning = true;
}

PLUGIN_API VOID OnEndZone(VOID)
{
	char szTemp[MAX_STRING];
	Check_OnPulse = 0;
	if (GetPrivateProfileString("Settings","WizOnly",NULL,szTemp,MAX_STRING,INIFileName)) {
		if (!_stricmp(szTemp,"on"))
			WizOnly = true;
		else
			WizOnly = false;
	}
	else {
		WritePrivateProfileString("Settings","WizOnly", "off",INIFileName); 
		WizOnly = false;
	}
	MeZoning = false;
	srand((unsigned)time(NULL));
	RI = (ULONG)((double)rand() / (RAND_MAX + 1) * 5000 + 5000);
}

PLUGIN_API VOID OnPulse(VOID) 
{
	unsigned long Tmp;
	// let's only check about every 20 pulses or so
	Check_OnPulse++;
	if(Check_OnPulse<20)
		return;
	Check_OnPulse = 0;
	if (gGameState==GAMESTATE_INGAME && !MeZoning) {
        Tmp=GetTickCount();
		if(Tmp>=Check_TS+RI) {
			if(Have_Fam) {
				KillFam();
			}
		}
	}
} 