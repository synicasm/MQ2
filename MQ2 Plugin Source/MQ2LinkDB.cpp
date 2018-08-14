// MQ2LinkDB 
// 
// Version 2.2 - 16th Aug 2007 - Originally by Ziggy, 
//  then updated for DoD, PoR, SS, BS, modifications by rswiders
//  updated for CoF
//
//  Currently linkbot functions are not working.
// 
// Personal link database 
// 
// Usage: /link               - Display statistics 
//        /link /import       - Import items.txt as downloaded from 13th Floor 
//                              (Unzip the items.txt file to the mq2\release 
//                              directory) 
//        /link /max n        - Set number of maximum displayed results 
//                              (default 10) 
//        /link /scan on|off  - Turn on and off scanning incoming chat 
//        /link /click on|off - 
//        /link search        - Find items containg search string 
// 
// Item links are displayed as tells from Linkdb in normal chat windows. This is 
// so you can use the links given. MQ2's ChatWnd removes links. 
// They do not go to log file. Nor will you get the Linkdb name pop up when 
// you hit your reply button. 
// 
// Incoming chat is scanned for links if specified, and the database is 
// checked for this item. If it's not in database, it will be added. 
// 
// The TLO LinkDB is also added by this plugin. The LinkDB TLO supports
// a simple lookup for items by name and returns the item's link text.
// Note that since MQ2ChatWnd strips out stuff, you can't click links in
// there, so you'll have to stick the output in a variable then use
// a macro to control where you want it to go.
// 
// The TLO supports substring matches and exact matches. If you pass
// =Item Name to the TLO, it will do an exact match. If you just do
// Item Name, then it will use a prefix match. If there are multiple
// items (i.e. multiple items with the same name in an exact, or
// multiple items with the same prefix in a non-exact), then the TLO
// will return the first match and you will have no idea there were
// multiple results.
// 
// Example TLO usage:
// /declare BABYLINK string outer
// /varset BABYLINK ${LinkDB[=Baby Joseph Sayer]}
// 
// /shout OMG I'm a dork! I have ${LinkDB[=Baby Joseph Sayer]} in my pack. Ha!
// 
// If the item is not found, the TLO returns an empty string, so you probably don't
// want to be directly shouting about Baby Joseph Sayer in your backpack.
// If you do and misspell his name, you will end up shouting about an empty string
// which isn't recommended.
//
//
// Changes: 
// 2.3 - Eqmule 07-22-2016 - Added string safety.
// 2.2  Updated for CoF release. Linkbot ability is not working
// 2.1  Updated to fix charm links.  Added all the new fields as defined on 13-floor and
//      corrected a long standing issue with an escaped delimiter in the names of 3 items.
//      You NO LONGER have to remove the left and right hand entries, they are created
//      as links correctly.
// 2.0  This version, with linkbot ability, must remain in the VIP forums.
//		Added linkbot functionality using an authorization list.  Will automatically
//		click an exact match link.  Added the ability to retrieve a link based on
//		the item id using /item #.
//
//		Linkbot called with tells using the !link command.  It will respond to the
//		caller with the list of links as if you entered a /link command directly.
// 1.7  Added simple TLO for accessing links from item names. 
// 1.6  Updated for Broken Seas item format changes. Thanks to ksmith and 
//      Nilwean at EQItems. See 
//      http://eqitems.13th-floor.org/phpBB2/viewtopic.php?t=229 
// 1.5  Updated for 12/5 item format changes. Thanks to Nilwean and ksmith 
//      at EQItems. See 
//      http://eqitems.13th-floor.org/phpBB2/viewtopic.php?t=215 
// 1.4  Updated for SS expansion. Thanks to ksmith at EQItems. See 
//          http://eqitems.13th-floor.org/phpBB2/viewtopic.php?t=202 
// 1.3  Updated for PoR expansion. Thanks to ksmith at EQItems. See 
//          http://eqitems.13th-floor.org/phpBB2/viewtopic.php?t=170 
// 
// 1.2  Added ScanChat ini setting to toggle whether to snarf links from 
//          seen chatlines. Defaults to on to simulate current behavior. 
//          Also updated for EQItems fixes to their export which was missing 
//          a field. 
// 
// 1.1  Updated to reflect new link format. Thanks to ksmith and Nilwean 
//          and topside from 
//          http://eqitems.13th-floor.org/phpBB2/viewtopic.php?t=145 
//      
// 1.06 Alpha sorts the list 
//      Makes sure that if an exact match is found, then it's displayed, even 
//      if we've already displayed max results 
//      Only searches the name for the item name, previously searched the 
//      whole link (eg: EB41 would have matched 2 items from their hash key) 
// 
// 1.05 Fixed the really stupid mistake with item bitfield. And found out 
//      a new equation: 
//      Moved items.txt import into plugin (You can still use it external if 
//      you want) 
// 
// 1.04 Added Max item list so we don't get too spammed by results. 
// 
// 1.03 Added some more clues for debugging purposes (do: /link /quiet to show) 
// 
// 1.02 Fixed file locking fudge up. Should now add items to database properly. 
// 
// 1.01 Added item index so we know already which items the DB has to save a 
//      bunch of time with checking when adding new items 
// 
// TODO: 
// Auto update from 13th floor website 
// 
// 

#include "../MQ2Plugin.h" 

PreSetup("MQ2LinkDB"); 
PLUGIN_VERSION(2.3); 
#define MY_STRING    "MQ2LinkDB \ar2.3\ax by Ziggy, modifications by rswiders, updated for CoF" 
#ifdef ISXEQ
#define ISINDEX() (argc>0)
#define ISNUMBER() (IsNumber(argv[0]))
#define GETNUMBER() (atoi(argv[0]))
#define GETFIRST()	argv[0]
#else
#define ISINDEX() (szIndex[0])
#define ISNUMBER() (IsNumber(szIndex))
#define GETNUMBER() (atoi(szIndex))
#define GETFIRST() szIndex
#endif

template <unsigned int _Size>int SearchLinkDB(char** ppOutputArray, CHAR(&searchText)[_Size], BOOL bExact);

// Keep the last 10 results we've done and then cycle through. 
// When I just kept the last one, doing two ${LinkDB[]} calls in one 
// macro link crashed EQ. Well now you can do 10 on one line. If that's 
// not enough, increase the LAST_RESULT_CACHE_SIZE. 

#define LAST_RESULT_CACHE_SIZE 10 
#define NEXT_RESULT_POSITION(x) (x = ((x)+1) % LAST_RESULT_CACHE_SIZE) 
static char g_tloLastResult[LAST_RESULT_CACHE_SIZE][MAX_STRING]; 
static int g_iLastResultPosition = 0; 

static int ConvertItemsDotTxt (void); 

static bool bReplyMode = false;
static bool bQuietMode = true;               // Display debug chatter? 
static int iAddedThisSession = 0;            // How many new links found since startup 
static int iTotalInDB = 0;                   // Number of links in db 
static bool bKnowTotal = false;              // Do we know how many links in db? 
static int iMaxResults = 10;                 // Display at most this many results 
static int iFindItemID = 0;					 // Item ID to /link
static bool bScanChat = true;                // Scan incoming chat for links 
static bool bClickLinks = false;			 // click on link generated?

#define START_LINKTEXT 0x33					 // starting position of link text
#define MAX_PRESENT 150000 
static unsigned char * abPresent = NULL;     // Bitfield to say yes/no we have/don't have each item id 

#define MAX_INTERNAL_RESULTS  500            // Max size of our sort array 
#define SORTEM 

static char cLink[256] = {0};
static int iCurrentID = 0;
static int iNextID = 0;

class MQ2LinkType *pLinkType = 0; 

class MQ2LinkType : public MQ2Type 
{ 
public: 
   enum LinkMembers { 
      Link=1, 
      CurrentID=2, 
      NextID=3, 
   }; 

   MQ2LinkType():MQ2Type("linkdb") 
   { 
      TypeMember(Link); 
      TypeMember(CurrentID); 
      TypeMember(NextID); 
   } 

   ~MQ2LinkType() 
   { 
   } 

   bool MQ2LinkType::GETMEMBER()
   { 
      PMQ2TYPEMEMBER pMember=MQ2LinkType::FindMember(Member); 
      if (!pMember) 
         return false; 
      switch((LinkMembers)pMember->ID) 
      { 
      case Link: 
         strcpy_s(DataTypeTemp,cLink); 
         Dest.Ptr=DataTypeTemp; 
         Dest.Type=pStringType; 
         return true; 
      case CurrentID: 
         Dest.DWord=iCurrentID; 
         Dest.Type=pIntType; 
         return true; 
      case NextID: 
         Dest.DWord=iNextID; 
         Dest.Type=pIntType; 
         return true; 
      } 
      return false; 
   } 

   bool ToString(MQ2VARPTR VarPtr, PCHAR Destination) 
   { 
	  strcpy_s(Destination,MAX_STRING,cLink); 
      return true; 
   } 

   bool FromData(MQ2VARPTR &VarPtr, MQ2TYPEVAR &Source) 
   { 
      return false; 
   } 
   bool FromString(MQ2VARPTR &VarPtr, PCHAR Source) 
   { 
      return false; 
   } 
}; 

BOOL dataLinkDB(PCHAR szIndex, MQ2TYPEVAR &Ret) 
{ 
   if (!ISINDEX())
   {
	  Ret.DWord=0; 
      Ret.Type=pLinkType; 
      return true; 
   }

   iFindItemID = 0;
   PCHAR pItemName = GETFIRST(); 
   BOOL bExact = false; 

   if (*pItemName == '=') 
   { 
      bExact = true; 
      pItemName++; 
   } 
   CHAR szCmd[MAX_STRING] = { 0 };
   strcpy_s(szCmd, pItemName);

   char** ppMatches = new char*[MAX_INTERNAL_RESULTS]; 
   memset (ppMatches, 0, sizeof(char *) *MAX_INTERNAL_RESULTS); 

   int iFound = SearchLinkDB(ppMatches, szCmd, bExact); 
   BOOL bReturnFilled = false; 

   for (int i = 0; i < iFound; i++) 
   { 
      if (! bReturnFilled) 
      { 
         strcpy_s(g_tloLastResult[g_iLastResultPosition], ppMatches[i]); 
         bReturnFilled = true; 
      } 

      free (ppMatches[i]); 
      ppMatches[i] = NULL; 
   } 

   delete (ppMatches); 

   if (!bReturnFilled)
	   g_tloLastResult[g_iLastResultPosition][0] = '\0';
   Ret.Ptr = g_tloLastResult[g_iLastResultPosition]; 
   if (bReturnFilled)
      NEXT_RESULT_POSITION(g_iLastResultPosition); 

   Ret.Type = pStringType;
   return true;
} 

// 
// static void SaveSettings (void) 
// 
static void SaveSettings (void) 
{ 
   char cFilename[MAX_PATH]; 
   sprintf_s(cFilename,"%s\\MQ2LinkDB.ini", gszINIPath); 

   char cNumber[16] = { 0 };
   _itoa_s(iMaxResults, cNumber, 10); 

   WritePrivateProfileString("Settings", "MaxResults", cNumber, cFilename); 
   WritePrivateProfileString("Settings", "ScanChat", bScanChat ? "1" : "0", cFilename); 
   WritePrivateProfileString("Settings", "ClickLinks", bClickLinks ? "1" : "0", cFilename); 
} 

// 
// static void LoadSettings (void) 
// 
static void LoadSettings (void) 
{ 
   char cFilename[MAX_PATH]; 
   sprintf_s(cFilename,"%s\\MQ2LinkDB.ini", gszINIPath); 

   iMaxResults = GetPrivateProfileInt ("Settings", "MaxResults", 10, cFilename); 
   if (iMaxResults < 1) iMaxResults = 1; 

   int iScanChat = GetPrivateProfileInt("Settings", "ScanChat", 1, cFilename); 
   bScanChat = iScanChat > 0; 

   int iClickLinks = GetPrivateProfileInt("Settings", "ClickLinks", 0, cFilename); 
   bClickLinks = iClickLinks > 0; 
} 

// 
// static int ItemID (const char * cLink) 
// 
static int ItemID (const char * cLink) 
{ 
   char cMid[6]; 

   // Skip \x12 and first number 
   memcpy (cMid, cLink + 2, 5); 
   cMid[5] = '\0'; 

   return (int) (strtol (cMid, NULL, 16)); 
} 

// 
// void CreateIndex (void) 
// 
void CreateIndex (void) 
{ 
   if (abPresent != NULL) { 
      return; 
   } 

   // TODO: Make this dynamic size 
   abPresent = new unsigned char[MAX_PRESENT / 8 + 1]; 
   memset (abPresent, 0, (MAX_PRESENT / 8) + 1); 

   char cFilename[MAX_PATH]; 
   sprintf_s(cFilename,"%s\\MQ2LinkDB.txt", gszINIPath); 
   FILE * File = 0;
   errno_t err = fopen_s(&File,cFilename, "rt");
   iTotalInDB = 0; 
   bKnowTotal = true; 

   if (!err) { 
	   char cLine[1024] = { 0 };
      while (fgets (cLine, 1024, File) != NULL) { 
         int iItemID = ItemID (cLine); 

         if (iItemID >= MAX_PRESENT) { 
            WriteChatf ("MQ2LinkDB: ERROR! Make max index size bigger. (Max: %d, ItemID: %d)", MAX_PRESENT, iItemID); 
            continue; 
         } 

         unsigned char cOR = 1 << (iItemID % 8);
		 int insert = iItemID / 8;
		 if (insert < (MAX_PRESENT / 8)) {
			 abPresent[insert] |= cOR;
		 }

         iTotalInDB++; 
      } 
      fclose (File); 
   }
} 

// 
// static bool IsAuged (const char * cLink) 
// Make sure no augs in the link 
// 
static bool IsAuged (const char * cLink) 
{ 
   char cMid[6]; 
   for (int i = 0; i < 5; i++) { 
      memcpy (cMid, cLink + 7 + (i * 5), 5); 
      cMid[5] = '\0'; 
      if (atoi (cMid) != 0) { 
         return (true); 
      } 
   } 

   return (false); 
} 

// 
// static bool FindLink (const char * cLink) 
// 
static bool FindLink (const char * cLink) 
{ 
   int iItemID = ItemID (cLink); 

   if (abPresent != NULL) { 
      unsigned char cOR = 1 << (iItemID % 8); 
	  int insert = iItemID / 8;
	  if (insert < (MAX_PRESENT / 8)) {
		  if ((abPresent[insert] & cOR) != 0) {
			  if (!bQuietMode) {
				  WriteChatf("MQ2LinkDB: Saw link \ay%d\ax, but we already have it.", iItemID);
			  }
			  return (true);
		  }
	  }
   } 


   // Since we're scanning the file anyway, we'll make the index here to save some time, and to 
   // account for ppl creating new MQ2LinkDB.txt files or whatever. 
   if (abPresent == NULL) { 
      abPresent = new unsigned char[MAX_PRESENT / 8 + 1]; 
   } 
   memset (abPresent, 0, MAX_PRESENT / 8 + 1); 

   bool bRet = false; 
   char cFilename[MAX_PATH]; 
   sprintf_s(cFilename,"%s\\MQ2LinkDB.txt", gszINIPath); 

   FILE * File = 0;
   errno_t err = fopen_s(&File,cFilename, "rt");
   if (!err) { 
		char cLine[1024] = { 0 };
      while (fgets (cLine, 1024, File) != NULL) { 
         cLine[strlen (cLine) - 1] = '\0';   // No LF pls 

		 if (ItemID(cLine) == iItemID) {
			 unsigned char cOR = 1 << (iItemID % 8);
			 int insert = iItemID / 8;
			 if (insert < (MAX_PRESENT / 8)) {
				 abPresent[insert] |= cOR;
			 }

            bRet = true; 

            if (IsAuged (cLine) && !IsAuged (cLink)) { 
               if (strlen (cLine) == strlen (cLink)) { 
                  long lPos = ftell (File); 
                  fclose (File); 

                  if (!bQuietMode) { 
                     WriteChatf ("MQ2LinkDB: Replacing auged link with un-auged link for item \ay%d\ax", iItemID); 
                  } 

				  FILE *File2 = 0;
				  err = fopen_s(&File2,cFilename, "r+");
                  if (!err) { 
                     fseek (File2, lPos - strlen (cLine) - 2, SEEK_SET); 

                     // Double check position - paranoia! 
                     char cTemp[10]; 
                     fread (cTemp, 10, 1, File2); 
                     if (memcmp (cTemp, cLink, 8) == 0) { 
                        fseek (File2, lPos - strlen (cLine) - 2, SEEK_SET); // Seek same place again 
                        fwrite (cLink, strlen (cLink), 1, File2); 
                     } else { 
                        if (!bQuietMode) { 
                           WriteChatf ("MQ2LinkDB: \arERROR - Sanity check failed while replacing"); 
                        } 
                     } 

                     fclose (File2);
                  } else { 
                     if (!bQuietMode) { 
                        WriteChatf ("MQ2LinkDB: \arERROR - Could not open db file for writing (%d)", errno); 
                     } 
                  } 

                  return (true); 
               } 
            } else { 
               if (!bQuietMode) { 
                  WriteChatf ("MQ2LinkDB: Saw link \ay%d\ax, but we already have it.", iItemID); 
               } 
            } 
         } 
      } 

      fclose (File); 
   } 

   return (bRet); 
} 

// 
// static void StoreLink (const char * cLink) 
// 
static void StoreLink (const char * cLink) 
{ 
   char cFilename[MAX_PATH]; 
   sprintf_s(cFilename,"%s\\MQ2LinkDB.txt", gszINIPath); 

   FILE * File = 0;
   errno_t err = fopen_s(&File,cFilename, "at");
   if (!err) { 
      fputs (cLink, File); 
      fputs ("\n", File); 
      iAddedThisSession++; 
      iTotalInDB++; 

      CreateIndex ();         // Won't create if it's already there 
      if (abPresent != NULL) { 
         int iItemID = ItemID (cLink); 
         unsigned char cOR = 1 << (iItemID % 8);
		 int insert = iItemID / 8;
		 if (insert < (MAX_PRESENT / 8)) {
			 abPresent[insert] |= cOR;
		 }
      } 

      if (!bQuietMode) { 
         WriteChatf ("MQ2LinkDB: Stored link for item ID: \ay%d\ax (\ay%d\ax stored this session)", ItemID (cLink), iAddedThisSession); 
      } 

      fclose (File); 
   } else { 
      if (!bQuietMode) { 
         WriteChatf ("MQ2LinkDB: \arERROR - Could not open db file for writing (%d)", errno); 
      } 
   } 
} 

// 
// static char * LinkExtract (char * cLink) 
// 
static char * LinkExtract (char * cLink) 
{ 
	if (char * cTemp = _strdup(cLink)) {
		char * cEnd = strchr(cTemp + START_LINKTEXT, '\x12');
		int iLen = 1;

		if (cEnd != NULL) {
			*(cEnd + 1) = '\0';
			iLen = strlen(cTemp);

			//WriteChatf ("MQ2LinkDB: Chat - %s", cTemp + 1); 

			if (!FindLink(cTemp)) {
				StoreLink(cTemp);
			}
		}
		free(cTemp);
		return (cLink + iLen);
	}
	return 0;
} 

// 
// static void ChatTell(PSPAWNINFO pChar, char *cLine) 
// 
static void ChatTell(PSPAWNINFO pChar, char *cLine) 
{ 
   DebugSpew("MQ2LinkDB::ChatTell(%s)",cLine); 
   //WriteChatf("MQ2LinkDB::ChatTell(%s, %s)",pChar->Name,cLine); 

   char cTemp[1024]; 
   if (!bReplyMode) {
       sprintf_s(cTemp, "Linkdb told you, '%s'", cLine); 
       dsp_chat_no_events(cTemp, USERCOLOR_TELL, false);
	} else {
       //sprintf_s(cTemp, ";tell %s %s", pChar->Name, cLine);
       sprintf_s(cTemp, "/tell %s %s", pChar->Name, cLine);
       //WriteChatf("MQ2LinkDB::DoCommand(%s)", cTemp);
	   //pEverQuest->send_tell(pChar->Name,cLine);
	   //DoCommand(pChar,cTemp);
       //dsp_chat_no_events(cTemp, USERCOLOR_TELL, false); 
    }
} 

// 
// static void DoParameters (PCHAR cParams) 
// 
template <unsigned int _Size>static void DoParameters (CHAR(&cParams)[_Size]) 
{ 
   bool bAnyParams = false; 
   _strlwr_s(cParams);
    char * cWord = NULL;
	char *next_token1 = NULL;

	cWord = strtok_s(cParams, " ", &next_token1);
	//cWord = strtok_s(NULL, "'", &next_token1);
   while (cWord != NULL) { 
      if (strcmp (cWord, "/quiet") == 0) { 
         bQuietMode = !bQuietMode; 

         WriteChatf ("MQ2LinkDB: Quiet mode \ay%s\ax", bQuietMode ? "on" : "off"); 
         bAnyParams = true; 

      } else if (strcmp (cWord, "/max") == 0) { 
         cWord = strtok_s(NULL, " ", &next_token1);
         if (atoi (cWord) > 0) { 
            iMaxResults = atoi (cWord); 
            WriteChatf ("MQ2LinkDB: Max results now \ay%d\ax", iMaxResults); 
            SaveSettings (); 
         } 
         bAnyParams = true; 
	  } else if (strcmp(cWord, "/item") == 0) {
		  cWord = strtok_s(NULL, " ", &next_token1);
		  if (atoi(cWord) > 0) {
			  iFindItemID = atoi(cWord);
		  }
		  bAnyParams = true;
	  }
	  else if (strcmp(cWord, "/click") == 0) {
         cWord = strtok_s(NULL, " ", &next_token1);

         if (_stricmp(cWord, "on") == 0 || 
            _stricmp(cWord, "yes") == 0 || 
            _stricmp(cWord, "true") == 0 || 
            _stricmp(cWord, "1") == 0) { 
               bClickLinks = true; 
               WriteChatf ("MQ2LinkDB: Will auto-click exact match links it generates.");    
            } else { 
               bClickLinks = false; 
               WriteChatf ("MQ2LinkDB: Will not auto-click exact match links it generates."); 
            } 

            SaveSettings (); 

            bAnyParams = true; 
      } else if (strcmp (cWord, "/scan") == 0) { 
         cWord = strtok_s(NULL, " ", &next_token1);

         if (_stricmp(cWord, "on") == 0 || 
            _stricmp(cWord, "yes") == 0 || 
            _stricmp(cWord, "true") == 0 || 
            _stricmp(cWord, "1") == 0) { 
               bScanChat = true; 
               WriteChatf ("MQ2LinkDB: Will scan incoming chat for item links.", iMaxResults);    
            } else { 
               bScanChat = false; 
               WriteChatf ("MQ2LinkDB: Will not scan incoming chat for item links.", iMaxResults); 
            } 

            SaveSettings (); 

            bAnyParams = true; 
      } else if (strcmp (cWord, "/import") == 0) { 
         ConvertItemsDotTxt (); 
         bAnyParams = true; 
      } 

      cWord = strtok_s(NULL, " ", &next_token1);
   } 

   if (!bAnyParams) { 
      WriteChatf ("%s",MY_STRING); 
	  WriteChatf ("MQ2LinkDB: Syntax: \ay/link [/max n] [/scan on|off] [/click on|off] [/item #][search string]\ax"); 
      if (bKnowTotal) { 
         WriteChatf ("MQ2LinkDB: \ay%d\ax links in database, \ay%d\ax links added this session", iTotalInDB, iAddedThisSession); 
      } else { 
         WriteChatf ("MQ2LinkDB: \ay%d\ax links added this session", iAddedThisSession); 
      } 
      WriteChatf ("MQ2LinkDB: Max Results \ay%d\ax", iMaxResults); 
      if (bScanChat) { 
         WriteChatf ("MQ2LinkDB: Scanning incoming chat for item links"); 
      } else { 
         WriteChatf ("MQ2LinkDB: Not scanning incoming chat"); 
      } 
      if (bClickLinks) { 
         WriteChatf ("MQ2LinkDB: Auto-clicking links on exact matches"); 
      } else { 
         WriteChatf ("MQ2LinkDB: Not auto-clicking links on exact matches"); 
      } 
   } 
} 

// This routine will authorize a user for the /link and /recipe commands...
bool AuthorizedUser(PCHAR szName)
{
	DebugSpew("MQ2LinkDB::AuthorizedUser(%s)",szName);
	//WriteChatf("MQ2LinkDB::AuthorizedUser(%s)",szName);

	char szTemp[MAX_STRING] = {0};
	DWORD Result = GetPrivateProfileString("Users",szName,"off",szTemp,MAX_STRING,INIFileName); 
	//WriteChatf("MQLinkDB(%d) User(%s)=%s in file '%s'", Result, szName, szTemp, INIFileName); return TRUE;
	return (_strnicmp(szTemp,"on",3)==0);
}

// This routine will send a link click to EQ to retrieve the item data
VOID ClickLink (PSPAWNINFO pChar, PCHAR szLink)
{
	DebugSpew("MQ2LinkDB::ClickLink(%s)",szLink);
	//WriteChatf("MQ2LinkDB::ClickLink(%s)",szLink);

	char szCommand[MAX_STRING] = {0};
	char szLinkStruct[MAX_STRING] = {0};
	strncpy_s(szLinkStruct,szLink+2,START_LINKTEXT-2);

	sprintf_s(szCommand, "/notify ChatWindow CW_ChatOutput link %s", szLinkStruct);
	DoCommand(pChar, szCommand);
}

// 
// Do the actual local file search. This searches the local name->link
// data file line by line returning matches in the pre-alloced array
// passed in. That memory is yours, not mine sucker. Be kind.
template <unsigned int _Size>int SearchLinkDB(char** ppOutputArray, CHAR(&searchText)[_Size], BOOL bExact)
{
    char** ppCurrent = ppOutputArray;

    if (abPresent == NULL) { 
        abPresent = new unsigned char[MAX_PRESENT / 8 + 1];
    }
    memset (abPresent, 0, MAX_PRESENT / 8 + 1);

	iNextID = 0; iCurrentID = 0;
    int iFound = 0, iTotal = 0;
    char cFilename[MAX_PATH];
    sprintf_s(cFilename,"%s\\MQ2LinkDB.txt", gszINIPath);
	FILE * File = 0;
	errno_t err = fopen_s(&File,cFilename, "rt");

	if (!err) {
		if (!bQuietMode) { 
			WriteChatf ("MQ2LinkDB: Searching for '\ay%s\ax'...", searchText);
		}
        _strlwr_s (searchText);
		char cLine[256] = { 0 };
		char cCopy[256] = { 0 };
		bool bNextID = false;

        while (fgets (cLine, sizeof (cLine), File) != NULL)
        {
            int iItemID = ItemID (cLine);
			if (bNextID) {
				bNextID = false;
				iNextID = iItemID;
			}
            unsigned char cOR = 1 << (iItemID % 8);
			int insert = iItemID / 8;
			if (insert < (MAX_PRESENT / 8)) {
				abPresent[insert] |= cOR;
			}

            cLine[strlen (cLine) - 1] = '\0';   // No LF pls
            strcpy_s(cCopy, cLine + START_LINKTEXT);
            _strlwr_s(cCopy);

            if ((iItemID == iFindItemID) || (bExact && _strnicmp(cCopy, searchText, strlen(cCopy) - 1) == 0) ||
                (!bExact && strstr(cCopy, searchText) != NULL)) {
                if (iFound < MAX_INTERNAL_RESULTS) {
					
                    *ppCurrent = _strdup(cLine);
                    ppCurrent++;
                }
	            if (iFindItemID || (strlen (cLine + START_LINKTEXT + 1) == strlen (searchText) && _memicmp (cLine + START_LINKTEXT, searchText, strlen (searchText)) == 0)) { 
					bNextID = true;
				}

                iFound++;
            }

            iTotal++;
        }

        bKnowTotal = true;
        iTotalInDB = iTotal;

        fclose (File);
	} else {
        WriteChatf ("MQ2LinkDB: No item database yet");
    }

    return iFound;
}

static VOID CommandLink(PSPAWNINFO pChar, PCHAR szLine) 
{ 
	CHAR szCmd[MAX_STRING] = { 0 };
	strcpy_s(szCmd, szLine);
	iFindItemID = 0;
	bRunNextCommand = TRUE;
	if (strlen (szCmd) < 3) {
		CHAR szEmpty[MAX_STRING] = { 0 };
		DoParameters(szEmpty); 
		return;       // We don't list entire DB. that's just not funny 
	} 

	if (szCmd[0] == '/' || szCmd[0] == '-') { 
		DoParameters (szCmd); 
		if (!iFindItemID)
			return; 
	} 

	char ** cArray = new char * [MAX_INTERNAL_RESULTS]; 
	memset (cArray, 0, sizeof (char *) * MAX_INTERNAL_RESULTS); 

	int iFound = SearchLinkDB(cArray, szCmd, false); 

	bool bForcedExtra = false; 
	int iMaxLoop = (iFound > MAX_INTERNAL_RESULTS ? MAX_INTERNAL_RESULTS : iFound); 
	if (iFound > 0) { 
		// Sort the list 
		for (int j = 0; j < iMaxLoop; j++) { 
			for (int i = 0; i < iMaxLoop - 1; i++) { 
				if (strcmp (cArray[i] + START_LINKTEXT, cArray[i + 1] + START_LINKTEXT) > 0) { 
					char * cTemp = cArray[i]; 
					cArray[i] = cArray[i + 1]; 
					cArray[i + 1] = cTemp; 
				} 
			} 
		} 

		// Show list 
		for (int i = 0; i < iMaxLoop; i++) { 
			bool bShow = i < iMaxResults; 
			char cTemp[256] = { 0 };
            strcpy_s(cTemp, cArray[i]); 

            if (IsAuged (cArray[i])) { 
               strcat_s(cTemp, " (Augmented)"); 
            } 

            if (iFindItemID || (strlen (cArray[i] + START_LINKTEXT + 1) == strlen(szCmd) && _memicmp(cArray[i] + START_LINKTEXT, szCmd, strlen(szCmd)) == 0)) { 
			   strcpy_s(cLink, cArray[i]);
			   strcat_s(cTemp, " <Exact Match>"); 
               bShow = true;        // We display this result even if we've already shown iMaxResults count 
               bForcedExtra = i >= iMaxResults; 
               if (bClickLinks && !bReplyMode) ClickLink(pChar, cArray[i]);
            } 

            if (bShow) { 
               ChatTell(pChar, cTemp); 
            } 

            free (cArray[i]); 
            cArray[i] = NULL; 
		} 
	} 

	delete (cArray); 

	char cTemp3[128] = { 0 };
	char cTemp[128] = { 0 };
	sprintf_s(cTemp3, "MQ2LinkDB: Found \ay%d\ax items from database of \ay%d\ax total items", iFound, iTotalInDB); 
	sprintf_s(cTemp, "Found %d items from database of %d total items", iFound, iTotalInDB); 

	if (iFound > iMaxResults) { 
		char cTemp2[64]; 
		sprintf_s(cTemp2, " - \arList capped to \ay%d\ar results", iMaxResults); 
		strcat_s(cTemp3, cTemp2); 

		sprintf_s(cTemp2, " - List capped to %d results", iMaxResults); 
		strcat_s(cTemp, cTemp2); 

		if (bForcedExtra) { 
			strcat_s(cTemp, " plus exact match"); 
			strcat_s(cTemp3, " plus exact match"); 
		} 
	} 

	if (!bQuietMode) { 
		WriteChatf("%s",cTemp3); 
	}
	ChatTell(pChar, cTemp); 
	bReplyMode = false;
} 

// Called once, when the plugin is to initialize 
PLUGIN_API VOID InitializePlugin(VOID) 
{ 
   DebugSpewAlways("Initializing MQ2LinkDB"); 
   AddCommand("/link",CommandLink); 
   AddMQ2Data("LinkDB",dataLinkDB); 

   pLinkType = new MQ2LinkType; 

   LoadSettings (); 

   abPresent = NULL; 
} 

// Called once, when the plugin is to shutdown 
PLUGIN_API VOID ShutdownPlugin(VOID) 
{ 
   DebugSpewAlways("Shutting down MQ2LinkDB"); 
   RemoveCommand("/link"); 
   RemoveMQ2Data("LinkDB"); 

   delete pLinkType; 

   free (abPresent); 
   abPresent = NULL; 
} 
/*1 a.ITEM_NUMBER
2 a.NAME
3 a.LORE_DESCRIPTION
4 a.ADVANCED_LORE_TEXT_FILENAME
5 a.TYPE
6 a.VALUE
7 CONCAT('IT',a.ACTOR_TAG)
8 a.IMAGE_NUMBER
9 a.SIZE_ITEM
10 a.WEIGHT
11 a.MAX_ITEM_COUNT
12 a.ITEM_CLASS
13 a.IS_LORE
14 a.IS_ARTIFACT
15 a.IS_SUMMONED
16 a.REQUIRED_LEVEL
17 a.RECOMMENDED_LEVEL
18 a.RECOMMENDED_SKILL
19 a.NO_DROP
20 a.RENTABLE
21 a.NO_ALT_TRANSFER
22 a.FV_NO_DROP
23 a.NO_PET_EQUIP
24 a.CHARM
25 a.EARS
26 a.HEAD
27 a.FACE
28 a.NECK
29 a.SHOULDERS
30 a.ARMS
31 a.BACK
32 a.WRISTS
33 a.RANGE
34 a.HANDS
35 a.PRIMARY
36 a.SECONDARY
37 a.FINGERS
38 a.CHEST
39 a.LEGS
40 a.FEET
41 a.WAIST
42 a.POWER_SLOT
43 a.AMMO
44 a.WARRIOR_USEABLE
45 a.CLERIC_USEABLE
46 a.PALADIN_USEABLE
47 a.RANGER_USEABLE
48 a.SHADOWKNIGHT_USEABLE
49 a.DRUID_USEABLE
50 a.MONK_USEABLE
51 a.BARD_USEABLE
52 a.ROGUE_USEABLE
53 a.SHAMAN_USEABLE
54 a.NECROMANCER_USEABLE
55 a.WIZARD_USEABLE
56 a.MAGICIAN_USEABLE
57 a.ENCHANTER_USEABLE
58 a.BEASTLORD_USEABLE
59 a.BERSERKER_USEABLE
60 a.MERCENARY_USABLE
61 a.HUMAN_USEABLE
62 a.BARBARIAN_USEABLE
63 a.ERUDITE_USEABLE
64 a.WOODELF_USEABLE
65 a.HIGHELF_USEABLE
66 a.DARKELF_USEABLE
67 a.HALFELF_USEABLE
68 a.DWARF_USEABLE
69 a.TROLL_USEABLE
70 a.OGRE_USEABLE
71 a.HALFLING_USEABLE
72 a.GNOME_USEABLE
73 a.IKSAR_USEABLE
74 a.VAHSHIR_USEABLE
75 a.FROGLOK_USEABLE
76 a.DRAKKIN_USEABLE
77 a.TEMPLATE_USEABLE
78 a.REQ_AGNOSTIC
79 a.REQ_BERTOXXULOUS
80 a.REQ_BRELLSERILIS
81 a.REQ_CAZICTHULE
82 a.REQ_EROLLISIMARR
83 a.REQ_FIZZLETHORP
84 a.REQ_INNORUUK
85 a.REQ_KARANA
86 a.REQ_MITHMARR
87 a.REQ_PREXUS
88 a.REQ_QUELLIOUS
89 a.REQ_RALLOSZEK
90 a.REQ_RODCETNIFE
91 a.REQ_SOLUSEKRO
92 a.REQ_TRIBUNAL
93 a.REQ_TUNARE
94 a.REQ_VEESHAN
95 a.MODFACTION1_NUM
96 a.MODFACTION1_VALUE
97 a.MODFACTION2_NUM
98 a.MODFACTION2_VALUE
99 a.MODFACTION3_NUM
100 a.MODFACTION3_VALUE
101 a.MODFACTION4_NUM
102 a.MODFACTION4_VALUE
103 a.GEM_SIZE
104 a.SOCKET_CLASS
105 a.SOLVENT_ITEM_ID
106 a.POINT_TYPE
107 a.POINT_THEME_BIT_MASK
108 a.POINT_COST
109 a.POINT_BUY_BACK_PERCENT
110 a.ADVENTURE_ESTEEM_NEEDED
111 a.FOOD_DURATION
112 a.LIGHT_TYPE
113 a.MAGIC
114 a.SKIN_TYPE
115 a.ARMOR_VARIANT
116 a.ARMOR_MAT
117 a.R_TINT
118 a.G_TINT
119 a.B_TINT
120 a.ANIMATION_OVERRIDE
121 a.TINT_PALETTE_INDEX
122 a.MERCHANT_MULTIPLIER
123 a.LOG
124 a.LOOT_LOG
125 a.REQ_AVATAR
126 a.SKILL_MOD
127 a.SKILL_BONUS
128 a.SPECIAL_SKILL_CAP
129 a.POOF_ON_DEATH
130 a.INSTRUMENT_TYPE
131 a.INSTRUMENT_PERCENTAGE_MOD
132 a.SCRIPTID
133 a.SCRIPT_FILE_NAME
134 a.TRADESKILL
135 a.TRIBUTE_VALUE
136 a.GUILD_TRIBUTE_VALUE
137 a.HIGH_PROFILE
138 a.IS_POTION_BELT_ALLOWED
139 a.NUM_POTION_SLOTS
140 a.CAN_USE_FILTER_ID
141 a.RIGHT_CLICK_SCRIPT_ID
142 a.IS_QUEST_ITEM
143 a.NO_ENDLESS_QUIVER
144 a.POWER_CHARGES
145 a.POWER_PURITY
146 a.IS_EPIC_1
147 a.IS_EPIC_15
148 a.IS_EPIC_2
149 b.STR_MOD
150 b.INT_MOD
151 b.WIS_MOD
152 b.AGI_MOD
153 b.DEX_MOD
154 b.STA_MOD
155 b.CHA_MOD
156 b.HP_MOD
157 b.MANA_MOD
158 b.AC_MOD
159 b.ENDURANCE_MOD
160 b.SAVE_MAGIC_MOD
161 b.SAVE_FIRE_MOD
162 b.SAVE_COLD_MOD
163 b.SAVE_DISEASE_MOD
164 b.SAVE_POISON_MOD
165 b.SAVE_CORRUPTION_MOD
166 c.HASTE
167 c.ATTACK
168 c.HP_REGEN
169 c.MANA_REGEN
170 c.ENDURANCE_REGEN
171 c.DAMAGE_SHIELD
172 c.COMBAT_EFFECTS
173 c.SHIELDING
174 c.SPELL_SHIELDING
175 c.AVOIDANCE
176 c.ACCURACY
177 c.STUN_RESIST
178 c.STRIKETHROUGH
179 c.SKILL_DAMAGE_NUM
180 c.SKILL_DAMAGE_MOD
181 c.DOT_SHIELDING
182 d.DELAY
183 d.BASE_DAMAGE
184 d.ITEM_RANGE
185 d.BANE_RACE
186 d.BANE_BODYTYPE
187 d.RACE_BANE_DAMAGE
188 d.BODYTYPE_BANE_DAMAGE
189 d.ELEMENT_CRIT
190 d.ELEMENT_DAMAGE
191 e.CONTAINER_TYPE
192 e.CONTAINER_CAPACITY
193 e.CONTAINER_ITEM_SIZE_LIMIT
194 e.CONTAINER_WEIGHT_RDX
195 f.NOTE_TYPE
196 f.NOTE_LANGUAGE
197 f.NOTE_TEXTFILE
198 d.BACKSTAB_DAMAGE
199 c.DAMAGE_SHIELD_MITIGATION
200 b.HEROIC_STR_MOD
201 b.HEROIC_INT_MOD
202 b.HEROIC_WIS_MOD
203 b.HEROIC_AGI_MOD
204 b.HEROIC_DEX_MOD
205 b.HEROIC_STA_MOD
206 b.HEROIC_CHA_MOD
207 b.HEROIC_SAVE_MAGIC_MOD
208 b.HEROIC_SAVE_FIRE_MOD
209 b.HEROIC_SAVE_COLD_MOD
210 b.HEROIC_SAVE_DISEASE_MOD
211 b.HEROIC_SAVE_POISON_MOD
212 b.HEROIC_SAVE_CORRUPTION_MOD
213 c.HEAL_AMOUNT
214 c.SPELL_DAMAGE
215 c.CLAIRVOYANCE
216 a.SUB_CLASS
217 a.LOGIN_REGISTRATION_REQUIRED
218 a.LAUNCH_SCRIPT_ID
219 a.HEIRLOOM
220 g.EQG_ID
221 g.PLACEMENT_FLAGS
222 g.IGNORE_COLLISIONS
223 g.PLACEMENT_TYPE
224 g.REAL_ESTATE_DEF_ID
225 g.PLACEABLE_SCALE_RANGE_MIN
226 g.PLACEABLE_SCALE_RANGE_MAX
227 g.REAL_ESTATE_UPKEEP_ID
228 g.MAX_PER_REAL_ESTATE
229 g.NPC_FILENAME
230 g.TROPHY_BENEFIT_ID
231 g.DISABLE_PLACEMENT_ROTATION
232 g.DISABLE_FREE_PLACEMENT
233 g.NPC_RESPAWN_INTERVAL
234 g.PLACEABLE_SCALE_DEFAULT
235 g.PLACEABLE_ORIENTATION_HEADING
236 g.PLACEABLE_ORIENTATION_PITCH
237 g.PLACEABLE_ORIENTATION_ROLL
238 a.NO_BANK
239 CONCAT('IT',a.SECONDARY_ACTOR_TAG)
240 g.IS_INTERACTIVE_OBJECT
241 a.SKILL_MOD_OFFSET
242 a.SOCKET_SUB_CLASSES
243 a.NEW_ARMOR_ID
244 a.ITEM_RANK
245 a.SOCKET_SKIN_TYPE_MASK
246 a.IS_COLLECTIBLE
247 a.NO_DESTROY
248 a.NO_NPC
249 a.NO_ZONE
250 a.CREATOR_ID
251 a.NO_GROUND
252 a.NO_LOOT

*/
// This is called every time EQ shows a line of chat with CEverQuest::dsp_chat, 
// but after MQ filters and chat events are taken care of. 
PLUGIN_API DWORD OnIncomingChat(PCHAR Line, DWORD Color) 
{ 
   //DebugSpew("MQ2LinkDB::OnIncomingChat(%s)",Line); 

   if (strstr(Line,"tells you,") || strstr(Line,"told you,")) { 

	   char * cTemp = _strdup (Line);
	   char * cRequest = NULL;
	   char *next_token1 = NULL;

	   cRequest = strtok_s(cTemp, "'", &next_token1);
	   cRequest = strtok_s(NULL, "'", &next_token1);
	   if (cRequest == NULL) {
		   free(cTemp);
		   return 0;
	   }

	   //WriteChatf("MQ2LinkDB:: cRequest = '%s'", cRequest);

	   if (_memicmp(cRequest, "!link ", 6) == 0) {
		   bReplyMode = true;
		   char szName[MAX_STRING] = {0};
           GetArg(szName,Line,1);

		   if (AuthorizedUser(szName)) {
			   PSPAWNINFO pChSpawn=new SPAWNINFO;
               memset(pChSpawn,0,sizeof(SPAWNINFO));
               strncpy_s(pChSpawn->Name,szName,sizeof(pChSpawn->Name));

               CommandLink(pChSpawn, cRequest + 6);
		   }
       } 
       free(cTemp); 
   } 

   if (bScanChat) { 
      //DebugSpew("MQ2LinkDB::OnIncomingChat(%s)",Line); 
      char * cPtr = Line; 
      if(cPtr[0] == '\x12') { 
         // move past the linked character name           
         if ( strchr(&Line[2], ',')) { 
            cPtr = strchr(&Line[2], ','); 
         } else { 
            return 0; 
         } 
      }
	  while (*cPtr) { 
         if (*cPtr == '\x12') { 
            cPtr = LinkExtract (cPtr); 
         } else { 
            cPtr++; 
         } 
      } 
   } 

   return 0; 
} 

// DB Converter now part of plugin 
typedef struct { 
   int  itemclass; 
   char name[64]; 
   char lore[64]; 
   char lorefile[255]; 
   char idfile[16]; 
   unsigned long id; 
   int weight; 
   short norent; 
   short nodrop; 
   short size; 
   short slots; 
   long price; 
   short icon; 
   short UNK013; 
   short UNK014; 
   int benefitflag; 
   short tradeskills; 
   short cr; 
   short dr; 
   short pr; 
   short mr; 
   short fr; 
   short svcorruption; 
   short astr; 
   short asta; 
   short aagi; 
   short adex; 
   short acha; 
   short aint; 
   short awis; 
   short hp; 
   short mana; 
   short endur; 
   short ac; 
   short classes; 
   short races; 
   short deity; 
   short skillmodvalue; 
   short UNK038; 
   short skillmodtype; 
   short banedmgrace; 
   short banedmgbody; 
   short banedmgraceamt; 
   short banedmgamt; 
   short magic; 
   short casttime_; 
   short reqlevel; 
   short reclevel; 
   short recskill; 
   short bardtype; 
   short bardvalue; 
   short light; 
   short delay; 
   short elemdmgtype; 
   short elemdmgamt; 
   short range; 
   short damage; 
   short color; 
   short itemtype; 
   short material; 
   short UNK060; 
   short elitematerial; 
   short sellrate; 
   short combateffects; 
   short shielding; 
   short stunresist; 
   short strikethrough; 
   short extradmgskill; 
   short extradmgamt; 
   short spellshield; 
   short avoidance; 
   short accuracy; 
   short charmfileid; 
   short factionmod1; 
   short factionamt1; 
   short factionmod2; 
   short factionamt2; 
   short factionmod3; 
   short factionamt3; 
   short factionmod4; 
   short factionamt4; 
   short charmfile; 
   short augtype; 
   short augrestrict; 
   short augdistiller; 
   short augslot1type; 
   short augslot1visible; 
   short augslot1unk2; 
   short augslot2type; 
   short augslot2visible; 
   short augslot2unk2; 
   short augslot3type; 
   short augslot3visible; 
   short augslot3unk2; 
   short augslot4type; 
   short augslot4visible; 
   short augslot4unk2; 
   short augslot5type; 
   short augslot5visible; 
   short augslot5unk2; 
   short pointtype; 
   short ldontheme; 
   short ldonprice; 
   short ldonsellbackrate; 
   short ldonsold; 
   short bagtype; 
   short bagslots; 
   short bagsize; 
   short bagwr; 
   short book; 
   short booktype; 
   short filename; 
   short loregroup; 
   short artifactflag; 
   short UNK109; 
   short favor; 
   short guildfavor; 
   short fvnodrop; 
   short dotshielding; 
   short attack; 
   short regen; 
   short manaregen; 
   short enduranceregen; 
   short haste; 
   short damageshield; 
   short UNK120; 
   short UNK121; 
   short attuneable; 
   short nopet; 
   short UNK124; 
   short potionbelt; 
   short potionbeltslots; 
   short stacksize; 
   short notransfer; 
   short scriptfile; 
   short questitemflag; 
   short expendablearrow; 
   char  UNK132[255]; 
   short clickeffect; 
   short clicktype; 
   short clicklevel2; 
   short clicklevel; 
   short maxcharges; 
   short casttime; 
   short recasttype; 
   short recastdelay; 
   short clickunk5; 
   short clickname; 
   short clickunk7; 
   short proceffect; 
   short proctype; 
   short proclevel2; 
   short proclevel; 
   short procunk1; 
   short procunk2; 
   short procunk3; 
   short procunk4; 
   short procrate; 
   short procname; 
   short procunk7; 
   short worneffect; 
   short worntype; 
   short wornlevel2; 
   short wornlevel; 
   short wornunk1; 
   short wornunk2; 
   short wornunk3; 
   short wornunk4; 
   short wornunk5; 
   short wornname; 
   short wornunk7; 
   short focuseffect; 
   short focustype; 
   short focuslevel2; 
   short focuslevel; 
   short focusunk1; 
   short focusunk2; 
   short focusunk3; 
   short focusunk4; 
   short focusunk5; 
   short focusname; 
   short focusunk7; 
   short scrolleffect; 
   short scrolltype; 
   short scrolllevel2; 
   short scrolllevel; 
   short scrollunk1; 
   short scrollunk2; 
   short scrollunk3; 
   short scrollunk4; 
   short scrollunk5; 
   short scrollname; 
   short scrollunk7; 
   short powersourcecapacity; 
   short purity; 
   short dsmitigation;
   short heroic_str;
   short heroic_int;
   short heroic_wis;
   short heroic_agi;
   short heroic_dex;
   short heroic_sta;
   short heroic_cha;
   short healamt;
   short spelldmg;
   short clairvoyance;
   short backstabdmg; //206
   short bardeffect;
   short bardeffecttype;
   short bardlevel2;
   short bardlevel;
   short bardunk1;
   short bardunk2;
   short bardunk3;
   short bardunk4;
   short bardunk5;
   short bardname;
   short bardunk7;
   short UNK214;
   short UNK219;
   short UNK220;
   short UNK221;
   short UNK222;
   short UNK223;
   short UNK224;
   short UNK225;
   short UNK226;
   short UNK227;
   short UNK228;
   short UNK229;
   short UNK230;
   short UNK231;
   short UNK232;
   short UNK233;
   short UNK234;
   short UNK235;
   short UNK236;
   short UNK237;
   short UNK238;
   short UNK239;
   short UNK240;
   short UNK241;
   short UNK242;
   short evolvinglevel;
   short verified; 
   short created;
} EQITEM, * PEQITEM; 

// 
// void SetField (PEQITEM Item, int iField, const char * cField) 
// 
void SetField (PEQITEM Item, int iField, const char * cField) 
{ 
   int iInt = atoi (cField); 
   bool bBool = iInt > 0; 
   switch (iField) { 
      case   0: Item->itemclass = iInt; break; 
      case   1: strcpy_s(Item->name, cField); break; 
      case   2: strcpy_s(Item->lore, cField); break; 
      case   3: strcpy_s(Item->lorefile, cField); break; 
      case   4: strcpy_s(Item->idfile, cField); break; 
      case   5: Item->id = iInt; break; 
      case   6: Item->weight = iInt; break; 
      case   7: Item->norent = iInt; break; 
      case   8: Item->nodrop = iInt; break; 
      case   9: Item->size = iInt; break; 
      case  10: Item->slots = iInt; break; 
      case  11: Item->price = iInt; break; 
      case  12: Item->icon = iInt; break; 
      case  13: Item->UNK013 = iInt; break; 
      case  14: Item->UNK014 = iInt; break; 
      case  15: Item->benefitflag = iInt; break; 
      case  16: Item->tradeskills = bBool; 
      case  17: Item->cr = iInt; break; 
      case  18: Item->dr = iInt; break; 
      case  19: Item->pr = iInt; break; 
      case  20: Item->mr = iInt; break; 
      case  21: Item->fr = iInt; break; 
      case  22: Item->svcorruption = iInt; break; 
      case  23: Item->astr = iInt; break; 
      case  24: Item->asta = iInt; break; 
      case  25: Item->aagi = iInt; break; 
      case  26: Item->adex = iInt; break; 
      case  27: Item->acha = iInt; break; 
      case  28: Item->aint = iInt; break; 
      case  29: Item->awis = iInt; break; 
      case  30: Item->hp = iInt; break; 
      case  31: Item->mana = iInt; break; 
      case  32: Item->endur = iInt; break; 
      case  33: Item->ac = iInt; break; 
      case  34: Item->classes = iInt; break; 
      case  35: Item->races = iInt; break; 
      case  36: Item->deity = iInt; break; 
      case  37: Item->skillmodvalue = iInt; break; 
      case  38: Item->UNK038 = iInt; break; 
      case  39: Item->skillmodtype = iInt; break; 
      case  40: Item->banedmgrace = iInt; break; 
      case  41: Item->banedmgbody = iInt; break; 
      case  42: Item->banedmgraceamt = iInt; break; 
      case  43: Item->banedmgamt = iInt; break; 
      case  44: Item->magic = iInt; break; 
      case  45: Item->casttime_ = iInt; break; 
      case  46: Item->reqlevel = iInt; break; 
      case  47: Item->reclevel = iInt; break; 
      case  48: Item->recskill = iInt; break; 
      case  49: Item->bardtype = iInt; break; 
      case  50: Item->bardvalue = iInt; break; 
      case  51: Item->light = iInt; break; 
      case  52: Item->delay = iInt; break; 
      case  53: Item->elemdmgtype = iInt; break; 
      case  54: Item->elemdmgamt = iInt; break; 
      case  55: Item->range = iInt; break; 
      case  56: Item->damage = iInt; break; 
      case  57: Item->color = iInt; break; 
      case  58: Item->itemtype = iInt; break; 
      case  59: Item->material = iInt; break; 
      case  60: Item->UNK060 = iInt; break; 
      case  61: Item->elitematerial = iInt; break; 
      case  62: Item->sellrate = iInt; break; 
      case  63: Item->combateffects = iInt; break; 
      case  64: Item->shielding = iInt; break; 
      case  65: Item->stunresist = iInt; break; 
      case  66: Item->strikethrough = iInt; break; 
      case  67: Item->extradmgskill = iInt; break; 
      case  68: Item->extradmgamt = iInt; break; 
      case  69: Item->spellshield = iInt; break; 
      case  70: Item->avoidance = iInt; break; 
      case  71: Item->accuracy = iInt; break; 
      case  72: Item->charmfileid = iInt; break; 
      case  73: Item->factionmod1 = iInt; break; 
      case  74: Item->factionamt1 = iInt; break; 
      case  75: Item->factionmod2 = iInt; break; 
      case  76: Item->factionamt2 = iInt; break; 
      case  77: Item->factionmod3 = iInt; break; 
      case  78: Item->factionamt3 = iInt; break; 
      case  79: Item->factionmod4 = iInt; break; 
      case  80: Item->factionamt4 = iInt; break; 
      case  81: Item->charmfile = iInt; break; 
      case  82: Item->augtype = iInt; break; 
      case  83: Item->augrestrict = iInt; break; 
      case  84: Item->augdistiller = iInt; break; 
      case  85: Item->augslot1type = iInt; break; 
      case  86: Item->augslot1visible = iInt; break; 
      case  87: Item->augslot1unk2 = iInt; break; 
      case  88: Item->augslot2type = iInt; break; 
      case  89: Item->augslot2visible = iInt; break; 
      case  90: Item->augslot2unk2 = iInt; break; 
      case  91: Item->augslot3type = iInt; break; 
      case  92: Item->augslot3visible = iInt; break; 
      case  93: Item->augslot3unk2 = iInt; break; 
      case  94: Item->augslot4type = iInt; break; 
      case  95: Item->augslot4visible = iInt; break; 
      case  96: Item->augslot4unk2 = iInt; break; 
      case  97: Item->augslot5type = iInt; break; 
      case  98: Item->augslot5visible = iInt; break; 
      case  99: Item->augslot5unk2 = iInt; break; 
      case 100: Item->pointtype = iInt; break; 
      case 101: Item->ldontheme = iInt; break; 
      case 102: Item->ldonprice = iInt; break; 
      case 103: Item->ldonsellbackrate = iInt; break; 
      case 104: Item->ldonsold = iInt; break; 
      case 105: Item->bagtype = iInt; break; 
      case 106: Item->bagslots = iInt; break; 
      case 107: Item->bagsize = iInt; break; 
      case 108: Item->bagwr = iInt; break; 
      case 109: Item->book = iInt; break; 
      case 110: Item->booktype = iInt; break; 
      case 111: Item->filename = iInt; break; 
      case 112: Item->loregroup = iInt; break; 
      case 113: Item->artifactflag = iInt; break; 
      case 114: Item->UNK109 = iInt; break; 
      case 115: Item->favor = iInt; break; 
      case 116: Item->guildfavor = iInt; break; 
      case 117: Item->fvnodrop = iInt; break; 
      case 118: Item->dotshielding = iInt; break; 
      case 119: Item->attack = iInt; break; 
      case 120: Item->regen = iInt; break; 
      case 121: Item->manaregen = iInt; break; 
      case 122: Item->enduranceregen = iInt; break; 
      case 123: Item->haste = iInt; break; 
      case 124: Item->damageshield = iInt; break; 
      case 125: Item->UNK120 = iInt; break; 
      case 126: Item->UNK121 = iInt; break; 
      case 127: Item->attuneable = iInt; break; 
      case 128: Item->nopet = iInt; break; 
      case 129: Item->UNK124 = iInt; break; 
      case 130: Item->potionbelt = iInt; break; 
      case 131: Item->potionbeltslots = iInt; break; 
      case 132: Item->stacksize = iInt; break; 
      case 133: Item->notransfer = iInt; break; 
      case 134: Item->scriptfile = iInt; break; 
      case 135: Item->questitemflag = iInt; break; 
      case 136: Item->expendablearrow = iInt; break; 
      case 137: strcpy_s(Item->UNK132, cField); break; 
      case 138: Item->clickeffect = iInt; break; 
      case 139: Item->clicktype = iInt; break; 
      case 140: Item->clicklevel2 = iInt; break; 
      case 141: Item->clicklevel = iInt; break; 
      case 142: Item->maxcharges = iInt; break; 
      case 143: Item->casttime = iInt; break; 
      case 144: Item->recastdelay = iInt; break; 
      case 145: Item->recasttype = iInt; break; 
      case 146: Item->clickunk5 = iInt; break; 
      case 147: Item->clickname = iInt; break; 
      case 148: Item->clickunk7 = iInt; break; 
      case 149: Item->proceffect = iInt; break; 
      case 150: Item->proctype = iInt; break; 
      case 151: Item->proclevel2 = iInt; break; 
      case 152: Item->proclevel = iInt; break; 
      case 153: Item->procunk1 = iInt; break; 
      case 154: Item->procunk2 = iInt; break; 
      case 155: Item->procunk3 = iInt; break; 
      case 156: Item->procunk4 = iInt; break; 
      case 157: Item->procrate = iInt; break; 
      case 158: Item->procname = iInt; break; 
      case 159: Item->procunk7 = iInt; break; 
      case 160: Item->worneffect = iInt; break; 
	  case 161: Item->worntype = iInt; break; 
      case 162: Item->wornlevel2 = iInt; break; 
      case 163: Item->wornlevel = iInt; break; 
      case 164: Item->wornunk1 = iInt; break; 
      case 165: Item->wornunk2 = iInt; break; 
      case 166: Item->wornunk3 = iInt; break; 
      case 167: Item->wornunk4 = iInt; break; 
      case 168: Item->wornunk5 = iInt; break; 
      case 169: Item->wornname = iInt; break; 
      case 170: Item->wornunk7 = iInt; break; 
      case 171: Item->focuseffect = iInt; break; 
      case 172: Item->focustype = iInt; break; 
      case 173: Item->focuslevel2 = iInt; break; 
      case 174: Item->focuslevel = iInt; break; 
      case 175: Item->focusunk1 = iInt; break; 
      case 176: Item->focusunk2 = iInt; break; 
      case 177: Item->focusunk3 = iInt; break; 
      case 178: Item->focusunk4 = iInt; break; 
      case 179: Item->focusunk5 = iInt; break; 
      case 180: Item->focusname = iInt; break; 
      case 181: Item->focusunk7 = iInt; break; 
      case 182: Item->scrolleffect = iInt; break; 
      case 183: Item->scrolltype = iInt; break; 
      case 184: Item->scrolllevel2 = iInt; break; 
      case 185: Item->scrolllevel = iInt; break; 
      case 186: Item->scrollunk1 = iInt; break; 
      case 187: Item->scrollunk2 = iInt; break; 
      case 188: Item->scrollunk3 = iInt; break; 
      case 189: Item->scrollunk4 = iInt; break; 
      case 190: Item->scrollunk5 = iInt; break; 
      case 191: Item->scrollname = iInt; break; 
      case 192: Item->scrollunk7 = iInt; break; 
      case 193: Item->powersourcecapacity = iInt; break; 
      case 194: Item->purity = iInt; break; 
	  case 195: Item->dsmitigation = iInt; break;
	  case 196: Item->heroic_str = iInt; break;
	  case 197: Item->heroic_int = iInt; break;
	  case 198: Item->heroic_wis = iInt; break;
	  case 199: Item->heroic_agi = iInt; break;
	  case 200: Item->heroic_dex = iInt; break;
	  case 201: Item->heroic_sta = iInt; break;
	  case 202: Item->heroic_cha = iInt; break;
	  case 203: Item->healamt = iInt; break;
	  case 204: Item->spelldmg = iInt; break;
	  case 205: Item->clairvoyance = iInt; break;
	  case 206: Item->backstabdmg = iInt; break;
	  case 207: Item->bardeffect = iInt; break;
	  case 208: Item->bardeffecttype = iInt; break;
	  case 209: Item->bardlevel2 = iInt; break;
	  case 210: Item->bardlevel = iInt; break;
      case 211: Item->bardunk1 = iInt; break;
      case 212: Item->bardunk2 = iInt; break;
      case 213: Item->bardunk3 = iInt; break;
      case 214: Item->bardunk4 = iInt; break;
      case 215: Item->bardunk5 = iInt; break;
      case 216: Item->bardname = iInt; break;
      case 217: Item->bardunk7 = iInt; break;
      case 218: Item->UNK214 = iInt; break;
      case 219: Item->UNK219 = iInt; break;
      case 220: Item->UNK220 = iInt; break;
      case 221: Item->UNK221 = iInt; break;
      case 222: Item->UNK222 = iInt; break;
      case 223: Item->UNK223 = iInt; break;
      case 224: Item->UNK224 = iInt; break;
      case 225: Item->UNK225 = iInt; break;
      case 226: Item->UNK226 = iInt; break;
      case 227: Item->UNK227 = iInt; break;
      case 228: Item->UNK228 = iInt; break;
      case 229: Item->UNK229 = iInt; break;
      case 230: Item->UNK230 = iInt; break;
      case 231: Item->UNK231 = iInt; break;
      case 232: Item->UNK232 = iInt; break;
      case 233: Item->UNK233 = iInt; break;
      case 234: Item->UNK234 = iInt; break;
      case 235: Item->UNK235 = iInt; break;
      case 236: Item->UNK236 = iInt; break;
      case 237: Item->UNK237 = iInt; break;
      case 238: Item->UNK238 = iInt; break;
      case 239: Item->UNK239 = iInt; break;
      case 240: Item->UNK240 = iInt; break;
      case 241: Item->UNK241 = iInt; break;
	  case 242: Item->UNK242 = iInt; break;
	  case 243: Item->evolvinglevel = iInt; break;
      case 244: Item->verified = iInt; break; 
   } 
} 

// 
// void ReadItem (char * cLine) 
// 
static void ReadItem (PEQITEM Item, char * cLine) 
{ 
   char * cPtr = cLine; 
   int iField = 0; 

   while (*cPtr) { 
      char cField[256]; 
      char * cDest = cField;
	  bool bEscape = false;

	  //DebugSpew("Escape: %s, cPtr: %c", bEscape?"True":"False", *cPtr);
      while ((*cPtr != '|' || bEscape) && *cPtr != '\0') { 
		 if (bEscape) bEscape = !bEscape; else bEscape = *cPtr == '\\';
		 if (bEscape) {cPtr++ ; /*DebugSpew("Escape: %s, cPtr: %c", bEscape?"True":"False", *cPtr);*/ continue;}
		 *(cDest++) = *(cPtr++); 
		 //DebugSpew("Escape: %s, cPtr: %c", bEscape?"True":"False", *cPtr);
      } 
      *cDest = '\0'; 
      cPtr++; 

      //WriteChatf("cField: %s", cField); 
      SetField (Item, iField++, cField); 
   } 
} 
typedef unsigned int uint32_t;
// 
// uint32_t calc_hash (const char *string) 
// 
static uint32_t calc_hash (const char *string) 
{ 
   register int hash = 0; 

   while (*string != '\0') { 
      register int c = toupper(*string); 
      hash *= 0x1F; 
      hash += (int) c; 

      string++; 
   } 

   return hash; 
} 

template <unsigned int _Size>static void MakeLink (PEQITEM Item, CHAR(&cLink)[_Size]) 
{ 
	char hashstr[512] = { 0 };

   // charm
   if(Item->itemclass == 0 && Item->charmfileid != 0) { 
      sprintf_s(hashstr, "%d%s%d %d %d %d %d %d %d %d", 
         Item->id, Item->name, 
         Item->light, Item->icon, Item->price, Item->size, 
         Item->itemclass, Item->itemtype, Item->favor, 
         Item->guildfavor); 

      //WriteChatf("Charm: %s", Item->name); 
      //WriteChatf("Hash: %s", hashstr); 
   // books
   } else if(Item->itemclass == 2) { 
      sprintf_s(hashstr, "%d%s%d%d%09X", 
         Item->id, Item->name, 
         Item->weight, Item->booktype, Item->price); 

      //WriteChatf("Book: %s", Item->name); 
      //WriteChatf("Hash: %s", hashstr); 
   // bags
   } else if(Item->itemclass == 1) { 
      sprintf_s(hashstr, "%d%s%x%d%09X%d", 
         Item->id, Item->name, 
         Item->bagslots, Item->bagwr, Item->price, Item->weight); 

      //WriteChatf("Bag: %s", Item->name); 
      //WriteChatf("Hash: %s", hashstr); 
   // normal items
   } else { 
      sprintf_s(hashstr, "%d%s%d %d %d %d %d %d %d %d %d %d %d %d %d", 
         Item->id, Item->name, 
         Item->mana, Item->hp, Item->favor, Item->light, 
         Item->icon, Item->price, Item->weight, Item->reqlevel, 
         Item->size, Item->itemclass, Item->itemtype, Item->ac, 
         Item->guildfavor); 

      //WriteChatf("Item: %s", Item->name); 
      //WriteChatf("Hash: %s", hashstr); 
   } 

   uint32_t hash = calc_hash (hashstr); 

   if (Item->loregroup > 1000) 
   { 
      // Evolving 
      sprintf_s(cLink, "\x12%d%05X%05X%05X%05X%05X%05X%06X%1d%04X%1X%05X%08X%s\x12", 
         0, 
         Item->id, 
         0, 0, 0, 0, 0, // Augs 
		 0, // New field in CoF
         1, Item->loregroup, (Item->id%10)+1, // Evolving items (0=no 1=evolving, lore group, level) 
		 0, // Item->icon,
         hash, // Item hash 
         Item->name); 
   } 
   else 
   { 
      // Non-evolving 
      sprintf_s(cLink, "\x12%d%05X%05X%05X%05X%05X%05X%06X%1d%04X%1X%05X%08X%s\x12", 
         0, 
         Item->id, 
         0, 0, 0, 0, 0, // Augs 
		 0, // New field in CoF
         0, 0, 0, // Evolving items (0=no 1=evolving, lore group, level) 
		 0, // Item->icon,
         hash, // Item hash 
         Item->name); 
   } 
} 

// 
// static int ConvertItemsDotTxt (void) 
// 
static int ConvertItemsDotTxt (void) 
{ 
   WriteChatf ("MQ2LinkDB: Importing items.txt..."); 

   char cFilename[MAX_PATH]; 
   sprintf_s(cFilename,"%s\\items.txt", gszINIPath); 
   FILE * File = 0;
   errno_t err = fopen_s(&File,cFilename, "rt");
   if (!err) { 
      char cFilename2[MAX_PATH]; 
      sprintf_s(cFilename2, "%s\\MQ2LinkDB.txt", gszINIPath); 
	  FILE * LinkFile = 0;
	  err = fopen_s(&LinkFile, cFilename2, "wt");

      if (!err) { 
         if (abPresent != NULL) { 
            bKnowTotal = false; 
            free (abPresent); 
            abPresent = NULL; 
         } 

         WriteChatf ("MQ2LinkDB: Generating links..."); 
		 char cLine[MAX_STRING] = { 0 };

         bool bFirst = true;; 
         int iCount = 0; 
         while (fgets (cLine, MAX_STRING, File) != NULL) { 
            cLine[strlen (cLine) - 1] = '\0'; 

            if (bFirst) { 
               // Small sanity check on file 
               char * cCheck = "items.itemclass|items.name|items.lore|items.lorefile|items.idfile|items.id|items.weight|items.norent|items.nodrop|items.size|items.slots|items.price"; 
               if (memcmp (cLine, cCheck, strlen (cCheck)) != 0) { 
                  WriteChatf ("MQ2LinkDB: \arInvalid items.txt file. Aborting", iCount); 
                  break; 
               } 

               bFirst = false; 
            } else { 
               EQITEM Item; 
               memset (&Item, 0, sizeof (Item)); 
               ReadItem (&Item, cLine); 

			   if (Item.id) {
		          char cLink[256]; 
                  MakeLink (&Item, cLink); 

                  //WriteChatf("Test ItemID: %d", ItemID(cLink)); 

                  fprintf (LinkFile, "%s\n", cLink); 
                  iCount++; 
			   }
            } 
         } 

         WriteChatf ("MQ2LinkDB: Complete! \ay%d\ax links generated", iCount); 

         fclose (LinkFile); 
      } else { 
         WriteChatf ("MQ2LinkDB: \arCould not create link file (MQ2LinkDB.txt) (err: %d)", errno); 
      } 

      fclose (File); 
   } else { 
      WriteChatf ("MQ2LinkDB: \arSource file not found (items.txt)"); 
   } 

   return 0; 
} 
