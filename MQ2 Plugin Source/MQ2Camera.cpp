

//
// MQ2Camera.cpp
// written by brainiac
// https://github.com/brainiac/MQ2Camera
//
// version information
//Version 1.10 Updated patterns - SwiftyMUSE Feb 21 2018
//Version 2.00 Removed patterns - SwiftyMUSE Apr 23 2018
//Version 2.01 WriteChatf CrashFix - Eqmule May 29 2018

#define PLUGIN_NAME  "MQ2Camera"		// Plugin Name
#define PLUGIN_DATE   20180423			// Plugin Date
#define PLUGIN_VERS   2.01				// Plugin Version

#ifndef PLUGIN_API
#include "../MQ2Plugin.h"
PreSetup(PLUGIN_NAME);
PLUGIN_VERSION(PLUGIN_VERS);
#endif PLUGIN_API

#define PLUGIN_MSG "\ag[MQ2Camera]\ax "

#include <algorithm>
using namespace std;

namespace
{
	float s_origZoomCameraMaxDistance = 0.0;
	float s_origUserCameraMaxDistance = 0.0;

	bool s_initialized = false;

	float* ZoomCameraMaxDistance = 0;
	float* UserCameraMaxDistance = 0;
}

bool InitValues()
{
	UserCameraMaxDistance = (float *)FixOffset(__gfMaxCameraDistance_x);
	ZoomCameraMaxDistance = (float *)FixOffset(__gfMaxZoomCameraDistance_x);

	s_origUserCameraMaxDistance = *UserCameraMaxDistance;
	s_origZoomCameraMaxDistance = *ZoomCameraMaxDistance;

	return true;
}

void SetCameraValue(float* cameraValue, float value)
{
	DWORD oldperm;
	VirtualProtectEx(GetCurrentProcess(), (LPVOID)cameraValue, 4, PAGE_EXECUTE_READWRITE, &oldperm);
	*cameraValue = value;
	VirtualProtectEx(GetCurrentProcess(), (LPVOID)cameraValue, 4, oldperm, &oldperm);
}

void ResetCameraDistance()
{
	if (s_initialized)
	{
		SetCameraValue(ZoomCameraMaxDistance, s_origZoomCameraMaxDistance);
		SetCameraValue(UserCameraMaxDistance, s_origUserCameraMaxDistance);
	}
}

void SetCameraDistance(float distance)
{
	if (s_initialized)
	{
#ifdef max
#undef max
#endif
		float zoomValue = std::max(distance, s_origZoomCameraMaxDistance);
		SetCameraValue(ZoomCameraMaxDistance, zoomValue);

		float userValue = std::max(distance, s_origUserCameraMaxDistance);
		SetCameraValue(UserCameraMaxDistance, userValue);

		WriteChatf("%s:: Camera max distance set to \ay%.2f\ax", PLUGIN_MSG, distance);
	}
}

void SaveCameraDistance(float distance)
{
	char szValue[256];
	sprintf_s(szValue, "%.2f", distance);

	WritePrivateProfileString("MQ2Camera", "MaxDistance", szValue, INIFileName);
}

float LoadCameraDistance()
{
	char szValue[256] = { 0 };

	GetPrivateProfileString("MQ2Camera", "MaxDistance", "", szValue, 256, INIFileName);

	float fValue = 0.0;
	sscanf_s(szValue, "%f", &fValue);

	return fValue;
}

bool s_attachedCamera = false;

void AttachCameraToSpawn(PSPAWNINFO pSpawn = NULL)
{
	bool attaching = true;

	if (!pSpawn)
	{
		// if no spawn is provided, reset to the current player
		pSpawn = GetCharInfo() ? GetCharInfo()->pSpawn : NULL;
		attaching = false;
	}

	if (!pSpawn)
		return;

	// we need to get the actorinterface for the spawn. This is the pactorex
	// field of ActorClient
	struct T3D_tagACTORINSTANCE * actor = (struct T3D_tagACTORINSTANCE *)pSpawn->mActorClient.pcactorex;

	if (actor)
	{
		pDisplay->SetViewActor(actor);

		s_attachedCamera = attaching;

		if (attaching)
		{
			WriteChatf("%s:: Attaching camera to \ay%s", PLUGIN_MSG, pSpawn->DisplayedName);
		}
	}
}

VOID Cmd_Camera(PSPAWNINFO pChar, PCHAR szLine)
{
	if (!s_initialized)
	{
		WriteChatf("%s:: Plugin is disabled due to initialization error.", PLUGIN_MSG);
		return;
	}

	CHAR Command[MAX_STRING];
	GetArg(Command, szLine, 1);

	CHAR Param[MAX_STRING];
	GetArg(Param, szLine, 2);

	if (!_stricmp(Command, "distance"))
	{
		bool reset = !_stricmp(Command, "reset");
		if (reset)
			ResetCameraDistance();
		else
			SetCameraDistance(static_cast<float>(atof(Param)));

		// check to see if we want to save
		GetArg(Command, szLine, 3);
		if (!_stricmp(Command, "save"))
		{
			SaveCameraDistance(reset ? 0.0f : static_cast<float>(atof(Param)));
		}
	}
	else if (!_stricmp(Command, "info"))
	{
		WriteChatf("%s:: Zoom camera max distance: \ay%.2f\ax (default: %.2f)", PLUGIN_MSG,	*ZoomCameraMaxDistance, s_origZoomCameraMaxDistance);
		WriteChatf("%s:: User camera max distance: \ay%.2f\ax (default: %.2f)", PLUGIN_MSG,	*UserCameraMaxDistance, s_origUserCameraMaxDistance);
	}
	else if (!_stricmp(Command, "attach"))
	{
		// /camera attach target
		if (!_stricmp(Param, "target"))
		{
			if (pTarget)
				AttachCameraToSpawn((PSPAWNINFO)pTarget);
			else
				WriteChatf("%s:: \arNo target to attach camera to", PLUGIN_MSG);
		}
		// /camera attach id #
		else if (!_stricmp(Param, "id"))
		{
			CHAR Id[MAX_STRING];
			GetArg(Id, szLine, 3);

			PSPAWNINFO pSpawn = (PSPAWNINFO)GetSpawnByID(atoi(Id));
			if (pSpawn)
				AttachCameraToSpawn(pSpawn);
			else
				WriteChatf("%s:: \arCould not find spawn id '%s'", PLUGIN_MSG, Id);
		}
		// /camera attach name_of_some_spawn
		else if (strlen(Param) > 0)
		{
			// This behavior simulates SetViewActorByName
			PSPAWNINFO pSpawn = (PSPAWNINFO)GetSpawnByName(Param);
			if (!pSpawn)
				pSpawn = (PSPAWNINFO)GetSpawnByPartialName(Param);
			if (pSpawn)
				AttachCameraToSpawn(pSpawn);
			else
				WriteChatf("%s:: \arCould not find spawn named '%s'", PLUGIN_MSG, Param);
		}
		else
		{
			WriteChatf("%s:: Resetting camera", PLUGIN_MSG);
			AttachCameraToSpawn();
		}
	}
	else if (!_stricmp(Command, "detach") || !_stricmp(Command, "reset"))
	{
		// reset the camera
		WriteChatf("%s:: Resetting camera", PLUGIN_MSG);
		AttachCameraToSpawn();
	}
	else
	{
		WriteChatf("%s:: Usage:", PLUGIN_MSG);
		WriteChatf("%s:: \ag/camera distance [ reset | <distance> ] [ save ]\ax - set camera max distance or reset to default. Specify 'save' to save the value", PLUGIN_MSG);
		WriteChatf("%s:: \ag/camera info\ax - report current camera information", PLUGIN_MSG);
		WriteChatf("%s:: \ag/camera attach [ target | id # | <spawn name> ]\ax - attach camera to another spawn", PLUGIN_MSG);
		WriteChatf("%s:: \ag/camera detach\ax - Reset camera attachment", PLUGIN_MSG);
	}
}

PLUGIN_API VOID InitializePlugin(VOID)
{
	WriteChatf("%s:: v2.00 by brainiac (\aohttps://github.com/brainiac/MQ2Camera\ax) updated by SwiftyMUSE", PLUGIN_MSG);

	if (!InitValues()) {
		WriteChatf("%s:: \arFailed to initialize offsets. Plugin will not function.", PLUGIN_MSG);
		EzCommand("/timed 1 /plugin mq2camera unload");
	}
	else {
		DebugSpewAlways("%s::gfZoomCameraMaxDistance = 0x%x, 0x%x", PLUGIN_NAME, ZoomCameraMaxDistance, FixOffset(__gfMaxZoomCameraDistance_x));
		DebugSpewAlways("%s::gfUserCameraMaxDistance = 0x%x, 0x%x", PLUGIN_NAME, UserCameraMaxDistance, FixOffset(__gfMaxCameraDistance_x));

		AddCommand("/camera", Cmd_Camera);
		AddDetour((DWORD)ZoomCameraMaxDistance);
		AddDetour((DWORD)UserCameraMaxDistance);
		s_initialized = true;

		WriteChatf("%s:: Type \ag/camera\ax for more information", PLUGIN_MSG);

		float distance = LoadCameraDistance();
		if (distance > 1.0)
			SetCameraDistance(distance);
	}
}

PLUGIN_API VOID ShutdownPlugin(VOID)
{
	if (s_initialized) {
		RemoveCommand("/camera");
		ResetCameraDistance();
		RemoveDetour((DWORD)ZoomCameraMaxDistance);
		RemoveDetour((DWORD)UserCameraMaxDistance);
	}
}