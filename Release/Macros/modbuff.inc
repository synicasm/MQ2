
#event OUTDOORS "#*#You can only cast this spell in the outdoors#*#"
#event OUTDOORS "#*#You can't levitate in this zone#*#"

Sub LoadABINIVals
   /declare a int local
   /call LoadINIVar AdvBuff ABCount 0 ABCount "${IniFile}" int
   /call LoadINIVar AdvBuff ABNewFormat 1 ABNewFormat "${IniFile}" int
   /declare ABCheckTimer timer outer
   /declare ABAliasList string outer
   /if (${ABCount}) {
      /call LoadINIVar AdvBuff ABMobMax 18 ABMobMax "${IniFile}" int
      /call LoadINIVar AdvBuff ABCheckTime 8 ABCheckTime "${IniFile}" int
      /declare DoBuffsCBT bool outer FALSE
      /declare ABMBL[${ABMobMax}] string outer
      /declare ABGem[${ABCount}] string outer
      /declare ABSpell[${ABCount}] string outer
      /declare ABSpellFoci[${ABCount}] string outer
      /declare ABDurMod[${ABCount}] string outer
      /declare ABSpellMinMana[${ABCount}] int outer
      /declare ABTarCnt[${ABCount}] int outer
      /declare ABTarType[${ABCount}] string outer
      /declare ABSpellAlias[${ABCount}] string outer
      /declare ABAnnounce[${ABCount}] string outer
      /declare ABScriptTime[${ABCount}] int outer
      /declare ABRecast[${ABCount}] bool outer
      /declare ABSpellIcon[${ABCount}] string outer
      /declare ABPreCondition[${ABCount}] string outer
      /declare tSpellID int local
      /declare ABGemInUse string outer
      /declare SMPetCast bool outer FALSE
      /declare OutDoorsFail bool outer FALSE
      /for a 1 to ${ABCount}
         /if (${ABNewFormat}) {
            /call LoadINIVar AB${a} Gem " " ABGem[${a}] ${IniFile}
            /call LoadINIVar AB${a} Spell " " ABSpell[${a}] ${IniFile}
            /call LoadINIVar AB${a} SpellFoci " " ABSpellFoci[${a}] ${IniFile}
            /call LoadINIVar AB${a} DurMod 0 ABDurMod[${a}] ${IniFile}
            /call LoadINIVar AB${a} SpellAlias " " ABSpellAlias[${a}] ${IniFile}
            /call LoadINIVar AB${a} Announce " " ABAnnounce[${a}] ${IniFile}
            /call LoadINIVar AB${a} SpellMinMana 0 ABSpellMinMana[${a}] ${IniFile}
            /call LoadINIVar AB${a} TarCnt 0 ABTarCnt[${a}] ${IniFile}
            /call LoadINIVar AB${a} TarType "tank war shd pal rng mnk rog brd bst ber shm clr dru wiz mag enc nec self mypet grp pet cbt idle" ABTarType[${a}] ${IniFile}
            /call LoadINIVar AB${a} Recast FALSE ABRecast[${a}] ${IniFile}
            /call LoadINIVar AB${a} SpellIcon " " ABSpellIcon[${a}] ${IniFile}
            /call LoadINIVar AB${a} PreCondition "TRUE" ABPreCondition[${a}] ${IniFile}
         } else {    
            /call LoadINIVar AdvBuff ABGem${a} " " ABGem[${a}] ${IniFile}
            /call LoadINIVar AdvBuff ABSpell${a} " " ABSpell[${a}] ${IniFile}
            /call LoadINIVar AdvBuff ABSpellFoci${a} " " ABSpellFoci[${a}] ${IniFile}
            /call LoadINIVar AdvBuff ABDurMod${a} 0 ABDurMod[${a}] ${IniFile}
            /call LoadINIVar AdvBuff ABSpellAlias${a} " " ABSpellAlias[${a}] ${IniFile}
            /call LoadINIVar AdvBuff ABAnnounce${a} " " ABAnnounce[${a}] ${IniFile}
            /call LoadINIVar AdvBuff ABSpellMinMana${a} 0 ABSpellMinMana[${a}] ${IniFile}
            /call LoadINIVar AdvBuff ABTarCnt${a} 0 ABTarCnt[${a}] ${IniFile}
            /call LoadINIVar AdvBuff ABTarType${a} "tank war shd pal rng mnk rog brd bst ber shm clr dru wiz mag enc nec self mypet grp pet cbt idle" ABTarType[${a}] ${IniFile}
            /call LoadINIVar AdvBuff ABRecast${a} FALSE ABRecast[${a}] ${IniFile}
            /call LoadINIVar AdvBuff ABSpellIcon${a} " " ABSpellIcon[${a}] ${IniFile}
            /call LoadINIVar AdvBuff ABPreCondition${a} "TRUE" ABPreCondition[${a}] ${IniFile}
         }
         /if (${ABSpellIcon[${a}].Length}>1 && !${Spell[${ABSpellIcon[${a}]}].ID}) /echo ABSpellIcon${a} does not evaluate to a spell name.  Please check your spelling.
         /if (${ABTarType[${a}].Find[cbt]}) /varset DoBuffsCBT TRUE
         /varset ABAliasList ${ABAliasList} ${ABSpellAlias[${a}]}|
         /if (${Select[${ABGem[${a}]},1,2,3,4,5,6,7,8,9,10,11,12,13]} && ${ABSpell[${a}].Length}) {
            /if (${Me.Book[${Me.Book[${ABSpell[${a}]}]}].ID}) {
               /varset ABSpell[${a}] ${Me.Book[${Me.Book[${ABSpell[${a}]}]}].Name}
            } else /if (${Me.Book[${Me.Book[${ABSpell[${a}]} ]}].ID}) {
               /varset ABSpell[${a}] ${Me.Book[${Me.Book[${ABSpell[${a}]} ]}].Name}
            } else /if (${Me.Book[${Spell[${ABSpell[${a}]}].RankName}]}) {
               /varset ABSpell[${a}] ${Spell[${ABSpell[${a}]}].RankName}
            } else {
               /bc Spell ${ABSpell[${a}]} Not found in your book. Please check your .ini settings.
               /varset ABTarCnt[${a}] 0
            }
         } else /if (${ABGem[${a}].Equal[alt]}) {
            /if (!${Me.AltAbility[${ABSpell[${a}]}].ID} && !${Me.Shrouded}) {
               /bc You don't have the AltAbility ${ABSpell[${a}]}. Please check your .ini settings.
               /varset ABTarCnt[${a}] 0
            }
         } else /if (${ABGem[${a}].Equal[disc]}) {
            /if (${Me.CombatAbility[${Spell[${ABSpell[${a}]}].RankName}]}) {
               /varset ABSpell[${a}] ${Spell[${ABSpell[${a}]}].RankName}
            } else {
               /bc You don't have the ${ABSpell[${a}]} Disc. Please check your .ini settings.
               /varset ABTarCnt[${a}] 0
            }
         }
         /if ((${Spell[${tSpellID}].TargetType.Find[Group]} || ${ABTarType[${a}].Find[self]}) && ${ABRecast[${a}]}) /varset ABRecast[${a}] FALSE
      /next a
   }
/return


Sub ABBuffCheck
   /varset ABCheckTimer ${ABCheckTime}s
   /if (${MBDebug}) /echo Macro.RunTime:${Macro.RunTime} - Sub:ABBuffCheck

   /declare a int local
   /declare b int local
   /declare x int local
   /declare tSpellID int local
   /declare tGem int local
   /declare tTargetID int local
   /declare tName string local
   /declare tBot bool local
   /declare zPetID int local
  
   /if (!${Defined[tABNBCnt]}) {
      /declare tABNBCnt int outer
      /declare tABCnt int outer
      /declare sABNBClnt string outer
   } 
   /varset tABNBCnt ${NetBots.Counts}
   /varset tABCnt ${NetBots.Counts} 
   /varset sABNBClnt

   /call ADSpawnCheck
   /if (${ADMobCount} && (${DoMelee} && !${ACState} || !${DoBuffsCBT})) /return

   /for a 1 to ${NetBots.Counts}
      /varset sABNBClnt ${sABNBClnt}${NetBots.Client.Arg[${a}]}|
   /next a
   /if (${Group.Members}) {
      /for a 0 to ${Group.Members}
         /if (!${NetBots[${Group.Member[${a}].Name}].ID} && ${Spawn[${Group.Member[${a}].Name}].ID}) {
            /varset sABNBClnt ${sABNBClnt}${Group.Member[${a}].Name}|
            /varcalc tABCnt ${tABCnt}+1
         }
      /next a
   }

   /for a 1 to ${ABCount}
      /if (!${DoBuffs} || ${MBPause} || ${Me.State.Equal[feign]}) /return
      /if (${FollowFlag} && (${Spawn[FollowID].Distance}>${FollowDistance} || ${Me.Moving})) /return
      /if (${Cursor.ID}) /call ClearCursor
      /if (!${ABTarCnt[${a}]}) /goto :tNextBuff
      /if (${ABSpell[${a}].Left[5].Equal[Yaulp]} && ${Me.Mount.ID}) /goto :tNextBuff
      /varset tGem 0
      /varset tSpellID 0

      /if (${Select[${ABGem[${a}]},1,2,3,4,5,6,7,8,9,10,11,12,13]}) {
|/echo was Spell
         /varset tSpellID ${Me.Book[${Me.Book[${ABSpell[${a}]}]}].ID}
         /varset tGem ${ABGem[${a}]}
         /if (${Me.CurrentMana}+${Me.ManaRegen}<${Spell[${tSpellID}].Mana} || ${Me.PctMana}<${ABSpellMinMana[${a}]}) /goto :tNextBuff
         /if (${Me.Class.ShortName.Equal[BRD]}) {
            /if (${ABTarType[${a}].Find[aura]}) {
               /if (!${Me.Aura[${ABSpell[${a}]}].Name.Equal[${ABSpell[${a}]}]} && ${ABSpellAlias[${a}].Length}) /call CastCall ${Me.CleanName} "cast ${ABSpellAlias[${a}]}"
               /goto :tNextBuff
            }
            /if (${ABTarCnt[${a}]}>1 && ${Math.Calc[${Group.Members}+1]}<${ABTarCnt[${a}]}) /goto :tNextBuff
            /if (${Me.Gem[${tGem}].ID}!=${tSpellID}) {
               /delay 2s ${Cast.Status.Equal[i]}
               /squelch /memorize "${Spell[${tSpellID}]}|gem${tGem}"
               /delay 14s ${Cast.Status.Equal[i]}
               /if (${Me.Gem[${tGem}].ID}!=${tSpellID}) { 
                  /bc BUFF - ${ABSpell[${a}]} - FAILED MEMORIZE
                  /goto :tNextBuff
               }
            }
            /if (${ABTarType[${a}].Find[cbt]} && !${ABTarType[${a}].Find[idle]}) {
               /if (!${BardCombatGems.Find[${tGem}]}) /varset BardCombatGems ${BardCombatGems} ${tGem}
               /if (!${ADMobCount}) /goto :tNextBuff
            } else /if (${ABTarType[${a}].Find[cbt]} && ${ABTarType[${a}].Find[idle]}) {
               /if (!${BardCombatGems.Find[${tGem}]}) /varset BardCombatGems ${BardCombatGems} ${tGem}
               /if (!${BardRestGems.Find[${tGem}]}) /varset BardRestGems ${BardRestGems} ${tGem}
            } else /if (!${ABTarType[${a}].Find[cbt]}) {
               /if (!${BardRestGems.Find[${tGem}]}) /varset BardRestGems ${BardRestGems} ${tGem}
               /if (${ADMobCount}) /goto :tNextBuff
            }
            /if (${BardRestGems.Find[${tGem}]} || ${BardCombatGems.Find[${tGem}]}) /goto :tNextBuff
            /if (${ABAnnounce[${a}].Length}) {
               /if (${ABAnnounce[${a}].Length}>=5) {
                  /call ParseAnnounce "${ABAnnounce[${a}]}" "${Me.Class}" "${ABSpell[${a}]}"
               } else {
                  /docommand ${ABAnnounce[${a}]} Buffed: -[ ${Me.Class} ]- with ${ABSpell[${a}]}]}
                  /goto :tNextBuff
               }
            }
            /if (${Twist}) /squelch /stoptwist
         }
      } else /if (${ABGem[${a}].Find[item]} && ${Bool[${FindItem[=${ABSpell[${a}]}]}]}) {
|/echo was Item
         /if (${FindItem[=${ABSpell[${a}]}].Timer}) /goto :tNextBuff
         /varset tSpellID ${FindItem[=${ABSpell[${a}]}].Spell.ID}
         /varset tGem 20
      } else /if (${ABGem[${a}].Equal[alt]} && ${Me.AltAbility[${ABSpell[${a}]}].ID}) {
|/echo was Alt
         /if (!${Me.AltAbilityReady[${ABSpell[${a}]}]} || ${Me.CurrentMana}<${Me.AltAbility[${ABSpell[${a}]}].Spell.Mana}) /goto :tNextBuff
         /varset tSpellID ${Me.AltAbility[${ABSpell[${a}]}].Spell.ID}
         /varset tGem 30
      }
      /if (${ABTarType[${a}].Find[petspell]} || ${ABTarType[${a}].Find[petcast]}) {
         /if (!${Me.Pet.ID} && ${DoPet} && ${ABTarCnt[${a}]}) {
            /if (${ADMobCount} && !${ABTarType[${a}].Find[cbt]}) /goto :tNextBuff
            /echo Casting pet.
            /varset MBCurrentSpell AB|${a}
            /if (${Me.AltAbilityReady[Companion's Suspension]} && !${SMPetCast}) {
               /varset SMPetCast TRUE
               /call MQ2Cast "Companion's Suspension" alt 3s CastCheck
            }
            /delay 1s ${Me.Pet.ID}
            /if (!${Me.Pet.ID}) /call MQ2Cast "${ABSpell[${a}]}" ${If[${Select[${ABGem[${a}]},1,2,3,4,5,6,7,8,9,10,11,12,13]},gem,]}${ABGem[${a}]} 5s CastCheck
            /delay 1s ${Me.Pet.ID}
         }
         /goto :tNextBuff
      }
      /if (${ABSpellIcon[${a}].Length}>1) {
         /varset tSpellID ${Spell[${ABSpellIcon[${a}]}].ID}
         /if (!${Spell[${ABSpellIcon[${a}]}].ID}) /goto :tNextBuff
      }
      /if (${ABTarType[${a}].Find[self]}) {
         /varset tBot TRUE
         /varset tTargetID ${Me.ID}
         /call BuffThem ${a} ${tSpellID} ${tGem} ${tTargetID} ${tBot}
      } else /if (${ABTarType[${a}].Find[mypet]}) {
         /if (${Me.Pet.ID} && ${Select[${Me.Class.ShortName},shm,nec,mag,bst,enc,shd]}) {
            /varset tBot TRUE
            /varset tTargetID ${Me.Pet.ID}
            /call BuffThem ${a} ${tSpellID} ${tGem} ${tTargetID} ${tBot}
         }
      } else /if (${ABTarType[${a}].Find[tank]}) {
         /varset tTargetID ${Spawn[=${TankName}].ID}
         /if (${tTargetID}) {
            /varset tBot ${If[${NetBots[${TankName}].ID},TRUE,FALSE]}
            /call BuffThem ${a} ${tSpellID} ${tGem} ${tTargetID} ${tBot}
         }
      } else {
         /for b 1 to ${tABCnt}
            /varset tName ${sABNBClnt.Arg[${b},|]}
            /if (${b}>${tABNBCnt}) {
               /varset tBot FALSE
               /varset tTargetID ${Group.Member[${tName}].ID}
               /varset zPetID ${Group.Member[${tName}].Pet.ID}
            } else {
               /varset tBot TRUE
               /varset tTargetID ${NetBots[${tName}].ID}
               /varset zPetID ${NetBots[${tName}].PetID}
            }
            /if (${tTargetID} && ((!${ABTarType[${a}].Find[grp]} && !${ABTarType[${a}].Find[oog]}) || (${ABTarType[${a}].Find[grp]} && (${Group.Member[${tName}].Index} || ${tName.Equal[${Me.CleanName}]})) || (${ABTarType[${a}].Find[oog]} && !${Group.Member[${tName}].Index} && ${tName.NotEqual[${Me.CleanName}]}))) {
               /if (${ABTarType[${a}].Find[${Spawn[${tTargetID}].Class.ShortName}]}) /call BuffThem ${a} ${tSpellID} ${tGem} ${tTargetID} ${tBot}
               /if (${zPetID} && ${ABTarType[${a}].Find[pet]}) {
                  /if (${Select[${Spawn[${tTargetID}].Class.ShortName},shm,nec,mag,bst,enc,shd]}) {
                     /varset tTargetID ${zPetID}
                     /call BuffThem ${a} ${tSpellID} ${tGem} ${tTargetID} ${tBot}
                  }
               }
            }
:tNextBot
         /next b
      }
:tNextBuff
   /next a
/return


Sub BuffThem(int a, int tSpellID, int tGem, int tTargetID, bool tBot) 
   /declare c int local
   /declare d int local
   /declare tSpellRange int local
   /declare tSpellDuration int local
   /declare tSpellDelay int local
   /declare tSpellSID string local
   /declare sVar string local
   /declare sVarL string local
   /declare sVarM string local
   /declare sVarR string local
   /declare sVarT string local
   /declare sVarSpell string local
   /declare tTime int local
   /declare tTarCnt int local
   /declare tName string local
   /declare tABMob int local
   /declare ABtTargetID int local
   /declare tBuffs string local
   /declare xFoundBot bool local TRUE

   /if (!${DoBuffs} || ${MBPause} || ${Me.State.Equal[feign]}) /return
   /if (${FollowFlag}) {
      /if (${Me.Moving}) /return
      /if (${Spawn[FollowID].Distance}>${FollowDistance}) /return
   }
   /varset tSpellSID ${tSpellID}
   /varset tName ${Spawn[${tTargetID}].CleanName}
   /varset tSpellRange ${If[${Spell[${tSpellID}].AERange},${Spell[${tSpellID}].AERange},${Spell[${tSpellID}].Range}]}
   /if (!${tTargetID} || !${tName.Length}) /return

:tFindSlot
   /varset xFoundBot FALSE
   /for c 1 to ${ABMobMax}
      /if (${tName.Equal[${ABMBL[${c}].Arg[1,|]}]}) {
         /varset tABMob ${c}
         /varset xFoundBot TRUE
         /break
      }
      /if (!${ABMBL[${c}].Length}) {
         /varset ABMBL[${c}] ${tName}|
         /varset tABMob ${c}
         /varset xFoundBot TRUE
         /break
      }
   /next c
   /if (${xFoundBot}) /goto :tFoundBot
   /for c 1 to ${ABMobMax}
       /if (${ABMBL[${c}].Length} && !${Spawn[${ABMBL[${c}].Arg[1,|]}].ID}) {
          /varset ABMBL[${c}]  
          /varset tName DoneScrubbing
       } 
   /next c
   /if (${tName.Equal[donescrubbing]}) /goto :tFindSlot
   /bc CAN NOT TRACK ANY MORE
   /return

:tFoundBot
   /if (${ADMobCount} && ((${DoMelee} && !${ACState}) || !${DoBuffsCBT})) /return
   /if ((!${ABTarType[${a}].Find[cbt]} && ${ADMobCount}) || (${ABTarType[${a}].Find[cbt]} && !${ABTarType[${a}].Find[idle]} && !${ADMobCount})) /return
   /if (${ABPreCondition[${a}].Length}>4 && ${ABPreCondition[${a}].Left[4].NotEqual[TRUE]}) {
      /call IniCommand "${ABPreCondition[${a}]}"
      /if (${Macro.Return.Equal[FALSE]}) /return
   }
   /if (${ABTarCnt[${a}]}>1) {
      /varset tTarCnt 0
      /for d 1 to ${tABCnt}
         /if (${d}<=${tABNBCnt}) {
            /if (${NetBots[${sABNBClnt.Arg[${d},|]}].Grouped} && (${ABTarType[${a}].Find[${NetBots[${sABNBClnt.Arg[${d},|]}].Class.ShortName}]} || ${ABTarType[${a}].Find[self]}) && ${Spawn[radius ${tSpellRange} PC ${sABNBClnt.Arg[${d},|]}].ID}) /varcalc tTarCnt ${tTarCnt}+1
         } else /if (${Spawn[${sABNBClnt.Arg[${d},|]}].ID}) {
            /if (${Spawn[radius ${tSpellRange} ${sABNBClnt.Arg[${d},|]}].ID} && (${ABTarType[${a}].Find[${Spawn[${sABNBClnt.Arg[${d},|]}].Class.ShortName}]} || ${ABTarType[${a}].Find[self]})) /varcalc tTarCnt ${tTarCnt}+1
         }
      /next d
      /if (${tTarCnt}<${ABTarCnt[${a}]}) /return
   }
   /if (${ABTarType[${a}].Find[aura]}) {
      /if (${ABTarType[${a}].Find[self]}) { 
         /if (${ABGem[${a}].Equal[disc]}) {
            /if (!${Spell[${Me.Aura}].ID}) {
               /disc ${Spell[${ABSpell[${a}]}].ID}
               /delay 1s ${Me.Casting.ID}
               /delay ${Spell[${ABSpell[${a}]}].MyCastTime.Float}s !${Me.Casting.ID}
            }
            /return
         }
         /if (!${ABSpell[${a}].Find[${Me.Aura[1]}]} && !${ABSpell[${a}].Find[${Me.Aura[2]}]} && (!${Spell[${Me.Aura}].ID} || (${Me.AltAbility[Auroria Mastery]} || ${Me.AltAbility[Spirit Mastery]}) && (!${Spell[${Me.Aura[1]}].ID} || !${Spell[${Me.Aura[2]}].ID}))) /goto :tSetsVar
      } else /if (${Me.Class.ShortName.Equal[mag]} && ${ABTarType[${a}].Find[mypet]}) { 
         /if (!${Me.PetBuff[${ABSpell[${a}]} Effect]} && ${NetBots[${Me.CleanName}].StacksPet[${ABSpell[${a}]} Effect]}) /goto :tSetsVar
      }
      /return
   } else /if (${ABGem[${a}].Equal[disc]}) {
|/echo was disc (not aura)
      /if (${Me.PctEndurance}<${ABSpellMinMana[${a}]} || ${ABScriptTime[${a}]}>${Macro.RunTime} || !${Me.CombatAbilityReady[${ABSpell[${a}]}]}) /return
      /if (${ABScriptTime[${a}]}<=${Macro.RunTime}) {
         /disc ${Spell[${ABSpell[${a}]}].ID}
         /delay 1s ${Me.Casting.ID}
         /delay ${Spell[${ABSpell[${a}]}].MyCastTime.Float}s !${Me.Casting.ID}
         /varset ABScriptTime[${a}] ${Math.Calc[${ABDurMod[${a}]}+${Macro.RunTime}]}
      }
      /return
   }
:tSetsVar
   /if (${ABGem[${a}].Equal[script]}) {
|/echo was Script
      /if (${ABScriptTime[${a}]}<=${Macro.RunTime}) {
         /call MBScript ${ABSpell[${a}]} "${tName}" ${tTargetID} ${a}
         /varset ABScriptTime[${a}] ${Math.Calc[${ABDurMod[${a}]}+${Macro.RunTime}]}
      }
      /goto :tNextBot
   }

   /if (!${tGem} || !${tSpellID}) /return
   /if (!${DoBuffs} || ${Me.CurrentMana}<${Spell[${tSpellID}].Mana} && ${tGem}<20 || ${Me.PctMana}<${ABSpellMinMana[${a}]} || ${Spell[${tSpellID}].TargetType.Find[group]} && ${Me.GroupSize}<${ABTarCnt[${a}]} && ${ABTarCnt[${a}]}>1) /return
   /if (${Select[${Spell[${tSpellID}].TargetType},self,single]} && (${tTargetID}==${Me.ID} || ${ABTarType[${a}].Find[self]}) && (${Me.Buff[${Spell[${tSpellID}].Name}].Duration.Ticks}>6 || !${Me.Buff[${Spell[${tSpellID}]}].ID} && !${Me.FreeBuffSlots} || !${NetBots[${Me.CleanName}].Stacks[${tSpellID}]})) /goto :tNextBot
   /if ((${Spell[${tSpellID}].Level}==255 && ${Spell[${ABSpellIcon[${a}]}].ID}==${tSpellID})) /goto :tNextBot
   /varset sVar ${ABMBL[${tABMob}].Mid[${ABMBL[${tABMob}].Find[b${tSpellSID}]}|,14]}
   /varset sVarT ${sVar.Arg[2,|]}
   /varset tTime ${sVarT.Mid[2,${sVarT.Length}]}
   /if (${tBot}) {
      /varset tBuffs ${If[${Spawn[${tTargetID}].Type.Equal[pc]},${NetBots[${tName}].Buff} ${NetBots[${tName}].ShortBuff},${NetBots[${Spawn[${tTargetID}].Master.CleanName}].PetBuff}]}
      /if (${Spawn[${tTargetID}].Type.Equal[pc]} && (!${tBuffs.Find[${tSpellID}]} && !${NetBots[${tName}].FreeBuffSlots})) /goto :tNextBot
      /if (${tBuffs.Find[${tSpellID}]}) {
         /if (!${tTime} || ${Spawn[${tTargetID}].Type.Equal[pet]}) /goto :tNextBot
      } else {
         /if (${tBuffs.Length} && ${ABTarType[${a}].Find[${Spawn[${tTargetID}].Class.ShortName}]} || (${ABTarType[${a}].Find[self]} && ${tTargetID}==${Me.ID}) || (${ABTarType[${a}].Find[pet]} && ${Spawn[${tTargetID}].Master.ID}) || ${ABTarType[${a}].Find[tank]}) {
            /if (${Spawn[${tTargetID}].Type.Equal[pc]}) {
               /if (!${NetBots[${tName}].Stacks[${tSpellID}]}) {
|                 /echo ${ABSpell[${a}] - Won't stack on ${tName}
                  /goto :tNextBot
               }
            } else {
               /if (!${NetBots[${Spawn[${tTargetID}].Master.CleanName}].StacksPet[${tSpellID}]}) /goto :tNextBot
            }
            /varset tTime 0
         }
      }
   }
   /if (${tName.Equal[${Me.CleanName}]} && ${ABTarType[${a}].Find[self]}) {
      /if (${NetBots[${Me.CleanName}].Stacks[${tSpellID}]} && ${Me.FreeBuffSlots} && !${Me.Buff[${Spell[${tSpellID}]}].ID} && (${ABTarCnt[${a}]}<=${SpawnCount[pc group radius ${tSpellRange}]} || ${ABTarCnt[${a}]}==1)) /varset tTime 0
   }
   /if (${tTime}>${Macro.RunTime} || ${Spawn[${tTargetID}].Distance3D}>${tSpellRange}) /goto :tNextBot
   /if (${tGem}<19) {
      /if (${Me.Gem[${tGem}].ID}!=${tSpellID}) {
         /if (${ABGemInUse.Length}>4 && ${ABGemInUse.Arg[1,|].Equal[${ABGem[${a}]}]} && ${ABGemInUse.Arg[3,|]}>${Macro.RunTime}) /return
         /squelch /memorize "${ABSpell[${a}]}|gem${tGem}"
|        /delay 1s ${Cast.Status.Equal[m]}
         /delay 14s ${Cast.Status.Equal[i]}
         /if (${Me.Gem[${tGem}].ID}!=${Me.Book[${Me.Book[${ABSpell[${a}]}]}].ID}) { 
            /bc BUFF - ${ABSpell[${a}]} - FAILED MEMORIZE
            /return
         }
      }
      /if (!${ABGemInUse.Length} || ${ABGemInUse.Arg[2,|].NotEqual[${ABSpell[${a}]}]}) /varset ABGemInUse ${ABGem[${a}]}|${ABSpell[${a}]}|${Math.Calc[${Macro.RunTime}+${Spell[${ABSpell[${a}]}].RecastTime.Float}+10]}
   }
   /if (${Spell[${tSpellID}].TargetType.Equal[self]}) /varset tTargetID 0
   /varset ABtTargetID 0
   /varset tSpellDelay 0
   /if (${Window[SpellBookWnd].Open}) /keypress ctrl+b
   /varset CastResult UNKNOWN
:tCastBuff
   /if (${DoMelee} && !${ACState} || !${DoBuffsCBT}) {
      /if (!${ADMobCount}) /call ADSpawnCheck
      /if (${ADMobCount}) /return
   }
   /if (${NetBots[${Me.CleanName}].Silenced} || ${NetBots[${Me.CleanName}].Stunned}  || ${NetBots[${Me.CleanName}].Charmed} || ${Window[RespawnWnd].Open}) /return
   /if (${DoHeals} && ${AHCount}) /call AHHealCheck
   /if (!${Cast.Ready} && !${ABtTargetID}) /delay 1s ${Cast.Ready}
   /if (!${ABtTargetID} && ${Cast.Ready} && !${Cast.Ready[${ABSpell[${a}]}]} && ${Spell[${tSpellID}].RecastTime.Float}>5) /return
   /if (${ABtTargetID}) {
      /if (${Cast.Status.Equal[i]}) {
         /doevents OUTDOORS
         /if (${Select[${Cast.Result},CAST_OUTDOORS]} || ${OutDoorsFail}) {
            /bc Outdoor only spell, turning off buff [ ${ABSpell[${a}]} ] for this zone.  Check "${MBCommon}" if incorrect setting.
            /if (!${Ini["${MBCommon}",RestrictedList,${Zone.ID}].Find[${tSpellID}|]}) {
               /if (${Ini["${MBCommon}",RestrictedList,${Zone.ID}].Length} >0 ) {
   	             /ini "${MBCommon}" RestrictedList ${Zone.ID} ${Ini["${MBCommon}",RestrictedList,${Zone.ID}]}${tSpellID}|
               } else {
                  /ini "${MBCommon}" RestrictedList ${Zone.ID} ${tSpellID}|
                  /echo /ini "${MBCommon}" RestrictedList ${Zone.ID} ${tSpellID}|
               }
            }
            /varset OutDoorsFail FALSE
            /return
         }
         /if (${Cast.Result.Equal[cast_success]} || ${Me.Class.ShortName.Equal[BRD]}) /goto :tContBuff
         /if (${Select[${Cast.Result},CAST_FIZZLE,CAST_RECOVER,CAST_OUTOFMANA]}) { 
            /if (${Me.CurrentMana}<${Spell[${tSpellID}].Mana} || ${Me.PctMana}<${ABSpellMinMana[${a}]}) /return
            /varset ABtTargetID 0
         }
         /if (${Select[${Cast.Result},CAST_IMMUNE,CAST_TAKEHOLD]}) { 
            /if (!${Spell[${tSpellID}].TargetType.Find[group]}) /varset tSpellDelay 900
            /goto :tContBuff
         }
         /if (${Select[${Cast.Result},CAST_OUTOFRANGE,CAST_CANNOTSEE,CAST_DISTRACTED,CAST_NOTARGET,CAST_INTERRUPTED,CAST_CANCELLED,CAST_NOTREADY]} && !${OutDoorsFail}) /goto :tNextBot
      }
   } else /if (${Cast.Ready[${ABSpell[${a}]}|${ABGem[${a}]}]}) {
      /varset ABtTargetID ${tTargetID}
      /if (${Me.Shrouded}) /delay 1s
      /if (${Me.Class.ShortName.Equal[BRD]}) {
         /squelch /stoptwist
         /delay 3s !${Twist}
      }
      /varset MBCurrentSpell AB|${a}|${ABtTargetID}
      /doevents flush
      /call MQ2Cast "${ABSpell[${a}]}" ${If[${Select[${ABGem[${a}]},1,2,3,4,5,6,7,8,9,10,11,12,13]},gem,]}${ABGem[${a}]} ${Math.Calc[${Spell[${ABSpell[${a}]}].MyCastTime.Float}+1]}s CastCheck ${If[${ABtTargetID},${If[${Spawn[${ABtTargetID}].Type.Equal[pet]} && ${Spell[${tSpellID}].TargetType.Find[group]},-targetid|${Spawn[${tTargetID}].Master.ID},-targetid|${ABtTargetID}]}, ]} -bandolier|${ABSpellFoci[${a}]}
      /if (${Me.Class.ShortName.Equal[BRD]}) /delay 3s
      /if (!${ABtTargetID}) /varset ABtTargetID ${Me.ID}
   }
   /if (${interruptFlag}) /return
   /goto :tCastBuff
:tContBuff
   /varset MiscCheckTimer 5s
   /if (${ABAnnounce[${a}].Length} && (!${Select[${Cast.Result},cast_immune,cast_takehold]} || ${ABTarCnt[${a}]}>1)) { 
      /if (${ABAnnounce[${a}].Length}>10) { 
         /call ParseAnnounce "${ABAnnounce[${a}]}" "${Spawn[${ABtTargetID}].CleanName}" "${ABSpell[${a}]}" 
      } else { 
         /docommand ${ABAnnounce[${a}]} Buffed: -[ ${Spawn[${ABtTargetID}].CleanName} ]- with ${ABSpell[${a}]} 
      } 
   }
   /call AddtoBuffArray ${tTargetID} ${tSpellID} ${tSpellDelay} ${a}
   /varset ABGemInUse
   /varset MBCurrentSpell
   /if (${ADMobCount} || ${MBPause} || ${FollowFlag} && ${Spawn[pc ${FollowID}].Distance}>${FollowDistance}) /return
   /if (${Cursor.ID}) /call ClearCursor
   /if (${DoMount} && !${Me.Mount.ID} && ${MountCast.Length}>5 && !${Me.Invis}) /call PMCheck "m"
   /call ADSpawnCheck
:tNextBot
   /doevents
/return


Sub AddtoBuffArray(int tTargetID, string tSpellID, int tSpellDelay, int a)
|  /echo AddtoString - Target: ${tTargetID} - Spell: ${Spell[${tSpellID}].Name} - Delay: ${tSpellDelay} - BuffNumber: ${a}
   /declare tABMob int local 0
   /declare c int local 0
   /declare tSpellDuration int local 0
   /declare sVarL string local
   /declare sVarR string local
   /if (!${tTargetID}) /varset tTargetID ${Me.ID}
   /for c 1 to ${ABMobMax}
      /if (${ABMBL[${c}].Arg[1,|].Equal[${Spawn[id ${tTargetID}].Name}]} || ${ABMBL[${c}].Arg[1,|].Equal[${Spawn[id ${tTargetID}].CleanName}]}) /varset tABMob ${c}
|     /if (${ABMBL[${c}].Arg[1,|].Equal[${Me.CleanName}]} && (!${tTargetID} || ${tTargetID}==${Me.ID})) /varset tABMob ${c}
   /next c
   /if (!${tABMob}) /echo Target not found in array.  Buff not added.
   /if (${a}) {
      /if (${Me.Shrouded}) {
         /varcalc tSpellDuration 900*1.${ABDurMod[${a}]}+1+${tSpellDelay}+${Macro.RunTime}-20
      } else {
         /varcalc tSpellDuration ((${ABDurMod[${a}]}/100)*${Spell[${tSpellID}].Duration.TotalSeconds})+${Spell[${tSpellID}].Duration.TotalSeconds}+${tSpellDelay}+${Macro.RunTime}-20
      }
   } else {
      /varset tSpellDuration ${tSpellDelay}+${Macro.RunTime}-20
   }
   /if (${ABMBL[${tABMob}].Find[|b${tSpellID}]}) {
      /varset sVarL ${ABMBL[${tABMob}].Left[${Math.Calc[${ABMBL[${tABMob}].Find[|b${tSpellID}]}+${tSpellID.Length}+1]}]}
      /varset sVarR ${ABMBL[${tABMob}].Right[-${Math.Calc[${ABMBL[${tABMob}].Find[|b${tSpellID}]}+${tSpellID.Length}+2]}]}
      /varset sVarR ${sVarR.Right[-${sVarR.Arg[1,|].Length}]}
|/echo Left: ${sVarL}  Right: ${sVarR}
      /varset ABMBL[${tABMob}] ${sVarL}|t${tSpellDuration}${sVarR}
|/echo ${ABMBL[${tABMob}]}
   } else {
      /varset ABMBL[${tABMob}] ${ABMBL[${tABMob}]}b${tSpellID}|t${tSpellDuration}|
   }
/return


Sub Event_OUTDOORS()
   /echo Outdoor Spell..
   /varset OutDoorsFail TRUE
   /if (${Me.Class.ShortName.Equal[brd]}) {
      /declare x int local 0
      /echo ${Cast.Stored} will be turned off for this zone.  Re-start macro or use alias to turn it back on.
      /for x 1 to ${ABCount}
          /if (${ABSpell[${x}].Equal[${Cast.Stored}]}) /varset ABTarCnt[${x}] 0
      /next x
      /if (!${ACState}) { 
         /varset BardRestGems
         /varset BardCombatGems
      }
      /call ABBuffCheck
   }
/return