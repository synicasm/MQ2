

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// Projet: MQ2Cast.cpp    | Set DEBUGGING to true for DEBUGGING msg
// Author: s0rCieR        | 
//         A_Enchanter_00 |
//         htw            |
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// Last edited by: devestator 10/30/2010 Updated for HoT & MQ2BagWindow
// Last edited by: Maskoi  4/15/2011 ->item changes
// 10.0 edited by: EqMule  12/14/2013 Removed MQ2BagWindow dependecy - Sponsored by RedGuides.com
// Since a recent patch, I think that there are very few items that require a swap/must equip to be cast.
// Therefore, this version does NOT swap stuff in and out of bags and/or use bandolier anymore.
// If you need that in your macro, use mq2exchange or /itemnotify to move/swap things. (or a version prior to 9.11)
// I also, made this plugin debugable again.
// Please future editors, keep the {} in the if's and while's
// Cause debugging without them is a total pain. (cant set bps if the return is on same line for example)
// 10.02 edited by: three-p-o 1/12/2014 switched over item casting to use EQ's own /cast command.
//    Fixed issue in CastHandle with {} not matching up properly. Resolves issue with cast returning before casting is completed.
// Also added in my changes to return CAST_UNKNOWN if you are trying to cast or memorize a spell that is not in your book.
// 10.03 edited by: three-p-o 3/23/2014  Updated for MQ2-20140322
// 10.04 edited by: trev 3/28/2014  Fixed: spells not getting memmed
// 10.05 edited by: eqmule 2/17/2016  new spell system
// 11.0 - Eqmule 07-22-2016 - Added string safety.
// 11.1 - Eqmule 08-22-2017 - Dont check Twisting if Mq2Twist is NOT loaded and some other tweaks to improve performance.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

bool DEBUGGING = false;

#ifndef PLUGIN_API
#include "../MQ2Plugin.h"
#include "../Blech/Blech.h"
PreSetup("MQ2Cast");
PLUGIN_VERSION(11.1);
#endif

#define         DELAY_CAST    12000
#define         DELAY_STOP     4000
#define         DELAY_PULSE     125

#define         CAST_SUCCESS      0
#define         CAST_INTERRUPTED  1
#define         CAST_RESIST       2
#define         CAST_COLLAPSE     3
#define         CAST_RECOVER      4
#define         CAST_FIZZLE       5
#define         CAST_STANDING     6
#define         CAST_STUNNED      7
#define         CAST_INVISIBLE    8
#define         CAST_NOTREADY     9
#define         CAST_OUTOFMANA   10
#define         CAST_OUTOFRANGE  11
#define         CAST_NOTARGET    12
#define         CAST_CANNOTSEE   13
#define         CAST_COMPONENTS  14
#define         CAST_OUTDOORS    15
#define         CAST_TAKEHOLD    16
#define         CAST_IMMUNE      17
#define         CAST_DISTRACTED  18
#define         CAST_ABORTED     19
#define         CAST_UNKNOWN     20

#define         FLAG_COMPLETE     0 
#define         FLAG_REQUEST     -1
#define         FLAG_PROGRESS1   -2 
#define         FLAG_PROGRESS2   -3 
#define         FLAG_PROGRESS3   -4 
#define         FLAG_PROGRESS4   -5 

#define         DONE_COMPLETE    -3
#define         DONE_ABORTED     -2 
#define         DONE_PROGRESS    -1 
#define         DONE_SUCCESS      0

#define         TYPE_SPELL        1
#define         TYPE_ALT          2
#define         TYPE_ITEM         3

#define         RECAST_DEAD       2
#define         RECAST_LAND       1
#define         RECAST_ZERO       0

#define         NOID             -1

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
long         DELAY_MEMO = 10000;

bool         BardBeta = true;
bool         Immobile = false;        // Immobile?
bool         Invisible = false;        // Invisibility Check?
bool         Twisting = false;        // Twisting?
bool         Casting = false;        // Casting Window was opened?
long         Resultat = CAST_SUCCESS; // Resultat
ULONGLONG    ImmobileT = 0;             // Estimate when it be immobilized!

long         CastingD = NOID;           // Casting Spell Detected
long         CastingC = NOID;           // Casting Current ID
long         CastingE = CAST_SUCCESS;   // Casting Current Result
long         CastingL = NOID;           // Casting LastOne ID
long         CastingX = CAST_SUCCESS;   // Casting LastOne Result
ULONGLONG    CastingT = 0;              // Casting Timeout
long         CastingO = NOID;           // Casting OnTarget
ULONGLONG    CastingP = 0;              // Casting Pulse

long         TargI = 0;                 // Target ID
long         TargC = 0;                 // Target Current

long         StopF = FLAG_COMPLETE;     // Stop Event Flag Progress? 
long         StopE = DONE_SUCCESS;      // Stop Event Exit Value 
ULONGLONG    StopM = 0;                 // Stop Event Mark 

long         MoveA = FLAG_COMPLETE;     // Move Event AdvPath? 
long         MoveS = FLAG_COMPLETE;     // Move Event Stick? 
long         MoveF = FLAG_COMPLETE;     // Move Event MQ2AdvPath Following?
long         MoveP = FLAG_COMPLETE;     // Move Event MQ2AdvPath Pathing?

long         MemoF = FLAG_COMPLETE;     // Memo Event Flag 
long         MemoE = DONE_SUCCESS;      // Memo Event Exit 
ULONGLONG    MemoM = 0;                 // Memo Event Mark 

long         ItemF = FLAG_COMPLETE;     // Item Flag
long         ItemA[NUM_INV_SLOTS];         // Item Arrays

long         DuckF = FLAG_COMPLETE;     // Duck Flag
ULONGLONG    DuckM = 0;                 // Duck Time Stamp

long         CastF = FLAG_COMPLETE;     // Cast Flag
long         CastE = CAST_SUCCESS;      // Cast Exit Return value
long         CastG = NOID;              // Cast Gem ID
void         *CastI = NULL;              // Cast ID   [spell/alt/item]
long         CastK = NOID;              // Cast Kind [spell/alt/item]
long         CastT = 0;                 // Cast Time [spell/alt/item]
ULONGLONG    CastM = 0;                 // Cast TimeMark Start Casting
long         CastR = 0;                 // Cast Retry Counter
long         CastW = 0;                 // Cast Retry Type
char         CastB[MAX_STRING];       // Cast Bandolier In
char         CastC[MAX_STRING];       // Cast SpellType
char         CastN[MAX_STRING];       // Cast SpellName
PSPELL         CastS = NULL;              // Cast Spell Pointer

char         zOutputDly[MAX_STRING] = { 0 };    //Delayed command for item casting
const unsigned long EX_DELAY = 125;        //The amount of time to delay for the item casting
ULONGLONG   ulTimer = 0;        //Delay timer
unsigned long   ulTimerR = 0;        //Delay timer 2
bool         cPendingEq = false;    //Pending equipment cast

bool         Parsed = false;            // BTree List Found Flags
Blech         LIST013('#');            // BTree List for OnChat Message on Color  13
Blech         LIST264('#');            // BTree List for OnChat Message on Color 264
Blech         LIST289('#');            // BTree List for OnChat Message on Color 289
Blech         UNKNOWN('#');            // BTree List for OnChat Message on UNKNOWN Yet Color
Blech         SUCCESS('#');            // BTree List for OnChat Message on SUCCESS Detection

PCONTENTS      fPACK = 0;                 // ItemFound/ItemSearch - Find Pack Contents
PCONTENTS      fITEM = 0;                 // ItemFound/ItemSearch - Find Item Contents
long         fSLOT = 0;                 // ItemFound/ItemSearch - Find Item SlotID
long         SwapSlot = 0;                // Item Swapped to slot#
int            PulseCount = 0;

PSPELL        fFIND;                   // SpellFind - Casting Spell Effect
void         *fINFO;                   // SpellFind - Casting Type Structure
int           fTYPE;                   // SpellFind - Casting Type
int           fTIME;                   // SpellFind - Casting Time
PCHAR         fNAME;                   // SpellFind - Casting Name

SPELLFAVORITE SpellToMemorize;         // Favorite Spells Array
long          SpellTotal;              // Favorite Spells Total
void WinClick(CXWnd *Wnd, PCHAR ScreenID, PCHAR ClickNotification, DWORD KeyState);
void ClickBack();

PCHAR         ListGems[] = { "1","2","3","4","5","6","7","8","9","A","B","C","D","E","F" };

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

#define aCastEvent(List,Value,Filter) List.AddEvent(Filter,CastEvent,(void*)Value);

void __stdcall CastEvent(unsigned int ID, void *pData, PBLECHVALUE pValues)
{
	Parsed = true;
	if (CastingE<(long)pData) {
		CastingE = (long)pData;
	}
	if (DEBUGGING) {
		WriteChatf("[%I64u] MQ2Cast:[OnChat]: Result=[%d] Called=[%d].", GetTickCount642(), CastingE, (long)pData);
	}
}

PALTABILITY AltAbility(PCHAR ID)
{
	if (ID[0]) {
		int level = -1;
		if (PSPAWNINFO pMe = (PSPAWNINFO)pLocalPlayer) {
			level = pMe->Level;
		}
		int Number = IsNumber(ID);
		int Values = atoi(ID);
		for (DWORD nAbility = 0; nAbility < AA_CHAR_MAX_REAL; nAbility++) {
			if (GetCharInfo2()->AAList[nAbility].AAIndex) {
				if (Number) {
					if (PALTABILITY pAbility = GetAAByIdWrapper(GetCharInfo2()->AAList[nAbility].AAIndex)) {
						if (pAbility->ID == Values) {
							return pAbility;
						}
					}
				} else {
					if (PALTABILITY pAbility = GetAAByIdWrapper(GetCharInfo2()->AAList[nAbility].AAIndex,level)) {
						if (PCHAR pName = pCDBStr->GetString(pAbility->nName, 1, NULL)) {
							if (!_stricmp(ID, pName)) {
								return pAbility;
							}
						}
					}
				}
			}
		}
	}
	return NULL;
}

bool BardClass()
{
	if (PCHARINFO pChar = GetCharInfo()) {
		if (pChar->pSpawn) {
			return (strncmp(pEverQuest->GetClassDesc(pChar->pSpawn->mActorClient.Class & 0xFF), "Bard", 5)) ? false : true;
		}
	}
	return false;
}

void Cast(PCHAR zFormat, ...)
{
	char zOutput[MAX_STRING] = { 0 }; va_list vaList; va_start(vaList, zFormat);
	vsprintf_s(zOutput, zFormat, vaList);
	if (!zOutput[0]) {
		return;
	}
	//  WriteChatf("Cast = %s : zOutput %s  Line 222 - Cast()",GetCharInfo()->pSpawn,zOutput);
	Cast(GetCharInfo()->pSpawn, zOutput);
}

long CastingLeft()
{
	long CL = 0;
	if (pCastingWnd && (PCSIDLWND)pCastingWnd->dShow) {
		CL = GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp;
		if (CL<1) {
			CL = 1;
		}
	}
	return CL;
}
long Evaluate(PCHAR zFormat, ...)
{
	char zOutput[MAX_STRING] = { 0 }; va_list vaList; va_start(vaList, zFormat);
	vsprintf_s(zOutput, zFormat, vaList);
	if (!zOutput[0]) {
		return 1;
	}
	ParseMacroData(zOutput, sizeof(zOutput));
	return atoi(zOutput);
}

void CastTimer(int TargetID, int SpellID, long TickE)
{
	typedef VOID(__cdecl *CastTimerCALL) (int, int, long);
	PMQPLUGIN pLook = pPlugins;
	while (pLook && _strnicmp(pLook->szFilename, "MQ2Casttimer", 11)) {
		pLook = pLook->pNext;
	}
	if (pLook && pLook->fpVersion>1.1100) {
		if (CastTimerCALL Request = (CastTimerCALL)GetProcAddress(pLook->hModule, "TimerCastHandle")) {
			Request(TargetID, SpellID, TickE);
		}
	}
}

void Execute(PCHAR zFormat, ...)
{
	char zOutput[MAX_STRING] = { 0 }; va_list vaList; va_start(vaList, zFormat);
	vsprintf_s(zOutput, zFormat, vaList);
	if (!zOutput[0]) {
		return;
	}
	DoCommand(GetCharInfo()->pSpawn, zOutput);
}

void ExecuteDly(PCHAR zFormat, ...)
{
	//char zOutput[MAX_STRING]={0}; 
	if (cPendingEq) {
		MacroError("MQ2Cast: There is already an item pending to cast.");
		return;
	}
	va_list vaList; va_start(vaList, zFormat);
	vsprintf_s(zOutputDly, zFormat, vaList);
	if (!zOutputDly[0]) {
		return;
	}
	ulTimer = GetTickCount642() + EX_DELAY;
	cPendingEq = true;
	//DoCommand(GetCharInfo()->pSpawn,zOutput);
}

bool Flags()
{
	if (!BardClass() && pCastingWnd && (PCSIDLWND)pCastingWnd->dShow) {
		if (DEBUGGING) {
			WriteChatf("MQ2Cast: pCastingWnd=TRUE");
		}
		return true;
	}
	if (CastF != FLAG_COMPLETE) { if (DEBUGGING) WriteChatf("MQ2Cast: CastF!=FLAG_COMPLETE"); return true; }
	if (DuckF != FLAG_COMPLETE) { if (DEBUGGING) WriteChatf("MQ2Cast: DuckF!=FLAG_COMPLETE"); return true; }
	if (ItemF != FLAG_COMPLETE) { if (DEBUGGING) WriteChatf("MQ2Cast: ItemF!=FLAG_COMPLETE"); return true; }
	if (MemoF != FLAG_COMPLETE) { if (DEBUGGING) WriteChatf("MQ2Cast: MemoF!=FLAG_COMPLETE"); return true; }
	if (StopF != FLAG_COMPLETE) { if (DEBUGGING) WriteChatf("MQ2Cast: StopF!=FLAG_COMPLETE"); return true; }
	if (MoveS != FLAG_COMPLETE) { if (DEBUGGING) WriteChatf("MQ2Cast: MoveS!=FLAG_COMPLETE"); return true; }
	if (MoveF != FLAG_COMPLETE) { if (DEBUGGING) WriteChatf("MQ2Cast: MoveF!=FLAG_COMPLETE"); return true; }
	if (MoveP != FLAG_COMPLETE) { if (DEBUGGING) WriteChatf("MQ2Cast: MoveP!=FLAG_COMPLETE"); return true; }
	if (MoveA != FLAG_COMPLETE) { if (DEBUGGING) WriteChatf("MQ2Cast: MoveA!=FLAG_COMPLETE"); return true; }
	return false;
}

long GEMID(LONG ID)
{
	for (int GEM = 0; GEM < NUM_SPELL_GEMS; GEM++) {
		if (GetMemorizedSpell(GEM) == ID) {
			return GEM;
		}
	}
	return NOID;
}

bool GEMReady(LONG ID)
{
	if (GetSpellByID(GetMemorizedSpell(ID))) {
		if (((PCDISPLAY)pDisplay)->TimeStamp >((PSPAWNINFO)pLocalPlayer)->SpellGemETA[ID] && ((PCDISPLAY)pDisplay)->TimeStamp > ((PSPAWNINFO)pLocalPlayer)->SpellCooldownETA) {
			return true;
		}
	}
	return false;
}

bool GiftOfMana()
{
	for (unsigned long nBuff = 0; nBuff < 35; nBuff++) {
		if (PSPELL pSpell = GetSpellByID(GetCharInfo2()->ShortBuff[nBuff].SpellID)) {
			if (!_stricmp("Gift of Mana", pSpell->Name)) {
				return true;
			}
		}
	}
	return false;
}

void MemoLoad(long Gem, PSPELL Spell)
{
	if (!Spell || Spell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class]>GetCharInfo()->pSpawn->Level) {
		return;
	}
	for (int sp = 0; sp<NUM_SPELL_GEMS; sp++) {
		if (SpellToMemorize.SpellId[sp] == Spell->ID) {
			SpellToMemorize.SpellId[sp] = 0xFFFFFFFF;
		}
	}
	SpellToMemorize.SpellId[((DWORD)Gem<NUM_SPELL_GEMS) ? Gem : 4] = Spell->ID;
}

float Speed()
{
	float MySpeed = 0.0f;
	if (PSPAWNINFO Self = GetCharInfo()->pSpawn) {
		MySpeed = Self->SpeedRun;
		if (PSPAWNINFO Mount = FindMount(Self)) {
			MySpeed = Mount->SpeedRun;
		}
	}
	return MySpeed;
}

void Success(PSPELL Cast)
{
	SUCCESS.Reset();
	// WriteChatf("Success(Cast) = %d",Cast);
	if (Cast) {
		char Temps[MAX_STRING];
		bool Added = false;
		/*CastByMe,CastByOther,CastOnYou,CastOnAnother,WearOff*/
		if (char*str = GetSpellString(Cast->ID,2)) { 
			sprintf_s(Temps, "%s#*#", str);
			aCastEvent(SUCCESS, CAST_SUCCESS, Temps);
			Added = true;
		}
		if (char*str = GetSpellString(Cast->ID,3)) { 
			sprintf_s(Temps, "#*#%s#*#", str);
			aCastEvent(SUCCESS, CAST_SUCCESS, Temps);
			Added = true;
		}
		//if(BardClass() && GetCharInfo()->pSpawn->CastingData.SpellID) {
		//   Added=true;
		//}
		if (!Added) {
			aCastEvent(SUCCESS, CAST_SUCCESS, "You begin casting#*#");
		}
	}
}

bool Moving()
{
	ULONGLONG MyTimer = GetTickCount642();
	if (Speed() != 0.0f) {
		ImmobileT = MyTimer + 500;
	}
	return (!MQ2Globals::gbMoving && (!ImmobileT || MyTimer>ImmobileT));
}

CXWnd*TributeMasterWnd = 0;
CXWnd*GuildBankWnd = 0;

BOOL Paused()
{
	if (BardClass()) {
		return false;
	}
	if (pLootWnd && (PCSIDLWND)pLootWnd->dShow) {
		return true;
	}
	if (pBankWnd && (PCSIDLWND)pBankWnd->dShow) {
		return true;
	}
	if (pMerchantWnd && (PCSIDLWND)pMerchantWnd->dShow) {
		return true;
	}
	if (pTradeWnd && (PCSIDLWND)pTradeWnd->dShow) {
		return true;
	}
	if (pGiveWnd && (PCSIDLWND)pGiveWnd->dShow) {
		return true;
	}
	//calling Open is super cpu expensive
	//if (Open("TributeMasterWnd")) {
	//I needed to change this cause it slows us down A LOT!
	if (!TributeMasterWnd) {
		TributeMasterWnd = FindMQ2Window("TributeMasterWnd");
	} else {
		if(TributeMasterWnd->dShow)
			return true;
	}
	if (!GuildBankWnd) {
		GuildBankWnd = FindMQ2Window("GuildBankWnd");
	}
	else {
		if (GuildBankWnd->dShow)
			return true;
	}
	return false;
}

void Reset() {
	TargI = 0;                 // Target ID
	TargC = 0;                 // Target Check ID
	StopF = FLAG_COMPLETE;     // Stop Event Flag Progress? 
	StopE = DONE_SUCCESS;      // Stop Event Exit Value 
	MoveA = FLAG_COMPLETE;     // Stop Event AdvPath? 
	MoveS = FLAG_COMPLETE;     // Stop Event Stick? 
	MoveF = FLAG_COMPLETE;     // Stop Event MQ2AdvPath Following?
	MoveP = FLAG_COMPLETE;     // Stop Event MQ2AdvPath Pathing?
	MemoF = FLAG_COMPLETE;     // Memo Event Flag 
	MemoE = DONE_SUCCESS;      // Memo Event Exit 
	ItemF = FLAG_COMPLETE;     // Item Flag
	DuckF = FLAG_COMPLETE;     // Duck Flag
	CastF = FLAG_COMPLETE;     // Cast Flag
	CastE = CAST_SUCCESS;      // Cast Exit Return value
	CastG = NOID;              // Cast Gem ID
	CastI = NULL;              // Cast ID   [spell/alt/item/disc]
	CastK = NOID;              // Cast Kind [spell/alt/item/disc] [-1=unknown]
	CastT = 0;                 // Cast Time [spell/alt/item/disc]
	CastB[0] = 0;              // Cast Bandolier In
	CastB[0] = 0;              // Cast Bandolier Out
	CastC[0] = 0;              // Cast SpellType
	CastN[0] = 0;              // Cast SpellName
	CastR = 1;                 // Cast Retry Counter
	CastW = 0;                 // Cast Retry Type
	Invisible = false;         // Invisibility Check?
	ZeroMemory(&SpellToMemorize, sizeof(SPELLFAVORITE));
	strcpy_s(SpellToMemorize.Name, "Mem a Spell");
	SpellToMemorize.inuse = 1;
	for (int sp = 0; sp<NUM_SPELL_GEMS; sp++) {
		SpellToMemorize.SpellId[sp] = 0xFFFFFFFF;
	}
	SpellTotal = 0;
}

long SlotID(PCHAR ID)
{
	long Search = IsNumber(ID);
	DWORD Number = atoi(ID);
	if (Search) {
		return (Number<NUM_INV_SLOTS) ? Number : NOID;
	}
	for (Number = 0; szItemSlot[Number]; Number++) {
		if (!_stricmp(ID, szItemSlot[Number])) {
			return Number;
		}
	}
	return NOID;
}

PSPELL SpellBook(PCHAR ID)
{
	if (ID[0]) {
		if (IsNumber(ID)) {
			int Number = atoi(ID);
			for (DWORD nSpell = 0; nSpell < NUM_BOOK_SLOTS; nSpell++) {
				if (GetCharInfo2()->SpellBook[nSpell] == Number) {
					return GetSpellByID(Number);
				}
			}
		}
		else {
			for (DWORD nSpell = 0; nSpell < NUM_BOOK_SLOTS; nSpell++) {
				if (PSPELL pSpell = GetSpellByID(GetCharInfo2()->SpellBook[nSpell])) {
					if (!_stricmp(ID, pSpell->Name)) {
						return pSpell;
					}
				}
			}
		}
	}
	return NULL;
}
//returns true if an item has a clicky effect equal to szItemName
bool ItemSearch(PCHAR szItemName, long B, long E)
{
	//can it ever be a number?
	//I dont see any calls to it where it is...
	PCONTENTS pItem = 0;
	if (IsNumber(szItemName)) {
		pItem = FindItemByID(atoi(szItemName));
		//return ItemFound(atoi(szItemName),B,E);
	}
	else {
		pItem = FindItemByName(szItemName, 1);
	}
	if (pItem) {
		fSLOT = pItem->GlobalIndex.Index.Slot1;
		fITEM = pItem;
		fPACK = NULL;
		if (PCONTENTS pPack = FindItemBySlot(pItem->GlobalIndex.Index.Slot1)) {
			if (GetItemFromContents(pPack)->Type == ITEMTYPE_PACK) {
				fPACK = pPack;
			}
		}
		return true;
	}
	fSLOT = 0;
	fITEM = 0;
	fPACK = 0;
	return false;
}
bool SpellFind(PCHAR szSpellorAltorItemName, PCHAR szTYPE) {
	DWORD n = 0;
	if (szSpellorAltorItemName[0]) {
		// is it an alt ability?
		if (!szTYPE[0] || !_strnicmp(szTYPE, "alt", 3)) {
			if (PALTABILITY Search = AltAbility(szSpellorAltorItemName)) {
				if (PSPELL spell = GetSpellByID(Search->SpellID)) {
					fFIND = spell;
					fINFO = Search;
					fTIME = fFIND->CastTime;
					fNAME = (PCHAR)fFIND->Name;
					fTYPE = TYPE_ALT;
					return true;
				}
			}
		}
		// nope was'nt an altability, so is it a spell?
		if (!szTYPE[0] || !_strnicmp(szTYPE, "gem", 3) || IsNumber(szTYPE)) {
			if (PSPELL Search = SpellBook(szSpellorAltorItemName)) {
				fFIND = Search;
				fINFO = Search;
				fTIME = pCharData1->GetAACastingTimeModifier((EQ_Spell*)fFIND) +
					pCharData1->GetFocusCastingTimeModifier((EQ_Spell*)fFIND, (EQ_Equipment**)&n, 0) +
					fFIND->CastTime;
				fNAME = (PCHAR)fFIND->Name;
				fTYPE = TYPE_SPELL;
				return true;
			}
		}
		// ok... ehm, not a spell, is it a clicky then?
		if (ItemSearch(szSpellorAltorItemName, 0, NUM_INV_SLOTS)) {
			//if(GetItemFromContents(fITEM)->Clicky.SpellID) {
			if (GetSpellByID(GetItemFromContents(fITEM)->Clicky.SpellID)) {
				fFIND = GetSpellByID(GetItemFromContents(fITEM)->Clicky.SpellID);
				fINFO = fITEM;
				fTIME = GetItemFromContents(fITEM)->Clicky.CastTime;
				fNAME = (PCHAR)GetItemFromContents(fITEM)->Name;
				fTYPE = TYPE_ITEM;
				return true;
			}
		}
	}
	fFIND = NULL;
	fINFO = NULL;
	fTYPE = 0;
	return false;
}
long SpellTimer(long Type, void *Data)
{
	int Ready = 0;
	switch (Type) {
	case TYPE_SPELL:
	{
		if (GEMReady(GEMID(((PSPELL)Data)->ID))) {
			return 0;
		}
		if (BardClass()) {
			return 2;
		}
		if (GetCharInfo()->pSpawn->Level<4) {
			return (long)((PSPELL)Data)->RecoveryTime * 2;
		}
		return (long)((PSPELL)Data)->RecoveryTime;
	}
	case TYPE_ALT:
	{
		if (pAltAdvManager->GetCalculatedTimer(pPCData, (PALTABILITY)Data)>0) {
			pAltAdvManager->IsAbilityReady(pPCData, (PALTABILITY)Data, &Ready);
			return (Ready<1) ? 0 : Ready * 1000;
		}
		return 999999;
	}
	case TYPE_ITEM:
	{
		return GetItemTimer((PCONTENTS)Data) * 1000;
	}
	}
	return 999999;
}

bool SpellReady(PCHAR szSpellName)
{
	if (szSpellName[0] == 0) {
		return true;
	}
	if (IsNumber(szSpellName)) {
		long number = atoi(szSpellName) - 1;
		if ((DWORD)number<NUM_SPELL_GEMS) {
			return GEMReady(number);
		}
	}
	if (szSpellName[0] == 'M' && strlen(szSpellName) == 1) {
		if (FindMQ2DataType("Twist"))
			Twisting = (bool)Evaluate("${If[${Twist.Twisting},1,0]}");
		return !Twisting ? true : false;
	}
	char zName[MAX_STRING];
	GetArg(zName, szSpellName, 1, FALSE, FALSE, FALSE, '|');
	char zType[MAX_STRING];
	GetArg(zType, szSpellName, 2, FALSE, FALSE, FALSE, '|');
	if (SpellFind(zName, zType)) {
		if (!SpellTimer(fTYPE, fINFO)) {
			return true;
		}
	}
	return false;
}

void Stick(PCHAR zFormat, ...)
{
	typedef VOID(__cdecl *StickCALL) (PSPAWNINFO, PCHAR);
	char zOutput[MAX_STRING]; va_list vaList; va_start(vaList, zFormat);
	vsprintf_s(zOutput, zFormat, vaList);
	PMQPLUGIN pLook = pPlugins;
	while (pLook && _strnicmp(pLook->szFilename, "MQ2MoveUtils", 12)) {
		pLook = pLook->pNext;
	}
	if (pLook && pLook->fpVersion>0.9999 && pLook->RemoveSpawn) {
		if (StickCALL Request = (StickCALL)GetProcAddress(pLook->hModule, "StickCommand")) {
			Request(GetCharInfo()->pSpawn, zOutput);
		}
	}
}

void FollowPath(PCHAR zFormat, ...)
{
	typedef VOID(__cdecl *FollowCALL) (PSPAWNINFO, PCHAR);
	char zOutput[MAX_STRING]; va_list vaList; va_start(vaList, zFormat);
	vsprintf_s(zOutput, zFormat, vaList);
	PMQPLUGIN pLook = pPlugins;
	while (pLook && _strnicmp(pLook->szFilename, "MQ2AdvPath", 10)) {
		pLook = pLook->pNext;
	}
	if (pLook && pLook->fpVersion>0.999 && pLook->RemoveSpawn) {
		if (FollowCALL Request = (FollowCALL)GetProcAddress(pLook->hModule, "MQFollowCommand")) {
			Request(GetCharInfo()->pSpawn, zOutput);
		}
	}
}
void Path(PCHAR zFormat, ...)
{
	typedef VOID(__cdecl *FollowCALL) (PSPAWNINFO, PCHAR);
	char zOutput[MAX_STRING]; va_list vaList; va_start(vaList, zFormat);
	vsprintf_s(zOutput, zFormat, vaList);
	PMQPLUGIN pLook = pPlugins;
	while (pLook && _strnicmp(pLook->szFilename, "MQ2AdvPath", 10)) {
		pLook = pLook->pNext;
	}
	if (pLook && pLook->fpVersion>0.999 && pLook->RemoveSpawn) {
		if (FollowCALL Request = (FollowCALL)GetProcAddress(pLook->hModule, "MQPlayCommand")) {
			Request(GetCharInfo()->pSpawn, zOutput);
		}
	}
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

class MQ2CastType *pCastType = 0;
class MQ2CastType : public MQ2Type
{
private:
	char Temps[MAX_STRING];
public:
	enum CastMembers {
		Active = 1,
		Effect = 2,
		Stored = 3,
		Result = 4,
		Return = 5,
		Status = 6,
		Timing = 7,
		Taken = 8,
		Ready = 9,
	};
	MQ2CastType() :MQ2Type("Cast") {
		TypeMember(Active);
		TypeMember(Effect);
		TypeMember(Stored);
		TypeMember(Result);
		TypeMember(Return);
		TypeMember(Status);
		TypeMember(Timing);
		TypeMember(Taken);
		TypeMember(Ready);
	}
	bool GetMember(MQ2VARPTR VarPtr, PCHAR Member, PCHAR Index, MQ2TYPEVAR &Dest) {
		PMQ2TYPEMEMBER pMember = MQ2CastType::FindMember(Member);
		if (pMember) {
			switch ((CastMembers)pMember->ID)
			{
			case Active:
				Dest.DWord = (gbInZone);
				Dest.Type = pBoolType;
				return true;
			case Effect:
				Dest.DWord = GetCharInfo()->pSpawn->CastingData.SpellID;
				if ((long)Dest.DWord == NOID && CastF != FLAG_COMPLETE) {
					Dest.DWord = CastS->ID;
				}
				if ((long)Dest.DWord != NOID) {
					Dest.Ptr = GetSpellByID(Dest.DWord);
					Dest.Type = pSpellType;
				}
				return true;
			case Stored:
				if (CastingL != NOID) {
					Dest.Ptr = GetSpellByID(CastingL);
					Dest.Type = pSpellType;
				}
				return true;
			case Timing:
				Dest.DWord = (DWORD)CastingLeft();
				Dest.Type = pIntType;
				return true;
			case Status:
				Temps[0] = '\0';
				if (CastingC != NOID || CastF != FLAG_COMPLETE || (pCastingWnd && (PCSIDLWND)pCastingWnd->dShow)) {
					strcat_s(Temps, "C");
				}
				if (StopF != FLAG_COMPLETE) strcat_s(Temps, "S");
				if (MoveA != FLAG_COMPLETE) strcat_s(Temps, "A");
				if (MoveS != FLAG_COMPLETE) strcat_s(Temps, "F");
				if (MoveF != FLAG_COMPLETE) strcat_s(Temps, "P");
				if (MoveP != FLAG_COMPLETE) strcat_s(Temps, "P");
				if (MemoF != FLAG_COMPLETE) strcat_s(Temps, "M");
				if (DuckF != FLAG_COMPLETE) strcat_s(Temps, "D");
				if (ItemF != FLAG_COMPLETE) strcat_s(Temps, "E");
				if (!Temps[0]) {
					strcat_s(Temps, "I");
				}
				Dest.Ptr = Temps;
				Dest.Type = pStringType;
				return true;
			case Result:
			case Return:
				switch ((pMember->ID == Result) ? CastingX : Resultat)
				{
				case DONE_PROGRESS:
				case CAST_SUCCESS:      strcpy_s(Temps, "CAST_SUCCESS");     break;
				case CAST_INTERRUPTED:  strcpy_s(Temps, "CAST_INTERRUPTED"); break;
				case CAST_RESIST:       strcpy_s(Temps, "CAST_RESIST");      break;
				case CAST_COLLAPSE:     strcpy_s(Temps, "CAST_COLLAPSE");    break;
				case CAST_RECOVER:      strcpy_s(Temps, "CAST_RECOVER");     break;
				case CAST_FIZZLE:       strcpy_s(Temps, "CAST_FIZZLE");      break;
				case CAST_STANDING:     strcpy_s(Temps, "CAST_STANDING");    break;
				case CAST_STUNNED:      strcpy_s(Temps, "CAST_STUNNED");     break;
				case CAST_INVISIBLE:    strcpy_s(Temps, "CAST_INVISIBLE");   break;
				case CAST_NOTREADY:     strcpy_s(Temps, "CAST_NOTREADY");    break;
				case CAST_OUTOFMANA:    strcpy_s(Temps, "CAST_OUTOFMANA");   break;
				case CAST_OUTOFRANGE:   strcpy_s(Temps, "CAST_OUTOFRANGE");  break;
				case CAST_NOTARGET:     strcpy_s(Temps, "CAST_NOTARGET");    break;
				case CAST_CANNOTSEE:    strcpy_s(Temps, "CAST_CANNOTSEE");   break;
				case CAST_COMPONENTS:   strcpy_s(Temps, "CAST_COMPONENTS");  break;
				case CAST_OUTDOORS:     strcpy_s(Temps, "CAST_OUTDOORS");    break;
				case CAST_TAKEHOLD:     strcpy_s(Temps, "CAST_TAKEHOLD");    break;
				case CAST_IMMUNE:       strcpy_s(Temps, "CAST_IMMUNE");      break;
				case CAST_DISTRACTED:   strcpy_s(Temps, "CAST_DISTRACTED");  break;
				case CAST_ABORTED:      strcpy_s(Temps, "CAST_CANCELLED");   break;
				case CAST_UNKNOWN:      strcpy_s(Temps, "CAST_UNKNOWN");     break;
				default:                strcpy_s(Temps, "CAST_NEEDFIXTYPE"); break;
				}
				Dest.Ptr = Temps;
				Dest.Type = pStringType;
				return true;
			case Ready:
				Dest.DWord = (gbInZone && !Flags() && !Paused() && (pSpellBookWnd && !pSpellBookWnd->dShow) && !(GetCharInfo()->Stunned) && SpellReady(Index));
				Dest.Type = pBoolType;
				return true;
			case Taken:
				Dest.DWord = (CastingX == CAST_TAKEHOLD);
				Dest.Type = pBoolType;
				return true;
			}
		}
		strcpy_s(Temps, "NULL");
		Dest.Type = pStringType;
		Dest.Ptr = Temps;
		return true;
	}
	bool ToString(MQ2VARPTR VarPtr, PCHAR Destination) {
		strcpy_s(Destination, MAX_STRING, "TRUE");
		return true;
	}
	bool FromData(MQ2VARPTR &VarPtr, MQ2TYPEVAR &Source) {
		return false;
	}
	bool FromString(MQ2VARPTR &VarPtr, PCHAR Source) {
		return false;
	}
	~MQ2CastType() {}
};

BOOL dataCast(PCHAR szName, MQ2TYPEVAR &Dest)
{
	Dest.DWord = 1;
	Dest.Type = pCastType;
	return true;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

void StopEnding()
{
	if (MoveS != FLAG_COMPLETE) {
		if (DEBUGGING) {
			WriteChatf("[%I64u] MQ2Cast:[Immobilize]: Stick UnPause Request.", GetTickCount642());
		}
		Stick("unpause");
		MoveS = FLAG_COMPLETE;
	}
	if (MoveA != FLAG_COMPLETE) {
		if (DEBUGGING) {
			WriteChatf("[%I64u] MQ2Cast:[Immobilize]: AdvPath UnPause Request.", GetTickCount642());
		}
		Execute("/varcalc PauseFlag 0");
		MoveA = FLAG_COMPLETE;
	}
	if (MoveF != FLAG_COMPLETE) {
		if (DEBUGGING) {
			WriteChatf("[%I64u] MQ2Cast:[Immobilize]: MQ2AdvPath UnPause Request.", GetTickCount642());
		}
		FollowPath("unpause");
		MoveF = FLAG_COMPLETE;
	}
	if (MoveP != FLAG_COMPLETE) {
		if (DEBUGGING) {
			WriteChatf("[%I64u] MQ2Cast:[Immobilize]: MQ2AdvPath UnPause Request.", GetTickCount642());
		}
		Path("unpause");
		MoveP = FLAG_COMPLETE;
	}
}

void StopHandle()
{
	if (StopF == FLAG_REQUEST) {
		if (DEBUGGING) {
			WriteChatf("[%I64u] MQ2Cast:[Immobilize]: Request.", GetTickCount642());
		}
		StopM = GetTickCount642() + DELAY_STOP;
		StopF = FLAG_PROGRESS1;
		StopE = DONE_PROGRESS;
	}
	if (Evaluate("${If[${Stick.Status.Equal[ON]},1,0]}")) {
		if (DEBUGGING) {
			WriteChatf("[%I64u] MQ2Cast:[Immobilize]: Stick Pause Request.", GetTickCount642());
		}
		Stick("pause");
		MoveS = FLAG_PROGRESS1;
	}
	if (FindMQ2DataVariable("FollowFlag")) {
		//looks like AdvPath.inc is loaded... ok then, fine we will check its tlo...
		if (Evaluate("${If[${Bool[${FollowFlag}]},1,0]}")) {
			if (DEBUGGING) {
				WriteChatf("[%I64u] MQ2Cast:[Immobilize]: AdvPath Pause Request.", GetTickCount642());
			}
			Execute("/varcalc PauseFlag 1");
			MoveA = FLAG_PROGRESS1;
		}
	}
	if (FindMQ2DataType("AdvPath")) {
		if (Evaluate("${If[${AdvPath.Following} && !${AdvPath.Paused},1,0]}")) {
			if (DEBUGGING) {
				WriteChatf("[%I64u] MQ2Cast:[Immobilize]: MQ2AdvPath Pause Request.", GetTickCount642());
			}
			FollowPath("pause");
			MoveF = FLAG_PROGRESS1;
		}
		if (Evaluate("${If[${AdvPath.Playing} && !${AdvPath.Paused},1,0]}")) {
			if (DEBUGGING) {
				WriteChatf("[%I64u] MQ2Cast:[Immobilize]: MQ2AdvPath Pause Request.", GetTickCount642());
			}
			Path("pause");
			MoveP = FLAG_PROGRESS1;
		}
	}
	if (Immobile = Moving()) {
		if (DEBUGGING) {
			WriteChatf("[%I64u] MQ2Cast:[Immobilize]: Complete.", GetTickCount642());
		}
		StopF = FLAG_COMPLETE;
		StopE = DONE_SUCCESS;
	}
	if (GetTickCount642() > StopM) {
		WriteChatf("[%I64u] MQ2Cast:[Immobilize]: Aborting!", GetTickCount642());
		StopF = FLAG_COMPLETE;
		StopE = DONE_ABORTED;
		return;
	}
	if (StopF == FLAG_PROGRESS1) {
		StopF = FLAG_PROGRESS2;
		if (Speed() != 0.0f) {
			MQ2Globals::ExecuteCmd(FindMappableCommand("back"), 1, 0);
			MQ2Globals::ExecuteCmd(FindMappableCommand("back"), 0, 0);
		}
	}
}

void MemoHandle()
{
	if (!pSpellBookWnd) {
		MemoE = DONE_ABORTED;
	}
	else {
		bool Complete = true;
		for (int sp = 0; sp<NUM_SPELL_GEMS; sp++) {
			if (SpellToMemorize.SpellId[sp] != 0xFFFFFFFF && SpellToMemorize.SpellId[sp] != GetCharInfo2()->MemorizedSpells[sp]) {
				Complete = false;
				break;
			}
		}
		if (!Complete) {
			if (MemoF == FLAG_REQUEST) {
				if (DEBUGGING) {
					WriteChatf("[%I64u] MQ2Cast:[Memorize]: Immobilize.", GetTickCount642());
				}
				MemoF = FLAG_PROGRESS1;
				MemoE = DONE_PROGRESS;
				if (GetCharInfo()->pSpawn->Level<4 && DELAY_MEMO<15000) {
					DELAY_MEMO = 15000;
				}
				MemoM = GetTickCount642() + DELAY_STOP + DELAY_MEMO*SpellTotal;
				if (StopF == FLAG_COMPLETE) {
					StopE = DONE_SUCCESS;
				}
				if (StopF == FLAG_COMPLETE) {
					StopF = FLAG_REQUEST;
				}
				if (StopF != FLAG_COMPLETE) {
					StopHandle();
				}
			}
			if (MemoF == FLAG_PROGRESS1 && StopE == DONE_SUCCESS) {
				if (DEBUGGING) {
					WriteChatf("[%I64u] MQ2Cast:[Memorize]: Spell(s).", GetTickCount642());
				}
				MemoF = FLAG_PROGRESS2;
				DWORD Favorite = (DWORD)&SpellToMemorize;
				pSpellBookWnd->MemorizeSet((int*)Favorite, NUM_SPELL_GEMS);
			}
			if (StopE == DONE_ABORTED || GetTickCount642()>MemoM) {
				MemoE = DONE_ABORTED;
			}
		}
		else {
			if (DEBUGGING) {
				WriteChatf("[%I64u] MQ2Cast:[Memorize]: Complete.", GetTickCount642());
			}
			MemoF = FLAG_COMPLETE;
			MemoE = DONE_SUCCESS;
		}
	}
	if (MemoE == DONE_ABORTED || !pSpellBookWnd) {
		WriteChatf("[%I64u] MQ2Cast:[Memorize]: Aborting!", GetTickCount642());
		MemoF = FLAG_COMPLETE;
	}
	if (MemoF == FLAG_COMPLETE && (pSpellBookWnd && (PCSIDLWND)pSpellBookWnd->dShow)) {
		if (DEBUGGING) {
			WriteChatf("[%I64u] MQ2Cast:[Memorize]: Closebook.", GetTickCount642());
		}
		Execute("/book");
	}
}

void DuckHandle(int flag)
{
	if (DEBUGGING) {
		WriteChatf("[%I64u] MQ2Cast:[Duck]: StopCast.", GetTickCount642());
	}
	Execute("/stopcast");
	CastingE = CAST_ABORTED;
	DuckF = FLAG_COMPLETE;
	CastR = 0;
}

void CastHandle()
{
	// we got the casting request cookies, request immobilize/memorize if needed.
	if (CastF == FLAG_REQUEST) {
		if (DEBUGGING) {
			WriteChatf("[%I64u] MQ2Cast:[Casting]: Request.", GetTickCount642());
		}
		CastF = FLAG_PROGRESS1;
		if (StopF == FLAG_COMPLETE) {
			StopF = DONE_SUCCESS;
		}
		if (StopF == FLAG_COMPLETE && CastT>100 && !BardClass()) {
			StopF = FLAG_REQUEST;
		}
		if (MemoF != FLAG_COMPLETE) {
			MemoHandle();
		}
		else {
			if (StopF != FLAG_COMPLETE) {
				StopHandle();
			}
		}
	}

	// waiting on the casting results to take actions.
	if (CastF == FLAG_PROGRESS3 && CastingE != DONE_PROGRESS) {
		CastF = FLAG_PROGRESS4;
		if (CastR) {
			CastR--;
		}
		if (CastR) {
			if ((CastingE == CAST_SUCCESS && CastW != RECAST_LAND) || (CastingE == CAST_COLLAPSE) || (CastingE == CAST_FIZZLE) ||
				(CastingE == CAST_INTERRUPTED) || (CastingE == CAST_RECOVER) || (CastingE == CAST_RESIST)) {
				if (DEBUGGING) {
					WriteChatf("[%I64u] MQ2Cast:[Casting]: AutoRecast [%d].", GetTickCount642(), CastingE);
				}
				if (CastW != RECAST_ZERO && !TargC) {
					TargC = (pTarget) ? ((PSPAWNINFO)pTarget)->SpawnID : 0;
				}
				CastM = GetTickCount642() + DELAY_CAST;
				CastF = FLAG_REQUEST;
			}
		}
	}

	// casting is over, grab latest casting results and exit.
	if (CastF == FLAG_PROGRESS4) {
		if (CastE>CastingE) {
			CastingE = CastE;
		}
		CastF = FLAG_COMPLETE;
	}

	// evaluate if we are taking too long, or immobilize/memorize event failed.
	if (CastF != FLAG_COMPLETE) {
		if (StopE == DONE_ABORTED || MemoE == DONE_ABORTED || GetTickCount642()>CastM) {
			WriteChatf("[%I64u] MQ2Cast:[Casting]: Aborting! (%s)", GetTickCount642(), StopE == DONE_ABORTED ? "StopE" : (MemoE == DONE_ABORTED ? "MemoE" : "CastM"));
			CastF = FLAG_PROGRESS4;
			CastE = CAST_NOTREADY;
		}
	}

	// waiting for opportunity to start casting, end if conditions not favorables.
	if (CastF == FLAG_PROGRESS1) {
		if (pCastingWnd && (PCSIDLWND)pCastingWnd->dShow) {
			return; // casting going on
		}
		CastingC = CastS->ID;
		CastF = FLAG_PROGRESS4;
		if (TargC && (!pTarget || (pTarget && ((PSPAWNINFO)pTarget)->SpawnID != TargC))) {
			if (CastW == RECAST_DEAD) {
				CastE = CAST_NOTARGET;
			}
			else if (CastW == RECAST_LAND) {
				CastE = CAST_ABORTED;
			}
		}
		else {
			if (Invisible && GetCharInfo()->pSpawn->HideMode) {
				CastE = CAST_INVISIBLE;
			}
			else if (GetCharInfo()->Stunned) {
				CastE = CAST_STUNNED;
			}
			else if (StopF != FLAG_COMPLETE || MemoF != FLAG_COMPLETE) {
				CastF = FLAG_PROGRESS1;
			}
			else {
				long TimeReady = SpellTimer(CastK, CastI);  // get estimate time before it's ready.
				if (TimeReady>3000) {
					CastE = CAST_NOTREADY;   // if estimate higher then 3 seconds, abort.
				}
				else if (!TimeReady) {
					CastF = FLAG_PROGRESS2;  // estimate says it's ready, so cast it
				}
				else {
					CastF = FLAG_PROGRESS1;  // otherwise give it some time to be ready.
				}
			}
		}
	}

	// we got the final approbation to cast, so lets do it.
	//this is where it breaks
	if (CastF == FLAG_PROGRESS2) {
		if (DEBUGGING) {
			WriteChatf("[%I64u] MQ2Cast:[Casting]: Cast.", GetTickCount642());
		}
		Success(CastS);
		//ItemHandle(true);
		CastF = FLAG_PROGRESS3;
		CastE = DONE_PROGRESS;
		CastingT = GetTickCount642() + CastT + 250 + (pConnection->Last) * 4;
		CastingE = DONE_PROGRESS;
		CastingC = CastS->ID;
		if ((long)GetCharInfo()->pSpawn->CastingData.SpellID>0) {
			CastingX = (CastingE<CAST_SUCCESS) ? CAST_SUCCESS : CastingE;
			CastingL = CastingC;
			if (CastK == TYPE_SPELL) {
				Execute("/multiline ; /stopsong ; /cast \"%s\"", CastN);
			}
			else if (CastK == TYPE_ITEM) {
				if (!BardBeta) {
					Execute("/multiline ; /stopsong ; /useitem \"%s\"", CastN);
				}
				else {
					Execute("/useitem \"%s\"", CastN);
				}
			}
			else if (CastK == TYPE_ALT) {
				if (!BardBeta) {
					Execute("/multiline ; /stopsong ; /alt activate %d", ((PALTABILITY)CastI)->ID);
				}
				else {
					Execute("/alt activate %d", ((PALTABILITY)CastI)->ID);
				}
			}
		}
		else {
			if (CastK == TYPE_SPELL) {
				Cast("\"%s\"", CastN);
			}
			else if (CastK == TYPE_ITEM) {
				if (DEBUGGING) {
					WriteChatf("/useitem \"%s\"", CastN);
				}
				Execute("/useitem \"%s\"", CastN);
			}
			else if (CastK == TYPE_ALT) {
				Execute("/alt activate %d", ((PALTABILITY)CastI)->ID);
			}
		}
	}
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

PLUGIN_API VOID CastDebug(PSPAWNINFO pChar, PCHAR Cmd)
{
	char zParm[MAX_STRING];
	GetArg(zParm, Cmd, 1);
	if (zParm[0] == 0) {
		DEBUGGING = !DEBUGGING;
	}
	else if (!_strnicmp(zParm, "on", 2)) {
		DEBUGGING = true;
	}
	else if (!_strnicmp(zParm, "off", 2)) {
		DEBUGGING = false;
	}
	else {
		DEBUGGING = !DEBUGGING;
	}
	WriteChatf("\arMQ2Cast\ax::\amDEBUGGING is now %s\ax.", DEBUGGING ? "\aoON" : "\agOFF");
}

PLUGIN_API VOID CastCommand(PSPAWNINFO pChar, PCHAR Cmd)
{
	Resultat = CAST_DISTRACTED;
	if (!gbInZone || Flags() || Paused() || (pSpellBookWnd && (PCSIDLWND)pSpellBookWnd->dShow)) {
		if (DEBUGGING) {
			WriteChatf("[%I64u] MQ2Cast:[Casting]: Complete. [%d][%s%s%s%s]", GetTickCount642(), Resultat,
				gbInZone ? " ZONE " : "", Flags() ? " FLAGS " : "", Paused() ? " PAUSED " : "", (pSpellBookWnd && (PCSIDLWND)pSpellBookWnd->dShow) ? " SHOW " : "");
		}
		return;
	}
	Reset();
	char zParm[MAX_STRING];
	long iParm = 0;
	do {
		GetArg(zParm, Cmd, ++iParm);
		if (zParm[0] == 0) {
			break;
		}
		else if (!_strnicmp(zParm, "-targetid|", 10)) {
			TargI = atoi(&zParm[10]);
		}
		else if (!_strnicmp(zParm, "-kill", 5)) {
			CastW = RECAST_DEAD; CastR = 9999;
		}
		else if (!_strnicmp(zParm, "-maxtries|", 10)) {
			CastW = RECAST_LAND; CastR = atoi(&zParm[10]);
		}
		else if (!_strnicmp(zParm, "-recast|", 8)) {
			CastW = RECAST_ZERO; CastR = atoi(&zParm[8]);
		}
		else if (!_strnicmp(zParm, "-setin|", 6)) {
			GetArg(CastB, zParm, 2, FALSE, FALSE, FALSE, '|');
		}
		else if (!_strnicmp(zParm, "-invis", 6)) {
			Invisible = true;
		}
		else if (zParm[0] != '-' && CastN[0] == 0) {
			GetArg(CastN, zParm, 1, FALSE, FALSE, FALSE, '|');
			GetArg(CastC, zParm, 2, FALSE, FALSE, FALSE, '|');
		}
		else if (zParm[0] != '-' && CastC[0] == 0) {
			GetArg(CastC, zParm, 1, FALSE, FALSE, FALSE, '|');
		}
	} while (true);

	Resultat = CAST_SUCCESS;
	if (GetCharInfo()->Stunned) {
		Resultat = CAST_STUNNED;
	}
	else if (Invisible && GetCharInfo()->pSpawn->HideMode) {
		Resultat = CAST_INVISIBLE;
	}
	else if (!SpellFind(CastN, CastC)) {
		Resultat = CAST_UNKNOWN;
	}
	else if (fTYPE != TYPE_SPELL && SpellTimer(fTYPE, fINFO)) {
		Resultat = CAST_NOTREADY;
	}
	else if (TargI) {
		if (PSPAWNINFO Target = (PSPAWNINFO)GetSpawnByID(TargI)) {
			*(PSPAWNINFO*)ppTarget = Target;
		}
		else {
			Resultat = CAST_NOTARGET;
		}
	}
	if (Resultat == CAST_SUCCESS && fTYPE == TYPE_SPELL) {
		if (BardClass()) {
			if (Twisting) {
				Execute("/stoptwist");
			}
			if (GetCharInfo()->pSpawn->CastingData.SpellID) {
				Execute("/stopsong");
			}
		}
		CastG = GEMID(fFIND->ID);
		if (CastG == NOID) {
			CastG = atoi(&CastC[(_strnicmp(CastC, "gem", 3)) ? 0 : 3]) - 1;
			MemoLoad(CastG, fFIND);
			SpellTotal = 1;
			MemoF = FLAG_REQUEST;
			MemoE = DONE_SUCCESS;
		}
	}
	if (Resultat != CAST_SUCCESS) {
		if (DEBUGGING) {
			WriteChatf("[%I64u] MQ2Cast:[Casting]: Complete. [%d]", GetTickCount642(), Resultat);
		}
		return;
	}
	CastF = FLAG_REQUEST;
	CastI = fINFO;
	CastK = fTYPE;
	CastT = fTIME;
	CastS = fFIND;
	CastM = GetTickCount642() + DELAY_CAST;
	strcpy_s(CastN, fNAME);
	if (DEBUGGING) {
		WriteChatf("[%I64u] MQ2Cast:[Casting]: Name<%s> Type<%d>.", GetTickCount642(), CastN, CastK);
	}
	CastHandle();
}

PLUGIN_API VOID DuckCommand(PSPAWNINFO pChar, PCHAR Cmd)
{
	if (gbInZone) {
		if (CastF != FLAG_COMPLETE) {
			CastR = 0;
		}
		if ((pCastingWnd && (PCSIDLWND)pCastingWnd->dShow) && CastingLeft()>500) {
			DuckF = FLAG_REQUEST;
			DuckHandle(DuckF);
		}
	}
	Resultat = CAST_SUCCESS;
}

PLUGIN_API VOID MemoCommand(PSPAWNINFO pChar, PCHAR zLine)
{
	Resultat = CAST_DISTRACTED;
	if (!gbInZone || Flags() || Paused() || !pSpellBookWnd) {
		return;
	}
	if (GetCharInfo()->Stunned) {
		Resultat = CAST_STUNNED;
		return;
	}
	Reset();
	long iParm = 0;
	char zParm[MAX_STRING];
	char zTemp[MAX_STRING];
	CastingX = CAST_SUCCESS;

	do {
		GetArg(zParm, zLine, ++iParm);
		if (!zParm[0]) {
			break;
		}
		GetArg(zTemp, zParm, 1, FALSE, FALSE, FALSE, '|');
		if (PSPELL Search = SpellBook(zTemp)) {
			if (DEBUGGING) {
				WriteChatf("[%d] MQ2Cast:[Memorize]: Spell Found.", (long)GetTickCount642());
			}
			GetArg(zTemp, zParm, 2, FALSE, FALSE, FALSE, '|');
			long Gem = atoi(&zTemp[(_strnicmp(zTemp, "gem", 3)) ? 0 : 3]) - 1;
			if (!((DWORD)Gem<NUM_SPELL_GEMS)) {
				GetArg(zTemp, zLine, 1 + iParm);
				Gem = atoi(&zTemp[(_strnicmp(zTemp, "gem", 3)) ? 0 : 3]) - 1;
				if ((DWORD)Gem<NUM_SPELL_GEMS) {
					iParm++;
				}
			}
			MemoLoad(Gem, Search);
		}
		else {
			CastingX = CAST_UNKNOWN;
			if (DEBUGGING) {
				WriteChatf("[%d] MQ2Cast:[Memorize]: Spell Not Found. %d", (long)GetTickCount642(), CastingX);
			}
			return;
		}
	} while (true);

	for (int sp = 0; sp<NUM_SPELL_GEMS; sp++) {
		if (SpellToMemorize.SpellId[sp] != 0xFFFFFFFF && SpellToMemorize.SpellId[sp] != GetCharInfo2()->MemorizedSpells[sp]) {
			SpellTotal++;
		}
	}
	if (SpellTotal) {
		MemoF = FLAG_REQUEST;
		MemoE = DONE_SUCCESS;
		MemoHandle();
	}
}

PLUGIN_API VOID SpellSetDelete(PSPAWNINFO pChar, PCHAR Cmd)
{
	Resultat = CAST_ABORTED;
	if (!gbInZone) {
		return;
	}
	else if (!Cmd[0]) {
		MacroError("Usage: /ssd setname");
	}
	else {
		Resultat = CAST_SUCCESS;
		sprintf_s(INIFileName, "%s\\%s_%s.ini", gszINIPath, EQADDR_SERVERNAME, GetCharInfo()->Name);
		WritePrivateProfileString("MQ2Cast(SpellSet)", Cmd, NULL, INIFileName);
	}
}

PLUGIN_API VOID SpellSetList(PSPAWNINFO pChar, PCHAR Cmd)
{
	Resultat = CAST_SUCCESS;
	if (!gbInZone)
		return;
	char Keys[MAX_STRING*NUM_SPELL_GEMS] = { 0 };
	char Temp[MAX_STRING];
	PCHAR pKeys = Keys;
	long Disp = 0;
	Resultat = CAST_SUCCESS;
	sprintf_s(INIFileName, "%s\\%s_%s.ini", gszINIPath, EQADDR_SERVERNAME, GetCharInfo()->Name);
	WriteChatf("MQ2Cast:: SpellSet [\ay Listing... \ax].", Disp);
	GetPrivateProfileString("MQ2Cast(SpellSet)", NULL, "", Keys, MAX_STRING * 10, INIFileName);
	while (pKeys[0]) {
		GetPrivateProfileString("MQ2Cast(SpellSet)", pKeys, "", Temp, MAX_STRING, INIFileName);
		if (Temp[0]) {
			if (!Disp)
				WriteChatf("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-");
			WriteChatf("\ay%s\ax", pKeys);
			Disp++;
		}
		pKeys += strlen(pKeys) + 1;
	}
	if (Disp)
		WriteChatf("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-");
	WriteChatf("MQ2Cast:: SpellSet [\ay %d Displayed\ax ].", Disp);
}

PLUGIN_API VOID SpellSetMemorize(PSPAWNINFO pChar, PCHAR Cmd)
{
	Resultat = CAST_UNKNOWN;
	if (!gbInZone) {
		return;
	}
	else if (!Cmd[0]) {
		MacroError("Usage: /ssm setname");
	}
	else {
		char List[MAX_STRING];
		Resultat = CAST_SUCCESS;
		sprintf_s(INIFileName, "%s\\%s_%s.ini", gszINIPath, EQADDR_SERVERNAME, GetCharInfo()->Name);
		GetPrivateProfileString("MQ2Cast(SpellSet)", Cmd, "", List, MAX_STRING, INIFileName);
		if (List[0])
			MemoCommand(GetCharInfo()->pSpawn, List);
	}
}

PLUGIN_API VOID SpellSetSave(PSPAWNINFO pChar, PCHAR Cmd)
{
	if (!gbInZone) {
		return;
	}
	char zSet[MAX_STRING]; GetArg(zSet, Cmd, 1);
	char zGem[MAX_STRING]; GetArg(zGem, Cmd, 2);
	Resultat = CAST_ABORTED;
	if (!zSet[0]) {
		MacroError("Usage: /sss setname <gemlist>");
		return;
	}
	if (!zGem[0]) {
		sprintf_s(zGem, "123456789ABC");
	}
	char zLst[MAX_STRING] = { 0 };
	char zTmp[MAX_STRING];
	long find = 0;
	for (int g = 0; g<NUM_SPELL_GEMS; g++)
		if ((long)GetCharInfo2()->MemorizedSpells[g]>0) {
			if (strstr(zGem, ListGems[g])) {
				sprintf_s(zTmp, "%d|%d", GetCharInfo2()->MemorizedSpells[g], g + 1);
				if (find) {
					strcat_s(zLst, " ");
				}
				strcat_s(zLst, zTmp);
				find++;
			}
		}
	Resultat = CAST_UNKNOWN;
	if (find) {
		Resultat = CAST_SUCCESS;
		sprintf_s(INIFileName, "%s\\%s_%s.ini", gszINIPath, EQADDR_SERVERNAME, GetCharInfo()->Name);
		WritePrivateProfileString("MQ2Cast(SpellSet)", Cmd, zLst, INIFileName);
	}
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

PLUGIN_API VOID InitializePlugin(VOID)
{
	CHAR BetaSwitch[MAX_STRING] = { 0 };
	if (GetPrivateProfileString("Settings", "Normal", "", BetaSwitch, MAX_STRING, INIFileName)) {
		BardBeta = false;
	}
	aCastEvent(LIST289, CAST_COLLAPSE, "Your gate is too unstable, and collapses#*#");
	aCastEvent(LIST289, CAST_CANNOTSEE, "You cannot see your target#*#");
	aCastEvent(LIST289, CAST_COMPONENTS, "You are missing some required components#*#");
	aCastEvent(UNKNOWN, CAST_COMPONENTS, "Your ability to use this item has been disabled because you do not have at least a gold membership#*#");
	aCastEvent(LIST289, CAST_COMPONENTS, "You need to play a#*#instrument for this song#*#");
	aCastEvent(LIST289, CAST_DISTRACTED, "You are too distracted to cast a spell now#*#");
	aCastEvent(LIST289, CAST_DISTRACTED, "You can't cast spells while invulnerable#*#");
	aCastEvent(LIST289, CAST_DISTRACTED, "You *CANNOT* cast spells, you have been silenced#*#");
	aCastEvent(LIST289, CAST_IMMUNE, "Your target has no mana to affect#*#");
	aCastEvent(LIST013, CAST_IMMUNE, "Your target is immune to changes in its attack speed#*#");
	aCastEvent(LIST013, CAST_IMMUNE, "Your target is immune to changes in its run speed#*#");
	aCastEvent(LIST013, CAST_IMMUNE, "Your target is immune to snare spells#*#");
	aCastEvent(LIST289, CAST_IMMUNE, "Your target cannot be mesmerized#*#");
	aCastEvent(UNKNOWN, CAST_IMMUNE, "Your target looks unaffected#*#");
	aCastEvent(LIST264, CAST_INTERRUPTED, "Your spell is interrupted#*#");
	aCastEvent(UNKNOWN, CAST_INTERRUPTED, "Your casting has been interrupted#*#");
	aCastEvent(LIST289, CAST_FIZZLE, "Your spell fizzles#*#");
	aCastEvent(LIST289, CAST_FIZZLE, "You miss a note, bringing your song to a close#*#");
	aCastEvent(LIST289, CAST_NOTARGET, "You must first select a target for this spell#*#");
	aCastEvent(LIST289, CAST_NOTARGET, "This spell only works on#*#");
	aCastEvent(LIST289, CAST_NOTARGET, "You must first target a group member#*#");
	aCastEvent(LIST289, CAST_NOTREADY, "Spell recast time not yet met#*#");
	aCastEvent(LIST289, CAST_OUTOFMANA, "Insufficient Mana to cast this spell#*#");
	aCastEvent(LIST289, CAST_OUTOFRANGE, "Your target is out of range, get closer#*#");
	aCastEvent(LIST289, CAST_OUTDOORS, "This spell does not work here#*#");
	aCastEvent(LIST289, CAST_OUTDOORS, "You can only cast this spell in the outdoors#*#");
	aCastEvent(LIST289, CAST_OUTDOORS, "You can not summon a mount here#*#");
	aCastEvent(LIST289, CAST_OUTDOORS, "You must have both the Horse Models and your current Luclin Character Model enabled to summon a mount#*#");
	aCastEvent(LIST264, CAST_RECOVER, "You haven't recovered yet#*#");
	aCastEvent(LIST289, CAST_RECOVER, "Spell recovery time not yet met#*#");
	aCastEvent(LIST289, CAST_RESIST, "Your target resisted the#*#spell#*#");
	aCastEvent(LIST289, CAST_STANDING, "You must be standing to cast a spell#*#");
	aCastEvent(LIST289, CAST_STUNNED, "You can't cast spells while stunned#*#");
	aCastEvent(LIST289, CAST_SUCCESS, "You are already on a mount#*#");
	aCastEvent(LIST289, CAST_TAKEHOLD, "Your spell did not take hold#*#");
	aCastEvent(LIST289, CAST_TAKEHOLD, "Your spell would not have taken hold#*#");
	aCastEvent(LIST289, CAST_TAKEHOLD, "Your spell is too powerfull for your intended target#*#");
	aCastEvent(LIST289, CAST_TAKEHOLD, "You need to be in a more open area to summon a mount#*#");
	aCastEvent(LIST289, CAST_TAKEHOLD, "You can only summon a mount on dry land#*#");
	aCastEvent(LIST289, CAST_TAKEHOLD, "This pet may not be made invisible#*#");
	pCastType = new MQ2CastType;
	AddMQ2Data("Cast", dataCast);
	AddCommand("/castdebug", CastDebug);
	AddCommand("/casting", CastCommand);
	AddCommand("/interrupt", DuckCommand);
	AddCommand("/memorize", MemoCommand);
	AddCommand("/ssd", SpellSetDelete);
	AddCommand("/ssl", SpellSetList);
	AddCommand("/ssm", SpellSetMemorize);
	AddCommand("/sss", SpellSetSave);
}


PLUGIN_API void SetGameState(unsigned long ulGameState)
{
	if (GetGameState() != GAMESTATE_INGAME)
	{
		cPendingEq = false;
		ulTimer = 0;
		ulTimerR = 0;
		TributeMasterWnd = 0;
		GuildBankWnd = 0;
	}
}

PLUGIN_API VOID ShutdownPlugin(VOID)
{
	RemoveMQ2Data("Cast");
	delete pCastType;
	RemoveCommand("/castdebug");
	RemoveCommand("/casting");
	RemoveCommand("/interrupt");
	RemoveCommand("/memorize");
	RemoveCommand("/ssd");
	RemoveCommand("/ssl");
	RemoveCommand("/ssm");
	RemoveCommand("/sss");
}

PLUGIN_API VOID OnEndZone(VOID)
{
	Reset();
	CastingO = NOID;
	CastingC = NOID;
	CastingE = CAST_SUCCESS;
	CastingT = 0;
	ImmobileT = 0;
}

PLUGIN_API DWORD OnIncomingChat(PCHAR Line, DWORD Color)
{
	CHAR szLine[MAX_STRING] = { 0 };
	strcpy_s(szLine, Line);
	if (gbInZone) {
		if (CastingC != NOID && !Twisting) {
			Parsed = false;
			if (DEBUGGING) {
				WriteChatf("[%I64u] OnIncomingChat:: ChatLine: %s Color: %d", GetTickCount642(), szLine, Color);
			}
			if (Color == 264) {
				LIST264.Feed(szLine);
				SUCCESS.Feed(szLine);
			}
			else if (Color == 289) {
				LIST289.Feed(szLine);
			}
			else if (Color == 13) {
				LIST013.Feed(szLine);
			}
			if (!Parsed) {
				UNKNOWN.Feed(szLine);
				if (Parsed) {
					WriteChatf("\arMQ2Cast::Note for Author[\ay%s\ar]=(\ag%d\ar)\ax", szLine, Color);
				}
			}
		}
	}
	return 0;
}
PLUGIN_API VOID OnPulse(VOID)
{
	if (gbInZone && GetTickCount642()>CastingP && GetCharInfo() && GetCharInfo()->pSpawn) {
		CastingP = GetTickCount642() + DELAY_PULSE;

		// evaluate immobile flag and handle immobilize request
		Immobile = Moving();
		if (StopF != FLAG_COMPLETE) {
			StopHandle();
		}
		CastingD = GetCharInfo()->pSpawn->CastingData.SpellID;

		// casting window currently openened?
		if (pCastingWnd && (PCSIDLWND)pCastingWnd->dShow) {
			Casting = true;
			if (CastingO == NOID) {
				CastingO = (pTarget) ? ((long)((PSPAWNINFO)pTarget)->SpawnID) : 0;
			}

			// was this an unecpected cast?
			if (CastingD != CastingC && CastingD != NOID) {
				CastingE = DONE_PROGRESS;
				CastingC = CastingD;
				CastingT = GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp +
					GetTickCount642() + 450 + (pConnection->Last) * 4;
				Success(GetSpellByID(CastingD));
			}

			// are we attempting to interrupt this?
			if (DuckF != FLAG_COMPLETE) {
				DuckHandle(DuckF);
			}
			return;
		}

		// wait for incoming chat, timers, and windows to be closed.
		DuckF = FLAG_COMPLETE;
		if(FindMQ2DataType("Twist"))
			Twisting = Evaluate("${If[${Twist.Twisting},1,0]}") ? true : false;
		if (Casting) {
			if (CastingC == CastingD) {
				if (PSPELL Spell = GetSpellByID(CastingC))
				{
					switch (Spell->TargetType)
					{
					case 18: // Uber Dragons
					case 17: // Uber Giants
					case 16: // Plant
					case 15: // Corpse
					case 14: // Pet
					case 11: // Summoned
					case 10: // Undead
					case  9: // Animal
					case  5: // Single
						if (!pTarget) {
							CastingE = CAST_NOTARGET;
						}
						break;
					}
				}
			}
			// re-evaluate casting timer after cast window close
			CastingT = GetTickCount642() + 450 + (pConnection->Last) * 2;
			Casting = false;
		}
		if (CastingE == DONE_PROGRESS) {
			if (GetTickCount642()>CastingT) {
				CastingE = CAST_SUCCESS;
			}
			else if (!Twisting) {
				return;
			}
		}
		if (Paused()) {
			if ((long)GetCharInfo()->pSpawn->CastingData.SpellID>0) {
				Execute("/stopsong");
			}
			return;
		}

		// give time to proceed other casting events
		if (MemoF != FLAG_COMPLETE)
			MemoHandle();
		if (MemoF != FLAG_COMPLETE)
			return;
		if (CastF != FLAG_COMPLETE)
			CastHandle();

		// make sure we get final casting results
		if ((CastF == FLAG_COMPLETE && CastingC != NOID && CastingD == NOID) || (BardClass() && CastingC != NOID && (CastingD != NOID))) {
			CastingX = (CastingE<CAST_SUCCESS) ? CAST_SUCCESS : CastingE;
			CastingL = CastingC;
			CastingE = DONE_COMPLETE;
			if (!Twisting) {
				if (DEBUGGING) {
					WriteChatf("[%I64u] MQ2Cast:: Casting Complete ID[%d] Result=[%d]", GetTickCount642(), CastingL, CastingX);
				}
				CastTimer(CastingO, CastingC, CastingX); // patches for ae but sound illogicials
			}
			CastingC = NOID;
			CastingO = NOID;
		}

		// make sure we finish other casting events
		if (CastF == FLAG_COMPLETE) {
			StopEnding();
			if (PulseCount) {
				PITEMINFO pCursor = GetItemFromContents(GetCharInfo2()->pInventoryArray->Inventory.Cursor);
				if (PulseCount>5 && !pCursor) {
					PulseCount = 0;
					return;
				}
				if (GetCharInfo2()->pInventoryArray->Inventory.Cursor && PulseCount) {
					ClickBack();
				}
				if (PulseCount && PulseCount<7) {
					PulseCount++;
				}
			}
		}
	}
}

void WinClick(CXWnd *Wnd, PCHAR ScreenID, PCHAR ClickNotification, DWORD KeyState)
{
	if (Wnd) {
		if (CXWnd *Child = Wnd->GetChildItem(ScreenID)) {
			BOOL KeyboardFlags[4];
			*(DWORD*)&KeyboardFlags = *(DWORD*)&((PCXWNDMGR)pWndMgr)->KeyboardFlags;
			*(DWORD*)&((PCXWNDMGR)pWndMgr)->KeyboardFlags = KeyState;
			SendWndClick2(Child, ClickNotification);
			*(DWORD*)&((PCXWNDMGR)pWndMgr)->KeyboardFlags = *(DWORD*)&KeyboardFlags;
		}
	}
	return;
}

void ClickBack()
{
	if (!GetCharInfo2()->pInventoryArray->Inventory.Cursor || (pCastingWnd && (PCSIDLWND)pCastingWnd->dShow) ||
		(pSpellBookWnd && (PCSIDLWND)pSpellBookWnd->dShow)) {
		return;
	}
	if (GetCharInfo2()->pInventoryArray->Inventory.Cursor && PulseCount) {
		PITEMINFO pCursor = GetItemFromContents(GetCharInfo2()->pInventoryArray->Inventory.Cursor);
		if (pCursor && pCursor->Type == ITEMTYPE_PACK) {
			//if(GetCharInfo2()->pInventoryArray->Inventory.Cursor->Item->Type==ITEMTYPE_PACK) {
			WriteChatf("Pack Type");
			WinClick((CXWnd*)pInventoryWnd, "InvSlot30", "leftmouseup", 0);
			PulseCount = 1;
			return;
		}
		PITEMINFO pCursor2 = GetItemFromContents(GetCharInfo2()->pInventoryArray->Inventory.Cursor);
		if (pCursor2->Type != ITEMTYPE_PACK) {
			//if(GetCharInfo2()->pInventoryArray->Inventory.Cursor->Item->Type!=ITEMTYPE_PACK) {    
			WriteChatf("Not a pack");
			WinClick((CXWnd*)pInventoryWnd, "IW_CharacterView", "leftmouseup", 0);
			PulseCount = 0;
			return;
		}
		return;
	}
}

PLUGIN_API VOID OnReloadUI()
{
	TributeMasterWnd = 0;
	GuildBankWnd = 0;
}