
#Event AHDA "#*#can't cast spells while invulnerable#*#"

Sub LoadAHINIVals
   /declare a int local
   /declare b int local
   /declare tCastClass string local enc wiz mag nec clr dru shm pal shd war bst rng ber rog brd mnk
   /call LoadINIVar AdvHeal AHCount 0 AHCount "${IniFile}" int
   /call LoadINIVar AdvHeal AHNewFormat 1 AHNewFormat "${IniFile}" int
   /call LoadINIVar AdvHeal AHAllowDismount TRUE AHAllowDismount "${IniFile}" bool
   /declare AHCheckTimer timer outer
   /declare AHHealList string outer |
   /if (${AHCount}) {
      /call LoadINIVar AdvHeal AHCheckTime 2 AHCheckTime "${IniFile}" int
      /call LoadINIVar AdvHeal AHHealOOBC FALSE AHHealOOBC "${IniFile}" bool
      /call LoadINIVar AdvHeal AHHealMode "0|0|12" AHHealMode "${IniFile}" string
      /call LoadINIVar AdvHeal AHInterruptLevel 2 AHInterruptLevel "${IniFile}" int
      /call LoadINIVar AdvHeal AHClassPriority "enc,wiz,mag,nec,clr,dru,shm,pal,shd,war,bst,rng,ber,rog,brd,mnk" AHClassPriority "${IniFile}" string
      /declare AHGem[${AHCount}] string outer
      /declare AHSpell[${AHCount}] string outer
      /declare AHSpellFoci[${AHCount}] string outer
      /declare AHSpellAlias[${AHCount}] string outer
      /declare AHDurMod[${AHCount}] string outer
      /declare AHSpellMinMana[${AHCount}] int outer 0
      /declare AHClass[${AHCount}] string outer
      /declare AHTarCnt[${AHCount}] int outer 0
      /declare AHTargetName string outer
      /declare AHAnnounce[${AHCount}] string outer
      /declare AHCondition[${AHCount}] string outer
      /declare tSpellID int local
      /declare PRSpell int outer
      /declare AHThreshold[${AHCount},5] int outer 0

      /for a 1 to ${AHCount}
         /if (${AHNewFormat}) {
            /call LoadINIVar AH${a} Gem " " AHGem[${a}] ${IniFile}
            /call LoadINIVar AH${a} Spell " " AHSpell[${a}] ${IniFile}
            /call LoadINIVar AH${a} SpellFoci " " AHSpellFoci[${a}] ${IniFile}
            /call LoadINIVar AH${a} DurMod 0 AHDurMod[${a}] ${IniFile}
            /call LoadINIVar AH${a} SpellMinMana 0 AHSpellMinMana[${a}] ${IniFile}
            /call LoadINIVar AH${a} SpellAlias " " AHSpellAlias[${a}] ${IniFile}
            /call LoadINIVar AH${a} Announce " " AHAnnounce[${a}] ${IniFile}
            /call LoadINIVar AH${a} TarCnt 0 AHTarCnt[${a}] ${IniFile}
            /call LoadINIVar AH${a} Class "pc pet group hp0 war shd pal rng mnk rog brd bst ber shm clr dru wiz mag enc nec tnt mypet self" AHClass[${a}] ${IniFile}
            /call LoadINIVar AH${a} PreCondition "TRUE" AHCondition[${a}] ${IniFile}
         } else {
            /call LoadINIVar AdvHeal AHGem${a} " " AHGem[${a}] ${IniFile}
            /call LoadINIVar AdvHeal AHSpell${a} " " AHSpell[${a}] ${IniFile}
            /call LoadINIVar AdvHeal AHSpellFoci${a} " " AHSpellFoci[${a}] ${IniFile}
            /call LoadINIVar AdvHeal AHDurMod${a} 0 AHDurMod[${a}] ${IniFile}
            /call LoadINIVar AdvHeal AHSpellMinMana${a} 0 AHSpellMinMana[${a}] ${IniFile}
            /call LoadINIVar AdvHeal AHSpellAlias${a} " " AHSpellAlias[${a}] ${IniFile}
            /call LoadINIVar AdvHeal AHAnnounce${a} " " AHAnnounce[${a}] ${IniFile}
            /call LoadINIVar AdvHeal AHTarCnt${a} 0 AHTarCnt[${a}] ${IniFile}
            /call LoadINIVar AdvHeal AHClass${a} "pc pet group hp0 war shd pal rng mnk rog brd bst ber shm clr dru wiz mag enc nec tnt mypet self" AHClass[${a}] ${IniFile}
            /call LoadINIVar AdvHeal AHPreCondition${a} "TRUE" AHCondition[${a}] ${IniFile}
         }
         /if (${AHGem[${a}].NotEqual[script]} && ${AHSpell[${a}].Length}>2) {
            /if (${Select[${AHGem[${a}]},1,2,3,4,5,6,7,8,9,10,11,12,13]}) {
               /if (${Me.Book[${Me.Book[${AHSpell[${a}]}]}].ID}) {
                  /varset AHSpell[${a}] ${Me.Book[${Me.Book[${AHSpell[${a}]}]}].Name}
               } else /if (${Me.Book[${Me.Book[${AHSpell[${a}]} ]}].ID}) {
                  /varset AHSpell[${a}] ${Me.Book[${Me.Book[${AHSpell[${a}]} ]}].Name}
               } else /if (${Me.Book[${Spell[${AHSpell[${a}]}].RankName}]}) {
                  /varset AHSpell[${a}] ${Spell[${AHSpell[${a}]}].RankName}
               } else {
                  /bc Spell ${AHSpell[${a}]} Not found in your book.  Check your .ini settings.
                  /varset AHTarCnt[${a}] 0
               } 
               /if (${Select[${Spell[${AHSpell[${a}]}].ID},9755,9756,9757,14249,18270,18271,18272,18534,18535,18536,25165,25166,25167,28197,28198,28199,34184,34185,34186,43178,43179,43180]}) /varset PRSpell ${a}
            } else /if (${AHGem[${a}].Equal[alt]}) {
               /if (!${Me.AltAbility[${AHSpell[${a}]}].ID} && !${Me.Shrouded}) {
                  /bc You don't have the AltAbility ${AHSpell[${a}]}.  Check your .ini settings
                  /varset AHTarCnt[${a}] 0
               }
            } 
            /if (${AHGem[${a}].Find[item]}) {
               /if (${FindItem[${AHSpell[${a}]}].Spell.Name.NotEqual[NULL]}) /varset AHHealList ${AHHealList}${FindItem[${AHSpell[${a}]}].Spell}|
            } else {
               /varset AHHealList ${AHHealList}${AHSpell[${a}]}|
            }
         }
| Build the Threshold Heal amounts
| 1 - PCs
         /if (${AHClass[${a}].Find[self]}) {
            /call CheckAHArgs "${AHClass[${a}]}"|self
            /if (${Macro.Return}>0) /varset AHThreshold[${a},1] ${Macro.Return}
         } else /if (${AHClass[${a}].Find[pc]}) {
            /for b 1 to 16
               /if (${AHClass[${a}].Find[${tCastClass.Arg[${b}]}]}) {
                  /call CheckAHArgs "${AHClass[${a}]}"|${tCastClass.Arg[${b}]} 
                  /if (${Macro.Return} && ${Macro.Return}!=${AHThreshold[${a},1]}) {
| 5- Set to "1" if more than one heal threshold for the Class string
                     /if (${AHThreshold[${a},1]}) /varset AHThreshold[${a},5] 1 
                     /if (${Macro.Return}>${AHThreshold[${a},1]}) /varset AHThreshold[${a},1] ${Macro.Return}
                  }
               }
            /next b
         }
| 2 - Pets
         /if (${AHClass[${a}].Find[mypet]}) {
            /call CheckAHArgs "${AHClass[${a}]}"|mypet
            /if (${Macro.Return}>0) /varset AHThreshold[${a},2] ${Macro.Return}
         } else /if (${AHClass[${a}].Find[pet]}) {
            /call CheckAHArgs "${AHClass[${a}]}"|pet
            /if (${Macro.Return}>0) /varset AHThreshold[${a},2] ${Macro.Return}
         }
| 3 - Tank
         /if (${AHClass[${a}].Find[tank]} && ${AHClass[${a}].Find[pc]}) {
            /call CheckAHArgs "${AHClass[${a}]}"|tank
            /if (${Macro.Return}>0) /varset AHThreshold[${a},3] ${Macro.Return}
         }
| 4 - Grp
         /if (${AHClass[${a}].Find[grp]}) {
            /call CheckAHArgs "${AHClass[${a}]}"|grp
            /if (${Macro.Return}>0) /varset AHThreshold[${a},4] ${Macro.Return}
         }
| /echo ${a} - ${AHThreshold[${a},1]} - ${AHThreshold[${a},2]} - ${AHThreshold[${a},3]} - ${AHThreshold[${a},4]} - ${AHThreshold[${a},5]}
      /next a
   }
/return


Sub AHHealCheck
   /if (${AHCheckTimer} || !${DoHeals} || !${AHCount}) /return
   /if (${MBDebug}) /echo Macro.RunTime:${Macro.RunTime} - Sub:AHHealCheck
   /varset AHCheckTimer ${AHCheckTime}s

   /declare a int local
   /declare x int local
   /declare tGem int local
   /declare tSpellID int local
   /declare tSpellRange int local
   /declare AHtDurMod int local
  
   /if (!${Defined[AHtCnt]}) {
      /declare AHtCnt int outer 0
      /declare AHttl int outer 0
      /declare AHtSpellID int outer 0
      /declare AHtSpellRange int outer 0
      /declare AHtMembers string outer
      /declare AHtTargetHP int outer 0
      /declare AHtPctPS int outer 0
      /declare AHtDPS float outer 0
      /declare AHtPctAvr int outer 0
      /declare AHtDPSAvr int outer 0
      /declare AHtPctCntr float outer 0
      /declare tHurt int outer 0
      /declare AHtPreHP int outer 0
      /declare AHtPreID int outer 0
      /declare AHtTime int outer 0
      /declare AHtTimer timer outer 0
      /declare AHtPriority int outer
      /declare tMembers string outer
      /declare CastTimer timer outer 0
      /declare tPriority int outer 0
      /declare HealOnlyTimer timer outer 0
      /declare tPRSpell bool outer FALSE
      /declare AHtTargetID int outer 0
      /declare AHbTargetID int outer 0
      /declare AHgotoTop bool outer
      /declare AHcontinue bool outer
      /declare NeedtoHeal bool outer
      /declare AHPrevTarget int outer    
   }
   /if (!${AHHealList.Find[${Me.Casting.Name}]} || !${Me.Casting.ID}) {
      /varset AHtCnt 0
      /varset AHttl 0
      /varset AHtSpellID 0
      /varset AHtSpellRange 0
      /varset AHtMembers 
      /varset AHtTargetHP 0
      /varset AHtPctPS 0
      /varset AHtDPS 0
      /varset AHtPctAvr 0
      /varset AHtDPSAvr 0
      /varset AHtPctCntr 0
      /varset tHurt 0
      /varset AHtPreHP 0
      /varset AHtPreID 0
      /varset AHtTime 0
      /varset AHtTimer 0
      /varset AHtPriority 0
      /varset tMembers 
      /varset CastTimer 0
      /varset tPriority 0
      /varset AHbTargetID 0
      /varset AHgotoTop FALSE
      /varset AHcontinue FALSE
      /varset NeedtoHeal FALSE
      /varset AHPrevTarget 0
   }

:tHealTop
   /doevents AHDA
   /for a 1 to ${AHCount}
      /if (!${DoHeals} || ${MBPause} || ${NetBots[${Me.CleanName}].Silenced} || ${NetBots[${Me.CleanName}].Stunned} || ${NetBots[${Me.CleanName}].Charmed}) /return
      /if (${Me.Ducking}) /keypress duck
      /if (!${AHTarCnt[${a}]}) /goto :tNextHeal
      /varset tGem 0
      /varset tSpellID 0
      /varset AHgotoTop FALSE
      /if (!${AHHealList.Find[${Me.Casting.Name}]}) /varset AHtTargetID 0
      /if (${Select[${AHGem[${a}]},1,2,3,4,5,6,7,8,9,10,11,12,13]} ) {
|/echo was Spell
         /if (${Me.CurrentMana}<${Me.Book[${Me.Book[${AHSpell[${a}]}]}].Mana} || ${Me.PctMana}<=${AHSpellMinMana[${a}]}) /goto :tNextHeal
         /varset tSpellID ${Me.Book[${Me.Book[${AHSpell[${a}]}]}].ID} 
         /varset tGem ${AHGem[${a}]}
         /if (${Me.Gem[${tGem}].ID}!=${tSpellID}) {
            /squelch /memorize "${Spell[${tSpellID}]}|gem${tGem}"
            /delay 16s ${Me.Gem[${tGem}].ID}==${tSpellID}
            /if (${Me.Gem[${tGem}].ID}!=${tSpellID}) {
               /bc HEAL ${AHSpell[${a}]} - FAILED MEMORIZE
               /goto :tNextHeal
            }
         }
         /if (${Me.Class.ShortName.NotEqual[BRD]} && ${Cast.Ready} && !${Cast.Ready[${tSpellID}|${tGem}]} && (${Me.GemTimer[${tGem}]}>4000 || (${Me.GemTimer[${tGem}]}>3000 && !${Me.SpellInCooldown}))) /goto :tNextHeal
      } else /if (${AHGem[${a}].Find[item]} && ${Bool[${FindItem[=${AHSpell[${a}]}]}]}) {
|/echo was Item
         /if (${FindItem[=${AHSpell[${a}]}].Timer} ) /goto :tNextHeal
         /varset tSpellID ${FindItem[=${AHSpell[${a}]}].Spell.ID} 
         /varset tGem 20
      } else /if (${AHGem[${a}].Equal[alt]}) {
|/echo was Alt
         /if (!${Me.AltAbilityReady[${AHSpell[${a}]}]} || ${Me.CurrentMana}<${Me.AltAbility[${AHSpell[${a}]}].Spell.Mana} || ${Me.PctMana}<${AHSpellMinMana[${a}]} || !${Me.AltAbility[${AHSpell[${a}]}].ID}) /goto :tNextHeal
         /varset tSpellID ${Me.AltAbility[${AHSpell[${a}]}].Spell.ID} 
         /varset tGem 30
      } else /if (${AHGem[${a}].Equal[script]}) {
|/echo was Script
         /varset tGem 40
         /varset tSpellID 0
      } else /if (${AHGem[${a}].Equal[ability]}) {
|/echo was Ability
         /if (!${Me.AbilityReady[${AHSpell[${a}]}]}) /goto :tNextHeal
         /varset tSpellID 0
         /varset tSpellRange 50
         /varset tGem 50
      }
      /if (!${tGem}) /goto :tNextHeal
      /varset tSpellRange ${If[${Spell[${tSpellID}].AERange} && ${tGem}<39,${Spell[${tSpellID}].AERange},${Spell[${tSpellID}].Range}]}
      /varset tHurt 0
      /varset tMembers
      /call GrpEval "${AHClass[${a}]}" ${tSpellRange} ${AHTarCnt[${a}]} ${a}
      /if (${Macro.Return.Equal[FALSE]}) /goto :tNextHeal
      /if (${Macro.Return.Length}) {
         /varset tHurt ${Macro.Return.Right[-${Math.Calc[${Macro.Return.Find[|]}]}]}
         /varset tMembers ${Macro.Return.Left[${Math.Calc[${Macro.Return.Find[|]}-1]}]}
         /varset tMembers ${tMembers.Right[-1]}
      }
      /if (!${tHurt} && ${AHtTargetID} && ${AHbTargetID} && ${AHtTargetID}==${AHbTargetID} && ${AHtSpellID} && ${AHtSpellID}==${Me.Casting.ID} && (${Spawn[${AHtTargetID}].PctHPs}>99 && !${Spell[${Me.Casting.ID}].Duration})) {
         /echo interrupt no heal needed
         /interrupt
         /goto :tNextHeal
      }
      /if (!${tHurt} || (${AHTarCnt[${a}]}>1 && ${AHTarCnt[${a}]}>${tHurt} && !${AHClass[${a}].Find[grp]})) /goto :tNextHeal

      /call HealThem ${a} ${tSpellID} ${tGem} ${tSpellRange}

      /if (${AHgotoTop}) {
         /if (!${AHtTargetID}) /goto :tHealTop
         /goto :skipEvents
      }
      /if (${AHcontinue}) /goto :tContinue

:tNextHeal
      /doevents
:skipEvents
   /next a
   /if (${AHgotoTop}) /goto :tHealTop

   /if (!${AHtTargetID}) /goto :tCheckTimer
   /if (${Cast.Status.NotEqual[i]} || ${Me.Casting.ID} || ${AHHealList.Find[${Me.Casting.Name}]}) /goto :tHealTop

:tContinue
   /if (${Spell[${AHtSpellID}].Duration}) {
      /if (${AHDurMod[${AHtCnt}]}) {
         /varcalc AHtDurMod ((${Spell[${AHtSpellID}].Duration.TotalSeconds}*(${AHDurMod[${a}]}/100))+${Spell[${AHtSpellID}].Duration.TotalSeconds}*10)
      } else {
         /varcalc AHtDurMod ${Spell[${AHtSpellID}].Duration.TotalSeconds}*10
      }
      /if (${AHtCnt}==${PRSpell}) /varcalc AHtDurMod ${AHtDurMod}+60
      /if (${Spell[${AHtSpellID}].AERange} && ${AHtMembers.Count[ ]}) {
         /for x 1 to ${Math.Calc[${AHtMembers.Count[ ]}]}
            /if (!${Defined[${Spawn[${AHtMembers.Arg[${x}]}].Name}${AHtCnt}]}) /declare ${Spawn[${AHtMembers.Arg[${x}]}].Name}${AHtCnt} timer outer
            /varset ${Spawn[${AHtMembers.Arg[${x}]}].Name}${AHtCnt} ${AHtDurMod}
         /next x
      } else {
         /if (!${Defined[${Spawn[${AHtTargetID}].Name}${AHtCnt}]}) /declare ${Spawn[${AHtTargetID}].Name}${AHtCnt} timer outer
         /varset ${Spawn[${AHtTargetID}].Name}${AHtCnt} ${AHtDurMod}
      }
   }
   /if (${NeedtoHeal} || ${HealOnlyTimer} || ${AHHealList.Find[${Me.Casting.Name}]}) /goto :tHealTop
:tCheckTimer
   /if (${HealOnlyTimer}) {
      /varset AHtTargetID 0
      /goto :tHealTop
   }
   /varset MBCurrentSpell
   /if (!${AHHealList.Find[${Me.Casting.Name}]}) /varset AHtTargetID 0
   /if (${AdvPath.Paused} && ${FollowFlag}==3) /squelch /afollow unpause
   /if (${AHPrevTarget}) /target id ${AHPrevTarget}
/return


Sub HealThem(int a, int tSpellID, int tGem, int tSpellRange)
   /declare b int local
   /declare tTarCnt int local
   /declare tHealClass string local
   /declare tgtLevel int local
   /declare tgtName string local

   /varset tTarCnt ${AHTarCnt[${a}]}
   /varset tHealClass ${AHClass[${a}]}

   /for b 1 to ${tHurt}
      /varset AHbTargetID ${tMembers.Arg[${b}]}
      /if (${Spawn[${AHbTargetID}].Type.Equal[corpse]} || ${tTarCnt}==1 && ${AHbTargetID}==${Me.ID} && ${Me.Song[${AHSpell[${a}]}].ID}) /goto :tNextTarget
      /if (${AHCondition[${a}].Length}>4 || (${AHCondition[${a}].NotEqual[TRUE]} && ${AHCondition[${a}].NotEqual[pr]})) {
         /if (${AHCondition[${a}].Left[1].Find[/]}) {
            /call IniCommand "${AHCondition[${a}]}"
            /if (${Macro.Return.NotEqual[TRUE]}) /return
         } else {
            /call MBScript ${AHCondition[${a}]} ${AHbTargetID} ${tSpellID}    	
            /if (!${Macro.Return}) /return
         }
      }
      /if (${Corpse.Open}) /keypress esc

      /if (${Select[${Spawn[${AHbTargetID}].Class.ShortName},shd,nec,mnk,brd]}) {
         /if (!${NetBots[${AHbTargetID}].Feigning} || ${NetBots[${AHbTargetID}].CombatState}==0) {
            /if (${Spawn[${AHbTargetID}].Distance}>${ACLeash}) /goto :tNextTarget
         }
      }

      /if (${tHealClass.Find[tnt]}) {
         /if (!${ACMATarget} || !${ACState} || !${Spawn[${ACMATarget}].ID} || ${Spawn[${ACMATarget}].Type.Equal[corpse]}) /return
         /varset AHbTargetID ${ACMATarget}
      } else /if (${tHealClass.Find[self]}) {
         /if (!${tMembers.Find[${Me.ID}]}) /return
         /varset AHbTargetID ${Me.ID}
      } else {
         /varset tgtLevel ${If[${Spawn[${AHbTargetID}].Type.Equal[pc]},${Spawn[${AHbTargetID}].Level},${Spawn[${AHbTargetID}].Master.Level}]}
         /varset tgtName ${Spawn[${AHbTargetID}].Name}
         /if (${a}==${PRSpell}) {
            /if (${NetBots[${tgtName}].Buff.Find[${tSpellID}]}) /return
            /if (${Defined[${tgtName}${PRSpell}]}) {
               /if (${${tgtName}${PRSpell}} && ${${tgtName}${PRSpell}}/10+1<${Spell[${tSpellID}].MyCastTime.Float}-2) /return
            }
         }
         /if (${PRSpell}) {
            /if (${Defined[${tgtName}${PRSpell}]}) {
               /if (${tTarCnt}==1 && (${Spell[${AHSpell[${a}]}].Duration} && ${NetBots[${tgtName}].Buff.Find[${tSpellID}]} || (${AHCondition[${a}].Equal[pr]} || ${tSpellID}==13) && (${NetBots[${tgtName}].Buff.Find[9755]} || ${NetBots[${tgtName}].Buff.Find[9756]} || ${NetBots[${tgtName}].Buff.Find[9757]} || ${NetBots[${tgtName}].Buff.Find[18270]} || ${NetBots[${tgtName}].Buff.Find[18271]} || ${NetBots[${tgtName}].Buff.Find[18272]} || ${NetBots[${tgtName}].Buff.Find[18534]} || ${NetBots[${tgtName}].Buff.Find[18535]} || ${NetBots[${tgtName}].Buff.Find[18536]} || ${NetBots[${tgtName}].Buff.Find[25165]} || ${NetBots[${tgtName}].Buff.Find[25166]} || ${NetBots[${tgtName}].Buff.Find[25167]} || ${NetBots[${tgtName}].Buff.Find[28197]} || ${NetBots[${tgtName}].Buff.Find[28198]} || ${NetBots[${tgtName}].Buff.Find[28199]} || ${NetBots[${tgtName}].Buff.Find[34184]} || ${NetBots[${tgtName}].Buff.Find[34185]} || ${NetBots[${tgtName}].Buff.Find[34186]} || ${NetBots[${tgtName}].Buff.Find[43178]} || ${NetBots[${tgtName}].Buff.Find[43179]} || ${NetBots[${tgtName}].Buff.Find[43180]}))) {
                  /if (${${tgtName}${PRSpell}}/10+1<${Spell[${tSpellID}].MyCastTime.Float}) /return
               }
            }        
         }
         /if (${NetBots[${tgtName}].ShortBuff.Find[${tSpellID}]}) /return
         /if (${tTarCnt}==1 && ${Spell[${tSpellID}].Duration} && !${Select[${AHGem[${a}]},item,alt]}) {
            /if (${Spell[${tSpellID}].Level}<255 && (${tgtLevel}<61 && ${Spell[${tSpellID}].Level}>65 || ${tgtLevel}<46 && ${Spell[${tSpellID}].Level}>60)) /return
            /if (${Defined[${tgtName}${a}]}) {
               /if (${${tgtName}${a}}>0) /return
            }
         }
         /if (${tHealClass.Find[mypet]} && ${AHbTargetID}!=${Me.Pet.ID}) /goto :tNextTarget 
         /if (${Spawn[${AHbTargetID}].Master.Type.Equal[pc]} && ${Select[${Spawn[${AHbTargetID}].Master.Class.ShortName},clr,wiz]}) /goto :tNextTarget
         /if (${Spell[${tSpellID}].TargetType.Equal[free target]} && !${Spawn[${AHbTargetID}].CanSplashLand}) /goto :tNextTarget
         /if (!${AHHealOOBC}) {
|            /if (${Spawn[${AHbTargetID}].Type.Equal[pc]} && !${NetBots.Client.Find[${tgtName}]} || ${Spawn[${AHbTargetID}].Master.Type.Equal[pc]} && !${NetBots.Client.Find[${Spawn[${AHbTargetID}].Master.CleanName}]})) /goto :tNextTarget
/if ((${Spawn[${AHbTargetID}].Type.Equal[pc]} && !${NetBots.Client.Find[${tgtName}]}) || (${Spawn[${AHbTargetID}].Master.Type.Equal[pc]} && !${NetBots.Client.Find[${Spawn[${AHbTargetID}].Master.CleanName}]})) /goto :tNextTarget
         }

         /if (${AHtTargetID} && !${Spawn[${AHtTargetID}].Type.Equal[corpse]}) /call CalcPPS ${tSpellID}
         /varset tPriority ${Select[${Spawn[${AHbTargetID}].Class.ShortName},${AHClassPriority}]}
         /if (!${Cast.Status.Equal[i]} && ${Me.Casting.ID}) {
            /if ((!${AHHealList.Find[|${Spell[${Me.Casting.ID}].Name}|]} || ${Me.Casting.ID}==13 && ${tHurt}>1 && ${AHTarCnt[${a}]}>1) && (${AHtCnt} && ${AHtCnt}>=${AHInterruptLevel} && (${AHtCnt}>${a} && ${AHtSpellID} && ${AHtSpellID}!=${tSpellID} && ((${CastTimer}>(${Spell[${tSpellID}].MyCastTime}/100) && ${AHttl}>0 && ${Math.Calc[${CastTimer}*10]}>${AHttl}) || ${AHbTargetID}!=${AHtPreID} && ${AHtPriority} && ${AHtPriority}>${tPriority} )))) {
               /if (${Me.AltAbility[${AHSpell[${a}]}]} && !${Me.AltAbilityReady[${AHSpell[${a}]}]}) /return
               /echo Interrupted!!!
               /interrupt
               /varset interruptFlag TRUE
               /varset AHtTargetID 0
               /delay 1s (${Cast.Status.Equal[i]} && ${Cast.Ready[${AHSpell[${a}]}|${AHGem[${a}]}]})
            }
         }
         /if (${Spell[${tSpellID}].TargetType.Find[Group]}) {
            /varset AHbTargetID 99999
         } else /if (${Spell[${tSpellID}].TargetType.Equal[pet]} && ${tMembers.Find[${AHbTargetID}]} && ${Me.Pet.ID}) {
            /varset AHbTargetID ${Me.Pet.ID}
         }
      }
      /if (${AHtTargetID}) {
         /if (${AHHealList.Find[${Me.Casting.Name}]} && ${Me.Casting.ID}) {
            /varset AHgotoTop TRUE
            /return
         }
         /if (!${Me.Casting.ID} && ${Cast.Result.Equal[CAST_INTERRUPTED]}) { 
            /varset AHtTargetID 0
            /varset AHgotoTop TRUE
            /return
         }
      }
:tCastHeal
      /if (${tGem}>=40) {
         /if (${Target.ID}!=${AHbTargetID} && ${AHbTargetID}) /squelch /tar id ${If[${AHbTargetID}==99999,${Me.ID},${AHbTargetID}]}
         /if (${tGem}==40) /call MBScript ${AHSpell[${a}]}
         /if (${tGem}==50) /doability ${AHSpell[${a}]}
         /return
      }
      /varset NeedtoHeal TRUE
      /if (${TankName.Equal[${Me.CleanName}]} && ${Target.ID}==${ACMATarget}) /varset AHPrevTarget ${Target.ID}

      /call CastHeal ${a} ${AHbTargetID} ${tSpellID} ${tSpellRange}   
      /if (${tPRSpell}) {
         /varset AHcontinue TRUE
         /return
      }
:tNextTarget
      /if (${Window[RespawnWnd].Open}) /return	
   /next b
/return


Sub CastHeal(int a, int tTargetID, int tSpellID, int tSpellRange)
   /varset tPRSpell FALSE
   /if (!${AHtTargetID} && ${Cast.Status.Equal[i]} && ${Cast.Ready[${AHSpell[${a}]}|${AHGem[${a}]}]}) {
      /if (${Me.State.Equal[sit]}) /stand
      /if (${MakeCamp.Status.Equal[on]}) /makecamp pause
      /if (${Stick.Status.Equal[on]}) {
         /stick pause
         /delay 3 !${Me.Moving}
      }
      /if (${AdvPath.Following} && !${AdvPath.Paused} && ${FollowFlag}<2) { 
         /varset FollowFlag 3
         /afollow pause
      }
      /varset MBCurrentSpell AH|${a}|${If[${tTargetID}==99999,0,${tTargetID}]}
      /if (${Spell[${tSpellID}].TargetType.Find[self]} && ${tTargetID}==${Me.ID} || ${tTargetID}==99999) /varset tTargetID ${Me.ID}

      /casting "${AHSpell[${a}]}|${AHGem[${a}]}" ${If[!${tTargetID}, ,-targetID|${tTargetID}]} ${AHSpellFoci[${a}]} -maxtries|2

      /if (${AHHealMode.Arg[1,|]} && ${a}<=${AHHealMode.Arg[2,|]}) /varset HealOnlyTimer ${AHHealMode.Arg[3,|]}s
      /varset AHtTargetID ${tTargetID}
      /varset AHtSpellID ${tSpellID}
      /varset AHtSpellRange ${tSpellRange}
      /varset AHtCnt ${a}
      /varset AHtTime ${Math.Calc[${Spell[${tSpellID}].MyCastTime}/100]}
      /varset AHtTimer 1000
      /varset CastTimer ${Math.Calc[${Spell[${tSpellID}].MyCastTime}/100]}
      /varset AHtMembers ${tMembers}
      /if (!${AHtTargetID} || ${AHtTargetID}==99999) /varset AHtTargetID ${Me.ID}
      /varset AHtTargetHP ${If[${Target.ID}==${AHtTargetID},${Target.PctHPs},${Spawn[${AHtTargetID}].PctHPs}]}

      /if (${AHAnnounce[${a}].Length}) {
         /if (${AHAnnounce[${a}].Length}>10) {
            /call ParseAnnounce "${AHAnnounce[${a}]}" "${Spawn[${AHtTargetID}].CleanName}" "${AHSpell[${a}]}"
         } else {
            /docommand ${AHAnnounce[${a}]} Healing: -[ ${Spawn[${AHtTargetID}].CleanName} ]- with ${AHSpell[${a}]}
         }
      }
      /varset AHtDPS 1
      /varset AHtDPSAvr 1
      /varset AHtPctCntr 1
      /varset AHtPreHP ${Spawn[${tTargetID}].PctHPs}
      /if (${tTargetID} && ${tTargetID}!=99999) /varset AHtPreID ${tTargetID}
      /varset AHttl 100000
      /varset AHtPriority ${tPriority}
      /if (${AHtCnt}==${PRSpell}) {
         /varset tPRSpell TRUE
         /return
      }
   }
   /varset tHurt 0
   /varset tMembers
/return


Sub CalcPPS(int tSpellID)
   /if (!${AHtTargetID} || ${Spawn[${AHtTargetID}].Type.Equal[corpse]}) /return
   /if (!${AHtPreID}) /varset AHtPreID ${AHtTargetID}
| Calculate mob DPS and find the time-to-live to compare to cast time.
   /if (${Target.ID}!=${AHtPreID} && !${Spawn[group ${AHtPreID}].ID} && ${AHtPreID} && ${Spawn[${AHtPreID}].ID}) /target id ${AHtPreID}
   /delay 5 (${Spawn[group ${AHtPreID}].ID}||${Target.ID}==${AHtPreID})
   /if (!${Spawn[group ${AHtPreID}].ID} && ${AHtPreHP}>${Target.PctHPs} && ${Target.PctHPs}!=100) {
      /varcalc AHtDPS ${AHtDPS}+(${AHtPreHP}-${Target.PctHPs})
   } else {
      /if (${Spawn[group ${AHtPreID}].ID} && ${AHtPreHP}>${Spawn[group ${AHtPreID}].PctHPs} && ${Spawn[group ${AHtPreID}].PctHPs}!=100) /varcalc AHtDPS ${AHtDPS}+(${AHtPreHP}-${Spawn[group ${AHtPreID}].PctHPs})
   }
   /varcalc AHtPctCntr ${AHtDPS}/((1001-${AHtTimer})/10)
   /if (${AHtTimer}<900) {
      /varcalc AHtDPS ${AHtPctCntr}
      /varcalc AHtTimer 1000
   }
   /if (!${AHtPctCntr}) {
      /varset AHttl 10000
   } else {
      /varcalc AHttl (${Spawn[${AHtPreID}].PctHPs}/${AHtPctCntr})*100
   }
   /varset AHtPreHP ${Target.PctHPs}
|  /echo Healing! CastTimer: ${CastTimer} TTL: ${AHttl}  == ${Math.Calc[${CastTimer}*10]}>${AHttl} ? (${CastTimer}>(${Math.Calc[${Spell[${tSpellID}].MyCastTime}/100]})
/return


Sub GrpEval(string AHLine, int radius, int tarcnt, int a)
|  Check self then tank and grp HP, then look for netbots clients out of group and pets (use Netbots only if out of group). return string of ID's with count as last arg.
   /declare i int local
   /declare matches string local
   /declare totalcount int local 0
   /declare Lowest int local 0
   /declare TankID int local
   /declare tPctHPs int local

   /if (${AHLine.Find[self]}) {
      /if (${Me.PctHPs}<${AHThreshold[${a},1]}) /return "${Me.ID}|1"
      /return FALSE
   }

   /if (${AHLine.Find[mypet]}) {
      /if (${Me.Pet.ID} && ${Me.Pet.PctHPs}<${AHThreshold[${a},2]}) {
         /if (!${radius} || ${Me.Pet.Distance}<=${radius}) /return "${Me.Pet.ID}|1"
      }
      /return FALSE
   }

|  Check Grp (Group.Injured)
   /if (${AHLine.Find[grp]}) {
      /if (${Group.Injured[${AHThreshold[${a},4]}]}>=${tarcnt}) /return "${Me.ID}|1"
      /return FALSE
   }

|  Check Tank
   /if (${AHLine.Find[tank]} && ${AHLine.Find[pc]}) {
      /if (!${AHLine.Find[group]} || ${Group.Member[${TankName}].Index}) {
         /varset TankID ${Spawn[=${TankName}].ID}
         /varset tPctHPs ${If[${Group.Member[${TankName}].Index} || !${NetBots[${TankName}].ID},${Spawn[${TankID}].PctHPs},${NetBots[${TankName}].PctHPs}]}
         /if (${TankID} && ${tPctHPs}<${AHThreshold[${a},3]}) {
            /if (!${radius} || ${Spawn[${TankID}].Distance}<=${radius}) /return "${TankID}|1"
         }
      }
   }

   /if (!${AHThreshold[${a},1]} && !${AHThreshold[${a},2]}) /return FALSE

|  Check myself	
   /if (${AHLine.Find[${Me.Class.ShortName}]} && ${AHLine.Find[pc]} && ${Me.PctHPs}<${AHThreshold[${a},1]}) {
      /if (${AHThreshold[${a},5]}) {
         /call CheckAHArgs "${AHLine}"|${Me.Class.ShortName}
         /if (!${Macro.Return} || ${Macro.Return}<${Me.PctHPs}) /goto :tCheckmypet
      }
      /varset Lowest ${Me.PctHPs}
      /varset matches ${Me.ID} ${matches}
      /varcalc totalcount ${totalcount}+1
   }

|  Check my pet
:tCheckmypet
   /if (${Me.Pet.ID} && ${AHLine.Find[pet]} && ${Me.Pet.PctHPs}<${AHThreshold[${a},2]}) {
      /if (${Select[${Me.Class.ShortName},shm,nec,mag,bst,dru,enc,shd]} && !${Me.Pet.Name.Find[familiar]}) {
         /varset matches ${matches} ${Me.Pet.ID}
         /varcalc totalcount ${totalcount}+1
      }
   }

|  Check group members/pets (other than myself/my pet)
   /if (${Group.Members}>0) {
      /if (${Group.Injured[${AHThreshold[${a},1]}]}>=1) {
         /for i 1 to ${Group.Members}
            /if (${Group.Member[${i}].Present} && ${Group.Member[${i}].PctHPs}<${AHThreshold[${a},1]} && !${Group.Member[${i}].Type.Equal[corpse]} && ${AHLine.Find[${Group.Member[${i}].Class.ShortName}]}) {
               /if (${AHThreshold[${a},5]}) {
                  /call CheckAHArgs "${AHLine}"|${Group.Member[${i}].Class.ShortName}
                  /if (!${Macro.Return} || ${Macro.Return}<${Group.Member[${i}].PctHPs}) /goto :tNextGrp
               }
               /if (!${radius} || ${Group.Member[${i}].Distance}<=${radius}) {
                  /if (!${Lowest} || ${Group.Member[${i}].PctHPs}<${Lowest}) {
                     /varset Lowest ${Group.Member[${i}].PctHPs}
                     /varset matches ${Group.Member[${i}].ID} ${matches}
                  } else {
                     /varset matches ${matches} ${Group.Member[${i}].ID}
                  }
                  /varcalc totalcount ${totalcount}+1
               }
            }
:tNextGrp
         /next i
      }
      /if (${AHThreshold[${a},2]}) { 
         /for i 1 to ${Group.Members}
            /if (${Group.Member[${i}].Pet.ID} && ${Group.Member[${i}].Pet.PctHPs}<${AHThreshold[${a},2]}) {
               /if (${Select[${Group.Member[${i}].Class.ShortName},shm,nec,mag,bst,dru,enc,shd]} && !${Group.Member[${i}].Pet.Name.Find[familiar]}) {
                  /if (!${radius} || ${Group.Member[${i}].Pet.Distance}<=${radius}) { 
                     /varset matches ${matches} ${Group.Member[${i}].Pet.ID}
                     /varcalc totalcount ${totalcount}+1
                  }
               }
            }
         /next i
      }
   }
 
|  Check NetBots members/pets only out of group since group check already done.  woobs
   /if (!${AHLine.Find[group]}) { 
      /for i 1 to ${NetBots.Counts}
         /if (${AHThreshold[${a},1]}) { 
            /if (${Group.Member[${NetBots.Client.Arg[${i}]}].Index} || ${NetBots.Client.Arg[${i}].Equal[${Me.CleanName}]} || !${Spawn[pc ${NetBots.Client.Arg[${i}]}].PctHPs} || ${Spawn[${NetBots.Client.Arg[${i}]}].Type.Equal[corpse]}) /goto :tNextNB
            /if (${NetBots[${NetBots.Client.Arg[${i}]}].PctHPs}<${AHThreshold[${a},1]} && ${AHLine.Find[${NetBots[${NetBots.Client.Arg[${i}]}].Class.ShortName}]}) {
               /if (${AHThreshold[${a},5]}) {
                  /call CheckAHArgs "${AHLine}"|${NetBots[${NetBots.Client.Arg[${i}]}].Class.ShortName}
                  /if (!${Macro.Return} || ${Macro.Return}<${NetBots[${NetBots.Client.Arg[${i}]}].PctHPs}) /goto :tCheckNBpet
               }
               /if (!${radius} || ${Spawn[pc ${NetBots.Client.Arg[${i}]}].Distance}<=${radius}) { 
                  /if (!${Lowest} || ${NetBots[${NetBots.Client.Arg[${i}]}].PctHPs}<${Lowest}) { 
                     /varset Lowest ${NetBots[${NetBots.Client.Arg[${i}]}].PctHPs}
                     /varset matches ${NetBots[${NetBots.Client.Arg[${i}]}].ID} ${matches}
                  } else {
                     /varset matches ${matches} ${NetBots[${NetBots.Client.Arg[${i}]}].ID}
                  }
                  /varcalc totalcount ${totalcount}+1
               }
            }
         }
:tCheckNBpet
         /if (${AHThreshold[${a},2]}) { 
            /if (${NetBots[${NetBots.Client.Arg[${i}]}].PetID} && ${NetBots[${NetBots.Client.Arg[${i}]}].PetHP}<${AHThreshold[${a},2]}) {
               /if (${Select[${NetBots[${NetBots.Client.Arg[${i}]}].Class.ShortName},shm,nec,mag,bst,dru,enc,shd]} && !${Spawn[${NetBots[${NetBots.Client.Arg[${i}]}].PetID}].Name.Find[familiar]}) {
                  /if (!${radius} || ${Spawn[${NetBots[${NetBots.Client.Arg[${i}]}].PetID}].Distance}<=${radius}) {
                     /varset matches ${matches} ${NetBots[${NetBots.Client.Arg[${i}]}].PetID}
                     /varcalc totalcount ${totalcount}+1
                  }
               }
            }
         } 
:tNextNB		
      /next i
   }
   /if (${matches.Length} && ${totalcount}) /return "${matches}|${totalcount}"
/return FALSE


Sub CheckAHArgs(string CheckString)
   /declare AHLine string local ${CheckString.Arg[1,|]}
   /declare ClassChk string local ${CheckString.Arg[2,|]}
   /declare ParseLine string local
   /if (${AHLine.Find[${ClassChk}]}) {
| Find if class shortname has a value, if not - use hp setting
:tRecheck
      /if (${AHLine.Arg[1].Find[${ClassChk}]}) {
         /varset ParseLine ${AHLine.Arg[1]}
      } else {		
         /varset ParseLine ${AHLine.Right[-${Math.Calc[${AHLine.Find[${ClassChk}]}-1]}]}
      }	
      /if (${AHLine.Find[hp]} && ${ClassChk.NotEqual[hp]} && (!${ParseLine.Arg[1].Find[${ClassChk}]} || ${ParseLine.Arg[1].Length}==${ClassChk.Length} || ${ParseLine.Arg[1].Equal[NULL]})) {
         /varset ClassChk hp
         /goto :tRecheck
      }
      /if (${ParseLine.Arg[1].Length}!=${ClassChk.Length}) /call IsNumber ${ParseLine.Arg[1].Right[-${ClassChk.Length}]}
      /if (${Macro.Return.Length} && ${Macro.Return.NotEqual[FALSE]}) /return ${ParseLine.Arg[1].Right[-${ClassChk.Length}]}
|     /if (${Macro.Return.Equal[FALSE]}) /echo False Return from ${ParseLine.Arg[1]}
   }
|  /echo Problem determining heal percentage for Class ${Class} in AHClass line of ${AHLine}
/return FALSE


Sub event_AHDA
   /docommand /${BeepCommand}
   /bc DA on ME
   /echo DA on ME
/return