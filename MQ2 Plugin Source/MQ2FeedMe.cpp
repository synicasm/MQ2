

//****************************************************************//
// MQ2FeedMe.cpp
// based on snippet FeedMe 2.3 from A_Druid_00
// by s0rCieR 2005.12.10
//****************************************************************//
// 2010 pms - updates to work with HoT
// 2011 pms - updates to add announce and list
// 3.00 edited by: woobs 01/19/2014 
//    Removed MQ2BagWindow dependency.
//    Removed the swapping of Food/Drink to consume.
//    Restructured to scan food/drink list, then check inventory
//    for match (was the reverse).
//    Add Food/Drink Warning toggles/displays.
// 3.01 edited by: MacQ 02/28/2014
//    Incorporated IsCasting(), AbilityInUse(), and CursorHasItem()
//    from moveitem.h.  Now moveitem.h no longer required.
// 4.0 edited by: Eqmule 07/26/2016 to add string safety
//****************************************************************//
// It will eat food and drink from lists you specify if your hunger
// or thirst levels fall below the thresholds you specifiy.  These 
// lists/thresholds are set in the ini (MQ2FeedMe.ini).
// Threshold level is checked every 15 seconds or so (SKIP_PULSES).
//
// Lots of code stolen, credits goes to the author of those.
//****************************************************************//
// Usage: /autodrink       -> Force manual drinking
//        /autodrink 0     -> Turn off autodrinking
//        /autodrink 3500  -> Set Level where plugin should drink
//        /autodrink warn  -> Toggle Drink warnings on/off
//        /autodrink list  -> Display current drink list and levels
//
//        /autofeed        -> Force manual feeding
//        /autofeed 0      -> Turn off autofeeding
//        /autofeed 3500   -> Set Level where plugin should eat
//        /autofeed warn   -> Togle Food warnings on/off
//        /autofeed list   -> Display current food list and levels
//****************************************************************//

#include "../MQ2Plugin.h"
#include <list>
#include <string>
using namespace std;
PreSetup("MQ2FeedMe");
PLUGIN_VERSION(4.0);

#define	SKIP_PULSES	500
#define	NOID -1

bool         Loaded=0;                           // List Loaded?
long         Pulses=0;                           // Pulses Skipped Counter

long         FeedAt=0;                           // Feed Level
long         DrnkAt=0;                           // Drink Level

char         FindName[ITEM_NAME_LEN];            // Find Food/Drink Name
char         Buffer[16]={0};
list<string> Hunger;                             // Hunger Fix List
list<string> Thirst;                             // Thirst Fix List

int          bAnnLevels = 1;                     // Announce Levels
int          bFoodWarn = 0;                      // Announce No Food
int          bDrinkWarn = 0;                     // Announce No Drink

const char* PLUGIN_NAME = "MQ2FeedMe";

BOOL WindowOpen(PCHAR WindowName)
{
  PCSIDLWND pWnd=(PCSIDLWND)FindMQ2Window(WindowName);
  return (!pWnd)?false:(BOOL)pWnd->dShow;
}

BOOL IsCasting()
{
    return (pCharSpawn && ((PSPAWNINFO)pCharSpawn)->CastingData.SpellID != NOID);
}

BOOL AbilityInUse()
{
    if (pCharSpawn && ((PSPAWNINFO)pCharSpawn)->CastingData.SpellETA == 0) return false;
    return true;
}

BOOL CursorHasItem()
{
    if (GetCharInfo2()->pInventoryArray->Inventory.Cursor) return true;
    return false;
}

void ReadList(list<string> *MyList, PCHAR fSec)
{
  char Buffer[MAX_STRING*10];
  MyList->clear();
  if(GetPrivateProfileString(fSec,NULL,"",Buffer,MAX_STRING*10,INIFileName)) {
    char  szTemp[MAX_STRING];
    PCHAR pBuffer=Buffer;
    while (pBuffer[0]!=0) {
      GetPrivateProfileString(fSec,pBuffer,"",szTemp,MAX_STRING,INIFileName);
      if(szTemp[0]!=0) MyList->push_back(string(szTemp));
      pBuffer+=strlen(pBuffer)+1;
    }
  }
}

bool GoodToFeed()
{
  if(MQ2Globals::gGameState==GAMESTATE_INGAME)   // currently ingame
  if(GetCharInfo2())                             // get charinfo
  if(!CursorHasItem())                           // nothing on cursor
  if(!IsCasting())                               // not casting
  if(!AbilityInUse())                            // not using abilities
  if(!WindowOpen("SpellBookWnd"))                // not medding a spell
  if(!WindowOpen("MerchantWnd"))                 // not interacting with vendor
  if(!WindowOpen("TradeWnd"))                    // not trading with someone
  if(!WindowOpen("BigBankWnd"))                  // not banking
  if(!WindowOpen("BankWnd"))                     // not banking
  if(!WindowOpen("LootWnd"))                     // not looting
    return true;                                 // then return true
  return false;                                  // otherwise false
}

void ListTypes(list<string> fTempList)
{
    list<string>::iterator pTempList;
    int i = 1;
    pTempList = fTempList.begin();
    while (pTempList != fTempList.end()) {
        WriteChatf("\ag - %d. \aw%s", i, pTempList->c_str());
        i++;
        pTempList++;
    }
}

void Execute(PCHAR zFormat, ...)
{
  char zOutput[MAX_STRING]={0}; va_list vaList; va_start(vaList,zFormat);
  vsprintf_s(zOutput,zFormat,vaList); if(!zOutput[0]) return;
  DoCommand(GetCharInfo()->pSpawn,zOutput);
}

void Consume(BYTE fTYPE, list<string> fLIST)
{
    list<string>::iterator pTempList;
    pTempList = fLIST.begin();
    while (pTempList != fLIST.end()) {
        strcpy_s(FindName, pTempList->c_str());
        if (PCONTENTS pItem = FindItemByName(FindName,true)) {
            if (GetItemFromContents(pItem)->ItemType == fTYPE) {
                WriteChatf("\ay%s\aw:: Consuming -> \ag%s.", PLUGIN_NAME, FindName);
                Execute("/useitem %d %d", pItem->Contents.ItemSlot, pItem->Contents.ItemSlot2);
                return;
	        }
	    }
	    pTempList++;
    }
    if (fTYPE == ITEMITEMTYPE_FOOD) {
        if (bFoodWarn) WriteChatf("\ay%s\aw:: No Food to Consume", PLUGIN_NAME);
    } else {
        if (bDrinkWarn) WriteChatf("\ay%s\aw:: No Drink to Consume", PLUGIN_NAME);
    }
}

void AutoFeedCmd(PSPAWNINFO pLPlayer, char* szLine)
{
     if (szLine[0] != 0) {
        if (!_strnicmp(szLine, "list", 5)) {
            WriteChatf("\ay%s\aw:: Listing Food:", PLUGIN_NAME);
            ListTypes(Hunger);
		    if (bAnnLevels) {
		        sprintf_s(Buffer, "%d", FeedAt);
			    WriteChatf("\ay%s\aw:: AutoFeed(\ag%s\ax).", PLUGIN_NAME, (FeedAt) ? Buffer : "\aroff");
			    WriteChatf("\ay%s\aw:: Current Hunger(\ag%d\ax)", PLUGIN_NAME, GetCharInfo2()->hungerlevel);
		    }
        } else if (!_strnicmp(szLine, "warn", 5)) {
            if (bFoodWarn) {
                bFoodWarn = 0;
                WriteChatf("\ay%s\aw:: Food Warning Off", PLUGIN_NAME);
            } else {
                bFoodWarn = 1;
                WriteChatf("\ay%s\aw:: Food Warning On", PLUGIN_NAME);
            }
        } else {
            FeedAt = atoi(szLine);
            if (FeedAt < 0) FeedAt = 0;
            else if (FeedAt > 5000) FeedAt = 5000;
            sprintf_s(Buffer, "%d", FeedAt);
            WritePrivateProfileString(GetCharInfo()->Name, "AutoFeed", Buffer, INIFileName);
            WriteChatf("\ay%s\aw:: AutoFeed(\ag%s\ax).", PLUGIN_NAME, (FeedAt) ? Buffer : "\aroff");
            if (bAnnLevels) WriteChatf("\ay%s\aw:: Current Thirst(\ag%d\ax) Hunger(\ag%d\ax)", PLUGIN_NAME, GetCharInfo2()->thirstlevel, GetCharInfo2()->hungerlevel);
        }
    }
    else if (GoodToFeed()) Consume(ITEMITEMTYPE_FOOD,Hunger);
}

void AutoDrinkCmd(PSPAWNINFO pLPlayer, char* szLine)
{
    if (szLine[0] != 0) {
        if (!_strnicmp(szLine, "list", 5)) {
            WriteChatf("\ay%s\aw:: Listing Drink:", PLUGIN_NAME);
            ListTypes(Thirst);
            if (bAnnLevels) {
                sprintf_s(Buffer, "%d", DrnkAt);
                WriteChatf("\ay%s\aw:: AutoDrink(\ag%s\ax).", PLUGIN_NAME, (DrnkAt) ? Buffer : "\aroff");
                WriteChatf("\ay%s\aw:: Current Thirst(\ag%d\ax)", PLUGIN_NAME, GetCharInfo2()->thirstlevel);
            }
        } else if (!_strnicmp(szLine, "warn", 5)) {
            if (bDrinkWarn) {
                bDrinkWarn = 0;
                WriteChatf("\ay%s\aw:: Drink Warning Off", PLUGIN_NAME);
            } else {
                bDrinkWarn = 1;
                WriteChatf("\ay%s\aw:: Drink Warning On", PLUGIN_NAME);
            }
        } else {
            DrnkAt = atoi(szLine);
            if (DrnkAt < 0) DrnkAt = 0;
            else if (DrnkAt > 5000) DrnkAt = 5000;
            sprintf_s(Buffer, "%d", DrnkAt);
            WritePrivateProfileString(GetCharInfo()->Name, "AutoDrink", Buffer, INIFileName);
            WriteChatf("\ay%s\aw:: AutoDrink(\ag%s\ax).", PLUGIN_NAME, (DrnkAt) ? Buffer :"\aroff");
            if (bAnnLevels) WriteChatf("\ay%s\aw:: Current Thirst(\ag%d\ax) Hunger(\ag%d\ax)", PLUGIN_NAME, GetCharInfo2()->thirstlevel, GetCharInfo2()->hungerlevel);
        }
    }
    else if (GoodToFeed()) Consume(ITEMITEMTYPE_WATER,Thirst);
}

PLUGIN_API void OnPulse()
{
    if (++Pulses < SKIP_PULSES) return;
    Pulses = 0;
    if (!GoodToFeed()) return;
    if (DrnkAt && (LONG)GetCharInfo2()->thirstlevel < DrnkAt) Consume(ITEMITEMTYPE_WATER,Thirst);
    if (FeedAt && (LONG)GetCharInfo2()->hungerlevel < FeedAt) Consume(ITEMITEMTYPE_FOOD,Hunger);
}
template <unsigned int _Size>LPSTR SafeItoa(int _Value,char(&_Buffer)[_Size], int _Radix)
{
	errno_t err = _itoa_s(_Value, _Buffer, _Radix);
	if (!err) {
		return _Buffer;
	}
	return "";
}
PLUGIN_API void SetGameState(DWORD GameState)
{
    if (GetGameState() == GAMESTATE_INGAME) {
        if (GetCharInfo2()) {
            DrnkAt     = GetPrivateProfileInt(GetCharInfo()->Name, "AutoDrink", 0, INIFileName);
            FeedAt     = GetPrivateProfileInt(GetCharInfo()->Name, "AutoFeed",  0, INIFileName);
            bAnnLevels = GetPrivateProfileInt("Settings",          "Announce",  1, INIFileName);
            bFoodWarn  = GetPrivateProfileInt("Settings",          "FoodWarn",  0, INIFileName);
            bDrinkWarn = GetPrivateProfileInt("Settings",          "DrinkWarn",  0, INIFileName);

            WritePrivateProfileString("Settings", "Announce", SafeItoa(bAnnLevels, Buffer, 10), INIFileName);
            WritePrivateProfileString("Settings", "FoodWarn", SafeItoa(bFoodWarn, Buffer, 10), INIFileName);
            WritePrivateProfileString("Settings", "DrinkWarn", SafeItoa(bDrinkWarn, Buffer, 10), INIFileName);
            if (!Loaded) {
                ReadList(&Hunger,"FOOD");
                ReadList(&Thirst,"DRINK");
                Loaded = true;
            }
        }
    }
}

PLUGIN_API void InitializePlugin()
{
    AddCommand("/autofeed",  AutoFeedCmd);
    AddCommand("/autodrink", AutoDrinkCmd);
}

PLUGIN_API void ShutdownPlugin()
{
    RemoveCommand("/autofeed");
    RemoveCommand("/autodrink");
}