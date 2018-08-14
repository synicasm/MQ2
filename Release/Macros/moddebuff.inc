
#Event MezResist "#*#target resisted the #1# spell.#*#"

Sub LoadADINIVals
   /call LoadINIVar AdvDebuff ADCount 0 ADCount "${IniFile}" int
   /call LoadINIVar AdvDebuff ADNewFormat 1 ADNewFormat "${IniFile}" int
   
   /call LoadINIVar AdvDebuff ADMobMax 20 ADMobMax "${IniFile}" int
   /call LoadINIVar AdvDebuff ADCheckTime 2 ADCheckTime "${IniFile}" int
   /call LoadINIVar AdvDebuff ADAggroOnly 0 ADAggroOnly "${IniFile}" int
 
   /call LoadINIVar AdvDebuff ADHold "0|1|1|   1=on 0=off|Debuff spell #|Time to wait for debuff|" ADHold "${IniFile}" string
   /call LoadINIVar Settings CheckTargetDebuffs FALSE CheckTargetDebuffs "${MBCommon}" bool

   /declare a int local
   /declare b int local
   /declare ADMobCount int outer 0
   /declare ADCheckTimer timer outer
   /declare ADHoldTimer timer outer
   /declare ADMobList[${ADMobMax}] int outer 0
   /declare MLDebuffed[${ADMobMax}] bool outer FALSE
   /declare CurrentMobCount int outer 0
   /declare SpawnCheckTimer timer outer
   /declare AddSpawn bool outer FALSE
   /declare ADCharmID int outer 0
   /declare ADCharmGem int outer 0
   /declare ADCharmType int outer 0
   /declare ADCharmName string outer
|  /declare ADCharmSpell string outer xxx (We have an #EVENT that will declare this)

   /if (${ADCount}) {
      /declare ADMDL[${ADMobMax},${ADCount}] int outer 0
   } else {
      /declare ADMDL[${ADMobMax},1] int outer 0
   }

   /varset ADCharmSpell xxx
   /if (!${Select[${Me.Class.ShortName},enc,brd]}) /varset ADAggroOnly 0
 
   /if (${ADCount}) {
      /declare ADGem[${ADCount}] string outer
      /declare ADSpell[${ADCount}] string outer
      /declare ADSpellFoci[${ADCount}] string outer
      /declare ADDurMod[${ADCount}] string outer
      /declare ADSpellMinMana[${ADCount}] int outer
      /declare ADSpellRecast[${ADCount}] int outer
      /declare ADSpellCastonResist[${ADCount}] string outer
      /declare ADSpellDelay[${ADCount}] int outer
      /declare ADTarCnt[${ADCount}] int outer
      /declare ADTarType[${ADCount}] int outer
      /declare ADTarBegHP[${ADCount}] int outer
      /declare ADTarEndHP[${ADCount}] int outer
      /declare ADSpellAlias[${ADCount}] string outer
      /declare ADAnnounce[${ADCount}] string outer
      /declare ADDiscTimer[${ADCount}] int outer
      /declare ADIfSpellImmune[${ADCount}] string outer
      /declare ADUseHoTT[${ADCount}] int outer
      /declare ADCondition[${ADCount}] string outer
      /declare ADSameSpell[${ADCount}] int outer
      /declare ADOrigDelay[${ADCount}] int outer
      /declare tSpellID int local
      /for a 1 to ${ADCount}
         /if (${ADNewFormat}) { 
            /call LoadINIVar AD${a} Gem " " ADGem[${a}] ${IniFile}
            /call LoadINIVar AD${a} Spell " " ADSpell[${a}] ${IniFile}
            /call LoadINIVar AD${a} SpellFoci " " ADSpellFoci[${a}] ${IniFile}
            /call LoadINIVar AD${a} DurMod 0 ADDurMod[${a}] ${IniFile}
            /call LoadINIVar AD${a} SpellAlias " " ADSpellAlias[${a}] ${IniFile}
            /call LoadINIVar AD${a} Announce " " ADAnnounce[${a}] ${IniFile}
            /call LoadINIVar AD${a} SpellMinMana 0 ADSpellMinMana[${a}] ${IniFile}
            /call LoadINIVar AD${a} SpellRecast 0 ADSpellRecast[${a}] ${IniFile}
            /call LoadINIVar AD${a} SpellCastonResist " " ADSpellCastonResist[${a}] ${IniFile}
            /call LoadINIVar AD${a} SpellDelay 0 ADSpellDelay[${a}] ${IniFile}
            /call LoadINIVar AD${a} TarCnt 0 ADTarCnt[${a}] ${IniFile}
            /call LoadINIVar AD${a} TarType 0 ADTarType[${a}] ${IniFile}
            /call LoadINIVar AD${a} TarBegHP 0 ADTarBegHP[${a}] ${IniFile}
            /call LoadINIVar AD${a} TarEndHP 0 ADTarEndHP[${a}] ${IniFile}
            /call LoadINIVar AD${a} IfSpellImmune " " ADIfSpellImmune[${a}] ${IniFile}
            /call LoadINIVar AD${a} UseHoTT "0" ADUseHoTT[${a}] ${IniFile}
            /call LoadINIVar AD${a} PreCondition "TRUE" ADCondition[${a}] ${IniFile}
         } else { 
            /call LoadINIVar AdvDebuff ADGem${a} " " ADGem[${a}] ${IniFile}
            /call LoadINIVar AdvDebuff ADSpell${a} " " ADSpell[${a}] ${IniFile}
            /call LoadINIVar AdvDebuff ADSpellFoci${a} " " ADSpellFoci[${a}] ${IniFile}
            /call LoadINIVar AdvDebuff ADDurMod${a} 0 ADDurMod[${a}] ${IniFile}
            /call LoadINIVar AdvDebuff ADSpellAlias${a} " " ADSpellAlias[${a}] ${IniFile}
            /call LoadINIVar AdvDebuff ADAnnounce${a} " " ADAnnounce[${a}] ${IniFile}
            /call LoadINIVar AdvDebuff ADSpellMinMana${a} 0 ADSpellMinMana[${a}] ${IniFile}
            /call LoadINIVar AdvDebuff ADSpellRecast${a} 0 ADSpellRecast[${a}] ${IniFile}
            /call LoadINIVar AdvDebuff ADSpellCastonResist${a} " " ADSpellCastonResist[${a}] ${IniFile}
            /call LoadINIVar AdvDebuff ADSpellDelay${a} 0 ADSpellDelay[${a}] ${IniFile}
            /call LoadINIVar AdvDebuff ADTarCnt${a} 0 ADTarCnt[${a}] ${IniFile}
            /call LoadINIVar AdvDebuff ADTarType${a} 0 ADTarType[${a}] ${IniFile}
            /call LoadINIVar AdvDebuff ADTarBegHP${a} 0 ADTarBegHP[${a}] ${IniFile}
            /call LoadINIVar AdvDebuff ADTarEndHP${a} 0 ADTarEndHP[${a}] ${IniFile}
            /call LoadINIVar AdvDebuff ADIfSpellImmune${a} " " ADIfSpellImmune[${a}] ${IniFile}
            /call LoadINIVar AdvDebuff ADUseHoTT${a} "0" ADUseHoTT[${a}] ${IniFile}
            /call LoadINIVar AdvDebuff ADPreCondition${a} "TRUE" ADCondition[${a}] ${IniFile}
         }
         /if (${Select[${ADGem[${a}]},1,2,3,4,5,6,7,8,9,10,11,12,13]} && ${ADSpell[${a}].Length}) {
            /if (${Me.Book[${Me.Book[${ADSpell[${a}]}]}].ID}) {
               /varset ADSpell[${a}] ${Me.Book[${Me.Book[${ADSpell[${a}]}]}].Name}
            } else /if (${Me.Book[${Me.Book[${ADSpell[${a}]} ]}].ID}) {
               /varset ADSpell[${a}] ${Me.Book[${Me.Book[${ADSpell[${a}]} ]}].Name}
            } else /if (${Me.Book[${Spell[${ADSpell[${a}]}].RankName}]}) {
               /varset ADSpell[${a}] ${Spell[${ADSpell[${a}]}].RankName}
            } else {
               /bc Spell ${ADSpell[${a}]} Not found in your book.  Check your .ini settings.
               /varset ADTarCnt[${a}] 0
            }
         } else /if (${ADGem[${a}].Equal[alt]}) {
            /if (!${Me.AltAbility[${ADSpell[${a}]}].ID} && !${Me.Shrouded}) {
               /bc You don't have the AltAbility ${ADSpell[${a}]}.  Check your .ini settings.
               /varset ADTarCnt[${a}] 0
            }
         } else /if (${ADGem[${a}].Equal[disc]}) {
            /if (${Me.CombatAbility[${Spell[${ADSpell[${a}]}].RankName}]}) {
               /varset ADSpell[${a}] ${Spell[${ADSpell[${a}]}].RankName}
            } else {
               /bc You don't have the ${ADSpell[${a}]} Disc.  Please check ADSpell${a} in your .ini
               /varset ADTarCnt[${a}] 0
            }
         }
         /varset ADOrigDelay[${a}] ADSpellDelay[${a}]
         /call LoadINIVar "${Spell[${ADSpell[${a}]}].Name}" "MaxLevel" "150" "MaxLevel${Spell[${ADSpell[${a}]}].ID}" "${MBCommon}" string
         /if (${CheckTargetDebuffs} && ${Spell[${ADSpell[${a}]}].Duration} && ${Spell[${ADSpell[${a}]}].SpellType.Equal[Detrimental]}) /call LoadINIVar "${Spell[${ADSpell[${a}]}].Name}" "OverwritesID" "| Pipe delimited list of spell ID's this should overwrite - |Number|Number| " "OverWrite${Spell[${ADSpell[${a}]}].ID}" "${MBCommon}" string

         /if (!${Defined[Imm${Spell[${ADSpell[${a}]}].ID}]}) /declare Imm${Spell[${ADSpell[${a}]}].ID} string outer
         /if (${Ini["${MBCommon}","${Spell[${ADSpell[${a}]}].Name}","${Zone.ShortName}Immune"].Find[|]}) /call LoadINIVar "${Spell[${ADSpell[${a}]}].Name}" "${Zone.ShortName}Immune" " " "Imm${Spell[${ADSpell[${a}]}].ID}" "${MBCommon}" string
      /next a
      /if (${PetCast.Find[charm ]}) {
         /varset ADCharmGem ${PetCast.Arg[2]}
         /varset ADCharmType ${PetCast.Arg[3]}
         /varset ADCharmName ${PetCast.Arg[4]}
         /for a 1 to ${ADCount}
            /if (${ADGem[${a}]}==${ADCharmGem}) {
               /varset ADCharmSpell ${ADSpell[${a}]}
               /if (${Me.Pet.ID} && (!${ADCharmType} || ${ADCharmType}==1 && !${Select[${Spawn[${Me.Pet.ID}].Body},undead,vampyre]} || ${ADCharmType}==2 && !${Select[${Spawn[${Me.Pet.ID}].Body},animal]} || ${ADCharmType}==3 && !${Select[${Spawn[${Me.Pet.ID}].Body},summoned]} || ${ADCharmType}==4 && ${ADCharmName.Find[${Spawn[${Me.Pet.ID}].CleanName}]})) {
                  /varset ADCharmID ${Me.Pet.ID}
                  /varset PetAssist 1
                  /varset DoPet TRUE
                  /varset ADMobList[1] ${Me.Pet.ID}
                  /varset ADMDL[1,${a}] ${Spell[${ADCharmSpell}].Duration.TotalSeconds}
               }
            }
         /next a
      }
      /for a 1 to ${ADCount}
         /for b 1 to ${ADCount}
            /if (${Select[${ADGem[${b}]},1,2,3,4,5,6,7,8,9,10,11,12,13,alt]} && ${ADGem[${b}].Equal[${ADGem[${a}]}]} && ${Spell[${ADSpell[${b}]}].ID}==${Spell[${ADSpell[${a}]}].ID} && ${b}!=${a}) {
               /varset ADSameSpell[${a}] ${b}
               /break
            }
         /next b
      /next a
   }
/return


Sub ADSpawnCheck
   /if (${SpawnCheckTimer}) /return
   /declare a int local
   /declare b int local
   /declare tNPCCount int local 0
   /declare npcid int local 0
   /declare tADAdd bool local FALSE

   /varset ADMobCount 0
   /varset AddSpawn FALSE

   /if (${Select[${Zone.ShortName},GuildHall,GuildLobby,PoKnowledge,Nexus,Bazaar,AbysmalSea,potranquility]}) /return FALSE
   /if (${ADAggroOnly}) {
      /declare sNBClnt string local ${NetBots.Client}
      /for a 0 to ${Group.Members}
         /if (!${NetBots[${Group.Member[${a}].Name}].ID}) /varset sNBClnt ${sNBClnt} ${Group.Member[${a}].Name}
      /next a
      /if (${ADAggroOnly}<${Math.Calc[${Macro.RunTime}-3]}) {
         /squelch /alert clear 30
         /varset ADAggroOnly ${Macro.RunTime}
      }
   }
   /if (${ZoneName.NotEqual[${Zone.Name}]}) /call ProcessExcludeList
   /declare TotalNPCs int local ${SpawnCount[any radius ${Radius} zradius ${ZRadius} noalert ${AlertList}]}
:GetSpawn
   /varcalc tNPCCount ${tNPCCount}+1
   /if (${tNPCCount}>${TotalNPCs}) /goto :CleanseArray
   /varset npcid ${NearestSpawn[${tNPCCount},any radius ${Radius} zradius ${ZRadius} noalert ${AlertList}].ID}
   /if (!${npcid}) /goto :CleanseArray
   /if (${Spawn[${npcid}].Type.Equal[pc]} || ${Spawn[${npcid}].Master.Type.Equal[pc]}) /goto :GetSpawn
   /if (!${Spawn[${npcid}].Name.Length} || !${Spawn[${npcid}].Standing} || ${Select[${Spawn[${npcid}].Type},chest,untargetable,trigger,trap,timer,item,object,banner,campfire,mercenary,pc,mount,aura,corpse,flyer]} || ${Spawn[${npcid}].Body.Name.Equal[object]} || ${Spawn[${npcid}].Race.Name.Equal[campfire]} || (${Spawn[${npcid}].Name.Left[1].Equal[_]} && ${Spawn[${npcid}].CleanName.Length}<2)) /goto :GetSpawn
   /if (!${Spawn[${npcid}].LineOfSight}) {
      /if (${ACMATarget}!=${npcid}) /goto :GetSpawn
      /if (${Spawn[${npcid}].Distance3D}>25) /goto :GetSpawn
   }
   /if (${ADAggroOnly}) {
      /if ((${FollowFlag} && !${Spawn[${FollowName}].ID}) || (!${Spawn[id ${npcid} noalert 30].ID})) /goto :GetSpawn
      /if (${Target.ID}!=${npcid}) /squelch /tar id ${npcid}
      /delay 1s ${Target.ID}==${npcid}
      /varset b 0
      /if (${Target.ID}==${npcid}) {
         /if (${Window[TargetOfTargetWindow].Open}) {
            /if (${sNBClnt.Find[${Me.TargetOfTarget.CleanName}]}) /varset b ${npcid}
         } else {
            /assist
            /timed 4 /assist
            /delay 5 ${Target.ID}!=${npcid}
            /if (${sNBClnt.Find[${Spawn[${Target.ID}].CleanName}]}) /varset b ${npcid}
         }
      }
      /if (${b}!=${npcid}) {
         /squelch /alert add 30 id ${npcid}
         /goto :GetSpawn
      }
   }
   /varcalc ADMobCount ${ADMobCount}+1
   /if (${ADMobCount}>${ADMobMax}) {
      /echo Too many mobs to track
      /return ${tADAdd}
   }
   /if (${ADMobList[${ADMobCount}]}) {
      /if (!${Spawn[${ADMobList[${ADMobCount}]}].ID} || ${Spawn[${ADMobList[${ADMobCount}]}].Type.Equal[Corpse]}) {
         /varset ADMobList[${ADMobCount}] 0
      } else {
         /if (${Spawn[${ADMobList[${ADMobCount}]}].Distance}>${Radius}) /varset ADMobList[${ADMobCount}] 0
      }
   }
   /for a 1 to ${ADMobMax}
      /if (${npcid}==${ADMobList[${a}]}) /goto :GetSpawn
   /next a
   /for a 1 to ${ADMobMax}
      /if (!${ADMobList[${a}]}) {
         /if (!${tADAdd}) {
            /varset tADAdd TRUE
            /varset AddSpawn TRUE
         }
         /varset ADMobList[${a}] ${npcid}
         /varset MLDebuffed[${a}] FALSE
         /for b 1 to ${ADCount}
            /varset ADMDL[${a},${b}] 0
         /next b
         /goto :GetSpawn
      }
   /next a
:CleanseArray
   /if (${ADMobMax}>${ADMobCount}) {
      /for a ${Math.Calc[${ADMobMax}-${If[${ADMobCount},${ADMobCount},1]}]} to ${ADMobMax}
         /if (${ADMobList[${a}]}) {
            /varset MLDebuffed[${a}] FALSE
            /for b 1 to ${ADCount}
               /varset ADMDL[${a},${b}] 0
            /next b
         }
      /next a
   }
   /if (${DoPull}) {
      /if (!${ADMobCount} && ${CurrentMobCount}==1) {
         /varset APCheckTimer ${APCheckTime}s
         /varset APPullDelay 0
      }
   }
   /if (${CurrentMobCount} && ${CurrentMobCount}>${ADMobCount}) {
      /varcalc CurrentMobCount ${CurrentMobCount}-1
   } else {
      /varset CurrentMobCount ${ADMobCount}
   }
   /varset SpawnCheckTimer 1s
/return ${tADAdd}


Sub ADDebuffCheck
   /if (${MBPause} || !${ADCount}) /return
   /if (${MBDebug}) /echo Macro.RunTime:${Macro.RunTime} - Sub:ADDebuffCheck
   /varset ADCheckTimer ${ADCheckTime}s
   /varset ADHoldTimer timer 0

   /declare a int local

   /if (!${Defined[tADTime]}) {
      /declare tADTime int outer
      /declare tADCount int outer
      /declare tADAddDetected bool outer
      /declare tADexistingTar int outer
   }
   /varset tADexistingTar 0

   /call ADSpawnCheck
   /if (!${ADMobCount}) {
      /call ADResetTarType
      /return
   }
   /if (${Melee.Combat}) {
      /if (${OffTank}) /varset tADexistingTar ${Target.ID}
      /if (${Me.Class.ShortName.Equal[BRD]}) {
         /if (!${Twist} && ${BardCombatGems.Length}>0) /squelch /twist ${BardCombatGems}
      }
   }

:tADTop
   /varset tADTime ${Macro.RunTime}
   /varset tADCount 1
   /varset tADAddDetected FALSE

:tADCountTop
   /for a 1 to ${ADMobMax}
      /if (${ADMobList[${a}]} && ${Spawn[${ADMobList[${a}]}].ID}) {
         /if (${Spawn[${ADMobList[${a}]}].Distance}<=${Radius}) {
            /varset MLDebuffed[${a}] TRUE
            /if (!${ACState} && ${TankName.Length}>2) {
               /if (${Spawn[pc ${TankName}].Distance}<=200) /call ADSetTarget
            }
            /call DebuffMob ${a}
            /if (${tADAddDetected}) {
               /echo Add detected starting debuffs
               /call ADResetTarType
               /goto :tADTop
            }
         }
      }
      /if (!${DoDebuffs} || ${MBPause} || ${NetBots[${Me.CleanName}].Silenced} || ${NetBots[${Me.CleanName}].Stunned} || ${Window[RespawnWnd].Open}) /goto :tEnd
   /next a
   /if (${ADTarType[${tADCount}]}>9) {
      /varcalc tADCount ${tADCount}+1
      /goto :tADCountTop
   }
:tEnd
   /if (${ADHoldTimer}) /goto :tADTop
   /if (${OffTank} && ${tADexistingTar}) /tar ID ${tADexistingTar}
/return


Sub ADSetTarget
   /if (!${tADexistingTar} && ${ACMATarget}) /varset tADexistingTar ${ACMATarget}
   /if (${NetBots[${TankName}].ID}) {
      /varset ACMATarget ${NetBots[${TankName}].TargetID}
   } else {
      /call ACGetAssist ${TankName}
      /varset ACMATarget ${Macro.Return}
      /if (${PetAssist}>1 && (!${ACMATarget} || !${OffTank} && ${tADexistingTar} && ${tADexistingTar}!=${ACMATarget})) /varset PetAssist 1
   }
/return


Sub ADResetTarType
   /declare a int local
   /for a 1 to ${ADCount}
      /if (${ADTarType[${a}]}>99) {
         /varcalc ADTarType[${a}] ${ADTarType[${a}]}-100
         /if (${ADSpellDelay[${a}]}>=${Macro.RunTime}) /varset ADSpellDelay[${a}] 0
      }
   /next a
/return


Sub DebuffMob(int a)
   /declare b int local
   /declare c int local
   /declare d int local
   /declare e int local
   /declare x int local
   /declare tGem int local
   /declare tSpellID int local
   /declare tSpellRange int local
   /declare tSpellDuration int local
   /declare tSpellDelay int local
   /declare tCastTime timer local
   /declare tCT int local
   /declare ADtTargetID int local
   /declare tTarCnt int local
   /declare tRecast int local
   /declare tCastonRes int local
   /declare HoTTTarget bool local

   /for b ${tADCount} to ${ADCount}
      /if (!${DoDebuffs} || ${MBPause}) /return 
      /varset tCastonRes 0
      /if (${PetCast.Find[charm ]} && ${DoPet}) {
         /if (${ADGem[${b}]}==${ADCharmGem}) {
            /if (${Me.Pet.ID} || (${ADCharmType}==1 && !${Select[${Spawn[${ADMobList[${a}]}].Body},undead,vampyre]}) || (${ADCharmType}==2 && !${Select[${Spawn[${ADMobList[${a}]}].Body},animal]}) || (${ADCharmType}==3 && !${Select[${Spawn[${ADMobList[${a}]}].Body},summoned]}) || (${ADCharmType}==4 && !${ADCharmName.Find[${Spawn[${ADMobList[${a}]}].CleanName}]})) /goto :tNextDebuff
            /if (!${Me.Pet.ID}) {
               /if (${ADCharmID}) {
                  /if (!${Spawn[${ADCharmID}].ID}) {
                     /varset ADCharmID 0
                     /goto :tCheckforAdds
                  }
                  /echo Lost pet, Prior pet was ${ADCharmID} - ${Spawn[${ADMobList[${a}]}].CleanName}
                  /varset ADMDL[${a},${b}] 0
               }
            }
         }
         /if (${ADMobList[${a}]}==${ADCharmID} && ${Me.Pet.ID}) /return
      }
      /if (${ADMDL[${a},${b}]}>=${tADTime} || ${ADTarType[${b}]}>99) /goto :tWhatNext
      /if (!${ADTarCnt[${b}]}) /goto :tNextDebuff
      /if (${Spawn[${ADMobList[${a}]}].Type.Equal[Corpse]}) { 
         /if (${Target.ID}==${Spawn[${ADMobList[${a}]}].ID} && ${Me.Casting.ID} && ${Spell[${Me.Casting.ID}].SpellType.Equal[detrimental]}) /interrupt
         /return
      }
      /if (${ADTarType[${b}]}>9) {
         /if (${ADMDL[${a},${b}]}>=${tADTime}) /return
         /if (${ADTarType[${b}]}==11) {
            /if (${ADMobList[${a}]}!=${ACMATarget}) /return
         } else /if (${ADTarType[${b}]}==12) {
            /if (${ADMobList[${a}]}==${ACMATarget}) /return
         } else /if (${ADTarType[${b}]}==13) {
            /if (${ADMobList[${a}]}!=${ACMATarget}) /return
            /if (!${Spawn[${ACMATarget}].Named} && (!${NamedID} || (${NamedID} && ${Spawn[${ACMATarget}].ID}!=${NamedID}))) /return
         }
      }
      /if (${ADCondition[${b}].Length}>4) {
         /call IniCommand "${ADCondition[${b}]}"
         /if (${Macro.Return.NotEqual[TRUE]}) /goto :tWhatNext
      }
      /if (${ADHold.Arg[1,|]}) {
         /if (!${ADHoldTimer} && ${b}==${ADHold.Arg[2,|]}) {
            /if (${Spawn[${ADMobList[${a}]}].ID} && ${ADMDL[${a},${b}]}<=${Macro.RunTime}+${ADHold.Arg[3,|]} && ${ADMDL[${a},${b}]}>=${Macro.RunTime} && ${ADTarType[${b}]}==12 && ${ADMobList[${a}]}!=${ACMATarget}) /varset ADHoldTimer ${Math.Calc[${ADHold.Arg[3,|]}+.5]}s
         }
      }
      /if (${ADTarType[${b}]}>99) {
         /if (!ADOrigDelay[${b}] && ${ADSpellDelay[${b}]} && ${ADSpellDelay[${b}]}<=${Macro.RunTime}) {
            /echo AE Reset 202
            /varcalc ADTarType[${b}] ${ADTarType[${b}]}-100
            /varset ADSpellDelay[${b}] 0
         }
      }
      /if (${ADTarCnt[${b}]}>1 && ${ADMobCount}<${ADTarCnt[${b}]}) /goto :tNextDebuff 
      /if (${ADTarType[${b}]}<10) {
         /if (${ADTarType[${b}]}==1) {
            /if (${ADMobList[${a}]}!=${ACMATarget}) /goto :tNextDebuff
         } else /if (${ADTarType[${b}]}==2) {
            /if (${ADMobList[${a}]}==${ACMATarget}) /goto :tNextDebuff
         } else /if (${ADTarType[${b}]}==3) {
            /if (${ADMobList[${a}]}!=${ACMATarget}) /goto :tNextDebuff
            /if (!${Spawn[${ACMATarget}].Named} && (!${NamedID} || (${NamedID} && ${Spawn[${ACMATarget}].ID}!=${NamedID}))) /goto :tNextDebuff
         }
      }
      /if (${ADMDL[${a},${b}]}>=${tADTime} || ${Spawn[${ADMobList[${a}]}].PctHPs}>${ADTarBegHP[${b}]} || ${Spawn[${ADMobList[${a}]}].PctHPs}<${ADTarEndHP[${b}]} || (${NetBots[${TankName}].TargetID}==${ADMobList[${a}]} && ${NetBots[${TankName}].TargetHP}<${ADTarEndHP[${b}]}) || ${ADMobList[${a}]}==${Me.Pet.ID}) /goto :tNextDebuff
      /if (${ADUseHoTT[${b}]}==101) {
         /if (!${Spawn[id ${ADMobList[${a}]} xtarhater].ID}) /goto :tNextDebuff
      }
      /varset ADMDL[${a},${b}] ${tADTime}
      /varset tGem 0
      /if (${Select[${ADGem[${b}]},1,2,3,4,5,6,7,8,9,10,11,12,13]}) {
|/echo was Spell
         /if (${Me.CurrentMana}+20<${Me.Book[${Me.Book[${ADSpell[${b}]}]}].Mana} || ${Me.PctMana}<${ADSpellMinMana[${b}]}) /goto :tWhatNext
         /varset tSpellID ${Me.Book[${Me.Book[${ADSpell[${b}]}]}].ID}
         /varset tGem ${ADGem[${b}]} 
         /varcalc tCT ${Spell[${tSpellID}].MyCastTime}/100+50
         /if (${Spell[${tSpellID}].SpellType.Find[Beneficial]} && ${Spell[${tSpellID}].Duration} && ${Me.Buff[${Spell[${tSpellID}].Name}].ID}) /goto :tWhatNext
         /if (${Me.Gem[${tGem}].ID}!=${tSpellID}) {
            /delay 2s ${Cast.Status.Equal[i]}
            /squelch /memorize "${Spell[${tSpellID}]}|gem${tGem}"
            /delay 5s ${Cast.Status.Equal[i]}
            /if (${Me.Gem[${tGem}].ID}!=${tSpellID}) {
               /bc DEBUFF - ${ADSpell[${b}]} - FAILED MEMORIZE
               /goto :tWhatNext
            }
            /varcalc tCT ${Spell[${tSpellID}].RecastTime}/100+${tCT}
         }
         /if (${Me.Class.ShortName.Equal[BRD]} && ${Select[${ADTarType[${b}]},1,11]}) {
            /if (${BardCombatGems.Find[${tGem}]}) /goto :tWhatNext
            /varset BardCombatGems ${BardCombatGems} ${tGem}
            /goto :tWhatNext
         }
      } else /if ( ${ADGem[${b}].Find[item]} && ${Bool[${FindItem[=${ADSpell[${b}]}]}]} ) {
|/echo was Item
         /if (${FindItem[=${ADSpell[${b}]}].Timer} ) /goto :tWhatNext
         /varset tSpellID ${FindItem[=${ADSpell[${b}]}].Spell.ID}
         /varset tGem 20 
         /varcalc tCT ${FindItem[=${ADSpell[${b}]}].CastTime}*10+40
      } else /if (${ADGem[${b}].Equal[alt]}) {
|/echo was Alt
         /if (${Me.CurrentMana}<${Me.AltAbility[${ADSpell[${b}]}].Spell.Mana}) /goto :tNextDebuff
         /if (${Me.Shrouded}) /delay 3s ${Me.AltAbilityReady[${ADSpell[${b}]}]}
         /if (!${Me.AltAbilityReady[${ADSpell[${b}]}]}) /goto :tNextDebuff
         /varset tSpellID ${Me.AltAbility[${ADSpell[${b}]}].Spell.ID} 
         /varset tGem 30 
         /varcalc tCT ${Me.AltAbility[${ADSpell[${b}]}].Spell.CastTime}/100+40
      } else /if (${ADGem[${b}].Equal[script]}) {
|/echo was Script
         /call MBScript ${ADSpell[${b}]}
         /varset tSpellDelay 0
         /varset tGem 40
         /varset ADtTargetID ${Target.ID}
         /varset tSpellID 0
         /goto :tContDebuff
      } else /if (${ADGem[${b}].Equal[ability]}) {
|/echo was Ability
         /if (${Target.ID}!=${ADMobList[${a}]}) /squelch /tar id ${ADMobList[${a}]}
         /delay 1s ${Target.ID}==${ADMobList[${a}]}
         /if (${Target.ID}!=${ADMobList[${a}]}) /goto :tWhatNext
         /doability ${ADSpell[${b}]}
         /varset tSpellDelay 0
         /varset tGem 50
         /varset ADtTargetID ${Target.ID}
         /varset tSpellID 0
         /goto :tContDebuff
      } else /if (${ADGem[${b}].Equal[disc]}) {
|/echo was disc
         /if (${Me.PctEndurance}<${ADSpellMinMana[${b}]} || ${ADDiscTimer[${b}]}>${Macro.RunTime} || !${Me.CombatAbilityReady[${ADSpell[${b}]}]}) /goto :tNextDebuff
         /varset ADDiscTimer[${b}] ${Macro.RunTime}+${ADSpellRecast[${b}]}
         /if (${Target.ID}!=${ADMobList[${a}]}) /squelch /tar id ${ADMobList[${a}]}
         /delay 1s ${Target.ID}==${ADMobList[${a}]}
         /if (${Target.ID}!=${ADMobList[${a}]}) /goto :tWhatNext
         /disc ${Spell[${ADSpell[${b}]}].ID}
         /varset tSpellDelay 0
         /varset tGem 50
         /varset ADtTargetID ${Target.ID}
         /varset tSpellID 0
         /goto :tContDebuff
      }
      /if (!${tGem} || ${Spawn[${ADMobList[${a}]}].Type.Equal[corpse]}) /goto :tWhatNext 
      /if (${Spell[${tSpellID}].Range}) {
         /if (${Spawn[${ADMobList[${a}]}].Distance}>${Spell[${tSpellID}].MyRange}) /goto :tWhatNext
      }
      /if (${Defined[MaxLevel${Spell[${ADSpell[${b}]}].ID}]}) {
         /if (${Spawn[${ADMobList[${a}]}].Level}>${MaxLevel${Spell[${ADSpell[${b}]}].ID}}) /goto :tWhatNext
      }
      /if (!${Defined[Imm${Spell[${ADSpell[${b}]}].ID}]}) /declare Imm${Spell[${ADSpell[${b}]}].ID} string outer
| /echo Imm list ${Imm${Spell[${ADSpell[${b}]}].ID}}
      /if (${Imm${Spell[${ADSpell[${b}]}].ID}.Find[|${Spawn[${ADMobList[${a}]}].CleanName}|]} && !${Imm${Spell[${ADSpell[${b}]}].ID}.Find[|${Spawn[${ADMobList[${a}]}].CleanName}*|]}) {
         /if (${ADIfSpellImmune[${b}].Length}>3) {
            /target id ${ADMobList[${a}]}
            /delay 1s ${Target.ID}==${ADMobList[${a}]}
            /call IniCommand "${ADIfSpellImmune[${b}]}" ${ADMobList[${a}]}
         }
         /goto :tWhatNext
      }
      /if (${Spell[${tSpellID}].AERange} && !${Select[${Spell[${tSpellID}].TargetType},PB AE,single]} && !${Spell[${tSpellID}].SpellType.Find[Beneficial]} && !${Spell[${tSpellID}].CastOnYou.Find[drowsy]}) {
         /if (!${Spell[${tSpellID}].Range}) {
            /if (${Spawn[${ADMobList[${a}]}].Distance}>${Spell[${tSpellID}].AERange}) /goto :tWhatNext
         } else {
            /if (${Spawn[${ADMobList[${a}]}].Distance}<${Spell[${tSpellID}].AERange} && !${Spell[${tSpellID}].Name.Find[Explosion of Hatred]}) /goto :tWhatNext
         }
      }
      /if (!${ADUseHoTT[${b}]} || ${ADUseHoTT[${b}]}==101) {
         /varset HoTTTarget TRUE
      } else { 
         /if (${Target.ID}!=${ADMobList[${a}]}) {
           /squelch /tar id ${ADMobList[${a}]}
           /delay 3s ${Target.ID}==${ADMobList[${a}]}
| Have to wait for ToT to update on new target.
           /delay 1s !${Me.TargetOfTarget.ID}
         }
         /delay 2s ${Me.TargetOfTarget.ID} 
         /if (${Target.ID} && (${Group.Member[${Me.TargetOfTarget.Name}].Index} || ${Spawn[raid id ${Me.TargetOfTarget.ID}].ID} || ${Me.TargetOfTarget.ID}==${Me.ID} || ${NetBots[${Me.TargetOfTarget.Name}].ID})) /varset HoTTTarget TRUE
         /if (!${Me.TargetOfTarget.ID}) /varset HoTTTarget FALSE
         /if (!${HoTTTarget}) {
            /echo Skipping mob - ${Spawn[${ADMobList[${a}]}].CleanName}
            /varset tGem 60
            /varset tSpellDelay ${ADUseHoTT[${b}]}
            /goto :tContDebuff
         }
      }
      /varset ADtTargetID 0
      /varset tRecast 0
      /varset tSpellDelay 0
      /varset tCastTime ${tCT}
      /varset CastResult UNKNOWN
      /varset d 0
      /if (${PetCast.Find[charm ]} && ${DoPet}) {
         /if (!${Me.Pet.ID} && ${ADCharmID} && ${tSpellID}!=${Spell[${ADCharmSpell}].ID}) {
            /interrupt
            /if (${Me.Class.ShortName.Equal[BRD]}) /squelch /stoptwist
            /goto :tCheckforAdds
         }
      }
:tCastDebuff
      /if (${NetBots[${Me.CleanName}].Silenced} || ${NetBots[${Me.CleanName}].Stunned} || ${Window[RespawnWnd].Open}) /return
      /if (${Me.Class.ShortName.NotEqual[BRD]} && ${ADTarType[${b}]}<10 && ${Cast.Ready} && !${Cast.Ready[${tSpellID}|${ADGem[${b}]}]} && ${Select[${ADGem[${b}]},1,2,3,4,5,6,7,8,9,10,11,12,13]}) {
         /if (${Me.GemTimer[${ADGem[${b}]}]}>5000 || (${Me.GemTimer[${ADGem[${b}]}]}>3000 && !${Me.SpellInCooldown})) /goto :tNextDebuff
      }
      /if (${DoHeals} && ${AHCount}) {
         /call AHHealCheck
|        /if (${AHFlag}) {
|          /if (${Me.Shrouded}) /delay 15
|          /varset tCastTime ${tCT}
|          /varset ADtTargetID 0
|        }
      }
      /if (${Target.ID}==${ACMATarget} && ${DoMelee} && ${Target.PctHPs}<=${ACAssistPct}) {
         /if (${Me.Pet.ID} && ${PetAssist}==1) {
            /pet attack
            /varset PetAssist 2
         }
         /pet swarm
      }
      /doevents eqbc
      /if (${Spawn[${ADMobList[${a}]}].Type.Equal[corpse]}) /return
      /if (${ADtTargetID}) {
         /if (${Spawn[${ADtTargetID}].Type.Equal[Corpse]} || !${Spawn[${ADtTargetID}].ID} || !${Me.Class.ShortName.Equal[brd]} && ${NetBots[${TankName}].ID} && ${Target.ID}==${NetBots[${TankName}].TargetID} && (${ADTarType[${b}]}==2 || ${ADTarType[${b}]}==12)) {
            /interrupt
            /return
         }
         /if (${PetCast.Find[charm ]} && ${DoPet}) { 
            /if (!${Me.Pet.ID} && ${ADCharmID} && ${tSpellID}!=${Spell[${ADCharmSpell}].ID}) {
               /interrupt
               /if (${Me.Class.ShortName.Equal[BRD]}) /squelch /stoptwist
               /goto :tCheckforAdds
            }
         }
         /if (${Me.Class.ShortName.Equal[BRD]}) {
            /doevents CastRst
            /doevents CastStn
            /doevents CastImm
            /if (!${CastResult.Equal[pending]}) {
               /if (${Select[${CastResult},success]}) /goto :tContDebuff
               /if (${Select[${CastResult},immune]}) {
                  /bc ${Spawn[${ADMobList[${a}]}].CleanName} [+r+]IMMUNE[+x+] to ${ADSpell[${b}]}
                  /if (${ADIfSpellImmune[${b}].Length}>3) /call IniCommand "${ADIfSpellImmune[${b}]}" ${ADMobList[${a}]}
	          /doevents MezResist
                  /if (!${Imm${Spell[${ADSpell[${b}]}].ID}.Find[|${Spawn[${ADtTargetID}].CleanName}|]} && !${Imm${Spell[${ADSpell[${b}]}].ID}.Find[|${Spawn[${ADtTargetID}].CleanName}*|]} && ${EventSpell.Find[${Cast.Stored.Name}]}) {
                     /bc Adding ${Spawn[${ADtTargetID}].CleanName} to the immune list.
                     /varset Imm${Spell[${ADSpell[${b}]}].ID} ${Imm${Spell[${ADSpell[${b}]}].ID}}|${Spawn[${ADtTargetID}].CleanName}|
                     /ini "${MBCommon}" "${Spell[${ADSpell[${b}]}].Name}" "${Zone.ShortName}Immune" "${Imm${Spell[${ADSpell[${b}]}].ID}}"
                  }
                  /varset tSpellDelay 4000
                  /goto :tContDebuff
               }
               /if (${Select[${ADTarType[${b}]},2,12]} && ${Select[${CastResult},resisted,stunned,reset]} && ${ADtTargetID}==${ACMATarget}) /goto :tCheckforAdds
               /if (${Select[${CastResult},resisted,stunned,reset]} && (${ADSpellRecast[${b}]} && ${ADSpellRecast[${b}]}>=${tRecast} || !${ADSpellRecast[${b}]})) {
                  /varcalc tRecast ${tRecast}+1
                  /varset ADtTargetID 0
                  /if (${tRecast}<${ADSpellRecast[${b}]}) /goto :tCastDebuff
               }
               /goto :tCheckforAdds
            }
            /delay 1s
            /if (${DoMelee} && ${ACState} && ${ACMATarget} && ${Target.ID}!=${ACMATarget}) {
               /if (${Target.ID}) /target clear
               /tar id ${ACMATarget}
               /delay 5
               /squelch /stick off
               /killthis
            }
            /if (!${tCastTime} && !${Cast.Timing}) /varset CastResult SUCCESS
            /goto :tCastDebuff
         } else /if (${Cast.Status.Equal[i]}) {
            /if (${Select[${Cast.Result},cast_recover]}) {
               /varset tCastTime ${tCT}+30
               /varset ADtTargetID 0
               /delay 5
               /goto :tCastDebuff
            }
            /if (${Spell[${ADSpell[${b}]}].TargetType.Find[ ae]} && !${Select[${Cast.Result},cast_fizzle]} && !${ADSpellRecast[${b}]}) {
               /varcalc ADTarType[${b}] ${ADTarType[${b}]}+100
               /if (${Me.Book[${Me.Book[${ADSpell[${b}]}]}].Duration}>2 && !${ADSpellDelay[${b}]} && ${ADTarCnt[${b}]}>1) /varcalc ADSpellDelay[${b}] ${Macro.RunTime}+${Math.Calc[${Me.Book[${Me.Book[${ADSpell[${b}]}]}].Duration.TotalSeconds}*10]}
               /goto :tContDebuff
            }
            /if (${Select[${Cast.Result},cast_immune]}) {
               /for e 1 to 5
                  /bc ${Spawn[${ADMobList[${a}]}].CleanName} [+r+]IMMUNE[+x+] to ${ADSpell[${b}]}
               /next e
               /docommand /${BeepCommand}
               /if (${ADIfSpellImmune[${b}].Length}>3) /call IniCommand "${ADIfSpellImmune[${b}]}" ${ADMobList[${a}]}
               /doevents MezResist
               /if (!${Imm${Spell[${ADSpell[${b}]}].ID}.Find[|${Spawn[${ADtTargetID}].CleanName}|]} && !${Imm${Spell[${ADSpell[${b}]}].ID}.Find[|${Spawn[${ADtTargetID}].CleanName}*|]} && ${EventSpell.Find[${Cast.Stored.Name}]}) {
                  /bc Adding ${Spawn[${ADtTargetID}].CleanName} to the immune list.
                  /varset Imm${Spell[${ADSpell[${b}]}].ID} ${Imm${Spell[${ADSpell[${b}]}].ID}}|${Spawn[${ADtTargetID}].CleanName}|
                  /ini "${MBCommon}" "${Spell[${ADSpell[${b}]}].Name}" "${Zone.ShortName}Immune" "${Imm${Spell[${ADSpell[${b}]}].ID}}"
               }
               /varset tSpellDelay 4000
               /goto :tContDebuff
            }
            /if (${Select[${Cast.Result},cast_takehold]}) { 
               /echo ${ADSpell[${b}]} did not take hold
               /varset tSpellDelay 4000
               /varset tGem 60
               /goto :tContDebuff
            }
            /if (${ADSpellRecast[${b}]} && ${ADSpellRecast[${b}]}>=${tRecast} && ${Select[${Cast.Result},cast_resist,cast_fizzle,cast_interrupted]}) {
               /if (${Cast.Result.Equal[cast_resist]} && ${ADSpellCastonResist[${b}].Length} && ${tCastonRes}<1) {
| /echo Casting additional spell - ${ADSpellCastonResist[${b}]} 
                  /call CastCall ${Me.CleanName} "coresist ${ADSpellCastonResist[${b}]}"
| /echo Additional spell - Macro.Return: ${Macro.Return} : 1:${Macro.Return.Arg[1]} - 2:${Macro.Return.Arg[2]} - 3:${Macro.Return.Arg[3]} - 4:${Macro.Return.Arg[4]} - 5:${Macro.Return.Arg[5]}
                  /if (${Macro.Return.Arg[1].Equal[cast_success]} && ${Macro.Return.Arg[2].Equal[ad]} && ${Macro.Return.Arg[5]}==${ADMobList[${a}]}) {
                     /if (${Spell[${Macro.Return.Arg[4]}].Duration}) {
                        /varset ADMDL[${a},${Macro.Return.Arg[3]}] ${Math.Calc[${Macro.RunTime}+${Spell[${Macro.Return.Arg[4]}].Duration.TotalSeconds}-5]}
                     } else {
                        /varset ADMDL[${a},${Macro.Return.Arg[3]}] ${Macro.RunTime}
                     }
                     /if (${ADSameSpell[${Macro.Return.Arg[3]}]}) /varset ADMDL[${a},${ADSameSpell[${Macro.Return.Arg[3]}]}] ${ADMDL[${a},${Macro.Return.Arg[3]}]}
                  }
                  /varcalc tCastonRes ${tCastonRes}+1
               }
               /delay 2s ${Cast.Status.Equal[i]}
               /varcalc tRecast ${tRecast}+1
               /varset tCastTime ${tCT}+30
               /varset ADtTargetID 0
               /goto :tCastDebuff
            } else /if (${ADSpellRecast[${b}]} && ${Select[${Cast.Result},cast_resist,cast_fizzle,cast_interrupted]} && ${tRecast}>=${ADSpellRecast[${b}]}) {
               /varset tSpellDelay 4000
               /goto :tContDebuff
            }
            /if (${Select[${Cast.Result},cast_outofrange,cast_cannotsee,cast_distracted,cast_notarget,CAST_CANCELLED,CAST_NOTREADY]} || !${ADSpellRecast[${b}]} && ${Select[${Cast.Result},cast_resist,cast_fizzle,cast_interrupted]}) /goto :tCheckforAdds
            /if (${Select[${Cast.Result},cast_outdoors]}) {
               /bc Outdoor only spell, turning off debuff [ ${ADSpell[${b}]} ]
               /varset ADTarCnt[${b}] 0
               /goto :tNextDebuff
            }
            /if (${Cast.Result.Equal[cast_success]}) /goto :tContDebuff
         }
      } else {
         /if (!${Cast.Ready[${ADSpell[${b}]}|${ADGem[${b}]}]} && !${Me.Class.ShortName.Equal[BRD]} && ${Spawn[${ADMobList[${a}]}].Type.NotEqual[corpse]}) /delay 2s ${Cast.Ready[${ADSpell[${b}]}|${ADGem[${b}]}]}
         /if (${Cast.Ready[${ADSpell[${b}]}|${ADGem[${b}]}]} || ${Me.Class.ShortName.Equal[BRD]} && ${Spawn[${ADMobList[${a}]}].Type.NotEqual[corpse]}) {
            /if (${Target.ID}!=${ADMobList[${a}]}) {
               /if (${Me.Class.ShortName.Equal[BRD]}) /squelch /stoptwist
               /if (${Melee.Combat}) /attack off
               /squelch /tar id ${ADMobList[${a}]}
               /delay 1s ${Target.ID}==${ADMobList[${a}]}
            }
            /varset ADtTargetID ${ADMobList[${a}]}
            /varset tCastTime ${tCT}
            /if (${Me.Class.ShortName.Equal[BRD]}) {
               /if (${tGem}==30 && ${Me.AltAbility[${ADSpell[${b}]}].Spell.CastTime.Float}<1) {
                  /alt act ${Me.AltAbility[${ADSpell[${b}]}].ID}
                  /delay 2
                  /varset tGem 41
                  /goto :tContDebuff
               } else {
                  /squelch /twist once ${ADGem[${b}]}
                  /varset CastResult PENDING
                  /varset tCastTime 32
               }
            } else {
               /varset MBCurrentSpell AD|${b}|${ADtTargetID}
               /call mq2cast "${ADSpell[${b}]}" ${If[${Select[${ADGem[${b}]},1,2,3,4,5,6,7,8,9,10,11,12,13]},gem,]}${ADGem[${b}]} ${tCastTime} CastCheck -targetid|${ADtTargetID} -bandolier|${ADSpellFoci[${b}]} 
            }
         }
      }
      /if (${ADtTargetID} && ${Target.ID}==${ADtTargetID} && ${Me.Casting.ID} && ${Spell[${Me.Casting.ID}].SpellType.Equal[detrimental]} && (${Spawn[${ADtTargetID}].Type.Equal[Corpse]} || !${Spawn[${ADtTargetID}].ID})) {
         /interrupt
         /return
      }
      /if (${interruptFlag}) /goto :tCheckforAdds
      /if (${tCastTime} || ${Cast.Timing}) /goto :tCastDebuff
      /if (${AHHealList.Find[${Cast.Stored}]} && ${Cast.Status.Equal[I]} && ${ADtTargetID} && ${Spawn[${ADtTargetID}].ID} && ${Spawn[${ADtTargetID}].Type.NotEqual[Corpse]}) { 
         /varset ADtTargetID 0
         /goto :tCastDebuff
      }
|     /echo Cast took too long, aborting this cast ${Spell[${tSpellID}].Name}
|     /echo Cast.Stored ${Cast.Stored} - Cast.Status ${Cast.Status} - Cast.Result ${Cast.Result} - TargetID ${ADtTargetID}
      /goto :tCheckforAdds
:tContDebuff
      /varset MiscCheckTimer 5s
      /if (${HoTTTarget} && ${tGem}<60 && ${ADAnnounce[${b}].Length} && (!${Select[${Cast.Result},cast_immune,cast_takehold]} && !${CastResult.Equal[immune]} || ${ADTarCnt}<=1)) {
         /if (${ADAnnounce[${b}].Length}>10 && ${tGem}<60) {
            /call ParseAnnounce "${ADAnnounce[${b}]}" "${Spawn[${ADtTargetID}].CleanName}" "${ADSpell[${b}]}"
         } else {
            /docommand ${ADAnnounce[${b}]} Debuffed: -[ ${Spawn[${ADtTargetID}].CleanName} ]- with ${ADSpell[${b}]}
         }
      }
      /if (${Spell[${tSpellID}].Duration} && ${tGem}<40 || ${tGem}==60) {
         /if (${ADDurMod[${b}]}) {
            /varcalc tSpellDuration ((${ADDurMod[${b}]}/100)*${Spell[${tSpellID}].Duration.TotalSeconds})+${Spell[${tSpellID}].Duration.TotalSeconds}-${Spell[${tSpellID}].MyCastTime.TotalSeconds}-5
         } else {
            /varcalc tSpellDuration ${Spell[${tSpellID}].Duration.TotalSeconds}-${Spell[${tSpellID}].MyCastTime.TotalSeconds}-5
         }
         /varcalc tSpellDelay ${tSpellDelay}+${ADSpellDelay[${b}]}+${tSpellDuration}+${Macro.RunTime}
      } else {
         /varcalc tSpellDelay ${tSpellDelay}+${ADSpellDelay[${b}]}+${Macro.RunTime}-${Spell[${tSpellID}].MyCastTime.TotalSeconds}-5
      }
      /if ((!${tSpellDelay} || ${tSpellDelay}<${ADSpellDelay[${b}]}) && ${ADSpellDelay[${b}]}) /varcalc tSpellDelay ${ADSpellDelay[${b}]}+${Macro.RunTime}
      /if (${Spell[${tSpellID}].RecastTime.TotalSeconds} && (${tSpellDelay}<=${Macro.RunTime} || ${Math.Calc[${tSpellDelay}-${Macro.RunTime}]}<${Spell[${tSpellID}].RecastTime.TotalSeconds})) /varset tSpellDelay ${Spell[${tSpellID}].RecastTime.TotalSeconds}
      /varcalc ADMDL[${a},${b}] ${tSpellDelay}
      /if (${ADSameSpell[${b}]}) /varset ADMDL[${a},${ADSameSpell[${b}]}] ${ADMDL[${a},${b}]}
      /if (${PetCast.Find[charm ]} && ${DoPet}) {
         /if (${Me.Pet.ID} && ${ADGem[${b}]}==${ADCharmGem}) {
            /varset ADCharmID ${ADMobList[${a}]}
            /varset PetAssist 1
            /echo Charmed a pet ${ADMobList[${a}]}
         }
      }
      /if (${tGem}<40 && ${Me.Class.ShortName.Equal[BRD]}) /squelch /stoptwist
      /varset MBCurrentSpell
      /doevents
:tCheckforAdds
      /call ADSpawnCheck
      /if (!${ADMobCount}) /return
      /if (${AddSpawn}) {
         /varset tADAddDetected TRUE
         /return
      }
:tWhatNext
      /if (${Me.Pet.ID}) {
         /if (${DoMelee} && !${ACState} && ${Spawn[${NetBots[${TankName}].TargetID}].PctHPs}<${ACAssistPct} && ${NetBots[${TankName}].TargetID}) /call AdvCombat
      }
      /if (${ADTarType[${b}]}>9 && ${ADTarType[${b}]}<99 && ${ADTarCnt[${b}]}>0) /return
:tNextDebuff
   /next b
/return


Sub Event_MezResist(string Line,string SpellName)
   /if (!${Defined[EventSpell]}) /declare EventSpell string outer
   /varset EventSpell ${SpellName}
|  /echo MezResist: ${Line} - ${EventSpell}
/return

