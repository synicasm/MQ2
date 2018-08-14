
Sub LoadAQINIVals
   /declare a int local
   /call LoadINIVar AdvCure AQCount 0 AQCount "${IniFile}" int
   /call LoadINIVar AdvCure AQNewFormat 1 AQNewFormat "${IniFile}" int
   /declare AQCheckTimer timer outer
   /if (${AQCount}) {
      /call LoadINIVar AdvCure AQCheckTime 8 AQCheckTime "${IniFile}" int
|once I enter this sub, do I leave before I'm 100% done?
      /declare AQPriority bool outer
      /declare AQGem[${AQCount}] string outer
      /declare AQSpell[${AQCount}] string outer
|curetype - poisoned, cursed, diseased, all
      /declare AQSpellCureType[${AQCount}] string outer
      /declare AQSpellMinMana[${AQCount}] string outer
|amount of counters cured per cast -  not used
      /declare AQSpellCntr[${AQCount}] int outer
      /declare AQSpellFoci[${AQCount}] string outer
      /declare AQSpellRecast[${AQCount}] int outer
      /declare AQTarCnt[${AQCount}] int outer
|tartype war shm nec ? grp ? self? cbt?
      /declare AQTarType[${AQCount}] string outer
      /declare AQSpellAlias[${AQCount}] string outer
      /declare AQAnnounce[${AQCount}] string outer
      /declare tSpellID int local
      /for a 1 to ${AQCount}
         /if (${AQNewFormat}) {
            /call LoadINIVar AQ${a} Gem " " AQGem[${a}] ${IniFile}
            /call LoadINIVar AQ${a} Spell " " AQSpell[${a}] ${IniFile}
            /call LoadINIVar AQ${a} SpellCntr 0 AQSpellCntr[${a}] ${IniFile}
            /call LoadINIVar AQ${a} SpellFoci " " AQSpellFoci[${a}] ${IniFile}
            /call LoadINIVar AQ${a} SpellCureType "Cursed Diseased Poisoned Corrupted EnduDrain LifeDrain ManaDrain Blind CastingLevel Charmed Feared Healing Mesmerized Resistance Rooted Silenced Slowed Snared SpellCost SpellSlowed SpellDamage Trigger All" AQSpellCureType[${a}] ${IniFile}
            /call LoadINIVar AQ${a} SpellMinMana 0 AQSpellMinMana[${a}] ${IniFile}
            /call LoadINIVar AQ${a} SpellRecast " " AQSpellRecast[${a}] ${IniFile}
            /call LoadINIVar AQ${a} TarCnt " " AQTarCnt[${a}] ${IniFile}
            /call LoadINIVar AQ${a} TarType "grp cbt idle " AQTarType[${a}] ${IniFile}
            /call LoadINIVar AQ${a} SpellAlias " " AQSpellAlias[${a}] ${IniFile}
            /call LoadINIVar AQ${a} Announce " " AQAnnounce[${a}] ${IniFile}
         } else {
            /call LoadINIVar AdvCure AQGem${a} " " AQGem[${a}] ${IniFile}
            /call LoadINIVar AdvCure AQSpell${a} " " AQSpell[${a}] ${IniFile}
            /call LoadINIVar AdvCure AQSpellCntr${a} 0 AQSpellCntr[${a}] ${IniFile}
            /call LoadINIVar AdvCure AQSpellFoci${a} " " AQSpellFoci[${a}] ${IniFile}
            /call LoadINIVar AdvCure AQSpellCureType${a} "Cursed Diseased Poisoned Corrupted EnduDrain LifeDrain ManaDrain Blind CastingLevel Charmed Feared Healing Mesmerized Resistance Rooted Silenced Slowed Snared SpellCost SpellSlowed SpellDamage Trigger All" AQSpellCureType[${a}] ${IniFile}
            /call LoadINIVar AdvCure AQSpellMinMana${a} 0 AQSpellMinMana[${a}] ${IniFile}
            /call LoadINIVar AdvCure AQSpellRecast${a} " " AQSpellRecast[${a}] ${IniFile}
            /call LoadINIVar AdvCure AQTarCnt${a} " " AQTarCnt[${a}] ${IniFile}
            /call LoadINIVar AdvCure AQTarType${a} "grp cbt idle " AQTarType[${a}] ${IniFile}
            /call LoadINIVar AdvCure AQSpellAlias${a} " " AQSpellAlias[${a}] ${IniFile}
            /call LoadINIVar AdvCure AQAnnounce${a} " " AQAnnounce[${a}] ${IniFile}
         }
         /if (${AQGem[${a}].NotEqual[script]} && ${AQSpell[${a}].Length}) {
            /if (${Select[${AQGem[${a}]},1,2,3,4,5,6,7,8,9,10,11,12,13]}) {
               /if (${Me.Book[${Me.Book[${AQSpell[${a}]}]}].ID}) {
                  /varset AQSpell[${a}] ${Me.Book[${Me.Book[${AQSpell[${a}]}]}].Name}
               } else /if (${Me.Book[${Me.Book[${AQSpell[${a}]} ]}].ID}) {
                  /varset AQSpell[${a}] ${Me.Book[${Me.Book[${AQSpell[${a}]} ]}].Name}
               } else /if (${Me.Book[${Spell[${AQSpell[${a}]}].RankName}]}) {
                  /varset AQSpell[${a}] ${Spell[${AQSpell[${a}]}].RankName}
               } else {
                  /bc Spell ${AQSpell[${a}]} Not found in your book.  Check your .ini settings.
                  /varset AQTarCnt[${a}] 0
               } 
            } else /if (${AQGem[${a}].Equal[alt]}) {
               /if (!${Me.AltAbility[${AQSpell[${a}]}].ID} && !${Me.Shrouded}) {
                  /bc You don't have the AltAbility ${AQSpell[${a}]}.  Check your .ini settings
                  /varset AQTarCnt[${a}] 0
               }
            } 
         } 
      /next a
   }
/return


Sub AQCureCheck
   /if (${AQCheckTimer} || !${DoCures} || ${MBPause} || !${AQCount}) /return
   /varset AQCheckTimer ${AQCheckTime}s
   /if (${MBDebug}) /echo Macro.RunTime:${Macro.RunTime} - Sub:AQCureCheck
   /declare x int local
   /declare tBotCnt int local ${NetBots.Counts}
   /declare tBotClient string local ${NetBots.Client}

   /for x 1 to ${tBotCnt}
|     /if (${NetBots[${tBotClient.Arg[${x}]}].Detrimentals} > ${NetBots[${tBotClient.Arg[${x}]}].Invulnerable}) /goto :cCureNeeded
      /if (${NetBots[${tBotClient.Arg[${x}]}].Detrimentals} > ${NetBots[${tBotClient.Arg[${x}]}].NoCure}) /goto :cCureNeeded 
| Testing
|     /if (${NetBots[${tBotClient.Arg[${x}]}].Detrimentals} > 0) {
|        /bc Detrimentals on ${NetBots[${tBotClient.Arg[${x}]}].Name} but showing NoCure.
|        /goto :cCureNeeded
|     }
   /next x

   /return

:cCureNeeded
|/echo Cure needed - ${NetBots[${tBotClient.Arg[${x}]}].Name}
   /declare a int local
   /declare b int local
   /declare c int local
   /declare tGem int local
   /declare tSpellID int local
   /declare tSpellRange int local
   /declare tSpellDuration int local
   /declare tSpellType string local
   /declare tCastTime timer local
   /declare tCT int local
   /declare tTargetID int local
   /declare AQtTargetID int local
   /declare tRecast int local
   /declare tUpdated int local
   /declare existingTar int local ${Target.ID}
   /declare tBuffs string local
   /declare tSongs string local
   /declare tSpellFound bool local

   /if (${Melee.Combat} && ${OffTank}) /varset existingTar ${Target.ID}
   /for a 1 to ${AQCount}
      /if (!${DoCures} || ${MBPause} || ${NetBots[${Me.CleanName}].Silenced} || ${NetBots[${Me.CleanName}].Stunned}  || ${NetBots[${Me.CleanName}].Charmed}) /return
      /if (${Me.Ducking}) /keypress duck
      /if (!${AQTarCnt[${a}]} || (!${AQTarType[${a}].Find[cbt]} && ${ADMobCount}) || (${AQTarType[${a}].Find[cbt]} && !${ADMobCount} && !${AQTarType[${a}].Find[idle]})) /goto :tNextCureSpell
      /for b 1 to ${tBotCnt}
|        /if (!${NetBots[${tBotClient.Arg[${b}]}].Detrimentals} > ${NetBots[${tBotClient.Arg[${x}]}].Invulnerable}) /goto :tNextBot
         /if (!${NetBots[${tBotClient.Arg[${b}]}].Detrimentals} > ${NetBots[${tBotClient.Arg[${x}]}].NoCure}) /goto :tNextBot
         /if (${AQTarType[${a}].Find[grp]} && !${tBotClient.Arg[${b}].Equal[${Me.CleanName}]} && (!${Me.Grouped} || !${Group.Member[${tBotClient.Arg[${b}]}].Index})) /goto :tNextBot
         /if (${AQTarType[${a}].Find[self]} && !${tBotClient.Arg[${b}].Equal[${Me.CleanName}]}) /goto :tNextBot
         /if (${NetBots[${tBotClient.Arg[${b}]}].Buff.Arg[1].Equal[756]}) /goto :tNextBot 
         /if (${AQSpellCureType[${a}].Find[all]}) /goto :cSpellFound

         /varset tSpellFound FALSE 
         /for c 1 to ${Math.Calc[${AQSpellCureType[${a}].Count[ ]}+1]}
            /if (${NetBots[${tBotClient.Arg[${b}]}].${AQSpellCureType[${a}].Arg[${c}, ]}}) {
               /varset tSpellFound TRUE
               /break
            }
         /next c  
         /if (!${tSpellFound}) /goto :tNextBot

:cSpellFound
         /if (${Corpse.Open}) /keypress esc

|/echo Cure Needed 2 - ${NetBots[${tBotClient.Arg[${b}]}].Name} - ${AQSpell[${a}]} 
         /if (${Select[${AQGem[${a}]},1,2,3,4,5,6,7,8,9,10,11,12,13]}) {
|/echo was Spell
            /varset tSpellID ${Me.Book[${Me.Book[${AQSpell[${a}]}]}].ID} 
            /varset tGem ${AQGem[${a}]} 
            /varcalc tCT ${Spell[${tSpellID}].MyCastTime}/100+40
            /if (${Me.CurrentMana}<${Me.Book[${Me.Book[${AQSpell[${a}]}]}].Mana} || ${Me.PctMana}<${AQSpellMinMana[${a}]}) /break
            /if (${Me.Gem[${tGem}].ID}!=${tSpellID}) {
               /if (${Me.Casting.ID}) /delay 2s !${Me.Casting.ID}
               /squelch /memorize "${Spell[${tSpellID}]}|gem${tGem}"
               /delay 5s ${Me.Gem[${tGem}].ID}==${tSpellID}
               /if (${Me.Gem[${tGem}].ID}!=${tSpellID}) { 
                  /bc CURE - ${AQSpell[${a}]} - FAILED MEMORIZE
                  /break
               }
               /varcalc tCT ${Spell[${tSpellID}].RecastTime}/100+${tCT}
            }
            /if (${Me.Class.ShortName.NotEqual[BRD]} && ${Cast.Ready} && !${Cast.Ready[${tSpellID}|${tGem}]} && (${Me.GemTimer[${tGem}]}>4000 || (${Me.GemTimer[${tGem}]}>3000 && !${Me.SpellInCooldown}))) /break
         } else /if ( ${AQGem[${a}].Find[item]} && ${Bool[${FindItem[=${AQSpell[${a}]}]}]} ) {
|/echo was Item
            /if (${FindItem[=${AQSpell[${a}]}].Timer} ) /break
            /varset tSpellID ${FindItem[=${AQSpell[${a}]}].Spell.ID}
            /varset tGem 20 
            /varcalc tCT ${FindItem[=${AQSpell[${a}]}].CastTime}*10+40
         } else /if (${AQGem[${a}].Equal[alt]} && ${Me.AltAbility[${AQSpell[${a}]}].ID}) {
|/echo was Alt
            /varset tSpellID ${Me.AltAbility[${AQSpell[${a}]}].ID}
            /if (!${Me.AltAbilityReady[${AQSpell[${a}]}]} || ${Me.CurrentMana}<${Me.AltAbility[${AQSpell[${a}]}].Spell.Mana}) /break
            /varset tSpellID ${Me.AltAbility[${AQSpell[${a}]}].Spell.ID} 
            /varset tGem 30 
            /varcalc tCT ${Me.AltAbility[${AQSpell[${a}]}].Spell.CastTime}/100+40
         } else /if (${AQGem[${a}].Equal[script]}) {
|/echo was Script
            /call MBScript ${AQSpell[${a}]}
            /goto :tContCure
         }
         /if (!${tGem} || ${Me.PctMana}<${AQSpellMinMana[${a}]} || !${tSpellID}) /break
         /if (${Spell[${tSpellID}].Range}) {
            /if (${Spawn[${tBotClient.Arg[${b}]}].Distance}>${Spell[${tSpellID}].Range}) /break
         } else /if (${Spell[${tSpellID}].AERange}) {
            /if (${Spawn[${tBotClient.Arg[${b}]}].Distance}>${Spell[${tSpellID}].AERange}) /break
         }
         /if (${Spell[${tSpellID}].TargetType.Equal[free target]} && !${Spawn[${tBotClient.Arg[${b}]}].CanSplashLand}) /break
         /varset tTargetID ${NetBots[${tBotClient.Arg[${b}]}].ID}
         /if (!${tTargetID}) /goto :tNextBot
         /varset AQtTargetID 0
         /varset tRecast 0
         /varset tCastTime ${tCT}
:tCastCure
         /if (${NetBots[${Me.CleanName}].Silenced} || ${NetBots[${Me.CleanName}].Stunned} || ${Window[RespawnWnd].Open}) /return
         /doevents eqbc
         /if (${AQtTargetID}) {
            /if (${Spawn[${AQtTargetID}].Type.Equal[corpse]} || !${Spawn[${AQtTargetID}].ID}) { 
               /interrupt
               /goto :tNextBot
            }
            /if (${Me.Class.ShortName.Equal[BRD]}) {
               /doevents
               /if (!${CastResult.Equal[pending]}) {
                  /if (${Select[${CastResult},resisted,stunned,reset]} && (${AQSpellRecast[${a}]} && ${AQSpellRecast[${a}]}>=${tRecast} || !${AQSpellRecast[${a}]})) { 
                     /varcalc tRecast ${tRecast}+1
                     /varset AQtTargetID 0
                     /if (${AQSpellRecast[${a}]}) /goto :tCastCure
                  }
                  /goto :tContCure
               }
               /delay 5
               /if (!${tCastTime} && !${Cast.Timing}) /varset CastResult SUCCESS
               /goto :tCastCure
            } else /if (${Cast.Status.Equal[i]}) {
               /if (${Select[${Cast.Result},cast_recover]}) { 
                  /varset tCastTime ${tCT}+30
                  /varset AQtTargetID 0
                  /echo recovery time issue
                  /goto :tCastCure
               }
               /if (${Select[${Cast.Result},cast_immune,cast_takehold]}) { 
                  /echo immune or did not take hold
                  /goto :tContCure
               }
               /if (${AQSpellRecast[${a}]} && (${Select[${Cast.Result},cast_resist,cast_fizzle,CAST_CANCELLED,CAST_NOTREADY]} && ${AQSpellRecast[${a}]}>=${tRecast} || ${Select[${Cast.Result},cast_interrupted,CAST_CANCELLED]})) {
                  /varcalc tRecast ${tRecast}+1
                  /varset tCastTime ${tCT}+30
                  /varset AQtTargetID 0
                  /echo recast or interrupted
                  /goto :tCastCure
               }
               /if (${Select[${Cast.Result},cast_outofrange,cast_cannotsee,cast_distracted,cast_notarget,CAST_CANCELLED,CAST_NOTREADY]} || !${AQSpellRecast[${a}]} && ${Select[${Cast.Result},cast_resist,cast_fizzle,CAST_CANCELLED,CAST_NOTREADY]}) { 
                  /echo fizzle or resisted
                  /goto :tNextBot
               }
               /if (${Cast.Result.Equal[cast_success]}) /goto :tContCure
            }
         } else /if (${Cast.Ready[${AQSpell[${a}]}|${AQGem[${a}]}]} || ${Me.Class.ShortName.Equal[BRD]}) {
            /if (${Target.ID}!=${tTargetID}) {
               /if (${Me.Class.ShortName.Equal[BRD]}) /squelch /stoptwist
               /if (${Melee.Combat}) /attack off
               /squelch /tar id ${tTargetID}
               /delay 1s ${Target.ID}==${tTargetID}
            }
            /varset AQtTargetID ${tTargetID}
            /varset tCastTime ${tCT}
            /if (${Me.Class.ShortName.Equal[BRD]}) {
               /squelch /twist once ${AQGem[${a}]}
               /varset CastResult PENDING
               /varset tCastTime 32
            } else {
               /varset MBCurrentSpell AQ|${a}|${AQtTargetID}
|              /casting "${AQSpell[${a}]}|${AQGem[${a}]}" -targetid|${AQtTargetID} ${AQSpellFoci[${a}]}
               /call MQ2Cast "${AQSpell[${a}]}" ${If[${Select[${AQGem[${a}]},1,2,3,4,5,6,7,8,9,10,11,12,13]},gem,]}${AQGem[${a}]} ${Spell[${AQSpell[${a}]}].MyCastTime.Float}s CastCheck -targetid|${AQtTargetID} -bandolier|${AQSpellFoci[${a}]}
               /delay 5
            }
         }
         /if (${tCastTime} || ${Cast.Timing} || ${Cast.Status.Equal[i]}) /goto :tCastCure
         /echo Cast took too long, aborting this cast ${Spell[${tSpellID}].Name}
         /echo ${Cast.Status} - ${Cast.Result} - ${AQtTargetID} ${Spawn[${AQtTargetID}].Name}
         /echo a=${a} b=${b} c=${c} 
         /goto :tNextBot
:tContCure
         /varset MBCurrentSpell  
         /if (${AQAnnounce[${a}].Length} && !${Select[${Cast.Result},cast_immune,cast_takehold]} && !${CastResult.Equal[immune]}) /docommand ${If[${AQAnnounce[${a}].Length}>5,/call ParseAnnounce "${AQAnnounce[${a}]}" "${Spawn[${AQtTargetID}].CleanName}" "${AQSpell[${a}]}",/docommand ${AQAnnounce[${a}]} Curing: -[ ${Spawn[${AQtTargetID}].CleanName} ]- with ${AQSpell[${a}]}]}
|        /docommand ${AQAnnounce[${a}]} Curing: -[ ${Spawn[${AQtTargetID}].CleanName} ]- with ${AQSpell[${a}]}
         /return
         /delay 5
:tNextBot
      /next b
:tNextCureSpell
   /next a
/return
