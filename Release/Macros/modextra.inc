
| Some extra things - Not necessary for ModBot.
|-------------------------------------------------------

| Bind xbots - Sample of the new #bind. The /xbots command will fill the XTargets (bottom-up) with the other characters on NetBots (not in my group), auto-hate the rest.
#bind XBots /xbots

Sub Bind_XBots
   /declare b int local
   /declare x int local
   /declare sNBClnt string local
   /if (!${NetBots.Counts} || !${Me.XTargetSlots}) /return
   /varset x ${Me.XTargetSlots}
   /for b 1 to ${NetBots.Counts}
      /if (${x}<1) /return
      /varset sNBClnt ${NetBots.Client.Arg[${b}]}
      /if (!${sNBClnt.Equal[${Me.CleanName}]} && !${NetBots[${sNBClnt}].InGroup}) {
         /docommand /xtarget set ${x} ${sNBClnt}
         /varcalc x ${x}-1
      }
   /next b
   /if (${x}<1) /return
   /for b ${x} downto 1
      /docommand /xtarget set ${b} AH
   /next b
/return


| Bind - autoskills - The /autoskills command will list the combat skills you currently have on /autoskill.
#bind AutoSkills /autoskills

Sub Bind_AutoSkills
   /declare used int local
   /declare x int local
| Currently, only two concurrent skills allowed
   /for x 1 to 2
      /if (${Me.AutoSkill[${x}].Name.NotEqual[NULL]}) {
         /varcalc used ${used}+1
         /echo AutoSkill ${used} -> ${Me.AutoSkill[${x}]}
      }
   /next x
   /if (${used}<1) /echo No AutoSkills in use
/return
