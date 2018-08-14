

#define   PLUGIN_NAME   "MQ2AutoGroup"
#define   PLUGIN_DATE   20160726
#define   PLUGIN_VERS   3.00

#include "../MQ2Plugin.h"
using namespace std;
#include <vector>
PreSetup(PLUGIN_NAME);
PLUGIN_VERSION(PLUGIN_VERS);

vector<string> AutoGroup;
bool bAutoGroup = false;
bool bAutoGroupGuild = false;
bool bAcceptDialog = false;
ULONGLONG ulDialogTimer = 0;

void LoadINI(void)
{
    int KeyIndex;
    char szTemp[MAX_STRING], KeyName[MAX_STRING], KeyValue[MAX_STRING];
    if (GetPrivateProfileString("Settings","AutoGroup",NULL,szTemp,MAX_STRING,INIFileName))
    {
        if (!_stricmp(szTemp,"on"))
        {
            bAutoGroup = true;
        }
        else
        {
            bAutoGroup = false;
        }
    }
    if (GetPrivateProfileString("Settings","AutoGroupGuild",NULL,szTemp,MAX_STRING,INIFileName))
    {
        if (!_stricmp(szTemp,"on"))
        {
            bAutoGroupGuild = true;
        }
        else
        {
            bAutoGroupGuild = false;
        }
    }
    KeyIndex = 1;
    AutoGroup.clear();
    do
    {
        sprintf_s(KeyName, "Name%d", KeyIndex);
        GetPrivateProfileString("Names", KeyName, NULL, KeyValue, MAX_STRING, INIFileName);
        if (strlen(KeyValue)>0)
        {
            _strlwr_s(KeyValue);
            AutoGroup.push_back(KeyValue);
        }
        KeyIndex++;
    }
    while (strlen(KeyValue)>0);
    WriteChatf("\at%s\ax [\agv%1.2f\ax]", PLUGIN_NAME, PLUGIN_VERS);
    WriteChatf("%s::AutoGroup [%s\ax].", PLUGIN_NAME, bAutoGroup?"\agON":"\arOFF");
    WriteChatf("%s::AutoGroup guild [%s\ax].", PLUGIN_NAME, bAutoGroupGuild?"\agON":"\arOFF");
    WriteChatf("%s::Names [\ag%d\ax].", PLUGIN_NAME, AutoGroup.size());
}

void SaveINI(void)
{
    char KeyName[MAX_STRING];
    WritePrivateProfileSection("Settings", "", INIFileName);
    WritePrivateProfileSection("Names", "", INIFileName);
    WritePrivateProfileString("Settings","AutoGroup",bAutoGroup?"on":"off",INIFileName);
    WritePrivateProfileString("Settings","AutoGroupGuild",bAutoGroupGuild?"on":"off",INIFileName);
    for (register unsigned int Index = 0; Index < AutoGroup.size(); Index++)
    {
        string& VectorRef = AutoGroup[Index];
        sprintf_s(KeyName, "Name%d", Index+1);
        WritePrivateProfileString("Names",KeyName,VectorRef.c_str(),INIFileName);
    }
    WriteChatf("%s::\agSettings saved to file\ax.", PLUGIN_NAME);
}

void AutoGroupCommand(PSPAWNINFO pChar, PCHAR szLine)
{
    char szTemp[MAX_STRING], szBuffer[MAX_STRING];
    GetArg(szTemp, szLine, 1);
    // Turn autogroup on
    if (!_stricmp(szTemp, "on"))
    {
        bAutoGroup = true;
        WriteChatf("%s::AutoGroup now [\agON\ax].", PLUGIN_NAME);
    }
    // Turn autogroup off
    else if (!_stricmp(szTemp, "off"))
    {
        bAutoGroup = false;
        WriteChatf("%s::AutoGroup now [\arOFF\ax].", PLUGIN_NAME);
    }
    // Toggle autogroup guild
    else if (!_stricmp(szTemp, "guild"))
    {
        bAutoGroupGuild=!bAutoGroupGuild;
        WriteChatf("%s::AutoGroup guild now [%s\ax]", PLUGIN_NAME, bAutoGroupGuild?"\agON":"\arOFF");
    }
    // Reload .ini
    else if (!_stricmp(szTemp, "load"))
    {
        LoadINI();
    }
    // Save current settings & names
    else if (!_stricmp(szTemp, "save"))
    {
        SaveINI();
    }
    // Add name
    else if (!_stricmp(szTemp, "add"))
    {
        GetArg(szBuffer, szLine, 2);
        if (strlen(szBuffer)>0)
        {
            AutoGroup.push_back(szBuffer);
            WriteChatf("%s::Added [\ag%s\ax].", PLUGIN_NAME, szBuffer);
        }
        else
        {
            WriteChatf("%s::\arName not specified\ax.", PLUGIN_NAME);
        }
    }
    // Delete name #
    else if (!_stricmp(szTemp, "del"))
    {
        GetArg(szBuffer, szLine, 2);
        unsigned int Index = atoi(szBuffer);
        if (Index > 0 && Index < AutoGroup.size()+1)
        {
            string& VectorRef = AutoGroup[Index-1];
            WriteChatf("%s::Deleted [\ar%s\ax].", PLUGIN_NAME, VectorRef.c_str());
            AutoGroup.erase(AutoGroup.begin() + (Index-1));
        }
        else
        {
            WriteChatf("%s::\arName %d does not exist\ax.", PLUGIN_NAME, Index);
        }
    }
    // Clear all names
    else if (!_stricmp(szTemp, "clear"))
    {
        AutoGroup.clear();
        WriteChatf("%s::\ayNames cleared\ax.", PLUGIN_NAME);
    }
    // List all names
    else if (!_stricmp(szTemp, "list"))
    {
        WriteChatf("%s::Names loaded: \ag%d\at.\n", PLUGIN_NAME, AutoGroup.size());
        for (register unsigned int Index = 0; Index < AutoGroup.size(); Index++)
        {
            string& VectorRef = AutoGroup[Index];
            WriteChatf("\at%d\ax:\ay %s", Index+1, VectorRef.c_str());
        }
    }
    // No parameter is status
    else if (!strlen(szTemp))
    {
        WriteChatf("%s::AutoGroup [%s\ax].", PLUGIN_NAME, bAutoGroup?"\agON":"\arOFF");
        WriteChatf("%s::AutoGroup guild [%s\ax].", PLUGIN_NAME, bAutoGroupGuild?"\agON":"\arOFF");
        WriteChatf("%s::Names loaded [\ag%d\ax].", PLUGIN_NAME, AutoGroup.size());
        WriteChatf("%s::\ayHelp: \ax/autogroup help", PLUGIN_NAME);
    }
    // Otherwise, show help
    else
    {
        WriteChatf("\agAutomatically accepts group/raid invite from specified chars, or guild.");
        WriteChatf("\agNames are NOT case sensitive.");
        WriteChatf("\agUsing \ay/autogroup \agwith no options will show current status.");
        WriteChatf("\at/autogroup on \ay(Turns automatic group/raid invite accept from specified chars on)\ax.");
        WriteChatf("\at/autogroup off \ay(Turns automatic group/raid invite accept from specified chars off)\ax.");
        WriteChatf("\at/autogroup guild \ay(Toggles automatic group/raid invite accept from your guild members)\ax.");
        WriteChatf("\at/autogroup load \ay(Loads options and names from file)\ax.");
        WriteChatf("\ay(any current unsaved names will be lost!)\ax.");
        WriteChatf("\at/autogroup save \ay(Updates file to match current settings and names)\ax.");
        WriteChatf("\at/autogroup add \ay(Add a new char name. ex: \ar/autogroup add bubbawar\ay)\ax.");
        WriteChatf("\at/autogroup del \ay(Delete a char name. ex: \ar/autogroup del 15\ay)\ax.");
        WriteChatf("\at/autogroup clear \ay(Clears all char names)\ax.");
        WriteChatf("\at/autogroup list \ay(Lists current char names)\ax.");
    }
}

BOOL CheckNames(PCHAR szName, bool &isGuildMember)
{
    CHAR szTemp[MAX_STRING];
    if (gGameState==GAMESTATE_INGAME)
    {
        if (pGuild && bAutoGroupGuild)
        {
            if (pGuild->FindMemberByName(szName))
            {
                isGuildMember = true;
                return true;
            }
        }
        if (pCharData && bAutoGroup)
        {
            strcpy_s(szTemp, szName);
            _strlwr_s(szTemp);
            for (register unsigned int Index = 0; Index < AutoGroup.size(); Index++)
            {
                string& VectorRef = AutoGroup[Index];
                if (!_stricmp(szTemp, VectorRef.c_str())) return true;
            }
        }
    }
    return false;
}

void CheckDialog(void)
{
    CXWnd *Child;
    CXWnd *pWnd;
    char InputCXStr[512];
    pWnd=(CXWnd *)FindMQ2Window("ConfirmationDialogBox");
    if (pWnd)
    {
        if (((PCSIDLWND)(pWnd))->dShow==0) return;
        Child = pWnd->GetChildItem("cd_textoutput");
        if (Child)
        {
            ZeroMemory(InputCXStr,sizeof(InputCXStr));
            GetCXStr(((PCSIDLWND)Child)->SidlText,InputCXStr,sizeof(InputCXStr));
            if (strstr(InputCXStr,"invite?") || strstr(InputCXStr,"invites"))
            {
                bAcceptDialog = true;
                return;
            }
        }
    }
}

PLUGIN_API DWORD OnIncomingChat(PCHAR Line, DWORD Color)
{
    CHAR szName[MAX_STRING];
    bool isGuildMember = false;
    if (strstr(Line,"invites you to join a group."))
    {
        GetArg(szName,Line,1);
        if (CheckNames(szName, isGuildMember))
        {
            WriteChatf("%s::Accepting group invite [\ay%s\ax]%s.", PLUGIN_NAME, szName, isGuildMember?"\ax[\atGuild\ax]":"");
            DoCommand(GetCharInfo()->pSpawn,"/keypress invite_follow");
            ulDialogTimer = GetTickCount642() + 3000;
        }
    } else if (strstr(Line,"invites you to join a raid"))
    {
        GetArg(szName,Line,1);
        if (CheckNames(szName, isGuildMember))
        {
            WriteChatf("%s::Accepting raid invite [\ay%s\ax]%s.", PLUGIN_NAME, szName, isGuildMember?"\ax[\atGuild\ax]":"");
            DoCommand(GetCharInfo()->pSpawn,"/raidaccept");
            ulDialogTimer = GetTickCount642() + 3000;
        }
    }
    return 0;
}

PLUGIN_API VOID OnPulse()
{
    if (GetTickCount642()>ulDialogTimer) CheckDialog();
    if (bAcceptDialog)
    {
        DoCommand(GetCharInfo()->pSpawn,"/notify ConfirmationDialogBox Yes_Button leftmouseup");
        bAcceptDialog = false;
    }
}

PLUGIN_API VOID InitializePlugin(VOID)
{
    DebugSpewAlways("Initializing MQ2AutoGroup");
    LoadINI();
    AddCommand("/autogroup", AutoGroupCommand);
}

PLUGIN_API VOID ShutdownPlugin(VOID)
{
    DebugSpewAlways("Shutting down MQ2AutoGroup");
    RemoveCommand("/autogroup");
}