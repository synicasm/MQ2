


// MQ2SoundControl.cpp : Defines the entry point for the DLL application.
//
//
// PLUGIN_API is only to be used for callbacks.  All existing callbacks at this time
// are shown below. Remove the ones your plugin does not use.  Always use Initialize
// and Shutdown for setup and cleanup, do NOT do it in DllMain.
//
// This program is free software; you can redistribute it and/or modify 
// it under the terms of the GNU General Public License, version 2, as published by 
// the Free Software Foundation. 
//
// This program is distributed in the hope that it will be useful, 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
// GNU General Public License for more details.
//Version 2.0 Removed patterns - SwiftyMUSE Apr 23 2018

#define PLUGIN_NAME  "MQ2SoundControl"	// Plugin Name
#define PLUGIN_DATE   20180423			// Plugin Date
#define PLUGIN_VERS   2.00				// Plugin Version

#ifndef PLUGIN_API
#include "../MQ2Plugin.h"
PreSetup(PLUGIN_NAME);
PLUGIN_VERSION(PLUGIN_VERS);
#endif PLUGIN_API

using namespace std;

std::map<int, float> soundPref;
bool bDebugSounds = false;


struct _SOUNDMANAGER {
	/*0x000*/ BYTE  Unknown0x0[0x24];
	/*0x024*/ DWORD Unknown0x24;
	/*0x028*/ DWORD Unknown0x28;
	/*0x02c*/ DWORD Unknown0x2c;
	/*0x030*/ BYTE  Unknown0x30[0xfbc];
	/*0xfec*/ float SoundRealism;
	/*0xff0*/ float SoundVolume;
};

struct _SOUNDASSET {
	/*0x000*/ BYTE  Unknown0x0[0x8];
	/*0x008*/ char  Name[0x40];
	/*0x048*/ BYTE  Unknown0x48[0x1c0];
	/*0x208*/ DWORD Unknown0x208;
	/*0x20c*/ DWORD nSound;
	/*0x210*/ DWORD fileType;// 1=wav, 2=mp3, 3=ogg
	/*0x214*/ DWORD Unknown0x214;
	/*0x218*/ DWORD Unknown0x218;
	/*0x21c*/ DWORD Unknown0x21c;
	/*0x220*/
};

struct _WAVEINSTANCE {
	/*0x00*/ void *vftable;
	/*0x04*/ DWORD Unknown0x4;
	/*0x08*/ void *v3Loc;
	/*0x0c*/ _SOUNDASSET *pSoundAsset;
	/*0x10*/ BYTE Unknown0x10[0x8];
	/*0x18*/ void *sample;
	/*0x1c*/ BYTE Unknown0x1c[0x8];
	/*0x24*/ float f24;
};

_SOUNDMANAGER *pEQSoundManager = 0;

bool FindSoundPref(int n)
{
	map<int, float>::iterator it = soundPref.find(n);
	if (it == soundPref.end())
	{
		return false;
	}
	return true;
}

void SetPref(int nSound, float volume)
{
	if (volume == 0.0f)
	{
		// EQ doesn't seem to like if the volume is set to 0, so we'll just make it so quiet you can't even hear it
		volume = 0.001f;
	}
	soundPref[nSound] = volume / 100.0f;
}

void LoadIni()
{
	char szTemp[MAX_STRING] = { 0 };
	GetPrivateProfileString("SoundVolumes", 0, 0, szTemp, MAX_STRING, INIFileName);
	char *p = szTemp;
	for (int i = 0; i == 0 || (szTemp[i - 1] != 0 || szTemp[i] != 0); i++)
	{
		if (szTemp[i] == 0)
		{
			int n = atoi(p);
			int f = (int)((pEQSoundManager->SoundVolume + 0.005f) * 100.0f);
			float v = (float)GetPrivateProfileIntA("SoundVolumes", p, i, INIFileName);
			SetPref(n, v);
			p = &szTemp[i + 1];
		}
	}
}

void Cmd_Sound(PSPAWNINFO pChar, char *szLine)
{
	char szArg1[MAX_STRING] = { 0 };
	char szArg2[MAX_STRING] = { 0 };
	char szArg3[MAX_STRING] = { 0 };

	GetArg(szArg1, szLine, 1);
	GetArg(szArg2, szLine, 2);
	GetArg(szArg3, szLine, 3);

	if (szArg1[0])
	{
		if (!_stricmp(szArg1, "vol") && szArg2[0] && szArg3[0])
		{
			int n = atoi(szArg2);
			float f = (float)atof(szArg3);

			if (n)
			{
				WritePrivateProfileString("SoundVolumes", szArg2, szArg3, INIFileName);
				SetPref(n, f);
				WriteChatf("Sound \ag%d\ax volume set to \ag%.0f\ax", n, f);
				return;
			}
		}
		else if (!_stricmp(szArg1, "debug"))
		{
			bDebugSounds = !bDebugSounds;
			WriteChatf("Sound debugging is %s", bDebugSounds ? "\agON\ax" : "\arOFF\ax");
			return;
		}
		else if (!_stricmp(szArg1, "del") && szArg2[0])
		{
			WritePrivateProfileString("SoundVolumes", szArg2, 0, INIFileName);
			WriteChatf("Sound adjustment for \ag%d\ax deleted", atoi(szArg2));
			return;
		}
	}

	WriteChatf("\ayUsage:\ax");
	WriteChatf("/sound vol <sound number> <volume level> \at(volume level is 0 - 100)\ax");
	WriteChatf("/sound debug \at(outputs sound numbers to MQ2ChatWnd)\ax");
}

void ClearPrefMap()
{
	soundPref.clear();
}

class _SoundControl
{
public:
	_SoundControl()
	{
		memset(this, 0, sizeof(_SoundControl));
		soundVolume = pEQSoundManager->SoundVolume;
		Unknown0x8 = 1;
		soundRealism = pEQSoundManager->SoundRealism;
	}
	/*0x00*/ float  soundVolume;
	/*0x04*/ DWORD  Unknown0x4;
	/*0x08*/ DWORD  Unknown0x8;
	/*0x0c*/ float  Unknown0xc;
	/*0x10*/ float  soundLocY;
	/*0x14*/ float  soundLocX;
	/*0x18*/ float  soundLocZ;
	/*0x1c*/ float  Unknown0x1c;
	/*0x20*/ float  Unknown0x20;
	/*0x24*/ float  soundRealism;
	/*0x28*/ DWORD  Unknown0x28;
	/*0x2c*/ BYTE   Unknown0x2c;
	/*0x2d*/ BYTE   Unknown0x2d;
	/*0x2e*/ BYTE   Unknown0x2e[0x2];
	/*0x30*/
};

class WaveInstance_Hook
{
public:
	bool Play_T(_SoundControl*);
	bool Play_D(_SoundControl *s)
	{
		_WAVEINSTANCE *wIn = (_WAVEINSTANCE*)this;

		if (!s)
		{
			if (wIn->pSoundAsset)
			{
				bool b = FindSoundPref(wIn->pSoundAsset->nSound);

				if (bDebugSounds && wIn->pSoundAsset->Name[0])
				{
					char *c = strrchr(wIn->pSoundAsset->Name, '\\') + 1;
					if (b)
					{
						WriteChatf("\ay%s\ax - %d (%d)", c ? c : "ERROR", wIn->pSoundAsset->nSound, (DWORD)((soundPref[wIn->pSoundAsset->nSound] + 0.005f) * 100));
					}
					else
					{
						WriteChatf("\at%s\ax - %d", c ? c : "ERROR", wIn->pSoundAsset->nSound);
					}
				}

				_SoundControl sc;

				if (b)
				{
					sc.soundVolume = soundPref[wIn->pSoundAsset->nSound];
				}

				return Play_T(&sc);
			}
		}

		return Play_T(s);
	}
};

class SoundAssetHook
{
public:
	void *Play_T(_SoundControl*);
	void *Play_D(_SoundControl *s)
	{
		_SOUNDASSET *sa = (_SOUNDASSET*)this;

		bool b = FindSoundPref(sa->nSound);

		if (bDebugSounds && sa->Name[0])
		{
			if (b)
			{
				WriteChatf("\ay%s\ax - %d (%d)", sa->Name, sa->nSound, (DWORD)((soundPref[sa->nSound] + 0.005f) * 100));
			}
			else
			{
				WriteChatf("\at%s\ax - %d", sa->Name, sa->nSound);
			}
		}

		if (b)
		{
			if (s)
			{
				float f = s->soundVolume;
				s->soundVolume = soundPref[sa->nSound];
				void *ret = Play_T(s);
				s->soundVolume = f;
				return ret;
			}
		}

		return Play_T(s);
	}
};

DETOUR_TRAMPOLINE_EMPTY(bool WaveInstance_Hook::Play_T(_SoundControl*));
DETOUR_TRAMPOLINE_EMPTY(void *SoundAssetHook::Play_T(_SoundControl*));

PLUGIN_API VOID InitializePlugin(VOID)
{
	DebugSpewAlways("Initializing MQ2SoundControl");

	pEQSoundManager = *(_SOUNDMANAGER**)FixOffset(pinstEQSoundManager_x);

#if EqSoundManager__WaveInstancePlay_x
	EzDetour(EqSoundManager__WaveInstancePlay, &WaveInstance_Hook::Play_D, &WaveInstance_Hook::Play_T);
#endif
#if EqSoundManager__SoundAssistPlay_x
	EzDetour(EqSoundManager__SoundAssistPlay, &SoundAssetHook::Play_D, &SoundAssetHook::Play_T);
#endif

	AddCommand("/sound", Cmd_Sound);

	LoadIni();
}

PLUGIN_API VOID ShutdownPlugin(VOID)
{
	DebugSpewAlways("Shutting down MQ2SoundControl");

#if EqSoundManager__WaveInstancePlay_x
	RemoveDetour(EqSoundManager__WaveInstancePlay);
#endif
#if EqSoundManager__SoundAssistPlay_x
	RemoveDetour(EqSoundManager__SoundAssistPlay);
#endif

	RemoveCommand("/sound");

	ClearPrefMap();
}