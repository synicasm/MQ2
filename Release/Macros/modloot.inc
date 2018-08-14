
#Event ALCantLoot         "#*#may not loot this corpse#*#"
#Event ALCantLoot         "#*#cannot loot this Lore Item#*#"

Sub SetupAdvLootVars
 /declare x int local
 /declare ALDontLoot        int    outer
 /declare CorpseRotTimer    timer  outer
 /declare CorpseList        string outer
 /declare ALDontLootTimer  timer outer
 /declare RelootTimer timer outer
 /call LoadINIVar Settings LootIni "Loot.ini" LootIni "${MBCommon}" string
 /call LoadINIVar Settings LootMobs TRUE LootMobs "${LootIni}" bool
 /call LoadINIVar Settings CorpseRadius 100 CorpseRadius "${LootIni}" int
 /call LoadINIVar Settings MobsTooClose 50 MobsTooClose "${LootIni}" int
 /call LoadINIVar Settings CorpseRotTime 440s CorpseRotTime "${LootIni}" string
 /call LoadINIVar Settings ReportLoot FALSE ReportLoot "${LootIni}" bool
 /call LoadINIVar Settings LootChannel bc LootChannel "${LootIni}" string
 /call LoadINIVar Settings Category "Tradeskill,Spellscroll" Category "${LootIni}" string
 /call LoadINIVar Settings LootChannel bc LootChannel "${LootIni}" string
 /call LoadINIVar Settings QuestOnly "FALSE" QuestOnly "${LootIni}" bool
 /for x 1 to ${Math.Calc[${Category.Count[,]}+1]}
   /call LoadINIVar Settings ${Category.Arg[${x},,]} "Toon1Name,Toon2Name" ${Category.Arg[${x},,]} "${LootIni}" string
|  /echo Category arg ${x} : ${Category.Arg[${x},,]}
 /next x
 /call LoadINIVar Settings KeepScript " " KeepScript "${LootIni}" string
 /squelch /alert clear 25
/return


sub CloseCorpse
 /declare corpselock timer local 10s
 /nomodkey /notify LootWnd LW_DoneButton leftmouseup
 /delay 5 !${Corpse.Open}	
 /if (!${corpselock}) /keypress esc
 /if (${Corpse.Open}) {
:clickdoneffs
   /nomodkey /notify LootWnd LW_DoneButton leftmouseup
   /delay 5 !${Corpse.Open}
   /if (${Corpse.Open}) /goto :clickdoneffs
 }
 /if (${Corpse.Open}) /keypress esc
/return


Sub LootCorpse
 /echo LootCorpse
 /declare i int local
 /declare y int local
 /declare LootList    string local
 /declare CurrentItem string local
 /declare CurrIni     string local
 /declare x int local 0
 /declare z int local 0
 /declare corpselock timer local
 /declare CanLoot bool local FALSE
 /declare loottotal  int    local
:ALauto_inv
 /if (${Cursor.ID}) /call ClearCursor
 /if (!${Target.ID}) /return
 /doevents flush
 /loot
 /delay 1s ${Corpse.Open}
 /doevents ALCantLoot
 /if (${Target.ID} && ${Target.ID}==${ALDontLoot} && ${Spawn[${ALDontLoot}].ID} && ${ALDontLoot}) {
   /squelch /alert add 25 id ${ALDontLoot}
   /call CloseCorpse
   /return
 }
 /if (!${Corpse.Open} && ${Target.ID}!=${ALDontLoot} && ${Target.ID}) /goto :ALauto_inv
 /delay 3s ${Corpse.Items} || ${Target.ID}==${ALDontLoot}
 /if (${Corpse.Open} && ${Corpse.Items}) {
   /varset corpselock 5s
:ALlootlag
   /varset loottotal ${Corpse.Items}
   /delay 1s ${loottotal}!=${Corpse.Items}
   /if (${loottotal}!=${Corpse.Items} & !${corpselock}) /goto :ALlootlag
   /for i 1 to ${loottotal}
     /if (!${Corpse.Item[${i}].ID}) /next i
     /echo Item - ${i} - ${Corpse.Item[${i}]}
     /multiline ; /varset CanLoot FALSE;/varset CurrentItem ${If[${Corpse.Item[${i}].Name.Find[,]},${Corpse.Item[${i}].Name.Arg[1,,]}${Corpse.Item[${i}].Name.Arg[2,,]},${Corpse.Item[${i}].Name}]}
     /varset CurrIni ${Ini["${LootIni}","${Corpse.Item[${i}].Name.Left[1]}","${CurrentItem}"]}
     /if (${CurrIni.Equal[Destroy]}) {
       /if (!${QuestOnly}) /call LootItem ${i} Destroy left
       /next i
     }
     /if (${CurrIni.Left[5].Equal[Keep|]} && ${FindItemCount[${Corpse.Item[${i}]}]}>=${CurrIni.Arg[2,|]}) /multiline ; /varset LootList ${LootList}${Corpse.Item[${i}]},;/next i
     /if (${Corpse.Item[${i}].ID} && !${Select[${CurrIni},Ignore,Keep,Destroy,Sell,Buy,${Me.Name}]} && !${CurrIni.Find[Quest|]} && !${Select[${CurrIni},${Category}]}) {
       /if (${Corpse.Item[${i}].NoDrop}) /ini "${LootIni}" "${Corpse.Item[${i}].Name.Left[1]}" "${CurrentItem}" Ignore
       /if (!${Corpse.Item[${i}].NoDrop}) /ini "${LootIni}" "${Corpse.Item[${i}].Name.Left[1]}" "${CurrentItem}" Keep
       /varset CurrIni ${Ini["${LootIni}","${Corpse.Item[${i}].Name.Left[1]}","${CurrentItem}"]}
     }
     /if ((${Corpse.Item[${i}].Lore} && !${FindItem[${Corpse.Item[${i}]}].ID} && !${FindItemBank[${Corpse.Item[${i}]}].ID} || !${Corpse.Item[${i}].Lore}) && (${Me.FreeInventory} || (${FindItemCount[=${Corpse.Item[${i}].Name}]} && ${Corpse.Item[${i}].Stackable} && ${Corpse.Item[${i}].FreeStack})) && (${CurrIni.Equal[Keep]} || ${CurrIni.Equal[Sell]} || ${CurrIni.Find[${Me.Name}]} || (${Select[${CurrIni},${Category}]} && !${CurrIni.Find[Quest|]}))) {
       /if (!${QuestOnly} && (${CurrIni.Equal[Sell]} || ${CurrIni.Equal[Keep]} || ${CurrIni.Left[5].Equal[Keep|]} && ${FindItemCount[${Corpse.Item[${i}]}]}<${CurrIni.Arg[2,|]})) /multiline ; /varset CanLoot TRUE;/goto :doLoot
       /for z 1 to ${Math.Calc[${CurrIni.Count[|]} + 1]}
	 /if (${CurrIni.Arg[${z},|].Equal[${Me.Name}]} || ${CurrIni.Arg[${z},|].Equal[Keep]}) /multiline ; /varset CanLoot TRUE;/goto :doLoot
	 /if (${Select[${CurrIni.Arg[${z},|]},${Category}]} && !${CurrIni.Find[Quest|]}) {
	   /if (!${${CurrIni.Arg[${z},|]}.Find[${Me.Name}]} && !${${CurrIni.Arg[${z},|]}.Find[${Me.Class.ShortName}]} && !${Corpse.Item[${i}].NoDrop}) {
 	     /for y 1 to ${Math.Calc[${${CurrIni.Arg[${z},|]}.Count[,]}+1]}
| this loop is just here to loot the item if no-one matching the catagory params in in group.  If toon not here, loot, otherwise check quests.
| /echo LootCheck:  Catagory: ${CurrIni.Arg[${z},|]}  Spawncheck for: ${${CurrIni.Arg[${z},|]}.Arg[${y}, ]}
               /if (${Spawn[pc group ${${CurrIni.Arg[${z},|]}.Arg[${y}, ]}].ID}) /goto :tContinue
    	     /next y
    	     /if (!${QuestOnly}) /multiline ; /varset CanLoot TRUE;/goto :doLoot
    	   } else /if (${${CurrIni.Arg[${z},|]}.Find[${Me.Name}]} || ${${CurrIni.Arg[${z},|]}.Find[${Me.Class.ShortName}]}) {
    	     /if (!${QuestOnly}) /multiline ; /varset CanLoot TRUE;/goto :doLoot
    	   } else /if (!${Select[${CurrIni},${Category}]} && !${CurrIni.Find[Quest|]} && !${QuestOnly}) {
    	     /docommand ${If[${KeepScript.Length} && ${CurrIni.Find[Sell]} || ${CurrIni.Find[Script]},/call MBScript ${KeepScript} ${i},/call LootItem ${i} Keep right]}	
	   }
	 }
       /next z
     }
:tContinue
     /if ((${DoQuest} || ${QuestOnly}) && ${CurrIni.Find[Quest|]} && ${FindItemCount[=${Corpse.Item[${i}].Name}]}<${CurrIni.Arg[2,|]} && (${Me.FreeInventory} || (${FindItemCount[=${Corpse.Item[${i}].Name}]} && ${Corpse.Item[${i}].Stackable} && ${Corpse.Item[${i}].FreeStack}))) {
       /if (${CurrIni.Arg[3,|].Length}) {
         /if (!${Window[TaskWnd].Open} && !${Window[TaskWnd].Child[TASK_TaskList].Items}) /keypress alt+q
         /delay 1s ${Window[TaskWnd].Open}
         /for x 1 to ${Window[TaskWnd].Child[TASK_TaskList].Items}
           /if (${Window[TaskWnd].Child[TASK_TaskList].List[${x},2].Equal[${CurrIni.Arg[3,|]}]} ) /multiline ; /varset CanLoot TRUE;/goto :doLoot
         /next x
         /if (${Window[TaskWnd].Open}) /keypress alt+q
       } else {
    	 /if (!${CurrIni.Arg[3,|].Length}) /echo Please specify quest name for ${Corpse.Item[${i}].Name}.  Format for "${LootIni}" is =Quest|<count>|<quest name>|  Looting item.
    	 /varset CanLoot TRUE
       }
     }
:doLoot
| /echo ${CanLoot} : ${Corpse.Item[${i}]}
     /if (${CanLoot}) /call LootItem ${i} Keep right
     /if (${Corpse.Item[${i}].ID}) /varset LootList ${LootList}${Corpse.Item[${i}]},
   /next i
   /if (${Corpse.Items}) {
     /if (${ReportLoot} && ${LootList.Length}) /${LootChannel} [+r+]${LootList} left on corpse.[+x+] ID#: ${Target.ID}
     /if (${Target.ID} && ${Target.Type.Equal[corpse]}) /squelch /alert add 25 id ${Target.ID}
     /if (!${CorpseRotTimer}) /varset CorpseRotTimer ${CorpseRotTime}
   }
 }
 /call CloseCorpse
| 	/echo 142	
/return


Sub LootItem(int i,DoWhat,WhichButton)
| /echo LootItem(int i,DoWhat,WhichButton) - ${i}, ${DoWhat} ${WhichButton}
 /declare CorpseItemID int local ${Corpse.Item[${i}].ID}
| /nomodkey /shift /itemnotify Loot${i} ${WhichButton}mouseup
 /nomodkey /itemnotify Loot${i} ${WhichButton}mouseup
 /delay 2
 /delay 5s ${Window[ConfirmationDialogBox].Open} || !${Corpse.Item[${i}].NoDrop}
 /if (${Window[ConfirmationDialogBox].Open}) /nomodkey /notify ConfirmationDialogBox Yes_Button leftmouseup
 /delay 5s ${Cursor.ID} || ${WhichButton.NotEqual[left]}
 /if (${DoWhat.Equal[Destroy]} && ${Cursor.ID}==${CorpseItemID}) /destroy
 /delay 3s !${Corpse.Item[${i}].ID}
:ALauto_inv
 /if (${Cursor.ID}) {
   /if (${Cursor.NoDrop} && !${Me.FreeInventory}) /return
   /timed 5 /autoinventory
   /delay 2
   /delay 3s !${Cursor.ID}
   /if (!${Cursor.ID}) /goto :CursDone
   /delay 2
   /goto :ALauto_inv
 }
:CursDone
/return


Sub LootMobs
 /if (!${LootMobs} || ${SpawnCount[npc los radius ${MobsTooClose} zradius ${ZRadius} noalert ${AlertList}]}>${SpawnCount[npc untargetable los radius ${MobsTooClose} zradius ${ZRadius} noalert ${AlertList}]} || !${SpawnCount[corpse radius ${CorpseRadius} zradius ${ZRadius} noalert 25]} || ${Me.Combat} || (${Cursor.NoDrop} && !${Me.FreeInventory}) || ${RelootTimer}) /return
 /if (!${CorpseRotTimer}) /squelch /alert clear 25
 /declare i int local ${Math.Rand[20]}
 /varset RelootTimer ${Math.Calc[20 + ${i}]}s
 /varset CorpseList |
 /if (${Select[${MakeCamp},on]} && ${CampStatus}!=2) /return
 /varset i 1
 /for i 1 to ${SpawnCount[corpse radius ${CorpseRadius} zradius ${ZRadius} noalert 25]}
   /varset CorpseList ${CorpseList}${NearestSpawn[${i},corpse radius ${CorpseRadius} noalert 25].ID}|
 /next i
 /declare DeadCount int local ${SpawnCount[corpse radius ${CorpseRadius} zradius ${ZRadius} noalert 25]}
 /if (${DeadCount}) {
   /if (${Me.Sneaking}) /multiline ; /doability sneak;/delay 5
   /if (${FollowFlag}) {
     /declare LMFollow bool local TRUE
     /squelch /stick off
     /if (${FollowFlag} && !${AdvPath.Paused}) {
       /varset FollowFlag 3
       /squelch /afollow pause
       /echo Follow paused.
     }
   } else {
     /declare LMStartX int local ${Me.X}
     /declare LMStartY int local ${Me.Y}
     /declare LMStartHeading ${Me.Heading.DegreesCCW}
   }
 }
 /if (${Me.Mount.ID}) /dismount
 /for i 1 to ${DeadCount}
   /if (${Spawn[${CorpseList.Arg[${i},|]}].Deity.ID} && ${Spawn[${CorpseList.Arg[${i},|]}].ID}) /squelch /alert add 25 id ${Spawn[${CorpseList.Arg[${i},|]}].ID}
   /if (${Spawn[${CorpseList.Arg[${i},|]}].ID} && ${Spawn[${CorpseList.Arg[${i},|]}].LineOfSight} && !${Spawn[${CorpseList.Arg[${i},|]}].Deity.ID}) {
     /if (${Target.ID}!=${Spawn[${CorpseList.Arg[${i},|]}].ID}) /target id ${Spawn[${CorpseList.Arg[${i},|]}].ID}
     /if (!${Me.Standing}) /stand
     /delay 2s ${Target.ID}==${Spawn[${CorpseList.Arg[${i},|]}].ID} && ${Me.Standing}
     /if (${Target.Z}<${Me.Z}) /look -10
     /if (${Spawn[${CorpseList.Arg[${i},|]}].Distance3D}>12) /call MBMoveTo ${Spawn[${CorpseList.Arg[${i},|]}].Y} ${Spawn[${CorpseList.Arg[${i},|]}].X}
     /delay 10s ${Spawn[${CorpseList.Arg[${i},|]}].Distance3D}<12
     /if (${Spawn[${CorpseList.Arg[${i},|]}].Distance3D}<15) /call LootCorpse
     /delay 1
     /if (${Spawn[${CorpseList.Arg[${i},|]}].Distance3D}>15 && ${Target.ID} && ${Target.Type.Equal[corpse]}) /squelch /alert add 25 id ${Target.ID}
     /squelch /stick off
   }
   /call ADSpawnCheck
   /if (${ADMobCount}) /goto :tMobsAround
 /next i
:tMobsAround
 /if (${LMFollow}) {
   /varset FollowFlag 1
   /call AdvPathCall
   /varset FollowFlag 1
   /squelch /afollow unpause
 } else {
   /call MBMoveTo ${LMStartY} ${LMStartX}
   /delay 15s !${Me.Moving}
   /face heading ${LMStartHeading}
 }
 /if (${Select[${MakeCamp},on]}) /squelch /makecamp mpause
 /if (${Corpse.Open}) /call CloseCorpse
 /doevents
/return

Sub Event_ALCantLoot
 /if (${Target.ID} && ${Target.Type.Equal[corpse]}) /varset ALDontLoot ${Target.ID}
 /varset ALDontLootTimer 2m
/return