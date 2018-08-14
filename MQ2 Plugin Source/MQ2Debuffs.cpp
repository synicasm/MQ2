

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// MQ2Debuffs.cpp           |  Added Corruptions and Corrupted (pinkfloydx33)
// Author: PinkFloyd33      |
// Author: s0rcier          |
// Version: 1.2             |
// Date: 20061223           | 
// v2.0 - Eqmule 07-22-2016 - Added string safety.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// This plugin is designed to help with curing detrimental effects. It is used to reports harmful effects,
// number of curse/disease/poison counters and various other detrimentals.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// Debuff                   (bool True if you have debuffs on that have counters on them, false if not)
// Debuff.Poisoned          (int  # of poison counters on you)
// Debuff.Diseased          (int  # of disease counters on you)
// Debuff.Cursed            (int  # of curse counters on you)
// Debuff.Corrupted         (int  # of corruption counters on you)
// Debuff.Poisons           (int  # of poison spells affecting you)
// Debuff.Diseases          (int  # of disease spells affecting you)
// Debuff.Curses            (int  # of curse spells affecting you)
// Debuff.Corruptions       (int  # of corruption spells affecting you)
// Debuff.Count             (int  # of debuffs that need cured, does not include snare)
// Debuff.HPDrain           (int  Amount of HP you are losing per tick from debuffs. This value is POSITIVE)
// Debuff.HPDrain[X]        (int  X= Disease, Poison, Curse, All: Number of specific counters effecting HP)
// Debuff.ManaDrain         (int  Amount of Mana you are losing per tick from debuffs. This value is POSITIVE)
// Debuff.ManaDrain[X]      (int  X= Disease, Poison, Curse, All: Number of specific counters effecting Mana)
// Debuff.EnduranceDrain    (int  Amount of Endurance you are losing per tick from debuffs. This value is POSITIVE)
// Debuff.EnduranceDrain[X] (int  X= Disease, Poison, Curse, All: Number of specific counters effecting Endurance)
// Debuff.Slowed            (bool True if you are Slowed (melee attacks), False if not)
// Debuff.SpellSlowed       (bool True if you are SpellSlowed (spell haste reduction), False if not)
// Debuff.Snared            (bool True if your are Snared, False if not)
// Debuff.ManaCost          (bool True if your Spell Mana Cost has been raised, False if not)
// Debuff.CastingLevel      (bool True if your Effective Casting Level has been reduced, False if not)
// Debuff.HealingEff        (bool True if your Healing Effectiveness has been reduced, False if not)
// Debuff.SpellDmgEff       (bool True if your Spell Damage Effectiveness has been reduced, False if not)
// Debuff.Blind             (bool True if you are blind)
// Debuff.Charmed           (bool True if you are charmed)
// Debuff.Feared            (bool True if you are feared)
// Debuff.Silenced          (bool True if you are silenced)
// Debuff.Invulnerable      (bool True if you are invulnerable)
// Debuff.Detrimentals      (bool True if you have any detrimental effects on you)
// Debuff.Counters          (int  # of poison/disease/curse/corruption counters on yourself)
// Debuff.Rooted            (bool True if you are rooted)
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// The TLO has been enhanced, revamped, recored, to be able to get Debuff Informations others then selfbuff.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// Debuff.X or Debuff[self].X or Debuff[myself].X : return infos for buff from self.
// Debuff[pet].X or Debuff[warder].X              : return infos for buff from pet.
// Debuff[2899].X                                 : return infos for buff for spell "feeblemind".
// Debuff[5682].X                                 : return infos for buff for spell "Chains of Anguish".
// Debuff[5682 2899 887].X                        : return infos for buff from this bufflistid.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

#include "../MQ2Plugin.h"

PreSetup("MQ2Debuff");
PLUGIN_VERSION(2.0);

#define MAXBUFF_WARDER         30
#define MAXBUFF_MYSELF         25

#define COUNTER_DISEASE        35
#define COUNTER_POISON         36
#define COUNTER_CURSE          116
#define COUNTER_CORRUPTION     369

#define DRAIN_HP               0
#define DRAIN_MANA             15
#define DRAIN_ENDU             189

#define DEBUFF_MOVESPEED       3
#define DEBUFF_ATTACKSPEED     11
#define DEBUFF_CASTLEVEL       112
#define DEBUFF_HEALINGEFF      120
#define DEBUFF_SPELLDMGEFF     124
#define DEBUFF_SPELLHASTE      127
#define DEBUFF_SPELLMANACOST   132

#define DEBUFF_ROOTSLOT        2
#define DEBUFF_ROOT            99

#define EFFECT_BLINDNESS       20
#define EFFECT_CHARM           22
#define EFFECT_INVULNERABILITY 40
#define EFFECT_FEAR            23
#define EFFECT_SILENCE         96

#define RETURN_VALUE           1
#define RETURN_DISEASE         2
#define RETURN_POISON          3
#define RETURN_CURSE           4
#define RETURN_CORRUPTION      5
#define RETURN_ALL             6


class MQ2DebuffType *pDebuffType=0;
class MQ2DebuffType : public MQ2Type {
private:
  PSPELL dList[30]; long dSize;
  PSPELL bList[30]; long bSize;
  PSPELL aList[30]; long aSize;
  char   Temp[MAX_STRING];

  int GetSlotDebuff(PSPELL spell)
  {
	  for (int slot = 0; slot < GetSpellNumEffects(spell); slot++) {
		  if (GetSpellAttrib(spell, slot) == COUNTER_DISEASE ||
			  GetSpellAttrib(spell, slot) == COUNTER_POISON ||
			  GetSpellAttrib(spell, slot) == COUNTER_CURSE ||
			  GetSpellAttrib(spell, slot) == COUNTER_CORRUPTION) {
			  return slot + 1;
		  }
	  }
	return 0;
  }

  int GetSpellCount(int CounterType) {
    int counters=0;
    for(int buff=0; buff<dSize; buff++) {
      for(int slot=0; slot<GetSpellNumEffects(dList[buff]); slot++) {
		  if(GetSpellAttrib(dList[buff],slot)==CounterType) {
          counters++;
          break;
        }
      }
    }
    return counters;
  }

  int GetTotalCounters(int CounterType) {
    int counters=0;
    for(int buff=0; buff<dSize; buff++) {
	  for(int slot=0; slot<GetSpellNumEffects(dList[buff]); slot++) {
        if(GetSpellAttrib(dList[buff],slot)==CounterType)
          counters+=(int)GetSpellBase(dList[buff],slot);
      }
    }
    return counters;
  }

  int SlotCalculate(PSPELL spell, int slot) {
    char Buffer[MAX_STRING]={0};
    SlotValueCalculate(Buffer,spell,slot,1);
    return atoi(Buffer);
  }

  bool GetDebuffInfo(int DebuffType, bool HaveCounter, bool BaseCheck) {
    for(long buff=0; buff<dSize; buff++) {
      if(HaveCounter && !GetSlotDebuff(dList[buff])) continue;
      for(int slot=0; slot<GetSpellNumEffects(dList[buff]); slot++) {
        if(GetSpellAttrib(dList[buff],slot)==DebuffType)
        if(!BaseCheck || GetSpellBase(dList[buff],slot)<=0)
        return true;
      }
    }
    return false;
  }

  bool GetEffectInfo(int EffectType) {
    for(long buff=0; buff<aSize; buff++) {
      for(int slot=0; slot<GetSpellNumEffects(aList[buff]); slot++) {
        if(GetSpellAttrib(aList[buff],slot)==EffectType)
        return true;
      }
    }
    return false;
  }

  bool GetRooted() {
    for(int buff=0; buff<dSize; buff++) {
        if(GetSpellAttrib(dList[buff],DEBUFF_ROOTSLOT)==DEBUFF_ROOT) {
          return true;
          break;
        }
      }
   return false;
  }

  int GetDrainInfo(int DrainType, int ReturnType) {
    int amount=0;
    if(ReturnType) {
      int CountC=0; int CountD=0; int CountP=0; int CountCo=0;
      for(int buff=0; buff<dSize; buff++) {
        if(GetSlotDebuff(dList[buff])) {
          int LocalC=0; int LocalD=0; int LocalP=0; int LocalCo=0; int DrainValue=0;
		  for (int slot = 0; slot < GetSpellNumEffects(dList[buff]); slot++) {
			  switch (GetSpellAttrib(dList[buff], slot)) {
				  case COUNTER_CURSE:   LocalC += (int)GetSpellBase(dList[buff], slot); break;
				  case COUNTER_DISEASE: LocalD += (int)GetSpellBase(dList[buff], slot); break;
				  case COUNTER_POISON:  LocalP += (int)GetSpellBase(dList[buff], slot); break;
				  case COUNTER_CORRUPTION: LocalCo += (int)GetSpellBase(dList[buff], slot); break;
				  default:
					  if (GetSpellAttrib(dList[buff], slot) == DrainType && GetSpellBase(dList[buff], slot) < 0) {
						  DrainValue = SlotCalculate(dList[buff], slot);
					  }
			  }
		  }
          if(DrainValue) {
            amount+=DrainValue;
            CountC+=LocalC;
            CountD+=LocalD;
            CountP+=LocalP;
         CountCo+=LocalCo;
          }
        }
      }
      switch(ReturnType) {
        case RETURN_ALL:     return CountC+CountD+CountP+CountCo;
        case RETURN_CURSE:   return CountC;
        case RETURN_DISEASE: return CountD;
        case RETURN_POISON:  return CountP;
      case RETURN_CORRUPTION: return CountCo;
      }
    }
    return amount;
  }

  bool GetSlowedState() {
    for(long buff=0; buff<dSize; buff++) {
      for(int slot=0; slot<GetSpellNumEffects(dList[buff]); slot++) {
        if(GetSpellAttrib(dList[buff],slot)!=DEBUFF_ATTACKSPEED) continue;
        int Slow=((GetSpellMax(dList[buff],slot)) ? GetSpellMax(dList[buff],slot): GetSpellBase(dList[buff],slot))-100;
        if(Slow<0) return true;
      }
    }
    return false;
  }

  int ReturnType(PCHAR Index) {
    if(!Index[0])                 return RETURN_VALUE;
    if(!_stricmp(Index,"Disease")) return RETURN_DISEASE;
    if(!_stricmp(Index,"Poison"))  return RETURN_POISON;
    if(!_stricmp(Index,"Curse"))   return RETURN_CURSE;
   if(!_stricmp(Index,"Corruption")) return RETURN_CORRUPTION;
    if(!_stricmp(Index,"All"))     return RETURN_ALL;
    return 0;
  }

public:
  enum DebuffMembers {
    Poisoned=1,
    Diseased=2,
    Cursed=3,
    Poisons=4,
    Diseases=5,
    Curses=6,
    Count=7,
    HPDrain=8,
    EnduranceDrain=9,
    ManaDrain=10,
    Slowed=11,
    SpellSlowed=12,
    CastingLevel=13,
    HealingEff=14,
    SpellDmgEff=15,
    Snared=16,
    ManaCost=17,
    Blind=18,
    Charmed=19,
    Feared=20,
    Silenced=21,
    Invulnerable=22,
    Detrimentals=23,
    Counters=24,
    Rooted=25,
   Corruptions=26,
   Corrupted=27,
  };

  MQ2DebuffType():MQ2Type("Debuff") {
    TypeMember(Poisoned);
    TypeMember(Diseased);
    TypeMember(Cursed);
    TypeMember(Poisons);
    TypeMember(Diseases);
    TypeMember(Curses);
    TypeMember(Count);
    TypeMember(HPDrain);
    TypeMember(EnduranceDrain);
    TypeMember(ManaDrain);
    TypeMember(Slowed);
    TypeMember(SpellSlowed);
    TypeMember(CastingLevel);
    TypeMember(HealingEff);
    TypeMember(SpellDmgEff);
    TypeMember(Snared);
    TypeMember(ManaCost);
    TypeMember(Blind);
    TypeMember(Charmed);
    TypeMember(Feared);
    TypeMember(Silenced);
    TypeMember(Invulnerable);
    TypeMember(Detrimentals);
    TypeMember(Counters);
    TypeMember(Rooted);
   TypeMember(Corruptions);
   TypeMember(Corrupted);
  }

  void SetBuffs(PCHAR Index) {
    ZeroMemory(&dList,sizeof(dList)); dSize=0;
    ZeroMemory(&bList,sizeof(bList)); bSize=0;
    ZeroMemory(&aList,sizeof(aList)); aSize=0;
    if(!Index[0] || !_stricmp(Index,"self") || !_stricmp(Index,"myself")) {
      for(int b=0; b<MAXBUFF_MYSELF; b++) {
        if(PSPELL spell=GetSpellByID(GetCharInfo2()->Buff[b].SpellID))
          if(spell->DurationCap>0) {
            ((spell->SpellType)?bList[bSize++]:dList[dSize++])=spell;
            aList[aSize++]=spell;
          }
      }
      return;
    }
    if(!_stricmp(Index,"pet") || !_stricmp(Index,"warder")) {
      if(pPetInfoWnd && GetCharInfo() && GetCharInfo()->pSpawn && GetCharInfo()->pSpawn->PetID>0)
        for(int b=0; b<MAXBUFF_WARDER; b++) {
          if(PSPELL spell=GetSpellByID(((PEQPETINFOWINDOW)pPetInfoWnd)->Buff[b]))
            if(spell->DurationCap>0) {
              ((spell->SpellType)?bList[bSize++]:dList[dSize++])=spell;
              aList[aSize++]=spell;
            }
        }
      return;
    }
    char BuffID[MAX_STRING];
    for(int b=0; b<MAXBUFF_WARDER; b++) {
      GetArg(BuffID,Index,b+1);
      if(PSPELL spell=GetSpellByID(atol(BuffID)))
        if(spell->DurationCap>0) {
          ((spell->SpellType)?bList[bSize++]:dList[dSize++])=spell;
          aList[aSize++]=spell;
        }
    }
  }

  bool GetMember(MQ2VARPTR VarPtr, PCHAR Member, PCHAR Index, MQ2TYPEVAR &Dest) {
    if(PMQ2TYPEMEMBER pMember=MQ2DebuffType::FindMember(Member))
      switch((DebuffMembers)pMember->ID) {
        case Poisoned:
          Dest.Type=pIntType;
          Dest.Int =GetTotalCounters(COUNTER_POISON);
          return true;
        case Diseased:
          Dest.Type=pIntType;
          Dest.Int=GetTotalCounters(COUNTER_DISEASE);
          return true;
        case Cursed:
          Dest.Type=pIntType;
          Dest.Int=GetTotalCounters(COUNTER_CURSE);
          return true;
      case Corrupted:
          Dest.Type=pIntType;
          Dest.Int=GetTotalCounters(COUNTER_CORRUPTION);
          return true;
        case Poisons:
          Dest.Type=pIntType;
          Dest.Int=GetSpellCount(COUNTER_POISON);
          return true;
        case Diseases:
          Dest.Type=pIntType;
          Dest.Int=GetSpellCount(COUNTER_DISEASE);
          return true;
        case Curses:
          Dest.Type=pIntType;
          Dest.Int=GetSpellCount(COUNTER_CURSE);
          return true;
        case Corruptions:
          Dest.Type=pIntType;
          Dest.Int=GetSpellCount(COUNTER_CORRUPTION);
          return true;
        case Count:
          Dest.Type=pIntType;
          Dest.Int=GetSpellCount(COUNTER_POISON) +
                   GetSpellCount(COUNTER_DISEASE) +
                   GetSpellCount(COUNTER_CURSE) +
               GetSpellCount(COUNTER_CORRUPTION);
          return true;
        case HPDrain:
          Dest.Type=pIntType;
          Dest.Int=GetDrainInfo(DRAIN_HP,ReturnType(Index));
          return true;
        case EnduranceDrain:
          Dest.Type=pIntType;
          Dest.Int=GetDrainInfo(DRAIN_ENDU,ReturnType(Index));
          return true;
        case ManaDrain:
          Dest.Type=pIntType;
          Dest.Int=GetDrainInfo(DRAIN_MANA,ReturnType(Index));
          return true;
        case Slowed:
          Dest.Type=pBoolType;
          Dest.DWord=GetSlowedState();
          return true;
        case SpellSlowed:
          Dest.Type=pBoolType;
          Dest.DWord=GetDebuffInfo(DEBUFF_SPELLHASTE,true,true);
          return true;
        case CastingLevel:
          Dest.Type=pBoolType;
          Dest.DWord=GetDebuffInfo(DEBUFF_CASTLEVEL,true,true);
          return true;
        case HealingEff:
          Dest.Type=pBoolType;
          Dest.DWord=GetDebuffInfo(DEBUFF_HEALINGEFF,true,true);
          return true;
        case SpellDmgEff:
          Dest.Type=pBoolType;
          Dest.DWord=GetDebuffInfo(DEBUFF_SPELLDMGEFF,true,true);
          return true;
        case ManaCost:
          Dest.Type=pBoolType;
          Dest.DWord=GetDebuffInfo(DEBUFF_SPELLMANACOST,true,true);
          return true;
        case Snared:
          Dest.Type=pBoolType;
          Dest.DWord=GetDebuffInfo(DEBUFF_MOVESPEED,false,true);
          return true;
        case Rooted:
          Dest.Type=pBoolType;
          Dest.DWord=GetRooted();
          return true;
        case Blind:
          Dest.Type=pBoolType;
          Dest.DWord=GetDebuffInfo(EFFECT_BLINDNESS,false,false);
          return true;
        case Charmed:
          Dest.Type=pBoolType;
          Dest.DWord=GetDebuffInfo(EFFECT_CHARM,false,false);
          return true;
        case Feared:
          Dest.Type=pBoolType;
          Dest.DWord=GetDebuffInfo(EFFECT_FEAR,false,false);
          return true;
        case Silenced:
          Dest.Type=pBoolType;
          Dest.DWord=GetDebuffInfo(EFFECT_SILENCE,false,false);
          return true;
        case Invulnerable:
          Dest.Type=pBoolType;
          Dest.DWord=GetEffectInfo(EFFECT_INVULNERABILITY);
          return true;
        case Detrimentals:
          Dest.Type=pBoolType;
          Dest.DWord=(dSize)?true:false;
          return true;
        case Counters:
          Dest.Type=pIntType;
          Dest.Int =GetTotalCounters(COUNTER_POISON)  +
                    GetTotalCounters(COUNTER_DISEASE) +
                    GetTotalCounters(COUNTER_CURSE) +
               GetTotalCounters(COUNTER_CORRUPTION);
          return true;
    }
    Dest.Type=pStringType;
    Dest.Ptr=&Temp[0];
    strcpy_s(Temp,"NULL");
    return true;
  }

  bool FromData(MQ2VARPTR &VarPtr, MQ2TYPEVAR &Source) {
    return false;
  }

  bool FromString(MQ2VARPTR &VarPtr, PCHAR Source) {
    return false;
  }

  bool ToString(MQ2VARPTR VarPtr, PCHAR Destination) {
    bool HasDebuff=false;
    for(long buff=0; !HasDebuff && buff<dSize; buff++)
      if(GetSlotDebuff(dList[buff])) HasDebuff=true;
    strcpy_s(Destination,MAX_STRING,HasDebuff?"TRUE":"FALSE");
    return true;
  }

  ~MQ2DebuffType() { }
};

BOOL dataDebuff(PCHAR Index, MQ2TYPEVAR &Dest) {
  Dest.DWord=1;
  Dest.Type=pDebuffType;
  pDebuffType->SetBuffs(Index);
  return true;
}

PLUGIN_API VOID InitializePlugin(VOID) {
  pDebuffType = new MQ2DebuffType;
  AddMQ2Data("Debuff",dataDebuff);
}

PLUGIN_API VOID ShutdownPlugin(VOID) {
  RemoveMQ2Data("Debuff");
  delete pDebuffType;
}