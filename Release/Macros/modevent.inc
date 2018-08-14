#Event AECEventA "#*#|${AECustomE1}|#*#"
#Event AECEventB "#*#|${AECustomE2}|#*#"
#Event AECEventC "#*#|${AECustomE3}|#*#"

Sub Event_AECEventA
   /call MBScript AECustomEvent1
/return


Sub Event_AECEventB
   /call MBScript AECustomEvent2
/return


Sub Event_AECEventC
   /call MBScript AECustomEvent3
/return


Sub LoadAEINIVals
   /declare a int local
   /declare AECustom int local 3
   /if (${AECustom}) {
      /for a 1 to ${AECustom}
          /call LoadINIVar AdvEvent AECustom${a} " " AECustomE${a} ${IniFile}
          /if (${AECustomE${a}.Length}<=2) /varset AECustomE${a} ${Math.Rand[999]}${Math.Rand[999]}${Math.Calc[${Me.Name.Length}*77]}${Math.Rand[999]}${Math.Rand[999]}
      /next a
   }
   /call LoadINIVar AdvEvent AECount 0 AECount "${IniFile}" int
   /call LoadINIVar AdvEvent AENewFormat 1 AENewFormat "${IniFile}" int
   /declare AECheckTimer timer outer
   /if (${AECount}) {
      /call LoadINIVar AdvEvent AECheckTime 8 AECheckTime "${IniFile}" int
      /declare AEGem[${AECount}] string outer
      /declare AESpell[${AECount}] string outer
      /declare AESpellFoci[${AECount}] string outer
      /declare AEDurMod[${AECount}] string outer
      /declare AEEventMinMana[${AECount}] int outer
      /declare AEEventMinHP[${AECount}] int outer
      /declare AEEventMinEndur[${AECount}] int outer
      /declare AEMinMana[${AECount}] int outer
      /declare AEMaxMana[${AECount}] int outer
      /declare AEMinHP[${AECount}] int outer
      /declare AEMaxHP[${AECount}] int outer
      /declare AEMinEndur[${AECount}] int outer
      /declare AEMaxEndur[${AECount}] int outer
      /declare AETarType[${AECount}] string outer
      /declare AESpellAlias[${AECount}] string outer
      /declare AEAnnounce[${AECount}] string outer
      /declare AEDelay[${AECount}] int outer
      /declare AETotDelay[${AECount}] int outer
      /declare AETarCnt[${AECount}] int outer
      /declare AEDiscTimer[${AECount}] int outer
      /declare tSpellID int local
      /for a 1 to ${AECount}
         /if (${AENewFormat}) {
            /call LoadINIVar AE${a} Gem " " AEGem[${a}] ${IniFile}
            /call LoadINIVar AE${a} Spell " " AESpell[${a}] ${IniFile}
            /call LoadINIVar AE${a} SpellFoci " " AESpellFoci[${a}] ${IniFile}
            /call LoadINIVar AE${a} DurMod 0 AEDurMod[${a}] ${IniFile}
            /call LoadINIVar AE${a} Delay 0 AEDelay[${a}] ${IniFile}
            /call LoadINIVar AE${a} EventMinMana 0 AEEventMinMana[${a}] ${IniFile}
            /call LoadINIVar AE${a} EventMinHP 0 AEEventMinHP[${a}] ${IniFile}
            /call LoadINIVar AE${a} EventMinEndur 0 AEEventMinEndur[${a}] ${IniFile}
            /call LoadINIVar AE${a} MinMana 0 AEMinMana[${a}] ${IniFile}
            /call LoadINIVar AE${a} MaxMana 101 AEMaxMana[${a}] ${IniFile}
            /call LoadINIVar AE${a} MinHP 0 AEMinHP[${a}] ${IniFile}
            /call LoadINIVar AE${a} MaxHP 101 AEMaxHP[${a}] ${IniFile}
            /call LoadINIVar AE${a} MinEndur 0 AEMinEndur[${a}] ${IniFile}
            /call LoadINIVar AE${a} MaxEndur 101 AEMaxEndur[${a}] ${IniFile}
            /call LoadINIVar AE${a} TarType "war shd pal rng mnk rog brd bst ber shm clr dru wiz mag enc nec self tnt" AETarType[${a}] ${IniFile}
            /call LoadINIVar AE${a} SpellAlias " " AESpellAlias[${a}] ${IniFile}
            /call LoadINIVar AE${a} Announce " " AEAnnounce[${a}] ${IniFile}
            /call LoadINIVar AE${a} TarCnt 1 AETarCnt[${a}] ${IniFile}
         } else {
            /call LoadINIVar AdvEvent AEGem${a} " " AEGem[${a}] ${IniFile}
            /call LoadINIVar AdvEvent AESpell${a} " " AESpell[${a}] ${IniFile}
            /call LoadINIVar AdvEvent AESpellFoci${a} " " AESpellFoci[${a}] ${IniFile}
            /call LoadINIVar AdvEvent AEDurMod${a} 0 AEDurMod[${a}] ${IniFile}
            /call LoadINIVar AdvEvent AEDelay${a} 0 AEDelay[${a}] ${IniFile}
            /call LoadINIVar AdvEvent AEEventMinMana${a} 0 AEEventMinMana[${a}] ${IniFile}
            /call LoadINIVar AdvEvent AEEventMinHP${a} 0 AEEventMinHP[${a}] ${IniFile}
            /call LoadINIVar AdvEvent AEEventMinEndur${a} 0 AEEventMinEndur[${a}] ${IniFile}
            /call LoadINIVar AdvEvent AEMinMana${a} 0 AEMinMana[${a}] ${IniFile}
            /call LoadINIVar AdvEvent AEMaxMana${a} 101 AEMaxMana[${a}] ${IniFile}
            /call LoadINIVar AdvEvent AEMinHP${a} 0 AEMinHP[${a}] ${IniFile}
            /call LoadINIVar AdvEvent AEMaxHP${a} 101 AEMaxHP[${a}] ${IniFile}
            /call LoadINIVar AdvEvent AEMinEndur${a} 0 AEMinEndur[${a}] ${IniFile}
            /call LoadINIVar AdvEvent AEMaxEndur${a} 101 AEMaxEndur[${a}] ${IniFile}
            /call LoadINIVar AdvEvent AETarType${a} "war shd pal rng mnk rog brd bst ber shm clr dru wiz mag enc nec self tnt" AETarType[${a}] ${IniFile}
            /call LoadINIVar AdvEvent AESpellAlias${a} " " AESpellAlias[${a}] ${IniFile}
            /call LoadINIVar AdvEvent AEAnnounce${a} " " AEAnnounce[${a}] ${IniFile}
            /call LoadINIVar AdvEvent AETarCnt${a} 1 AETarCnt[${a}] ${IniFile}
         }
         /if (${Select[${AEGem[${a}]},1,2,3,4,5,6,7,8,9,10,11,12,13]} && ${AESpell[${a}].Length}) {
            /if (${Me.Book[${Me.Book[${AESpell[${a}]}]}].ID}) {
               /varset AESpell[${a}] ${Me.Book[${Me.Book[${AESpell[${a}]}]}].Name}
            } else /if (${Me.Book[${Me.Book[${AESpell[${a}]} ]}].ID}) {
               /varset AESpell[${a}] ${Me.Book[${Me.Book[${AESpell[${a}]} ]}].Name}
            } else /if (${Me.Book[${Spell[${AESpell[${a}]}].RankName}]}) {
               /varset AESpell[${a}] ${Spell[${AESpell[${a}]}].RankName}
            } else {
               /bc Spell ${AESpell[${a}]} Not found in your book. Please check your .ini settings.
               /varset AETarCnt[${a}] 0
            }
         } else /if (${AEGem[${a}].Equal[alt]}) {
            /if (!${Me.AltAbility[${AESpell[${a}]}].ID} && !${Me.Shrouded}) {
               /bc You don't have the AltAbility ${AESpell[${a}]}. Please check your .ini settings.
               /varset AETarCnt[${a}] 0
            }
         } else /if (${AEGem[${a}].Equal[disc]}) {
            /if (${Me.CombatAbility[${Spell[${AESpell[${a}]}].RankName}]}) {
               /varset AESpell[${a}] ${Spell[${AESpell[${a}]}].RankName}
            } else {
               /bc You don't have the Disc ${AESpell[${a}]}.  Please check AESpell${a} in your .ini
               /varset AETarCnt[${a}] 0
            }
         }
      /next a
   }
/return


Sub AEEventCheck
   /if (${MBPause} || !${AECount} || ${AdvPath.Following} && !${AdvPath.Idle}) /return
   /if (${MBDebug}) /echo Macro.RunTime:${Macro.RunTime} - Sub:AEEventCheck
   /varset AECheckTimer ${AECheckTime}s
   /declare a int local
   /declare b int local
   /declare c int local
   /declare tCT int local
   /declare tSpellID int local
   /declare tGem int local
   /declare tSpellRange int local
   /declare tSpellDuration int local
   /declare tSpellDelay int local
   /declare tCastTime timer local
   /declare AEtTargetID int local
   /declare tTargetID int local
   /declare tName string local
   /declare tAEMob int local
   /declare tNBCnt int local ${NetBots.Counts}
   /declare sNBClnt string local ${NetBots.Client}
   /declare tCntr int local
   /declare xFoundBot bool local TRUE

   /if (${ADCount} && !${ADMobCount}) /call ADSpawnCheck
|  /if (${Me.Class.ShortName.Equal[shm]} && ${DoMelee} && !${ACState} && ${ADMobCount}) /return
   /for a 1 to ${AECount}
      /if (!${DoEvents}) /return
      /if (${AETotDelay[${a}]}>${Macro.RunTime} || ${Me.PctHPs}<${AEEventMinHP[${a}]} || ${Me.PctMana}<${AEEventMinMana[${a}]} || ${Me.PctEndurance}<${AEEventMinEndur[${a}]} || !${AETarCnt[${a}]}) /goto :tNextEvent
      /varset tGem 0
      /varset tSpellID 0
      /if (${AETarType[${a}].Find[cbt]} && !${AETarType[${a}].Find[idle]} && !${ADMobCount}) /goto :tNextEvent
      /if (${AETarType[${a}].Find[self]} && ${Me.PctHPs}>=${AEMinHP[${a}]} && ${Me.PctHPs}<=${AEMaxHP[${a}]} && ${Me.PctMana}>=${AEMinMana[${a}]} && ${Me.PctMana}<=${AEMaxMana[${a}]} && ${Me.PctEndurance}>=${AEMinEndur[${a}]} && ${Me.PctEndurance}<=${AEMaxEndur[${a}]}) { 
         /varset tTargetID ${Me.ID}
         /goto :tFoundBot
      }
      /if (${AETarType[${a}].Find[tnt]} && ${NetBots[${TankName}].Attacking}) { 
         /varset tTargetID ${NetBots[${TankName}].TargetID}
         /goto :tFoundBot
      }
      /if (${AETarType[${a}].Length}>2) {
         /docommand ${If[${AETarType[${a}].Find[any]},/varset tCntr ${AETarType[${a}].Count[ ]},/varcalc tCntr ${AETarType[${a}].Count[ ]}+1]}
         /varset xFoundBot FALSE
         /for b 1 to ${tNBCnt}
            /for c 1 to ${Math.Calc[${AETarType[${a}].Count[ ]}+1]}
               /if (${AETarType[${a}].Arg[${c}, ].Find[${Spawn[pc ${sNBClnt.Arg[${b}]}].Class.ShortName}]}) {
                  /varset tName ${sNBClnt.Arg[${b}]}
                  /varset tTargetID ${NetBots[${tName}].ID}
                  /if (${NetBots[${tName}].PctHPs}>${AEMinHP[${a}]} && ${NetBots[${tName}].PctHPs}<=${AEMaxHP[${a}]} && ${NetBots[${tName}].PctMana}>${AEMinMana[${a}]} && ${NetBots[${tName}].PctMana}<=${AEMaxMana[${a}]} && ${NetBots[${tName}].PctEndurance}>${AEMinEndur[${a}]} && ${NetBots[${tName}].PctEndurance}<=${AEMaxEndur[${a}]}) {
                     /if (${tCntr}<=1 || ${tCntr}>1 && ${AETarType[${a}].Find[any]}) {
                        /varset xFoundBot TRUE
                        /break
                     }
                     /varcalc tCntr ${tCntr}-1
                  }
               }
            /next c
            /if (${xFoundBot}) /break
         /next b
         /if (!${xFoundBot}) /goto :tNextEvent
      }
:tFoundBot
      /if (${Select[${AEGem[${a}]},1,2,3,4,5,6,7,8,9,10,11,12,13]}) {
|/echo was Spell
         /if (${Me.CurrentMana}+${Me.ManaRegen}<${Me.Book[${Me.Book[${AESpell[${a}]}]}].Mana} || ${Me.PctMana}<${AEEventMinMana[${a}]}) /goto :tNextEvent
         /varset tSpellID ${Me.Book[${Me.Book[${AESpell[${a}]}]}].ID}
         /varset tGem ${AEGem[${a}]}
         /varcalc tCT ${Spell[${tSpellID}].MyCastTime}/100+40
      } else /if ( ${AEGem[${a}].Find[item]} && ${Bool[${FindItem[=${AESpell[${a}]}]}]} ) {
|/echo was Item
         /if (${FindItem[=${AESpell[${a}]}].Timer} ) /goto :tNextEvent
         /varset tSpellID ${FindItem[=${AESpell[${a}]}].Spell.ID}
         /varset tGem 20
         /varcalc tCT ${FindItem[=${AESpell[${a}]}].CastTime}*10+40
      } else /if ( ${AEGem[${a}].Equal[alt]} && ${Me.AltAbility[${AESpell[${a}]}].ID} ) {
|/echo was AA
         /if (!${Me.AltAbilityReady[${AESpell[${a}]}]}) /goto :tNextEvent
         /varset tSpellID ${Me.AltAbility[${AESpell[${a}]}].Spell.ID}
         /varset tGem 30
         /varcalc tCT ${Me.AltAbility[${AESpell[${a}]}].Spell.CastTime}/100+40
      } else /if (${AEGem[${a}].Equal[script]}) {
|/echo was Script
         /call MBScript ${AESpell[${a}]} 
         /varset tSpellDelay 0
         /varset tGem 40
         /varset tSpellID 0
      } else /if (${AEGem[${a}].Equal[disc]}) {
|/echo was disc
         /if (${AEDiscTimer[${a}]}>${Macro.RunTime} || !${Me.CombatAbilityReady[${AESpell[${a}]}]}) /goto :tNextEvent
         /varset tSpellID ${Me.CombatAbility[${Me.CombatAbility[${AESpell[${a}]}]}].ID}
         /varset tGem 50
         /varcalc tCT ${Spell[${tSpellID}].MyCastTime}/100+40
      }
      /if (!${DoEvents} || ${MBPause} || ${Window[RespawnWnd].Open}) /return
      /varset tSpellRange ${Spell[${tSpellID}].Range}
      /if (!${tGem} || !${tSpellID} || !${tTargetID} || ${Spawn[${tTargetID}].Distance3D}>${tSpellRange}) /goto :tNextEvent
      /if (${tGem}<19) {
         /if (${Me.Gem[${tGem}].ID}!=${tSpellID}) {
            /squelch /memorize "${Spell[${tSpellID}]}|gem${tGem}"
            /delay 5s ${Me.Gem[${tGem}].ID}==${tSpellID}
            /if (${Me.Gem[${tGem}].ID}!=${tSpellID}) { 
               /bc EVENTS - FAILED MEMORIZE
               /goto :tNextEvent
            }
            /if (!${Cast.Ready[${tGem}]}) /varcalc tCT ${Spell[${tSpellID}].RecastTime}/100+${tCT}
         }
      }
|     /if (${Me.Class.ShortName.Equal[shm]} && ${Select[${Spell[${tSpellID}].Name},Pained Memory,Ancestral Bargain,Ancestral Hearkening,Cannibalize II]} && ${Me.CombatState.Equal[RESTING]} && !${Me.Mount.ID}) /goto :tNextEvent
|      /if (${Me.CombatState.Equal[RESTING]} && !${Me.Mount.ID}) {
|         /if (${Me.Class.ShortName.Equal[shm]} && ${Spell[${tSpellID}].TargetType.Equal[Self]} && ${Spell[${tSpellID}].SpellType.Equal[Beneficial]} && ${Spell[${tSpellID}].Subcategory.Equal[Conversions]}) /goto :tNextEvent
|         /if (${Me.Class.ShortName.Equal[bst]} && ${Spell[${tSpellID}].Name.Find[Consumption of Spirit]}) /goto :tNextEvent
|         /if (${Me.Class.ShortName.Equal[mag]} && ${Spell[${tSpellID}].Name.Find[Elemental Conversion]}) /goto :tNextEvent
|      }
|     /if (${tGem}<19 && !${Cast.Ready[${tGem}]}) /varcalc tCT ${Spell[${tSpellID}].RecastTime}/100+${tCT}
      /if (${Spell[${tSpellID}].TargetType.Equal[self]}) /varset tTargetID 0
      /if (${Spell[${tSpellID}].TargetType.Find[group]}) /docommand ${If[${Target.Type.Equal[pc]},/varset tTargetID ${Me.ID},/varset tTargetID 0]}
      /varset AEtTargetID 0
      /varset tSpellDelay ${AEDelay[${a}]}
      /varset tCastTime ${tCT}
      /varset CastResult UNKNOWN
:tCastEvent
      /if (${NetBots[${Me.CleanName}].Silenced} || ${NetBots[${Me.CleanName}].Stunned}  || ${NetBots[${Me.CleanName}].Charmed} || ${Window[RespawnWnd].Open}) /return
|     /if (${Me.Class.ShortName.Equal[shm]} && ${Cast.Ready} && !${Cast.Ready[${tSpellID}|${AEGem[${a}]}]} && ${AEGem[${a}].NotEqual[alt]}) /goto :tNextEvent
      /if (${Spell[${tSpellID}].RecastTime.Float}>20 && ${Cast.Ready} && !${Cast.Ready[${tSpellID}|${AEGem[${a}]}]} && ${Select[${AEGem[${a}]},1,2,3,4,5,6,7,8,9,10,11,12,13]}) /goto :tNextEvent
      /if (${DoHeals} && ${AHCount}) /call AHHealCheck
      /if (${tGem}==50) {
         /varset AEDiscTimer[${a}] ${Macro.RunTime}+${AESpellRecast[${a}]}
         /disc ${tSpellID}
         /varset tSpellDelay 0
         /if (${tTargetID}) {
            /varset AEtTargetID ${tTargetID}
         } else {
            /varset AEtTargetID ${Me.ID}
         }
         /varset tSpellID 0
         /goto :tContEvent
      }
      /if (${AEtTargetID}) {
         /if (${Me.Class.ShortName.Equal[BRD]}) {
            /doevents CastRst
            /doevents CastImm
            /doevents CastStn
            /if (!${CastReturn.Equal[pending]}) /goto :tNextEvent
            /docommand ${If[${Window[CastingWindow].Open},/goto :tCastEvent,/goto :tContEvent]}
         }
         /if (${Cast.Status.Equal[i]}) {
            /if (${Select[${Cast.Result},cast_immune,cast_takehold]}) { 
               /varset tSpellDelay 99999
               /goto :tContEvent
            }
            /if (${Select[${Cast.Result},cast_outofrange,cast_cannotsee,cast_distracted,cast_notarget,CAST_CANCELLED,CAST_NOTREADY]}) /goto :tNextEvent
            /if (${Cast.Result.Equal[cast_success]}) /goto :tContEvent
            /goto :tNextEvent
         }
      } else /if (${Cast.Ready[${AESpell[${a}]}|${AEGem[${a}]}]} || ${Me.Class.ShortName.Equal[BRD]}) {
         /varset AEtTargetID ${tTargetID}
         /if (${Me.Class.ShortName.Equal[BRD]}) {
            /squelch /stoptwist
            /if (${Target.ID}!=${AEtTargetID}) {
               /squelch /target clear
               /delay 1s !${Target.ID}
               /squelch /tar id ${AEtTargetID}
               /delay 1s ${Target.ID}==${AEtTargetID}
            }
            /squelch /twist ${AEGem[${a}]}
            /varset CastResult PENDING
            /delay 5
         } else {
            /varset MBCurrentSpell AE|${a}|${AEtTargetID}
|           /casting "${AESpell[${a}]}|${AEGem[${a}]}" -targetid|${AEtTargetID} ${AESpellFoci[${a}]}
            /call MQ2Cast "${AESpell[${a}]}" ${If[${Select[${AEGem[${a}]},1,2,3,4,5,6,7,8,9,10,11,12,13]},gem,]}${AEGem[${a}]} ${Math.Calc[${Spell[${tSpellID}].CastTime.Float}+1]}s CastCheck -targetid|${AEtTargetID} -bandolier|${AESpellFoci[${a}]}
            /if (!${AEtTargetID}) /varset AEtTargetID ${Me.ID}
         }
         /if (!${Select[${Cast.Result},cast_immune,cast_takehold,cast_outofrange,cast_cannotsee,cast_distracted,cast_notarget]}) /goto :tContEvent
      }
      /if (${interruptFlag}) /return
      /if (${tCastTime} || ${Cast.Timing}) /goto :tCastEvent
      /echo WTF - Event : ${AESpell[${a}]}
      /goto :tNextEvent
:tContEvent
      /varset MiscCheckTimer 5s
      /if (${AEAnnounce[${a}].Length}) /docommand ${If[${AEAnnounce[${a}].Length}>10,/call ParseAnnounce "${AEAnnounce[${a}]}" "${Spawn[${AEtTargetID}].CleanName}" "${AESpell[${a}]}",/docommand ${AEAnnounce[${a}]} Evented: -[ ${Spawn[${AEtTargetID}].CleanName} ]- with ${AESpell[${a}]}]}
|     /docommand ${AEAnnounce[${a}]} Evented: -[ ${Spawn[${AEtTargetID}].CleanName} ]- with ${AESpell[${a}]}
      /if (${Spell[${tSpellID}].Duration} && ${tGem}<40) {
         /docommand ${If[${AEDurMod[${a}]},/varcalc tSpellDuration ((${AEDurMod[${a}]}/100)*${Spell[${tSpellID}].Duration.TotalSeconds})+${Spell[${tSpellID}].Duration.TotalSeconds}-${Spell[${tSpellID}].MyCastTime.TotalSeconds}-5,/varcalc tSpellDuration ${Spell[${tSpellID}].Duration.TotalSeconds}-${Spell[${tSpellID}].MyCastTime.TotalSeconds}-5]}
         /varcalc tSpellDelay ${tSpellDelay}+${AEDelay[${a}]}+${tSpellDuration}+${Macro.RunTime}
      } else {
         /varcalc tSpellDelay ${tSpellDelay}+${AEDelay[${a}]}+${Macro.RunTime}-${Spell[${tSpellID}].MyCastTime.TotalSeconds}-5
      }
      /if ((!${tSpellDelay} || ${tSpellDelay}<${AEDelay[${a}]}) && ${AEDelay[${a}]}) /varset tSpellDelay ${AEDelay[${a}]}+${Macro.RunTime}
      /if (${Spell[${tSpellID}].RecastTime.TotalSeconds} && (${tSpellDelay}<=${Macro.RunTime} || ${Math.Calc[${tSpellDelay}-${Macro.RunTime}]}<${Spell[${tSpellID}].RecastTime.TotalSeconds})) /varset tSpellDelay ${Spell[${tSpellID}].RecastTime.TotalSeconds}
      /varcalc AETotDelay[${a}] ${tSpellDelay}
      /varset MBCurrentSpell
      /return
:tNextEvent
      /doevents
   /next a
/return