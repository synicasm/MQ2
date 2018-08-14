
#Event MobProb "#*#Your target is too far away#*#"
#Event MobProb "#*#You cannot see your target#*#"
#Event MobProb "#*#You can't hit them from here#*#"
#Event PetTargetMez "|${Me.Pet.CleanName}| tells you, 'I am unable to wake#*#"
#Event EnrageStart "#*# has become ENRAGED#*#"
#Event EnrageEnd "#*# is no longer enraged#*#"


Sub LoadACINIVals
   /declare ACStartX int outer
   /declare ACStartY int outer
   /declare ACStartHeading int outer
   /declare ACMATarget int outer
   /declare ACState int outer
   /declare ACBeginHP int outer
   /declare ACEndHP int outer
   /declare ACTarID int outer
   /declare ACMobProbT timer outer 5s
   /declare ReTargetT timer outer 5s
   /call LoadINIVar Melee OffTank FALSE OffTank "${IniFile}" bool
   /call LoadINIVar Melee ACLeash 50 ACLeash "${IniFile}" int
   /call LoadINIVar Melee ACAssistPct 95 ACAssistPct "${IniFile}" int
   /call LoadINIVar Melee ACManaPct 70 ACManaPct "${IniFile}" int
   /call LoadINIVar Melee ACAnnounce " " ACAnnounce "${IniFile}" string
   /call LoadINIVar Melee ACMeleeCmd "/melee plugin=1" ACMeleeCmd "${IniFile}" string
   /call LoadINIVar Melee ACBefore " " ACBefore "${IniFile}" string
   /call LoadINIVar Melee ACAfter " " ACAfter "${IniFile}" string
   /call LoadINIVar Melee ACPetEnrage TRUE ACPetEnrage "${IniFile}" bool
   /if (${ACMeleeCmd.Length}>10) /docommand ${ACMeleeCmd}
   /assist off
/return


Sub AdvCombat(P0,P1,P2)
   /if (${MBPause}) /return
   /if (${MBDebug}) /echo Macro.RunTime:${Macro.RunTime} - Sub:AdvCombat
|  /if (${DoMelee} && !${Melee.Enable} && (${MakeCampY} && ${MakeCampX} && ${Math.Distance[${Me.Y},${Me.X}:${MakeCampY},${MakeCampX}]}<40 || ${Ini["${MBWayPnt}",${Section},WP1].Length}>2 && ${Math.Distance[${Me.Y},${Me.X}:${Ini["${MBWayPnt}",${Section},WP1].Arg[1]},${Ini["${MBWayPnt}",${Section},WP1].Arg[2]}]}<40)) /squelch /melee plugin=1
   /if (${DoMelee} && !${Melee.Enable} && ${MakeCampY} && ${MakeCampX} && ${Math.Distance[${Me.Y},${Me.X}:${MakeCampY},${MakeCampX}]}<40) /squelch /melee plugin=1
   /if (${TankName.Equal[${Me.Name}]} && ${Target.Type.Equal[npc]} && ${ACMATarget} && ${Spawn[${ACMATarget}].ID} && !${Spawn[${ACMATarget}].Type.Equal[corpse]}) {
      /if (${ReTargetT} || ${Me.Combat}) /return
      /varset ReTargetT 10s
   }
   /if (${Me.State.Equal[feign]} && ${PullerName.Equal[${Me.CleanName}]}) /return

   /declare a int local
   /declare sVarC string local
   /declare sVarL string local
   /declare sNBClnt string local ${NetBots.Client}
   /declare OTID int local ${Target.ID}

   /call ADSpawnCheck
   /for a 0 to ${Group.Members}
      /if (!${NetBots[${Group.Member[${a}].Name}].ID}) /varset sNBClnt ${sNBClnt} ${Group.Member[${a}].Name}
   /next a
   /if (${AHCount}) /call AHHealCheck
   /if (${ACState}==100 || !${ACState} || ${OffTank} && !${ReTargetT} && ${ADMobCount}>1 || ${OffTank} && ${P0.Length}) {
      /if (${TankName.Equal[${Me.CleanName}]} && ${DoPull} && !${ACMATarget} && ${APTargetID}) /varset ACMATarget ${APTargetID}
      /if (!${ACMATarget} && ${TankName.Equal[${Me.CleanName}]} && ${PullerName.Length}>2 && ${PullerName.NotEqual[${Me.CleanName}]} && ${Spawn[npc id ${NetBots[${PullerName}].TargetID}].Distance}<${ACLeash} && !${ExcludeList.Find[|${Spawn[${NetBots[${PullerName}].TargetID}].CleanName}|]} && ${Spawn[${NetBots[${PullerName}].TargetID}].PctHPs}<=${ACAssistPct} && !${DoPull}) {
         /varset ACMATarget ${NetBots[${PullerName}].TargetID}
         /goto :GotTarget
      }
      /if (!${OffTank} || ${ADMobCount}<2) {
         /if (${Spawn[pc ${TankName}].Distance}<200) {
            /if (${NetBots[${TankName}].ID}) {
               /if (!${ExcludeList.Find[|${Spawn[${NetBots[${TankName}].TargetID}].CleanName}|]} && ${Spawn[${NetBots[${TankName}].TargetID}].Standing}) /varset ACMATarget ${NetBots[${TankName}].TargetID}
            } else /if (!${NetBots[${TankName}].ID}) {
               /call ACGetAssist ${TankName}
               /if (!${ExcludeList.Find[|${Spawn[${Macro.Return}].CleanName}|]}) /varset ACMATarget ${Macro.Return}
            }
         }
         /if (${ACMATarget} && ${ACState}!=100 && ${If[${NetBots[${TankName}].ID},${NetBots[${TankName}].TargetHP},${Spawn[${ACMATarget}].PctHPs}]}>${ACAssistPct} || !${Spawn[${ACMATarget}].Standing}) /varset ACMATarget 0
      } else /if (${OffTank}) {
|        /varset ACTarID 0
         /if (${P0.Length}) {
            /if (${Spawn[npc radius 100 ${P0}].ID}) /varset ACTarID ${Spawn[${P0}].ID}
            /if (!${ACTarID}) {
               /if (${sNBClnt.Find[${P0}]} && ${P0.NotEqual[${Me.CleanName}]}) {
                  /varset ACTarID ${NetBots[${P0}].TargetID}
               } else {
                  /call ACGetAssist ${P0}
                  /varset ACTarID ${Macro.Return}
               }
            }
            /varset ReTargetT 5s
            /varset ACBeginHP ${If[${P1.Length},${P1},100]} 
            /varset ACEndHP ${If[${P2.Length},${P2},0]} 
         } else {
:ChangeTarget	    	
            /if (!${TankName.Equal[${Me.CleanName}]} && ${ADMobCount}) {
               /if (!${NetBots[${TankName}].TargetID} && ${Spawn[pc ${TankName}].Distance}<${ACLeash} && ${PullerName.Equal[${Me.CleanName}]} && ${Target.ID}) /delay 4s ${NetBots[${TankName}].TargetID}
               /if (${NetBots[${TankName}].TargetID} && ${ACMATarget} && ${ACTarID} && ${Spawn[${ACTarID}].Type.NotEqual[corpse]} && ${Target.ID}==${ACTarID} && ${NetBots[${TankName}].TargetID}!=${Target.ID} && ${Melee.Combat}) /return
               /if (${Target.ID}==${NetBots[${TankName}].TargetID} && ${ADMobCount}>1) {
                  /varset ACTarID 0
                  /varset ACMATarget 0
               }
               /for a ${ADMobMax} downto 1
                  /if (${ADMobList[${a}]} && ${ADMobList[${a}]}!=${NetBots[${TankName}].TargetID} && ${Spawn[${ADMobList[${a}]}].Type.NotEqual[corpse]} && ${Spawn[${ADMobList[${a}]}].Distance}<${ACLeash} && !${ExcludeList.Find[|${Spawn[${ACTarID}].CleanName}|]}) /varset ACTarID ${ADMobList[${a}]}
               /next a
               /if (${ACTarID}) {
                  /if (${ACTarID} && ${ACTarID}!=${Target.ID} && ${Spawn[${ACTarID}].Type.NotEqual[corpse]}) {
                     /squelch /target id ${ACTarID}
                     /delay 1s ${Target.ID}==${ACTarID}
                  }
                  /if (${ACTarID} && ${ACTarID}!=${Me.ID} && ${Spawn[${ACTarID}].Type.NotEqual[corpse]} && ${ACTarID}!=${NetBots[${TankName}].TargetID}) /bc Off-tanking a ${Spawn[id ${ACTarID}].CleanName}
                  /varset ACBeginHP ${If[${P1.Length},${P1},100]} 
                  /varset ACEndHP ${If[${P2.Length},${P2},0]} 
               }
               /if (!${ACTarID}) /varset ACTarID ${NetBots[${TankName}].TargetID}
            }
         }
         /if ((!${ACTarID} || ${ACState}==100) && !${ExcludeList.Find[|${Spawn[${NetBots[${TankName}].TargetID}].CleanName}|]} && ${Spawn[${NetBots[${TankName}].TargetID}].Standing}) {
            /varset ACTarID ${NetBots[${TankName}].TargetID}
            /varset ACMATarget ${NetBots[${TankName}].TargetID}
         }
         /if (${ACTarID} && ${ACBeginHP}) {
            /if ((${Spawn[${ACTarID}].PctHPs}<${ACBeginHP} || ${Spawn[${ACTarID}].PctHPs}>=${ACEndHP}) && ${Spawn[${ACTarID}].Standing}) /varset ACMATarget ${ACTarID}
         } else {
            /varset ACMATarget 0
            /if (!${TankName.Equal[${Me.CleanName}]} && ${ACTarID} && ${Target.ID}==${ACTarID}) {
               /varset ACTarID ${Target.ID}
               /varset ACMATarget ${Target.ID}
            }
         }
         /if (${TankName.Equal[${Me.CleanName}]} && ${DoPull} && !${ACMATarget} && ${APTargetID}) /varset ACMATarget ${APTargetID}
      }
:GotTarget
      /varset ReTargetT 5s
      /if (${ACMATarget} && ${Select[${Spawn[${ACMATarget}].Type},npc,pet]} && !${Spawn[${ACMATarget}].Master.Type.Equal[pc]} && ${Spawn[${ACMATarget}].Distance3D}<=${ACLeash}) {
         /if (${ACState}==100 && ${Target.ID}!=${ACMATarget}) {
            /squelch /tar id ${ACMATarget}
            /delay 2s ${Target.ID}==${ACMATarget}
         }
         /if ((!${OTID} || !${Spawn[${OTID}].Type.Equal[npc]}) && ${ACMATarget}) /varset OTID ${ACMATarget}
         /if (${ACBefore.Length}>3 && ${OTID}==${ACMATarget}) /call IniCommand "${ACBefore}"
         /if (${ACAnnounce.Length}>5 && ${OTID}==${ACMATarget}) {
            /call ParseAnnounce "${ACAnnounce}" "${Spawn[${ACMATarget}].CleanName}" " "
         } else /if (${ACAnnounce.Length}>1 && ${ACAnnounce.Length}<6) {
            /docommand ${ACAnnounce} Meleeing: -[ ${Spawn[${ACMATarget}].CleanName} ]-
         }
         /if (${FollowFlag}) {
            /varset ACState 2
            /varset FollowFlag 0
            /if (${AdvPath.Active}) {
               /call AdvPlugCall stop
            } else {
               /squelch /stick off
            }
         } else {
            /varset ACStartX ${If[${ACState}==100 && (${Math.Distance[${Me.Y},${Me.X}:${ACStartY},${ACStartX}]}>50 || !${ACStartY}),${Spawn[${TankName}].X},${Me.X}]} 
            /varset ACStartY ${If[${ACState}==100,${Spawn[${TankName}].Y},${Me.Y}]} 
            /varset ACStartHeading ${Me.Heading.DegreesCCW}
            /varset ACState 1
         }
      } else {
         /varset ACState 0
         /varset ACMATarget 0
         /varset ACTarID 0
         /varset ACBeginHP 0
      }
   }

   /if (!${ACState}) /return

   /doevents MobProb
   /if (!${ACMATarget} || ${ACState}>2 || ${Spawn[${ACMATarget}].Type.Equal[corpse]} || !${Spawn[${ACMATarget}].ID} || ${Math.Distance[${Me.Y},${Me.X}:${ACStartY},${ACStartX}]}>${ACLeash} && ${Spawn[pc ${TankName}].Distance}>30) {
      /if (${Me.Class.ShortName.Equal[BRD]} || ${Me.Casting.ID}) {
         /interrupt
         /squelch /stoptwist
      }
      /squelch /tar clear
      /if (${Melee.Combat}) /attack off
      /if (${APTargetID}) {
         /varset APTargetID 0
         /varset APAggro 0
      }
      /delay 1s !${Melee.Combat}
      /squelch /stick off
      /keypress centerview
      /if (${Me.Pet.ID} && ${PetAssist}) {
         /varset PetAssist 1
         /pet back off
      }
      /if (${Select[${ACState},2,4]} && (!${Me.Combat} && !${Melee.Combat} || !${ACMATarget} || ${Spawn[FollowName].Distance}>${ACLeash} || !${CurrentMobCount})) {
         /if (${AdvPath.Active}) {
            /call AdvPlugCall "${FollowName}"
         }
         /varset FollowFlag 1
      } else {
         /if (${CampStatus} && !${FollowFlag} && !${ADMobCount}) {
            /varset CampStatus 1
         } else /if (${Math.Distance[${Me.Y},${Me.X}:${ACStartY},${ACStartX}]}>10 && !${FollowFlag} && !${ADMobCount}) {
            /squelch /moveto loc ${ACStartY} ${ACStartX}
         } else {
            /face Heading ${ACStartHeading}
         }
         /if (${CampStatus} && !${FollowFlag} && !${ADMobCount}) {
           /delay 2s ${Math.Distance[${Me.Y},${Me.X}:${ACStartY},${ACStartX}]}<=10
           /face Heading ${ACStartHeading}
|          /multiline ; /face Heading ${ACStartHeading};/varset ACStartX ${Me.X};/varset ACStartY ${Me.Y}
         }
      }
      /call ADSpawnCheck
      /if (${ACAfter.Length}>3) /call IniCommand "${ACAfter}"
      /if (${DoLoot} && ${LootMobs} && (!${ADMobCount} || ${MobsTooClose}<=5)) {
         /if (${SpawnCount[corpse radius ${CorpseRadius} zradius ${ZRadius} noalert 25]}) /call LootMobs
      }
      /varset ACMATarget 0
      /varset ACState 0
      /varset ACTarID 0
      /varset ACBeginHP 0
      /return
   }
   /if (!${OffTank}) {
      /if (!${ReTargetT}) {
         /varset ReTargetT 3s
         /if (${NetBots[${TankName}].ID} && ${Target.ID}!=${NetBots[${TankName}].TargetID} && !${sNBClnt.Find[${Spawn[${NetBots[${TankName}].TargetID}].CleanName}]}) {
            /if (!${ExcludeList.Find[|${Spawn[${NetBots[${TankName}].TargetID}].CleanName}|]} && ${Spawn[${NetBots[${TankName}].TargetID}].Standing}) /varset ACMATarget ${NetBots[${TankName}].TargetID}
         } else /if (!${NetBots[${TankName}].ID} && ${Target.ID}!=${ACMATarget}) {
            /echo Assisting again to get target
            /call ACGetAssist ${TankName}
            /if (!${ExcludeList.Find[|${Spawn[${Macro.Return}].CleanName}|]} && ${Spawn[${Macro.Return}].Standing}) /varset ACMATarget ${Macro.Return}
         }
      }
   }
   /if (${OffTank} && ${Target.ID}!=${ACMATarget} && ${ACTarID}) {
      /echo 188; ${Target.ID}  : ${ACTarID}
      /varset ACMATarget ${ACTarID}
      /if (${ACTarID} && ${ACBeginHP}) {
         /if  (${Spawn[${ACTarID}].PctHPs}>${ACBeginHP} || ${Spawn[${ACTarID}].PctHPs}<=${ACEndHP}) /varset ACMATarget 0
      } else {
         /varset ACMATarget 0
      }
   }
   /if (${Target.ID}!=${ACMATarget} && ${ACMATarget} && (${Me.PctMana}>${ACManaPct} || !${Me.PctMana} || ${ACManaPct}==101)) {
      /squelch /tar id ${ACMATarget}
      /delay 2s ${Target.ID}==${ACMATarget}
   }
   /if (${Target.Distance3D}<=${ACLeash} && ${ACMATarget}) {
      /if (${Me.Class.ShortName.Equal[BRD]}) {
         /if (${Twist.List.NotEqual[${BardCombatGems} ]} && ${BardCombatGems.Length} > 0) /squelch /twist ${BardCombatGems}
      }
      /if (${Me.Pet.ID} && (${PetAssist}==1 || ${PetAssist} && ${PetAssist}!=${ACMATarget})) {
         /pet attack
         /varset PetAssist ${ACMATarget}
      }
      /pet swarm
      /if (${Me.PctMana}>${ACManaPct} || (${Me.MaxMana}==0 && ${ACManaPct}!=101)) {
         /if (!${Me.Mount.ID} && !${Me.Standing} && (!${Melee.Combat} || !${Me.Combat})) /stand
         /if (${DoMelee} && ${Me.Standing} && ${Target.ID}==${ACMATarget} && (!${Melee.Combat} || !${Me.Combat})) {
            /killthis
            /varset ACMobProbT 3s
         }
      } else /if (${Melee.Combat}) {
         /squelch /tar clear
         /if (${Melee.Combat}) /attack off
         /delay 1s !${Melee.Combat}
         /squelch /stick off
      }
   }
/return


Sub ACGetAssist(P0)
   /declare tTargetID int local 0
   /if (!${P0.Length} || !${Spawn[pc ${P0}].ID} && !${Spawn[mercenary ${P0}].ID}) /return
   /if (${Spawn[pc ${P0}].Distance}>=200) /return
   /assist ${P0}
   /delay 1s ${Target.ID}!=${Spawn[${P0}].ID}
   /varset tTargetID ${If[${Target.ID}!=${Spawn[${P0}].ID} && ${Target.Type.NotEqual[PC]} && ${Target.Standing},${Target.ID},0]}
/return ${tTargetID}


Sub Event_EnrageStart()
   /if (${Me.Pet.ID} && ${ACPetEnrage}) { 
      /pet back off
      /echo Backing off Pet
   }
/return


Sub Event_EnrageEnd()
   /if (${Target.ID}==${ACMATarget} && ${Me.Pet.ID} && ${ACPetEnrage}) {
      /pet attack
      /echo Enrage ended, re-sending Pet.
   }
/return


Sub Event_MobProb
   /if (!${ACMobProbT} && ${Melee.Combat} && ${Me.Standing} && ${Target.ID}==${ACMATarget} && ${Me.Combat} && (!${Me.Casting.ID} || ${Me.Class.ShortName.Equal[brd]})) {
      /if (${Target.Distance3D}<=${ACLeash} && ${Target.Distance3D}>${Target.MaxRangeTo} && !${Stick.Status.NotEqual[on]}) /squelch /stick ${Math.Calc[${Target.MaxRangeTo}-4]}
      /if (!${Melee.Combat}) /killthis
      /if (${Melee.Combat} && ${Stick.Status.NotEqual[on]} && ${Target.Distance}<=25) /squelch /stick 10 ${Target.ID}
      /if (${Melee.Combat} && ${Stick.Status.Equal[on]} && ${Target.Distance}<=25) {
         /if (${Me.Levitating}) /face ${Target}
         /squelch /stick -2
      }
      /varset ACMobProbT 2s
|     /echo MobProb
   }
/return


Sub Event_PetTargetMez
   /if (${Me.Pet.ID} && ${PetAssist}) /varset PetAssist 1
/return
