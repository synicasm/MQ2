

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// Projet: MQ2Melee.cpp     | 2008-04-02: Updated by Wasted
// Author: s0rCieR          | 2008-11-08: Updated by Jobey
//                          | 2008-11-26: Updated by htw
//                          | 2009-02-21: Updated by pms (MoveUtils 9.x support)
//                          | 2010-11-01: Updated by pms (BagWindow support)
//                          | 2010-10-21: Updated by maskoi (House of Thule abilities)
//                          | 2011-11-15: Updated by maskoi (Veil of Alaris abilities)
//                          | 2012-12-02: Updated by Teichou (Rain of Fear abilities), with pet fix by Gomer) (Pet fix)
//                          | 2012-12-31: Updated by Teichou(with pet fix by Gomer) (Pet fix)
//                          | 2014-04-15: Updated by Cr4zyb4rd (-BagWindow +moveitem2)
//                          | 2014-04-19: Updated by Maskoi various fixes
//                          | 2014-05-05: Updated by Cr4zyb4rd rogue fixes
//                          | 2014-05-06: Updated by rswiders ranger and pet fixes
//                          | 2014-05-13: Updated by Cr4zyb4rd -BagWindow +moveitem2, equip/swapping fixes, rogue fixes
//                          | 2014-06-02: Updated by rswiders fix for doMELEE/doPETASSIST,ranger and pet fixes, added cleric yaulp for mana
//                          | 2014-11-17: Updated by Nayenia (The Darkened Sea abilities)
//                          | 2014-12-20: Updated by Maskoi  (The Darkened Sea abilities), Pet fixes, Added Ghold thanks eqmule
//                          | 2014-12-22: Updated by rswiders fix for 64bit getticks
//                          | 2015-02-10: Updated by Ctaylor22 addition of global downshitIf, and 60 slots for down/holy shits.
//                          | 2015-05-23: Updated by winnower berserker TDS fixes and improvements
//                          | 2015-07-14: Updated by Eqmule CombatabilityTimer change
//                          | 2016-07-24: Updated by Eqmule Added string safety
//                          | 2016-08-30: Updated by Eqmule GetAAbyId Level changes
//                          | 2017-02-15: Updated by rswiders XTarget changes
//                          | 2017-08-22: Updated by Eqmule downflag and holyflag now takes 0, 1 or 2, if its set to 2 it tells the plugin to only parse it if a macro IS running.
//                          | 2017-12-12: UPdated by rswiders (Ring of Scale abilities)
//                          | 2018-01-19: Updated by Saar (rest of the RoS abilities)
//                          | 2018-04-24: Updated by Eqmule (Slam Fix)
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// SHOW_ABILITY:    0=0ff, 1=Display every ability that plugin use.
// SHOW_ATTACKING:  0=0ff, 1=Display Attacking Target
// SHOW_CASTING:    0=0ff, 1=Display MQ2Cast Target
// SHOW_CONTROL:    0=0ff, 1=Display Pet Control
// SHOW_ENRAGING:   0=0ff, 1=Display Enrage/Infuriate
// SHOW_FEIGN:      0=0ff, 1=Display Fallen Detected
// SHOW_OVERRIDE:   0=0ff, 1=Display Override Warning
// SHOW_PROVOKING:  0=0ff, 1=Display Provoke/Aggro AA/Disc/Spell
// SHOW_STICKING:   0=0ff, 1=Display Stick Arguments
// SHOW_STUNNING    0=0ff, 1=Display Stunning AA/Disc/Spell
// SHOW_SWITCHING:  0=0ff, 1=Display Switch Melee/Range
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// Distribution of this code in compile form without source code is prohibited.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
#define   PLUGIN_NAME  "MQ2Melee"   // Plugin Name
#define   PLUGIN_DATE   20180424    // Plugin Date
#define   PLUGIN_VERS   8.7         // Plugin Version

#define   SHOW_ABILITY         0
#define   SHOW_ATTACKING       1
#define   SHOW_CASTING         0
#define   SHOW_CONTROL         0
#define   SHOW_ENRAGING        0
#define   SHOW_FEIGN           1
#define   SHOW_OVERRIDE        0
#define   SHOW_PROVOKING       0
#define   SHOW_STICKING        0
#define   SHOW_STUNNING        0
#define   SHOW_SWITCHING       1

#define   NOID                -1
#define   delay              250

enum { Tiny, Small, Medium, Large, Giant, Huge }; // container sizes

enum {
    st_x          =0x0000,    // SpawnType: NONE
    st_cn         =0x0020,    // SpawnType: CORPSENPC
    st_cp         =0x0010,    // SpawnType: CORPSEPLAYER
    st_wn         =0x0008,    // SpawnType: PETNPC
    st_wp         =0x0004,    // SpawnType: PETPLAYER
    st_n          =0x0002,    // SpawnType: NPC
    st_p          =0x0001,    // SpawnType: PLAYER
};

enum {
    inv_range         =11,    // Inventory.Range      Slot ID
    inv_primary       =13,    // Inventory.Primary    Slot ID
    inv_secondary     =14,    // Inventory.Secondary  Slot ID
    inv_ammo          =22,    // Inventory.Ammo       Slot ID
};

#ifndef PLUGIN_API
#include "../MQ2Plugin.h"
PreSetup(PLUGIN_NAME);
PLUGIN_VERSION(PLUGIN_VERS);
#include <map>
#include <string>
#include "../Blech/Blech.h"
#endif PLUGIN_API

#include "../moveitem.h"

using namespace std;
//MoveUtils 11.x
void(*fStickCommand)(PSPAWNINFO pChar, char* szLine);
bool* pbStickOn;
PLUGIN_API bool bMULoaded = false;
bool bMUPointers = false;

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

bool      DebugReady     = false;           // Use for debugging Ability->Ready();
bool      BardClass      = false;           // Bard Class?
bool      BerserkerClass = false;           // Beserker Class?
bool      MonkClass      = false;           // Monk Class?
bool      RogueClass     = false;           // Rogue Class?
bool      Silenced       = false;           // Silenced?
long      BuffMax        = NUM_LONG_BUFFS;  // Maximum Number of Buffs
long      SongMax        = NUM_SHORT_BUFFS; // Maximum Number of Songs
long      GemsMax        = 12;              // Maximum Number of Gems
short     PET_BUTTONS    = 14;              // Number of buttons on Pet UI window

long      InvSlot        = NOID;            // slot # where item is found
PCONTENTS InvCont        = NULL;            // slot content pointer

bool      Sticking       = false;           // Stick Saved State On/Off?
char      StickArg[128]  = { 0 };           // Stick Saved Arguments

char      Reserved[MAX_STRING] = { 0 };     // string buffer
char      Workings[MAX_STRING] = { 0 };     // string buffer

typedef   void(__cdecl *Function)(void);
struct    infodata { long i, t; } *pinfodata;
//Rogue Strike Fix htw 2/20/2011
bool StrikeFail = false;
ULONGLONG PressDelay = 0;

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// These definitions are used for various conditions in MQ2Melee.
#define   d_assassin1       4676      // Duelist Discipline lv 58 ROG
#define   d_assassin2       10898     // Assassin's Discipline lv 75 ROG TSS
#define   d_assassin3       10899     // Assassin Discipline Rk. II
#define   d_assassin4       10900     // Assassin Discipline Rk. III
#define   d_assassin5       29252     // Eradicator's Discipline lv 95 ROG VOA
#define   d_assassin6       29253     // Eradicator's Discipline Rk. II
#define   d_assassin7       29254     // Eradicator's Discipline Rk. III

#define   d_cleaverage1     5037      // Cleaving Rage Discipline 54
#define   d_cleaverage2     5043      // Cleaving Anger Discipline 65
#define   d_cleaverage3     27257     // Cleaving Acrimony Discipline lv 86 BER HoT
#define   d_cleaverage4     27258     // Cleaving Acrimony Discipline rk. II
#define   d_cleaverage5     27259     // Cleaving Acrimony Discipline rk. III

#define   i_disarm            16
#define   i_forage            27
#define   i_intimidation      71
#define   i_kick              30
#define   i_mend              32
#define   i_taunt             73

infodata
btlleap        = { 611    ,4 },        // aa: battle leap
btlstromp      = { 1252   ,4 },        // aa: Battle Stomp
asp            = { 986    ,4 },        // aa: bite of the asp

assault1       = { 22540  ,3 },        // disc: Assault Lv 85 rog UF
assault2       = { 22541  ,3 },        // disc: Assault Rk. II
assault3       = { 22542  ,3 },        // disc: Assault Rk. III
assault4       = { 26142  ,3 },        // disc: Battery Lv 90 rog HoT
assault5       = { 26143  ,3 },        // disc: Battery Rk. II
assault6       = { 26144  ,3 },        // disc: Battery Rk. III
assault7       = { 29243  ,3 },        // disc: Onslaught Lv 95 rog VoA
assault8       = { 29244  ,3 },        // disc: Onslaught Rk. II
assault9       = { 29245  ,3 },        // disc: Onslaught Rk. III
assault10      = { 35299  ,3 },        // disc: Incursion Lv 100 rog RoF
assault11      = { 35300  ,3 },        // disc: Incursion Rk. II
assault12      = { 35301  ,3 },        // disc: Incursion Rk. III
assault13      = { 44172  ,3 },        // disc: Barrage Lv 105 rog TDS
assault14      = { 44173  ,3 },        // disc: Barrage Rk. II
assault15      = { 44174  ,3 },        // disc: Barrage Rk. III
assault16      = { 56324  ,3 },        // disc: Fellstrike Lv 110 rog RoS
assault17      = { 56325  ,3 },        // disc: Fellstrike Rk. II
assault18      = { 56326  ,3 },        // disc: Fellstrike Rk. III

banestrike     = { 15073  ,4 },        // aa: Banestrike

bladesrng1     = { 40105  ,3 },        // disc: storm of blades rk i
bladesrng2     = { 40106  ,3 },        // disc: storm of blades rk ii
bladesrng3     = { 40107  ,3 },        // disc: storm of blades rk iii
bladesrng4     = { 40108  ,3 },        // disc: focused storm of blades rk i
bladesrng5     = { 40109  ,3 },        // disc: focused storm of blades rk ii
bladesrng6     = { 40110  ,3 },        // disc: focused storm of blades rk iii

bleed1         = { 19247  ,3 },        // disc: bleed Lv 83 rog UF
bleed2         = { 19248  ,3 },        // disc: bleed Rk. II
bleed3         = { 19249  ,3 },        // disc: bleed Rk. III
bleed4         = { 26127  ,3 },        // disc: Wound Lv 88 rog HoT
bleed5         = { 26128  ,3 },        // disc: Wound Rk. II
bleed6         = { 26129  ,3 },        // disc: Wound Rk. III
bleed7         = { 29228  ,3 },        // disc: Lacerate Lv 93 rog VoA
bleed8         = { 29229  ,3 },        // disc: Lacerate Rk. II
bleed9         = { 29230  ,3 },        // disc: Lacerate Rk. III
bleed10        = { 35284  ,3 },        // disc: Gash Lv 98 rog RoF
bleed11        = { 35285  ,3 },        // disc: Gash Rk. II
bleed12        = { 35286  ,3 },        // disc: Gash Rk. III
bleed13        = { 44151  ,3 },        // disc: Hack Lv 103 rog TDS
bleed14        = { 44152  ,3 },        // disc: Hack Rk. II
bleed15        = { 44153  ,3 },        // disc: Hack Rk. III
bleed16        = { 56303  ,3 },        // disc: Slice Lv 108 rog RoS
bleed17        = { 56304  ,3 },        // disc: Slice Rk. II
bleed18        = { 56305  ,3 },        // disc: Slice Rk. III

bloodlust1     = { 22506  ,3 },        // disc: Shared Bloodlust Lv 85 ber UF
bloodlust2     = { 22507  ,3 },        // disc: Shared Bloodlust  Rk. II
bloodlust3     = { 22508  ,3 },        // disc: Shared Bloodlust  Rk. III
bloodlust4     = { 27317  ,3 },        // disc: Shared Brutality Lv 90 ber HoT
bloodlust5     = { 27318  ,3 },        // disc: Shared Brutality Rk. II
bloodlust6     = { 27319  ,3 },        // disc: Shared Brutality Rk. III
bloodlust7     = { 30475  ,3 },        // disc: Shared Savagery Lv 95 ber VoA
bloodlust8     = { 30476  ,3 },        // disc: Shared Savagery Rk. II
bloodlust9     = { 30477  ,3 },        // disc: Shared Savagery Rk. III
bloodlust10    = { 36541  ,3 },        // disc: Shared Viciousness Lv 100 ber RoF
bloodlust11    = { 36542  ,3 },        // disc: Shared Viciousness Rk. II
bloodlust12    = { 36543  ,3 },        // disc: Shared Viciousness Rk. III
bloodlust13    = { 45278  ,3 },        // disc: Shared Cruelty Lv 105 ber TDS
bloodlust14    = { 45279  ,3 },        // disc: Shared Cruelty Rk. II
bloodlust15    = { 45280  ,3 },        // disc: Shared Cruelty Rk. III
bloodlust16    = { 57558  ,3 },        // disc: Shared Ruthlessness Lv 110 Ber RoS
bloodlust17    = { 57559  ,3 },        // disc: Shared Ruthlessness Rk. II
bloodlust18    = { 57560  ,3 },        // disc: Shared Ruthlessness Rk. III

bvivi1         = { 27098  ,3 },        // disc: Bestial Vivisection lv 86 BST HoT
bvivi2         = { 27099  ,3 },        // disc: Bestial Vivisection Rk. II
bvivi3         = { 27100  ,3 },        // disc: Bestial Vivisection Rk. III
bvivi4         = { 30238  ,3 },        // disc: Bestial Rending lv 91 BST VoA
bvivi5         = { 30239  ,3 },        // disc: Bestial Rending Rk. II
bvivi6         = { 30240  ,3 },        // disc: Bestial Rending Rk. III
bvivi7         = { 36319  ,3 },        // disc: Bestial Evulsing lv 96 BST RoF
bvivi8         = { 36320  ,3 },        // disc: Bestial Evulsing Rk. II
bvivi9         = { 36321  ,3 },        // disc: Bestial Evulsing Rk. III
bvivi10        = { 57335  ,3 },        // disc: Bestial Savagery Lv 106 Bst RoS
bvivi11        = { 57336  ,3 },        // disc: Bestial Savagery Rk. II
bvivi12        = { 57337  ,3 },        // disc: Bestial Savagery Rk. III

boastful       = { 199    ,4 },        // aa: boastful bellow
callchal       = { 552    ,4 },        // aa: call of challenge
commanding     = { 8000   ,3 },        // disc: commanding voice

cloud1         = { 25914  ,3 },        // disc: Cloud of Fists Lv 87 mnk HoT
cloud2         = { 25915  ,3 },        // disc: Cloud of Fists rk. ii
cloud3         = { 25916  ,3 },        // disc: Cloud of Fists rk. iii
cloud4         = { 40229  ,3 },        // disc: Phantom Partisan Lv 100 mnk HoT
cloud5         = { 40230  ,3 },        // disc: Phantom Partisan rk. ii
cloud6         = { 40231  ,3 },        // disc: Phantom Partisan rk. iii

cripple1       = { 4928   ,3 },        // disc: leg strike
cripple2       = { 4929   ,3 },        // disc: leg cut
cripple3       = { 4930   ,3 },        // disc: leg slice
cripple4       = { 8205   ,3 },        // disc: crippling strike
cripple5       = { 10908  ,3 },        // disc: tendon cleave
cripple6       = { 10909  ,3 },        // disc: tendon cleave rk. ii
cripple7       = { 10910  ,3 },        // disc: tendon cleave rk. iii
cripple8       = { 14177  ,3 },        // disc: tendon sever
cripple9       = { 14178  ,3 },        // disc: tendon sever rk. ii
cripple10      = { 14179  ,3 },        // disc: tendon sever rk. iii
cripple11      = { 18198  ,3 },        // disc: tendon shear Lv 81 ber UF
cripple12      = { 18199  ,3 },        // disc: tendon shear rk. ii
cripple13      = { 18200  ,3 },        // disc: tendon shear rk. iii
cripple14      = { 27263  ,3 },        // disc: tendon lacerate Lc 86 ber HoT
cripple15      = { 27264  ,3 },        // disc: tendon lacerate rk. ii
cripple16      = { 27265  ,3 },        // disc: tendon lacerate rk. iii
cripple17      = { 30412  ,3 },        // disc: Tendon Slash Lv 91 ber VoA
cripple18      = { 30413  ,3 },        // disc: Tendon Slash rk. ii
cripple19      = { 30414  ,3 },        // disc: Tendon Slash rk. iii
cripple20      = { 36505  ,3 },        // disc: Tendon Gash Lv 96 ber RoF
cripple21      = { 36506  ,3 },        // disc: Tendon Gash Rk. II
cripple22      = { 36507  ,3 },        // disc: Tendon Gash Rk. III
cripple23      = { 45224  ,3 },        // disc: Tendon Tear Lv 101 ber TDS
cripple24      = { 45225  ,3 },        // disc: Tendon Tear Rk. II
cripple25      = { 45226  ,3 },        // disc: Tendon Tear Rk. III
cripple26      = { 57516  ,3 },        // disc: Tendon Rupture Lv 106 Ber RoS
cripple27      = { 57517  ,3 },        // disc: Tendon Rupture Rk. II
cripple28      = { 57518  ,3 },        // disc: Tendon Rupture Rk. III

cryhavoc1      = { 8003   ,3 },        // disc: cry havoc
cryhavoc2      = { 36556  ,3 },        // disc: Cry Carnage lev 98 Rof
cryhavoc3      = { 36557  ,3 },        // disc: Cry Carnage Rk. II 
cryhavoc4      = { 36558  ,3 },        // disc: Cry Carnage Rk. III

cstrike        = { 11080  ,4 },        // aa: Chameleon Strike

defense1       = { 22556  ,3 },        // disc: Bracing Defense Lv 85 war UF
defense2       = { 22557  ,3 },        // disc: Bracing Defense Rk. II
defense3       = { 22558  ,3 },        // disc: Bracing Defense Rk. III
defense4       = { 25051  ,3 },        // disc: Staunch Defense Lc 90 war HoT
defense5       = { 25052  ,3 },        // disc: Staunch Defense Rk. II
defense6       = { 25053  ,3 },        // disc: Staunch Defense Rk. III
defense7       = { 28066  ,3 },        // disc: Stalwart Defense Lv 95 war VoA
defense8       = { 28067  ,3 },        // disc: Stalwart Defense Rk. II
defense9       = { 28068  ,3 },        // disc: Stalwart Defense Rk. III
defense10      = { 34042  ,3 },        // disc: Steadfast Defense Lv 100 war RoF
defense11      = { 34043  ,3 },        // disc: Steadfast Defense Rk. II
defense12      = { 34044  ,3 },        // disc: Steadfast Defense Rk. III
defense13      = { 43060  ,3 },        // disc: Stout Defense Lv 105 war TDS
defense14      = { 43061  ,3 },        // disc: Stout Defense Rk. II
defense15      = { 43062  ,3 },        // disc: Stout Defense Rk. III
defense16      = { 55057  ,3 },        // disc: Resolute Defense Lv 110 War RoS
defense17      = { 55058  ,3 },        // disc: Resolute Defense Rk. II
defense18      = { 55059  ,3 },        // disc: Resolute Defense Rk. III

enragingkick1  = { 28506  ,3 },        // disc: Enraging Crescent lv 92 Voa rng
enragingkick2  = { 28507  ,3 },        // disc: Enraging Crescent Kicks Rk. III
enragingkick3  = { 28508  ,3 },        // disc: Enraging Crescent Kicks Rk. III
enragingkick4  = { 34527  ,3 },        // disc: Enraging Heel Kicks lv 97 Rof rng
enragingkick5  = { 34528  ,3 },        // disc: Enraging Heel Kicks Rk. III
enragingkick6  = { 34529  ,3 },        // disc: Enraging Heel Kicks Rk. III
enragingkick7  = { 43463  ,3 },        // disc: Enraging Cut Kicks lv 102 TDS rng
enragingkick8  = { 43464  ,3 },        // disc: Enraging Cut Kicks Rk. III
enragingkick9  = { 43465  ,3 },        // disc: Enraging Cut Kicks Rk. III
enragingkick10 = { 55539  ,3 },        // disc: Enraging Wheel Kicks Lv 107 Rng RoS
enragingkick11 = { 55540  ,3 },        // disc: Enraging Wheel Kicks Rk. II
enragingkick12 = { 55541  ,3 },        // disc: Enraging Wheel Kicks Rk. III

escape         = { 102    ,4 },        // aa: escape
eyegouge       = { 470    ,4 },        // aa: eye gouge

feignid        = { 420    ,4 },        // aa: imitate death
feigndp        = { 428    ,4 },        // aa: death peace
feigns1        = { 366    ,5 },        // spell: feign death
feigns2        = { 3685   ,5 },        // spell: comatose
feigns3        = { 1460   ,5 },        // spell: death peace
feigns4        = { 10306  ,5 },        // spell: last breath
feigns5        = { 10307  ,5 },        // spell: last breath rk ii
feigns6        = { 10308  ,5 },        // spell: last breath rk iii
feigns7        = { 15223  ,5 },        // spell: rigor mortis
feigns8        = { 15224  ,5 },        // spell: rigor mortis rk ii
feigns9        = { 15225  ,5 },        // spell: rigor mortis rk iii
feigns10       = { 15190  ,5 },        // spell: last gasp
feigns11       = { 15191  ,5 },        // spell: last gasp rk ii
feigns12       = { 15192  ,5 },        // spell: last gasp rk iii
feigns13       = { 19343  ,5 },        // spell: Final Breath Lv 85 sk UF
feigns14       = { 19344  ,5 },        // spell: Final Breath rk ii
feigns15       = { 19345  ,5 },        // spell: Final Breath rk iii
feigns16       = { 25662  ,5 },        // spell: Last Breath Lv 90 sk Hot
feigns17       = { 25663  ,5 },        // spell: Last Breath rk ii
feigns18       = { 25664  ,5 },        // spell: Last Breath rk iii
feigns19       = { 28760  ,5 },        // spell: Final Gasp Lv 95 sk Voa
feigns20       = { 28761  ,5 },        // spell: Final Gasp rk ii
feigns21       = { 28762  ,5 },        // spell: Final Gasp rk iii
feigns22       = { 34775  ,5 },        // spell: Terminal Breath Lv 100 sk RoF
feigns23       = { 34776  ,5 },        // spell: Terminal Breath Rk. II
feigns24       = { 34777  ,5 },        // spell: Terminal Breath Rk. III

feign_bst      = { 11073  ,4 },        // aa: playing possum

feign_n1       = { 25662  ,5 },        // spell: Scapegoat
feign_n2       = { 25663  ,5 },        // spell: Scapegoat rk ii
feign_n3       = { 25664  ,5 },        // spell: Scapegoat rk iii

fieldarm1      = { 19917  ,3 },        // disc: Field Armorer Lv 85 war UF
fieldarm2      = { 19918  ,3 },        // disc: Field Armorer Rk. II
fieldarm3      = { 19919  ,3 },        // disc: Field Armorer Rk. III
fieldarm4      = { 25036  ,3 },        // disc: Field Outfitter Lv 90 war HoT
fieldarm5      = { 25037  ,3 },        // disc: Field Outfitter Rk. II
fieldarm6      = { 25038  ,3 },        // disc: Field Outfitter Rk. III
fieldarm7      = { 28051  ,3 },        // disc: Field Defender Lv 95 war VoA
fieldarm8      = { 28052  ,3 },        // disc: Field Defender Rk. II
fieldarm9      = { 28053  ,3 },        // disc: Field Defender Rk. III
fieldarm10     = { 34036  ,3 },        // disc: Field Guardian Lv 100 war RoF
fieldarm11     = { 34037  ,3 },        // disc: Field Guardian Rk. II
fieldarm12     = { 34038  ,3 },        // disc: Field Guardian Rk. III
fieldarm13     = { 43057  ,3 },        // disc: Field Protector Lv 105 war TDS
fieldarm14     = { 43058  ,3 },        // disc: Field Protector Rk. II
fieldarm15     = { 43059  ,3 },        // disc: Field Protector Rk. III
fieldarm16     = { 55054  ,3 },        // disc: Field Champion Lv 110 War RoS
fieldarm17     = { 55055  ,3 },        // disc: Field Champion Rk. II
fieldarm18     = { 55056  ,3 },        // disc: Field Champion Rk. III

//ferociouskick  = { 337    ,4 },        // aa: ferocious kick

feral1         = { 247    ,4 },        // aa: feral swipe
fistswu        = { 8002   ,3 },        // disc: fists of wu

fclaw1         = { 27119  ,3 },        // disc: flurry of claws lv 87 BST HoT
fclaw2         = { 27120  ,3 },        // disc: flurry of claws rk. ii
fclaw3         = { 27121  ,3 },        // disc: flurry of claws rk. iii
fclaw4         = { 30271  ,3 },        // disc: Tumult of claws lv 92 BST Voa
fclaw5         = { 30272  ,3 },        // disc: Tumult of claws rk. ii
fclaw6         = { 30273  ,3 },        // disc: Tumult of claws rk. iii
fclaw7         = { 36346  ,3 },        // disc: Clamor of Claws lv 97 BST Rof
fclaw8         = { 36347  ,3 },        // disc: Clamor of Claws rk. ii
fclaw9         = { 36348  ,3 },        // disc: Clamor of Claws rk. iii
fclaw10        = { 40516  ,3 },        // disc: Focused Clamor of Claws lv 98 BST Rof
fclaw11        = { 40517  ,3 },        // disc: Focused Clamor of Claws rk. ii
fclaw12        = { 40518  ,3 },        // disc: Focused Clamor of Claws rk. iii
fclaw13        = { 45103  ,3 },        // disc: Tempest of Claws lv 102 BST TDS
fclaw14        = { 45104  ,3 },        // disc: Tempest of Claws rk. ii
fclaw15        = { 45105  ,3 },        // disc: Tempest of Claws rk. iii
fclaw16        = { 57386  ,3 },        // disc: Storm of Claws Lv 107 Bst RoS
fclaw17        = { 57387  ,3 },        // disc: Storm of Claws Rk. II
fclaw18        = { 57388  ,3 },        // disc: Storm of Claws Rk. III

gblade1        = { 28687  ,3 },        // disc: Gouging Blade - SK Lv 92 VoA
gblade2        = { 28688  ,3 },        // disc: Gouging Blade Rk. II
gblade3        = { 28689  ,3 },        // disc: Gouging Blade Rk. III
gblade4        = { 34714  ,3 },        // disc: Gashing Blade - SK Lv 97 RoF
gblade5        = { 34715  ,3 },        // disc: Gashing Blade Rk. II
gblade6        = { 34716  ,3 },        // disc: Gashing Blade Rk. III
gblade7        = { 43637  ,3 },        // disc: Lacerating Blade - SK Lv 102 RoF
gblade8        = { 43638  ,3 },        // disc: Lacerating Blade Rk. II
gblade9        = { 43639  ,3 },        // disc: Lacerating Blade Rk. III
gblade10       = { 55731  ,3 },        // disc: Wounding Blade Lv 107 Shd RoS
gblade11       = { 55732  ,3 },        // disc: Wounding Blade Rk. II
gblade12       = { 55733  ,3 },        // disc: Wounding Blade Rk. III

gorillasmash   = { 988    ,4 },        // aa: gorilla smash
gutpunch       = { 3732   ,4 },        // aa: gut punch
harmtouch      = { 6000   ,4 },        // aa: harmtouch

honor1         = { 10173  ,5 },        // spell: challenge for honor
honor2         = { 10174  ,5 },        // spell: challenge for honor rk ii
honor3         = { 10175  ,5 },        // spell: challenge for honor rk iii
honor4         = { 14954  ,5 },        // spell: trial for honor
honor5         = { 14955  ,5 },        // spell: trial for honor rk ii
honor6         = { 14956  ,5 },        // spell: trial for honor rk iii
honor7         = { 19068  ,5 },        // spell: Charge for Honor Lv 85 pal UF
honor8         = { 19069  ,5 },        // spell: Charge for Honor rk ii
honor9         = { 19070  ,5 },        // spell: Charge for Honor rk iii
honor10        = { 25297  ,5 },        // spell: Confrontation for Honor Lv 90 pal HoT
honor11        = { 25298  ,5 },        // spell: Confrontation for Honor rk ii
honor12        = { 25299  ,5 },        // spell: Confrontation for Honor rk iii
honor13        = { 28347  ,5 },        // spell: Provocation for Honor Lv 95 pal VoA
honor14        = { 28348  ,5 },        // spell: Provocation for Honor rk ii
honor15        = { 28349  ,5 },        // spell: Provocation for Honor rk iii
honor16        = { 34350  ,5 },        // spell: Demand for Honor Lv 97 pal RoF
honor17        = { 34351  ,5 },        // spell: Demand for Honor rk. ii
honor18        = { 34352  ,5 },        // spell: Demand for Honor rk. iii
honor19        = { 43322  ,5 },        // spell: Impose for Honor Lv 102 pal TDS
honor20        = { 43323  ,5 },        // spell: Impose for Honor rk. ii
honor21        = { 43324  ,5 },        // spell: Impose for Honor rk. iii
honor22        = { 55353  ,5 },        // spell: Refute for Honor Lv 107 Pal RoS
honor23        = { 55354  ,5 },        // spell: Refute for Honor Rk. II
honor24        = { 55355  ,5 },        // spell: Refute for Honor Rk. III

joltber1       = { 4934   ,3 },        // disc: diversive strike
joltber2       = { 4935   ,3 },        // disc: distracting strike
joltber3       = { 4936   ,3 },        // disc: confusing strike
joltber4       = { 6171   ,3 },        // disc: baffling strike
joltber5       = { 10920  ,3 },        // disc: jarring strike
joltber6       = { 10921  ,3 },        // disc: jarring strike rk ii
joltber7       = { 10922  ,3 },        // disc: jarring strike rk iii
joltber8       = { 14186  ,3 },        // disc: jarring smash
joltber9       = { 14187  ,3 },        // disc: jarring smash rk ii
joltber10      = { 14188  ,3 },        // disc: jarring smash rk iii
joltber11      = { 18207  ,3 },        // disc: Jarring Clash Lev 85 ber UF
joltber12      = { 18208  ,3 },        // disc: Jarring Clash rk ii
joltber13      = { 18209  ,3 },        // disc: Jarring Clash rk iii
joltber14      = { 27290  ,3 },        // disc: Jarring Slam Lv 89 ber HoT
joltber15      = { 27291  ,3 },        // disc: Jarring Slam rk ii
joltber16      = { 27292  ,3 },        // disc: Jarring Slam rk iii
joltber17      = { 30445  ,3 },        // disc: Jarring Blow Lv 94 ber VoA
joltber18      = { 30446  ,3 },        // disc: Jarring Blow rk ii
joltber19      = { 30447  ,3 },        // disc: Jarring Blow rk iii
joltber20      = { 36526  ,3 },        // disc: Jarring Crush Lv 99 ber RoF
joltber21      = { 36527  ,3 },        // disc: Jarring Crush rk ii
joltber22      = { 36528  ,3 },        // disc: Jarring Crush rk iii
joltber23      = { 45266  ,3 },        // disc: Jarring Smite Lv 104 ber TDS
joltber24      = { 45267  ,3 },        // disc: Jarring Smite rk ii
joltber25      = { 45268  ,3 },        // disc: Jarring Smite rk iii
joltber26      = { 57549  ,3 },        // disc: Jarring Jolt Lv 109 Ber RoS
joltber27      = { 57550  ,3 },        // disc: Jarring Jolt Rk. II
joltber28      = { 57551  ,3 },        // disc: Jarring Jolt Rk. III

joltbst1       = { 362    ,4 },        // aa: roar of thunder

joltrng1       = { 1741   ,5 },        // spell: jolt
joltrng2       = { 1296   ,5 },        // spell: cinder jolt

jltkicks1      = { 10086  ,3 },        // disc: jolting kicks
jltkicks2      = { 10087  ,3 },        // disc: jolting kicks rk ii
jltkicks3      = { 10088  ,3 },        // disc: jolting kicks rk iii
jltkicks4      = { 15020  ,3 },        // disc: Jolting Snapkicks
jltkicks5      = { 15021  ,3 },        // disc: Jolting Snapkicks rk ii
jltkicks6      = { 15022  ,3 },        // disc: Jolting Snapkicks rk iii
jltkicks7      = { 19152  ,3 },        // disc: Jolting Frontkicks Lv 82 rng UF
jltkicks8      = { 19153  ,3 },        // disc: Jolting Frontkicks rk ii
jltkicks9      = { 19154  ,3 },        // disc: Jolting Frontkicks rk iii
jltkicks10     = { 25432  ,3 },        // disc: Jolting Hook kicks Lv Lv 87 rng HoT
jltkicks11     = { 25433  ,3 },        // disc: Jolting Hook kicks rk ii
jltkicks12     = { 25434  ,3 },        // disc: Jolting Hook kicks rk iii
jltkicks13     = { 28509  ,3 },        // disc: Jolting Crescent kicks lv 92 rng VoA
jltkicks14     = { 28510  ,3 },        // disc: Jolting Crescent kicks rk ii
jltkicks15     = { 28511  ,3 },        // disc: Jolting Crescent kicks rk iii
jltkicks16     = { 34530  ,3 },        // disc: Jolting Heel Kicks lv 97 rng RoF
jltkicks17     = { 34531  ,3 },        // disc: Jolting Heel Kicks rk ii
jltkicks18     = { 34532  ,3 },        // disc: Jolting Heel Kicks rk iii
jltkicks19     = { 43466  ,3 },        // disc: Jolting Cut Kicks lv 102 rng TDS
jltkicks20     = { 43467  ,3 },        // disc: Jolting Cut Kicks rk ii
jltkicks21     = { 43468  ,3 },        // disc: Jolting Cut Kicks rk iii
jltkicks22     = { 55542  ,3 },        // disc: Jolting Wheel Kicks Lv 107 Rng RoS
jltkicks23     = { 55543  ,3 },        // disc: Jolting Wheel Kicks Rk. II
jltkicks24     = { 55544  ,3 },        // disc: Jolting Wheel Kicks Rk. III

jugular1       = { 15121  ,3 },        // disc: Jugular Slash Lv 77 ROG
jugular2       = { 15122  ,3 },        // disc: Jugular Slash Rk. II
jugular3       = { 15123  ,3 },        // disc: Jugular Slash Rk. III
jugular4       = { 19268  ,3 },        // disc: Jugular Slice Lv 82 ROG
jugular5       = { 19269  ,3 },        // disc: Jugular Slice Rk. II
jugular6       = { 19270  ,3 },        // disc: Jugular Slice Rk. III
jugular7       = { 26115  ,3 },        // disc: Jugular Sever Lv 87 ROG HoT
jugular8       = { 26116  ,3 },        // disc: Jugular Sever Rk. II
jugular9       = { 26117  ,3 },        // disc: Jugular Sever Rk. III
jugular10      = { 29210  ,3 },        // disc: Jugular Gash Lv 92 ROG VoA
jugular11      = { 29211  ,3 },        // disc: Jugular Gash Rk. II
jugular12      = { 29212  ,3 },        // disc: Jugular Gash Rk. III
jugular13      = { 35263  ,3 },        // disc: Jugular Lacerate Lv 97 ROG RoF
jugular14      = { 35264  ,3 },        // disc: Jugular Lacerate Rk. II
jugular15      = { 35265  ,3 },        // disc: Jugular Lacerate Rk. III
jugular16      = { 44136  ,3 },        // disc: Jugular Hack Lv 102 ROG TDS
jugular17      = { 44137  ,3 },        // disc: Jugular Hack Rk. II
jugular18      = { 44138  ,3 },        // disc: Jugular Hack Rk. III
jugular19      = { 56285  ,3 },        // disc: Jugular Strike Lv 107 Rog RoS
jugular20      = { 56286  ,3 },        // disc: Jugular Strike Rk. II
jugular21      = { 56287  ,3 },        // disc: Jugular Strike Rk. III

kneestrike     = { 801    ,4 },        // aa: knee strike

knifeplay1     = { 40297  ,3 },        // disc: Knifeplay Discipline rog 97 Rof
knifeplay2     = { 40298  ,3 },        // disc: Knifeplay Disciplinen Rk. II
knifeplay3     = { 40299  ,3 },        // disc: Knifeplay Discipline Rk. III

layhand        = { 6001   ,4 },        // aa: lay on hands

leop1          = { 6752   ,3 },        // disc: leopard claw
leop2          = { 6727   ,3 },        // disc: dragon fang
leop3          = { 10944  ,3 },        // disc: clawstriker flurry
leop4          = { 10945  ,3 },        // disc: clawstriker flurry rk ii
leop5          = { 10946  ,3 },        // disc: clawstriker flurry rk iii
leop6          = { 14796  ,3 },        // disc: wheel of fists
leop7          = { 14797  ,3 },        // disc: wheel of fists rk ii
leop8          = { 14798  ,3 },        // disc: wheel of fists rk iii
leop9          = { 18901  ,3 },        // disc: whorl of fists Lv 84 mnk UF
leop10         = { 18902  ,3 },        // disc: whorl of fists rk ii
leop11         = { 18903  ,3 },        // disc: whorl of fists rk iii
leop12         = { 25926  ,3 },        // disc: Six-Step Pattern Lv 89 mnk Hot
leop13         = { 25927  ,3 },        // disc: Six-Step Pattern rk. ii
leop14         = { 25928  ,3 },        // disc: Six-Step Pattern rk iii
leop15         = { 29033  ,3 },        // disc: Seven-Step Pattern Lv 94 mnk VoA
leop16         = { 29034  ,3 },        // disc: Seven-Step Pattern rk .ii
leop17         = { 29035  ,3 },        // disc: Seven-Step Pattern rk iii
leop18         = { 35074  ,3 },        // disc: Eight-Step Pattern Lv 96 mnk RoF
leop19         = { 35075  ,3 },        // disc: Eight-Step Pattern Rk. II
leop20         = { 35076  ,3 },        // disc: Eight-Step Pattern Rk. III
leop21         = { 43974  ,3 },        // disc: Torrent of Fists Lv 104 mnk TDS
leop22         = { 43975  ,3 },        // disc: Torrent of Fists Rk. II
leop23         = { 43976  ,3 },        // disc: Torrent of Fists Rk. III
leop24         = { 56099  ,3 },        // disc: Firestorm of Fists Lv 110 Mnk RoS
leop25         = { 56100  ,3 },        // disc: Firestorm of Fists Rk. II
leop26         = { 56101  ,3 },        // disc: Firestorm of Fists Rk. III

mendpet1       = { 58     ,4 },        // aa: mend companion
mendpet2       = { 418    ,4 },        // aa: replenish companion

monkey1        = { 22525  ,3 },        // disc: Drunken Monkey Style Lv 85 mnk UF
monkey2        = { 22526  ,3 },        // disc: Drunken Monkey Style rk .ii
monkey3        = { 22527  ,3 },        // disc: Drunken Monkey Style rk iii

opfrenzy1      = { 16918  ,3 },        // disc: Overpowering Frenzy lv 81 ber UF
opfrenzy2      = { 16919  ,3 },        // disc: Overpowering Frenzy Rk. II
opfrenzy3      = { 16920  ,3 },        // disc: Overpowering Frenzy Rk. III
opfrenzy4      = { 27260  ,3 },        // disc: Overwhelming Frenzy  Lv 86 ber Hot
opfrenzy5      = { 27261  ,3 },        // disc: Overwhelming Frenzy Rk. II
opfrenzy6      = { 27262  ,3 },        // disc: Overwhelming Frenzy Rk. III
opfrenzy7      = { 30409  ,3 },        // disc: Conquering Frenzy  Lv 91 ber VoA
opfrenzy8      = { 30410  ,3 },        // disc: Conquering Frenzy Rk. II
opfrenzy9      = { 30411  ,3 },        // disc: Conquering Frenzy Rk. III
opfrenzy10     = { 36502  ,3 },        // disc: Vanquishing Frenzy  Lv 96 ber RoF
opfrenzy11     = { 36503  ,3 },        // disc: Vanquishing Frenzy Rk. II
opfrenzy12     = { 36504  ,3 },        // disc: Vanquishing Frenzy Rk. III
opfrenzy13     = { 45221  ,3 },        // disc: Demolishing Frenzy  Lv 101 ber TDS
opfrenzy14     = { 45222  ,3 },        // disc: Demolishing Frenzy Rk. II
opfrenzy15     = { 45223  ,3 },        // disc: Demolishing Frenzy Rk. III
opfrenzy16     = { 57513  ,3 },        // disc: Mangling Frenzy Lv 106 Ber RoS
opfrenzy17     = { 57514  ,3 },        // disc: Mangling Frenzy Rk. II
opfrenzy18     = { 57515  ,3 },        // disc: Mangling Frenzy Rk. III

opstrke1       = { 15375  ,3 },        // disc: Opportunistic Strike Lv 78 war
opstrke2       = { 15376  ,3 },        // disc: Opportunistic Strike rk ii
opstrke3       = { 15377  ,3 },        // disc: Opportunistic Strike rk iii
opstrke4       = { 25027  ,3 },        // disc: Strategic Strike lv 88 war HoT
opstrke5       = { 25028  ,3 },        // disc: Strategic Strike rk ii
opstrke6       = { 25029  ,3 },        // disc: Strategic Strike rk iii
opstrke7       = { 28036  ,3 },        // disc: Vital Strike Lv 93 war VoA
opstrke8       = { 28037  ,3 },        // disc: Vital Strike rk ii
opstrke9       = { 28038  ,3 },        // disc: Vital Strike rk iii
opstrke10      = { 43045  ,3 },        // disc: Calculated Strike Lv 104 war TDS
opstrke11      = { 43046  ,3 },        // disc: Calculated Strike rk ii
opstrke12      = { 43046  ,3 },        // disc: Calculated Strike rk iii
opstrke13      = { 55045  ,3 },        // disc: Cunning Strike Lv 109 War RoS
opstrke14      = { 55046  ,3 },        // disc: Cunning Strike Rk. II
opstrke15      = { 55047  ,3 },        // disc: Cunning Strike Rk. III

pinpoint1      = { 11925  ,3 },        // disc: Pinpoint Vulnerability Lv 74 ROG
pinpoint2      = { 11926  ,3 },        // disc: Pinpoint Vulnerability Rk. II
pinpoint3      = { 11927  ,3 },        // disc: Pinpoint Vulnerability Rk. III
pinpoint4      = { 15115  ,3 },        // disc: Pinpoint Weaknesses Lv 79 ROG
pinpoint5      = { 15116  ,3 },        // disc: Pinpoint Weaknesses Rk. II
pinpoint6      = { 15117  ,3 },        // disc: Pinpoint Weaknesses Rk. III
pinpoint7      = { 19262  ,3 },        // disc: Pinpoint Vitals Lv 84 ROG
pinpoint8      = { 19263  ,3 },        // disc: Pinpoint Vitals Rk. II
pinpoint9      = { 19264  ,3 },        // disc: Pinpoint Vitals Rk. III
pinpoint10     = { 26139  ,3 },        // disc: Pinpoint Flaws Lv 89 ROG HoT
pinpoint11     = { 26140  ,3 },        // disc: Pinpoint Flaws Rk. II
pinpoint12     = { 26141  ,3 },        // disc: Pinpoint Flaws Rk. III
pinpoint13     = { 29240  ,3 },        // disc: Pinpoint Liabilities Lv 94 ROG VoA
pinpoint14     = { 29241  ,3 },        // disc: Pinpoint Liabilities Rk. II
pinpoint15     = { 29242  ,3 },        // disc: Pinpoint Liabilities Rk. III
pinpoint16     = { 35296  ,3 },        // disc: Pinpoint Deficiencies Lv 99 ROG RoF
pinpoint17     = { 35297  ,3 },        // disc: Pinpoint Deficiencies Rk. II
pinpoint18     = { 35298  ,3 },        // disc: Pinpoint Deficiencies Rk. III

potfast0       = { 77789  ,7 },        // potion: Distillate of Divine Healing I
potfast1       = { 77790  ,7 },        // potion: Distillate of Divine Healing II
potfast2       = { 77791  ,7 },        // potion: Distillate of Divine Healing III
potfast3       = { 77792  ,7 },        // potion: Distillate of Divine Healing IV
potfast4       = { 77793  ,7 },        // potion: Distillate of Divine Healing V
potfast5       = { 77794  ,7 },        // potion: Distillate of Divine Healing VI
potfast6       = { 77795  ,7 },        // potion: Distillate of Divine Healing VII
potfast7       = { 77796  ,7 },        // potion: Distillate of Divine Healing VIII
potfast8       = { 77797  ,7 },        // potion: Distillate of Divine Healing IX
potfast9       = { 77798  ,7 },        // potion: Distillate of Divine Healing X
potfast10      = { 35930  ,7 },        // potion: Distillate of Divine Healing XI
potfast11      = { 35935  ,7 },        // potion: Distillate of Divine Healing XII
potfast12      = { 35940  ,7 },        // potion: Distillate of Divine Healing XIII
potfast13      = { 40554  ,7 },        // potion: Distillate of Divine Healing XIV
potfast14      = { 56941  ,7 },        // potion: Distillate of Divine Healing XV
potfast15      = { 64612  ,7 },        // potion: Distillate of Divine Healing XVI
potfast16      = { 135337 ,7 },       // potion: Distillate of Divine Healing XVII

potover0       = { 77779  ,7 },        // potion: Distillate of Celestial Healing I
potover1       = { 77780  ,7 },        // potion: Distillate of Celestial Healing II
potover2       = { 77781  ,7 },        // potion: Distillate of Celestial Healing III
potover3       = { 77782  ,7 },        // potion: Distillate of Celestial Healing IV
potover4       = { 77783  ,7 },        // potion: Distillate of Celestial Healing V
potover5       = { 77784  ,7 },        // potion: Distillate of Celestial Healing VI
potover6       = { 77785  ,7 },        // potion: Distillate of Celestial Healing VII
potover7       = { 77786  ,7 },        // potion: Distillate of Celestial Healing VIII
potover8       = { 77787  ,7 },        // potion: Distillate of Celestial Healing IX
potover9       = { 77788  ,7 },        // potion: Distillate of Celestial Healing X
potover10      = { 35931  ,7 },        // potion: Distillate of Celestial Healing XI
potover11      = { 35936  ,7 },        // potion: Distillate of Celestial Healing XII
potover12      = { 35941  ,7 },        // potion: Distillate of Celestial Healing XIII
potover13      = { 40555  ,7 },        // potion: Distillate of Celestial Healing XIV
potover14      = { 56942  ,7 },        // potion: Distillate of Celestial Healing XV
potover15      = { 64613  ,7 },        // potion: Distillate of Celestial Healing XVI
potover16      = { 135338 ,7 },        // potion: Distillate of Celestial Healing XVII

power1         = { 10260  ,5 },        // spell: challenge for power
power2         = { 10261  ,5 },        // spell: challenge for power rk. ii
power3         = { 10262  ,5 },        // spell: challenge for power rk. iii
power4         = { 15163  ,5 },        // spell: trial for power
power5         = { 15164  ,5 },        // spell: trial for power rk. ii
power6         = { 15165  ,5 },        // spell: trial for power rk. iii
power7         = { 19316  ,5 },        // spell: Charge for Power Lv 82 shd UF
power8         = { 19317  ,5 },        // spell: Charge for Power rk. ii
power9         = { 19318  ,5 },        // spell: Charge for Power rk. iii
power10        = { 25586  ,5 },        // spell: Confrontation for Power Lv 87 shd HoT
power11        = { 25587  ,5 },        // spell: Confrontation for Power rk. ii
power12        = { 25588  ,5 },        // spell: Confrontation for Powerr rk. iii
power13        = { 28663  ,5 },        // spell: Provocation for Power Lv 92 shd VoA
power14        = { 28664  ,5 },        // spell: Provocation for Power rk. ii
power15        = { 28665  ,5 },        // spell: Provocation for Power rk .iii
power16        = { 34693  ,5 },        // spell: Demand for Power  Lv 97 shd RoF
power17        = { 34694  ,5 },        // spell: Demand for Power rk. ii
power18        = { 34695  ,5 },        // spell: Demand for Power rk. iii
power19        = { 43619  ,5 },        // spell: Impose for Power  Lv 102 shd TDS
power20        = { 43620  ,5 },        // spell: Impose for Power rk. ii
power21        = { 43621  ,5 },        // spell: Impose for Power rk. iii
power22        = { 55713  ,5 },        // spell: Refute for Power Lv 107 Shd RoS
power23        = { 55714  ,5 },        // spell: Refute for Power Rk. II
power24        = { 55715  ,5 },        // spell: Refute for Power Rk. III

prowar1        = { 4608   ,3 },        // disc: provoke
prowar2        = { 4681   ,3 },        // disc: bellow
prowar3        = { 4682   ,3 },        // disc: berate
prowar4        = { 4697   ,3 },        // disc: incite
prowar5        = { 5015   ,3 },        // disc: bellow of the mastruq
prowar6        = { 5016   ,3 },        // disc: ancient: chaos cry
prowar7        = { 6173   ,3 },        // disc: bazu bellow
prowar8        = { 10974  ,3 },        // disc: scowl
prowar9        = { 10975  ,3 },        // disc: scowl rk ii
prowar10       = { 10976  ,3 },        // disc: scowl rk iii
prowar11       = { 15360  ,3 },        // disc: sneer
prowar12       = { 15361  ,3 },        // disc: sneer rk ii
prowar13       = { 15362  ,3 },        // disc: sneer rk iii
prowar14       = { 19537  ,3 },        // disc: bazu bluster Lv 81 war UF
prowar15       = { 19538  ,3 },        // disc: bazu bluster rk. ii
prowar16       = { 19539  ,3 },        // disc: bazu bluster rk. iii
prowar17       = { 19531  ,3 },        // disc: jeer rk i Lv 85 war UF
prowar18       = { 19532  ,3 },        // disc: jeer rk ii
prowar19       = { 19533  ,3 },        // disc: jeer rk iii
prowar20       = { 25018  ,3 },        // disc: bazu roar Lv 86 war HoT
prowar21       = { 25019  ,3 },        // disc: bazu roar rk. ii
prowar22       = { 25020  ,3 },        // disc: bazu roar rk. iii
prowar23       = { 25045  ,3 },        // disc: scoff Lv 90 war HoT
prowar24       = { 25046  ,3 },        // disc: scoff rk. ii
prowar25       = { 25047  ,3 },        // disc: scoff rk. iii
prowar26       = { 28021  ,3 },        // disc: Grendlaen Roar Lv 91 war VoA
prowar27       = { 28022  ,3 },        // disc: Grendlaen Roar rk. ii
prowar28       = { 28023  ,3 },        // disc: Grendlaen Roar rk. iii
prowar29       = { 28060  ,3 },        // disc: Scorn Lv 95 war VoA
prowar30       = { 28061  ,3 },        // disc: Scorn rk. ii
prowar31       = { 28062  ,3 },        // disc: Scorn rk. iii
prowar32       = { 34015  ,3 },        // disc: Krondal's Roar Lv 96 war RoF
prowar33       = { 34016  ,3 },        // disc: Krondal's Roar rk. ii
prowar34       = { 34017  ,3 },        // disc: Krondal's Roar rk. iii
prowar35       = { 34027  ,3 },        // disc: Ridicule Lv 98 war RoF
prowar36       = { 34028  ,3 },        // disc: Ridicule rk. ii
prowar37       = { 34029  ,3 },        // disc: Ridicule rk. iii
prowar38       = { 43021  ,3 },        // disc: Cyclone Roar
prowar39       = { 43022  ,3 },        // disc: Cyclone Roar rk. ii
prowar40       = { 43023  ,3 },        // disc: Cyclone Roar rk. iii
prowar41       = { 43033  ,3 },        // disc: Insult Lv 103 war TDS
prowar42       = { 43034  ,3 },        // disc: Insult rk. ii
prowar43       = { 43035  ,3 },        // disc: Insult rk. iii
prowar44       = { 55027  ,3 },        // disc: Slander Lv 108 War RoS
prowar45       = { 55028  ,3 },        // disc: Slander Rk. II
prowar46       = { 55029  ,3 },        // disc: Slander Rk. III
prowar47       = { 55009  ,3 },        // disc: Kluzen's Roar Lv 106 war RoS
prowar48       = { 55010  ,3 },        // disc: Kluzen's Roar Rk. II
prowar49       = { 55011  ,3 },        // disc: Kluzen's Roar Rk. III

rake1          = { 8782   ,3 },        // disc: rake
rake2          = { 14158  ,3 },        // disc: harrow
rake3          = { 14159  ,3 },        // disc: harrow rk. ii
rake4          = { 14160  ,3 },        // disc: harrow rk. iii
rake5          = { 18170  ,3 },        // disc: foray Lv 85 bst UF
rake6          = { 18171  ,3 },        // disc: foray rk. ii
rake7          = { 18172  ,3 },        // disc: foray rk. iii
rake8          = { 27219  ,3 },        // disc: rush Lv 90 bst Hot
rake9          = { 27220  ,3 },        // disc: rush rk. ii
rake10         = { 27221  ,3 },        // disc: rush rk. iii
rake11         = { 30368  ,3 },        // disc: Barrage Lv 95 bst Hot
rake12         = { 30369  ,3 },        // disc: Barrage rk. ii
rake13         = { 30370  ,3 },        // disc: Barrage rk. iii
rake14         = { 36425  ,3 },        // disc: Pummel Lv 100 bst RoF
rake15         = { 36426  ,3 },        // disc: Pummel rk. ii
rake16         = { 36427  ,3 },        // disc: Pummel rk. iii
rake17         = { 45164  ,3 },        // disc: Maul Lv 104 bst TDS
rake18         = { 45165  ,3 },        // disc: Maul rk. ii
rake19         = { 45166  ,3 },        // disc: Maul rk. iii
rake20         = { 57450  ,3 },        // disc: Mangle Lv 109 Bst RoS
rake21         = { 57451  ,3 },        // disc: Mangle Rk. II
rake22         = { 57452  ,3 },        // disc: Mangle Rk. III

rallos1        = { 19741  ,3 },        // disc: Axe of Rallos Lv 85 ber UF
rallos2        = { 19742  ,3 },        // disc: Axe of Rallos Rk. II
rallos3        = { 19743  ,3 },        // disc: Axe of Rallos Rk. III
rallos4        = { 27293  ,3 },        // disc: Axe of Graster Lv 90 ber HoT
rallos5        = { 27294  ,3 },        // disc: Axe of Graster Rk. II
rallos6        = { 27295  ,3 },        // disc: Axe of Graster Rk. III
rallos7        = { 30448  ,3 },        // disc: Axe of Illdaera Lv 95 ber VoA
rallos8        = { 30449  ,3 },        // disc: Axe of Illdaera Rk. II
rallos9        = { 30450  ,3 },        // disc: Axe of Illdaera Rk. III
rallos10       = { 30658  ,3 },        // disc: Axe of Zurel Lv 100 ber VoA
rallos11       = { 30659  ,3 },        // disc: Axe of Zurel Rk. II
rallos12       = { 30660  ,3 },        // disc: Axe of Zurel Rk. III
rallos13       = { 45284  ,3 },        // disc: Axe of Numicia Lv 105 ber TDS
rallos14       = { 45285  ,3 },        // disc: Axe of Numicia Rk. II
rallos15       = { 45286  ,3 },        // disc: Axe of Numicia Rk. III
rallos16       = { 57564  ,3 },        // disc: Axe of Rekatok Lv 110 Ber RoS
rallos17       = { 57565  ,3 },        // disc: Axe of Rekatok Rk. II
rallos18       = { 57566  ,3 },        // disc: Axe of Rekatok Rk. III

rightidg1      = { 25345  ,3 },        // disc: Righteous Indignation Lv 88 pal HoT
rightidg2      = { 25346  ,3 },        // disc: Righteous Indignation Rk. II
rightidg3      = { 25347  ,3 },        // disc: Righteous Indignation Rk. III
rightidg4      = { 28398  ,3 },        // disc: Righteous Vexation Lv 93 pal VoA
rightidg5      = { 28399  ,3 },        // disc: Righteous Vexation Rk. II
rightidg6      = { 28400  ,3 },        // disc: Righteous Vexation Rk. III
rightidg7      = { 34401  ,3 },        // disc: Righteous Umbrage Lv 98 pal RoF
rightidg8      = { 34402  ,3 },        // disc: Righteous Umbrage Rk. II
rightidg9      = { 34403  ,3 },        // disc: Righteous Umbrage Rk. III
rightidg10     = { 55389  ,3 },        // disc: Righteous Condemnation Lv 108 pal RoS
rightidg11     = { 55390  ,3 },        // disc: Righteous Condemnation Rk. II
rightidg12     = { 55391  ,3 },        // disc: Righteous Condemnation Rk. III

ravens         = { 987    ,4 },        // aa: raven's claw

slapface1      = { 27269  ,3 },        // disc: slap in the face Lv 87 ber HoT
slapface2      = { 27270  ,3 },        // disc: slap in the face rk. ii
slapface3      = { 27271  ,3 },        // disc: slap in the face rk. iii
slapface4      = { 30418  ,3 },        // disc: Kick in the Teeth Lv 92 ber VoA
slapface5      = { 30419  ,3 },        // disc: Kick in the Teeth rk. ii
slapface6      = { 30420  ,3 },        // disc: Kick in the Teeth rk. iii
slapface7      = { 36508  ,3 },        // disc: Punch in the Throat  Lv 97 ber RoF
slapface8      = { 36509  ,3 },        // disc: Punch in the Throat rk. ii
slapface9      = { 36510  ,3 },        // disc: Punch in the Throat rk. iii
slapface10     = { 45227  ,3 },        // disc: Kick in the Shins  Lv 102 ber TDS
slapface11     = { 45228  ,3 },        // disc: Kick in the Shins rk. ii
slapface12     = { 45229  ,3 },        // disc: Kick in the Shins rk. iii
slapface13     = { 57519  ,3 },        // disc: Sucker Punch Lv 107 Ber RoS
slapface14     = { 57520  ,3 },        // disc: Sucker Punch Rk. II
slapface15     = { 57521  ,3 },        // disc: Sucker Punch Rk. III

selos          = { 8205   ,4 },        // aa: selos

steely1        = { 19137  ,5 },        //spell Steely Stance lv 84 pal/shd UF
steely2        = { 19138  ,5 },        //spell Steely Stance Rk. II
steely3        = { 19139  ,5 },        //spell Steely Stance Rk. III
steely4        = { 25270  ,5 },        //spell Stubborn Stance lv 89 pal/shd HoT
steely5        = { 25271  ,5 },        //spell Stubborn Stance Rk. II
steely6        = { 25272  ,5 },        //spell Stubborn Stance Rk. III
steely7        = { 28314  ,5 },        //spell Stoic Stance lv 94 pal/shd VoA
steely8        = { 28315  ,5 },        //spell Stoic Stance Rk. II
steely9        = { 28316  ,5 },        //spell Stoic Stance Rk. III
steely10       = { 34320  ,5 },        //spell Steadfast Stance lv 99 pal/shd RoF
steely11       = { 34321  ,5 },        //spell Steadfast Stance Rk. II
steely12       = { 34322  ,5 },        //spell Steadfast Stance Rk. III
steely13       = { 43289  ,5 },        //spell Staunch Stance lv 104 pal/shd TDS
steely14       = { 43290  ,5 },        //spell Staunch Stance Rk. II
steely15       = { 43291  ,5 },        //spell Staunch Stance Rk. III
steely16       = { 55320  ,5 },        // spell: Defiant Stance Lv 109 Pal/Shd RoS
steely17       = { 55321  ,5 },        // spell: Defiant Stance Rk. II
steely18       = { 55322  ,5 },        // spell: Defiant Stance Rk. III


strike1        = { 4659   ,3 },        // disc: sneak attack
strike2        = { 4685   ,3 },        // disc: thief's vengeance
strike3        = { 4686   ,3 },        // disc: assassin strike
strike4        = { 5017   ,3 },        // disc: kyv strike
strike5        = { 5018   ,3 },        // disc: ancient chaos strike
strike6        = { 6174   ,3 },        // disc: daggerfall
strike7        = { 8470   ,3 },        // disc: razor arc
strike8        = { 15133  ,3 },        // disc: swiftblade
strike9        = { 15134  ,3 },        // disc: swiftblade rk. ii
strike10       = { 15135  ,3 },        // disc: swiftblade rk. iii
strike11       = { 19280  ,3 },        // disc: Daggerlunge Lv 85 rog UF
strike12       = { 19281  ,3 },        // disc: Daggerlunge rk. ii
strike13       = { 19282  ,3 },        // disc: Daggerlunge rk. iii
strike14       = { 26148  ,3 },        // disc: Daggerswipe Lv 90 rog HoT
strike15       = { 26149  ,3 },        // disc: Daggerswipe rk. ii
strike16       = { 26150  ,3 },        // disc: Daggerswipe rk. iii
strike17       = { 29249  ,3 },        // disc: Daggerstrike Lv 95 rog VoA
strike18       = { 29250  ,3 },        // disc: Daggerstrike rk. ii
strike19       = { 29251  ,3 },        // disc: Daggerstrike rk. iii
strike20       = { 35305  ,3 },        // disc: Daggerthrust Lv 100 rog RoF
strike21       = { 35306  ,3 },        // disc: Daggerthrust rk. ii
strike22       = { 35307  ,3 },        // disc: Daggerthrust rk. iii
strike23       = { 44178  ,3 },        // disc: Daggergash Lv 105 rog TDS
strike24       = { 44179  ,3 },        // disc: Daggergash rk. ii
strike25       = { 44180  ,3 },        // disc: Daggergash rk. iii
strike26       = { 56330  ,3 },        // disc: Daggerslice Lv 110 Rog RoS
strike27       = { 56331  ,3 },        // disc: Daggerslice Rk. II
strike28       = { 56332  ,3 },        // disc: Daggerslice Rk. III

stunber1       = { 4931   ,3 },        // disc: head strike
stunber2       = { 4932   ,3 },        // disc: head pummel
stunber3       = { 4933   ,3 },        // disc: head crush
stunber4       = { 6170   ,3 },        // disc: mind strike
stunber5       = { 10917  ,3 },        // disc: temple blow
stunber6       = { 10918  ,3 },        // disc: temple blow rk. ii
stunber7       = { 10919  ,3 },        // disc: temple blow rk. iii
stunber8       = { 14183  ,3 },        // disc: temple strike
stunber9       = { 14184  ,3 },        // disc: temple strike rk. ii
stunber10      = { 14185  ,3 },        // disc: temple strike rk. iii
stunber11      = { 18204  ,3 },        // disc: Temple Bash Lv 83 ber UF
stunber12      = { 18205  ,3 },        // disc: Temple Bash rk. ii
stunber13      = { 18206  ,3 },        // disc: Temple Bash rk. iii
stunber14      = { 27281  ,3 },        // disc: Temple Chop Lv 88 ber HoT
stunber15      = { 27282  ,3 },        // disc: Temple Chop rk. ii
stunber16      = { 27283  ,3 },        // disc: Temple Chop rk. iii
stunber17      = { 30430  ,3 },        // disc: Temple Smash Lv 93 ber VoA
stunber18      = { 30431  ,3 },        // disc: Temple Smash rk. ii
stunber19      = { 30432  ,3 },        // disc: Temple Smash rk. iii
stunber20      = { 36520  ,3 },        // disc: Temple Crush Lv 98 ber RoF
stunber21      = { 36521  ,3 },        // disc: Temple Crush rk. ii
stunber22      = { 36522  ,3 },        // disc: Temple Crush rk. iii
stunber23      = { 45251  ,3 },        // disc: Temple Demolish Lv 103 ber TDS
stunber24      = { 45252  ,3 },        // disc: Temple Demolish rk. ii
stunber25      = { 45253  ,3 },        // disc: Temple Demolish rk. iii
stunber26      = { 57534  ,3 },        // disc: Temple Slam Lv 108 Ber RoS
stunber27      = { 57535  ,3 },        // disc: Temple Slam Rk. II
stunber28      = { 57536  ,3 },        // disc: Temple Slam Rk. III

stunmnk1       = { 469    ,4 },        // aa: stunning kick
stunmnk2       = { 600    ,4 },        // aa: resounding kick

stunaas1       = { 73     ,4 },        // aa: divine stun
stunaas2       = { 702    ,4 },        // aa: hand of disruption
stunaas3       = { 3826   ,4 },        // aa: force of disruption

stunpal1       = { 216    ,5 },        // spell: stun
stunpal2       = { 123    ,5 },        // spell: holy might
stunpal3       = { 3975   ,5 },        // spell: force of akera
stunpal4       = { 3245   ,5 },        // spell: force of akilae
stunpal5       = { 4977   ,5 },        // spell: ancient force of chaos
stunpal6       = { 5284   ,5 },        // spell: force of piety
stunpal7       = { 5299   ,5 },        // spell: ancient force of jeron
stunpal8       = { 10158  ,5 },        // spell: sacred force
stunpal9       = { 10159  ,5 },        // spell: sacred force rk. ii
stunpal10      = { 10160  ,5 },        // spell: sacred force rk. iii
stunpal11      = { 11851  ,5 },        // spell: force of prexus
stunpal12      = { 11852  ,5 },        // spell: force of prexus rk. ii
stunpal13      = { 11853  ,5 },        // spell: force of prexus rk. iii
stunpal14      = { 14942  ,5 },        // spell: solemn force
stunpal15      = { 14943  ,5 },        // spell: solemn force rk. ii
stunpal16      = { 14944  ,5 },        // spell: solemn force rk. iii
stunpal17      = { 14984  ,5 },        // spell: Force of Timorous
stunpal18      = { 14985  ,5 },        // spell: Force of Timorous  rk. ii
stunpal19      = { 14986  ,5 },        // spell: Force of Timorous  rk. iii
stunpal20      = { 19056  ,5 },        // spell: Devout Force Lv 81 pal UF
stunpal21      = { 19057  ,5 },        // spell: Devout Force rk. ii
stunpal22      = { 19058  ,5 },        // spell: Devout Force rk. iii
stunpal23      = { 19098  ,5 },        // spell: Force of the Crying Seas Lv 85 pal UF
stunpal24      = { 19099  ,5 },        // spell: Force of the Crying Seas rk. ii
stunpal25      = { 19100  ,5 },        // spell: Force of the Crying Seas rk. iii
stunpal26      = { 25282  ,5 },        // spell: Earnest Force Lv 86 pal HoT
stunpal27      = { 25283  ,5 },        // spell: Earnest Force rk. ii
stunpal28      = { 25284  ,5 },        // spell: Earnest Force rk. iii
stunpal29      = { 25375  ,5 },        // spell: Force of Marr Lv 90 pal Hot
stunpal30      = { 25376  ,5 },        // spell: Force of Marr rk. ii
stunpal31      = { 25377  ,5 },        // spell: Force of Marr rk. iii
stunpal32      = { 28326  ,5 },        // spell: Zealous Force Lv 91 pal VoA
stunpal33      = { 28327  ,5 },        // spell: Zealous Force rk. ii
stunpal34      = { 28328  ,5 },        // spell: Zealous Force rk. iii
stunpal35      = { 28446  ,5 },        // spell: Force of Oseka Lv 95 pal VoA
stunpal36      = { 28447  ,5 },        // spell: Force of Oseka rk. ii
stunpal37      = { 28448  ,5 },        // spell: Force of Oseka rk. iii
stunpal38      = { 34332  ,5 },        // spell: Reverent Force Lv 96 pal VoA
stunpal39      = { 34333  ,5 },        // spell: Reverent Force rk. ii
stunpal40      = { 34334  ,5 },        // spell: Reverent Force rk. iii
stunpal41      = { 34452  ,5 },        // spell: Force of the Iceclad Lv 100 pal RoF
stunpal42      = { 34453  ,5 },        // spell: Force of the Iceclad rk. ii
stunpal43      = { 34454  ,5 },        // spell: Force of the Iceclad rk .iii
stunpal44      = { 43412  ,5 },        // spell: Force of the Darkened Sea Lv 105 pal TDS
stunpal45      = { 43413  ,5 },        // spell: Force of the Darkened Sea rk. ii
stunpal46      = { 43414  ,5 },        // spell: Force of the Darkened Sea rk .iii
stunpal47      = { 55479  ,5 },        // spell: Force of the Timorous Deep Lv 110 Pal RoS
stunpal48      = { 55480  ,5 },        // spell: Force of the Timorous Deep Rk. II
stunpal49      = { 55481  ,5 },        // spell: Force of the Timorous Deep Rk. III

synergy1       = { 18895  ,3 },        // disc: Calanin's Synergy Lv 81 mnk UF
synergy2       = { 18896  ,3 },        // disc: Calanin's Synergy Rk. II
synergy3       = { 18897  ,3 },        // disc: Calanin's Synergy Rk. III
synergy4       = { 25907  ,3 },        // disc: Dreamwalker's Synergy Lv 86 mnk HoT
synergy5       = { 25908  ,3 },        // disc: Dreamwalker's Synergy Rk. II
synergy6       = { 25909  ,3 },        // disc: Dreamwalker's Synergy Rk. III
synergy7       = { 29002  ,3 },        // disc: Veilwalker's Synergy Lv 91 mnk VoA
synergy8       = { 29003  ,3 },        // disc: Veilwalker's Synergy Rk. II
synergy9       = { 29004  ,3 },        // disc: Veilwalker's Synergy Rk. III
synergy10      = { 35043  ,3 },        // disc: Shadewalker's Synergy mnk RoF
synergy11      = { 35044  ,3 },        // disc: Shadewalker's Synergy Rk. II
synergy12      = { 35045  ,3 },        // disc: Shadewalker's Synergy Rk. III
synergy13      = { 43943  ,3 },        // disc: Doomwalker's Synergy mnk Lv 101 TDS
synergy14      = { 43944  ,3 },        // disc: Doomwalker's Synergy Rk. II
synergy15      = { 43945  ,3 },        // disc: Doomwalker's Synergy Rk. III
synergy16      = { 56058  ,3 },        // disc: Firewalker's Synergy Lv 106 Mnk RoS
synergy17      = { 56059  ,3 },        // disc: Firewalker's Synergy Rk. II
synergy18      = { 56060  ,3 },        // disc: Firewalker's Synergy Rk. III

terror1        = { 1221   ,5 },        // spell: terror of darkness
terror2        = { 1222   ,5 },        // spell: terror of shadows
terror3        = { 1223   ,5 },        // spell: terror of death
terror4        = { 1224   ,5 },        // spell: terror of terris
terror5        = { 3405   ,5 },        // spell: terror of thule
terror6        = { 5329   ,5 },        // spell: terror of discord
terror7        = { 10257  ,5 },        // spell: terror of vergalid
terror8        = { 10258  ,5 },        // spell: terror of vergalid rk. ii
terror9        = { 10259  ,5 },        // spell: terror of vergalid rk. iii
terror10       = { 15160  ,5 },        // spell: terror of the Soulbleeder
terror11       = { 15161  ,5 },        // spell: terror of the Soulbleeder rk. ii
terror12       = { 15162  ,5 },        // spell: terror of the Soulbleeder rk. iii
terror13       = { 19313  ,5 },        // spell: Terror of Jelvalak Lv 81 sk UF
terror14       = { 19314  ,5 },        // spell: Terror of Jelvalak rk. ii
terror15       = { 19315  ,5 },        // spell: Terror of Jelvalak rk. iii
terror16       = { 25580  ,5 },        // spell: Terror of Rerekalen lv 86 sk HoT
terror17       = { 25581  ,5 },        // spell: Terror of Rerekalen rk. ii
terror18       = { 25582  ,5 },        // spell: Terror of Rerekalen rk. iii
terror19       = { 28657  ,5 },        // spell: Terror of Desalin Lv 91 sk VoA
terror20       = { 28658  ,5 },        // spell: Terror of Desalin rk. ii
terror21       = { 28659  ,5 },        // spell: Terror of Desalin rk. iii
terror22       = { 34687  ,5 },        // spell: Terror of Poira Lv 96 sk RoF
terror23       = { 34688  ,5 },        // spell: Terror of Poira rk. ii
terror24       = { 34689  ,5 },        // spell: Terror of Poira rk. iii
terror25       = { 43607  ,5 },        // spell: Terror of Narus Lv 101 sk TDS
terror26       = { 43608  ,5 },        // spell: Terror of Narus rk. ii
terror27       = { 43609  ,5 },        // spell: Terror of Narus rk. iii
terror28       = { 55689  ,5 },        // spell: Terror of Kra'Du Lv 106 Sk RoS
terror29       = { 55690  ,5 },        // spell: Terror of Kra'Du Rk. II
terror30       = { 55691  ,5 },        // spell: Terror of Kra'Du Rk. III

thiefeye1      = { 8001   ,3 },        // disc: thief's eye
thiefeye2      = { 40294  ,3 },        // disc: Thief's Vision
thiefeye3      = { 40295  ,3 },        // disc: Thief's Vision rk. II
thiefeye4      = { 40296  ,3 },        // disc: Thief's Vision rk. III

throat1        = { 10968  ,3 },        // disc: throat jab
throat2        = { 10969  ,3 },        // disc: throat jab rk ii
throat3        = { 10970  ,3 },        // disc: throat jab rk iii

tstone         = { 5225   ,3 },        // disc: throw stone
twisted        = { 670    ,4 },        // aa: twisted shank

volley1        = { 6754   ,3 },        // disc: rage volley
volley2        = { 6729   ,3 },        // disc: destroyer's volley
volley3        = { 10926  ,3 },        // disc: giant slayer's volley
volley4        = { 10927  ,3 },        // disc: giant slayer's volley rk ii
volley5        = { 10928  ,3 },        // disc: giant slayer's volley rk iii
volley6        = { 11928  ,3 },        // disc: annihilator's volley
volley7        = { 11929  ,3 },        // disc: annihilator's volley rk ii
volley8        = { 11930  ,3 },        // disc: annihilator's volley rk iii
volley9        = { 14195  ,3 },        // disc: decimator's volley
volley10       = { 14196  ,3 },        // disc: decimator's volley rk ii
volley11       = { 14197  ,3 },        // disc: decimator's volley rk iii
volley12       = { 18216  ,3 },        // disc: Eradicator's Volley Lv 84 ber UF
volley13       = { 18217  ,3 },        // disc: Eradicator's Volley Rk. II
volley14       = { 18218  ,3 },        // disc: Eradicator's Volley Rk. III
volley15       = { 27287  ,3 },        // disc: Savage Volley Lv 89 ber HoT
volley16       = { 27288  ,3 },        // disc: Savage Volley Rk. II
volley17       = { 27289  ,3 },        // disc: Savage Volley Rk. III
volley18       = { 30442  ,3 },        // disc: Sundering Volley Lv 94 ber VoA
volley19       = { 30443  ,3 },        // disc: Sundering Volley Rk. II
volley20       = { 30444  ,3 },        // disc: Sundering Volley Rk. III
volley21       = { 36523  ,3 },        // disc: Brutal Volley Lv 99 ber RoF
volley22       = { 36524  ,3 },        // disc: Brutal Volley Rk. II
volley23       = { 36525  ,3 },        // disc: Brutal Volley Rk. III
volley24       = { 45263  ,3 },        // disc: Demolishing Volley Lv 104 ber TDS
volley25       = { 45264  ,3 },        // disc: Demolishing Volley Rk. II
volley26       = { 45265  ,3 },        // disc: Demolishing Volley Rk. III
volley27       = { 57546  ,3 },        // disc: Mangling Volley Lv 109 Ber RoS
volley28       = { 57547  ,3 },        // disc: Mangling Volley Rk. II
volley29       = { 57548  ,3 },        // disc: Mangling Volley Rk. III

vigber1        = { 19753  ,3 },        // disc: Vigorous Axe Throw Lv 83 ber UF
vigber2        = { 19754  ,3 },        // disc: Vigorous Axe Throw Rk. II
vigber3        = { 19755  ,3 },        // disc: Vigorous Axe Throw Rk. III
vigber4        = { 27278  ,3 },        // disc: Energetic Axe Throw Lv 88 ber HoT
vigber5        = { 27279  ,3 },        // disc: Energetic Axe Throw Rk. II
vigber6        = { 27280  ,3 },        // disc: Energetic Axe Throw Rk. III
vigber7        = { 30427  ,3 },        // disc: Spirited Axe Throw Lv 93 ber VoA
vigber8        = { 30428  ,3 },        // disc: Spirited Axe Throw Rk. II
vigber9        = { 30429  ,3 },        // disc: Spirited Axe Throw Rk. III
vigber10       = { 36517  ,3 },        // disc: Brutal Axe Throw Lv 98 ber RoF
vigber11       = { 36518  ,3 },        // disc: Brutal Axe Throw Rk. II
vigber12       = { 36519  ,3 },        // disc: Brutal Axe Throw Rk. III
vigber13       = { 45248  ,3 },        // disc: Demolishing Axe Throw Lv 103 ber TDS
vigber14       = { 45249  ,3 },        // disc: Demolishing Axe Throw Rk. II
vigber15       = { 45250  ,3 },        // disc: Demolishing Axe Throw Rk. III
vigber16       = { 57531  ,3 },        // disc: Mangling Axe Throw    Lv 198 Ber RoS
vigber17       = { 57532  ,3 },        // disc: Mangling Axe Throw Rk. II
vigber18       = { 57533  ,3 },        // disc: Mangling Axe Throw Rk. III

vigmnk1        = { 19826  ,3 },        // disc: Vigorous Shuriken
vigmnk2        = { 19827  ,3 },        // disc: Vigorous Shuriken Rk. II
vigmnk3        = { 19828  ,3 },        // disc: Vigorous Shuriken Rk. III

vigrog1        = { 19871  ,3 },        // disc: Vigorous Dagger-Throw
vigrog2        = { 19872  ,3 },        // disc: Vigorous Dagger-Throw Rk. II
vigrog3        = { 19873  ,3 },        // disc: Vigorous Dagger-Throw Rk. III
vigrog4        = { 26124  ,3 },        // disc: Vigorous Dagger-Strike
vigrog5        = { 26125  ,3 },        // disc: Vigorous Dagger-Strike Rk. II
vigrog6        = { 26126  ,3 },        // disc: Vigorous Dagger-Strike Rk. III
vigrog7        = { 29225  ,3 },        // disc: Energetic Dagger-Strike
vigrog8        = { 29226  ,3 },        // disc: Energetic Dagger-Strike Rk. II
vigrog9        = { 29227  ,3 },        // disc: Energetic Dagger-Strike Rk. III
vigrog10       = { 35281  ,3 },        // disc: Energetic Dagger-Throw 98 Rog RoF
vigrog11       = { 35282  ,3 },        // disc: Energetic Dagger-Throw Rk. II
vigrog12       = { 35283  ,3 },        // disc: Energetic Dagger-Throw Rk. III
vigrog13       = { 44148  ,3 },        // disc: Exuberant Dagger-Throw 103 Rog TDS
vigrog14       = { 44149  ,3 },        // disc: Exuberant Dagger-Throw Rk. II
vigrog15       = { 44150  ,3 },        // disc: Exuberant Dagger-Throw Rk. III
vigrog16       = { 56300  ,3 },        // disc: Forceful Dagger-Throw 108 Rog RoS
vigrog17       = { 56301  ,3 },        // disc: Forceful Dagger-Throw Rk. II
vigrog18       = { 56302  ,3 },        // disc: Forceful Dagger-Throw Rk. III

withstand1     = { 19131  ,3 },        // disc: Withstand Lv 83 pal/sk UF
withstand2     = { 19132  ,3 },        // disc: Withstand Rk. II
withstand3     = { 19133  ,3 },        // disc: Withstand Rk. III
withstand4     = { 25264  ,3 },        // disc: Defy Lv 88 pal/sk HoT
withstand5     = { 25265  ,3 },        // disc: Defy Rk. II
withstand6     = { 25266  ,3 },        // disc: Defy Rk. III
withstand7     = { 28308  ,3 },        // disc: Renounce Lv 93 pal/sk VoA
withstand8     = { 28309  ,3 },        // disc: Renounce Rk. II
withstand9     = { 28310  ,3 },        // disc: Renounce Rk. III
withstand10    = { 34314  ,3 },        // disc: Reprove Lv 98 pal/sk VoA
withstand11    = { 34315  ,3 },        // disc: Reprove Rk. II
withstand12    = { 34316  ,3 },        // disc: Reprove Rk. III
withstand13    = { 43283  ,3 },        // disc: Repel Lv 103 pal/sk TDS
withstand14    = { 43284  ,3 },        // disc: Repel Rk. II
withstand15    = { 43285  ,3 },        // disc: Repel Rk. III
withstand16    = { 55317  ,3 },        // disc: Spurn Lv 108 Pal/Sk RoS
withstand17    = { 55318  ,3 },        // disc: Spurn Rk. II
withstand18    = { 55319  ,3 },        // dics: Spurn Rk. III

yaulp          = { 489    ,4 },        // aa: yaulp

sbkstab        = { 8      ,2 },        // skill: backstab
sbash          = { 10     ,2 },        // skill: bash
sbegging       = { 67     ,2 },        // skill: begging
sdisarm        = { 16     ,2 },        // skill: disarm
sdrpunch       = { 21     ,2 },        // skill: dragon punch
sestrike       = { 23     ,2 },        // skill: eagle strike
sfeign         = { 25     ,2 },        // skill: feign death
sflykick       = { 26     ,2 },        // skill: flying kick
sforage        = { 27     ,2 },        // skill: forage
sfrenzy        = { 74     ,2 },        // skill: frenzy
shide          = { 29     ,2 },        // skill: hide
sintim         = { 71     ,2 },        // skill: intimidation
skick          = { 30     ,2 },        // skill: kick
smend          = { 32     ,2 },        // skill: mend
sppocket       = { 48     ,2 },        // skill: pick pockets
srndkick       = { 38     ,2 },        // skill: round kick
ssensetr       = { 62     ,2 },        // skill: sense trap
sslam          = { 111    ,2 },        // skill: slam
ssneak         = { 42     ,2 },        // skill: sneak
staunt         = { 73     ,2 },        // skill: taunt
stigclaw       = { 52     ,2 };        // skill: tigerclaw

#define DECLARE_ABILITY_OPTION( __var, __key, __help, __default, __show) char* __var[]  = {\
                                                                                  __key, \
                                                                                  __help, \
                                                                                  __default, \
                                                                                  __show, \
                                                                                  };
#define REGISTER_ABILITY_OPTION( __var, __func, __ability ) MapInsert(&CmdListe, Option(__var[0], __var[1], __var[2], __var[3], __func, __ability));

DECLARE_ABILITY_OPTION(pDEBUG, "debug", "[ON/OFF]?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[debug].Length},1,0]}");
DECLARE_ABILITY_OPTION(pAGGRO, "aggro", "[ON/OFF]?", "${If[${Select[${Me.Class.ShortName},WAR,PAL,SHD]},1,0]}", "${If[${meleemvi[plugin]},1,0]}");
DECLARE_ABILITY_OPTION(pAGGRP, "aggropri", "[ID] Primary (Aggro)?", "0", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && ${meleemvi[aggro]},1,0]}");
DECLARE_ABILITY_OPTION(pAGGRS, "aggrosec", "[ID] Offhand (Aggro)?", "0", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && ${meleemvi[aggro]} && ${meleemvi[aggropri]},1,0]}");
DECLARE_ABILITY_OPTION(pARROW, "arrow", "[ID] item?", "0", "${If[${meleemvi[plugin]} && (${Me.Skill[archery]} || ${Me.Skill[throwing]}),1,0]}");
DECLARE_ABILITY_OPTION(pASSAS, "assassinate", "Sneak/Hide/Behind/Strike/Stab [ON/OFF]?", "${If[${Me.Skill[backstab]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && ${meleemvi[backstab]},1,0]}");
DECLARE_ABILITY_OPTION(pASSLT, "assault", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[Assault]} || ${Me.CombatAbility[Assault Rk. II]} || ${Me.CombatAbility[Assault Rk. III]} || ${Me.CombatAbility[Battery]} || ${Me.CombatAbility[Battery Rk. II]} || ${Me.CombatAbility[Battery Rk. III]} || ${Me.CombatAbility[Onslaught]} || ${Me.CombatAbility[Onslaught Rk. II]} || ${Me.CombatAbility[Onslaught Rk. III]} || ${Me.CombatAbility[Incursion]} || ${Me.CombatAbility[Incursion Rk. II]} || ${Me.CombatAbility[Incursion Rk. III]} || ${Me.CombatAbility[Barrage]} || ${Me.CombatAbility[Barrage Rk. II]} || ${Me.CombatAbility[Barrage Rk. III]} || ${Me.CombatAbility[Fellstrike]} || ${Me.CombatAbility[Fellstrike Rk. II]} || ${Me.CombatAbility[Fellstrike Rk. III]},60,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Assault]} || ${Me.CombatAbility[Assault Rk. II]} || ${Me.CombatAbility[Assault Rk. III]} || ${Me.CombatAbility[Battery]} || ${Me.CombatAbility[Battery Rk. II]} || ${Me.CombatAbility[Battery Rk. III]} || ${Me.CombatAbility[Onslaught]} || ${Me.CombatAbility[Onslaught Rk. II]} || ${Me.CombatAbility[Onslaught Rk. III]} || ${Me.CombatAbility[Incursion]} || ${Me.CombatAbility[Incursion Rk. II]} || ${Me.CombatAbility[Incursion Rk. III]} || ${Me.CombatAbility[Barrage]} || ${Me.CombatAbility[Barrage Rk. II]} || ${Me.CombatAbility[Barrage Rk. III]} || ${Me.CombatAbility[Fellstrike]} || ${Me.CombatAbility[Fellstrike Rk. II]} || ${Me.CombatAbility[Fellstrike Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pBANST, "banestrike", "[ON/OFF]?", "${If[${Me.AltAbility[Banestrike]},1,0]}", "${If[${meleemvi[plugin]} && ${Me.AltAbility[Banestrike]},1,0]}");
DECLARE_ABILITY_OPTION(pBASHS, "bash", "[#] Bash 0=0ff", "${If[${Me.Skill[bash]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && ${Me.Skill[bash]},1,0]}");
DECLARE_ABILITY_OPTION(pBBLOW, "boastful", "[ON/OFF]?", "${If[${Me.AltAbility[boastful bellow]},0,0]}", "${If[${meleemvi[plugin]} && ${Me.AltAbility[boastful bellow]},1,0]}");
DECLARE_ABILITY_OPTION(pBGING, "begging", "[ON/OFF]?", "0", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && ${Me.Skill[begging]},1,0]}");
DECLARE_ABILITY_OPTION(pBKOFF, "backoff", "[#] Life% Below? 0=0ff", "0", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && !${meleemvi[aggro]},1,0]}");
DECLARE_ABILITY_OPTION(pBLEED, "bleed", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[Bleed]} || ${Me.CombatAbility[Bleed Rk. II]} || ${Me.CombatAbility[Bleed Rk. III]} || ${Me.CombatAbility[Wound]} || ${Me.CombatAbility[Wound Rk. II]} || ${Me.CombatAbility[Wound Rk. III]} || ${Me.CombatAbility[Lacerate]} || ${Me.CombatAbility[Lacerate Rk. II]} || ${Me.CombatAbility[Lacerate Rk. III]} || ${Me.CombatAbility[Gash]} || ${Me.CombatAbility[Gash Rk. II]} || ${Me.CombatAbility[Gash Rk. III]} || ${Me.CombatAbility[Hack]} || ${Me.CombatAbility[Hack Rk. II]} || ${Me.CombatAbility[Hack Rk. III]} || ${Me.CombatAbility[Slice]} || ${Me.CombatAbility[Slice Rk. II]} || ${Me.CombatAbility[Slice Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Bleed]} || ${Me.CombatAbility[Bleed Rk. II]} || ${Me.CombatAbility[Bleed Rk. III]}|| ${Me.CombatAbility[Wound]} || ${Me.CombatAbility[Wound Rk. II]} || ${Me.CombatAbility[Wound Rk. III]} || ${Me.CombatAbility[Lacerate]} || ${Me.CombatAbility[Lacerate Rk. II]} || ${Me.CombatAbility[Lacerate Rk. III]} || ${Me.CombatAbility[Gash]} || ${Me.CombatAbility[Gash Rk. II]} || ${Me.CombatAbility[Gash Rk. III]} || ${Me.CombatAbility[Hack]} || ${Me.CombatAbility[Hack Rk. II]} || ${Me.CombatAbility[Hack Rk. III]} || ${Me.CombatAbility[Slice]} || ${Me.CombatAbility[Slice Rk. II]} || ${Me.CombatAbility[Slice Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pBLUST, "bloodlust", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[Shared Bloodlust]} || ${Me.CombatAbility[Shared Bloodlust Rk. II]} || ${Me.CombatAbility[Shared Bloodlust Rk. III]} || ${Me.CombatAbility[Shared Brutality]} || ${Me.CombatAbility[Shared Brutality Rk. II]} || ${Me.CombatAbility[Shared Brutality Rk. III]} || ${Me.CombatAbility[Shared Savagery]} || ${Me.CombatAbility[Shared Savagery Rk. II]} || ${Me.CombatAbility[Shared Savagery Rk. III]} || ${Me.CombatAbility[Shared Viciousness]} || ${Me.CombatAbility[Shared Viciousness Rk. II]} || ${Me.CombatAbility[Shared Viciousness Rk. III]} || ${Me.CombatAbility[Shared Cruelty]} || ${Me.CombatAbility[Shared Cruelty Rk. II]} || ${Me.CombatAbility[Shared Cruelty Rk. III]} || ${Me.CombatAbility[Shared Ruthlessness]} || ${Me.CombatAbility[Shared Ruthlessness Rk. II]} || ${Me.CombatAbility[Shared Ruthlessness Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Shared Bloodlust]} || ${Me.CombatAbility[Shared Bloodlust Rk. II]} || ${Me.CombatAbility[Shared Bloodlust Rk. III]} || ${Me.CombatAbility[Shared Brutality]} || ${Me.CombatAbility[Shared Brutality Rk. II]} || ${Me.CombatAbility[Shared Brutality Rk. III]} || ${Me.CombatAbility[Shared Savagery]} || ${Me.CombatAbility[Shared Savagery Rk. II]} || ${Me.CombatAbility[Shared Savagery Rk. III]} || ${Me.CombatAbility[Shared Viciousness]} || ${Me.CombatAbility[Shared Viciousness Rk. II]} || ${Me.CombatAbility[Shared Viciousness Rk. III]} || ${Me.CombatAbility[Shared Cruelty]} || ${Me.CombatAbility[Shared Cruelty Rk. II]} || ${Me.CombatAbility[Shared Cruelty Rk. III]} || ${Me.CombatAbility[Shared Ruthlessness]} || ${Me.CombatAbility[Shared Ruthlessness Rk. II]} || ${Me.CombatAbility[Shared Ruthlessness Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pBOWID, "bow", "[ID] spell/disc/aa/item?", "0", "${If[${meleemvi[plugin]} && ${Me.Skill[archery]},1,0]}");
DECLARE_ABILITY_OPTION(pBSTAB, "backstab", "[ON/OFF]?", "${If[${Me.Skill[backstab]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && ${Me.Skill[backstab]},1,0]}");
DECLARE_ABILITY_OPTION(pBTASP, "asp", "[ON/OFF]?", "${If[${Me.AltAbility[bite of the asp]},1,0]}", "${If[${meleemvi[plugin]} && ${Me.AltAbility[bite of the asp]},1,0]}");
DECLARE_ABILITY_OPTION(pBTLLP, "battleleap", "[ON/OFF]?", "${If[${Me.AltAbility[Battle Leap]},0,0]}", "${If[${meleemvi[plugin]} && ${Me.AltAbility[Battle Leap]},1,0]}");
DECLARE_ABILITY_OPTION(pBVIVI, "bvivi", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[Bestial Vivisection]} || ${Me.CombatAbility[Bestial Vivisection Rk. II]} || ${Me.CombatAbility[Bestial Vivisection Rk. III]} || ${Me.CombatAbility[Bestial Rending]} || ${Me.CombatAbility[Bestial Rending Rk. II]} || ${Me.CombatAbility[Bestial Rending Rk. III]} || ${Me.CombatAbility[Bestial Evulsing]} || ${Me.CombatAbility[Bestial Evulsing Rk. II]} || ${Me.CombatAbility[Bestial Evulsing Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Bestial Vivisection]} || ${Me.CombatAbility[Bestial Vivisection Rk. II]} || ${Me.CombatAbility[Bestial Vivisection Rk. III]} || ${Me.CombatAbility[Bestial Rending]} || ${Me.CombatAbility[Bestial Rending Rk. II]} || ${Me.CombatAbility[Bestial Rending Rk. III]} || ${Me.CombatAbility[Bestial Evulsing]} || ${Me.CombatAbility[Bestial Evulsing Rk. II]} || ${Me.CombatAbility[Bestial Evulsing Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pCALLC, "callchallenge", "[ON/OFF]?", "${If[${Me.AltAbility[call of challenge]},1,0]}", "${If[${meleemvi[plugin]} && ${Me.AltAbility[call of challenge]},1,0]}");
DECLARE_ABILITY_OPTION(pCFIST, "cloud", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[Cloud of Fists]} || ${Me.CombatAbility[Cloud of Fists Rk. II]} || ${Me.CombatAbility[Cloud of Fists Rk. III]} || ${Me.CombatAbility[Phantom Partisan]} || ${Me.CombatAbility[Phantom Partisan Rk. II]} || ${Me.CombatAbility[Phantom Partisan Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Cloud of Fists]} || ${Me.CombatAbility[Cloud of Fists Rk. II]} || ${Me.CombatAbility[Cloud of Fists Rk. III]} || ${Me.CombatAbility[Phantom Partisan]} || ${Me.CombatAbility[Phantom Partisan Rk. II]} || ${Me.CombatAbility[Phantom Partisan Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pCHAMS, "cstrike", "[ON/OFF]?", "${If[${Me.AltAbility[Chameleon Strike]},1,0]}", "${If[${meleemvi[plugin]} && ${Me.AltAbility[Chameleon Strike]},1,0]}");
DECLARE_ABILITY_OPTION(pCHFOR, "challengefor", "[ON/OFF]?", "${If[${Me.Book[challenge for honor]} || ${Me.Book[challenge for honor rk. ii]} || ${Me.Book[challenge for honor rk. iii]} || ${Me.Book[trial for honor]} || ${Me.Book[trial for honor rk. ii]} || ${Me.Book[trial for honor rk. iii]} || ${Me.Book[charge for honor]} || ${Me.Book[charge for honor rk. ii]} || ${Me.Book[charge for honor rk. iii]} || ${Me.Book[challenge for power]} || ${Me.Book[challenge for power rk. ii]} || ${Me.Book[challenge for power rk. iii]} || ${Me.Book[trial for power]} || ${Me.Book[trial for power rk. ii]} || ${Me.Book[trial for power rk. iii]} || ${Me.Book[charge for honor]} || ${Me.Book[charge for honor rk. ii]} || ${Me.Book[charge for honor rk. iii]} || ${Me.Book[confrontation for power]} || ${Me.Book[confrontation for power rk. ii]} || ${Me.Book[confrontation for power rk. iii]} || ${Me.Book[confrontation for honor]} || ${Me.Book[confrontation for honor rk. ii]} || ${Me.Book[confrontation for honor rk. iii]} || ${Me.Book[Provocation for honor]} || ${Me.Book[Provocation for honor rk. ii]} || ${Me.Book[Provocation for honor rk. iii]} || ${Me.Book[Provocation for power]} || ${Me.Book[Provocation for power rk. ii]} || ${Me.Book[Provocation for power rk. iii]} || ${Me.Book[Demand for Power]} || ${Me.Book[Demand for Power rk. ii]} || ${Me.Book[Demand for Power rk. iii]} || ${Me.Book[Demand for Honor]} || ${Me.Book[Demand for Honor rk. ii]} || ${Me.Book[Demand for Honor rk. iii]} || ${Me.Book[Impose for Power]} || ${Me.Book[Impose for Power rk. ii]} ||${Me.Book[Impose for Power rk. iii]} || ${Me.Book[Impose for Honor]} || ${Me.Book[Impose for Honor rk. ii]} ||${Me.Book[Impose for Honor rk. iii]} || ${Me.Book[Refute for Power]} || ${Me.Book[Refute for Power Rk. II]} ||${Me.Book[Refute for Power Rk. III]} || ${Me.Book[Refute for Honor]} || ${Me.Book[Refute for Honor Rk. II]} ||${Me.Book[Refute for Honor Rk. III]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[aggro]} && (${Me.Book[challenge for honor]} || ${Me.Book[challenge for honor rk. ii]} || ${Me.Book[challenge for honor rk. iii]} || ${Me.Book[trial for honor]} || ${Me.Book[trial for honor rk. ii]} || ${Me.Book[trial for honor rk. iii]} || ${Me.Book[charge for honor]} || ${Me.Book[charge for honor rk. ii]} || ${Me.Book[charge for honor rk. iii]} || ${Me.Book[challenge for power]} || ${Me.Book[challenge for power rk. ii]} || ${Me.Book[challenge for power rk. iii]} || ${Me.Book[trial for power]} || ${Me.Book[trial for power rk. ii]} || ${Me.Book[trial for power rk. iii]} || ${Me.Book[charge for honor]} || ${Me.Book[charge for honor rk. ii]} || ${Me.Book[charge for honor rk. iii]} || ${Me.Book[confrontation for power]} || ${Me.Book[confrontation for power rk. ii]} || ${Me.Book[confrontation for power rk. iii]} || ${Me.Book[confrontation for honor]} || ${Me.Book[confrontation for honor rk. ii]} || ${Me.Book[confrontation for honor rk. iii]} || ${Me.Book[Provocation for honor]} || ${Me.Book[Provocation for honor rk. ii]} || ${Me.Book[Provocation for honor rk. iii]} || ${Me.Book[Provocation for power]} || ${Me.Book[Provocation for power rk. ii]} || ${Me.Book[Provocation for power rk. iii]} || ${Me.Book[Demand for Power]} || ${Me.Book[Demand for Power rk. ii]} || ${Me.Book[Demand for Power rk. iii]} || ${Me.Book[Demand for Honor]} || ${Me.Book[Demand for Honor rk. ii]} || ${Me.Book[Demand for Honor rk. iii]} || ${Me.Book[Impose for Power]} || ${Me.Book[Impose for Power rk. ii]} ||${Me.Book[Impose for Power rk. iii]} || ${Me.Book[Impose for Honor]} || ${Me.Book[Impose for Honor rk. ii]} ||${Me.Book[Impose for Honor rk. iii]} || ${Me.Book[Refute for Power]} || ${Me.Book[Refute for Power Rk. II]} ||${Me.Book[Refute for Power Rk. III]} || ${Me.Book[Refute for Honor]} || ${Me.Book[Refute for Honor Rk. II]} ||${Me.Book[Refute for Honor Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pCOMMG, "commanding", "[#] Endu% Above? 0=0ff", "${If[${Me.CombatAbility[commanding voice]},20,0]}", "${If[${meleemvi[plugin]} && ${Me.CombatAbility[commanding voice]},1,0]}");
DECLARE_ABILITY_OPTION(pCRIPS, "cripple", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[leg strike]} || ${Me.CombatAbility[leg cut]} || ${Me.CombatAbility[leg slice]} || ${Me.CombatAbility[crippling strike]} || ${Me.CombatAbility[tendon cleave]} || ${Me.CombatAbility[tendon cleave rk. ii]} || ${Me.CombatAbility[tendon cleave rk. iii]} || ${Me.CombatAbility[tendon sever]} || ${Me.CombatAbility[tendon sever rk. ii]} || ${Me.CombatAbility[tendon sever rk. iii]} || ${Me.CombatAbility[tendon shear]} || ${Me.CombatAbility[tendon shear rk. ii]} || ${Me.CombatAbility[tendon shear rk. iii]} || ${Me.CombatAbility[tendon lacerate]} || ${Me.CombatAbility[tendon lacerate rk. ii]} || ${Me.CombatAbility[tendon lacerate rk. iii]} || ${Me.CombatAbility[tendon Slash]} || ${Me.CombatAbility[tendon Slash rk. ii]} || ${Me.CombatAbility[tendon Slash rk. iii]} || ${Me.CombatAbility[Tendon Gash]} || ${Me.CombatAbility[Tendon Gash Rk. II]} || ${Me.CombatAbility[Tendon Gash Rk. III]} || ${Me.CombatAbility[Tendon Tear]} || ${Me.CombatAbility[Tendon Tear Rk. II]} || ${Me.CombatAbility[Tendon Tear Rk. III]} || ${Me.CombatAbility[Tendon Rupture]} || ${Me.CombatAbility[Tendon Rupture Rk. II]} || ${Me.CombatAbility[Tendon Rupture Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[leg strike]} || ${Me.CombatAbility[leg cut]} || ${Me.CombatAbility[leg slice]} || ${Me.CombatAbility[crippling strike]} || ${Me.CombatAbility[tendon cleave]} || ${Me.CombatAbility[tendon cleave rk. ii]} || ${Me.CombatAbility[tendon cleave rk. iii]} || ${Me.CombatAbility[tendon sever]} || ${Me.CombatAbility[tendon sever rk. ii]} || ${Me.CombatAbility[tendon sever rk. iii]} || ${Me.CombatAbility[tendon shear]} || ${Me.CombatAbility[tendon shear rk. ii]} || ${Me.CombatAbility[tendon shear rk. iii]} || ${Me.CombatAbility[tendon lacerate]} || ${Me.CombatAbility[tendon lacerate rk. ii]} || ${Me.CombatAbility[tendon lacerate rk. iii]} || ${Me.CombatAbility[tendon Slash]} || ${Me.CombatAbility[tendon Slash rk. ii]} || ${Me.CombatAbility[tendon Slash rk. iii]} || ${Me.CombatAbility[Tendon Gash]} || ${Me.CombatAbility[Tendon Gash Rk. II]} || ${Me.CombatAbility[Tendon Gash Rk. III]} || ${Me.CombatAbility[Tendon Tear]} || ${Me.CombatAbility[Tendon Tear Rk. II]} || ${Me.CombatAbility[Tendon Tear Rk. III]} || ${Me.CombatAbility[Tendon Rupture]} || ${Me.CombatAbility[Tendon Rupture Rk. II]} || ${Me.CombatAbility[Tendon Rupture Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pCRYHC, "cryhavoc", "[#] Endu% Above? 0=0ff", "${If[${Me.CombatAbility[cry havoc]} || ${Me.CombatAbility[Cry Carnage]} || ${Me.CombatAbility[Cry Carnage Rk. II]} || ${Me.CombatAbility[Cry Carnage Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[cry havoc]} || ${Me.CombatAbility[Cry Carnage]} || ${Me.CombatAbility[Cry Carnage Rk. II]} || ${Me.CombatAbility[Cry Carnage Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pDEFEN, "defense", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[Bracing Defense]} || ${Me.CombatAbility[Bracing Defense Rk. II]} || ${Me.CombatAbility[Bracing Defense Rk. III]} || ${Me.CombatAbility[Staunch Defense]} || ${Me.CombatAbility[Staunch Defense Rk. II]} || ${Me.CombatAbility[Staunch Defense Rk. III]} || ${Me.CombatAbility[Stalwart Defense]} || ${Me.CombatAbility[Stalwart Defense Rk. II]} || ${Me.CombatAbility[Stalwart Defense Rk. III]} || ${Me.CombatAbility[Steadfast Defense]} || ${Me.CombatAbility[Steadfast Defense Rk. II]} || ${Me.CombatAbility[Steadfast Defense Rk. III]} || ${Me.CombatAbility[Stout Defense]} || ${Me.CombatAbility[Stout Defense Rk. II]} || ${Me.CombatAbility[Stout Defense Rk. III]} || ${Me.CombatAbility[Resolute Defense]} || ${Me.CombatAbility[Resolute Defense Rk. II]} || ${Me.CombatAbility[Resolute Defense Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Bracing Defense]} || ${Me.CombatAbility[Bracing Defense Rk. II]} || ${Me.CombatAbility[Bracing Defense Rk. III]} || ${Me.CombatAbility[Staunch Defense]} || ${Me.CombatAbility[Staunch Defense Rk. II]} || ${Me.CombatAbility[Staunch Defense Rk. III]} || ${Me.CombatAbility[Stalwart Defense]} || ${Me.CombatAbility[Stalwart Defense Rk. II]} || ${Me.CombatAbility[Stalwart Defense Rk. III]} || ${Me.CombatAbility[Steadfast Defense]} || ${Me.CombatAbility[Steadfast Defense Rk. II]} || ${Me.CombatAbility[Steadfast Defense Rk. III]} || ${Me.CombatAbility[Stout Defense]} || ${Me.CombatAbility[Stout Defense Rk. II]} || ${Me.CombatAbility[Stout Defense Rk. III]} || ${Me.CombatAbility[Resolute Defense]} || ${Me.CombatAbility[Resolute Defense Rk. II]} || ${Me.CombatAbility[Resolute Defense Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pDISRM, "disarm", "[ON/OFF]?", "${If[${Me.Skill[disarm]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && ${Me.Skill[disarm]},1,0]}");
DECLARE_ABILITY_OPTION(pDMONK, "monkey", "[#] Endu% Above? 0=0ff", "${If[${Me.CombatAbility[Drunken Monkey Style]} || ${Me.CombatAbility[Drunken Monkey Style rk. ii]} || ${Me.CombatAbility[Drunken Monkey Style rk. iii]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Drunken Monkey Style]} || ${Me.CombatAbility[Drunken Monkey Style rk. ii]} || ${Me.CombatAbility[Drunken Monkey Style rk. iii]}),1,0]}");
DECLARE_ABILITY_OPTION(pDRPNC, "dragonpunch", "[ON/OFF]?", "${If[${Me.Skill[dragon punch]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && ${Me.Skill[dragon punch]},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF0, "downflag0", "[ON/OFF] downflag0?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit0].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF1, "downflag1", "[ON/OFF] downflag1?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit1].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF2, "downflag2", "[ON/OFF] downflag2?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit2].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF3, "downflag3", "[ON/OFF] downflag3?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit3].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF4, "downflag4", "[ON/OFF] downflag4?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit4].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF5, "downflag5", "[ON/OFF] downflag5?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit5].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF6, "downflag6", "[ON/OFF] downflag6?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit6].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF7, "downflag7", "[ON/OFF] downflag7?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit7].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF8, "downflag8", "[ON/OFF] downflag8?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit8].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF9, "downflag9", "[ON/OFF] downflag9?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit9].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF10, "downflag10", "[ON/OFF] downflag10?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit10].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF11, "downflag11", "[ON/OFF] downflag11?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit11].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF12, "downflag12", "[ON/OFF] downflag12?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit12].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF13, "downflag13", "[ON/OFF] downflag13?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit13].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF14, "downflag14", "[ON/OFF] downflag14?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit14].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF15, "downflag15", "[ON/OFF] downflag15?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit15].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF16, "downflag16", "[ON/OFF] downflag16?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit16].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF17, "downflag17", "[ON/OFF] downflag17?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit17].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF18, "downflag18", "[ON/OFF] downflag18?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit18].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF19, "downflag19", "[ON/OFF] downflag19?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit19].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF20, "downflag20", "[ON/OFF] downflag20?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit20].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF21, "downflag21", "[ON/OFF] downflag21?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit21].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF22, "downflag22", "[ON/OFF] downflag22?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit22].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF23, "downflag23", "[ON/OFF] downflag23?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit23].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF24, "downflag24", "[ON/OFF] downflag24?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit24].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF25, "downflag25", "[ON/OFF] downflag25?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit25].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF26, "downflag26", "[ON/OFF] downflag26?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit26].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF27, "downflag27", "[ON/OFF] downflag27?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit27].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF28, "downflag28", "[ON/OFF] downflag28?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit28].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF29, "downflag29", "[ON/OFF] downflag29?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit29].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF30, "downflag30", "[ON/OFF] downflag30?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit30].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF31, "downflag31", "[ON/OFF] downflag31?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit31].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF32, "downflag32", "[ON/OFF] downflag32?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit32].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF33, "downflag33", "[ON/OFF] downflag33?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit33].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF34, "downflag34", "[ON/OFF] downflag34?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit34].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF35, "downflag35", "[ON/OFF] downflag35?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit35].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF36, "downflag36", "[ON/OFF] downflag36?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit36].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF37, "downflag37", "[ON/OFF] downflag37?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit37].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF38, "downflag38", "[ON/OFF] downflag38?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit38].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF39, "downflag39", "[ON/OFF] downflag39?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit39].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF40, "downflag40", "[ON/OFF] downflag40?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit40].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF41, "downflag41", "[ON/OFF] downflag41?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit41].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF42, "downflag42", "[ON/OFF] downflag42?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit42].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF43, "downflag43", "[ON/OFF] downflag43?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit43].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF44, "downflag44", "[ON/OFF] downflag44?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit44].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF45, "downflag45", "[ON/OFF] downflag45?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit45].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF46, "downflag46", "[ON/OFF] downflag46?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit46].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF47, "downflag47", "[ON/OFF] downflag47?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit47].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF48, "downflag48", "[ON/OFF] downflag48?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit48].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF49, "downflag49", "[ON/OFF] downflag49?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit49].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF50, "downflag50", "[ON/OFF] downflag50?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit50].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF51, "downflag51", "[ON/OFF] downflag51?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit51].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF52, "downflag52", "[ON/OFF] downflag52?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit52].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF53, "downflag53", "[ON/OFF] downflag53?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit53].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF54, "downflag54", "[ON/OFF] downflag54?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit54].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF55, "downflag55", "[ON/OFF] downflag55?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit55].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF56, "downflag56", "[ON/OFF] downflag56?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit56].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF57, "downflag57", "[ON/OFF] downflag57?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit57].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF58, "downflag58", "[ON/OFF] downflag58?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit58].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF59, "downflag59", "[ON/OFF] downflag59?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit59].Length},1,0]}");
DECLARE_ABILITY_OPTION(pDWNF60, "downflag60", "[ON/OFF] downflag60?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[downshit60].Length},1,0]}");
DECLARE_ABILITY_OPTION(pEAGLE, "eaglestrike", "[ON/OFF]?", "${If[${Me.Skill[eagle strike]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && ${Me.Skill[eagle strike]},1,0]}");
DECLARE_ABILITY_OPTION(pERAGE, "enrage", "[ON/OFF]?", "1", "${If[${meleemvi[plugin]},1,0]}");
DECLARE_ABILITY_OPTION(pERKCK, "enragingkick", "[#] Life% Below? 0=0ff", "${If[${Me.CombatAbility[Enraging Crescent Kicks]} || ${Me.CombatAbility[Enraging Crescent Kicks Rk. II]} || ${Me.CombatAbility[Enraging Crescent Kicks Rk. III]} || ${Me.CombatAbility[Enraging Heel Kicks]} || ${Me.CombatAbility[Enraging Heel Kicks Rk. II]} || ${Me.CombatAbility[Enraging Heel Kicks Rk. III]} || ${Me.CombatAbility[Enraging Cut Kicks]} || ${Me.CombatAbility[Enraging Cut Kicks Rk. II]} || ${Me.CombatAbility[Enraging Cut Kicks Rk. III]} ,20,0]}", "${If[${meleemvi[plugin]} && !${meleemvi[aggro]} && (${Me.CombatAbility[Enraging Crescent Kicks]} || ${Me.CombatAbility[Enraging Crescent Kicks Rk. II]} || ${Me.CombatAbility[Enraging Crescent Kicks Rk. III]} || ${Me.CombatAbility[Enraging Heel Kicks]} || ${Me.CombatAbility[Enraging Heel Kicks Rk. II]} || ${Me.CombatAbility[Enraging Heel Kicks Rk. III]})|| ${Me.CombatAbility[Enraging Cut Kicks]} || ${Me.CombatAbility[Enraging Cut Kicks Rk. II]} || ${Me.CombatAbility[Enraging Cut Kicks Rk. III]},1,0]}");
DECLARE_ABILITY_OPTION(pESCAP, "escape", "[#] Life% Below? 0=0ff", "${If[${Me.AltAbility[escape]},20,0]}", "${If[${meleemvi[plugin]} && !${meleemvi[aggro]} && ${Me.AltAbility[escape]},1,0]}");
DECLARE_ABILITY_OPTION(pEVADE, "evade", "[#] [ON/OFF]?", "${If[${Me.Skill[hide]} && ${Me.Class.ShortName.Equal[ROG]},1,0]}", "${If[${meleemvi[plugin]} && !${meleemvi[aggro]} && ${Me.Skill[hide]} && ${Me.Class.ShortName.Equal[ROG]},1,0]}");
DECLARE_ABILITY_OPTION(pEYEGO, "eyegouge", "[ON/OFF]?", "${If[${Me.AltAbility[eye gouge]},1,0]}", "${If[${meleemvi[plugin]} && ${Me.AltAbility[eye gouge]},1,0]}");
DECLARE_ABILITY_OPTION(pFACES, "facing", "[ON/OFF] Face Target (Range)?", "1", "${If[${meleemvi[plugin]} && ${meleemvi[range]},1,0]}");
DECLARE_ABILITY_OPTION(pFALLS, "falls", "[ON/OFF] Auto-Feign?", "0", "${If[${meleemvi[plugin]} && !${meleemvi[aggro]} && ${Me.Class.ShortName.Equal[MNK]},1,0]}");
DECLARE_ABILITY_OPTION(pFCLAW, "fclaw", "[#] Endu% Above? 0=0ff", "${If[${Me.CombatAbility[flurry of claws]} || ${Me.CombatAbility[flurry of claws rk. ii]} || ${Me.CombatAbility[flurry of claws rk. iii]} || ${Me.CombatAbility[tumult of claws]} || ${Me.CombatAbility[tumult of claws rk. ii]} || ${Me.CombatAbility[tumult of claws rk. iii]} || ${Me.CombatAbility[clamor of claws]} || ${Me.CombatAbility[clamor of claws rk. ii]} || ${Me.CombatAbility[clamor of claws rk. iii]} || ${Me.CombatAbility[tempest of claws]} || ${Me.CombatAbility[tempest of claws rk. ii]} || ${Me.CombatAbility[tempest of claws rk. iii]} || ${Me.CombatAbility[Storm of claws]} || ${Me.CombatAbility[Storm of claws Rk. II]} || ${Me.CombatAbility[Storm of claws Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[flurry of claws]} || ${Me.CombatAbility[flurry of claws rk. ii]} || ${Me.CombatAbility[flurry of claws rk. iii]} || ${Me.CombatAbility[tumult of claws]} || ${Me.CombatAbility[tumult of claws rk. ii]} || ${Me.CombatAbility[tumult of claws rk. iii]} || ${Me.CombatAbility[clamor of claws]} || ${Me.CombatAbility[clamor of claws rk. ii]} || ${Me.CombatAbility[clamor of claws rk. iii]} || ${Me.CombatAbility[tempest of claws]} || ${Me.CombatAbility[tempest of claws rk. ii]} || ${Me.CombatAbility[tempest of claws rk. iii]} || ${Me.CombatAbility[Storm of claws]} || ${Me.CombatAbility[Storm of claws Rk. II]} || ${Me.CombatAbility[Storm of claws Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pFEIGN, "feigndeath", "[#] Life% Below? 0=0ff", "${If[${Select[${Me.Class.ShortName},BST,SHD,NEC,MNK]},30,0]}", "${If[${meleemvi[plugin]} && !${meleemvi[aggro]} && ${Select[${Me.Class.ShortName},BST,SHD,NEC,MNK]},1,0]}");
DECLARE_ABILITY_OPTION(pFERAL, "feralswipe", "[ON/OFF]?", "${If[${Me.AltAbility[feral swipe]},1,0]}", "${If[${meleemvi[plugin]} && ${Me.AltAbility[feral swipe]},1,0]}");
DECLARE_ABILITY_OPTION(pFIELD, "fieldarm", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[Field Armorer]} || ${Me.CombatAbility[Field Armorer Rk. II]} || ${Me.CombatAbility[Field Armorer Rk. III]} || ${Me.CombatAbility[Field Outfitter]} || ${Me.CombatAbility[Field Outfitter Rk. II]} || ${Me.CombatAbility[Field Outfitter Rk. III]} || ${Me.CombatAbility[Field Defender]} || ${Me.CombatAbility[Field Defender Rk. II]} || ${Me.CombatAbility[Field Defender Rk. III]} || ${Me.CombatAbility[Field Guardian]} || ${Me.CombatAbility[Field Guardian Rk. II]} || ${Me.CombatAbility[Field Guardian Rk. III]} || ${Me.CombatAbility[Field Protector]} || ${Me.CombatAbility[Field Protector Rk. II]} || ${Me.CombatAbility[Field Protector Rk. III]} || ${Me.CombatAbility[Field Champion]} || ${Me.CombatAbility[Field Champion Rk. II]} || ${Me.CombatAbility[Field Champion Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Field Armorer]} || ${Me.CombatAbility[Field Armorer Rk. II]} || ${Me.CombatAbility[Field Armorer Rk. III]} || ${Me.CombatAbility[Field Outfitter]} || ${Me.CombatAbility[Field Outfitter Rk. II]} || ${Me.CombatAbility[Field Outfitter Rk. III]} || ${Me.CombatAbility[Field Defender]} || ${Me.CombatAbility[Field Defender Rk. II]} || ${Me.CombatAbility[Field Defender Rk. III]} || ${Me.CombatAbility[Field Guardian]} || ${Me.CombatAbility[Field Guardian Rk. II]} || ${Me.CombatAbility[Field Guardian Rk. III]} || ${Me.CombatAbility[Field Protector]} || ${Me.CombatAbility[Field Protector Rk. II]} || ${Me.CombatAbility[Field Protector Rk. III]} || ${Me.CombatAbility[Field Champion]} || ${Me.CombatAbility[Field Champion Rk. II]} || ${Me.CombatAbility[Field Champion Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pFISTS, "fistsofwu", "[#] Endu% Above? 0=0ff", "${If[${Me.CombatAbility[fists of wu]},20,0]}", "${If[${meleemvi[plugin]} && ${Me.CombatAbility[fists of wu]},1,0]}");
//DECLARE_ABILITY_OPTION(pFKICK, "ferociouskick", "[ON/OFF]?", "${If[${Me.AltAbility[ferocious kick]},1,0]}", "${If[${meleemvi[plugin]} && ${Me.AltAbility[ferocious kick]},1,0]}");
DECLARE_ABILITY_OPTION(pFLYKC, "flyingkick", "[ON/OFF]?", "${If[${Me.Skill[flying kick]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && ${Me.Skill[flying kick]},1,0]}");
DECLARE_ABILITY_OPTION(pFORAG, "forage", "[ON/OFF]?", "0", "${If[${meleemvi[plugin]} && ${Me.Skill[forage]},1,0]}");
DECLARE_ABILITY_OPTION(pFRENZ, "frenzy", "[ON/OFF]?", "${If[${Me.Skill[frenzy]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[frenzy]} && ${Me.Skill[frenzy]},1,0]}");
DECLARE_ABILITY_OPTION(pGBLDE, "gblade", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[Gouging Blade]} || ${Me.CombatAbility[Gouging Blade Rk. II]} || ${Me.CombatAbility[Gouging Blade Rk. III]} || ${Me.CombatAbility[Gashing Blade]} || ${Me.CombatAbility[Gashing Blade Rk. II]} || ${Me.CombatAbility[Gashing Blade Rk. III]} || ${Me.CombatAbility[Lacerating Blade]} || ${Me.CombatAbility[Lacerating Blade Rk. II]} || ${Me.CombatAbility[Lacerating Blade Rk. III]} || ${Me.CombatAbility[Wounding Blade]} || ${Me.CombatAbility[Wounding Blade Rk. II]} || ${Me.CombatAbility[Wounding Blade Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && ${Me.CombatAbility[Gouging Blade]} || ${Me.CombatAbility[Gouging Blade Rk. II]} || ${Me.CombatAbility[Gouging Blade Rk. III]} || ${Me.CombatAbility[Gashing Blade]} || ${Me.CombatAbility[Gashing Blade Rk. II]} || ${Me.CombatAbility[Gashing Blade Rk. III]} || ${Me.CombatAbility[Lacerating Blade]} || ${Me.CombatAbility[Lacerating Blade Rk. II]} || ${Me.CombatAbility[Lacerating Blade Rk. III]} || ${Me.CombatAbility[Wounding Blade]} || ${Me.CombatAbility[Wounding Blade Rk. II]} || ${Me.CombatAbility[Wounding Blade Rk. III]},1,0]}");
DECLARE_ABILITY_OPTION(pGORSM, "gorillasmash", "[ON/OFF]?", "${If[${Me.AltAbility[gorilla smash]},1,0]}", "${If[${meleemvi[plugin]} && ${Me.AltAbility[gorilla smash]},1,0]}");
DECLARE_ABILITY_OPTION(pGTPUN, "gutpunch", "[ON/OFF]?", "${If[${Me.AltAbility[gut punch]},1,0]}", "${If[${meleemvi[plugin]} && ${Me.AltAbility[gut punch]},1,0]}");
DECLARE_ABILITY_OPTION(pHARMT, "harmtouch", "[ON/OFF]?", "${If[${Me.AltAbility[harm touch].ID},1,0]}", "${If[${meleemvi[plugin]} && ${Me.AltAbility[harm touch]}   && ${Me.Class.ShortName.Equal[SHD]},1,0]}");
DECLARE_ABILITY_OPTION(pHFAST, "pothealfast", "[#] MyLife% Below? 0=0ff (FAST)", "${If[${meleemvi[idpothealfast]},30,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[idpothealfast]},1,0]}");
DECLARE_ABILITY_OPTION(pHIDES, "hide", "[ON/OFF]?", "0", "${If[${meleemvi[plugin]} && ${Me.Skill[hide]},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF0, "holyflag0", "[ON/OFF] holyflag0?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit0].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF1, "holyflag1", "[ON/OFF] holyflag1?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit1].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF2, "holyflag2", "[ON/OFF] holyflag2?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit2].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF3, "holyflag3", "[ON/OFF] holyflag3?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit3].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF4, "holyflag4", "[ON/OFF] holyflag4?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit4].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF5, "holyflag5", "[ON/OFF] holyflag5?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit5].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF6, "holyflag6", "[ON/OFF] holyflag6?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit6].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF7, "holyflag7", "[ON/OFF] holyflag7?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit7].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF8, "holyflag8", "[ON/OFF] holyflag8?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit8].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF9, "holyflag9", "[ON/OFF] holyflag9?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit9].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF10, "holyflag10", "[ON/OFF] holyflag10?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit10].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF11, "holyflag11", "[ON/OFF] holyflag11?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit11].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF12, "holyflag12", "[ON/OFF] holyflag12?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit12].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF13, "holyflag13", "[ON/OFF] holyflag13?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit13].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF14, "holyflag14", "[ON/OFF] holyflag14?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit14].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF15, "holyflag15", "[ON/OFF] holyflag15?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit15].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF16, "holyflag16", "[ON/OFF] holyflag16?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit16].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF17, "holyflag17", "[ON/OFF] holyflag17?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit17].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF18, "holyflag18", "[ON/OFF] holyflag18?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit18].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF19, "holyflag19", "[ON/OFF] holyflag19?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit19].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF20, "holyflag20", "[ON/OFF] holyflag20?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit20].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF21, "holyflag21", "[ON/OFF] holyflag21?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit21].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF22, "holyflag22", "[ON/OFF] holyflag22?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit22].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF23, "holyflag23", "[ON/OFF] holyflag23?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit23].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF24, "holyflag24", "[ON/OFF] holyflag24?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit24].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF25, "holyflag25", "[ON/OFF] holyflag25?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit25].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF26, "holyflag26", "[ON/OFF] holyflag26?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit26].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF27, "holyflag27", "[ON/OFF] holyflag27?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit27].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF28, "holyflag28", "[ON/OFF] holyflag28?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit28].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF29, "holyflag29", "[ON/OFF] holyflag29?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit29].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF30, "holyflag30", "[ON/OFF] holyflag30?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit30].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF31, "holyflag31", "[ON/OFF] holyflag31?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit31].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF32, "holyflag32", "[ON/OFF] holyflag32?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit32].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF33, "holyflag33", "[ON/OFF] holyflag33?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit33].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF34, "holyflag34", "[ON/OFF] holyflag34?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit34].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF35, "holyflag35", "[ON/OFF] holyflag35?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit35].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF36, "holyflag36", "[ON/OFF] holyflag36?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit36].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF37, "holyflag37", "[ON/OFF] holyflag37?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit37].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF38, "holyflag38", "[ON/OFF] holyflag38?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit38].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF39, "holyflag39", "[ON/OFF] holyflag39?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit39].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF40, "holyflag40", "[ON/OFF] holyflag40?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit40].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF41, "holyflag41", "[ON/OFF] holyflag41?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit41].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF42, "holyflag42", "[ON/OFF] holyflag42?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit42].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF43, "holyflag43", "[ON/OFF] holyflag43?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit43].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF44, "holyflag44", "[ON/OFF] holyflag44?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit44].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF45, "holyflag45", "[ON/OFF] holyflag45?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit45].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF46, "holyflag46", "[ON/OFF] holyflag46?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit46].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF47, "holyflag47", "[ON/OFF] holyflag47?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit47].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF48, "holyflag48", "[ON/OFF] holyflag48?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit48].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF49, "holyflag49", "[ON/OFF] holyflag49?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit49].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF50, "holyflag50", "[ON/OFF] holyflag50?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit50].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF51, "holyflag51", "[ON/OFF] holyflag51?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit51].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF52, "holyflag52", "[ON/OFF] holyflag52?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit52].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF53, "holyflag53", "[ON/OFF] holyflag53?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit53].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF54, "holyflag54", "[ON/OFF] holyflag54?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit54].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF55, "holyflag55", "[ON/OFF] holyflag55?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit55].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF56, "holyflag56", "[ON/OFF] holyflag56?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit56].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF57, "holyflag57", "[ON/OFF] holyflag57?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit57].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF58, "holyflag58", "[ON/OFF] holyflag58?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit58].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF59, "holyflag59", "[ON/OFF] holyflag59?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit59].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOLF60, "holyflag60", "[ON/OFF] holyflag60?", "0", "${If[${meleemvi[plugin]} && ${meleemvs[holyshit60].Length},1,0]}");
DECLARE_ABILITY_OPTION(pHOVER, "pothealover", "[#] MyLife% Below? 0=0ff (OVER)", "${If[${meleemvi[idpothealover]},20,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[idpothealover]},1,0]}");
DECLARE_ABILITY_OPTION(pINFUR, "infuriate", "[ON/OFF]?", "1", "${If[${meleemvi[plugin]},1,0]}");
DECLARE_ABILITY_OPTION(pINTIM, "intimidation", "[ON/OFF]?", "0", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && ${Me.Skill[intimidation]},1,0]}");
DECLARE_ABILITY_OPTION(pJKICK, "jltkicks", "[#] Endu% Above? 0=0ff", "${If[${Me.CombatAbility[jolting kicks]} || ${Me.CombatAbility[jolting kicks rk. ii]} || ${Me.CombatAbility[jolting kicks rk. iii]} || ${Me.CombatAbility[Jolting Snapkicks]} || ${Me.CombatAbility[Jolting Snapkicks rk. ii]} || ${Me.CombatAbility[Jolting Snapkicks rk. iii]} || ${Me.CombatAbility[Jolting Frontkicks]} || ${Me.CombatAbility[Jolting Frontkicks rk. ii]} || ${Me.CombatAbility[Jolting Frontkicks rk. iii]} || ${Me.CombatAbility[Jolting Hook kicks]} || ${Me.CombatAbility[Jolting Hook kicks rk. ii]} || ${Me.CombatAbility[Jolting Hook kicks rk. iii]} || ${Me.CombatAbility[Jolting Crescent kicks]} || ${Me.CombatAbility[Jolting Crescent kicks rk. ii]} || ${Me.CombatAbility[Jolting Crescent kicks rk. iii]} || ${Me.CombatAbility[Jolting Heel Kicks]} || ${Me.CombatAbility[Jolting Heel Kicks rk. ii]} || ${Me.CombatAbility[Jolting Heel Kicks rk. iii]} || ${Me.CombatAbility[Jolting Cut Kicks]} || ${Me.CombatAbility[Jolting Cut Kicks rk. ii]} || ${Me.CombatAbility[Jolting Cut Kicks rk. iii]} || ${Me.CombatAbility[Jolting Wheel Kicks]} || ${Me.CombatAbility[Jolting Wheel Kicks Rk. II]} || ${Me.CombatAbility[Jolting Wheel Kicks Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[jolting kicks]} || ${Me.CombatAbility[jolting kicks rk. ii]} || ${Me.CombatAbility[jolting kicks rk. iii]} || ${Me.CombatAbility[Jolting Snapkicks]} || ${Me.CombatAbility[Jolting Snapkicks rk. ii]} || ${Me.CombatAbility[Jolting Snapkicks rk. iii]} || ${Me.CombatAbility[Jolting Frontkicks]} || ${Me.CombatAbility[Jolting Frontkicks rk. ii]} || ${Me.CombatAbility[Jolting Frontkicks rk. iii]} || ${Me.CombatAbility[Jolting Hook kicks]} || ${Me.CombatAbility[Jolting Hook kicks rk. ii]} || ${Me.CombatAbility[Jolting Hook kicks rk. iii]} || ${Me.CombatAbility[Jolting Crescent kicks]} || ${Me.CombatAbility[Jolting Crescent kicks rk. ii]} || ${Me.CombatAbility[Jolting Crescent kicks rk. iii]} || ${Me.CombatAbility[Jolting Heel Kicks]} || ${Me.CombatAbility[Jolting Heel Kicks rk. ii]} || ${Me.CombatAbility[Jolting Heel Kicks rk. iii]} || ${Me.CombatAbility[Jolting Cut Kicks]} || ${Me.CombatAbility[Jolting Cut Kicks rk. ii]} || ${Me.CombatAbility[Jolting Cut Kicks rk. iii]} || ${Me.CombatAbility[Jolting Wheel Kicks]} || ${Me.CombatAbility[Jolting Wheel Kicks Rk. II]} || ${Me.CombatAbility[Jolting Wheel Kicks Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pJOLTS, "jolt", "Every [#] of Hits,0=0ff", "0", "${If[${meleemvi[plugin]} && !${meleemvi[aggro]} && ${Select[${Me.Class.ShortName},BER,BST,RNG]},1,0]}");
DECLARE_ABILITY_OPTION(pJUGUL, "jugular", "[#] Endu% Above? 0=0ff", "${If[${Me.CombatAbility[Jugular Slash]} || ${Me.CombatAbility[Jugular Slash rk. ii]} || ${Me.CombatAbility[Jugular Slash rk. iii]} || ${Me.CombatAbility[Jugular Slice]} || ${Me.CombatAbility[Jugular Slice rk. ii]} || ${Me.CombatAbility[Jugular Slice rk. iii]} || ${Me.CombatAbility[Jugular Sever]} || ${Me.CombatAbility[Jugular Sever rk. ii]} || ${Me.CombatAbility[Jugular Sever rk. iii]} || ${Me.CombatAbility[Jugular Gash]} || ${Me.CombatAbility[Jugular Gash rk. ii]} || ${Me.CombatAbility[Jugular Gash rk. iii]} || ${Me.CombatAbility[Jugular Lacerate]} || ${Me.CombatAbility[Jugular Lacerate rk. ii]} || ${Me.CombatAbility[Jugular Lacerate rk. iii]} || ${Me.CombatAbility[Jugular Hack]} || ${Me.CombatAbility[Jugular Hack Rk. II]} || ${Me.CombatAbility[Jugular Hack Rk. III]} || ${Me.CombatAbility[Jugular Strike]} || ${Me.CombatAbility[Jugular Strike Rk. II]} || ${Me.CombatAbility[Jugular Strike Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Jugular Slash]} || ${Me.CombatAbility[Jugular Slash rk. ii]} || ${Me.CombatAbility[Jugular Slash rk. iii]} || ${Me.CombatAbility[Jugular Slice]} || ${Me.CombatAbility[Jugular Slice rk. ii]} || ${Me.CombatAbility[Jugular Slice rk. iii]} || ${Me.CombatAbility[Jugular Sever]} || ${Me.CombatAbility[Jugular Sever rk. ii]} || ${Me.CombatAbility[Jugular Sever rk. iii]} || ${Me.CombatAbility[Jugular Gash]} || ${Me.CombatAbility[Jugular Gash rk. ii]} || ${Me.CombatAbility[Jugular Gash rk. iii]} || ${Me.CombatAbility[Jugular Lacerate]} || ${Me.CombatAbility[Jugular Lacerate rk. ii]} || ${Me.CombatAbility[Jugular Lacerate rk. iii]} || ${Me.CombatAbility[Jugular Hack]} || ${Me.CombatAbility[Jugular Hack Rk. II]} || ${Me.CombatAbility[Jugular Hack Rk. III]} || ${Me.CombatAbility[Jugular Strike]} || ${Me.CombatAbility[Jugular Strike Rk. II]} || ${Me.CombatAbility[Jugular Strike Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pKICKS, "kick", "[ON/OFF]?", "${If[${Me.Skill[kick]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && ${Me.Skill[kick]},1,0]}");
DECLARE_ABILITY_OPTION(pKNEES, "kneestrike", "[ON/OFF]?", "${If[${Me.AltAbility[knee strike]},1,0]}", "${If[${meleemvi[plugin]} && ${Me.AltAbility[knee strike]},1,0]}");
DECLARE_ABILITY_OPTION(pKNFPL, "knifeplay", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[Knifeplay]} || ${Me.CombatAbility[Knifeplay Rk. II]} || ${Me.CombatAbility[Knifeplay Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Knifeplay]} || ${Me.CombatAbility[Knifeplay Rk. II]} || ${Me.CombatAbility[Knifeplay Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pLCLAW, "leopardclaw", "[#] Endu% Above? 0=0ff", "${If[${Me.CombatAbility[leopard claw]} || ${Me.CombatAbility[dragon fang]} || ${Me.CombatAbility[clawstriker's flurry]} || ${Me.CombatAbility[clawstriker's flurry rk. ii]} || ${Me.CombatAbility[clawstriker's flurry rk. iii]} || ${Me.CombatAbility[wheel of fists]} || ${Me.CombatAbility[wheel of fists rk. ii]} || ${Me.CombatAbility[wheel of fists rk. iii]} || ${Me.CombatAbility[Six-Step Pattern]} || ${Me.CombatAbility[Six-Step Pattern rk. ii]} || ${Me.CombatAbility[Six-Step Pattern rk. iii]} || ${Me.CombatAbility[Seven-Step Pattern]} || ${Me.CombatAbility[Seven-Step Pattern rk. ii]} || ${Me.CombatAbility[Seven-Step Pattern rk. iii]} || ${Me.CombatAbility[Eight-Step Pattern]} || ${Me.CombatAbility[Eight-Step Pattern Rk. II]} || ${Me.CombatAbility[Eight-Step Pattern Rk. III]} || ${Me.CombatAbility[Torrent of Fists]} || ${Me.CombatAbility[Torrent of Fists Rk. II]} || ${Me.CombatAbility[Torrent of Fists Rk. III]} || ${Me.CombatAbility[Firestorm of Fists]} || ${Me.CombatAbility[Firestorm of Fists Rk. II]} || ${Me.CombatAbility[Firestorm of Fists Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[leopard claw]} || ${Me.CombatAbility[dragon fang]} || ${Me.CombatAbility[clawstriker's flurry]} || ${Me.CombatAbility[clawstriker's flurry rk. ii]} || ${Me.CombatAbility[clawstriker's flurry rk. iii]} || ${Me.CombatAbility[wheel of fists]} || ${Me.CombatAbility[wheel of fists rk. ii]} || ${Me.CombatAbility[wheel of fists rk. iii]} || ${Me.CombatAbility[Six-Step Pattern]} || ${Me.CombatAbility[Six-Step Pattern rk. ii]} || ${Me.CombatAbility[Six-Step Pattern rk. iii]} || ${Me.CombatAbility[Seven-Step Pattern]} || ${Me.CombatAbility[Seven-Step Pattern rk. ii]} || ${Me.CombatAbility[Seven-Step Pattern rk. iii]} || ${Me.CombatAbility[Eight-Step Pattern]} || ${Me.CombatAbility[Eight-Step Pattern Rk. II]} || ${Me.CombatAbility[Eight-Step Pattern Rk. III]} || ${Me.CombatAbility[Torrent of Fists]} || ${Me.CombatAbility[Torrent of Fists Rk. II]} || ${Me.CombatAbility[Torrent of Fists Rk. III]} || ${Me.CombatAbility[Firestorm of Fists]} || ${Me.CombatAbility[Firestorm of Fists Rk. II]} || ${Me.CombatAbility[Firestorm of Fists Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pLHAND, "layhand", "[#] MyLife% Below? 0=0ff", "${If[${Me.AltAbility[Lay on Hands]},20,0]}", "${If[${meleemvi[plugin]} && ${Me.AltAbility[Lay on Hands]} && ${Me.Class.ShortName.Equal[PAL]},1,0]}");
DECLARE_ABILITY_OPTION(pMELEE, "melee", "[ON/OFF] Melee Mode? 0=0ff", "${If[${Select[${Me.Class.ShortName},WAR,PAL,RNG,SHD,MNK,BRD,ROG,BST,BER]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[melee]},1,0]}");
DECLARE_ABILITY_OPTION(pMELEP, "meleepri", "[ID] Primary (Melee)?", "0", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && !${meleemvi[aggro]},1,0]}");
DECLARE_ABILITY_OPTION(pMELES, "meleesec", "[ID] Offhand (Melee)?", "0", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && !${meleemvi[aggro]},1,0]} && ${meleemvi[meleepri]}");
DECLARE_ABILITY_OPTION(pMENDS, "mend", "[#] MyLife% Below? 0=0ff", "${If[${Me.Skill[mend]},1,0]}", "${If[${meleemvi[plugin]} && ${Me.Skill[mend]},1,0]}");
DECLARE_ABILITY_OPTION(pOFREN, "opfrenzy", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[Overpowering Frenzy]} || ${Me.CombatAbility[Overpowering Frenzy Rk. II]} || ${Me.CombatAbility[Overpowering Frenzy Rk. III]} || ${Me.CombatAbility[Overwhelming Frenzy]} || ${Me.CombatAbility[Overwhelming Frenzy Rk. II]} || ${Me.CombatAbility[Overwhelming Frenzy Rk. III]} || ${Me.CombatAbility[Conquering Frenzy]} || ${Me.CombatAbility[Conquering Frenzy Rk. II]} || ${Me.CombatAbility[Conquering Frenzy Rk. III]} || ${Me.CombatAbility[Vanquishing Frenzy]} || ${Me.CombatAbility[Vanquishing Frenzy Rk. II]} || ${Me.CombatAbility[Vanquishing Frenzy Rk. III]} || ${Me.CombatAbility[Demolishing Frenzy]} || ${Me.CombatAbility[Demolishing Frenzy Rk. II]} || ${Me.CombatAbility[Demolishing Frenzy Rk. III]} || ${Me.CombatAbility[Mangling Frenzy]} || ${Me.CombatAbility[Mangling Frenzy Rk. II]} || ${Me.CombatAbility[Mangling Frenzy Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Overpowering Frenzy]} || ${Me.CombatAbility[Overpowering Frenzy Rk. II]} || ${Me.CombatAbility[Overpowering Frenzy Rk. III]} || ${Me.CombatAbility[Overwhelming Frenzy]} || ${Me.CombatAbility[Overwhelming Frenzy Rk. II]} || ${Me.CombatAbility[Overwhelming Frenzy Rk. III]} || ${Me.CombatAbility[Conquering Frenzy]} || ${Me.CombatAbility[Conquering Frenzy Rk. II]} || ${Me.CombatAbility[Conquering Frenzy Rk. III]} || ${Me.CombatAbility[Vanquishing Frenzy]} || ${Me.CombatAbility[Vanquishing Frenzy Rk. II]} || ${Me.CombatAbility[Vanquishing Frenzy Rk. III]} || ${Me.CombatAbility[Demolishing Frenzy]} || ${Me.CombatAbility[Demolishing Frenzy Rk. II]} || ${Me.CombatAbility[Demolishing Frenzy Rk. III]} || ${Me.CombatAbility[Mangling Frenzy]} || ${Me.CombatAbility[Mangling Frenzy Rk. II]} || ${Me.CombatAbility[Mangling Frenzy Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pOSTRK, "opportunisticstrike", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[Opportunistic Strike]} || ${Me.CombatAbility[Opportunistic Strike Rk. II]} || ${Me.CombatAbility[Opportunistic Strike Rk. III]} || ${Me.CombatAbility[Strategic Strike]} || ${Me.CombatAbility[Strategic Strike Rk. II]} || ${Me.CombatAbility[Strategic Strike Rk. III]} || ${Me.CombatAbility[Vital Strike]} || ${Me.CombatAbility[Vital Strike Rk. II]} || ${Me.CombatAbility[Vital Strike Rk. III]} || ${Me.CombatAbility[Calculated Strike]} || ${Me.CombatAbility[Calculated Strike Rk. II]} || ${Me.CombatAbility[Calculated Strike Rk. III]} || ${Me.CombatAbility[Cunning Strike]} || ${Me.CombatAbility[Cunning Strike Rk. II]} || ${Me.CombatAbility[Cunning Strike Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Opportunistic Strike]} || ${Me.CombatAbility[Opportunistic Strike Rk. II]} || ${Me.CombatAbility[Opportunistic Strike Rk. III]} || ${Me.CombatAbility[Strategic Strike]} || ${Me.CombatAbility[Strategic Strike Rk. II]} || ${Me.CombatAbility[Strategic Strike Rk. III]} || ${Me.CombatAbility[Vital Strike]} || ${Me.CombatAbility[Vital Strike Rk. II]} || ${Me.CombatAbility[Vital Strike Rk. III]} || ${Me.CombatAbility[Calculated Strike]} || ${Me.CombatAbility[Calculated Strike Rk. II]} || ${Me.CombatAbility[Calculated Strike Rk. III]} || ${Me.CombatAbility[Cunning Strike]} || ${Me.CombatAbility[Cunning Strike Rk. II]} || ${Me.CombatAbility[Cunning Strike Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pPETAS, "petassist", "[ON/OFF] Assist Me?", "${If[${Select[${Me.Class.ShortName},SHD,DRU,SHM,NEC,MAG,ENC,BST]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[petassist]} && ${Select[${Me.Class.ShortName},SHD,DRU,SHM,NEC,MAG,ENC,BST]},1,0]}");
DECLARE_ABILITY_OPTION(pPETDE, "petdelay", "[#] # Sec Delay Before Engaging?", "${If[${Select[${Me.Class.ShortName},SHD,DRU,SHM,NEC,MAG,ENC,BST]},0,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[petassist]} && ${Select[${Me.Class.ShortName},SHD,DRU,SHM,NEC,MAG,ENC,BST]},1,0]}");
DECLARE_ABILITY_OPTION(pPETENG, "petengagehps", "[#] TargetCurrentHPS% Below? 0=0ff", "${If[${Select[${Me.Class.ShortName},SHD,DRU,SHM,NEC,MAG,ENC,BST]},98,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[petassist]} && ${Select[${Me.Class.ShortName},SHD,DRU,SHM,NEC,MAG,ENC,BST]},1,0]}");
DECLARE_ABILITY_OPTION(pPETMN, "petmend", "[#] Mend Pet Life % Below 0=0ff?", "${If[${Me.AltAbility[mend companion]},20,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[petassist]} && ${Me.AltAbility[mend companion]},1,0]}");
DECLARE_ABILITY_OPTION(pPETRN, "petrange", "[#] Target/Pet in this range?", "${If[${Select[${Me.Class.ShortName},SHD,DRU,SHM,NEC,MAG,ENC,BST]},75,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[petassist]} && ${Select[${Me.Class.ShortName},SHD,DRU,SHM,NEC,MAG,ENC,BST]},1,0]}");
DECLARE_ABILITY_OPTION(pPICKP, "pickpocket", "[ON/OFF]?", "${If[${Me.Skill[pick pockets]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && ${Me.Skill[pick pockets]},1,0]}");
DECLARE_ABILITY_OPTION(pPINPT, "pinpoint", "[#] Endu% Above? 0=0ff", "${If[${Me.CombatAbility[Pinpoint Vulnerability]} || ${Me.CombatAbility[Pinpoint Vulnerability rk. ii]} || ${Me.CombatAbility[Pinpoint Vulnerability rk. iii]} || ${Me.CombatAbility[Pinpoint Weaknesses]} || ${Me.CombatAbility[Pinpoint Weaknesses rk. ii]} || ${Me.CombatAbility[Pinpoint Weaknesses rk. iii]} || ${Me.CombatAbility[Pinpoint Vitals]} || ${Me.CombatAbility[Pinpoint Vitals rk. ii]} || ${Me.CombatAbility[Pinpoint Vitals rk. iii]} || ${Me.CombatAbility[Pinpoint Flaws]} || ${Me.CombatAbility[Pinpoint Flaws rk. ii]} || ${Me.CombatAbility[Pinpoint Flaws rk. iii]} || ${Me.CombatAbility[Pinpoint Liabilities]} || ${Me.CombatAbility[Pinpoint Liabilities rk. ii]} || ${Me.CombatAbility[Pinpoint Liabilities rk. iii]} || ${Me.CombatAbility[Pinpoint Deficiencies]} || ${Me.CombatAbility[Pinpoint Deficiencies rk. ii]} || ${Me.CombatAbility[Pinpoint Deficiencies rk. iii]} || ${Me.CombatAbility[Pinpoint Shortcomings]} || ${Me.CombatAbility[Pinpoint Shortcomings Rk. II]} || ${Me.CombatAbility[Pinpoint Shortcomings Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Pinpoint Vulnerability]} || ${Me.CombatAbility[Pinpoint Vulnerability rk. ii]} || ${Me.CombatAbility[Pinpoint Vulnerability rk. iii]} || ${Me.CombatAbility[Pinpoint Weaknesses]} || ${Me.CombatAbility[Pinpoint Weaknesses rk. ii]} || ${Me.CombatAbility[Pinpoint Weaknesses rk. iii]} || ${Me.CombatAbility[Pinpoint Vitals]} || ${Me.CombatAbility[Pinpoint Vitals rk. ii]} || ${Me.CombatAbility[Pinpoint Vitals rk. iii]} || ${Me.CombatAbility[Pinpoint Flaws]} || ${Me.CombatAbility[Pinpoint Flaws rk. ii]} || ${Me.CombatAbility[Pinpoint Flaws rk. iii]} || ${Me.CombatAbility[Pinpoint Liabilities]} || ${Me.CombatAbility[Pinpoint Liabilities rk. ii]} || ${Me.CombatAbility[Pinpoint Liabilities rk. iii]} || ${Me.CombatAbility[Pinpoint Deficiencies]} || ${Me.CombatAbility[Pinpoint Deficiencies rk. ii]} || ${Me.CombatAbility[Pinpoint Deficiencies rk. iii]} || ${Me.CombatAbility[Pinpoint Shortcomings]} || ${Me.CombatAbility[Pinpoint Shortcomings Rk. II]} || ${Me.CombatAbility[Pinpoint Shortcomings Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pPLUGS, "plugin", "[ON/OFF]?", "1", "${If[${meleemvi[plugin]},1,0]}");
DECLARE_ABILITY_OPTION(pPOKER, "poker", "[ID] item?", "0", "${If[${meleemvi[plugin]} && ${Me.Skill[backstab]},1,0]}");
DECLARE_ABILITY_OPTION(pPRVK0, "provoke0", "[ID] spell/disc/aa/item?", "0", "${If[${meleemvi[plugin]} && ${meleemvi[aggro]} && ${meleemvi[provokemax]} && ${Select[${Me.Class.ShortName},WAR,PAL,SHD,MNK,BER]},1,0]}");
DECLARE_ABILITY_OPTION(pPRVK1, "provoke1", "[ID] spell/disc/aa/item?", "0", "${If[${meleemvi[plugin]} && ${meleemvi[aggro]} && ${meleemvi[provokemax]} && ${Select[${Me.Class.ShortName},WAR,PAL,SHD,MNK,BER]},1,0]}");
DECLARE_ABILITY_OPTION(pPRVKE, "provokeend", "[#] Stop when Target Life% Below?", "${If[${Select[${Me.Class.ShortName},WAR,PAL,SHD,MNK,BER]},20,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[aggro]} && ${meleemvi[provokemax]} && ${Select[${Me.Class.ShortName},WAR,PAL,SHD,MNK,BER]},1,0]}");
DECLARE_ABILITY_OPTION(pPRVKM, "provokemax", "[#] Counter? ,1=try once, 0=0ff", "${If[${Select[${Me.Class.ShortName},WAR,PAL,SHD,MNK,BER]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[aggro]} && ${Select[${Me.Class.ShortName},WAR,PAL,SHD,MNK,BER]},1,0]}");
DECLARE_ABILITY_OPTION(pPRVKO, "provokeonce", "[ON/OFF]?", "${If[${Select[${Me.Class.ShortName},WAR,PAL,SHD,MNK,BER]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[aggro]} && ${Select[${Me.Class.ShortName},WAR,PAL,SHD,MNK,BER]},1,0]}");
DECLARE_ABILITY_OPTION(pRAKES, "rake", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[rake]} || ${Me.CombatAbility[harrow]} || ${Me.CombatAbility[harrow rk. ii]} || ${Me.CombatAbility[harrow rk. iii]} || ${Me.CombatAbility[foray]} || ${Me.CombatAbility[foray rk. ii]} || ${Me.CombatAbility[foray rk. iii]} || ${Me.CombatAbility[rush]} || ${Me.CombatAbility[rush rk. ii]} || ${Me.CombatAbility[rush rk. iii]} || ${Me.CombatAbility[Barrage]} || ${Me.CombatAbility[Barrage rk. ii]} || ${Me.CombatAbility[Barrage rk. iii]} || ${Me.CombatAbility[Pummel]} || ${Me.CombatAbility[Pummel rk. ii]} || ${Me.CombatAbility[Pummel rk. iii]} || ${Me.CombatAbility[Maul]} || ${Me.CombatAbility[Maul rk. ii]} || ${Me.CombatAbility[Maul rk. iii]} || ${Me.CombatAbility[Mangle]} || ${Me.CombatAbility[Mangle Rk. II]} || ${Me.CombatAbility[Mangle Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[rake]} || ${Me.CombatAbility[harrow]} || ${Me.CombatAbility[harrow rk. ii]} || ${Me.CombatAbility[harrow rk. iii]} || ${Me.CombatAbility[foray]} || ${Me.CombatAbility[foray rk. ii]} || ${Me.CombatAbility[foray rk. iii]} || ${Me.CombatAbility[rush]} || ${Me.CombatAbility[rush rk. ii]} || ${Me.CombatAbility[rush rk. iii]} || ${Me.CombatAbility[Barrage]} || ${Me.CombatAbility[Barrage rk. ii]} || ${Me.CombatAbility[Barrage rk. iii]} || ${Me.CombatAbility[Pummel]} || ${Me.CombatAbility[Pummel rk. ii]} || ${Me.CombatAbility[Pummel rk. iii]} || ${Me.CombatAbility[Maul]} || ${Me.CombatAbility[Maul rk. ii]} || ${Me.CombatAbility[Maul rk. iii]} || ${Me.CombatAbility[Mangle]} || ${Me.CombatAbility[Mangle Rk. II]} || ${Me.CombatAbility[Mangle Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pRALLO, "rallos", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[Axe of Rallos]} || ${Me.CombatAbility[Axe of Rallos Rk. II]} || ${Me.CombatAbility[Axe of Rallos Rk. III]} || ${Me.CombatAbility[Axe of Graster]} || ${Me.CombatAbility[Axe of Graster Rk. II]} || ${Me.CombatAbility[Axe of Graster Rk. III]} || ${Me.CombatAbility[Axe of Illdaera]} || ${Me.CombatAbility[Axe of Illdaera Rk. II]} || ${Me.CombatAbility[Axe of Illdaera Rk. III]} || ${Me.CombatAbility[Axe of Zurel]} || ${Me.CombatAbility[Axe of Zurel Rk. II]} || ${Me.CombatAbility[Axe of Zurel Rk. III]} || ${Me.CombatAbility[Axe of Numicia]} || ${Me.CombatAbility[Axe of Numicia Rk. II]} || ${Me.CombatAbility[Axe of Numicia Rk. III]} || ${Me.CombatAbility[Axe of Rekatok]} || ${Me.CombatAbility[Axe of Rekatok Rk. II]} || ${Me.CombatAbility[Axe of Rekatok Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Axe of Rallos]} || ${Me.CombatAbility[Axe of Rallos Rk. II]} || ${Me.CombatAbility[Axe of Rallos Rk. III]} || ${Me.CombatAbility[Axe of Graster]} || ${Me.CombatAbility[Axe of Graster Rk. II]} || ${Me.CombatAbility[Axe of Graster Rk. III]} || ${Me.CombatAbility[Axe of Illdaera]} || ${Me.CombatAbility[Axe of Illdaera Rk. II]} || ${Me.CombatAbility[Axe of Illdaera Rk. III]} ${Me.CombatAbility[Axe of Illdaera Rk. III]} || ${Me.CombatAbility[Axe of Zurel]} || ${Me.CombatAbility[Axe of Zurel Rk. II]} || ${Me.CombatAbility[Axe of Zurel Rk. III]} || ${Me.CombatAbility[Axe of Numicia]} || ${Me.CombatAbility[Axe of Numicia Rk. II]} || ${Me.CombatAbility[Axe of Numicia Rk. III]} || ${Me.CombatAbility[Axe of Rekatok]} || ${Me.CombatAbility[Axe of Rekatok Rk. II]} || ${Me.CombatAbility[Axe of Rekatok Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pRANGE, "range", "[#] Max Range? 0=0ff", "0", "${If[${meleemvi[plugin]},1,0]}");
DECLARE_ABILITY_OPTION(pRAVEN, "ravens", "[ON/OFF]?", "${If[${Me.AltAbility[raven's claw]},1,0]}", "${If[${meleemvi[plugin]} && ${Me.AltAbility[raven's claw]},1,0]}");
DECLARE_ABILITY_OPTION(pRAVOL, "ragevolley", "[#] Endu% Above? 0=0ff", "${If[${Me.CombatAbility[rage volley]} || ${Me.CombatAbility[destroyer's volley]} || ${Me.CombatAbility[giantslayer's volley]} || ${Me.CombatAbility[giantslayer's volley rk. ii]} || ${Me.CombatAbility[giantslayer's volley rk. iii]} || ${Me.CombatAbility[annihilator's volley]} || ${Me.CombatAbility[annihilator's volley rk. ii]} || ${Me.CombatAbility[annihilator's volley rk. iii]} || ${Me.CombatAbility[decimator's volley]} || ${Me.CombatAbility[decimator's volley rk. ii]} || ${Me.CombatAbility[decimator's volley rk. iii]} || ${Me.CombatAbility[Eradicator's Volley]} || ${Me.CombatAbility[Eradicator's Volley rk. ii]} || ${Me.CombatAbility[Eradicator's Volley rk. iii]} || ${Me.CombatAbility[Savage Volley]} || ${Me.CombatAbility[Savage Volley rk. ii]} || ${Me.CombatAbility[Savage Volley rk. iii]} || ${Me.CombatAbility[Sundering Volley]} || ${Me.CombatAbility[Sundering Volley rk. ii]} || ${Me.CombatAbility[Sundering Volley rk. iii]} || ${Me.CombatAbility[Brutal Volley]} || ${Me.CombatAbility[Brutal Volley rk. ii]} || ${Me.CombatAbility[Brutal Volley rk. iii]} || ${Me.CombatAbility[Demolishing Volley]} || ${Me.CombatAbility[Demolishing Volley rk. ii]} || ${Me.CombatAbility[Demolishing Volley rk. iii]} || ${Me.CombatAbility[Mangling Volley]} || ${Me.CombatAbility[Mangling Volley Rk. II]} || ${Me.CombatAbility[Mangling Volley Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[rage volley]} || ${Me.CombatAbility[destroyer's volley]} || ${Me.CombatAbility[giantslayer's volley]} || ${Me.CombatAbility[giantslayer's volley rk. ii]} || ${Me.CombatAbility[giantslayer's volley rk. iii]} || ${Me.CombatAbility[annihilator's volley]} || ${Me.CombatAbility[annihilator's volley rk. ii]} || ${Me.CombatAbility[annihilator's volley rk. iii]} || ${Me.CombatAbility[decimator's volley]} || ${Me.CombatAbility[decimator's volley rk. ii]} || ${Me.CombatAbility[decimator's volley rk. iii]} || ${Me.CombatAbility[Eradicator's Volley]} || ${Me.CombatAbility[Eradicator's Volley rk. ii]} || ${Me.CombatAbility[Eradicator's Volley rk. iii]} || ${Me.CombatAbility[Savage Volley]} || ${Me.CombatAbility[Savage Volley rk. ii]} || ${Me.CombatAbility[Savage Volley rk. iii]} || ${Me.CombatAbility[Sundering Volley]} || ${Me.CombatAbility[Sundering Volley rk. ii]} || ${Me.CombatAbility[Sundering Volley rk. iii]} || ${Me.CombatAbility[Brutal Volley]} || ${Me.CombatAbility[Brutal Volley rk. ii]} || ${Me.CombatAbility[Brutal Volley rk. iii]} || ${Me.CombatAbility[Demolishing Volley]} || ${Me.CombatAbility[Demolishing Volley rk. ii]} || ${Me.CombatAbility[Demolishing Volley rk. iii]} || ${Me.CombatAbility[Mangling Volley]} || ${Me.CombatAbility[Mangling Volley Rk. II]} || ${Me.CombatAbility[Mangling Volley Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pRESUM, "resume", "[#] Life% Above? 100=0ff", "75", "${If[${meleemvi[plugin]} && !${meleemvi[aggro]},1,0]}");
DECLARE_ABILITY_OPTION(pRGHTI, "rightidg", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[Righteous Indignation]} || ${Me.CombatAbility[Righteous Indignation rk. ii]} || ${Me.CombatAbility[Righteous Indignation rk. iii]} || ${Me.CombatAbility[Righteous Vexation]} || ${Me.CombatAbility[Righteous Vexation rk. ii]} || ${Me.CombatAbility[Righteous Vexation rk. iii]} || ${Me.CombatAbility[Righteous Umbrage]} || ${Me.CombatAbility[Righteous Umbrage rk. ii]} || ${Me.CombatAbility[Righteous Umbrage rk. iii]} || ${Me.CombatAbility[Righteous Condemnation]} || ${Me.CombatAbility[Righteous Condemnation Rk. II]} || ${Me.CombatAbility[Righteous Condemnation Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Righteous Indignation]} || ${Me.CombatAbility[Righteous Indignation rk. ii]} || ${Me.CombatAbility[Righteous Indignation rk. iii]} || ${Me.CombatAbility[Righteous Vexation]} || ${Me.CombatAbility[Righteous Vexation rk. ii]} || ${Me.CombatAbility[Righteous Vexation rk. iii]} || ${Me.CombatAbility[Righteous Umbrage]} || ${Me.CombatAbility[Righteous Umbrage rk. ii]} || ${Me.CombatAbility[Righteous Umbrage rk. iii]} || ${Me.CombatAbility[Righteous Condemnation]} || ${Me.CombatAbility[Righteous Condemnation Rk. II]} || ${Me.CombatAbility[Righteous Condemnation Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pRKICK, "roundkick", "[ON/OFF]?", "${If[${Me.Skill[round kick]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && ${Me.Skill[round kick]},1,0]}");
DECLARE_ABILITY_OPTION(pSBLADES, "stormblades", "[#] Endu% Above? 0=0ff", "${If[${Me.CombatAbility[focused storm of blades]} || ${Me.CombatAbility[focused storm of blades rk. ii]} || ${Me.CombatAbility[focused storm of blades rk. iii]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[focused storm of blades]} || ${Me.CombatAbility[focused storm of blades rk. ii]} || ${Me.CombatAbility[focused storm of blades rk. iii]}),1,0]}");
DECLARE_ABILITY_OPTION(pSELOK, "selos", "[ON/OFF]?", "${If[${Me.AltAbility[selo's kick]},1,0]}", "${If[${meleemvi[plugin]} && ${Me.AltAbility[selo's kick]},1,0]}");
DECLARE_ABILITY_OPTION(pSENSE, "sensetraps", "[ON/OFF]?", "0", "${If[${meleemvi[plugin]} && ${Me.Skill[sense traps]},1,0]}");
DECLARE_ABILITY_OPTION(pSHIEL, "shield", "[ID] item?", "0", "${If[${meleemvi[plugin]} && ${Me.Skill[bash]},0,0]}");
DECLARE_ABILITY_OPTION(pSLAMS, "slam", "[ON/OFF]?", "${If[${Select[${Me.Race.ID},2,9,10]},1,0]}", "${If[${meleemvi[plugin]} && ${Select[${Me.Race.ID},2,9,10]},1,0]}"); // 2=barbarian 9=troll 10=ogre
DECLARE_ABILITY_OPTION(pSLAPF, "slapface", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[Slap in the Face]} || ${Me.CombatAbility[Slap in the Face rk. ii]} || ${Me.CombatAbility[Slap in the Face rk. iii]} || ${Me.CombatAbility[Kick in the Teeth]} || ${Me.CombatAbility[Kick in the Teeth rk. ii]} || ${Me.CombatAbility[Kick in the Teeth rk. iii]} || ${Me.CombatAbility[Punch in the Throat]} || ${Me.CombatAbility[Punch in the Throat rk. ii]} || ${Me.CombatAbility[Punch in the Throat rk. iii]} || ${Me.CombatAbility[Kick in the Shins]} || ${Me.CombatAbility[Kick in the Shins rk. ii]} || ${Me.CombatAbility[Kick in the Shins rk. iii]} || ${Me.CombatAbility[Sucker Punch]} || ${Me.CombatAbility[Sucker Punch Rk. II]} || ${Me.CombatAbility[Sucker Punch Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Slap in the Face]} || ${Me.CombatAbility[Slap in the Face rk. ii]} || ${Me.CombatAbility[Slap in the Face rk. iii]} || ${Me.CombatAbility[Kick in the Teeth]} || ${Me.CombatAbility[Kick in the Teeth rk. ii]} || ${Me.CombatAbility[Kick in the Teeth rk. iii]} || ${Me.CombatAbility[Punch in the Throat]} || ${Me.CombatAbility[Punch in the Throat rk. ii]} || ${Me.CombatAbility[Punch in the Throat rk. iii]} || ${Me.CombatAbility[Kick in the Shins]} || ${Me.CombatAbility[Kick in the Shins rk. ii]} || ${Me.CombatAbility[Kick in the Shins rk. iii]} || ${Me.CombatAbility[Sucker Punch]} || ${Me.CombatAbility[Sucker Punch Rk. II]} || ${Me.CombatAbility[Sucker Punch Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pSNEAK, "sneak", "[ON/OFF]?", "0", "${If[${meleemvi[plugin]} && ${Me.Skill[sneak]},1,0]}");
DECLARE_ABILITY_OPTION(pSTAND, "standup", "[ON/OFF] Authorize to StandUp?", "0", "${If[${meleemvi[plugin]},1,0]}");
DECLARE_ABILITY_OPTION(pSTEEL, "steely", "[ON/OFF]", "${If[${Select[${Me.Class.ShortName},PAL,SHD]} && (${Me.Book[Steely Stance]} || ${Me.Book[Steely Stance rk. ii]} || ${Me.Book[Steely Stance rk. iii]} || ${Me.Book[Stubborn Stance]} || ${Me.Book[Stubborn Stance rk. ii]} || ${Me.Book[Stubborn Stance rk. iii]} || ${Me.Book[Stoic Stance]} || ${Me.Book[Stoic Stance rk. ii]} || ${Me.Book[Stoic Stance rk. iii]} || ${Me.Book[Steadfast Stance]} || ${Me.Book[Steadfast Stance rk. ii]} || ${Me.Book[Steadfast Stance rk. iii]} || ${Me.Book[Staunch Stance]} || ${Me.Book[Staunch Stance rk. ii]} || ${Me.Book[Staunch Stance rk. iii]} || ${Me.Book[Defiant Stance]} || ${Me.Book[Defiant Stance Rk. II]} || ${Me.Book[Defiant Stance Rk. III]}),0,0]}", "${If[${meleemvi[plugin]} && ${Select[${Me.Class.ShortName},PAL,SHD]} && (${Me.Book[Steely Stance]} || ${Me.Book[Steely Stance rk. ii]} || ${Me.Book[Steely Stance rk. iii]} || ${Me.Book[Stubborn Stance]} || ${Me.Book[Stubborn Stance rk. ii]} || ${Me.Book[Stubborn Stance rk. iii]} || ${Me.Book[Stoic Stance]} || ${Me.Book[Stoic Stance rk. ii]} || ${Me.Book[Stoic Stance rk. iii]} || ${Me.Book[Steadfast Stance]} || ${Me.Book[Steadfast Stance rk. ii]} || ${Me.Book[Steadfast Stance rk. iii]} || ${Me.Book[Staunch Stance]} || ${Me.Book[Staunch Stance rk. ii]} || ${Me.Book[Staunch Stance rk. iii]} || ${Me.Book[Defiant Stance]} || ${Me.Book[Defiant Stance Rk. II]} || ${Me.Book[Defiant Stance Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pSTIKD, "stickdelay", "[#] Sec to Wait Target in Range?", "0", "${If[${meleemvi[plugin]} && ${Stick.Status.NotEqual[NULL]},1,0]}");
DECLARE_ABILITY_OPTION(pSTIKKB, "stickbreak", "0=Normal, 1=Allow BreakOnKB", "0", "${If[${meleemvi[plugin]} && ${Stick.Status.NotEqual[NULL]},1,0]}");
DECLARE_ABILITY_OPTION(pSTIKM, "stickmode", "[ON/OFF] Use stickcmd from ini?", "0", "${If[${meleemvi[plugin]} && ${Stick.Status.NotEqual[NULL]},1,0]}");
DECLARE_ABILITY_OPTION(pSTIKNR, "sticknorange", "0=Normal, 1=No Range Check", "0", "${If[${meleemvi[plugin]} && ${Stick.Status.NotEqual[NULL]},1,0]}");
DECLARE_ABILITY_OPTION(pSTIKR, "stickrange", "[#] Target in Range? 0=0ff", "${If[${Stick.Status.NotEqual[NULL]},75,0]}", "${If[${meleemvi[plugin]} && ${Stick.Status.NotEqual[NULL]} && ${meleemvi[stickrange]},1,0]}");
DECLARE_ABILITY_OPTION(pSTRIK, "strike", "Use best sneak attack disc [ON/OFF]?", "0", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && ${meleemvi[backstab]} && ${meleemvi[idstrike]},1,0]}");
DECLARE_ABILITY_OPTION(pSTRIKM, "strikemode", "[ON/OFF] Use strikecmd from ini?", "0", "${If[${meleemvi[plugin]} && ${Stick.Status.NotEqual[NULL]},1,0]}");
DECLARE_ABILITY_OPTION(pSTUN0, "stun0", "[ID] spell/disc/aa/item?", "0", "${If[${meleemvi[plugin]} && ${meleemvi[stunning]},1,0]}");
DECLARE_ABILITY_OPTION(pSTUN1, "stun1", "[ID] spell/disc/aa/item?", "0", "${If[${meleemvi[plugin]} && ${meleemvi[stunning]},1,0]}");
DECLARE_ABILITY_OPTION(pSTUNS, "stunning", "[#] Target Life% Below? 0=0ff", "0", "${If[${meleemvi[plugin]},1,0]}");
DECLARE_ABILITY_OPTION(pSYNGY, "synergy", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[Calanin's Synergy]} || ${Me.CombatAbility[Calanin's Synergy Rk. II]} || ${Me.CombatAbility[Calanin's Synergy Rk. III]} || ${Me.CombatAbility[Dreamwalker's Synergy]} || ${Me.CombatAbility[Dreamwalker's Synergy Rk. II]} || ${Me.CombatAbility[Dreamwalker's Synergy Rk. III]} || ${Me.CombatAbility[Veilwalker's Synergy]} || ${Me.CombatAbility[Veilwalker's Synergy Rk. II]} || ${Me.CombatAbility[Veilwalker's Synergy Rk. III]} || ${Me.CombatAbility[Shadewalker's Synergy]} || ${Me.CombatAbility[Shadewalker's Synergy Rk. II]} || ${Me.CombatAbility[Shadewalker's Synergy Rk. III]} || ${Me.CombatAbility[Doomwalker's Synergy]} || ${Me.CombatAbility[Doomwalker's Synergy Rk. II]} || ${Me.CombatAbility[Doomwalker's Synergy Rk. III]} || ${Me.CombatAbility[Firewalker's Synergy]} || ${Me.CombatAbility[Firewalker's Synergy Rk. II]} || ${Me.CombatAbility[Firewalker's Synergy Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Calanin's Synergy]} || ${Me.CombatAbility[Calanin's Synergy Rk. II]} || ${Me.CombatAbility[Calanin's Synergy Rk. III]} || ${Me.CombatAbility[Dreamwalker's Synergy]} || ${Me.CombatAbility[Dreamwalker's Synergy Rk. II]} || ${Me.CombatAbility[Dreamwalker's Synergy Rk. III]} || ${Me.CombatAbility[Veilwalker's Synergy]} || ${Me.CombatAbility[Veilwalker's Synergy Rk. II]} || ${Me.CombatAbility[Veilwalker's Synergy Rk. III]} || ${Me.CombatAbility[Shadewalker's Synergy]} || ${Me.CombatAbility[Shadewalker's Synergy Rk. II]} || ${Me.CombatAbility[Shadewalker's Synergy Rk. III]} || ${Me.CombatAbility[Doomwalker's Synergy]} || ${Me.CombatAbility[Doomwalker's Synergy Rk. II]} || ${Me.CombatAbility[Doomwalker's Synergy Rk. III]} || ${Me.CombatAbility[Firewalker's Synergy]} || ${Me.CombatAbility[Firewalker's Synergy Rk. II]} || ${Me.CombatAbility[Firewalker's Synergy Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pTAUNT, "taunt", "[ON/OFF]?", "${If[${Me.Skill[taunt]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && ${meleemvi[aggro]} && ${Me.Skill[taunt]},1,0]}");
DECLARE_ABILITY_OPTION(pTHIEF, "thiefeye", "[#] Endu% Above? 0=0ff", "${If[${Me.CombatAbility[thief's eyes]} || ${Me.CombatAbility[Thief's Vision]} || ${Me.CombatAbility[Thief's Vision Rk. II]} || ${Me.CombatAbility[Thief's Vision Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[thief's eyes]} || ${Me.CombatAbility[Thief's Vision]} || ${Me.CombatAbility[Thief's Vision Rk. II]} || ${Me.CombatAbility[Thief's Vision Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pTHROW, "throwstone", "[#] Endu% Above? 0=0ff", "0", "${If[${meleemvi[plugin]} && ${Me.CombatAbility[throw stone]},1,0]}");
DECLARE_ABILITY_OPTION(pTIGER, "tigerclaw", "[ON/OFF]?", "${If[${Me.Skill[tiger claw]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && ${Me.Skill[tiger claw]},1,0]}");
DECLARE_ABILITY_OPTION(pTTJAB, "throatjab", "[ON/OFF]?", "${If[${Me.AltAbility[throat jab]},1,0]}", "${If[${meleemvi[plugin]} && ${Me.AltAbility[throat jab]},1,0]}");
DECLARE_ABILITY_OPTION(pTWIST, "twistedshank", "[ON/OFF]?", "${If[${Me.AltAbility[twisted shank]},1,0]}", "${If[${meleemvi[plugin]} && ${meleemvi[melee]} && ${Me.AltAbility[twisted shank]},1,0]}");
DECLARE_ABILITY_OPTION(pVIGAX, "vigaxe", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[Vigorous Axe Throw]} || ${Me.CombatAbility[Vigorous Axe Throw Rk. II]} || ${Me.CombatAbility[Vigorous Axe Throw Rk. III]} || ${Me.CombatAbility[Energetic Axe Throw]} || ${Me.CombatAbility[Energetic Axe Throw Rk. II]} || ${Me.CombatAbility[Energetic Axe Throw Rk. III]} || ${Me.CombatAbility[Spirited Axe Throw]} || ${Me.CombatAbility[Spirited Axe Throw Rk. II]} || ${Me.CombatAbility[Spirited Axe Throw Rk. III]} || ${Me.CombatAbility[Brutal Axe Throw]} || ${Me.CombatAbility[Brutal Axe Throw Rk. II]} || ${Me.CombatAbility[Brutal Axe Throw Rk. III]} || ${Me.CombatAbility[Demolishing Axe Throw]} || ${Me.CombatAbility[Demolishing Axe Throw Rk. II]} || ${Me.CombatAbility[Demolishing Axe Throw Rk. III]} || ${Me.CombatAbility[Mangling Axe Throw]} || ${Me.CombatAbility[Mangling Axe Throw Rk. II]} || ${Me.CombatAbility[Mangling Axe Throw Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Vigorous Axe Throw]} || ${Me.CombatAbility[Vigorous Axe Throw Rk. II]} || ${Me.CombatAbility[Vigorous Axe Throw Rk. III]} || ${Me.CombatAbility[Energetic Axe Throw]} || ${Me.CombatAbility[Energetic Axe Throw Rk. II]} || ${Me.CombatAbility[Energetic Axe Throw Rk. III]} || ${Me.CombatAbility[Spirited Axe Throw]} || ${Me.CombatAbility[Spirited Axe Throw Rk. II]} || ${Me.CombatAbility[Spirited Axe Throw Rk. III]} || ${Me.CombatAbility[Brutal Axe Throw]} || ${Me.CombatAbility[Brutal Axe Throw Rk. II]} || ${Me.CombatAbility[Brutal Axe Throw Rk. III]} || ${Me.CombatAbility[Demolishing Axe Throw]} || ${Me.CombatAbility[Demolishing Axe Throw Rk. II]} || ${Me.CombatAbility[Demolishing Axe Throw Rk. III]} || ${Me.CombatAbility[Mangling Axe Throw]} || ${Me.CombatAbility[Mangling Axe Throw Rk. II]} || ${Me.CombatAbility[Mangling Axe Throw Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pVIGDR, "vigdagger", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[Vigorous Dagger-Throw]} || ${Me.CombatAbility[Vigorous Dagger-Throw Rk. II]} || ${Me.CombatAbility[Vigorous Dagger-Throw Rk. III]} || ${Me.CombatAbility[Vigorous Dagger-Strike]} || ${Me.CombatAbility[Vigorous Dagger-Strike Rk. II]} || ${Me.CombatAbility[Vigorous Dagger-Strike Rk. III]} || ${Me.CombatAbility[Energetic Dagger-Strike]} || ${Me.CombatAbility[Energetic Dagger-Strike Rk. II]} || ${Me.CombatAbility[Energetic Dagger-Strike Rk. III]} || ${Me.CombatAbility[Energetic Dagger-Throw]} || ${Me.CombatAbility[Energetic Dagger-Throw Rk. II]} || ${Me.CombatAbility[Energetic Dagger-Throw Rk. III]} || ${Me.CombatAbility[Exuberant Dagger-Throw]} || ${Me.CombatAbility[Exuberant Dagger-Throw Rk. II]} || ${Me.CombatAbility[Exuberant Dagger-Throw Rk. III]} || ${Me.CombatAbility[Forceful Dagger-Throw]} || ${Me.CombatAbility[Forceful Dagger-Throw Rk. II]} || ${Me.CombatAbility[Forceful Dagger-Throw Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Vigorous Dagger-Throw]} || ${Me.CombatAbility[Vigorous Dagger-Throw Rk. II]} || ${Me.CombatAbility[Vigorous Dagger-Throw Rk. III]} || ${Me.CombatAbility[Vigorous Dagger-Strike]} || ${Me.CombatAbility[Vigorous Dagger-Strike Rk. II]} || ${Me.CombatAbility[Vigorous Dagger-Strike Rk. III]} || ${Me.CombatAbility[Energetic Dagger-Strike]} || ${Me.CombatAbility[Energetic Dagger-Strike Rk. II]} || ${Me.CombatAbility[Energetic Dagger-Strike Rk. III]} || ${Me.CombatAbility[Energetic Dagger-Throw]} || ${Me.CombatAbility[Energetic Dagger-Throw Rk. II]} || ${Me.CombatAbility[Energetic Dagger-Throw Rk. III]} || ${Me.CombatAbility[Exuberant Dagger-Throw]} || ${Me.CombatAbility[Exuberant Dagger-Throw Rk. II]} || ${Me.CombatAbility[Exuberant Dagger-Throw Rk. III]} || ${Me.CombatAbility[Forceful Dagger-Throw]} || ${Me.CombatAbility[Forceful Dagger-Throw Rk. II]} || ${Me.CombatAbility[Forceful Dagger-Throw Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pVIGSN, "vigshuriken", "[#] Endu% Above? 0=Off", "${If[${Me.CombatAbility[Vigorous Shuriken]} || ${Me.CombatAbility[Vigorous Shuriken Rk. II]} || ${Me.CombatAbility[Vigorous Shuriken Rk. III]},20,0]}", "${If[${meleemvi[plugin]} && (${Me.CombatAbility[Vigorous Shuriken]} || ${Me.CombatAbility[Vigorous Shuriken Rk. II]} || ${Me.CombatAbility[Vigorous Shuriken Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pWITHS, "withstand", "[#] Endu% Above? 0=Off", "${If[${Select[${Me.Class.ShortName},PAL,SHD]} && (${Me.CombatAbility[withstand]} || ${Me.CombatAbility[withstand rk. ii]} || ${Me.CombatAbility[withstand rk. iii]} || ${Me.CombatAbility[defy]} || ${Me.CombatAbility[defy rk. ii]} || ${Me.CombatAbility[defy rk. iii]} || ${Me.CombatAbility[Reprove]} || ${Me.CombatAbility[Reprove rk. ii]} || ${Me.CombatAbility[Reprove rk. iii]} || ${Me.CombatAbility[Repel]} || ${Me.CombatAbility[Repel rk. ii]} || ${Me.CombatAbility[Repel rk. iii]} || ${Me.CombatAbility[Spurn]} || ${Me.CombatAbility[Spurn Rk. II]} || ${Me.CombatAbility[Spurn Rk. III]}),20,0]}", "${If[${meleemvi[plugin]} && ${Select[${Me.Class.ShortName},PAL,SHD]} && (${Me.CombatAbility[withstand]} || ${Me.CombatAbility[withstand rk. ii]} || ${Me.CombatAbility[withstand rk. iii]} || ${Me.CombatAbility[defy]} || ${Me.CombatAbility[defy rk. ii]} || ${Me.CombatAbility[defy rk. iii]} || ${Me.CombatAbility[Reprove]} || ${Me.CombatAbility[Reprove rk. ii]} || ${Me.CombatAbility[Reprove rk. iii]} || ${Me.CombatAbility[Repel]} || ${Me.CombatAbility[Repel rk. ii]} || ${Me.CombatAbility[Repel rk. iii]} || ${Me.CombatAbility[Spurn]} || ${Me.CombatAbility[Spurn Rk. II]} || ${Me.CombatAbility[Spurn Rk. III]}),1,0]}");
DECLARE_ABILITY_OPTION(pYAULP, "yaulp", "[ON/OFF]?", "${If[${Me.AltAbility[yaulp]},1,0]}", "${If[${meleemvi[plugin]} && ${Me.AltAbility[yaulp]},1,0]}");


char* UI_PetBack = "back";
char* UI_PetAttk = "attack";

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

unsigned long   AACheck(unsigned long id);
unsigned long   AAPoint(unsigned long index);
int             AAReady(unsigned long index);
long            Aggroed(unsigned long id);
FLOAT           AngularDistance(float h1, float h2);
DOUBLE          AngularHeading(PSPAWNINFO t, PSPAWNINFO s);
int             CACheck(unsigned long id);
int             CAPress(unsigned long id);
int             Casting(char* command);
int             CursorEmpty();
long            Discipline();
int             Equip(unsigned long ID, long SlotID);
int             Equipped(unsigned long id);
long            Evaluate(char* zFormat, ...);
void            MeleeReset();
long            OkayToEquip(long Size = NOID);
PMQPLUGIN       Plugin(char* PluginName);
void*           PluginEntry(char* PluginName, char* FuncName);
int             SKCheck(unsigned long id);
int             SKReady(unsigned long id);
int             SKPress(unsigned long id);
long            SpawnMask(PSPAWNINFO x);
int             SpellCheck(unsigned long id);
long            SpellGemID(unsigned long ID, long slotid = NOID);
int             SpellReady(unsigned long ID, long SlotID = NOID);
int             Stick(char* command);
long            Unequip(long SlotID);
//void            WinClick(CXWnd *Wnd, char* ScreenID, char* ClickNotification, unsigned long KeyState);
PSTR            WinTexte(CXWnd *Wnd, char* ScreenID, PSTR Buffer);

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
long doDEBUG;

void Announce(unsigned long Wanted, char* Format, ...) {
    char Output[MAX_STRING] = { 0 }; va_list vaList; va_start(vaList, Format); vsprintf_s(Output, Format, vaList);
    if (Output[0]) {
        unsigned long Result = 0; PMQPLUGIN pPlugin = pPlugins;
        while (pPlugin) {
            fMQIncomingChat Request = (fMQIncomingChat)GetProcAddress(pPlugin->hModule, "OnMeleeChat");
            if (Request) Result |= Request(Output, Wanted);
            pPlugin = pPlugin->pNext;
        }
        if (!Result && ( Wanted || doDEBUG ) ) WriteChatf("%s", Output);
    }
}

#define GetSpawnID(spawnid) (PSPAWNINFO)GetSpawnByID(spawnid)
#define TargetIT(X) *(PSPAWNINFO*)ppTarget=X

static inline int WinState(CXWnd *Wnd) {
    return (Wnd && ((PCSIDLWND)Wnd)->dShow);
}

static inline PSPAWNINFO Target() {
    if (ppTarget) return (PSPAWNINFO)pTarget;
    return NULL;
}

static inline int TargetType(unsigned long mask) {
    return (SpawnMask(Target())&mask);
}

static inline int TargetID(unsigned long ID) {
    if (ID && pTarget) return (ID == Target()->SpawnID);
    return false;
}

static inline float SpeedRun(PSPAWNINFO x) {
    if (x && x->Mount) return x->Mount->SpeedRun;
    if (x) return x->SpeedRun;
    return 0.0f;
}

static inline int SpawnType(PSPAWNINFO x, unsigned long mask) {
    return (SpawnMask(x)&mask);
}

static inline PSPAWNINFO SpawnMe() {
    if(pCharSpawn) {
        return (PSPAWNINFO)pCharSpawn;
    }
	return NULL;
}

static inline PSPAWNINFO SpawnMount() {
    return SpawnMe() ? SpawnMe()->Mount : NULL;
}

static inline PSPAWNINFO SpawnPet() {
    return (SpawnMe() && (long)SpawnMe()->PetID > 0) ? (PSPAWNINFO)GetSpawnByID(SpawnMe()->PetID) : NULL;
}

static inline unsigned long StandState() {
    return SpawnMe() ? SpawnMe()->StandState : 0;
}

static inline int Stackable(PCONTENTS Item) {
    return (Item && GetItemFromContents(Item)->Type == ITEMTYPE_NORMAL && ((EQ_Item*)Item)->IsStackable());
}

static inline long StackUnit(PCONTENTS Item) {
    return (!Stackable(Item)) ? 1 : Item->StackCount;
}

static inline int IsStunned() {
    return (GetCharInfo() && GetCharInfo()->Stunned);
}

static inline int IsStanding() {
    if (SpawnMount()) return true;
    return StandState() == STANDSTATE_STAND;
}

static inline int IsSneaking() {
    return (SpawnMe() && SpawnMe()->Sneak);
}

static inline int IsInvisible() {
    return (SpawnMe() && SpawnMe()->HideMode);
}

static inline int IsGrouped() {
    return (GetCharInfo() && GetCharInfo()->pGroupInfo);
}

static inline int IsFeigning() {
    return StandState() == STANDSTATE_FEIGN;
}

static inline int InRange(PSPAWNINFO a, PSPAWNINFO b, float d) {
    if (!a || !b) return false;
    return (DistanceToSpawn(a, b) <= d);
}

static inline int InGame() {
    return (gbInZone && gGameState == GAMESTATE_INGAME && SpawnMe() && GetCharInfo2() && GetCharInfo() && GetCharInfo()->Stunned != 3);
}

static inline CXWnd* XMLChild(CXWnd* window, char* screenid) {
    if (window) return window->GetChildItem(screenid);
    return NULL;
}

static inline int XMLEnabled(CXWnd* window) {
    return (window && ((PCSIDLWND)window)->Enabled);
}

static inline PCONTENTS ContAmmo() {
    if (PCHARINFO2 Me = GetCharInfo2()) return Me->pInventoryArray->Inventory.Ammo;
    return NULL;
}

static inline PCONTENTS ContPrimary() {
    if (PCHARINFO2 Me = GetCharInfo2()) return Me->pInventoryArray->Inventory.Primary;
    return NULL;
}

static inline PCONTENTS ContRange() {
    if (PCHARINFO2 Me = GetCharInfo2()) return Me->pInventoryArray->Inventory.Range;
    return NULL;
}

static inline PCONTENTS ContSecondary() {
    if (PCHARINFO2 Me = GetCharInfo2()) return Me->pInventoryArray->Inventory.Secondary;
    return NULL;
}

static inline int PokerType(PCONTENTS item) {
    return (item && GetItemFromContents(item)->ItemType == 2);
}

static inline int ShieldType(PCONTENTS item) {
    return(item && GetItemFromContents(item)->ItemType == 8);
}

static inline int TwohandType(PCONTENTS item) {
    if (item) {
        if (GetItemFromContents(item)->ItemType == 1)   return true;
        if (GetItemFromContents(item)->ItemType == 4)   return true;
        if (GetItemFromContents(item)->ItemType == 35)  return true;
    }
    return false;
}

unsigned long AACheck(unsigned long id) {
    int level = -1;
    if (PSPAWNINFO pMe = (PSPAWNINFO)pLocalPlayer) {
        level = pMe->Level;
    }
    if (pAltAdvManager)
        if (PCHARINFO2 Me = GetCharInfo2())
            if (_AALIST* AAList = Me->AAList)
                if (id)
                    for (unsigned long nAbility = 0; nAbility < AA_CHAR_MAX_REAL; nAbility++)
                        if (long AAIndex = AAList[nAbility].AAIndex)
                            if (PALTABILITY ability = GetAAByIdWrapper(AAIndex, level))
                                if (ability->ID == id) return AAIndex;
    return false;
}

unsigned long AAPoint(unsigned long index) {
    if (PCHARINFO2 Me = GetCharInfo2())
        if (_AALIST* AAList = Me->AAList)
            if (index)
                for (unsigned long nAbility = 0; nAbility < AA_CHAR_MAX_REAL; nAbility++)
                    if (index == AAList[nAbility].AAIndex)
                        return  AAList[nAbility].PointsSpent;
    return 0;
}

int AAReady(unsigned long index) {
    int result = 0;
    int level = -1;
    if (PSPAWNINFO pMe = (PSPAWNINFO)pLocalPlayer) {
    level = pMe->Level;
    }
    if (pAltAdvManager)
    {
        if (index)
            if (PALTABILITY ability = GetAAByIdWrapper(index, level))
            {
                //unsigned long i = 0;
                //i = pAltAdvManager->GetCalculatedTimer(pPCData, ability);
                //DebugSpew("ability timer: %d", i);
                if (pAltAdvManager->GetCalculatedTimer(pPCData, ability) > 0)
                {
                    pAltAdvManager->IsAbilityReady(pPCData, ability, &result);
                    //DebugSpew("result: %d", result);
                }
            }
    }
    return (result < 0);
}

long Aggroed(unsigned long id) {
    if (PSPAWNINFO self = SpawnMe())
        if (PSPAWNINFO kill = GetSpawnID(id))
            if (PSPAWNINFO targ = Target()) {
                if (targ == kill && self->SpawnID == self->TargetOfTarget)  return  1; // im on hott
                if (fabs(AngularHeading(kill, self))<8.0f)       return  1; // it's facing me
                if (FindSpeed(kill)>0.0f && kill->HPCurrent<20) return -1; // it's moving
                if (InRange(self, targ, 25.0f))                   return -1; // close enough
            }
    return 0;
}

FLOAT AngularDistance(float h1, float h2) {
    if (h1 == h2) return 0.0;
    if (fabs(h1 - h2) > 256.0) * (h1 < h2 ? &h1 : &h2) += 512.0;
    return (fabs(h1 - h2) > 256.0) ? (h2 - h1) : (h1 - h2);
}

DOUBLE AngularHeading(PSPAWNINFO t, PSPAWNINFO s) {
    double Head = t->Heading - (float)atan2(s->X - t->X, s->Y - t->Y) * 256.0 / PI;
    if (Head > 256.0f) Head -= 512.0f; else if (Head < -256.0f) Head += 512.0f;
    return Head;
}

int CACheck(unsigned long id) {
    if (PCHARINFO2 Me = GetCharInfo2())
        if (unsigned long* CombatAbilities = Me->CombatAbilities)
            if (id)
                for (unsigned long nCombat = 0; nCombat < NUM_COMBAT_ABILITIES; nCombat++)
                    if (pCombatSkillsSelectWnd->ShouldDisplayThisSkill(nCombat))
                        if (id == CombatAbilities[nCombat]) return true;
    return false;
}

int CAPress(unsigned long id) {
    pCharData->DoCombatAbility(id);
    return true;
}

int Casting(char* command) {
    typedef void(__cdecl *fCALL)(PSPAWNINFO, char*);
    if (fCALL request = (fCALL)PluginEntry("mq2cast", "CastCommand")) {
        Announce(SHOW_CASTING, "%s::Casting [\ay%s\ax].", PLUGIN_NAME, command);
        request(NULL, command);
        return true;
    }
    return false;
}

void Command(char* zFormat, ...) {
    CHAR zOutput[MAX_STRING] = { 0 }; va_list vaList; va_start(vaList, zFormat);
    vsprintf_s(zOutput, zFormat, vaList);
    if (zOutput[0]) EzCommand(zOutput);
}

int CursorEmpty() {
    if (PCHARINFO2 Me = GetCharInfo2())
        if (!Me->pInventoryArray->Inventory.Cursor)
            if (!Me->CursorPlat)
                if (!Me->CursorGold)
                    if (!Me->CursorSilver)
                        if (!Me->CursorCopper)
                            return true;
    return false;
}

long Discipline() {
    char temps[MAX_STRING];
    PSPELL spell = GetSpellByName(WinTexte((CXWnd*)pCombatAbilityWnd, "CAW_CombatEffectLabel", temps));
    return (spell) ? spell->ID : 0;
}

int Equipped(unsigned long id) {
    if (id)
        for (int i = 0; i < BAG_SLOT_START; i++)
            if (PCONTENTS Cont = GetCharInfo2()->pInventoryArray->InventoryArray[i])
                if (id == GetItemFromContents(Cont)->ItemNumber) return true;
    return false;
}

long Evaluate(char* zFormat, ...) {
    char zOutput[MAX_STRING] = { 0 }; va_list vaList; va_start(vaList, zFormat);
    vsprintf_s(zOutput, zFormat, vaList); if (!zOutput[0]) return 1;
    //DebugSpewAlways("E[%s]",zOutput);
    ParseMacroData(zOutput, sizeof(zOutput));
    //DebugSpewAlways("R[%s]",zOutput);
    return atoi(zOutput);
}

long ItemTimer(PCONTENTS pItem) {
    if (GetItemFromContents(pItem)->Clicky.TimerID != 0xFFFFFFFF) return GetItemTimer(pItem);
    if (GetItemFromContents(pItem)->Clicky.SpellID != 0xFFFFFFFF) return 0;
    return 999999;
}

PMQPLUGIN Plugin(char* PluginName) {
    long Length = strlen(PluginName) + 1;
    PMQPLUGIN pLook = pPlugins;
    while (pLook && _strnicmp(PluginName, pLook->szFilename, Length)) pLook = pLook->pNext;
    return pLook;
}

void* PluginEntry(char* PluginName, char* FuncName) {
    if (PMQPLUGIN pLook = Plugin(PluginName))
        if (void* entry = GetProcAddress(pLook->hModule, FuncName))
            return entry;
    return NULL;
}

int SKCheck(unsigned long id) {
    if (id<100 && (pSkillMgr->pSkill[id]->Activated && GetCharInfo2()->Skill[id]))     return true;
    if (id>100 && id<128 && GetCharInfo2()->Skill[id] != 0xFF && strlen(szSkills[id])>3) return true;
    return false;
}

//VoA skills fix 11/20/2011 - ieatacid
int SKReady(unsigned long id) {
    if (id < 100 || id == 111 || id == 114 || id == 115 || id == 116)
    {
        return pCSkillMgr->IsAvailable(id);
    }
    if (id == 105 || id == 107) return LoH_HT_Ready();
    return false;
}

int SKPress(unsigned long id) {
    if (PCHARINFO pChar = GetCharInfo()) {
        if (pChar->vtable2) {
            pCharData1->UseSkill((unsigned char)id, (EQPlayer*)pCharData1);
            return true;
        }
    }
    return false;
}

long SpawnMask(PSPAWNINFO x) {
    if (!x)                         return st_x;
    if (x->Type == SPAWN_PLAYER)    return st_p;
    if (x->Type == SPAWN_CORPSE)    return x->Deity ? st_cp : st_cn;
    if (x->Type != SPAWN_NPC)       return st_x;
    if (strstr(x->Name, "s_Mount")) return st_x;
    if (!x->MasterID)               return st_n;
    PSPAWNINFO m = GetSpawnID(x->MasterID);
    return (!m || m->Type != SPAWN_PLAYER) ? st_wn : st_wp;
}

int SpellCheck(unsigned long ID) {
    if (ID)
        if (PCHARINFO2 Me = GetCharInfo2())
            for (unsigned long nSlot = 0; nSlot < NUM_BOOK_SLOTS; nSlot++)
                if (ID == Me->SpellBook[nSlot])
                    return true;
    return false;
}

long SpellGemID(unsigned long ID, long SlotID) {
    if (PCHARINFO2 Me = GetCharInfo2()) {
        if (SlotID != NOID && ID == Me->MemorizedSpells[SlotID]) return SlotID;
        for (long GEM = 0; GEM < GemsMax; GEM++)
            if (ID == Me->MemorizedSpells[GEM])
                return GEM;
    }
    return NOID;
}

int SpellReady(unsigned long ID, long SlotID) {
    if (pCastSpellWnd)
        if (PCHARINFO2 Me = GetCharInfo2()) {
            unsigned long GemID = (SlotID != NOID) ? SlotID : SpellGemID(ID);
            if (GemID < (unsigned long)GemsMax)
                if (Me->MemorizedSpells[GemID] == ID)
                    if ((long)((PEQCASTSPELLWINDOW)pCastSpellWnd)->SpellSlots[GemID]->spellicon != NOID)
                        if (BardClass || (long)((PEQCASTSPELLWINDOW)pCastSpellWnd)->SpellSlots[GemID]->spellstate != 1)
                            return true;
        }
    return false;
}

bool HandleMoveUtils(void)
{
    bMUPointers = false;
    fStickCommand = NULL;
    pbStickOn = NULL;
    if (PMQPLUGIN pLook = Plugin("mq2moveutils"))
    {
        fStickCommand = (void(*)(PSPAWNINFO pChar, char* szLine))GetProcAddress(pLook->hModule, "StickCommand");
        pbStickOn = (bool *)GetProcAddress(pLook->hModule, "bStickOn");
    }
    if (fStickCommand && pbStickOn)
    {
        bMUPointers = true;
        return true;
    }
    return false;
}

unsigned char PetButtonEnabled(char* pszButtonName)
{
    int i = 0;
    for (i = 0; i < PET_BUTTONS; i++)
    {
        if (((PEQPETINFOWINDOW)pPetInfoWnd)->pButton[i])
        {
            if (((PEQPETINFOWINDOW)pPetInfoWnd)->pButton[i]->Wnd.WindowText->Text[0])
            {
                if (!_stricmp(((PEQPETINFOWINDOW)pPetInfoWnd)->pButton[i]->Wnd.WindowText->Text, pszButtonName))
                {
                    return ((PEQPETINFOWINDOW)pPetInfoWnd)->pButton[i]->Wnd.Enabled;
                }
            }
        }
    }
    return 0;
}

int Stick(char* command)
{
    char szPasser[MAX_STRING] = { 0 };
    //typedef void (__cdecl *fCALL)(PSPAWNINFO, char*);
    //if (fCALL request = (fCALL)PluginEntry("mq2moveutils", "StickCommand"))
    if (bMULoaded && bMUPointers)
    {
        if (command[0])
        {
            Announce(SHOW_STICKING, "%s::Sticking [\ay%s\ax].", PLUGIN_NAME, command);
            fStickCommand(SpawnMe(), command);
            sprintf_s(szPasser, "/stick %s", command);
            //DebugSpew("MQ2Melee Stick call: %s", command);
        }
        else if (*pbStickOn)
        {
            fStickCommand(SpawnMe(), "off");
            sprintf_s(szPasser, "/stick off");
            //DebugSpew("MQ2Melee Stick call: off");
        }
        Sticking = *pbStickOn;
        //Sticking = (command[0]) ? true : false;
        strcpy_s(StickArg, *pbStickOn ? command : "OFF");
        return true;
    }
    return false;
}

unsigned long TimeSince(unsigned long Timer) {
    if (Timer) return (unsigned long)clock() - Timer;
    return 0;
}

PSTR WinTexte(CXWnd *Wnd, char* ScreenID, PSTR Buffer) {
    Buffer[0] = 0;
    if (Wnd)
        if (CXWnd *Child = (CXWnd*)Wnd->GetChildItem(ScreenID))
            GetCXStr(Child->WindowText, Buffer, MAX_STRING);
    return Buffer;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// item related

bool FitInPack(long Size)
{
    //long pSIZE = 20;
    //long pSLOT = 0;
    unsigned char ucSlot = 0;
    for (ucSlot = BAG_SLOT_START; ucSlot < NUM_INV_SLOTS; ucSlot++)
    {
        if (PCONTENTS pInvSlot = GetCharInfo2()->pInventoryArray->InventoryArray[ucSlot])
        {
            if (TypePack(pInvSlot) && GetItemFromContents(pInvSlot)->Combine != 2 && Size <= GetItemFromContents(pInvSlot)->SizeCapacity)// && (!pSLOT || GetItemFromContents(pInvSlot)->SizeCapacity < pSIZE))
            {
                if (!pInvSlot->Contents.ContainedItems.pItems)
                    return TRUE;
                unsigned char ucPack = 0;
                for (ucPack = 0; ucPack < GetItemFromContents(pInvSlot)->Slots; ucPack++)
                {
                    if (!pInvSlot->Contents.ContainedItems.pItems->Item[ucPack])
                    {
                        //pSIZE = cSlot->Item->SizeCapacity;
                        //break;
                        return TRUE;
                    }
                }
            }
        }
        //else
        //{
        //    return ucSlot;
        //}
    }
    return FALSE;
}

long OkayToEquip(long Size)
{
    if (!CursorEmpty() || IsCasting()) return false;
    if (Size != NOID) return FitInPack(Size);
    return true;
}

long Unequip(long SlotID)
{
    if (SlotID < NUM_INV_SLOTS)
    {
        PCONTENTS uCONT = GetCharInfo2()->pInventoryArray->InventoryArray[SlotID];
        if (!uCONT) return true;

        CItemLocation cUnequipTo;
        if (!PackFind(&cUnequipTo, uCONT)) return false;

        char szCommand[MAX_STRING] = { 0 };
        sprintf_s(szCommand, "/shiftkey /itemnotify %s leftmouseup", szItemSlot[SlotID]);
        EzCommand(szCommand);
        sprintf_s(szCommand, "/ctrlkey /itemnotify %s leftmouseup", szItemSlot[cUnequipTo.InvSlot]);
        EzCommand(szCommand);

        return true;
    }
    return false;
}

int Equip(unsigned long ID, long SlotID)
{
    if (!(SlotID < NUM_INV_SLOTS)) return false;                   // invalid destination slot id for equipping item to
    if (!OkayToEquip())      return false;                         // can't equip item right casting or cursor not free

    char szTempItem[25] = { 0 };
    sprintf_s(szTempItem, "%d", ID);
    CItemLocation cMoveItem;
    CItemLocation cUnequipTo;

    if (!ItemFind(&cMoveItem, szTempItem, BAG_SLOT_START, NUM_INV_SLOTS)) // assume that equipping item is in a backpack first
    {
        if (!ItemFind(&cMoveItem, szTempItem, 0, BAG_SLOT_START)) // wasnt found, check if already equipped somewhere
        {
            return false; // wasnt found, can't equip something we dont have
        }
    }
    if (SlotID == cMoveItem.InvSlot) return true; // item already in place

    // check class, level, deity and race to see if we have rights to equip this items.
    PCONTENTS fITEM = cMoveItem.pBagSlot;
    if (!(GetItemFromContents(fITEM)->Classes&(1 << ((GetCharInfo2()->Class) - 1))))                                     return false;
    if ((unsigned int)GetItemFromContents(fITEM)->RequiredLevel > GetCharInfo2()->Level)                                 return false;
    if (GetItemFromContents(fITEM)->Diety && !(GetItemFromContents(fITEM)->Diety&(1 << (GetCharInfo2()->Deity - 200))))  return false;
    long MyRace = (unsigned long)GetCharInfo2()->Race;
    switch (MyRace)
    {
        case 128: MyRace = 12;    break;
        case 130: MyRace = 13;    break;
        case 330: MyRace = 14;    break;
        case 522: MyRace = 15;    break;
        default:  MyRace--;
    }
    if (!(GetItemFromContents(fITEM)->Races&(1 << MyRace))) return false;
    if (SlotID == inv_primary && TwohandType(fITEM) && ContSecondary())  if (!Unequip(inv_secondary)) return false;
    if (SlotID == inv_secondary && TwohandType(ContPrimary()))           if (!Unequip(inv_primary))   return false;

    // if wearing something
    if (PCONTENTS dCONT = GetCharInfo2()->pInventoryArray->InventoryArray[SlotID])
    {
        if (cMoveItem.InvSlot >= NUM_INV_SLOTS) // if not a main inv slot
        {
            if (!PackFind(&cUnequipTo, dCONT)) return false; // search bags, if bags cant fit it, return false
        }
    }

    CHAR szCommand[MAX_STRING] = { 0 };
    if (cMoveItem.BagSlot != -1) {
        sprintf_s(szCommand, "/shiftkey /itemnotify in %s %d leftmouseup", szItemSlot[cMoveItem.InvSlot], cMoveItem.BagSlot + 1);
        EzCommand(szCommand);
    }
    else {
        sprintf_s(szCommand, "/shiftkey /itemnotify %s leftmouseup", szItemSlot[cMoveItem.InvSlot]);
        EzCommand(szCommand);
    }
    sprintf_s(szCommand, "/shiftkey /itemnotify %s leftmouseup", szItemSlot[SlotID]);
    EzCommand(szCommand);
    EzCommand("/autoinventory");
    return true;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

class Ability {
    CHAR   COMM[16];    // Ability Cast Command
    unsigned long  REUSE;       // Ability Reuse Time
    unsigned long  READY;       // Ability Ready
    long   INDEX;       // Ability Index
    PSPELL EFFECT;      // Ability Spell Effect
    enum { UNKNOWN, ITEM, SKILL, DISC, AA, SPELL, CLICKY, POTION };


public:
    long   ID;          // Ability ID
    long   TYPE;        // Ability Type
    CHAR   NAME[128];   // Ability Name
    int   SEARCH;      // Ability Searched

    int Avail() {
        SEARCH = 1;
        EFFECT = 0;
        if (ID > NOID) {
            if (TYPE == SKILL || TYPE == UNKNOWN) {
                if (SKCheck(ID)) {
                    strcpy_s(NAME, szSkills[ID]);
                    REUSE = (ID == i_forage) ? 101750 : delay * 3;
                    TYPE = SKILL;
                    return true;
                }
            }
            if (TYPE == DISC || TYPE == UNKNOWN) {
                if (CACheck(ID)) {
                    if (PSPELL spell = GetSpellByID(ID)) {
                        strcpy_s(NAME, spell->Name);
                        EFFECT = spell;
                        REUSE = spell->CastTime + spell->RecastTime + delay * 6;
                        TYPE = DISC;
                        return true;
                    }
                }
            }
            if (TYPE == AA || TYPE == UNKNOWN) {
                int level = -1;
                if (PSPAWNINFO pMe = (PSPAWNINFO)pLocalPlayer) {
                    level = pMe->Level;
                }
                if (long AAIndex = AACheck(ID)) {
                    if (PALTABILITY ability = GetAAByIdWrapper(AAIndex, level)) {
                        if (PSPELL spell = GetSpellByID(ability->SpellID)) {
                            strcpy_s(NAME, pCDBStr->GetString(ability->nName, 1, NULL));
                            sprintf_s(COMM, "%d|ALT", ID);
                            EFFECT = spell;
                            REUSE = pAltAdvManager->GetCalculatedTimer(pPCData, ability) * 1000 + spell->CastTime + delay * 3;
                            INDEX = AAIndex;
                            TYPE = AA;
                            return true;
                        }
                    }
                }
            }
            if (TYPE == SPELL || TYPE == UNKNOWN) {
                if (SpellCheck(ID)) {
                    if (PSPELL spell = GetSpellByID(ID)) {
                        strcpy_s(NAME, spell->Name);
                        sprintf_s(COMM, "%d|GEM5", ID);
                        EFFECT = spell;
                        REUSE = spell->CastTime + spell->RecastTime + delay * 3;
                        TYPE = SPELL;
                        return true;
                    }
                }
            }
            if (TYPE == POTION || TYPE == CLICKY || TYPE == ITEM || TYPE == UNKNOWN)
            {
                char szTempItem[25] = { 0 };
                sprintf_s(szTempItem, "%d", ID);
                CItemLocation cFindItem;
                if (ItemFind(&cFindItem, szTempItem))
                {
                    if (PCONTENTS find = cFindItem.pBagSlot)
                    {
                        INDEX = cFindItem.InvSlot;
                        strcpy_s(NAME, GetItemFromContents(find)->Name);
                        TYPE = ITEM;
                        if (PSPELL spell = GetSpellByID(GetItemFromContents(find)->Clicky.SpellID))
                        {
                            sprintf_s(COMM, "%d|ITEM", ID);
                            EFFECT = spell;
                            REUSE = GetItemFromContents(find)->Clicky.TimerID * 1000 + GetItemFromContents(find)->Clicky.CastTime + delay * 3;
                            TYPE = (GetItemFromContents(find)->ItemType == 21) ? POTION : CLICKY;
                        }
                        return true;
                    }
                }
            }
        }
        return false;
    }

    bool Found() {
        if (!SEARCH) Avail();
        return (ID > 0 && TYPE != UNKNOWN) ? true : false;
    }

    long Check(string test) {
        if (!Found())                        return 0x01;  // Ability Not Found
        if ((unsigned long)clock() <= READY) return 0x02;  // Ability Not Refreshed
        if (TYPE == SKILL)
        {
            if (SpawnMount())
            {
                switch (ID)
                {
                case i_disarm:        break;
                case i_forage:        break;
                case i_intimidation:  break;
                case i_kick:          break;
                case i_mend:          break;
                case i_taunt:         break;
                default:              return 0x03;  // Ability Do Not Work on Mount
                }
            }
            if (!SKReady(ID))         return 0x13;  // Ability Not Ready
        }
        else if (EFFECT)
        {
            if (!IsStanding())        return 0x05;  // Not Standing
            if (IsStunned())          return 0x06;  // Stunned
            if (TYPE > DISC)
            {
                if (IsInvisible())    return 0x04;  // Will Break Invisiblity
                if (Silenced)         return 0x07;  // Silenced
            }
            if (!BardClass && IsCasting())         return 0x08;  // already casting
            if (WinState((CXWnd*)pSpellBookWnd)) return 0x09;  // spellbook open
            if ((EFFECT->ReuseTimerIndex || EFFECT->ReuseTimerIndex == -1) && TYPE != AA)
            {
                //DebugSpew("EFFECT->ReuseTimerIndex Name: %s  ID: %d Type: %dVal: %d", NAME, ID, TYPE, EFFECT->ReuseTimerIndex);
                #ifndef EMU
                if (((unsigned long)pPCData->GetCombatAbilityTimer(EFFECT->ReuseTimerIndex, EFFECT->SpellGroup) - (unsigned long)time(NULL)) < 0) return 0x16; // dicipline timer not ready
                #else
                if (((unsigned long)pPCData->GetCombatAbilityTimer(EFFECT->ReuseTimerIndex) - (unsigned long)time(NULL)) < 0) return 0x16; // dicipline timer not ready
                #endif
            }
            if ((long)EFFECT->ReagentID[0]>0 && (long)CountItemByID(EFFECT->ReagentID[0]) < (long)EFFECT->ReagentCount[0])        return 0x0A;  // out of reagent
            if (EFFECT->EnduranceCost && GetCharInfo2()->Endurance < EFFECT->EnduranceCost)                                       return 0x0B;  // out of endurance
            if (EFFECT->ManaCost && GetCharInfo2()->Mana < EFFECT->ManaCost)                                                      return 0x0C;  // out of mana
            if (!EFFECT->SpellType)
            {
                if (!pTarget)                                                                                                     return 0x0D;  // no target
                float SpellRange = (EFFECT->Range) ? EFFECT->Range : EFFECT->AERange;
                if (SpellRange && !InRange(SpawnMe(), (PSPAWNINFO)pTarget, SpellRange))                                           return 0x0E;  // out of range
            }
            else if (EFFECT->DurationCap>0)
            {
                if (EFFECT->DurationWindow)
                {
                    for (int s = 0; s < SongMax; s++)
                    {
                        if (PSPELL buff = GetSpellByID(GetCharInfo2()->ShortBuff[s].SpellID))
                        {
                            if (EFFECT->ID == buff->ID)        return 0x0F; // already have
                            if (!BuffStackTest(EFFECT, buff)) return 0x10; // not stacking
                        }
                    }
                }
                else
                {
                    for (int b = 0; b<BuffMax; b++)
                    {
                        if (PSPELL buff = GetSpellByID(GetCharInfo2()->Buff[b].SpellID))
                        {
                            if (EFFECT->ID == buff->ID)        return 0x0F; // already have
                            if (!BuffStackTest(EFFECT, buff)) return 0x10; // not stacking
                        }
                    }
                }
            }
            if (TYPE>DISC && !Evaluate("${If[${Cast.Ready[%s]},1,0]}", COMM)) return 0x11; // mq2cast not ready
            if (TYPE == AA) {
                if (!AAReady(INDEX))       return 0x13;  // Ability Not Ready
            }
            else if (TYPE == SPELL) {
                if (!pCastSpellWnd)        return 0x12;  // No Casting Spell Bar
                INDEX = SpellGemID(ID, INDEX);
                if (!SpellReady(ID, INDEX)) return 0x13;  // Ability Not Ready
            }
            else if (TYPE == POTION || TYPE == CLICKY)
            {
                //PCONTENTS find=ItemLocate(ID,0,NUM_INV_SLOTS,INDEX);
                char szTempItem[25] = { 0 };
                sprintf_s(szTempItem, "%d", ID);
                CItemLocation cFindItem;
                if (ItemFind(&cFindItem, szTempItem))
                {
                    PCONTENTS find = cFindItem.pBagSlot;
                    INDEX = cFindItem.InvSlot;
                    //INDEX=InvSlot;
                    if (!find || !find->Charges || ItemTimer(find)) return 0x13;  // Ability Not Ready
                    if (!CursorEmpty())        return 0x14;  // Cursor Not Empty
                }
            }
        }
        if (!test.empty() && !Evaluate((char*)test.c_str())) return 0x15; // User Condition Abort
        return 0x00;
    }

    int Ready(string test) {
        long Result = Check(test);
        if (DebugReady && Result) {
            char* Message = "";
            switch (Result) {
            case 0x01:  Message = "NOT FOUND";                break;
            case 0x02:  Message = "NOT REFRESHED";            break;
            case 0x03:  Message = "NOT WORKING ON MOUNT";     break;
            case 0x04:  Message = "WILL BREAK INVISIBILITY";  break;
            case 0x05:  Message = "NOT STANDING";             break;
            case 0x06:  Message = "STUNNED";                  break;
            case 0x07:  Message = "SILENCED";                 break;
            case 0x08:  Message = "ALREADY CASTING";          break;
            case 0x09:  Message = "SPELLBOOK OPEN";           break;
            case 0x0A:  Message = "OUT OF REAGENT";           break;
            case 0x0B:  Message = "OUT OF ENDURANCE";         break;
            case 0x0C:  Message = "OUT OF MANA";              break;
            case 0x0D:  Message = "NO TARGET";                break;
            case 0x0E:  Message = "OUT OF RANGE";             break;
            case 0x0F:  Message = "ALREADY BUFFED WITH THIS"; break;
            case 0x10:  Message = "BUFF NOT STACKING";        break;
            case 0x11:  Message = "MQ2CAST NOT READY/FOUND";  break;
            case 0x12:  Message = "NO SPELL BAR";             break;
            case 0x13:  Message = "ABILITY NOT READY";        break;
            case 0x14:  Message = "CURSOR NOT EMPTY";         break;
            case 0x15:  Message = "USER CONDITION ABORT";     break;
            case 0x16:  Message = "TIMER NOT READY";          break;
            }
            if (ID && TYPE) Announce(DebugReady, "Ability[%d][%d][%s] <<%s>>.", ID, TYPE, NAME, Message);
        }
        return (!Result);
    }

    int Press() {
        int Casted = false;
        bool bFound = Found();
        if (bFound && (unsigned long)clock() > READY) {
            Announce(SHOW_ABILITY, "%s::Activate [\ay%s\ax].", PLUGIN_NAME, NAME);
            if (TYPE == SKILL)     Casted = SKPress(ID);
            else if (TYPE == DISC) Casted = CAPress(ID);
            else if (TYPE >= AA)   Casted = Casting(COMM);
            if (Casted) READY = (unsigned long)clock() + REUSE;
        }
        else
        {
            Announce(SHOW_ABILITY, "%s::Ability Not %s [\ay%s\ax].", PLUGIN_NAME, bFound ? "Ready" : "Found", NAME);
        }
        return Casted;
    }

    void Setup(long id, long type) {
        ID = id;            // Ability ID?
        TYPE = type;        // Ability Type?
        NAME[0] = 0;        // Ability Name?
        COMM[0] = 0;        // Ability Command?
        SEARCH = false;     // Ability Searched?
        READY = 0;          // Ability Ready Time
        INDEX = NOID;       // Ability Index
        EFFECT = NULL;      // Ability Spell Effect
    }

    Ability(long id, long type) {
        Setup(id, type);
    }

    Ability() {
        Setup(0, UNKNOWN);
    }
};

class Option {
public:
    char*    K;  // key?
    char*    H;  // help?
    char*    D;  // default?
    char*    S;  // show?
    string  *C;  // condition?
    Ability *A;  // ability?
    long    *V;  // value?
    Function F;  // function?
    int      U;  // update?

    Option(char* k, char* h, char* d, char* s, Function f, string *c) {
        K = k; H = h; D = d; S = s; F = f; U = false; C = c; A = NULL; V = NULL;
    }

    Option(char* k, char* h, char* d, char* s, Function f, Ability *a) {
        K = k; H = h; D = d; S = s; F = f; U = false; C = NULL; A = a; V = NULL;
    }

    Option(char* k, char* h, char* d, char* s, Function f, long *v) {
        K = k; H = h; D = d; S = s; F = f; U = false; C = NULL; A = NULL; V = v;
    }

    void Write() {
        if (K[0] && !C && Evaluate(S)) {
            long value = (A) ? A->ID : (V) ? *V : 0;
            if (value > 0) WriteChatf("%s::%s (\ag%d\ax) \ay%s\ax.", PLUGIN_NAME, K, value, H);
            else           WriteChatf("%s::%s (\ar0\ax) \ay%s\ax.", PLUGIN_NAME, K, H);
        }
    }

    void Setup(char* value) {
        if (C) *C = value;
        else if (A) A->Setup(atol(value), 0);
        else if (V) {
            if (!_stricmp("false", value) || !_stricmp("off", value))    *V = 0;
            else if (!_stricmp("true", value) || !_stricmp("on", value)) *V = 1;
            else *V = atol(value);
        }
        if (F) this->F();
    }

    void *Value() {
        if (A) return &A->ID;
        else if (V) return V;
        else if (C) return C;
        return NULL;
    }

    long Ready() {
        if (A) return A->Ready("");
        return NOID;
    }

    void Reset() {
        if (K[0]) {
            if (C) *C = D;
            else {
                strcpy_s(Reserved, D);
                if (Reserved[0])
                    ParseMacroData(Reserved, sizeof(Reserved));
                CHAR szTemp[2048] = { 0 };
                strcpy_s(szTemp, Reserved);
                long value = 0;
                try {
                    value = atol(szTemp);
                }
                catch (...) {
                    MessageBox(NULL, "Exception in MQ2Melee::Reset", "Debug", MB_OK);
                    //throw();
                }
                if (A)
                    A->Setup(value, 0);
                else if (V)
                    *V = value;

            }
            if (F)
                this->F();
        }
    }
};

typedef map<string, Option> Liste;    // declare a type so more easy to refer
Liste     CmdListe;                   // settings from command or ini
Liste     IniListe;                   // settings from ini only
Liste     VarListe;                   // settings from var liste

CHAR      section[256];               // ini section

long      doAGGRO,
doASP,
doASSASSINATE,
doASSAULT,
doBACKOFF,
doBACKSTAB,
doBANESTRIKE,
doBASH,
doBATTLELEAP,
doBEGGING,
doBLEED,
doBLOODLUST,
doBOASTFUL,
doBVIVI,
doCALLCHALLENGE,
doCLOUD,
doCHALLENGEFOR,
doCOMMANDING,
doCRIPPLE,
doCRYHAVOC,
doCSTRIKE,
doDEFENSE,
doDISARM,
doDOWNFLAG[61],
doDRAGONPUNCH,
doEAGLESTRIKE,
doENRAGE,
doESCAPE,
doENRAGINGKICK,
doEVADE,
doEYEGOUGE,
doFACING,
doFALLS,
doFEIGNDEATH,
doFERALSWIPE,
//doFEROCIOUSKICK,
doFIELDARM,
doFISTSOFWU,
doFCLAW,
doFLYINGKICK,
doFORAGE,
doFRENZY,
doGBLADE,
doGORILLASMASH,
doGUTPUNCH,
doHARMTOUCH,
doHIDE,
doHOLYFLAG[61],
doINFURIATE,
doINTIMIDATION,
doJLTKICKS,
doJUGULAR,
doJOLT,
doKICK,
doKNEESTRIKE,
doKNIFEPLAY,
doLAYHAND,
doLEOPARDCLAW,
doMELEE,
doMEND,
doMONKEY,
doOPFRENZY,
doOPPORTUNISTICSTRIKE,
doPETASSIST,
doPETDELAY,
doPETMEND,
doPETRANGE,
doPETENGAGEHPS,
doPICKPOCKET,
doPINPOINT,
doPOTHEALFAST,
doPOTHEALOVER,
doPROVOKEEND,
doPROVOKEMAX,
doPROVOKEONCE,
doRAGEVOLLEY,
doRAKE,
doRALLOS,
doRANGE,
doRAVENS,
doRESUME,
doRIGHTIND,
doROUNDKICK,
doSELOS,
doSENSETRAP,
doSKILL,
doSLAM,
doSLAPFACE,
doSNEAK,
doSTAB,
doSTAND,
doSTEELY,
doSTICKBREAK,
doSTICKDELAY,
doSTICKMODE,
doSTICKNORANGE,
doSTICKRANGE,
doSTORMBLADES,
doSTRIKE,
doSTRIKEMODE,
doSTUNNING,
doSYNERGY,
doTAUNT,
doTHIEFEYE,
doTHROATJAB,
doTHROWSTONE,
doTIGERCLAW,
doTWISTEDSHANK,
doVIGAXE,
doVIGDAGGER,
doVIGSHURIKEN,
doWITHSTAND,
doYAULP;

long      elARROWS,
elAGGROPRI,
elAGGROSEC,
elMELEEPRI,
elMELEESEC,
elPOKER,
elRANGED,
elSHIELD;

string    ifASP,
ifASSAULT,
ifBACKSTAB,
ifBANESTRIKE,
ifBASH,
ifBATTLELEAP,
ifBEGGING,
ifBOASTFUL,
ifBLEED,
ifBLOODLUST,
ifBVIVI,
ifCALLCHALLENGE,
ifCLOUD,
ifCHALLENGEFOR,
ifCOMMANDING,
ifCRIPPLE,
ifCRYHAVOC,
ifCSTRIKE,
ifDEFENSE,
ifDISARM,
ifDRAGONPUNCH,
ifEAGLESTRIKE,
ifEVADE,
ifENRAGINGKICK,
ifEYEGOUGE,
ifFALLS,
ifFERALSWIPE,
//ifFEROCIOUSKICK,
ifFIELDARM,
ifFISTSOFWU,
ifFCLAW,
ifFLYINGKICK,
ifFORAGE,
ifFRENZY,
ifGBLADE,
ifGORILLASMASH,
ifGUTPUNCH,
ifHARMTOUCH,
ifHIDE,
ifINTIMIDATION,
ifJLTKICKS,
ifJOLT,
ifJUGULAR,
ifKICK,
ifKNEESTRIKE,
ifKNIFEPLAY,
ifLAYHAND,
ifLEOPARDCLAW,
ifMEND,
ifMONKEY,
ifOPFRENZY,
ifOPPORTUNISTICSTRIKE,
ifPICKPOCKET,
ifPINPOINT,
ifPOTHEALFAST,
ifPOTHEALOVER,
ifPROVOKE,
ifRAGEVOLLEY,
ifRAKE,
ifRALLOS,
ifRAVENS,
ifRIGHTIND,
ifROUNDKICK,
ifSELOS,
ifSENSETRAP,
ifSLAM,
ifSLAPFACE,
ifSNEAK,
ifSTEELY,
ifSTORMBLADES,
ifSTRIKE,
ifSTUNNING,
ifSYNERGY,
ifTAUNT,
ifTHIEFEYE,
ifTHROATJAB,
ifTHROWSTONE,
ifTIGERCLAW,
ifTWISTEDSHANK,
ifVIGAXE,
ifVIGDAGGER,
ifVIGSHURIKEN,
ifWITHSTAND,
ifYAULP,
DOWNSHIT[61],
HOLYSHIT[61],
StickCMD,
StrikeCMD,
HOLYSHITIF,
DOWNSHITIF;

Ability  idASP,
idASSAULT,
idBACKSTAB,
idBANESTRIKE,
idBASH,
idBATTLELEAP,
idBEGGING,
idBLEED,
idBLOODLUST,
idBOASTFUL,
idBVIVI,
idCALLCHALLENGE,
idCLOUD,
idCSTRIKE,
idCHALLENGEFOR,
idCOMMANDING,
idCRIPPLE,
idCRYHAVOC,
idDEFENSE,
idDISARM,
idDRAGONPUNCH,
idEAGLESTRIKE,
idESCAPE,
idENRAGINGKICK,
idEYEGOUGE,
idFEIGN[2],
idFERALSWIPE,
//idFEROCIOUSKICK,
idFIELDARM,
idFISTSOFWU,
idFCLAW,
idFLYINGKICK,
idFORAGE,
idFRENZY,
idGBLADE,
idGORILLASMASH,
idGUTPUNCH,
idHARMTOUCH,
idHIDE,
idINTIMIDATION,
idJLTKICKS,
idJOLT,
idJUGULAR,
idKICK,
idKNEESTRIKE,
idKNIFEPLAY,
idLAYHAND,
idLEOPARDCLAW,
idMEND,
idMONKEY,
idOPFRENZY,
idOPPORTUNISTICSTRIKE,
idPETMEND,
idPETASSIST,
idPETDELAY,
idPETENGAGEHPS,
idPETRANGE,
idPICKPOCKET,
idPINPOINT,
idPOTHEALFAST,
idPOTHEALOVER,
idPROVOKE[2],
idRAGEVOLLEY,
idRAKE,
idRALLOS,
idRAVENS,
idRIGHTIND,
idROUNDKICK,
idSELOS,
idSENSETRAP,
idSLAM,
idSLAPFACE,
idSNEAK,
idSTEELY,
idSTORMBLADES,
idSTRIKE,
idSTUN[2],
idSYNERGY,
idTAUNT,
idTHIEFEYE,
idTHROATJAB,
idTHROWSTONE,
idTIGERCLAW,
idTWISTEDSHANK,
idVIGAXE,
idVIGDAGGER,
idVIGSHURIKEN,
idWITHSTAND,
idYAULP;

unsigned long     Shrouded    = false;        // True when shrouded.
bool      Binded              = false;        // Attack Key is Binded?
bool      Loaded              = false;        // Loaded?
bool      Moving              = false;        // Moving?
bool      Immobile            = false;        // Immobilized?
bool      AutoFire            = false;        // True when autofire is on.
bool      HaveBash            = false;        // Have Two Hand Bash?
bool      HaveHold            = false;        // Have Pet Hold?
bool      HaveGHold           = false;        // Have Pet GHold?
unsigned long     BrokenFD    = 0;            // Timer for Broken Feign Death

float     Travel              = 0.0f;         // Travel Speed?
long      Health              = 0;            // Current Health

unsigned long     MeleeTime   = 0;            // Melee Pulse Timer
long      MeleeTarg           = 0;            // Melee Target ID
long      MeleeType           = 0;            // Melee Target Type
long      MeleeFlee           = 0;            // Melee Target Fleeing?
__int64      MeleeLife           = 0;            // Melee Target Life %
long      MeleeCast           = 0;            // Melee Target Cast ?
long      MeleeSize           = 0;            // Melee Name Size
char      MeleeName[64]       = { 0 };        // Melee Name

double    MeleeSpeed          = 0.0f;         // Melee Target Speed
double    MeleeBack           = 0.0f;         // Melee Target Angle Back
double    MeleeView           = 0.0f;         // Melee Target Angle View
double    MeleeDist           = 0.0f;         // Melee Distance to Target
double    MeleeKill           = 0.0f;         // Melee Distance to Use Ability

long      onEVENT             = false;        // Ranged=0x8000,Begging=0x2000,PickPocket=0x1000,Feign=0x0040,Hide=0x0020,Backoff=0x0010,Infuriate=0x0002,Enrage=0x0001
long      onSTICK             = false;        // Do Stick? (turn false when stick command is issue)
long      onBELOW             = false;        // Below Flag? (turn false when no more provoke counter)
long      onCHALLENGEFOR      = false;        // Challenge Flag? (turn to false when use once)
long      onENRAGINGKICK      = false;        // Challenge Flag? (turn to false when use once)

unsigned long     NPC_TYPE    = 0x000A;       // NPC TYPE
char      MeleeKey[32];                       // Plugin Melee Key
char      RangeKey[32];                       // Plugin Range Key
unsigned long     SETBINDINGS = 1;            // Set Bindings?

unsigned long     PetInDist   = 0;            // Pet Target in Range
unsigned long     PetOnAttk   = 0;            // Pet Seen Attacking TimeStamp?
unsigned long     PetOnHold   = 0;            // Pet Hold?
unsigned long     PetOnWait   = 0;            // Pet Wait Assist Delay TimeStamp
unsigned long     PetTarget   = 0;            // Pet Target ID

unsigned long     TimerAttk   = 0;            // Timer Attk
unsigned long     TimerBack   = 0;            // Timer BackOff/Escape/Feign
unsigned long     TimerMove   = 0;            // Timer Move
unsigned long     TimerLife   = 0;            // Timer Life (Target his dieing)
unsigned long     TimerFace   = 0;            // Face Time Stamp when started
unsigned long     TimerStik   = 0;            // Stik Time Stamp when started
unsigned long     TimerStun   = 0;            // Timer Stun

long      SwingHits           = 0;            // Total Hits
long      TakenHits           = 0;            // Under Hits

long      doHOLY              = 0;            // Holy Shits while meleeing?
long      doDOWN              = 0;            // Down Shits while downtime?

Blech    *pMeleeEvent         = 0;            // blech event list
bool      IdlingArray[256];                   // animation array while idle
bool      AttackArray[256];                   // animation array while attacking

long      SaveList[50];                       // saved event list
long      SaveIndx;                           // saved event counters

unsigned long     HiddenTimer = 0;            // Last TimeStamp for Hide
unsigned long     SilentTimer = 0;            // Last TimeStamp for Sneak

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

class MQ2MeleeType *pMeleeTypes = 0;
class MQ2MeleeType : public MQ2Type {
private:
    long isKill;
    char Tempos[MAX_STRING];
public:
    enum Information {
        Enable = 1,
        Combat = 2,
        Casted = 3,
        Engage = 4,
        Status = 5,
        Target = 6,
        DiscID = 7,
        Enrage = 8,
        Infuriate = 9,
        AggroMode = 10,
        MeleeMode = 11,
        RangeMode = 12,
        BackAngle = 13,
        ViewAngle = 14,
        Immobilize = 15,
        Ammunition = 16,
        BackStabbing = 17,
        GotAggro = 18,
        Hidden = 19,
        Silent = 20,
        NumHits = 21,
        XTaggro = 22,
    };
    MQ2MeleeType() :MQ2Type("Melee") {
        TypeMember(Enable);
        TypeMember(Combat);
        TypeMember(Casted);
        TypeMember(Status);
        TypeMember(Target);
        TypeMember(DiscID);
        TypeMember(GotAggro);
        TypeMember(AggroMode);
        TypeMember(MeleeMode);
        TypeMember(RangeMode);
        TypeMember(Enrage);
        TypeMember(Infuriate);
        TypeMember(BackAngle);
        TypeMember(ViewAngle);
        TypeMember(Immobilize);
        TypeMember(Ammunition);
        TypeMember(BackStabbing);
        TypeMember(Hidden);
        TypeMember(Silent);
        TypeMember(NumHits);
        TypeMember(XTaggro);
    }
    bool MQ2MeleeType::GETMEMBER() {
        PMQ2TYPEMEMBER pMember = MQ2MeleeType::FindMember(Member);
        isKill = false; if (doSKILL) if (MeleeTarg) isKill = true;
        if (pMember)
        {
            switch ((Information)pMember->ID)
            {
                case Enable:
                    Dest.DWord = doSKILL;
                    Dest.Type = pBoolType;
                    return true;
                case Combat:
                    Dest.DWord = isKill;
                    Dest.Type = pBoolType;
                    return true;
                case Casted:
                    Dest.Int = (isKill && MeleeCast) ? labs((unsigned long)clock() - MeleeCast) : 60000;
                    Dest.Type = pIntType;
                    return true;
                case Status:
                    Tempos[0] = 0;
                    if (isKill) strcat_s(Tempos, "ENGAGED ");
                    else strcat_s(Tempos, "WAITING ");
                    if (*EQADDR_ATTACK) strcat_s(Tempos, "MELEE ");
                    else if (onEVENT & 0x8000) strcat_s(Tempos, "RANGE ");
                    if (onEVENT & 0x0001) strcat_s(Tempos, "ENRAGE ");
                    if (onEVENT & 0x0002) strcat_s(Tempos, "INFURIATE ");
                    if (onEVENT & 0x0010) strcat_s(Tempos, "BACKING ");
                    if (onEVENT & 0x0020) strcat_s(Tempos, "ESCAPING ");
                    if (onEVENT & 0x0040) strcat_s(Tempos, "FEIGNING ");
                    if (onEVENT & 0x0200) strcat_s(Tempos, "EVADING ");
                    if (onEVENT & 0x0400) strcat_s(Tempos, "FALLING ");
                    if (onEVENT & 0x1000) strcat_s(Tempos, "STEALING ");
                    if (onEVENT & 0x2000) strcat_s(Tempos, "BEGGING ");
                    Dest.Type = pStringType;
                    Dest.Ptr = &Tempos[0];
                    return true;
                case Target:
                    Dest.Int = isKill ? MeleeTarg : 0;
                    Dest.Type = pIntType;
                    return true;
                case DiscID:
                    Dest.DWord = Discipline();
                    Dest.Type = pIntType;
                    return true;
                case GotAggro:
                    Dest.DWord = (Aggroed(MeleeTarg) > 0);
                    Dest.Type = pBoolType;
                    return true;
                case AggroMode:
                    Dest.DWord = doAGGRO;
                    Dest.Type = pBoolType;
                    return true;
                case MeleeMode:
                    Dest.DWord = doMELEE;
                    Dest.Type = pBoolType;
                    return true;
                case RangeMode:
                    Dest.DWord = doRANGE;
                    Dest.Type = pBoolType;
                    return true;
                case Enrage:
                    Dest.DWord = onEVENT & 0x0001;
                    Dest.Type = pBoolType;
                    return true;
                case Infuriate:
                    Dest.DWord = onEVENT & 0x0002;
                    Dest.Type = pBoolType;
                    return true;
                case BackAngle:
                    Dest.Float = pTarget ? AngularDistance(((PSPAWNINFO)pTarget)->Heading, SpawnMe()->Heading) : 0.0f;
                    Dest.Type = pFloatType;
                    return true;
                case ViewAngle:
                    Dest.Float = pTarget ? (float)AngularHeading(SpawnMe(), (PSPAWNINFO)pTarget) : 0.0f;
                    Dest.Type = pFloatType;
                    return true;
                case Immobilize:
                    Dest.DWord = Immobile;
                    Dest.Type = pBoolType;
                    return true;
                case Ammunition:
                    Dest.DWord = CountItemByID(elARROWS);
                    if (PCONTENTS r = GetCharInfo2()->pInventoryArray->Inventory.Ammo)
                        if (GetItemFromContents(r)->ItemNumber != elARROWS)
                            if (GetItemFromContents(r)->ItemType == 7 || GetItemFromContents(r)->ItemType == 19 || GetItemFromContents(r)->ItemType == 27)
                                Dest.DWord = CountItemByID(GetItemFromContents(r)->ItemNumber);
                    Dest.Type = pIntType;
                    return true;
                case BackStabbing:
                    Dest.DWord = doBACKSTAB;
                    Dest.Type = pBoolType;
                    return true;
                case Hidden:
                    Dest.Int = TimeSince(HiddenTimer);
                    Dest.Type = pIntType;
                    return true;
                case Silent:
                    Dest.Int = TimeSince(SilentTimer);
                    Dest.Type = pIntType;
                    return true;
                case NumHits:
                    Dest.DWord = SwingHits;
                    Dest.Type = pIntType;
                    return true;
                case XTaggro:
                {
                    Dest.DWord = true;
                    Dest.Type = pBoolType;
                    if (PCHARINFO pChar = GetCharInfo()) {
                        if (ExtendedTargetList *xtm = pChar->pXTargetMgr) {
                            DWORD x = 0;
                            for (int n = 0; n < xtm->XTargetSlots.Count; n++)
                            {
                                XTARGETSLOT xts = xtm->XTargetSlots[n];
                                if (xts.xTargetType == XTARGET_AUTO_HATER && xts.XTargetSlotStatus)
                                {
                                    x++;
                                }
                            }
                            if (x > 1) {
                                if (pAggroInfo) {
                                    for (int i = 0; i < xtm->XTargetSlots.Count; i++) {
                                        XTARGETSLOT xts = xtm->XTargetSlots[i];
                                        if (DWORD spID = xts.SpawnID) {
                                            if (PSPAWNINFO pSpawn = (PSPAWNINFO)GetSpawnByID(spID)) {
                                                if (pTarget && ((PSPAWNINFO)pTarget)->SpawnID == pSpawn->SpawnID)
                                                    continue;
                                                if (pSpawn->Type == SPAWN_NPC && xts.xTargetType == XTARGET_AUTO_HATER) {
                                                    DWORD agropct = pAggroInfo->aggroData[AD_xTarget1 + i].AggroPct;
                                                    //WriteChatf("Checking aggro on %s its %d",xta->pXTargetData[i].Name,agropct);
                                                    if (agropct < 100) {
                                                        Dest.DWord = false;
                                                        break;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                return true;
            }
        }
        strcpy_s(Tempos, "NULL");
        Dest.Type = pStringType;
        Dest.Ptr = &Tempos[0];
        return true;
    }
    bool ToString(MQ2VARPTR VarPtr, char* Destination) {
        strcpy_s(Destination, MAX_STRING, "TRUE");
        return true;
    }
    bool FromData(MQ2VARPTR &VarPtr, MQ2TYPEVAR &Source) {
        return false;
    }
    bool FromString(MQ2VARPTR &VarPtr, char* Source) {
        return false;
    }
    ~MQ2MeleeType() { }
};

int DataMelee(char* Index, MQ2TYPEVAR &Dest) {
    Dest.Type = pMeleeTypes;
    Dest.DWord = 1;
    return true;
}

int datameleemvb(char* Index, MQ2TYPEVAR &Dest) {
    Dest.Type = pIntType;
    Dest.Int = NOID;
    Liste::iterator c;
    if (VarListe.end() != (c = VarListe.find(Index)))
        Dest.Int = (*c).second.Ready();
    return true;
}

int datameleemvi(char* Index, MQ2TYPEVAR &Dest) {
    Dest.Type = pIntType;
    Dest.DWord = 0;
    Liste::iterator c;
    if (CmdListe.end() != (c = CmdListe.find(Index))) {
        if (long *V = (long*)(*c).second.Value()) Dest.DWord = *V;
        return true;
    }
    if (VarListe.end() != (c = VarListe.find(Index))) {
        if (long *V = (long*)(*c).second.Value()) Dest.DWord = *V;
        return true;
    }
    return true;
}

int datameleemvs(char* Index, MQ2TYPEVAR &Dest) {
    Dest.Type = pStringType;
    Dest.Ptr = &Workings;
    Liste::iterator c = IniListe.find(Index);
    if (IniListe.end() != c) {
        if (string *S = (string*)(*c).second.Value())
            strcpy_s(Workings, S->c_str());
    }
    else Workings[0] = 0;
    return true;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

void AbilityFind(Ability *thisone, infodata *first, ...) {
    infodata *c = first;
    va_list  marker;
    va_start(marker, first);
    while (c) {
        thisone->Setup(c->i, c->t);
        if (thisone->Avail()) break;
        thisone->ID = 0;
        c = va_arg(marker, infodata *);
    }
    va_end(marker);
}

void AttackON() {
    if (*EQADDR_ATTACK || onEVENT & 0xFFF7 || IsFeigning() || !doMELEE || !TargetID(MeleeTarg)) return;
    EzCommand("/attack on");
    TimerAttk = (unsigned long)clock();
}

void AttackOFF() {
    if (*EQADDR_ATTACK) EzCommand("/attack off");
}

bool BashCheck() {
    if (ShieldType(ContSecondary())) return true;
    if (TwohandType(ContPrimary()))  return HaveBash;
    return (elSHIELD && CountItemByID(elSHIELD) && OkayToEquip(Giant));
}

void BashPress() {
    long savedpri = 0; long savedoff = 0; int got2hand = false;
    if (PCONTENTS pri = ContPrimary()) {
        got2hand = TwohandType(pri);
        savedpri = GetItemFromContents(pri)->ItemNumber;
    }
    if (PCONTENTS off = ContSecondary()) savedoff = GetItemFromContents(off)->ItemNumber;
    if (elSHIELD && CountItemByID(elSHIELD) && OkayToEquip(Giant)) Equip(elSHIELD, inv_secondary);
    if (ShieldType(ContSecondary()) || (got2hand && HaveBash)) idBASH.Press();
    if (savedoff) Equip(savedoff, inv_secondary);
    if (savedpri) Equip(savedpri, inv_primary);
}

void Configure() {
    PCHARINFO2 pChar2 = GetCharInfo2();
    PCHARINFO pChar = GetCharInfo();
    if (!pChar2 || !pChar)
        return;
    long Class = pChar2->Class;
    long Races = pChar2->Race;
    long Level = pChar2->Level;
    int SOValue = 0;
    sprintf_s(INIFileName, "%s\\%s_%s.ini", gszINIPath, EQADDR_SERVERNAME, pChar->Name);
    sprintf_s(section, "%s_%d_%s_%s", PLUGIN_NAME, Level, pEverQuest->GetRaceDesc(Races), pEverQuest->GetClassDesc(Class));
    Shrouded = pChar2->Shrouded; if (!Shrouded) section[strlen(PLUGIN_NAME)] = 0;
    BuffMax = GetCharMaxBuffSlots();
    HaveHold = GetAAIndexByName("Pet Discipline") ? true : false;
    if (HaveHold) {
        if (int pdindex = GetAAIndexByName("Pet Discipline")) {
            if (int pointsspentinpd = AAPoint(pdindex)) {
                if (pointsspentinpd >= 6) {
                    HaveGHold = true;
                }
            }
        }
    }
    HaveBash = GetAAIndexByName("2 Hand Bash") ? true : false;
    BardClass = false;
    BerserkerClass = false;
    MonkClass = false;
    RogueClass = false;
    char keys[MAX_STRING * 5];
    char temp[MAX_STRING];
    Liste::iterator c, i;
    Liste::iterator ec = CmdListe.end();
    Liste::iterator ei = IniListe.end();
    for (c = CmdListe.begin(); c != ec; c++) (*c).second.Reset();
    for (i = IniListe.begin(); i != ei; i++) (*i).second.Reset();

    idASP.Setup(0, 0);
    idASSAULT.Setup(0, 0);
    idBACKSTAB.Setup(0, 0);
    idBANESTRIKE.Setup(0, 0);
    idBASH.Setup(0, 0);
    idBATTLELEAP.Setup(0, 0);
    idBEGGING.Setup(0, 0);
    idBLEED.Setup(0, 0);
    idBLOODLUST.Setup(0, 0);
    idBOASTFUL.Setup(0, 0);
    idBVIVI.Setup(0, 0);
    idCALLCHALLENGE.Setup(0, 0);
    idCLOUD.Setup(0, 0);
    idCHALLENGEFOR.Setup(0, 0);
    idCOMMANDING.Setup(0, 0);
    idCRIPPLE.Setup(0, 0);
    idCRYHAVOC.Setup(0, 0);
    idCSTRIKE.Setup(0, 0);
    idDEFENSE.Setup(0, 0);
    idDISARM.Setup(0, 0);
    idDRAGONPUNCH.Setup(0, 0);
    idEAGLESTRIKE.Setup(0, 0);
    idESCAPE.Setup(0, 0);
    idENRAGINGKICK.Setup(0, 0);
    idEYEGOUGE.Setup(0, 0);
    idFCLAW.Setup(0, 0);
    idFEIGN[0].Setup(0, 0);
    idFEIGN[1].Setup(0, 0);
    idFIELDARM.Setup(0, 0);
    idFISTSOFWU.Setup(0, 0);
    //idFEROCIOUSKICK.Setup(0, 0);
    idFLYINGKICK.Setup(0, 0);
    idFORAGE.Setup(0, 0);
    idFRENZY.Setup(0, 0);
    idGBLADE.Setup(0, 0);
    idGORILLASMASH.Setup(0, 0);
    idGUTPUNCH.Setup(0, 0);
    idHARMTOUCH.Setup(0, 0);
    idHIDE.Setup(0, 0);
    idINTIMIDATION.Setup(0, 0);
    idJLTKICKS.Setup(0, 0);
    idJOLT.Setup(0, 0);
    idJUGULAR.Setup(0, 0);
    idKICK.Setup(0, 0);
    idKNEESTRIKE.Setup(0, 0);
    idKNIFEPLAY.Setup(0, 0);
    idLAYHAND.Setup(0, 0);
    idLEOPARDCLAW.Setup(0, 0);
    idMEND.Setup(0, 0);
    idMONKEY.Setup(0, 0);
    idOPFRENZY.Setup(0, 0);
    idOPPORTUNISTICSTRIKE.Setup(0, 0);
    idPETASSIST.Setup(0, 0);
    idPETDELAY.Setup(0, 0);
    idPETRANGE.Setup(0, 0);
    idPETMEND.Setup(0, 0);
    idPICKPOCKET.Setup(0, 0);
    idPINPOINT.Setup(0, 0);
    idPOTHEALFAST.Setup(0, 0);
    idPOTHEALOVER.Setup(0, 0);
    idPROVOKE[0].Setup(0, 0);
    idPROVOKE[1].Setup(0, 0);
    idRAGEVOLLEY.Setup(0, 0);
    idRAKE.Setup(0, 0);
    idRALLOS.Setup(0, 0);
    idRAVENS.Setup(0, 0);
    idRIGHTIND.Setup(0, 0);
    idROUNDKICK.Setup(0, 0);
    idSELOS.Setup(0, 0);
    idSENSETRAP.Setup(0, 0);
    idSLAM.Setup(0, 0);
    idSLAPFACE.Setup(0, 0);
    idSNEAK.Setup(0, 0);
    idSTEELY.Setup(0, 0);
    idSTORMBLADES.Setup(0, 0);
    idSTRIKE.Setup(0, 0);
    idSTUN[0].Setup(0, 0);
    idSTUN[1].Setup(0, 0);
    idSYNERGY.Setup(0, 0);
    idTAUNT.Setup(0, 0);
    idTHIEFEYE.Setup(0, 0);
    idTHROATJAB.Setup(0, 0);
    idTHROWSTONE.Setup(0, 0);
    idTIGERCLAW.Setup(0, 0);
    idTWISTEDSHANK.Setup(0, 0);
    idVIGAXE.Setup(0, 0);
    idVIGDAGGER.Setup(0, 0);
    idVIGSHURIKEN.Setup(0, 0);
    idWITHSTAND.Setup(0, 0);
    idYAULP.Setup(0, 0);

    AbilityFind(&idBACKSTAB, &sbkstab, 0);
    AbilityFind(&idBASH, &sbash, 0);
    AbilityFind(&idBANESTRIKE, &banestrike, 0);
    AbilityFind(&idBEGGING, &sbegging, 0);
    AbilityFind(&idDISARM, &sdisarm, 0);
    AbilityFind(&idFORAGE, &sforage, 0);
    AbilityFind(&idFRENZY, &sfrenzy, 0);
    AbilityFind(&idHIDE, &shide, 0);
    AbilityFind(&idINTIMIDATION, &sintim, 0);
    AbilityFind(&idKICK, &skick, 0);
    AbilityFind(&idLAYHAND, &layhand, 0);
    AbilityFind(&idMEND, &smend, 0);
    AbilityFind(&idPICKPOCKET, &sppocket, 0);
    AbilityFind(&idSENSETRAP, &ssensetr, 0);
    AbilityFind(&idSLAM, &sslam, 0);
    AbilityFind(&idSNEAK, &ssneak, 0);
    AbilityFind(&idTAUNT, &staunt, 0);
    AbilityFind(&idBATTLELEAP, &btlstromp, &btlleap, 0);  // prefer battle stomp over battle leap
    AbilityFind(&idTHROWSTONE, &tstone, 0);

    AbilityFind(&idPOTHEALOVER, &potover16, &potover15, &potover14, &potover13, &potover12, &potover11, &potover10, &potover9, &potover8, &potover7, &potover6, &potover5, &potover4, &potover3, &potover2, &potover1, &potover0, 0);
    AbilityFind(&idPOTHEALFAST, &potfast16, &potfast15, &potfast14, &potfast13, &potfast12, &potfast11, &potfast10, &potfast9, &potfast8, &potfast7, &potfast6, &potfast5, &potfast4, &potfast3, &potfast2, &potfast1, &potfast0, 0);
    doSTAB = 0;
    switch (Class) {
    case  Warrior: // WAR
        AbilityFind(&idPROVOKE[1], &prowar49, &prowar48, &prowar47, &prowar46, &prowar45, &prowar44, &prowar43, &prowar42, &prowar41, &prowar40, &prowar39, &prowar38, &prowar37, &prowar36, &prowar35, &prowar34, &prowar33, &prowar32, &prowar31, &prowar30, &prowar29, &prowar28, &prowar27, &prowar26, &prowar25, &prowar24, &prowar23, &prowar22, &prowar21, &prowar20, &prowar19, &prowar18, &prowar17, &prowar16, &prowar15, &prowar14, &prowar13, &prowar12, &prowar11, &prowar10, &prowar9, &prowar8, &prowar7, &prowar6, &prowar5, &prowar4, &prowar3, &prowar2, &prowar1, 0);
        AbilityFind(&idOPPORTUNISTICSTRIKE, &opstrke15, &opstrke14, &opstrke13, &opstrke12, &opstrke11, &opstrke10, &opstrke9, &opstrke8, &opstrke7, &opstrke6, &opstrke5, &opstrke4, &opstrke3, &opstrke2, &opstrke1, 0);
        AbilityFind(&idFIELDARM, &fieldarm18, &fieldarm17, &fieldarm16, &fieldarm15, &fieldarm14, &fieldarm13, &fieldarm12, &fieldarm11, &fieldarm10, &fieldarm9, &fieldarm8, &fieldarm7, &fieldarm6, &fieldarm5, &fieldarm4, &fieldarm3, &fieldarm2, &fieldarm1, 0);
        AbilityFind(&idDEFENSE, &defense18, &defense17, &defense16, &defense15, &defense14, &defense13, &defense12, &defense11, &defense10, &defense9, &defense8, &defense7, &defense6, &defense5, &defense4, &defense3, &defense2, &defense1, 0);
        AbilityFind(&idTHROATJAB, &throat3, &throat2, &throat1, 0);
        AbilityFind(&idKNEESTRIKE, &kneestrike, 0);
        AbilityFind(&idGUTPUNCH, &gutpunch, 0);
        AbilityFind(&idCALLCHALLENGE, &callchal, 0);
        AbilityFind(&idCOMMANDING, &commanding, 0);
        break;
    case Cleric: // CLR
        AbilityFind(&idYAULP, &yaulp, 0);
        break;
    case  Paladin: // PAL
        AbilityFind(&idCHALLENGEFOR, &honor24, &honor23, &honor22, &honor21, &honor20, &honor19, &honor18, &honor17, &honor16, &honor15, &honor14, &honor13, &honor12, &honor11, &honor10, &honor9, &honor8, &honor7, &honor6, &honor5, &honor4, &honor3, &honor2, &honor1, 0);
        AbilityFind(&idPROVOKE[0], &stunaas3, &stunaas2, &stunaas1, 0);
        AbilityFind(&idPROVOKE[1], &stunpal49, &stunpal48, &stunpal47, &stunpal46, &stunpal45, &stunpal44, &stunpal43, &stunpal42, &stunpal41, &stunpal40, &stunpal39, &stunpal38, &stunpal37, &stunpal36, &stunpal35, &stunpal34, &stunpal33, &stunpal32, &stunpal31, &stunpal30, &stunpal29, &stunpal28, &stunpal27, &stunpal26, &stunpal25, &stunpal24, &stunpal23, &stunpal22, &stunpal21, &stunpal20, &stunpal19, &stunpal18, &stunpal17, &stunpal16, &stunpal15, &stunpal14, &stunpal13, &stunpal12, &stunpal11, &stunpal10, &stunpal9, &stunpal8, &stunpal7, &stunpal6, &stunpal5, &stunpal4, &stunpal3, &stunpal2, &stunpal1, 0);
        AbilityFind(&idSTEELY, &steely18, &steely17, &steely16, &steely15, &steely14, &steely13, &steely12, &steely11, &steely10, &steely9, &steely8, &steely7, &steely6, &steely5, &steely4, &steely3, &steely2, &steely1, 0);
        AbilityFind(&idSTUN[0], &stunaas3, &stunaas2, &stunaas1, 0);
        AbilityFind(&idSTUN[1], &stunpal49, &stunpal48, &stunpal47, &stunpal46, &stunpal45, &stunpal44, &stunpal43, &stunpal42, &stunpal41, &stunpal40, &stunpal39, &stunpal38, &stunpal37, &stunpal36, &stunpal35, &stunpal34, &stunpal33, &stunpal32, &stunpal31, &stunpal30, &stunpal29, &stunpal28, &stunpal27, &stunpal26, &stunpal25, &stunpal24, &stunpal23, &stunpal22, &stunpal21, &stunpal20, &stunpal19, &stunpal18, &stunpal17, &stunpal16, &stunpal15, &stunpal14, &stunpal13, &stunpal12, &stunpal11, &stunpal10, &stunpal9, &stunpal8, &stunpal7, &stunpal6, &stunpal5, &stunpal4, &stunpal3, &stunpal2, &stunpal1, 0);
        AbilityFind(&idWITHSTAND, &withstand18, &withstand17, &withstand16, &withstand15, &withstand14, &withstand13, &withstand12, &withstand11, &withstand10, &withstand9, &withstand8, &withstand7, &withstand6, &withstand5, &withstand4, &withstand3, &withstand2, &withstand1, 0);
        AbilityFind(&idRIGHTIND, &rightidg12, &rightidg11, &rightidg10, &rightidg9, &rightidg8, &rightidg7, &rightidg6, &rightidg5, &rightidg4, &rightidg3, &rightidg2, &rightidg1, 0);
        break;
    case  Ranger: // RNG
        AbilityFind(&idJOLT, &joltrng2, &joltrng1, 0);
        AbilityFind(&idJLTKICKS, &jltkicks24, &jltkicks23, &jltkicks22, &jltkicks21, &jltkicks20, &jltkicks19, &jltkicks18, &jltkicks17, &jltkicks16, &jltkicks15, &jltkicks14, &jltkicks13, &jltkicks12, &jltkicks11, &jltkicks10, &jltkicks9, &jltkicks8, &jltkicks7, &jltkicks6, &jltkicks5, &jltkicks4, &jltkicks3, &jltkicks2, &jltkicks1, 0);
        AbilityFind(&idENRAGINGKICK, &enragingkick12, &enragingkick11, &enragingkick10, &enragingkick9, &enragingkick8, &enragingkick7, &enragingkick6, &enragingkick5, &enragingkick4, &enragingkick3, &enragingkick2, &enragingkick1, 0);
        AbilityFind(&idSTORMBLADES, &bladesrng6, &bladesrng5, &bladesrng4, &bladesrng3, &bladesrng2, &bladesrng1, 0);
        //AbilityFind(&idFEROCIOUSKICK, &ferociouskick, 0);
        break;
    case  Shadowknight: // SHD
        AbilityFind(&idFEIGN[0], &feigns24, &feigns23, &feigns22, &feigns21, &feigns20, &feigns19, &feigns18, &feigns17, &feigns16, &feigns15, &feigns14, &feigns13, &feigns12, &feigns11, &feigns10, &feigns9, &feigns8, &feigns7, &feigns6, &feigns5, &feigns4, &feigns3, &feigns2, &feigns1, 0);
        AbilityFind(&idFEIGN[1], &feigndp, 0);
        AbilityFind(&idHARMTOUCH, &harmtouch, 0);
        AbilityFind(&idGBLADE, &gblade12, &gblade11, &gblade10, &gblade9, &gblade8, &gblade7, &gblade6, &gblade5, &gblade4, &gblade3, &gblade2, &gblade1, 0);
        AbilityFind(&idPROVOKE[1], &terror30, &terror29, &terror28, &terror27, &terror26, &terror25, &terror24, &terror23, &terror22, &terror21, &terror20, &terror19, &terror18, &terror17, &terror16, &terror15, &terror14, &terror13, &terror12, &terror11, &terror10, &terror9, &terror8, &terror7, &terror6, &terror5, &terror4, &terror3, &terror2, &terror1, 0);
        AbilityFind(&idCHALLENGEFOR, &power24, &power23, &power22, &power21, &power20, &power19, &power18, &power17, &power16, &power15, &power14, &power13, &power12, &power11, &power10, &power9, &power8, &power7, &power6, &power5, &power4, &power3, &power2, &power1, 0);
        AbilityFind(&idSTEELY, &steely18, &steely17, &steely16, &steely15, &steely14, &steely13, &steely12, &steely11, &steely10, &steely9, &steely8, &steely7, &steely6, &steely5, &steely4, &steely3, &steely2, &steely1, 0);
        AbilityFind(&idWITHSTAND, &withstand18, &withstand17, &withstand16, &withstand15, &withstand14, &withstand13, &withstand12, &withstand11, &withstand10, &withstand9, &withstand8, &withstand7, &withstand6, &withstand5, &withstand4, &withstand3, &withstand2, &withstand1, 0);
        break;
    case  Monk: // MNK
        MonkClass = true;
        AbilityFind(&idFISTSOFWU, &fistswu, 0);
        AbilityFind(&idDRAGONPUNCH, &sdrpunch, 0);
        AbilityFind(&idEAGLESTRIKE, &sestrike, 0);
        AbilityFind(&idLEOPARDCLAW, &leop26, &leop25, &leop24, &leop23, &leop22, &leop21, &leop20, &leop19, &leop18, &leop17, &leop16, &leop15, &leop14, &leop13, &leop12, &leop11, &leop10, &leop9, &leop8, &leop7, &leop6, &leop5, &leop4, &leop3, &leop2, &leop1, 0);
        AbilityFind(&idTIGERCLAW, &stigclaw, 0);
        AbilityFind(&idROUNDKICK, &srndkick, 0);
        AbilityFind(&idFLYINGKICK, &sflykick, 0);
        AbilityFind(&idFEIGN[0], &sfeign, 0);
        AbilityFind(&idFEIGN[1], &feignid, 0);
        AbilityFind(&idPROVOKE[0], &stunmnk2, &stunmnk1, 0);
        AbilityFind(&idSTUN[0], &stunmnk2, &stunmnk1, 0);
        AbilityFind(&idSYNERGY, &synergy18, &synergy17, &synergy16, &synergy15, &synergy14, &synergy13, &synergy12, &synergy11, &synergy10, &synergy9, &synergy8, &synergy7, &synergy6, &synergy5, &synergy4, &synergy3, &synergy2, &synergy1, 0);
        AbilityFind(&idVIGSHURIKEN, &vigmnk3, &vigmnk2, &vigmnk1, 0);
        AbilityFind(&idCLOUD, &cloud5, &cloud5, &cloud4, &cloud3, &cloud2, &cloud1, 0);
        AbilityFind(&idMONKEY, &monkey3, &monkey2, &monkey1, 0);
        break;
    case  Bard: // BRD
        BardClass = true;
        AbilityFind(&idSELOS, &selos, 0);
        AbilityFind(&idBOASTFUL, &boastful, 0);
        break;
    case  Rogue: // ROG
        AbilityFind(&idTHIEFEYE, &thiefeye4, &thiefeye3, &thiefeye2, &thiefeye1, 0);
        AbilityFind(&idSTRIKE, &strike28, &strike27, &strike26, &strike25, &strike24, &strike23, &strike22, &strike21, &strike20, &strike19, &strike18, &strike17, &strike16, &strike15, &strike14, &strike13, &strike12, &strike11, &strike10, &strike9, &strike8, &strike7, &strike6, &strike5, &strike4, &strike3, &strike2, &strike1, 0);
        AbilityFind(&idKNIFEPLAY, &knifeplay3, &knifeplay2, &knifeplay1, 0);
        AbilityFind(&idBLEED, &bleed18, &bleed17, &bleed16, &bleed15, &bleed14, &bleed13, &bleed12, &bleed11, &bleed10, &bleed9, &bleed8, &bleed7, &bleed6, &bleed5, &bleed4, &bleed3, &bleed2, &bleed1, 0);
        AbilityFind(&idVIGDAGGER, &vigrog18, &vigrog17, &vigrog16, &vigrog15, &vigrog14, &vigrog13, &vigrog12, &vigrog11, &vigrog10, &vigrog9, &vigrog8, &vigrog7, &vigrog6, &vigrog5, &vigrog4, &vigrog3, &vigrog2, &vigrog1, 0);
        AbilityFind(&idASSAULT, &assault18, &assault17, &assault16, &assault15, &assault14, &assault13, &assault12, &assault11, &assault10, &assault9, &assault8, &assault7, &assault6, &assault5, &assault4, &assault3, &assault2, &assault1, 0);
        AbilityFind(&idPINPOINT, &pinpoint18, &pinpoint17, &pinpoint16, &pinpoint15, &pinpoint14, &pinpoint13, &pinpoint12, &pinpoint11, &pinpoint10, &pinpoint9, &pinpoint8, &pinpoint7, &pinpoint6, &pinpoint5, &pinpoint4, &pinpoint3, &pinpoint2, &pinpoint1, 0);
        AbilityFind(&idJUGULAR, &jugular21, &jugular20, &jugular19, &jugular18, &jugular17, &jugular16, &jugular15, &jugular14, &jugular13, &jugular12, &jugular11, &jugular10, &jugular9, &jugular8, &jugular7, &jugular6, &jugular5, &jugular4, &jugular3, &jugular2, &jugular1, 0);
        AbilityFind(&idESCAPE, &escape, 0);
        AbilityFind(&idTWISTEDSHANK, &twisted, 0);
        // Seized Opportunity position fix 05/01/2011 (htw)
        SOValue = AAPoint(GetAAIndexByName("Seized Opportunity"));
        if (SOValue >= 74)
            doSTAB = 512;
        else if (SOValue >= 60)
            doSTAB = 256;
        else if (SOValue >= 39)
            doSTAB = 192;
        else if (SOValue >= 18)
            doSTAB = 128;
        else
            doSTAB = 64;
        break;
    case Necromancer: // NEC
        AbilityFind(&idFEIGN[0], &feign_n3, &feign_n2, &feign_n1, &feigns3, &feigns2, &feigns1, 0);
        AbilityFind(&idFEIGN[1], &feigndp, 0);
        AbilityFind(&idPETMEND, &mendpet2, &mendpet1, 0);
        break;
    case Mage: // MAG
        AbilityFind(&idPETMEND, &mendpet2, &mendpet1, 0);
        break;
    case Beastlord: // BST
        AbilityFind(&idRAKE, &rake22, &rake21, &rake20, &rake19, &rake18, &rake17, &rake16, &rake15, &rake14, &rake13, &rake12, &rake11, &rake10, &rake9, &rake8, &rake7, &rake6, &rake5, &rake4, &rake3, &rake2, &rake1, 0);
        AbilityFind(&idFERALSWIPE, &feral1, 0);
        AbilityFind(&idPETMEND, &mendpet1, &mendpet2, 0);
        AbilityFind(&idJOLT, &joltbst1, 0);
        AbilityFind(&idFEIGN[0], &feign_bst, 0);
        AbilityFind(&idASP, &asp, 0);
        AbilityFind(&idCSTRIKE, &cstrike, 0);
        AbilityFind(&idRAVENS, &ravens, 0);
        AbilityFind(&idFCLAW, &fclaw18, &fclaw17, &fclaw16, &fclaw15, &fclaw14, &fclaw13, &fclaw12, &fclaw11, &fclaw10, &fclaw9, &fclaw8, &fclaw7, &fclaw6, &fclaw5, &fclaw4, &fclaw3, &fclaw2, &fclaw1, 0);
        AbilityFind(&idBVIVI, &bvivi12, &bvivi11, &bvivi10, &bvivi9, &bvivi8, &bvivi7, &bvivi6, &bvivi5, &bvivi4, &bvivi3, &bvivi2, &bvivi1, 0);
        AbilityFind(&idGORILLASMASH, &gorillasmash, 0);
        break;
    case Berserker: // BER
        BerserkerClass = true;
        AbilityFind(&idSLAPFACE, &slapface15, &slapface14, &slapface13, &slapface12, &slapface11, &slapface10, &slapface9, &slapface8, &slapface7, &slapface6, &slapface5, &slapface4, &slapface3, &slapface2, &slapface1, 0);
        AbilityFind(&idJOLT, &joltber28, &joltber27, &joltber26, &joltber25, &joltber24, &joltber23, &joltber22, &joltber21, &joltber20, &joltber19, &joltber18, &joltber17, &joltber16, &joltber15, &joltber14, &joltber13, &joltber12, &joltber11, &joltber10, &joltber9, &joltber8, &joltber7, &joltber6, &joltber5, &joltber4, &joltber3, &joltber2, &joltber1, 0);
        AbilityFind(&idRAGEVOLLEY, &volley29, &volley28, &volley27, &volley26, &volley25, &volley24, &volley23, &volley22, &volley21, &volley20, &volley19, &volley18, &volley17, &volley16, &volley15, &volley14, &volley13, &volley12, &volley11, &volley10, &volley9, &volley8, &volley7, &volley6, &volley5, &volley4, &volley3, &volley2, &volley1, 0);
        AbilityFind(&idPROVOKE[1], &stunber25, &stunber24, &stunber23, &stunber22, &stunber21, &stunber20, &stunber19, &stunber18, &stunber17, &stunber16, &stunber15, &stunber14, &stunber13, &stunber12, &stunber11, &stunber10, &stunber9, &stunber8, &stunber7, &stunber6, &stunber5, &stunber4, &stunber3, &stunber2, &stunber1, 0);
        AbilityFind(&idSTUN[1], &stunber28, &stunber27, &stunber26, &stunber25, &stunber24, &stunber23, &stunber22, &stunber21, &stunber20, &stunber19, &stunber18, &stunber17, &stunber16, &stunber15, &stunber14, &stunber13, &stunber12, &stunber11, &stunber10, &stunber9, &stunber8, &stunber7, &stunber6, &stunber5, &stunber4, &stunber3, &stunber2, &stunber1, 0);
        AbilityFind(&idCRIPPLE, &cripple28, &cripple27, &cripple26, &cripple25, &cripple24, &cripple23, &cripple22, &cripple21, &cripple20, &cripple19, &cripple18, &cripple17, &cripple16, &cripple15, &cripple14, &cripple13, &cripple12, &cripple11, &cripple10, &cripple9, &cripple8, &cripple7, &cripple6, &cripple5, &cripple4, &cripple3, &cripple2, &cripple1, 0);
        AbilityFind(&idCRYHAVOC, &cryhavoc4, &cryhavoc3, &cryhavoc2, &cryhavoc1, 0);
        AbilityFind(&idBLOODLUST, &bloodlust18, &bloodlust17, &bloodlust16, &bloodlust15, &bloodlust14, &bloodlust13, &bloodlust12, &bloodlust11, &bloodlust10, &bloodlust9, &bloodlust8, &bloodlust7, &bloodlust6, &bloodlust5, &bloodlust4, &bloodlust3, &bloodlust2, &bloodlust1, 0);
        AbilityFind(&idVIGAXE, &vigber18, &vigber17, &vigber16, &vigber15, &vigber14, &vigber13, &vigber12, &vigber11, &vigber10, &vigber9, &vigber8, &vigber7, &vigber6, &vigber5, &vigber4, &vigber3, &vigber2, &vigber1, 0);
        AbilityFind(&idOPFRENZY, &opfrenzy18, &opfrenzy17, &opfrenzy16, &opfrenzy15, &opfrenzy14, &opfrenzy13, &opfrenzy12, &opfrenzy11, &opfrenzy10, &opfrenzy9, &opfrenzy8, &opfrenzy7, &opfrenzy6, &opfrenzy5, &opfrenzy4, &opfrenzy3, &opfrenzy2, &opfrenzy1, 0);
        AbilityFind(&idRALLOS, &rallos18, &rallos17, &rallos16, &rallos15, &rallos14, &rallos13, &rallos12, &rallos11, &rallos10, &rallos9, &rallos8, &rallos7, &rallos6, &rallos5, &rallos4, &rallos3, &rallos2, &rallos1, 0);
        break;
    }

    if (GetPrivateProfileString(section, NULL, "", keys, sizeof(keys), INIFileName))
    {
        char* pKeys = keys;
        CHAR szTempKey[MAX_STRING] = { 0 };
        while (pKeys[0])
        {
            strcpy_s(szTempKey, pKeys);
            if (GetPrivateProfileString(section, szTempKey, "", temp, sizeof(temp), INIFileName))
            {
                _strlwr_s(szTempKey);
                if (ec != (c = CmdListe.find(szTempKey)))
                    (*c).second.Setup(temp);
                else if (ei != (i = IniListe.find(szTempKey)))
                    (*i).second.Setup(temp);
            }
            pKeys += strlen(pKeys) + 1;
        }
    }

    Loaded = true;
}

void Exporting() {
    char output[MAX_STRING];
    char defval[MAX_STRING];
    Liste::iterator c, e;
    WritePrivateProfileString(section, NULL, NULL, INIFileName);
    e = CmdListe.end();
    for (c = CmdListe.begin(); c != e; c++) {
        output[0] = 0;
        if ((*c).second.C)
            if (string *S = (string*)(*c).second.Value())
                strcpy_s(output, S->c_str());
        if ((*c).second.A || (*c).second.V)
            if (long *V = (long*)(*c).second.Value())
                _itoa_s(*V, output, 10);
        if (output[0]) {
            strcpy_s(defval, (*c).second.D);
            if (defval[0])
                ParseMacroData(defval, sizeof(defval));
            if (strcmp(output, "0") || strcmp(output, defval))
                WritePrivateProfileString(section, (*c).second.K, output, INIFileName);
        }
    }
    e = IniListe.end();
    for (c = IniListe.begin(); c != e; c++) {
        output[0] = 0;
        if ((*c).second.C)
            if (string *S = (string*)(*c).second.Value())
                strcpy_s(output, S->c_str());
        if ((*c).second.A || (*c).second.V)
            if (long *V = (long*)(*c).second.Value())
                _itoa_s(*V, output, 10);
        if (output[0]) {
            strcpy_s(defval, sizeof(defval), (*c).second.D);
            if (defval[0])
                ParseMacroData(defval, sizeof(defval));
            if (strcmp(output, "0") || strcmp(output, defval))
                WritePrivateProfileString(section, (*c).second.K, output, INIFileName);
        }
    }
    sprintf_s(output, "%1.3f", PLUGIN_VERS); WritePrivateProfileString(section, "version", output, INIFileName);
}

void MapInsert(Liste *MyList, Option MyOption)
{
    MyList->insert(Liste::value_type(MyOption.K, MyOption));
}

void MeleeHelp()
{
    WriteChatf("%s::-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-", PLUGIN_NAME);
    WriteChatf("%s::Version [\ag%1.3f\ax] Loaded!", PLUGIN_NAME, PLUGIN_VERS);
    WriteChatf("%s::-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-", PLUGIN_NAME);
    for (Liste::iterator i = CmdListe.begin(); i != CmdListe.end(); i++) (*i).second.Write();
    WriteChatf("%s::-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-", PLUGIN_NAME);
    if (NULL == PluginEntry("mq2cast", "CastCommand"))
    {
        WriteChatf("%s::Required Latest [\arMQ2Cast\ax] for AA/SPELL/ITEM casting.", PLUGIN_NAME);
    }
    if (NULL == PluginEntry("mq2moveutils", "StickCommand"))
    {
        WriteChatf("%s::Required Latest [\arMQ2MoveUtils\ax] for MOVEMENT.", PLUGIN_NAME);
    }
}

void PetSEEN()
{
    PetOnAttk = (unsigned long)clock() + 12000;
    PetOnHold = false;
}

void PetBACK()
{
    PetOnHold = true;
    if (doPETASSIST && pPetInfoWnd && PetButtonEnabled(UI_PetBack))
    {
        Announce(SHOW_CONTROL, "%s::Command [\ay%s\ax].", PLUGIN_NAME, "/pet back off");
        EzCommand("/pet back off");
    }
}

void PetATTK()
{
    if (MeleeTarg)
    {
        PSPAWNINFO MTarg = GetSpawnID(MeleeTarg);
        Announce(SHOW_CONTROL, "%s::Checking PetEngageHPs [\ay%d<%d\ax].", PLUGIN_NAME, MTarg->HPCurrent, doPETENGAGEHPS);
        if (doPETASSIST && !(MTarg->HPCurrent<doPETENGAGEHPS)) return;
    }
    PetSEEN();
    if (doPETASSIST && pPetInfoWnd && !(onEVENT & 0x0003) && MeleeTarg && PetButtonEnabled(UI_PetAttk) && SpawnType(GetSpawnID(MeleeTarg), NPC_TYPE))
    {
        Announce(SHOW_CONTROL, "%s::Command [\ay%s\ax].", PLUGIN_NAME, "/pet attack");
        if (!TargetID(MeleeTarg))
        {
            PSPAWNINFO Current = Target();
            TargetIT(GetSpawnID(MeleeTarg));
            EzCommand("/pet attack");
            if (Current) Command("/squelch /target id %d", Current->SpawnID);
        } 
        else
        {
            EzCommand("/pet attack");
        }
        PetTarget = MeleeTarg;
        if (!doMELEE) MeleeReset();
    }
}

void StickReset(void)
{
    TimerStik = 0;
    if (Sticking) Stick("");
    StickArg[0] = 0;
    onSTICK = (doMELEE && !(onEVENT & 0x8000) && (doSTICKRANGE || doSTICKNORANGE) && bMULoaded); // x && x && !ranged && moveutils_loaded
}

void RangeReset()
{
    if (!doRANGE && AutoFire) EzCommand("/autofire");
    if (!doRANGE && (onEVENT & 0x8000)) onEVENT &= 0x7FFF;
}

void OtherReset()
{
    MeleeTarg  = 0;
    MeleeType  = 0;
    MeleeLife  = 0;
    MeleeCast  = 0;
    MeleeFlee  = 0;
    TimerBack  = 0;
    TimerLife  = 0;
    TimerFace  = 0;
    TimerStun  = 0;
    PetTarget  = 0;
    PetOnWait  = 0;
    SwingHits  = 0;
    TakenHits  = 0;
    onEVENT    = 0;
    doDOWN     = 0;
    doHOLY     = 0;
    StrikeFail = false;
}

void MeleeReset()
{
    if (!doMELEE && *EQADDR_ATTACK)
    {
        MeleeTarg = 0;
        if (onEVENT & 0x0008) onEVENT |= 0x0008;
        AttackOFF();
        StickReset();
    }
}

void AggroReset()
{
    int onAGGRO = (doAGGRO && IsGrouped());
    onCHALLENGEFOR = (onAGGRO && idCHALLENGEFOR.Found());
    onENRAGINGKICK = (onAGGRO && idENRAGINGKICK.Found());
    onBELOW = (onAGGRO && doPROVOKEMAX) ? doPROVOKEMAX : 0;

    if (doMELEE)
    {
        if (long PW = (IsGrouped() && doAGGRO) ? elAGGROPRI : elMELEEPRI) Equip(PW, inv_primary);
        if (long SW = (IsGrouped() && doAGGRO) ? elAGGROSEC : elMELEESEC) Equip(SW, inv_secondary);
    }
}

void SneakOFF() {
    if (IsSneaking() && idSNEAK.Found()) idSNEAK.Press();
}

void HideOFF() {
    if (IsInvisible() && idHIDE.Found()) idHIDE.Press();
}

int StabCheck()
{
    if (elPOKER && OkayToEquip(Giant))
    {
        char szTempItem[25] = { 0 };
        sprintf_s(szTempItem, "%d", elPOKER);
        CItemLocation cFindItem;
        if (ItemFind(&cFindItem, szTempItem))
        {
            if (PokerType(cFindItem.pBagSlot)) return true;
        }
    }
    return PokerType(ContPrimary());
}

void StabPress() {
    long saveid = 0;
    CItemLocation cMoveItem;
    char szTempItem[25] = { 0 };
    sprintf_s(szTempItem, "%d", elPOKER);
    if (elPOKER && OkayToEquip(Giant) && ItemFind(&cMoveItem, szTempItem))
    {
        if (PCONTENTS pri = ContPrimary())
            if (GetItemFromContents(pri)->ItemNumber != elPOKER) {
                saveid = GetItemFromContents(pri)->ItemNumber;
                Equip(elPOKER, inv_primary);
            }
    }
    if (PokerType(ContPrimary())) {
        idBACKSTAB.Press();
        SwingHits++;
    }
    if (saveid) Equip(saveid, inv_primary);
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

PLUGIN_API void ThrowIT(PSPAWNINFO pChar, char* Cmd) {
    if (gbRangedAttackReady && pTarget && TargetType(NPC_TYPE) &&
        !InRange(SpawnMe(), (PSPAWNINFO)pTarget, 35) &&
        fabs(AngularHeading(SpawnMe(), (PSPAWNINFO)pTarget)) < 50 &&
        LineOfSight(SpawnMe(), (PSPAWNINFO)pTarget)) {

        unsigned long ulSlot = 0;
        char szTempItem[25] = { 0 };
        CItemLocation cMoveItem;
        char szItemName[MAX_STRING] = { 0 };
        // test if we could do ranged with current ammo/range configuration
        long crT = 99; long crI = 0; long caT = 99; long caI = 0; long caQ = 0;
        if (PCONTENTS r = ContRange())
        {
            crI = GetItemFromContents(r)->ItemNumber;
            crT = GetItemFromContents(r)->ItemType;
            strcpy_s(szItemName, GetItemFromContents(r)->Name);
        }
        if (PCONTENTS a = ContAmmo()) {
            caI = GetItemFromContents(a)->ItemNumber;
            caT = GetItemFromContents(a)->ItemType;
            if (caT == 7 || caT == 19 || caT == 27) caQ = CountItemByID(caI);
        }
        if (!(caI && ((caT == 27 && crT == 5) || (caI == crI && (caT == 7 || caT == 19)))))
        {
            if (!OkayToEquip(Giant)) return;

            // grab information about user defined range/ammunition
            long erT = 99; long eaT = 99; long eaQ = 0;

            PCONTENTS r = NULL;
            PCONTENTS a = NULL;

            //if(PCONTENTS r=ItemLocate(elRANGED)) erT=GetItemFromContents(r)->ItemType;
            sprintf_s(szTempItem, "%d", elRANGED);
            if (ItemFind(&cMoveItem, szTempItem))
            {
                r = cMoveItem.pBagSlot;
                erT = GetItemFromContents(r)->ItemType;
            }

            //if(PCONTENTS a=ItemLocate(elARROWS)) {
            //    eaT=a->Item->ItemType;
            //    if(eaT == 7 || eaT == 19 || eaT == 27) eaQ=ItemCounts(elARROWS);
            //}
            sprintf_s(szTempItem, "%d", elARROWS);
            if (ItemFind(&cMoveItem, szTempItem))
            {
                a = cMoveItem.pBagSlot;
                eaT = GetItemFromContents(a)->ItemType;
                if (eaT == 7 || eaT == 19 || eaT == 27) eaQ = CountItemByID(elARROWS);
            }


            // find equipping scenario (bow+arrow) or (throw/throw).
            long EquipRangeID = 0; long EquipArrowID = 0;
            if ((crT == 5 || erT == 5) && (caT == 27 || eaT == 27)) {
                EquipRangeID = (crT == 5) ? crI : elRANGED;
                EquipArrowID = (caT == 27) ? caI : elARROWS;
            }
            else if ((caQ > 2 && (caT == 7 || caT == 19)) || (eaQ > 2 && (eaT == 7 || eaT == 19)))
            {
                EquipRangeID = (caQ > 2 && (caT == 7 || caT == 19)) ? caI : elARROWS;
                EquipArrowID = (caQ > 2 && (caT == 7 || caT == 19)) ? caI : elARROWS;
            }
            else return;

            // load equipping scenario found!
            if (EquipRangeID && EquipArrowID) {
                if (crI != EquipRangeID) Equip(EquipRangeID, inv_range);
                if (caI != EquipArrowID) Equip(EquipArrowID, inv_ammo);
                if (EquipArrowID == EquipRangeID && !ContRange()) { // Reloads thrown weapons
                    EzCommand("/ctrlkey /itemnotify ammo leftmouseup");
                    EzCommand("/shiftkey /itemnotify ranged leftmouseup");
                }
            }
        }

        // we can't call do_ranged directly because it will hit before the item swaps
        EzCommand("/ranged");

        // preserve ranged slot
        if (elRANGED != crI) {
            char szCommand[MAX_STRING] = { 0 };
            sprintf_s(szCommand, "/shiftkey /itemnotify \"%s\" leftmouseup", szItemName);
            EzCommand(szCommand);
            EzCommand("/shiftkey /itemnotify ranged leftmouseup");
            EzCommand("/autoinventory");
        }
    }
}

PLUGIN_API void Override(PSPAWNINFO pChar, char* Cmd)
{
    Announce(SHOW_OVERRIDE, Cmd, PLUGIN_NAME);
    MeleeTime = (unsigned long)clock() + delay;
    AttackOFF();
    if (onEVENT & 0x0008) onEVENT |= 0x0008;
    if (AutoFire)
    {
        AutoFire = false;
        EzCommand("/autofire");
    }
    if (doPETASSIST) PetBACK();
    StickReset();
    OtherReset();
}

PLUGIN_API void Melee(PSPAWNINFO pChar, char* Cmd)
{
    char Tmp[MAX_STRING]; char Var[MAX_STRING]; char Set[MAX_STRING]; BYTE Parm = 1; bool Help = true;
    Liste::iterator c; Liste::iterator ec = CmdListe.end();
    do {
        GetArg(Tmp, Cmd, Parm++);
        _strlwr_s(Tmp);
        GetArg(Var, Tmp, 1, FALSE, FALSE, FALSE, '=');
        GetArg(Set, Tmp, 2, FALSE, FALSE, FALSE, '=');
        if (Var[0]) {
            c = CmdListe.find(Var);
            if (ec != c) {
                (*c).second.Setup(Set);
                (*c).second.Write();
                Help = false;
            }
            else if (!Set[0] && (!_stricmp(Var, "on") || !_stricmp(Var, "off"))) {
                if (ec != (c = CmdListe.find("plugin"))) (*c).second.Setup(Var);
                Help = false;
            }
            else if (!Set[0] && (!_stricmp(Var, "reload") || !_stricmp(Var, "load"))) {
                WriteChatf("%s::Loading...", PLUGIN_NAME);
                Configure();
                Help = false;
            }
            else if (!Set[0] && !_stricmp(Var, "save")) {
                WriteChatf("%s::Saving...", PLUGIN_NAME);
                Exporting();
                Help = false;
            }
            else if (!Set[0] && !_stricmp(Var, "key")) {
                char buffer[MAX_STRING]; KeyCombo combo;
                DescribeKeyCombo(pKeypressHandler->NormalKey[FindMappableCommand("AUTOPRIM")], buffer, sizeof(buffer));
                WriteChatf("%s::\ayATTACK\ax binded to [\ay%s\ax]", PLUGIN_NAME, buffer);
                GetMQ2KeyBind("MELEE", false, combo);
                DescribeKeyCombo(combo, buffer, sizeof(buffer));
                WriteChatf("%s::\ayMELEE\ax  binded to [\ay%s\ax]", PLUGIN_NAME, buffer);
                GetMQ2KeyBind("RANGE", false, combo);
                DescribeKeyCombo(combo, buffer, sizeof(buffer));
                WriteChatf("%s::\ayRANGE\ax  binded to [\ay%s\ax]", PLUGIN_NAME, buffer);
                Help = false;
            }
            else if (!Set[0] && !_stricmp(Var, "reset")) {
                Override(NULL, "%s::Resetting...");
                Help = false;
            }
            else {
                WriteChatf("%s::Unsupported Argument <\ar%s\ax>", PLUGIN_NAME, Var);
                break;
            }
        }
    } while (strlen(Tmp));
    if (Help) MeleeHelp();
}

PLUGIN_API void KillThis(PSPAWNINFO pChar, char* Cmd)
{
    if ((doMELEE || doPETASSIST)  && doSKILL && pTarget && !TargetID(MeleeTarg) && TargetType(NPC_TYPE) && InGame())
    {
        if (IsFeigning()) EzCommand("/stand");
        StickReset();
        OtherReset();
        AggroReset();
        MeleeTarg = ((PSPAWNINFO)pTarget)->SpawnID;
        strcpy_s(MeleeName, ((PSPAWNINFO)pTarget)->DisplayedName);
        MeleeSize = strlen(MeleeName) + 1;
        MeleeType = SpawnMask((PSPAWNINFO)pTarget);
        if (doMELEE) {
            onEVENT |= 0x0008;
            Announce(SHOW_ATTACKING, "%s::Attacking [\ay%s\ax].", PLUGIN_NAME, MeleeName, MeleeTarg);
        }
    }
}

PLUGIN_API void EnrageON(PSPAWNINFO pChar, char* Cmd) {
    if (long val = atol(Cmd)) if (val != MeleeTarg) return;
    if (doSKILL && doENRAGE && MeleeTarg && InGame()) {
        PSPAWNINFO KillTarg = GetSpawnID(MeleeTarg);
        if (!(onEVENT & 0x0001)) {
            if (!(onEVENT & 0x0002) && doPETASSIST) PetBACK();
            onEVENT |= 0x0001;
            Announce(SHOW_ENRAGING, "MQ2Melee::\arENRAGE\ax detected, taking action!");
        }
        if (*EQADDR_ATTACK && onEVENT & 0x0003 && SpawnType(KillTarg, NPC_TYPE)) {
            double Back = fabs(AngularDistance(KillTarg->Heading, SpawnMe()->Heading));
            double View = fabs(AngularHeading(SpawnMe(), KillTarg));
            if (Back > 92 || View > 60 || onEVENT & 0x0002) {
                onEVENT |= 0x0008;
                AttackOFF();
            }
        }
    }
}

PLUGIN_API void EnrageOFF(PSPAWNINFO pChar, char* Cmd) {
    if (long val = atol(Cmd)) if (val != MeleeTarg) return;
    if (doSKILL && doENRAGE && MeleeTarg && onEVENT & 0x0001 && InGame()) {
        onEVENT &= 0xFFFE;
        if (TargetID(PetTarget)) PetATTK();
        Announce(SHOW_ENRAGING, "MQ2Melee::\agENRAGE\ax ended, taking action!");
    }
}

PLUGIN_API void InfuriateON(PSPAWNINFO pChar, char* Cmd) {
    if (long val = atol(Cmd)) if (val != MeleeTarg) return;
    if (doSKILL && doINFURIATE && MeleeTarg && InGame()) {
        if (!(onEVENT & 0x0002)) {
            if (!(onEVENT & 0x0001) && doPETASSIST) PetBACK();
            onEVENT |= 0x0002;
            Announce(SHOW_ENRAGING, "MQ2Melee::\arINFURIATE\ax detected, taking action!");
        }
        if (*EQADDR_ATTACK) {
            AttackOFF();
            onEVENT |= 0x0008;
        }
    }
}

PLUGIN_API void InfuriateOFF(PSPAWNINFO pChar, char* Cmd) {
    if (long val = atol(Cmd)) if (val != MeleeTarg) return;
    if (doSKILL && doINFURIATE && MeleeTarg && onEVENT & 0x0002 && InGame()) {
        onEVENT &= 0xFFFD;
        if (TargetID(PetTarget)) PetATTK();
        Announce(SHOW_ENRAGING, "MQ2Melee:\agINFURIATE\ax ended, taking action!");
    }
}
BOOL ParseMacroLine(PCHAR szOriginal, SIZE_T BufferSize,std::list<std::string>&out)
{
    // find each {}
    PCHAR pBrace = strstr(szOriginal, "${");
    if (!pBrace)
        return false;
    unsigned long NewLength;
    BOOL Changed = false;
    //PCHAR pPos;
    //PCHAR pStart;
    //PCHAR pIndex;
    CHAR szCurrent[MAX_STRING] = { 0 };
    //MQ2TYPEVAR Result = { 0 };
    do
    {
        // find this brace's end
        PCHAR pEnd = &pBrace[1];
        BOOL Quote = false;
        BOOL BeginParam = false;
        int nBrace = 1;
        while (nBrace)
        {
            ++pEnd;
            if (BeginParam)
            {
                BeginParam = false;
                if (*pEnd == '\"')
                {
                    Quote = true;
                }
                continue;
            }
            if (*pEnd == 0)
            {// unmatched brace or quote
                goto pmdbottom;
            }
            if (Quote)
            {
                if (*pEnd == '\"')
                {
                    if (pEnd[1] == ']' || pEnd[1] == ',')
                    {
                        Quote = false;
                    }
                }
            }
            else
            {
                if (*pEnd == '}')
                {
                    nBrace--;
                }
                else if (*pEnd == '{')
                {
                    nBrace++;
                }
                else if (*pEnd == '[' || *pEnd == ',')
                    BeginParam = true;
            }

        }
        *pEnd = 0;
        strcpy_s(szCurrent, &pBrace[2]);
        if (szCurrent[0] == 0)
        {
            goto pmdbottom;
        }
        if (ParseMacroLine(szCurrent, sizeof(szCurrent),out))
        {
            unsigned long NewLength = strlen(szCurrent);
            memmove(&pBrace[NewLength + 1], &pEnd[1], strlen(&pEnd[1]) + 1);
            int addrlen = (int)(pBrace - szOriginal);
            memcpy_s(pBrace, BufferSize - addrlen, szCurrent, NewLength);
            pEnd = &pBrace[NewLength];
            *pEnd = 0;
        }
        if(!strchr(szCurrent,'[') && !strchr(szCurrent,'.'))
            out.push_back(szCurrent);

        NewLength = strlen(szCurrent);
        memmove(&pBrace[NewLength], &pEnd[1], strlen(&pEnd[1]) + 1);
        int addrlen = (int)(pBrace - szOriginal);
        memcpy_s(pBrace, BufferSize - addrlen, szCurrent, NewLength);
        if (bAllowCommandParse == false) {
            bAllowCommandParse = true;
            Changed = false;
            break;
        }
        else {
            Changed = true;
        }
    pmdbottom:;
    } while (pBrace = strstr(&pBrace[1], "${"));
    if (Changed)
        while (ParseMacroLine(szOriginal, BufferSize,out))
        {
            Sleep(0);
        }
    return Changed;
}
bool OKtoParseShit(std::string &str)
{
    CHAR szBuffer[MAX_STRING] = { 0 };
    strcpy_s(szBuffer, str.c_str());
    std::list<std::string> out;
    ParseMacroLine(szBuffer, MAX_STRING, out);
    bool bOkToCheck = true;
    if (out.size()) {
        for (std::list<std::string>::iterator i = out.begin(); i != out.end(); i++) {
            bOkToCheck = false;
			PCHAR pChar = (PCHAR)(*i).c_str();
			if (FindMQ2Data(pChar)) {
				bOkToCheck = true;
                continue;
            }
            if (!bOkToCheck)
            {
                //ok fine we didnt find it in the tlo map...
                //lets check variables
                if (FindMQ2DataVariable(pChar)) {
                    bOkToCheck = true;
                    continue;
                }
            }
            if (!bOkToCheck)
            {
                //still not found...
                break;
            }
        }
    }
    return bOkToCheck;
}
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

void DowntimeHandle()
{
    if (doSENSETRAP && idSENSETRAP.Ready(ifSENSETRAP)) idSENSETRAP.Press();
    if (!MeleeTarg && (unsigned long)clock() > TimerAttk && Immobile) {
        if (DOWNSHITIF.length() < 5 || Evaluate((char*)("${If[" + DOWNSHITIF.substr(5, DOWNSHITIF.length() - 6) + ",1,0]}").c_str())) {
            if (DOWNSHIT[doDOWN].size()) {
                switch (doDOWNFLAG[doDOWN])
                {
                    case 1://not a macro dependant flag
                    {
                        EzCommand((char*)DOWNSHIT[doDOWN].c_str());
                        break;
                    }
                    case 2://a macro must be running and the variable MUST exist
                    {
                        if (gRunning) {
                            if (OKtoParseShit(DOWNSHIT[doDOWN])) {
                                EzCommand((char*)DOWNSHIT[doDOWN].c_str());
                            }
                        }
                        break;
                    }
                    default:
                        break;
                }
            }
            for (long x = 1; x < 61; x++) {
                long n = (x + doDOWN) % 60;
                if (doDOWNFLAG[n]) {
                    doDOWN = n;
                    break;
                }
            }
        }
        if (doFORAGE && idFORAGE.Ready(ifFORAGE))
        {
            idFORAGE.Press();
        }
        else if (doSNEAK && !IsSneaking() && idSNEAK.Ready(ifSNEAK))
        {
            idSNEAK.Press();
        }
        else if (doHIDE && !IsInvisible() && idHIDE.Ready(ifHIDE))
        {
            idHIDE.Press();
        }
    }
}

void MeleeHandle()
{
    // check opened windows that wont let us perform any melee actions.
    if (!BardClass  && WinState((CXWnd*)pCastingWnd))  return;
    if (WinState((CXWnd*)pSpellBookWnd))               return;
    if (WinState((CXWnd*)pLootWnd))                    return;
    if (WinState((CXWnd*)pBankWnd))                    return;
    if (WinState((CXWnd*)pMerchantWnd))                return;
    if (WinState((CXWnd*)pTradeWnd))                   return;
    if (WinState((CXWnd*)pGiveWnd))                    return;
    Silenced = false;

    // check detrimental buff that wont let ya perform melee actions.
    for (int b = 0; b < BuffMax; b++) {
        long SpellID = GetCharInfo2()->Buff[b].SpellID;
        if (SpellID < 1) continue;
        if (PSPELL spell = GetSpellByID(SpellID)) {
            for (int a = 0; a < GetSpellNumEffects(spell); a++) {
                switch (GetSpellAttrib(spell, a)) {
                case 22: return;          // charmed
                case 23: return;          // feared
                case 31: return;          // mesmerized
                case 40: return;          // invulnerable
                case 96: Silenced = true; // silenced
                }
            }
        }
    }

    // check detrimental song that wont let ya perform melee actions.
    for (int s = 0; s < SongMax; s++) {
        long SpellID = GetCharInfo2()->ShortBuff[s].SpellID;
        if (SpellID < 1) continue;
        if (PSPELL spell = GetSpellByID(SpellID)) {
            for (int a = 0; a < GetSpellNumEffects(spell); a++) {
                switch (GetSpellAttrib(spell, a))
                {
                case 22: return;          // charmed
                case 23: return;          // feared
                case 31: return;          // mesmerized
                case 40: return;          // invulnerable
                case 96: Silenced = true; // silenced
                }
            }
        }
    }

    // yaulp for mana, if your a cleric
    if (doYAULP && idYAULP.Ready(ifYAULP)) idYAULP.Press();

    // check our health and perform some healing action if we can
    if ((Health = GetCurHPS() * 100 / GetMaxHPS()) < 100) {
        Ability *UseThis = NULL;
        if (doMEND && Health <= doMEND && idMEND.Ready(ifMEND))                  UseThis = &idMEND;
        else if (doLAYHAND && Health <= doLAYHAND && idLAYHAND.Ready(ifLAYHAND)) UseThis = &idLAYHAND;
        else if (doPOTHEALFAST && Health <= doPOTHEALFAST && idPOTHEALFAST.Ready(ifPOTHEALFAST)) UseThis = &idPOTHEALFAST;
        else if (doPOTHEALOVER && Health <= doPOTHEALOVER && idPOTHEALOVER.Ready(ifPOTHEALOVER)) UseThis = &idPOTHEALOVER;
        if (UseThis) {
            if (UseThis->ID == idLAYHAND.ID) {
                PSPAWNINFO TargetSave = pTarget ? (PSPAWNINFO)pTarget : NULL;
                *(PSPAWNINFO*)ppTarget = SpawnMe();
                idLAYHAND.Press();
                *(PSPAWNINFO*)ppTarget = TargetSave;
            }
            else UseThis->Press();
        }
    }

    // check if we are stunned, if so we can't perform any melee actions
    if (IsStunned()) return;

    // check if we still have a killing target or we acquiring a new one
    if ((pTarget && TargetType(NPC_TYPE)) && (*EQADDR_ATTACK || onEVENT & 0x8000))
    {
        if (!MeleeTarg)
        {
            KillThis(NULL, "");
        }
        else if (!TargetID(MeleeTarg))
        {
            Override(NULL, "%s::\arTARGET SWITCH\ax taking actions.");
            return;
        }
        TimerAttk = (unsigned long)clock() + delay * 12;
    }
    if (MeleeTarg)
    {
        if (PSPAWNINFO Tar = GetSpawnID(MeleeTarg))
        {
            if (!MeleeLife || Tar->HPCurrent < MeleeLife) TimerLife = (unsigned long)clock() + 1500;
            MeleeLife = Tar->HPCurrent;
        }
    }

    // check if we should standup from an interrupted feign death
    if (BrokenFD && (unsigned long)clock()>BrokenFD) {
        BrokenFD = false;
        if (IsFeigning()) {
            Announce(SHOW_FEIGN, "%s::\arFAILED FEIGN DEATH\ax taking action!", PLUGIN_NAME);
            EzCommand("/stand");
            return;
        }
    }

    // check it's a good time to perform some downtime actions?
    //if (!IsCasting()) DowntimeHandle();
    if (!IsCasting() || BardClass) DowntimeHandle();

    // check it's a good time to drop combat?
    Ability *FeignDeath = NULL;
    if (idFEIGN[0].Ready(""))      FeignDeath = &idFEIGN[0];
    else if (idFEIGN[1].Ready("")) FeignDeath = &idFEIGN[1];
    Health = GetCurHPS() * 100 / GetMaxHPS();
    if (!doAGGRO && !(onEVENT & 0x0FF0) && !IsFeigning() && !IsInvisible()) {
        bool fTime = (doFEIGNDEATH && Health <= doFEIGNDEATH && FeignDeath);
        bool eTime = (doESCAPE     && Health <= doESCAPE     && idESCAPE.Ready(""));
        bool bTime = (doBACKOFF    && Health <= doBACKOFF);

        if (fTime || eTime || bTime) {
            if (*EQADDR_ATTACK) {
                onEVENT |= 0x0008;
                AttackOFF();
            }
            else {
                if (fTime)      onEVENT |= 0x0040;
                else if (eTime) onEVENT |= 0x0020;
                else if (bTime) onEVENT |= 0x0010;
                if (onEVENT & 0x0020)      idESCAPE.Press();
                // else if (onEVENT & 0x0040) FeignDeath->Press();
                else if (onEVENT & 0x0040 && FeignDeath) FeignDeath->Press();
            }
            return;
        }
    }

    // check it's a good time to resume combat?
    if ((IsFeigning() || IsInvisible() || onEVENT & 0x0FF0) && !(onEVENT & 0x4000))
    {
        if (!TimerBack) TimerBack = (unsigned long)clock() + delay;
        else if ((unsigned long)clock() > TimerBack && (doAGGRO || (onEVENT & 0x0FF0 && Health > doRESUME)))
        {
            if ((IsInvisible() || onEVENT & 0x0220) && (!IsInvisible() || !(onEVENT & 0x0020))) onEVENT &= 0xFDDF;
            if ((IsFeigning() || onEVENT & 0x0440) && (!IsFeigning() || (IsFeigning() && doSTAND)))
            {
                onEVENT &= 0xFBBF;
                EzCommand("/stand");
                StickReset();
                return;
            }
            onEVENT &= 0xF99F;
            TimerBack = false;
        }
    }
    // end resume

    // time to handle dummy pet, check mending, check we have target in range, etc...
    if (doPETASSIST && MeleeTarg)
    {
        PSPAWNINFO Pet = SpawnPet();
        PSPAWNINFO Tar = GetSpawnID(MeleeTarg);
        if (Pet && Tar)
        {
            if (doPETMEND && Pet->HPCurrent <= doPETMEND && idPETMEND.Ready("")) idPETMEND.Press();
            PetInDist = (!doPETRANGE || InRange(Pet, Tar, (FLOAT)doPETRANGE));
            if (!PetOnWait && PetInDist) PetOnWait = (unsigned long)clock() + doPETDELAY * 1000;
            if (PetOnWait && (unsigned long)clock() > PetOnWait && PetInDist)
            {
                if ((unsigned long)clock() > TimerLife || !IdlingArray[Tar->Animation & 0xFF])
                {
                    if ((unsigned long)clock() > PetOnAttk) PetATTK();
                }
            }
        }
    }
    // end pet

    // hold on?
    if (!MeleeTarg || !TargetID(MeleeTarg) || !IsStanding() || onEVENT & 0x0FF2) // this event is never referenced elsewhere
    {
        if (*EQADDR_ATTACK) AttackOFF();
        if (onEVENT & 0x0008) onEVENT |= 0x0008;
        return;
    }

    // target is in range? could we engage and kill it?
    // ***** hard-coded override if we are more than 250 away  ****
    if ((MeleeDist = DistanceToSpawn(SpawnMe(), (PSPAWNINFO)pTarget)) > 250)
    {
        Override(NULL, "");
        return;
    }
    // end 250 range enforce

    MeleeSpeed = fabs(FindSpeed((PSPAWNINFO)pTarget));
    MeleeBack = fabs(AngularDistance(((PSPAWNINFO)pTarget)->Heading, SpawnMe()->Heading));
    MeleeView = fabs(AngularHeading(SpawnMe(), (PSPAWNINFO)pTarget));
    MeleeFlee = (MeleeFlee || (MeleeLife <= 85 && MeleeSpeed > 25.0f && IsMobFleeing(SpawnMe(), (PSPAWNINFO)pTarget)));
    MeleeKill = ((PSPAWNINFO)pTarget)->AvatarHeight + 12.0f;
    //Sticking=Evaluate("$If[$Stick.Active},1,0]}");
    if (bMULoaded && bMUPointers) Sticking = *pbStickOn;

    // are we discing? if so time to promote some actions?
    long disc = Discipline();
    if (disc && !(onEVENT & 0x7007)) switch (disc)
    {
    case d_assassin1:
    case d_assassin2:
    case d_assassin3:
    case d_assassin4:
    case d_assassin5:
    case d_assassin6:
    case d_assassin7:
        if (doMELEE && MeleeDist < MeleeKill && MeleeView<60 && MeleeBack<doSTAB && idBACKSTAB.Ready("") && StabCheck()) StabPress(); break;
    }

    // scripted rogue sequence striking/assassination codes
    if (doASSASSINATE && doBACKSTAB && doMELEE && onSTICK>0 && !SwingHits && !TakenHits && MeleeSpeed<2.0f && !*EQADDR_ATTACK && StabCheck())
    {
        if (!Moving && Immobile)
        {
            if (!IsSneaking() && idSNEAK.Ready("")) idSNEAK.Press();
            if (!IsInvisible() && idHIDE.Ready("")) idHIDE.Press();
            if (IsSneaking() && TimeSince(SilentTimer)>1000 && IsInvisible() && TimeSince(HiddenTimer)>1000)
            {
                if (doSTRIKEMODE)    /// strikemode = 1
                {
                    strcpy_s(Reserved, StrikeCMD.c_str()); // copy our strike ini command to Reserved
                    ParseMacroData(Reserved, sizeof(Reserved)); // parse out TLO evaluations
                }
                else   // strikemode not set
                {
                    if (doSTAB > 191) sprintf_s(Reserved, "%2.2f id %d snapdist %2.2f snaproll", MeleeKill - 3.0f, MeleeTarg, MeleeKill - 3.0f); //Strike stick code to adjust for Rogue AAs
                //  else           sprintf_s(Reserved, "%2.2f id %d behindonce", MeleeKill - 3.0f, MeleeTarg); //Old default strike stick code
                    else           sprintf_s(Reserved, "%2.2f id %d snapdist %2.2f snaproll", MeleeKill - 3.0f, MeleeTarg, MeleeKill - 3.0f); //New default strike stick code htw 2/20/2011
                }
                if (!Sticking && strcmp(Reserved, StickArg))
                {
                    Stick(Reserved);
                    return;
                }
                if (MeleeDist > MeleeKill || MeleeView > 60 || MeleeBack > doSTAB)
                {
                    SwingHits++;
                }
                else if (idBACKSTAB.Ready("") && TimeSince(HiddenTimer) > 3000)
                //Original less verbose strike code
                {
                    if (Sticking) Stick("");
                    if (doSTRIKE && idSTRIKE.Ready(ifSTRIKE) && !StrikeFail) idSTRIKE.Press(); //Rogue StrikeFail Fix htw 2/20/2011
                    else StabPress();
                }
            }
        }
        if (Sticking && (SwingHits || TakenHits || !IsSneaking() || !IsInvisible())) Stick("");
        return;
    }
    // end rogue striking/assassination

    // jolting times!
    if ((doJLTKICKS || doJOLT || doSTORMBLADES) && !doAGGRO && SwingHits > doJOLT)
    {
        long MyEndu = GetCharInfo2()->Endurance * 100 / GetMaxEndurance();
        if (idJOLT.Ready(ifJOLT))
        {
            idJOLT.Press();
            SwingHits = 1;
        }
        if (idJLTKICKS.Ready(ifJLTKICKS) && (MyEndu > doJLTKICKS))
        {
            idJLTKICKS.Press();
            SwingHits = 1;
        }
        if (idSTORMBLADES.Ready(ifSTORMBLADES) && (MyEndu > doSTORMBLADES))
        {
            idSTORMBLADES.Press();
            SwingHits = 1;
        }

    }
    //end jolt

    // handle melee
    if (doMELEE && !(onEVENT & 0x8000))
    {
        if (onEVENT & 0x0008 && !(onEVENT & 0xF007) && !*EQADDR_ATTACK) AttackON();

        // start stick processing
        if (onSTICK)      // would have value if StickReset called, or set by itself
        {
            if (onSTICK > 0) // value would be positive from StickReset
            {
                // if no timer (set here) -- if (no range set or distance to target less than stick range, set timer
                if (!TimerStik) if (!doSTICKRANGE || doSTICKNORANGE || MeleeDist < doSTICKRANGE) TimerStik = (unsigned long)clock() + doSTICKDELAY * 1000;
                // if not moving and not sticking and (no delay or timer is up) and no range set or distance to target less than stick range
                // set onstick negative so below if is processed
                // *** if a range is set and we are greater than this range, this may cause those "open space" problems
                if (Immobile && !Sticking && (!doSTICKDELAY || (unsigned long)clock()>TimerStik) && (!doSTICKRANGE || doSTICKNORANGE || MeleeDist < doSTICKRANGE)) onSTICK = -1;
            }
            if (onSTICK < 0)        // set negative by above in cases where we should stick
            {
                if (doSTICKMODE)        // if we have a custom stick command, use this
                {
                    strcpy_s(Reserved, StickCMD.c_str()); // copy our ini command to Reserved
                    ParseMacroData(Reserved, sizeof(Reserved));          // parse out TLO evaluations
                }
                else       // else melee decides how to process stick as follows
                {
                    long type = Aggroed(MeleeTarg); // set to 1 if cases acceptable to attack (on hott, within range)
                    bool swim = (SpawnMe()->UnderWater == 5); // use "uw" arg if underwater
                    bool stab = (type < 1 && doBACKSTAB && doSTAB<192); // use "behind" if not aggro and strike option enabled, else !front
                    bool tank = (type>0 || (!IsInvisible() && (doAGGRO || !GetCharInfo()->pGroupInfo))); // use "moveback" if tank and aggro, else process stab
                    double dist = MeleeKill - 3.0f - (MeleeFlee*3.0f); // stick numeric parameter for distance
                    sprintf_s(Reserved, "%2.2f id %d%s%s", dist, MeleeTarg, MeleeFlee ? "" : (tank ? " moveback" : (!stab ? " !front" : " behind")), swim ? " uw" : "");
                }
                //if(strcmp(Reserved,StickArg)) Stick(Reserved); // issue command if its not the same as the last issued (stickarg set to last arg given to Stick()??
                // most likely because if we are sticking and want to change the type of stick we issue, but this will fail if stick got
                // shut off and the arg never reset properly, so lets check also if we are not sticking by checking moveutils actual status
                if (bMULoaded && bMUPointers)  // if moveutils is loaded
                {
                    if (strcmp(Reserved, StickArg) || (!doSTICKBREAK && !*pbStickOn)) Stick(Reserved);
                }
            }
        }
        // end stick processing

        // not behind enraged/infuriated target?
        if (onEVENT & 0x0003 && *EQADDR_ATTACK)
        {
            long haveAggro = Aggroed(MeleeTarg); // set to 1 if cases acceptable to attack (on hott, within range)
            if ((MeleeBack > 92 || onEVENT & 0x0002) && (haveAggro < 1))
            {
                if (MeleeBack > 92) Announce(SHOW_ENRAGING, "%s::\arNOT BEHIND\ax enraged target, taking action!", PLUGIN_NAME);
                onEVENT |= 0x0008;
                AttackOFF();
                return;
            }
        }

        // check target is in melee range?
        if (MeleeDist < MeleeKill)
        {
            // attack is off, good time for stealing/begging or evading?
            if (!*EQADDR_ATTACK)
            {
                onEVENT &= 0xBFFF;
                if (!MeleeFlee)
                {
                    if (MeleeDist < 10 && doPICKPOCKET && idPICKPOCKET.Ready(ifPICKPOCKET))
                    {
                        if (!CursorEmpty()) EzCommand("/autoinventory");
                        idPICKPOCKET.Press();
                        onEVENT |= 0x1008;
                    }
                    else if (doBEGGING && !IsInvisible() && idBEGGING.Ready(ifBEGGING))
                    {
                        idBEGGING.Press();
                        onEVENT |= 0x2008;
                    }
                    //if (doEVADE && !doAGGRO && Immobile && !IsInvisible() && idHIDE.Ready(ifEVADE))
                    //{
                    //    idHIDE.Press();
                    //    onEVENT |= 0x0208;
                    //}
                    //else if (doFALLS && !doAGGRO && FeignDeath->Ready(ifFALLS))
                    else if (doFALLS && !doAGGRO && FeignDeath && FeignDeath->Ready(ifFALLS))
                    {
                        FeignDeath->Press();
                        onEVENT |= 0x0408;
                    }
                }
                if (onEVENT & 0x0001 && !(onEVENT & 0xFFF6) && MeleeBack < 92)
                {
                    Announce(SHOW_ENRAGING, "%s::\agBEHIND\ax TARGET kicking attack ON!!!", PLUGIN_NAME);
                    onEVENT &= 0xFFF6; AttackON(); onEVENT |= 0x0009;
                }
            }
            else   // attack is on so lets do some dps?
            {
                if (doBACKSTAB && MeleeBack < doSTAB && MeleeView < 60 && idBACKSTAB.Ready(ifBACKSTAB) && StabCheck()) StabPress();
                if (doBASH && idBASH.Ready(ifBASH) && BashCheck()) BashPress();
                if (doSLAM && idSLAM.Ready(ifSLAM)) idSLAM.Press();
                if (doFRENZY && idFRENZY.Ready(ifFRENZY)) idFRENZY.Press();
                if (doKICK && idKICK.Ready(ifKICK)) idKICK.Press();
                if (doEVADE && !doAGGRO && !IsInvisible() && idHIDE.Ready(ifEVADE)) idHIDE.Press();
                if (!disc && !IsInvisible() && !MeleeFlee && onEVENT & 0x4000 && *EQADDR_ATTACK)  AttackOFF();
                if (!disc && Immobile && !IsInvisible() && !MeleeFlee)
                {
                    if (MeleeDist < 10 && doPICKPOCKET   && idPICKPOCKET.Ready(ifPICKPOCKET))                    onEVENT |= 0x4008;
                    if (doBEGGING      && idBEGGING.Ready(ifBEGGING))                          onEVENT |= 0x4008;
                    // if (doFALLS        && !doAGGRO && FeignDeath->Ready(ifFALLS))              onEVENT |= 0x4008;
                    if (doFALLS        && !doAGGRO && FeignDeath && FeignDeath->Ready(ifFALLS))              onEVENT |= 0x4008;
                    // Hide/evade no longer requires attack be turned off
                    // if (doEVADE        && !(doAGGRO || !IsGrouped()) && idHIDE.Ready(ifEVADE)) onEVENT |= 0x4008;
                    // if (onEVENT & 0x4000 && *EQADDR_ATTACK) AttackOFF();
                }

                if (MeleeDist < 15)
                {
                    if (doDISARM && idDISARM.Ready(ifDISARM)) idDISARM.Press();
                }
            }
            if (doINTIMIDATION && idINTIMIDATION.Ready(ifINTIMIDATION)) idINTIMIDATION.Press();
            if (doTAUNT && doAGGRO && idTAUNT.Ready(ifTAUNT)) idTAUNT.Press();
        }
        //end target is in range

        if (MeleeFlee) SneakOFF();
        if (onEVENT & 0x3000) onEVENT &= 0xCFFF;
    }
    // end handle melee

    // handle ranged?
    if (doRANGE || onEVENT & 0x8000)
    {
        // should we face target? not moving, stopped >2sec, facing>2sec and not sticking?
        if (doFACING && Immobile && (unsigned long)clock() > TimerFace && !Sticking)
        {
            if (MeleeView > 30) Face(SpawnMe(), "nolook");
            TimerFace = (unsigned long)clock() + delay * 8;
        }
        // are we in good ranged for ranged?
        if (MeleeDist<(doRANGE ? doRANGE : 250) && MeleeDist>35 && MeleeDist > MeleeKill + 20)
        {
            if (!AutoFire && gbRangedAttackReady) ThrowIT(NULL, "");
            if (*EQADDR_ATTACK)
            {
                if (Sticking) Stick("");
                onEVENT |= 0x8000;
                AttackOFF();
                Announce(SHOW_SWITCHING, "%s::Switching [\ayRange\ax].", PLUGIN_NAME);
            }
        }
        else if (!*EQADDR_ATTACK) // target too close? or too far?
        {
            if (AutoFire)
            {
                EzCommand("/autofire");
                AutoFire = false;
            }
            if (Immobile && onEVENT & 0x8000)
            {
                onEVENT &= 0x7FF7;
                if (doMELEE)
                {
                    StickReset();
                    onEVENT |= 0x0008;
                    AttackON();
                    Announce(SHOW_SWITCHING, "%s::Switching [\ayMelee\ax].", PLUGIN_NAME);
                }
            }
        }
    }

    // time to handle spell casting?
    if (!TargetID(MeleeTarg) || MeleeDist > 200) return;
    long MyEndu = GetCharInfo2()->Endurance * 100 / GetMaxEndurance();

    // slap in the face needs to happen before combat starts
    if (doSLAPFACE && MyEndu > doSLAPFACE && idSLAPFACE.Ready(ifSLAPFACE)) idSLAPFACE.Press();

    // should we stun that target?
    if (doSTUNNING && MeleeLife <= doSTUNNING)
    {
        Ability *UseThis = NULL;
        if (idSTUN[0].Ready(ifSTUNNING))      UseThis = &idSTUN[0];
        else if (idSTUN[1].Ready(ifSTUNNING)) UseThis = &idSTUN[1];
        if (UseThis)
        {
            Announce(SHOW_STUNNING, "%s::Stunning [\ay%s\ax].", PLUGIN_NAME, MeleeName);
            UseThis->Press();
        }
    }

    // are we grouped?
    if (IsGrouped())
    {
        // Time to build and maintain aggro?
        if (doAGGRO)
        {
            long HaveAggro = Aggroed(MeleeTarg);

            // should we challenge for to maintain aggro over time?
            if (doCHALLENGEFOR && onCHALLENGEFOR && HaveAggro == 1 && idCHALLENGEFOR.Ready(ifCHALLENGEFOR)) {
                Announce(SHOW_PROVOKING, "%s::Challenging [\ay%s\ax].", PLUGIN_NAME, MeleeName);
                idCHALLENGEFOR.Press();
                onCHALLENGEFOR--;
            }

              // should we challenge for to maintain aggro over time?
            if (doENRAGINGKICK && onENRAGINGKICK && HaveAggro == 1 && idENRAGINGKICK.Ready(ifENRAGINGKICK)) {
                Announce(SHOW_PROVOKING, "%s::Enraging Kick [\ay%s\ax].", PLUGIN_NAME, MeleeName);
                idENRAGINGKICK.Press();
                onENRAGINGKICK--;
            }

            // should we provoke at least once and/or when aggro is lost?
            if (onBELOW && MeleeLife > doPROVOKEEND && MeleeDist < 100 && (HaveAggro < 1 || (doPROVOKEONCE && onBELOW == doPROVOKEMAX)))
            {
                Ability *UseThis = NULL;
                if (idPROVOKE[0].Ready(ifPROVOKE))      UseThis = &idPROVOKE[0];
                else if (idPROVOKE[1].Ready(ifPROVOKE)) UseThis = &idPROVOKE[1];
                if (UseThis)
                {
                    Announce(SHOW_PROVOKING, "%s::Provoking [\ay%s\ax].", PLUGIN_NAME, MeleeName);
                    UseThis->Press();
                    onBELOW--;
                }
            }
        }

        // should we use short duration melee buff?
        if (GetCharInfo2()->Endurance > 200)
        {
            if (doBLOODLUST  && MyEndu > doBLOODLUST && idBLOODLUST.Ready(ifBLOODLUST) && MeleeDist < 50) idBLOODLUST.Press();
            if (doCOMMANDING && MyEndu > doCOMMANDING && idCOMMANDING.Ready(ifCOMMANDING)) idCOMMANDING.Press();
            if (doFISTSOFWU  && MyEndu > doFISTSOFWU  && idFISTSOFWU.Ready(ifFISTSOFWU))   idFISTSOFWU.Press();
            if (doCRYHAVOC   && MyEndu > doCRYHAVOC   && idCRYHAVOC.Ready(ifCRYHAVOC) && disc != d_cleaverage1 && disc != d_cleaverage2 && disc != d_cleaverage4 && disc != d_cleaverage4 && disc != d_cleaverage5) idCRYHAVOC.Press();
            if (doTHIEFEYE   && MyEndu > doTHIEFEYE   && idTHIEFEYE.Ready(ifTHIEFEYE))     idTHIEFEYE.Press();
        }
    }
    // If berserker do discs
    if (BerserkerClass && PressDelay < MQGetTickCount64())
    {
        Ability *UseThis = NULL;
        PressDelay = 0;
        if (doCRIPPLE && MyEndu > doCRIPPLE && idCRIPPLE.Ready(ifCRIPPLE) && MeleeDist < 175) UseThis = &idCRIPPLE;
        else if (doRAGEVOLLEY && MyEndu > doRAGEVOLLEY && idRAGEVOLLEY.Ready(ifRAGEVOLLEY) && MeleeDist < 175)  UseThis = &idRAGEVOLLEY;
        else if (doVIGAXE && MyEndu > doVIGAXE && idVIGAXE.Ready(ifVIGAXE)) UseThis = &idVIGAXE;
        else if (doRALLOS && MyEndu > doRALLOS && idRALLOS.Ready(ifRALLOS) && MeleeDist < 50) UseThis = &idRALLOS;
        else if (doOPFRENZY && MyEndu > doOPFRENZY && idOPFRENZY.Ready(ifOPFRENZY)) UseThis = &idOPFRENZY;
        if (UseThis)
        {
            UseThis->Press();
            PressDelay = MQGetTickCount64() + 1000;
        }
    }
    // If Monk do discs and attacks
    if (MonkClass && PressDelay < MQGetTickCount64())
    {
        Ability *UseThis = NULL;
        PressDelay = 0;
        if (!disc && doMONKEY && MyEndu > doMONKEY && idMONKEY.Ready(ifMONKEY) && MeleeDist < 50) UseThis = &idMONKEY;
        else if (doLEOPARDCLAW && MyEndu > doLEOPARDCLAW && idLEOPARDCLAW.Ready(ifLEOPARDCLAW)) UseThis = &idLEOPARDCLAW;
        else if (doVIGSHURIKEN && MyEndu > doVIGSHURIKEN && idVIGSHURIKEN.Ready(ifVIGSHURIKEN)) UseThis = &idVIGSHURIKEN;
        else if (doDRAGONPUNCH && idDRAGONPUNCH.Ready(ifDRAGONPUNCH)) UseThis = &idDRAGONPUNCH;
        else if (doEAGLESTRIKE && idEAGLESTRIKE.Ready(ifEAGLESTRIKE)) UseThis = &idEAGLESTRIKE;
        else if (doTIGERCLAW && idTIGERCLAW.Ready(ifTIGERCLAW)) UseThis = &idTIGERCLAW;
        else if (doROUNDKICK && idROUNDKICK.Ready(ifROUNDKICK)) UseThis = &idROUNDKICK;
        else if (doSYNERGY && MyEndu > doSYNERGY && idSYNERGY.Ready(ifSYNERGY)) UseThis = &idSYNERGY;
        else if (doFLYINGKICK && idFLYINGKICK.Ready(ifFLYINGKICK)) UseThis = &idFLYINGKICK;
        else if (doCLOUD && MyEndu > doCLOUD && idCLOUD.Ready(ifCLOUD))  UseThis = &idCLOUD;
        if (UseThis)
        {
            UseThis->Press();
            PressDelay = MQGetTickCount64() + 500;
        }
    }
    // is target close enough for those?
    if (MeleeDist < 50)
    {
        // ON/OFF switch
        if (doBATTLELEAP             && idBATTLELEAP.Ready(ifBATTLELEAP))                    idBATTLELEAP.Press();
        if (doASP                    && idASP.Ready(ifASP))                                  idASP.Press();
        if (doBANESTRIKE             && idBANESTRIKE.Ready(ifBANESTRIKE))                    idBANESTRIKE.Press();
        if (doBOASTFUL               && idBOASTFUL.Ready(ifBOASTFUL))                        idBOASTFUL.Press();
        if (doHARMTOUCH              && idHARMTOUCH.Ready(ifHARMTOUCH))                      idHARMTOUCH.Press();
        if (doTHROATJAB              && idTHROATJAB.Ready(ifTHROATJAB))                      idTHROATJAB.Press();
        if (doCALLCHALLENGE          && idCALLCHALLENGE.Ready(ifCALLCHALLENGE))              idCALLCHALLENGE.Press();
        if (doCSTRIKE                && idCSTRIKE.Ready(ifCSTRIKE))                          idCSTRIKE.Press();
        if (doSELOS                  && idSELOS.Ready(ifSELOS))                              idSELOS.Press();
        if (doEYEGOUGE               && idEYEGOUGE.Ready(ifEYEGOUGE))                        idEYEGOUGE.Press();
        if (doFERALSWIPE             && idFERALSWIPE.Ready(ifFERALSWIPE))                    idFERALSWIPE.Press();
        if (doGORILLASMASH           && idGORILLASMASH.Ready(ifGORILLASMASH))                idGORILLASMASH.Press();
        if (doGUTPUNCH               && idGUTPUNCH.Ready(ifGUTPUNCH))                        idGUTPUNCH.Press();
        if (doJOLT                   && idJOLT.Ready(ifJOLT))                                idJOLT.Press();
        if (doKNEESTRIKE             && idKNEESTRIKE.Ready(ifKNEESTRIKE))                    idKNEESTRIKE.Press();
        if (doRAVENS                 && idRAVENS.Ready(ifRAVENS))                            idRAVENS.Press();
        if (doSTEELY                 && idSTEELY.Ready(ifSTEELY))                            idSTEELY.Press();

        // Endurance above disc switch
        //Rogue begin
        if (doPINPOINT && MyEndu > doPINPOINT && idPINPOINT.Ready(ifPINPOINT)) idPINPOINT.Press();
        if (doKNIFEPLAY && idKNIFEPLAY.Ready(ifKNIFEPLAY)) idKNIFEPLAY.Press();
        if (doVIGDAGGER && MyEndu > doVIGDAGGER && idVIGDAGGER.Ready(ifVIGDAGGER)) idVIGDAGGER.Press();
        if (doTWISTEDSHANK && idTWISTEDSHANK.Ready(ifTWISTEDSHANK)) idTWISTEDSHANK.Press();
        if (doJUGULAR && MyEndu > doJUGULAR && idJUGULAR.Ready(ifJUGULAR)) idJUGULAR.Press();
        if (doASSAULT && MyEndu > doASSAULT && idASSAULT.Ready(ifASSAULT)) idASSAULT.Press();
        if (doBLEED && MyEndu > doBLEED && idBLEED.Ready(ifBLEED)) idBLEED.Press();
        // end Rogue

        // Endurance above disc switch
        //Ranger begin
        if (doJLTKICKS && MyEndu > doJLTKICKS && idJLTKICKS.Ready(ifJLTKICKS)) idJLTKICKS.Press();
        if (doSTORMBLADES && MyEndu > doSTORMBLADES && idSTORMBLADES.Ready(ifSTORMBLADES)) idSTORMBLADES.Press();
        //if (doFEROCIOUSKICK && idFEROCIOUSKICK.Ready(ifFEROCIOUSKICK)) idFEROCIOUSKICK.Press();
        // end Ranger

        if (doBVIVI       && MyEndu > doBVIVI          && idBVIVI.Ready(ifBVIVI))              idBVIVI.Press();
        if (doDEFENSE     && MyEndu > doDEFENSE        && idDEFENSE.Ready(ifDEFENSE))          idDEFENSE.Press();
        if (doFCLAW       && MyEndu > doFCLAW          && idFCLAW.Ready(ifFCLAW))              idFCLAW.Press();
        if (doFIELDARM    && MyEndu > doFIELDARM       && idFIELDARM.Ready(ifFIELDARM))        idFIELDARM.Press();
        if (doGBLADE       && MyEndu> doGBLADE         && idGBLADE.Ready(ifGBLADE))            idGBLADE.Press();         
        if (doRAKE        && MyEndu > doRAKE           && idRAKE.Ready(ifRAKE))                idRAKE.Press();
        if (doRIGHTIND    && MyEndu > doRIGHTIND       && idRIGHTIND.Ready(ifRIGHTIND))        idRIGHTIND.Press();
        if (doTHROWSTONE  && MyEndu > doTHROWSTONE     && idTHROWSTONE.Ready(ifTHROWSTONE))    idTHROWSTONE.Press();
        if (doWITHSTAND   && MyEndu > doWITHSTAND      && idWITHSTAND.Ready(ifWITHSTAND))      idWITHSTAND.Press();
        // Opportunistic Strike check if mob health below 20%
        if (doOPPORTUNISTICSTRIKE && MyEndu > doOPPORTUNISTICSTRIKE && MeleeLife <= 20 && idOPPORTUNISTICSTRIKE.Ready(ifOPPORTUNISTICSTRIKE)) idOPPORTUNISTICSTRIKE.Press();
    }

    // time to handle holy shit?
    if (HOLYSHITIF.length()<5 || Evaluate((char*)("${If[" + HOLYSHITIF.substr(5, HOLYSHITIF.length() - 6) + ",1,0]}").c_str())) {

        if (HOLYSHIT[doHOLY].size()) {
            switch (doHOLYFLAG[doHOLY])
            {
            case 1://not a macro dependant flag
            {
                EzCommand((char*)HOLYSHIT[doHOLY].c_str());
                break;
            }
            case 2://a macro must be running and the variable MUST exist
            {
                if (gRunning) {
                    if (OKtoParseShit(HOLYSHIT[doHOLY])) {
                        EzCommand((char*)HOLYSHIT[doHOLY].c_str());
                    }
                }
                break;
            }
            default:
                break;
            }
        }
        for (long x = 1; x < 61; x++)
        {
            long n = (x + doHOLY) % 60;
            if (doHOLYFLAG[n])
            {
                doHOLY = n;
                break;
            }
        }
    }
}

void KeyMelee(char* NAME, int Down)
{
    if (Down && pTarget)
    {
        if (!doSKILL) EzCommand("/keypress AUTOPRIM");
        else if (!MeleeTarg) KillThis(NULL, "");
        else Override(NULL, "%s::\arOVERRIDE\ax taking actions!");
    }
}

void KeyRange(char* NAME, int Down)
{
    if (Down && pTarget)
    {
        if (!doSKILL) EzCommand("/keypress RANGED");
        else ThrowIT(NULL, "");
    }
}

void Bindding(bool BindMode)
{
    if (BindMode)
    {
        if (!Binded)
        {
            KeyCombo  MeleeCombo, RangeCombo;
            RemoveMQ2KeyBind("MELEE"); AddMQ2KeyBind("MELEE", KeyMelee); ParseKeyCombo(MeleeKey, MeleeCombo); SetMQ2KeyBind("MELEE", false, MeleeCombo);
            RemoveMQ2KeyBind("RANGE"); AddMQ2KeyBind("RANGE", KeyRange); ParseKeyCombo(RangeKey, RangeCombo); SetMQ2KeyBind("RANGE", false, RangeCombo);
            Binded = true;
        }
    }
    else if (Binded)
    {
        RemoveMQ2KeyBind("MELEE");
        RemoveMQ2KeyBind("RANGE");
        Binded = false;
    }
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

void __stdcall AUTOFIREOFF(unsigned int ID, void *pData, PBLECHVALUE pValues) {
    AutoFire = false;
    if (AutoFire) KeyMelee("", true);
}

void __stdcall AUTOFIREON(unsigned int ID, void *pData, PBLECHVALUE pValues) {
    if (pTarget && TargetType(NPC_TYPE) && InGame()) AutoFire = true;
}

void __stdcall CASTING(unsigned int ID, void *pData, PBLECHVALUE pValues) {
    if (doSKILL && MeleeTarg && !_strnicmp(pValues->Value, MeleeName, MeleeSize)) MeleeCast = (unsigned long)clock();
}

void __stdcall ENRAGEON(unsigned int ID, void *pData, PBLECHVALUE pValues) {
    if (MeleeTarg && !_strnicmp(pValues->Value, MeleeName, MeleeSize)) EnrageON(NULL, "");
}

void __stdcall ENRAGEOFF(unsigned int ID, void *pData, PBLECHVALUE pValues) {
    if (MeleeTarg && !_strnicmp(pValues->Value, MeleeName, MeleeSize)) EnrageOFF(NULL, "");
}

void __stdcall INFURIATEON(unsigned int ID, void *pData, PBLECHVALUE pValues) {
    if (MeleeTarg && !_strnicmp(pValues->Value, MeleeName, MeleeSize)) InfuriateON(NULL, "");
}

void __stdcall INFURIATEOFF(unsigned int ID, void *pData, PBLECHVALUE pValues) {
    if (MeleeTarg && !_strnicmp(pValues->Value, MeleeName, MeleeSize)) InfuriateOFF(NULL, "");
}

void __stdcall PETATTK(unsigned int ID, void *pData, PBLECHVALUE pValues) {
    PetSEEN();
}

void __stdcall PETBACK(unsigned int ID, void *pData, PBLECHVALUE pValues) {
   if (HaveGHold && doPETASSIST)
    {
        Announce(SHOW_CONTROL, "%s::Command [\ay%s\ax].", PLUGIN_NAME, "/pet ghold on");
        EzCommand("/pet ghold on");
        if (PetOnAttk) PetOnAttk = (unsigned long)clock() + 1000;
        PetOnHold = true;
    }
    else
    {
        if (HaveHold && doPETASSIST)
        {
            Announce(SHOW_CONTROL, "%s::Command [\ay%s\ax].", PLUGIN_NAME, "/pet hold on");
            EzCommand("/pet hold on");
            if (PetOnAttk) PetOnAttk = (unsigned long)clock() + 1000;
            PetOnHold = true;
        }
    }
}
void __stdcall PETHOLD(unsigned int ID, void *pData, PBLECHVALUE pValues) {
    PetOnHold = true;
}

void __stdcall FALLEN(unsigned int ID, void *pData, PBLECHVALUE pValues) {
    if (!doSKILL || ((long)pData && _strnicmp(pValues->Value, GetCharInfo()->Name, strlen(GetCharInfo()->Name) + 1))) return;
    Announce(SHOW_FEIGN, "%s::\arFAILED FEIGN DEATH\ax taking action!", PLUGIN_NAME);
    EzCommand("/stand");
}

void __stdcall BROKEN(unsigned int ID, void *pData, PBLECHVALUE pValues) {
    if (doSKILL) BrokenFD = (unsigned long)clock() + 1;
}

void __stdcall RESUME(unsigned int ID, void *pData, PBLECHVALUE pValues) {
    if (doSKILL) BrokenFD = 0;
}

void __stdcall SNEAKON(unsigned int ID, void *pData, PBLECHVALUE pValues) {
    SilentTimer = (unsigned long)clock();
}

void __stdcall SNEAKOFF(unsigned int ID, void *pData, PBLECHVALUE pValues) {
    SilentTimer = 0;
}

void __stdcall HIDEON(unsigned int ID, void *pData, PBLECHVALUE pValues) {
    HiddenTimer = (unsigned long)clock();
}

void __stdcall HIDEOFF(unsigned int ID, void *pData, PBLECHVALUE pValues) {
    HiddenTimer = 0;
}
//Rogue Strike Fix htw 2/20/2011
void __stdcall STRIKERESET(unsigned int ID, void *pData, PBLECHVALUE pValues)
{
    StrikeFail = true;
}
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

PLUGIN_API void InitializePlugin()
{
    NPC_TYPE = GetPrivateProfileInt("Settings", "SpawnType", 0x000A, INIFileName);
    GetPrivateProfileString("Settings", "MeleeKeys", "z", MeleeKey, sizeof(MeleeKey), INIFileName);
    GetPrivateProfileString("Settings", "RangeKeys", "x", RangeKey, sizeof(RangeKey), INIFileName);
    SETBINDINGS = GetPrivateProfileInt("Settings", "Bindings", 1, INIFileName);

    CmdListe.clear();
    REGISTER_ABILITY_OPTION(pDEBUG, NULL, &doDEBUG);
    REGISTER_ABILITY_OPTION(pAGGRO, AggroReset, &doAGGRO);
    REGISTER_ABILITY_OPTION(pAGGRP, NULL, &elAGGROPRI);
    REGISTER_ABILITY_OPTION(pAGGRS, NULL, &elAGGROSEC);
    REGISTER_ABILITY_OPTION(pARROW, NULL, &elARROWS);
    REGISTER_ABILITY_OPTION(pASSLT, NULL, &doASSAULT);
    REGISTER_ABILITY_OPTION(pASSAS, NULL, &doASSASSINATE);
    REGISTER_ABILITY_OPTION(pBANST, NULL, &doBANESTRIKE);
    REGISTER_ABILITY_OPTION(pBASHS, NULL, &doBASH);
    REGISTER_ABILITY_OPTION(pBBLOW, NULL, &doBOASTFUL);
    REGISTER_ABILITY_OPTION(pBGING, NULL, &doBEGGING);
    REGISTER_ABILITY_OPTION(pBKOFF, NULL, &doBACKOFF);
    REGISTER_ABILITY_OPTION(pBLEED, NULL, &doBLEED);
    REGISTER_ABILITY_OPTION(pBLUST, NULL, &doBLOODLUST);
    REGISTER_ABILITY_OPTION(pBOWID, NULL, &elRANGED);
    REGISTER_ABILITY_OPTION(pBSTAB, NULL, &doBACKSTAB);
    REGISTER_ABILITY_OPTION(pBTASP, NULL, &doASP);
    REGISTER_ABILITY_OPTION(pBTLLP, NULL, &doBATTLELEAP);
    REGISTER_ABILITY_OPTION(pBVIVI, NULL, &doBVIVI);
    REGISTER_ABILITY_OPTION(pCALLC, NULL, &doCALLCHALLENGE);
    REGISTER_ABILITY_OPTION(pCFIST, NULL, &doCLOUD);
    REGISTER_ABILITY_OPTION(pCHAMS, NULL, &doCSTRIKE);
    REGISTER_ABILITY_OPTION(pCHFOR, NULL, &doCHALLENGEFOR);
    REGISTER_ABILITY_OPTION(pCOMMG, NULL, &doCOMMANDING);
    REGISTER_ABILITY_OPTION(pCRIPS, NULL, &doCRIPPLE);
    REGISTER_ABILITY_OPTION(pCRYHC, NULL, &doCRYHAVOC);
    REGISTER_ABILITY_OPTION(pDEFEN, NULL, &doDEFENSE);
    REGISTER_ABILITY_OPTION(pDISRM, NULL, &doDISARM);
    REGISTER_ABILITY_OPTION(pDMONK, NULL, &doMONKEY);
    REGISTER_ABILITY_OPTION(pDRPNC, NULL, &doDRAGONPUNCH);
    REGISTER_ABILITY_OPTION(pDWNF0, NULL, &doDOWNFLAG[0]);
    REGISTER_ABILITY_OPTION(pDWNF1, NULL, &doDOWNFLAG[1]);
    REGISTER_ABILITY_OPTION(pDWNF2, NULL, &doDOWNFLAG[2]);
    REGISTER_ABILITY_OPTION(pDWNF3, NULL, &doDOWNFLAG[3]);
    REGISTER_ABILITY_OPTION(pDWNF4, NULL, &doDOWNFLAG[4]);
    REGISTER_ABILITY_OPTION(pDWNF5, NULL, &doDOWNFLAG[5]);
    REGISTER_ABILITY_OPTION(pDWNF6, NULL, &doDOWNFLAG[6]);
    REGISTER_ABILITY_OPTION(pDWNF7, NULL, &doDOWNFLAG[7]);
    REGISTER_ABILITY_OPTION(pDWNF8, NULL, &doDOWNFLAG[8]);
    REGISTER_ABILITY_OPTION(pDWNF9, NULL, &doDOWNFLAG[9]);
    REGISTER_ABILITY_OPTION(pDWNF10, NULL, &doDOWNFLAG[10]);
    REGISTER_ABILITY_OPTION(pDWNF11, NULL, &doDOWNFLAG[11]);
    REGISTER_ABILITY_OPTION(pDWNF12, NULL, &doDOWNFLAG[12]);
    REGISTER_ABILITY_OPTION(pDWNF13, NULL, &doDOWNFLAG[13]);
    REGISTER_ABILITY_OPTION(pDWNF14, NULL, &doDOWNFLAG[14]);
    REGISTER_ABILITY_OPTION(pDWNF15, NULL, &doDOWNFLAG[15]);
    REGISTER_ABILITY_OPTION(pDWNF16, NULL, &doDOWNFLAG[16]);
    REGISTER_ABILITY_OPTION(pDWNF17, NULL, &doDOWNFLAG[17]);
    REGISTER_ABILITY_OPTION(pDWNF18, NULL, &doDOWNFLAG[18]);
    REGISTER_ABILITY_OPTION(pDWNF19, NULL, &doDOWNFLAG[19]);
    REGISTER_ABILITY_OPTION(pDWNF20, NULL, &doDOWNFLAG[20]);
    REGISTER_ABILITY_OPTION(pDWNF21, NULL, &doDOWNFLAG[21]);
    REGISTER_ABILITY_OPTION(pDWNF22, NULL, &doDOWNFLAG[22]);
    REGISTER_ABILITY_OPTION(pDWNF23, NULL, &doDOWNFLAG[23]);
    REGISTER_ABILITY_OPTION(pDWNF24, NULL, &doDOWNFLAG[24]);
    REGISTER_ABILITY_OPTION(pDWNF25, NULL, &doDOWNFLAG[25]);
    REGISTER_ABILITY_OPTION(pDWNF26, NULL, &doDOWNFLAG[26]);
    REGISTER_ABILITY_OPTION(pDWNF27, NULL, &doDOWNFLAG[27]);
    REGISTER_ABILITY_OPTION(pDWNF28, NULL, &doDOWNFLAG[28]);
    REGISTER_ABILITY_OPTION(pDWNF29, NULL, &doDOWNFLAG[29]);
    REGISTER_ABILITY_OPTION(pDWNF30, NULL, &doDOWNFLAG[30]);
    REGISTER_ABILITY_OPTION(pDWNF31, NULL, &doDOWNFLAG[31]);
    REGISTER_ABILITY_OPTION(pDWNF32, NULL, &doDOWNFLAG[32]);
    REGISTER_ABILITY_OPTION(pDWNF33, NULL, &doDOWNFLAG[33]);
    REGISTER_ABILITY_OPTION(pDWNF34, NULL, &doDOWNFLAG[34]);
    REGISTER_ABILITY_OPTION(pDWNF35, NULL, &doDOWNFLAG[35]);
    REGISTER_ABILITY_OPTION(pDWNF36, NULL, &doDOWNFLAG[36]);
    REGISTER_ABILITY_OPTION(pDWNF37, NULL, &doDOWNFLAG[37]);
    REGISTER_ABILITY_OPTION(pDWNF38, NULL, &doDOWNFLAG[38]);
    REGISTER_ABILITY_OPTION(pDWNF39, NULL, &doDOWNFLAG[39]);
    REGISTER_ABILITY_OPTION(pDWNF40, NULL, &doDOWNFLAG[40]);
    REGISTER_ABILITY_OPTION(pDWNF41, NULL, &doDOWNFLAG[41]);
    REGISTER_ABILITY_OPTION(pDWNF42, NULL, &doDOWNFLAG[42]);
    REGISTER_ABILITY_OPTION(pDWNF43, NULL, &doDOWNFLAG[43]);
    REGISTER_ABILITY_OPTION(pDWNF44, NULL, &doDOWNFLAG[44]);
    REGISTER_ABILITY_OPTION(pDWNF45, NULL, &doDOWNFLAG[45]);
    REGISTER_ABILITY_OPTION(pDWNF46, NULL, &doDOWNFLAG[46]);
    REGISTER_ABILITY_OPTION(pDWNF47, NULL, &doDOWNFLAG[47]);
    REGISTER_ABILITY_OPTION(pDWNF48, NULL, &doDOWNFLAG[48]);
    REGISTER_ABILITY_OPTION(pDWNF49, NULL, &doDOWNFLAG[49]);
    REGISTER_ABILITY_OPTION(pDWNF50, NULL, &doDOWNFLAG[50]);
    REGISTER_ABILITY_OPTION(pDWNF51, NULL, &doDOWNFLAG[51]);
    REGISTER_ABILITY_OPTION(pDWNF52, NULL, &doDOWNFLAG[52]);
    REGISTER_ABILITY_OPTION(pDWNF53, NULL, &doDOWNFLAG[53]);
    REGISTER_ABILITY_OPTION(pDWNF54, NULL, &doDOWNFLAG[54]);
    REGISTER_ABILITY_OPTION(pDWNF55, NULL, &doDOWNFLAG[55]);
    REGISTER_ABILITY_OPTION(pDWNF56, NULL, &doDOWNFLAG[56]);
    REGISTER_ABILITY_OPTION(pDWNF57, NULL, &doDOWNFLAG[57]);
    REGISTER_ABILITY_OPTION(pDWNF58, NULL, &doDOWNFLAG[58]);
    REGISTER_ABILITY_OPTION(pDWNF59, NULL, &doDOWNFLAG[59]);
    REGISTER_ABILITY_OPTION(pDWNF60, NULL, &doDOWNFLAG[60]);
    REGISTER_ABILITY_OPTION(pEAGLE, NULL, &doEAGLESTRIKE);
    REGISTER_ABILITY_OPTION(pERAGE, NULL, &doENRAGE);
    REGISTER_ABILITY_OPTION(pERKCK, NULL, &doENRAGINGKICK);
    REGISTER_ABILITY_OPTION(pESCAP, NULL, &doESCAPE);
    REGISTER_ABILITY_OPTION(pEVADE, NULL, &doEVADE);
    REGISTER_ABILITY_OPTION(pEYEGO, NULL, &doEYEGOUGE);
    REGISTER_ABILITY_OPTION(pFACES, NULL, &doFACING);
    REGISTER_ABILITY_OPTION(pFALLS, NULL, &doFALLS);
    REGISTER_ABILITY_OPTION(pFCLAW, NULL, &doFCLAW);
    REGISTER_ABILITY_OPTION(pFEIGN, NULL, &doFEIGNDEATH);
    REGISTER_ABILITY_OPTION(pFERAL, NULL, &doFERALSWIPE);
    REGISTER_ABILITY_OPTION(pFIELD, NULL, &doFIELDARM);
    REGISTER_ABILITY_OPTION(pFISTS, NULL, &doFISTSOFWU);
    //REGISTER_ABILITY_OPTION(pFKICK, NULL, &doFEROCIOUSKICK);
    REGISTER_ABILITY_OPTION(pFLYKC, NULL, &doFLYINGKICK);
    REGISTER_ABILITY_OPTION(pFORAG, NULL, &doFORAGE);
    REGISTER_ABILITY_OPTION(pFRENZ, NULL, &doFRENZY);
    REGISTER_ABILITY_OPTION(pGBLDE, NULL, &doGBLADE);
    REGISTER_ABILITY_OPTION(pGORSM, NULL, &doGORILLASMASH);
    REGISTER_ABILITY_OPTION(pGTPUN, NULL, &doGUTPUNCH);
    REGISTER_ABILITY_OPTION(pHARMT, NULL, &doHARMTOUCH);
    REGISTER_ABILITY_OPTION(pHFAST, NULL, &doPOTHEALFAST);
    REGISTER_ABILITY_OPTION(pHIDES, NULL, &doHIDE);
    REGISTER_ABILITY_OPTION(pHOLF0, NULL, &doHOLYFLAG[0]);
    REGISTER_ABILITY_OPTION(pHOLF1, NULL, &doHOLYFLAG[1]);
    REGISTER_ABILITY_OPTION(pHOLF2, NULL, &doHOLYFLAG[2]);
    REGISTER_ABILITY_OPTION(pHOLF3, NULL, &doHOLYFLAG[3]);
    REGISTER_ABILITY_OPTION(pHOLF4, NULL, &doHOLYFLAG[4]);
    REGISTER_ABILITY_OPTION(pHOLF5, NULL, &doHOLYFLAG[5]);
    REGISTER_ABILITY_OPTION(pHOLF6, NULL, &doHOLYFLAG[6]);
    REGISTER_ABILITY_OPTION(pHOLF7, NULL, &doHOLYFLAG[7]);
    REGISTER_ABILITY_OPTION(pHOLF8, NULL, &doHOLYFLAG[8]);
    REGISTER_ABILITY_OPTION(pHOLF9, NULL, &doHOLYFLAG[9]);
    REGISTER_ABILITY_OPTION(pHOLF10, NULL, &doHOLYFLAG[10]);
    REGISTER_ABILITY_OPTION(pHOLF11, NULL, &doHOLYFLAG[11]);
    REGISTER_ABILITY_OPTION(pHOLF12, NULL, &doHOLYFLAG[12]);
    REGISTER_ABILITY_OPTION(pHOLF13, NULL, &doHOLYFLAG[13]);
    REGISTER_ABILITY_OPTION(pHOLF14, NULL, &doHOLYFLAG[14]);
    REGISTER_ABILITY_OPTION(pHOLF15, NULL, &doHOLYFLAG[15]);
    REGISTER_ABILITY_OPTION(pHOLF16, NULL, &doHOLYFLAG[16]);
    REGISTER_ABILITY_OPTION(pHOLF17, NULL, &doHOLYFLAG[17]);
    REGISTER_ABILITY_OPTION(pHOLF18, NULL, &doHOLYFLAG[18]);
    REGISTER_ABILITY_OPTION(pHOLF19, NULL, &doHOLYFLAG[19]);
    REGISTER_ABILITY_OPTION(pHOLF20, NULL, &doHOLYFLAG[20]);
    REGISTER_ABILITY_OPTION(pHOLF21, NULL, &doHOLYFLAG[21]);
    REGISTER_ABILITY_OPTION(pHOLF22, NULL, &doHOLYFLAG[22]);
    REGISTER_ABILITY_OPTION(pHOLF23, NULL, &doHOLYFLAG[23]);
    REGISTER_ABILITY_OPTION(pHOLF24, NULL, &doHOLYFLAG[24]);
    REGISTER_ABILITY_OPTION(pHOLF25, NULL, &doHOLYFLAG[25]);
    REGISTER_ABILITY_OPTION(pHOLF26, NULL, &doHOLYFLAG[26]);
    REGISTER_ABILITY_OPTION(pHOLF27, NULL, &doHOLYFLAG[27]);
    REGISTER_ABILITY_OPTION(pHOLF28, NULL, &doHOLYFLAG[28]);
    REGISTER_ABILITY_OPTION(pHOLF29, NULL, &doHOLYFLAG[29]);
    REGISTER_ABILITY_OPTION(pHOLF30, NULL, &doHOLYFLAG[30]);
    REGISTER_ABILITY_OPTION(pHOLF31, NULL, &doHOLYFLAG[31]);
    REGISTER_ABILITY_OPTION(pHOLF32, NULL, &doHOLYFLAG[32]);
    REGISTER_ABILITY_OPTION(pHOLF33, NULL, &doHOLYFLAG[33]);
    REGISTER_ABILITY_OPTION(pHOLF34, NULL, &doHOLYFLAG[34]);
    REGISTER_ABILITY_OPTION(pHOLF35, NULL, &doHOLYFLAG[35]);
    REGISTER_ABILITY_OPTION(pHOLF36, NULL, &doHOLYFLAG[36]);
    REGISTER_ABILITY_OPTION(pHOLF37, NULL, &doHOLYFLAG[37]);
    REGISTER_ABILITY_OPTION(pHOLF38, NULL, &doHOLYFLAG[38]);
    REGISTER_ABILITY_OPTION(pHOLF39, NULL, &doHOLYFLAG[39]);
    REGISTER_ABILITY_OPTION(pHOLF40, NULL, &doHOLYFLAG[40]);
    REGISTER_ABILITY_OPTION(pHOLF41, NULL, &doHOLYFLAG[41]);
    REGISTER_ABILITY_OPTION(pHOLF42, NULL, &doHOLYFLAG[42]);
    REGISTER_ABILITY_OPTION(pHOLF43, NULL, &doHOLYFLAG[43]);
    REGISTER_ABILITY_OPTION(pHOLF44, NULL, &doHOLYFLAG[44]);
    REGISTER_ABILITY_OPTION(pHOLF45, NULL, &doHOLYFLAG[45]);
    REGISTER_ABILITY_OPTION(pHOLF46, NULL, &doHOLYFLAG[46]);
    REGISTER_ABILITY_OPTION(pHOLF47, NULL, &doHOLYFLAG[47]);
    REGISTER_ABILITY_OPTION(pHOLF48, NULL, &doHOLYFLAG[48]);
    REGISTER_ABILITY_OPTION(pHOLF49, NULL, &doHOLYFLAG[49]);
    REGISTER_ABILITY_OPTION(pHOLF50, NULL, &doHOLYFLAG[50]);
    REGISTER_ABILITY_OPTION(pHOLF51, NULL, &doHOLYFLAG[51]);
    REGISTER_ABILITY_OPTION(pHOLF52, NULL, &doHOLYFLAG[52]);
    REGISTER_ABILITY_OPTION(pHOLF53, NULL, &doHOLYFLAG[53]);
    REGISTER_ABILITY_OPTION(pHOLF54, NULL, &doHOLYFLAG[54]);
    REGISTER_ABILITY_OPTION(pHOLF55, NULL, &doHOLYFLAG[55]);
    REGISTER_ABILITY_OPTION(pHOLF56, NULL, &doHOLYFLAG[56]);
    REGISTER_ABILITY_OPTION(pHOLF57, NULL, &doHOLYFLAG[57]);
    REGISTER_ABILITY_OPTION(pHOLF58, NULL, &doHOLYFLAG[58]);
    REGISTER_ABILITY_OPTION(pHOLF59, NULL, &doHOLYFLAG[59]);
    REGISTER_ABILITY_OPTION(pHOLF60, NULL, &doHOLYFLAG[60]);
    REGISTER_ABILITY_OPTION(pHOVER, NULL, &doPOTHEALOVER);
    REGISTER_ABILITY_OPTION(pINFUR, NULL, &doINFURIATE);
    REGISTER_ABILITY_OPTION(pINTIM, NULL, &doINTIMIDATION);
    REGISTER_ABILITY_OPTION(pJKICK, NULL, &doJLTKICKS);
    REGISTER_ABILITY_OPTION(pJOLTS, NULL, &doJOLT);
    REGISTER_ABILITY_OPTION(pJUGUL, NULL, &doJUGULAR);
    REGISTER_ABILITY_OPTION(pKICKS, NULL, &doKICK);
    REGISTER_ABILITY_OPTION(pKNEES, NULL, &doKNEESTRIKE);
    REGISTER_ABILITY_OPTION(pKNFPL, NULL, &doKNIFEPLAY);
    REGISTER_ABILITY_OPTION(pLCLAW, NULL, &doLEOPARDCLAW);
    REGISTER_ABILITY_OPTION(pLHAND, NULL, &doLAYHAND);
    REGISTER_ABILITY_OPTION(pMELEE, NULL, &doMELEE);
    REGISTER_ABILITY_OPTION(pMELEP, NULL, &elMELEEPRI);
    REGISTER_ABILITY_OPTION(pMELES, NULL, &elMELEESEC);
    REGISTER_ABILITY_OPTION(pMENDS, NULL, &doMEND);
    REGISTER_ABILITY_OPTION(pOFREN, NULL, &doOPFRENZY);
    REGISTER_ABILITY_OPTION(pOSTRK, NULL, &doOPPORTUNISTICSTRIKE);
    REGISTER_ABILITY_OPTION(pPETAS, NULL, &doPETASSIST);
    REGISTER_ABILITY_OPTION(pPETDE, NULL, &doPETDELAY);
    REGISTER_ABILITY_OPTION(pPETENG, NULL, &doPETENGAGEHPS);
    REGISTER_ABILITY_OPTION(pPETMN, NULL, &doPETMEND);
    REGISTER_ABILITY_OPTION(pPETRN, NULL, &doPETRANGE);
    REGISTER_ABILITY_OPTION(pPICKP, NULL, &doPICKPOCKET);
    REGISTER_ABILITY_OPTION(pPINPT, NULL, &doPINPOINT);
    REGISTER_ABILITY_OPTION(pPLUGS, NULL, &doSKILL);
    REGISTER_ABILITY_OPTION(pPOKER, NULL, &elPOKER);
    REGISTER_ABILITY_OPTION(pPRVK0, NULL, &idPROVOKE[0]);
    REGISTER_ABILITY_OPTION(pPRVK1, NULL, &idPROVOKE[1]);
    REGISTER_ABILITY_OPTION(pPRVKE, NULL, &doPROVOKEEND);
    REGISTER_ABILITY_OPTION(pPRVKM, AggroReset, &doPROVOKEMAX);
    REGISTER_ABILITY_OPTION(pPRVKO, NULL, &doPROVOKEONCE);
    REGISTER_ABILITY_OPTION(pRAKES, NULL, &doRAKE);
    REGISTER_ABILITY_OPTION(pRALLO, NULL, &doRALLOS);
    REGISTER_ABILITY_OPTION(pRANGE, RangeReset, &doRANGE);
    REGISTER_ABILITY_OPTION(pRAVEN, NULL, &doRAVENS);
    REGISTER_ABILITY_OPTION(pRAVOL, NULL, &doRAGEVOLLEY);
    REGISTER_ABILITY_OPTION(pRESUM, NULL, &doRESUME);
    REGISTER_ABILITY_OPTION(pRGHTI, NULL, &doRIGHTIND);
    REGISTER_ABILITY_OPTION(pRKICK, NULL, &doROUNDKICK);
    REGISTER_ABILITY_OPTION(pSBLADES, NULL, &doSTORMBLADES);
    REGISTER_ABILITY_OPTION(pSELOK, NULL, &doSELOS);
    REGISTER_ABILITY_OPTION(pSENSE, NULL, &doSENSETRAP);
    REGISTER_ABILITY_OPTION(pSHIEL, NULL, &elSHIELD);
    REGISTER_ABILITY_OPTION(pSLAMS, NULL, &doSLAM);
    REGISTER_ABILITY_OPTION(pSLAPF, NULL, &doSLAPFACE);
    REGISTER_ABILITY_OPTION(pSNEAK, NULL, &doSNEAK);
    REGISTER_ABILITY_OPTION(pSTAND, NULL, &doSTAND);
    REGISTER_ABILITY_OPTION(pSTEEL, NULL, &doSTEELY);
    REGISTER_ABILITY_OPTION(pSTIKD, StickReset, &doSTICKDELAY);
    REGISTER_ABILITY_OPTION(pSTIKKB, StickReset, &doSTICKBREAK);
    REGISTER_ABILITY_OPTION(pSTIKM, NULL, &doSTICKMODE);
    REGISTER_ABILITY_OPTION(pSTIKNR, StickReset, &doSTICKNORANGE);
    REGISTER_ABILITY_OPTION(pSTIKR, StickReset, &doSTICKRANGE);
    REGISTER_ABILITY_OPTION(pSTRIK, NULL, &doSTRIKE);
    REGISTER_ABILITY_OPTION(pSTRIKM, NULL, &doSTRIKEMODE);
    REGISTER_ABILITY_OPTION(pSTUN0, NULL, &idSTUN[0]);
    REGISTER_ABILITY_OPTION(pSTUN1, NULL, &idSTUN[1]);
    REGISTER_ABILITY_OPTION(pSTUNS, NULL, &doSTUNNING);
    REGISTER_ABILITY_OPTION(pSYNGY, NULL, &doSYNERGY);
    REGISTER_ABILITY_OPTION(pTAUNT, NULL, &doTAUNT);
    REGISTER_ABILITY_OPTION(pTHIEF, NULL, &doTHIEFEYE);
    REGISTER_ABILITY_OPTION(pTHROW, NULL, &doTHROWSTONE);
    REGISTER_ABILITY_OPTION(pTIGER, NULL, &doTIGERCLAW);
    REGISTER_ABILITY_OPTION(pTTJAB, NULL, &doTHROATJAB);
    REGISTER_ABILITY_OPTION(pTWIST, NULL, &doTWISTEDSHANK);
    REGISTER_ABILITY_OPTION(pVIGAX, NULL, &doVIGAXE);
    REGISTER_ABILITY_OPTION(pVIGDR, NULL, &doVIGDAGGER);
    REGISTER_ABILITY_OPTION(pVIGSN, NULL, &doVIGSHURIKEN);
    REGISTER_ABILITY_OPTION(pWITHS, NULL, &doWITHSTAND);
    REGISTER_ABILITY_OPTION(pYAULP, NULL, &doYAULP);

    IniListe.clear();
    MapInsert(&IniListe, Option("assaultif", "", "", "", NULL, &ifASSAULT));
    MapInsert(&IniListe, Option("backstabif", "", "", "", NULL, &ifBACKSTAB));
    MapInsert(&IniListe, Option("banestrikeif", "", "", "", NULL, &ifBANESTRIKE));
    MapInsert(&IniListe, Option("bashif", "", "", "", NULL, &ifBASH));
    MapInsert(&IniListe, Option("beggingif", "", "", "", NULL, &ifBEGGING));
    MapInsert(&IniListe, Option("aspif", "", "", "", NULL, &ifASP));
    MapInsert(&IniListe, Option("bviviif", "", "", "", NULL, &ifBVIVI));
    MapInsert(&IniListe, Option("cloudif", "", "", "", NULL, &ifCLOUD));
    MapInsert(&IniListe, Option("boastfulif", "", "", "", NULL, &ifBOASTFUL));
    MapInsert(&IniListe, Option("bleedif", "", "", "", NULL, &ifBLEED));
    MapInsert(&IniListe, Option("bloodlustif", "", "", "", NULL, &ifBLOODLUST));
    MapInsert(&IniListe, Option("callchallengeif", "", "", "", NULL, &ifCALLCHALLENGE));
    MapInsert(&IniListe, Option("cstrikeif", "", "", "", NULL, &ifCSTRIKE));
    MapInsert(&IniListe, Option("crippleif", "", "", "", NULL, &ifCRIPPLE));
    MapInsert(&IniListe, Option("gutpunchif", "", "", "", NULL, &ifGUTPUNCH));
    MapInsert(&IniListe, Option("opstrikeif", "", "", "", NULL, &ifOPPORTUNISTICSTRIKE));
    MapInsert(&IniListe, Option("throatjabif", "", "", "", NULL, &ifTHROATJAB));
    MapInsert(&IniListe, Option("battleleapif", "", "", "", NULL, &ifBATTLELEAP));
    MapInsert(&IniListe, Option("challengeforif", "", "", "", NULL, &ifCHALLENGEFOR));
    MapInsert(&IniListe, Option("commandingif", "", "", "", NULL, &ifCOMMANDING));
    MapInsert(&IniListe, Option("cryhavocif", "", "", "", NULL, &ifCRYHAVOC));
    MapInsert(&IniListe, Option("defenseif", "", "", "", NULL, &ifDEFENSE));
    MapInsert(&IniListe, Option("disarmif", "", "", "", NULL, &ifDISARM));
    MapInsert(&IniListe, Option("dragonpunchif", "", "", "", NULL, &ifDRAGONPUNCH));
    MapInsert(&IniListe, Option("eaglestrikeif", "", "", "", NULL, &ifEAGLESTRIKE));
    MapInsert(&IniListe, Option("enragingkickif", "", "", "", NULL, &ifENRAGINGKICK));
    MapInsert(&IniListe, Option("eyegougeif", "", "", "", NULL, &ifEYEGOUGE));
    MapInsert(&IniListe, Option("evadeif", "", "", "", NULL, &ifEVADE));
    MapInsert(&IniListe, Option("fallsif", "", "", "", NULL, &ifFALLS));
    MapInsert(&IniListe, Option("feralswipeif", "", "", "", NULL, &ifFERALSWIPE));
    //MapInsert(&IniListe, Option("ferociouskickif", "", "", "", NULL, &ifFEROCIOUSKICK));
    MapInsert(&IniListe, Option("fieldarmif", "", "", "", NULL, &ifFIELDARM));
    MapInsert(&IniListe, Option("fistofwuif", "", "", "", NULL, &ifFISTSOFWU));
    MapInsert(&IniListe, Option("fclaw", "", "", "", NULL, &ifFCLAW));
    MapInsert(&IniListe, Option("flyingkickif", "", "", "", NULL, &ifFLYINGKICK));
    MapInsert(&IniListe, Option("forageif", "", "", "", NULL, &ifFORAGE));
    MapInsert(&IniListe, Option("frenzyif", "", "", "", NULL, &ifFRENZY));
    MapInsert(&IniListe, Option("gbladeif", "", "", "", NULL, &ifGBLADE));
    MapInsert(&IniListe, Option("gorillasmashif", "", "", "", NULL, &ifGORILLASMASH));
    MapInsert(&IniListe, Option("harmtouchif", "", "", "", NULL, &ifHARMTOUCH));
    MapInsert(&IniListe, Option("pothealfastif", "", "", "", NULL, &ifPOTHEALFAST));
    MapInsert(&IniListe, Option("pothealoverif", "", "", "", NULL, &ifPOTHEALOVER));
    MapInsert(&IniListe, Option("hideif", "", "", "", NULL, &ifHIDE));
    MapInsert(&IniListe, Option("intimidationif", "", "", "", NULL, &ifINTIMIDATION));
    MapInsert(&IniListe, Option("joltif", "", "", "", NULL, &ifJOLT));
    MapInsert(&IniListe, Option("jltkicksif", "", "", "", NULL, &ifJLTKICKS));
    MapInsert(&IniListe, Option("jugularif", "", "", "", NULL, &ifJUGULAR));
    MapInsert(&IniListe, Option("kickif", "", "", "", NULL, &ifKICK));
    MapInsert(&IniListe, Option("kneestrikeif", "", "", "", NULL, &ifKNEESTRIKE));
    MapInsert(&IniListe, Option("knifeplayif", "", "", "", NULL, &ifKNIFEPLAY));
    MapInsert(&IniListe, Option("layhandif", "", "", "", NULL, &ifLAYHAND));
    MapInsert(&IniListe, Option("leopardclawif", "", "", "", NULL, &ifLEOPARDCLAW));
    MapInsert(&IniListe, Option("mendif", "", "", "", NULL, &ifMEND));
    MapInsert(&IniListe, Option("monkeyif", "", "", "", NULL, &ifMONKEY));
    MapInsert(&IniListe, Option("opfrenzyif", "", "", "", NULL, &ifOPFRENZY));
    MapInsert(&IniListe, Option("pickpocketif", "", "", "", NULL, &ifPICKPOCKET));
    MapInsert(&IniListe, Option("pinpointif", "", "", "", NULL, &ifPINPOINT));
    MapInsert(&IniListe, Option("provokeif", "", "", "", NULL, &ifPROVOKE));
    MapInsert(&IniListe, Option("ragevolleyif", "", "", "", NULL, &ifRAGEVOLLEY));
    MapInsert(&IniListe, Option("rakeif", "", "", "", NULL, &ifRAKE));
    MapInsert(&IniListe, Option("rallosif", "", "", "", NULL, &ifRALLOS));
    MapInsert(&IniListe, Option("ravensif", "", "", "", NULL, &ifRAVENS));
    MapInsert(&IniListe, Option("rightindif", "", "", "", NULL, &ifRIGHTIND));
    MapInsert(&IniListe, Option("roundkickif", "", "", "", NULL, &ifROUNDKICK));
    MapInsert(&IniListe, Option("sensetrapif", "", "", "", NULL, &ifSENSETRAP));
    MapInsert(&IniListe, Option("selosif", "", "", "", NULL, &ifSELOS));
    MapInsert(&IniListe, Option("slamif", "", "", "", NULL, &ifSLAM));
    MapInsert(&IniListe, Option("slapfaceif", "", "", "", NULL, &ifSLAPFACE));
    MapInsert(&IniListe, Option("sneakif", "", "", "", NULL, &ifSNEAK));
    MapInsert(&IniListe, Option("steelyif", "", "", "", NULL, &ifSTEELY));
    MapInsert(&IniListe, Option("stormbladesif", "", "", "", NULL, &ifSTORMBLADES));
    MapInsert(&IniListe, Option("strikeif", "", "", "", NULL, &ifSTRIKE));
    MapInsert(&IniListe, Option("stunningif", "", "", "", NULL, &ifSTUNNING));
    MapInsert(&IniListe, Option("synergyif", "", "", "", NULL, &ifSYNERGY));
    MapInsert(&IniListe, Option("tauntif", "", "", "", NULL, &ifTAUNT));
    MapInsert(&IniListe, Option("thiefeyeif", "", "", "", NULL, &ifTHIEFEYE));
    MapInsert(&IniListe, Option("throwstoneif", "", "", "", NULL, &ifTHROWSTONE));
    MapInsert(&IniListe, Option("tigerclawif", "", "", "", NULL, &ifTIGERCLAW));
    MapInsert(&IniListe, Option("twistedshankif", "", "", "", NULL, &ifTWISTEDSHANK));
    MapInsert(&IniListe, Option("vigaxeif", "", "", "", NULL, &ifVIGAXE));
    MapInsert(&IniListe, Option("vigdaggerif", "", "", "", NULL, &ifVIGDAGGER));
    MapInsert(&IniListe, Option("vigshurikenif", "", "", "", NULL, &ifVIGSHURIKEN));
    MapInsert(&IniListe, Option("withstandif", "", "", "", NULL, &ifWITHSTAND));
    MapInsert(&IniListe, Option("yaulp", "", "", "", NULL, &ifYAULP));
    MapInsert(&IniListe, Option("stickcmd", "", "", "", NULL, &StickCMD));
    MapInsert(&IniListe, Option("strikecmd", "", "", "", NULL, &StrikeCMD));
    MapInsert(&IniListe, Option("downshit0", "", "", "", NULL, &DOWNSHIT[0]));
    MapInsert(&IniListe, Option("downshit1", "", "", "", NULL, &DOWNSHIT[1]));
    MapInsert(&IniListe, Option("downshit2", "", "", "", NULL, &DOWNSHIT[2]));
    MapInsert(&IniListe, Option("downshit3", "", "", "", NULL, &DOWNSHIT[3]));
    MapInsert(&IniListe, Option("downshit4", "", "", "", NULL, &DOWNSHIT[4]));
    MapInsert(&IniListe, Option("downshit5", "", "", "", NULL, &DOWNSHIT[5]));
    MapInsert(&IniListe, Option("downshit6", "", "", "", NULL, &DOWNSHIT[6]));
    MapInsert(&IniListe, Option("downshit7", "", "", "", NULL, &DOWNSHIT[7]));
    MapInsert(&IniListe, Option("downshit8", "", "", "", NULL, &DOWNSHIT[8]));
    MapInsert(&IniListe, Option("downshit9", "", "", "", NULL, &DOWNSHIT[9]));
    MapInsert(&IniListe, Option("downshit10", "", "", "", NULL, &DOWNSHIT[10]));
    MapInsert(&IniListe, Option("downshit11", "", "", "", NULL, &DOWNSHIT[11]));
    MapInsert(&IniListe, Option("downshit12", "", "", "", NULL, &DOWNSHIT[12]));
    MapInsert(&IniListe, Option("downshit13", "", "", "", NULL, &DOWNSHIT[13]));
    MapInsert(&IniListe, Option("downshit14", "", "", "", NULL, &DOWNSHIT[14]));
    MapInsert(&IniListe, Option("downshit15", "", "", "", NULL, &DOWNSHIT[15]));
    MapInsert(&IniListe, Option("downshit16", "", "", "", NULL, &DOWNSHIT[16]));
    MapInsert(&IniListe, Option("downshit17", "", "", "", NULL, &DOWNSHIT[17]));
    MapInsert(&IniListe, Option("downshit18", "", "", "", NULL, &DOWNSHIT[18]));
    MapInsert(&IniListe, Option("downshit19", "", "", "", NULL, &DOWNSHIT[19]));
    MapInsert(&IniListe, Option("downshit20", "", "", "", NULL, &DOWNSHIT[20]));
    MapInsert(&IniListe, Option("downshit21", "", "", "", NULL, &DOWNSHIT[21]));
    MapInsert(&IniListe, Option("downshit22", "", "", "", NULL, &DOWNSHIT[22]));
    MapInsert(&IniListe, Option("downshit23", "", "", "", NULL, &DOWNSHIT[23]));
    MapInsert(&IniListe, Option("downshit24", "", "", "", NULL, &DOWNSHIT[24]));
    MapInsert(&IniListe, Option("downshit25", "", "", "", NULL, &DOWNSHIT[25]));
    MapInsert(&IniListe, Option("downshit26", "", "", "", NULL, &DOWNSHIT[26]));
    MapInsert(&IniListe, Option("downshit27", "", "", "", NULL, &DOWNSHIT[27]));
    MapInsert(&IniListe, Option("downshit28", "", "", "", NULL, &DOWNSHIT[28]));
    MapInsert(&IniListe, Option("downshit29", "", "", "", NULL, &DOWNSHIT[29]));
    MapInsert(&IniListe, Option("downshit30", "", "", "", NULL, &DOWNSHIT[30]));
    MapInsert(&IniListe, Option("downshit31", "", "", "", NULL, &DOWNSHIT[31]));
    MapInsert(&IniListe, Option("downshit32", "", "", "", NULL, &DOWNSHIT[32]));
    MapInsert(&IniListe, Option("downshit33", "", "", "", NULL, &DOWNSHIT[33]));
    MapInsert(&IniListe, Option("downshit34", "", "", "", NULL, &DOWNSHIT[34]));
    MapInsert(&IniListe, Option("downshit35", "", "", "", NULL, &DOWNSHIT[35]));
    MapInsert(&IniListe, Option("downshit36", "", "", "", NULL, &DOWNSHIT[36]));
    MapInsert(&IniListe, Option("downshit37", "", "", "", NULL, &DOWNSHIT[37]));
    MapInsert(&IniListe, Option("downshit38", "", "", "", NULL, &DOWNSHIT[38]));
    MapInsert(&IniListe, Option("downshit39", "", "", "", NULL, &DOWNSHIT[39]));
    MapInsert(&IniListe, Option("downshit40", "", "", "", NULL, &DOWNSHIT[40]));
    MapInsert(&IniListe, Option("downshit41", "", "", "", NULL, &DOWNSHIT[41]));
    MapInsert(&IniListe, Option("downshit42", "", "", "", NULL, &DOWNSHIT[42]));
    MapInsert(&IniListe, Option("downshit43", "", "", "", NULL, &DOWNSHIT[43]));
    MapInsert(&IniListe, Option("downshit44", "", "", "", NULL, &DOWNSHIT[44]));
    MapInsert(&IniListe, Option("downshit45", "", "", "", NULL, &DOWNSHIT[45]));
    MapInsert(&IniListe, Option("downshit46", "", "", "", NULL, &DOWNSHIT[46]));
    MapInsert(&IniListe, Option("downshit47", "", "", "", NULL, &DOWNSHIT[47]));
    MapInsert(&IniListe, Option("downshit48", "", "", "", NULL, &DOWNSHIT[48]));
    MapInsert(&IniListe, Option("downshit49", "", "", "", NULL, &DOWNSHIT[49]));
    MapInsert(&IniListe, Option("downshit50", "", "", "", NULL, &DOWNSHIT[50]));
    MapInsert(&IniListe, Option("downshit51", "", "", "", NULL, &DOWNSHIT[51]));
    MapInsert(&IniListe, Option("downshit52", "", "", "", NULL, &DOWNSHIT[52]));
    MapInsert(&IniListe, Option("downshit53", "", "", "", NULL, &DOWNSHIT[53]));
    MapInsert(&IniListe, Option("downshit54", "", "", "", NULL, &DOWNSHIT[54]));
    MapInsert(&IniListe, Option("downshit55", "", "", "", NULL, &DOWNSHIT[55]));
    MapInsert(&IniListe, Option("downshit56", "", "", "", NULL, &DOWNSHIT[56]));
    MapInsert(&IniListe, Option("downshit57", "", "", "", NULL, &DOWNSHIT[57]));
    MapInsert(&IniListe, Option("downshit58", "", "", "", NULL, &DOWNSHIT[58]));
    MapInsert(&IniListe, Option("downshit59", "", "", "", NULL, &DOWNSHIT[59]));
    MapInsert(&IniListe, Option("downshit60", "", "", "", NULL, &DOWNSHIT[60]));
    MapInsert(&IniListe, Option("downshitif", "", "", "", NULL, &DOWNSHITIF));
    MapInsert(&IniListe, Option("holyshit0", "", "", "", NULL, &HOLYSHIT[0]));
    MapInsert(&IniListe, Option("holyshit1", "", "", "", NULL, &HOLYSHIT[1]));
    MapInsert(&IniListe, Option("holyshit2", "", "", "", NULL, &HOLYSHIT[2]));
    MapInsert(&IniListe, Option("holyshit3", "", "", "", NULL, &HOLYSHIT[3]));
    MapInsert(&IniListe, Option("holyshit4", "", "", "", NULL, &HOLYSHIT[4]));
    MapInsert(&IniListe, Option("holyshit5", "", "", "", NULL, &HOLYSHIT[5]));
    MapInsert(&IniListe, Option("holyshit6", "", "", "", NULL, &HOLYSHIT[6]));
    MapInsert(&IniListe, Option("holyshit7", "", "", "", NULL, &HOLYSHIT[7]));
    MapInsert(&IniListe, Option("holyshit8", "", "", "", NULL, &HOLYSHIT[8]));
    MapInsert(&IniListe, Option("holyshit9", "", "", "", NULL, &HOLYSHIT[9]));
    MapInsert(&IniListe, Option("holyshit10", "", "", "", NULL, &HOLYSHIT[10]));
    MapInsert(&IniListe, Option("holyshit11", "", "", "", NULL, &HOLYSHIT[11]));
    MapInsert(&IniListe, Option("holyshit12", "", "", "", NULL, &HOLYSHIT[12]));
    MapInsert(&IniListe, Option("holyshit13", "", "", "", NULL, &HOLYSHIT[13]));
    MapInsert(&IniListe, Option("holyshit14", "", "", "", NULL, &HOLYSHIT[14]));
    MapInsert(&IniListe, Option("holyshit15", "", "", "", NULL, &HOLYSHIT[15]));
    MapInsert(&IniListe, Option("holyshit16", "", "", "", NULL, &HOLYSHIT[16]));
    MapInsert(&IniListe, Option("holyshit17", "", "", "", NULL, &HOLYSHIT[17]));
    MapInsert(&IniListe, Option("holyshit18", "", "", "", NULL, &HOLYSHIT[18]));
    MapInsert(&IniListe, Option("holyshit19", "", "", "", NULL, &HOLYSHIT[19]));
    MapInsert(&IniListe, Option("holyshit20", "", "", "", NULL, &HOLYSHIT[20]));
    MapInsert(&IniListe, Option("holyshit21", "", "", "", NULL, &HOLYSHIT[21]));
    MapInsert(&IniListe, Option("holyshit22", "", "", "", NULL, &HOLYSHIT[22]));
    MapInsert(&IniListe, Option("holyshit23", "", "", "", NULL, &HOLYSHIT[23]));
    MapInsert(&IniListe, Option("holyshit24", "", "", "", NULL, &HOLYSHIT[24]));
    MapInsert(&IniListe, Option("holyshit25", "", "", "", NULL, &HOLYSHIT[25]));
    MapInsert(&IniListe, Option("holyshit26", "", "", "", NULL, &HOLYSHIT[26]));
    MapInsert(&IniListe, Option("holyshit27", "", "", "", NULL, &HOLYSHIT[27]));
    MapInsert(&IniListe, Option("holyshit28", "", "", "", NULL, &HOLYSHIT[28]));
    MapInsert(&IniListe, Option("holyshit29", "", "", "", NULL, &HOLYSHIT[29]));
    MapInsert(&IniListe, Option("holyshit30", "", "", "", NULL, &HOLYSHIT[30]));
    MapInsert(&IniListe, Option("holyshit31", "", "", "", NULL, &HOLYSHIT[31]));
    MapInsert(&IniListe, Option("holyshit32", "", "", "", NULL, &HOLYSHIT[32]));
    MapInsert(&IniListe, Option("holyshit33", "", "", "", NULL, &HOLYSHIT[33]));
    MapInsert(&IniListe, Option("holyshit34", "", "", "", NULL, &HOLYSHIT[34]));
    MapInsert(&IniListe, Option("holyshit35", "", "", "", NULL, &HOLYSHIT[35]));
    MapInsert(&IniListe, Option("holyshit36", "", "", "", NULL, &HOLYSHIT[36]));
    MapInsert(&IniListe, Option("holyshit37", "", "", "", NULL, &HOLYSHIT[37]));
    MapInsert(&IniListe, Option("holyshit38", "", "", "", NULL, &HOLYSHIT[38]));
    MapInsert(&IniListe, Option("holyshit39", "", "", "", NULL, &HOLYSHIT[39]));
    MapInsert(&IniListe, Option("holyshit40", "", "", "", NULL, &HOLYSHIT[40]));
    MapInsert(&IniListe, Option("holyshit41", "", "", "", NULL, &HOLYSHIT[41]));
    MapInsert(&IniListe, Option("holyshit42", "", "", "", NULL, &HOLYSHIT[42]));
    MapInsert(&IniListe, Option("holyshit43", "", "", "", NULL, &HOLYSHIT[43]));
    MapInsert(&IniListe, Option("holyshit44", "", "", "", NULL, &HOLYSHIT[44]));
    MapInsert(&IniListe, Option("holyshit45", "", "", "", NULL, &HOLYSHIT[45]));
    MapInsert(&IniListe, Option("holyshit46", "", "", "", NULL, &HOLYSHIT[46]));
    MapInsert(&IniListe, Option("holyshit47", "", "", "", NULL, &HOLYSHIT[47]));
    MapInsert(&IniListe, Option("holyshit48", "", "", "", NULL, &HOLYSHIT[48]));
    MapInsert(&IniListe, Option("holyshit49", "", "", "", NULL, &HOLYSHIT[49]));
    MapInsert(&IniListe, Option("holyshit50", "", "", "", NULL, &HOLYSHIT[50]));
    MapInsert(&IniListe, Option("holyshit51", "", "", "", NULL, &HOLYSHIT[51]));
    MapInsert(&IniListe, Option("holyshit52", "", "", "", NULL, &HOLYSHIT[52]));
    MapInsert(&IniListe, Option("holyshit53", "", "", "", NULL, &HOLYSHIT[53]));
    MapInsert(&IniListe, Option("holyshit54", "", "", "", NULL, &HOLYSHIT[54]));
    MapInsert(&IniListe, Option("holyshit55", "", "", "", NULL, &HOLYSHIT[55]));
    MapInsert(&IniListe, Option("holyshit56", "", "", "", NULL, &HOLYSHIT[56]));
    MapInsert(&IniListe, Option("holyshit57", "", "", "", NULL, &HOLYSHIT[57]));
    MapInsert(&IniListe, Option("holyshit58", "", "", "", NULL, &HOLYSHIT[58]));
    MapInsert(&IniListe, Option("holyshit59", "", "", "", NULL, &HOLYSHIT[59]));
    MapInsert(&IniListe, Option("holyshit60", "", "", "", NULL, &HOLYSHIT[60]));
    MapInsert(&IniListe, Option("holyshitif", "", "", "", NULL, &HOLYSHITIF));

    VarListe.clear();
    MapInsert(&VarListe, Option("idassault", "", "", "", NULL, &idASSAULT));
    MapInsert(&VarListe, Option("idbackstab", "", "", "", NULL, &idBACKSTAB));
    MapInsert(&VarListe, Option("idbanestrike", "", "", "", NULL, &idBANESTRIKE));
    MapInsert(&VarListe, Option("idbash", "", "", "", NULL, &idBASH));
    MapInsert(&VarListe, Option("idbattleleap", "", "", "", NULL, &idBATTLELEAP));
    MapInsert(&VarListe, Option("idbvivi", "", "", "", NULL, &idBVIVI));
    MapInsert(&VarListe, Option("idcloud", "", "", "", NULL, &idCLOUD));
    MapInsert(&VarListe, Option("idbleed", "", "", "", NULL, &idBLEED));
    MapInsert(&VarListe, Option("idbloodlust", "", "", "", NULL, &idBLOODLUST));
    MapInsert(&VarListe, Option("idboastful", "", "", "", NULL, &idBOASTFUL));
    MapInsert(&VarListe, Option("idbegging", "", "", "", NULL, &idBEGGING));
    MapInsert(&VarListe, Option("idASP", "", "", "", NULL, &idASP));
    MapInsert(&VarListe, Option("idcallchallenge", "", "", "", NULL, &idCALLCHALLENGE));
    MapInsert(&VarListe, Option("idcstrike", "", "", "", NULL, &idCSTRIKE));
    MapInsert(&VarListe, Option("idcommanding", "", "", "", NULL, &idCOMMANDING));
    MapInsert(&VarListe, Option("idCRIPPLE", "", "", "", NULL, &idCRIPPLE));
    MapInsert(&VarListe, Option("idcryhavoc", "", "", "", NULL, &idCRYHAVOC));
    MapInsert(&VarListe, Option("iddefense", "", "", "", NULL, &idDEFENSE));
    MapInsert(&VarListe, Option("iddisarm", "", "", "", NULL, &idDISARM));
    MapInsert(&VarListe, Option("iddragonpunch", "", "", "", NULL, &idDRAGONPUNCH));
    MapInsert(&VarListe, Option("ideaglestrike", "", "", "", NULL, &idEAGLESTRIKE));
    MapInsert(&VarListe, Option("idenragingkick", "", "", "", NULL, &idENRAGINGKICK));
    MapInsert(&VarListe, Option("ideyegouge", "", "", "", NULL, &idEYEGOUGE));
    MapInsert(&VarListe, Option("idescape", "", "", "", NULL, &idESCAPE));
    MapInsert(&VarListe, Option("idferalswipe", "", "", "", NULL, &idFERALSWIPE));
    //MapInsert(&VarListe, Option("idferociouskick", "", "", "", NULL, &idFEROCIOUSKICK));
    MapInsert(&VarListe, Option("idfieldarm", "", "", "", NULL, &idFIELDARM));
    MapInsert(&VarListe, Option("idfistsofwu", "", "", "", NULL, &idFISTSOFWU));
    MapInsert(&VarListe, Option("idfclaw", "", "", "", NULL, &idFCLAW));
    MapInsert(&VarListe, Option("idflyingkick", "", "", "", NULL, &idFLYINGKICK));
    MapInsert(&VarListe, Option("idforage", "", "", "", NULL, &idFORAGE));
    MapInsert(&VarListe, Option("idfrenzy", "", "", "", NULL, &idFRENZY));
    MapInsert(&VarListe, Option("idgblade", "", "", "", NULL, &idGBLADE));
    MapInsert(&VarListe, Option("idgorillasmash", "", "", "", NULL, &idGORILLASMASH));
    MapInsert(&VarListe, Option("idgutpunch", "", "", "", NULL, &idGUTPUNCH));
    MapInsert(&VarListe, Option("idharmtouch", "", "", "", NULL, &idHARMTOUCH));
    MapInsert(&VarListe, Option("idhide", "", "", "", NULL, &idHIDE));
    MapInsert(&VarListe, Option("idintimidation", "", "", "", NULL, &idINTIMIDATION));
    MapInsert(&VarListe, Option("idjltkicks", "", "", "", NULL, &idJLTKICKS));
    MapInsert(&VarListe, Option("idjolt", "", "", "", NULL, &idJOLT));
    MapInsert(&VarListe, Option("idjugular", "", "", "", NULL, &idJUGULAR));
    MapInsert(&VarListe, Option("idkick", "", "", "", NULL, &idKICK));
    MapInsert(&VarListe, Option("idkneestrike", "", "", "", NULL, &idKNEESTRIKE));
    MapInsert(&VarListe, Option("idknifeplay", "", "", "", NULL, &idKNIFEPLAY));
    MapInsert(&VarListe, Option("idlayhand", "", "", "", NULL, &idLAYHAND));
    MapInsert(&VarListe, Option("idleopardclaw", "", "", "", NULL, &idLEOPARDCLAW));
    MapInsert(&VarListe, Option("idmend", "", "", "", NULL, &idMEND));
    MapInsert(&VarListe, Option("idmonkey", "", "", "", NULL, &idMONKEY));
    MapInsert(&VarListe, Option("idopfrenzy", "", "", "", NULL, &idOPFRENZY));
    MapInsert(&VarListe, Option("idpickpocket", "", "", "", NULL, &idPICKPOCKET));
    MapInsert(&VarListe, Option("idpinpoint", "", "", "", NULL, &idPINPOINT));
    MapInsert(&VarListe, Option("idoppstrike", "", "", "", NULL, &idOPPORTUNISTICSTRIKE));
    MapInsert(&VarListe, Option("idragevolley", "", "", "", NULL, &idRAGEVOLLEY));
    MapInsert(&VarListe, Option("idrake", "", "", "", NULL, &idRAKE));
    MapInsert(&VarListe, Option("idrallos", "", "", "", NULL, &idRALLOS));
    MapInsert(&VarListe, Option("idravens", "", "", "", NULL, &idRAVENS));
    MapInsert(&VarListe, Option("idroundkick", "", "", "", NULL, &idROUNDKICK));
    MapInsert(&VarListe, Option("idrightidg", "", "", "", NULL, &idRIGHTIND));
    MapInsert(&VarListe, Option("idselos", "", "", "", NULL, &idSELOS));
    MapInsert(&VarListe, Option("idsensetrap", "", "", "", NULL, &idSENSETRAP));
    MapInsert(&VarListe, Option("idslam", "", "", "", NULL, &idSLAM));
    MapInsert(&VarListe, Option("idslapface", "", "", "", NULL, &idSLAPFACE));
    MapInsert(&VarListe, Option("idsneak", "", "", "", NULL, &idSNEAK));
    MapInsert(&VarListe, Option("idsteely", "", "", "", NULL, &idSTEELY));
    MapInsert(&VarListe, Option("idstrike", "", "", "", NULL, &idSTRIKE));
    MapInsert(&VarListe, Option("idsynergy", "", "", "", NULL, &idSYNERGY));
    MapInsert(&VarListe, Option("idtaunt", "", "", "", NULL, &idTAUNT));
    MapInsert(&VarListe, Option("idthroatjab", "", "", "", NULL, &idTHROATJAB));
    MapInsert(&VarListe, Option("idthiefeye", "", "", "", NULL, &idTHIEFEYE));
    MapInsert(&VarListe, Option("idthrowstone", "", "", "", NULL, &idTHROWSTONE));
    MapInsert(&VarListe, Option("idtigerclaw", "", "", "", NULL, &idTIGERCLAW));
    MapInsert(&VarListe, Option("idvigaxe", "", "", "", NULL, &idVIGAXE));
    MapInsert(&VarListe, Option("idvigdagger", "", "", "", NULL, &idVIGDAGGER));
    MapInsert(&VarListe, Option("idvigshuriken", "", "", "", NULL, &idVIGSHURIKEN));
    MapInsert(&VarListe, Option("idwithstand", "", "", "", NULL, &idWITHSTAND));
    MapInsert(&VarListe, Option("idyaulp", "", "", "", NULL, &idYAULP));
    MapInsert(&VarListe, Option("idfeign0", "", "", "", NULL, &idFEIGN[0]));
    MapInsert(&VarListe, Option("idfeign1", "", "", "", NULL, &idFEIGN[1]));
    MapInsert(&VarListe, Option("idpetassist", "", "", "", NULL, &idPETASSIST));
    MapInsert(&VarListe, Option("idpetdelay", "", "", "", NULL, &idPETDELAY));
    MapInsert(&VarListe, Option("idpetrange", "", "", "", NULL, &idPETRANGE));
    MapInsert(&VarListe, Option("idpetmend", "", "", "", NULL, &idPETMEND));
    MapInsert(&VarListe, Option("idpothealfast", "", "", "", NULL, &idPOTHEALFAST));
    MapInsert(&VarListe, Option("idpothealover", "", "", "", NULL, &idPOTHEALOVER));
    MapInsert(&VarListe, Option("idprovoke0", "", "", "", NULL, &idPROVOKE[0]));
    MapInsert(&VarListe, Option("idprovoke1", "", "", "", NULL, &idPROVOKE[1]));
    MapInsert(&VarListe, Option("idstun0", "", "", "", NULL, &idSTUN[0]));
    MapInsert(&VarListe, Option("idstun1", "", "", "", NULL, &idSTUN[1]));

    ZeroMemory(AttackArray, sizeof(AttackArray));
    AttackArray[0]   = true;
    AttackArray[1]   = true;
    AttackArray[2]   = true;
    AttackArray[3]   = true;
    AttackArray[4]   = true;
    AttackArray[5]   = true; // hit with main hand
    AttackArray[6]   = true;
    AttackArray[7]   = true;
    AttackArray[8]   = true; // punch
    AttackArray[9]   = true;
    AttackArray[10]  = true;
    AttackArray[11]  = true;
    AttackArray[12]  = true; // hit with offhand
    AttackArray[42]  = true; // casting type 1
    AttackArray[43]  = true; // casting type 2
    AttackArray[44]  = true; // casting type 3
    AttackArray[80]  = true; // attacking
    AttackArray[106] = true; // attacking
    AttackArray[129] = true; // attacking
    AttackArray[133] = true; // attacking
    AttackArray[143] = true; // attacking
    AttackArray[144] = true; // attacking

    ZeroMemory(IdlingArray, sizeof(IdlingArray));
    IdlingArray[26]  = true; // mezzed maybe?
    IdlingArray[32]  = true; // sitting
    IdlingArray[71]  = true; // standing after sitting
    IdlingArray[104] = true; // standing still
    IdlingArray[107] = true; // standing still
    IdlingArray[110] = true; // standing still

    SaveIndx    = 0;
    pMeleeEvent = new Blech('#');

    ZeroMemory(SaveList, sizeof(SaveList));
    SaveList[SaveIndx++] = pMeleeEvent->AddEvent("Auto fire off#*#"                                                     , AUTOFIREOFF  , (void *)0);
    SaveList[SaveIndx++] = pMeleeEvent->AddEvent("Auto fire on#*#"                                                      , AUTOFIREON   , (void *)0);
    SaveList[SaveIndx++] = pMeleeEvent->AddEvent("#*#Attacking #*# Master.#*#"                                          , PETATTK      , (void *)0);
    SaveList[SaveIndx++] = pMeleeEvent->AddEvent("#*#Sorry#*#calming down.#*#"                                          , PETBACK      , (void *)0);
    SaveList[SaveIndx++] = pMeleeEvent->AddEvent("#*#Waiting for your order to attack, Master.#*#"                      , PETHOLD      , (void *)0);
    SaveList[SaveIndx++] = pMeleeEvent->AddEvent("#*#Now Holding#*#I will not attack until ordered.#*#"                 , PETHOLD      , (void *)0);
    SaveList[SaveIndx++] = pMeleeEvent->AddEvent("#*# begins casting a spell#*#"                                        , CASTING      , (void *)0);
    SaveList[SaveIndx++] = pMeleeEvent->AddEvent("#*# begins to cast#*#"                                                , CASTING      , (void *)0);
    SaveList[SaveIndx++] = pMeleeEvent->AddEvent("#*# has become ENRAGED#*#"                                            , ENRAGEON     , (void *)0);
    SaveList[SaveIndx++] = pMeleeEvent->AddEvent("#*# is no longer enraged#*#"                                          , ENRAGEOFF    , (void *)0);
    SaveList[SaveIndx++] = pMeleeEvent->AddEvent("#*# is infuriated#*#"                                                 , INFURIATEON  , (void *)0);
    SaveList[SaveIndx++] = pMeleeEvent->AddEvent("#*# no longer infuriated#*#"                                          , INFURIATEOFF , (void *)0);
    SaveList[SaveIndx++] = pMeleeEvent->AddEvent("#*# has fallen to the ground#*#"                                      , FALLEN       , (void *)1);
    SaveList[SaveIndx++] = pMeleeEvent->AddEvent("#*#You are no longer feigning death#*#"                               , BROKEN       , (void *)0);
    SaveList[SaveIndx++] = pMeleeEvent->AddEvent("#*#Your body aches from a wave of pain#*#"                            , BROKEN       , (void *)0);
    SaveList[SaveIndx++] = pMeleeEvent->AddEvent("#*#The strength of your will allows you to resume feigning death#*#"  , RESUME       , (void *)0);

    SaveList[SaveIndx++] = pMeleeEvent->AddEvent("#*#You failed to hide yourself#*#"                                    , HIDEOFF      , (void *)0);
    SaveList[SaveIndx++] = pMeleeEvent->AddEvent("#*#You stop hiding#*#"                                                , HIDEOFF      , (void *)0);
    SaveList[SaveIndx++] = pMeleeEvent->AddEvent("#*#You have moved and are no longer hidden#*#"                        , HIDEOFF      , (void *)0);
    SaveList[SaveIndx++] = pMeleeEvent->AddEvent("#*#You have hidden yourself from view#*#"                             , HIDEON       , (void *)0);
    SaveList[SaveIndx++] = pMeleeEvent->AddEvent("#*#You are as quiet as a herd of running elephants#*#"                , SNEAKOFF     , (void *)0);
    SaveList[SaveIndx++] = pMeleeEvent->AddEvent("#*#You stop sneaking#*#"                                              , SNEAKOFF     , (void *)0);
    SaveList[SaveIndx++] = pMeleeEvent->AddEvent("#*#You are as quiet as a cat stalking its prey#*#"                    , SNEAKON      , (void *)0);
    //Rogue Strike Fix htw 2/20/2011
    SaveList[SaveIndx++] = pMeleeEvent->AddEvent("#*#You can not use this ability because you have not been hidden for long enough.#*#", STRIKERESET, (void *)0);

    pMeleeTypes = new MQ2MeleeType;
    AddMQ2Data("Melee",    DataMelee);
    AddMQ2Data("meleemvb", datameleemvb);
    AddMQ2Data("meleemvi", datameleemvi);
    AddMQ2Data("meleemvs", datameleemvs);

    AddCommand("/enrageon",     EnrageON);
    AddCommand("/enrageoff",    EnrageOFF);
    AddCommand("/infuriateon",  InfuriateON);
    AddCommand("/infuriateoff", InfuriateOFF);
    AddCommand("/killthis",     KillThis);
    AddCommand("/melee",        Melee);
    AddCommand("/throwit",      ThrowIT);

    bMULoaded = HandleMoveUtils();
}

PLUGIN_API void SetGameState(unsigned long GameState)
{
    if (SETBINDINGS)
        Bindding(GameState == GAMESTATE_INGAME);

    if (GameState == GAMESTATE_INGAME)
    {
        if (!Loaded)
        {
            Configure();
            DebugSpew("MQ2Melee SetGameState GAMESTATE_INGAME & NOT LOADED, calling Configure()");
        }
        else
        {
            DebugSpew("MQ2Melee SetGameState GAMESTATE_INGAME & LOADED=true, not calling anything");
        }
    }
    else if (GameState != GAMESTATE_LOGGINGIN)
    {
        if (Loaded)
        {
            DebugSpew("MQ2Melee SetGameState not INGAME or LOGGINGIN & LOADED=true, setting LOADED false");
            Loaded = false;
        }
        else
        {
            DebugSpew("MQ2Melee SetGameState not INGAME and *is* LOGGINGIN, not changing loaded");
        }
    }
    DebugSpew("MQ2Melee SetGameState Loaded == %s", Loaded ? "true" : "false");
}

PLUGIN_API unsigned long OnIncomingChat(char* Line, unsigned long Color)
{
    CHAR szLine[MAX_STRING] = { 0 };
    strcpy_s(szLine, Line);
    if (doSKILL && gbInZone && pMeleeEvent)
    {
        switch (Color)
        {
        case 265:
        case 267:
            SwingHits++;
            break;
        case 266:
        case 268:
            TakenHits++;
            break;
        case 13:
        case 270:
        case 273:
        case 288:
        case 289:
        case 306:
        case 328:
        case 337:
            pMeleeEvent->Feed(szLine);
        }
    }
    return 0;
}

PLUGIN_API void OnPulse()
{
    if (doSKILL && Loaded && gbInZone && SpawnMe())
    {
        if (PCHARINFO2 Me = GetCharInfo2())
        {
            if (GetCharInfo2()->Shrouded != Shrouded)
            {
                SetGameState(GAMESTATE_UNLOADING);
                SetGameState(GAMESTATE_INGAME);
            }
            if (!HiddenTimer && IsInvisible()) HiddenTimer = (unsigned long)clock();
            if (!SilentTimer && IsSneaking())  SilentTimer = (unsigned long)clock();
            Travel = SpeedRun(SpawnMe());
            if (Moving = (Travel > 0.05 || Travel < -0.05)) TimerMove = (unsigned long)clock() + delay * 5;
            Immobile = (!(MQ2Globals::gbMoving) && (!TimerMove || (unsigned long)clock() > TimerMove));


            if (doPETASSIST)
            {
                if (PSPAWNINFO Pet = SpawnPet())
                {
                    if (Pet->WhoFollowing) PetSEEN();
                    //else if (!PetOnHold && (unsigned long)clock() > PetOnAttk && !Pet->WhoFollowing) PetBACK();
                }
            }
            if (MeleeTarg && (!pTarget || MeleeType != SpawnMask(GetSpawnID(MeleeTarg)))) Override(NULL, "");
            if ((unsigned long)clock() > MeleeTime)
            {
                MeleeTime = (unsigned long)clock() + delay;
                MeleeHandle();
            }
        }
    }
    if ((bMULoaded && !bMUPointers) || (bMUPointers && !bMULoaded))
    {
        bMULoaded = HandleMoveUtils();
    }
}

PLUGIN_API void ShutdownPlugin()
{
    SetGameState(GAMESTATE_UNLOADING);

    RemoveCommand("/enrageon");
    RemoveCommand("/enrageoff");
    RemoveCommand("/infuriateon");
    RemoveCommand("/infuriateoff");
    RemoveCommand("/killthis");
    RemoveCommand("/melee");
    RemoveCommand("/throwit");

    RemoveMQ2Data("Melee");
    RemoveMQ2Data("meleemvs");
    RemoveMQ2Data("meleemvi");
    RemoveMQ2Data("meleemvb");

    pMeleeEvent->Reset();
    delete pMeleeEvent;
    delete pMeleeTypes;

    if (SETBINDINGS)
        Bindding(false);
}