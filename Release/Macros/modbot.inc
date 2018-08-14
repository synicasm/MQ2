
#Event Invite "#*#invites you to join a #1#.#*#"
#Event Slain "#*#You have been slain by#*#"
#Event DelayOnZone "#*#You have entered#*#"
#Event DelayOnZone "#*#LOADING, PLEASE WAIT.#*#"
#Event SummonBoth "You are low on food and drink."
#Event SummonBoth "You are out of food and drink."
#Event SummonBoth "You are out of drink and low on food."
#Event SummonBoth "You are out of food and low on drink."
#Event SummonDrink "You are low on drink."
#Event SummonFood "You are low on food."
#Event SummonDrink "You are out of drink."
#Event SummonFood "You are out of food."
#Event CastRst "Your target resisted the#*#"
|#Event CastImm "Your target cannot be#*#"
#Event CastStn "You are stunned#*#"
#Event CastStn "You can't cast spells while stunned!#*#"
#Event CastStn "You miss a note#*#"
#Event CharmBroke "Your |${ADCharmSpell}| spell has worn off#*#"
|#event ResetMelee "You cannot see your target."
#Event WornOff "#*#Your #1# spell has worn off of #2#."
#Event Camping "#*#more seconds to prepare your camp#*#"
#Event GoM "#*#granted gift of #1# to #2#!"
#Event GoM "#*#granted a gracious gift of #1# to #2#!"
#Event OutOfStuff "Sorry, but you don't have everything you need for this recipe in your general inventory."
#Event Broke "#*#you cannot afford#*#"
#Event Broke "#*#you can't afford#*#"
#Event EQBC "<#1#> #2#"
#Event EQBC "[#1#(msg)] #2#"
#Event EQBC "[MQ2] mb- #2#"
#event AAEarned "#*#You have gained#*#ability point#*#" 


Sub Event_EQBC(EQBCSay,EQBCSender,EQBCCommand)
   /if (!${Defined[EQBCSender]}) /declare EQBCSender string local
   /varset EQBCCommand ${EQBCSay.Right[-${Math.Calc[${EQBCSay.Find[${EQBCCommand}]}-1]}]}
   /if (!${DoBuffBot} && !${Select[${EQBCSender},${MasterList}]} && ${EQBCSender.Length} && !${MasterList.Equal[Netbots]} && ${NetBots.Client.Find[${EQBCSender}]}) /return
   /if (${Select[${EQBCSender},${MasterList}]} && (${Select[${EQBCCommand.Arg[1]} || ${EQBCCommand.Arg[2]},Casted:,Buffed:,Debuffed:,Healing:,Meleeing:,Selling:,Setting,attacking]} || ${EQBCCommand.Find[immune to]})) /return
   /if (!${EQBCSender.Length}) /varset EQBCSender LOCAL  
   /if (${EQBCCommand.Arg[1].Equal[${Me.Name}]}) /varset EQBCCommand ${EQBCCommand.Mid[${Math.Calc[${Me.Name.Length}+1]},${Math.Calc[${EQBCCommand.Length}-${Me.Name.Length}]}]}
   /varset CurrentEQBCCommand ${EQBCCommand}
   /call Event_chat "eqbc" "${EQBCSender}" "${EQBCCommand}"
/return


Sub Event_chat(MsgType,MsgFrom,MsgText)
   /if ((${Select[${MsgText},Casted:,Buffed:,Debuffed:,Healing:,Meleeing:,Selling:,Setting,attacking, master]} || ${MsgFrom.Equal[${Me.Pet.Name}]} || ${Spawn[npc ${MsgFrom}].ID})) /return
   /if (${MsgType.Equal[eqbc]}) /varset MsgText ${CurrentEQBCCommand}
   /if (${MsgType.Equal[group]} && ${MsgText.Find[reconnect]}) {
      /if (${MsgText.Arg[2, ].Length}>0) {
         /if (${EQBC.Connected}) /bccmd quit
         /delay 5 !${EQBC.Connected}
         /delay 5
         /bccmd connect ${MsgText.Arg[2, ]} ${If[${MsgText.Arg[3, ].Length},${MsgText.Arg[3, ]},]}
      } else {	
         /bccmd reconnect
      }
      /return
   }
   /declare a int local
   /if (${MBDebug}) /echo Macro.RunTime:${Macro.RunTime} - Sub:Event_Chat
   /if (${MsgType.NotEqual[EQBC]} && ${MsgFrom.Left[1].Compare[ ]}<0) /varset MsgFrom ${MsgFrom.Right[-2].Left[-1]}
   /if (!${DoBuffBot} && !${Select[${MsgFrom},${MasterList},LOCAL]} && (!${MasterList.Equal[Netbots]} || !${NetBots.Client.Find[${MsgFrom}]}) || ${MsgFrom.Equal[${Me.Name}]}) {
      /if (!${NetBots[${MsgFrom}].ID} && ${MsgType.Equal[TELL]} && ${DoTells} && !${MsgFrom.Equal[${Me.Name}]} && !${MsgFrom.Equal[${Me.Pet.CleanName}]} && !${Spawn[npc ${MsgFrom}].ID}) /call TellEvent ${MsgFrom} "${MsgText}"
      /if (${MsgType.NotEqual[eqbc]} && !${Select[${MsgFrom},${MasterList},LOCAL]}) {
         /for a 1 to ${Math.Calc[${MsgText.Count[ ]}+1]}
            /if (${ABAliasList.Find[${MsgText.Arg[${a}]}]}) /call CastCall ${MsgFrom} "cast ${MsgText.Arg[${a}]}"
         /next a
      }
      /return
   }
   /if (${MsgFrom.Equal[local]}) {
      /varset SenderName ${Me.Name}
   } else {
      /varset SenderName ${MsgFrom}
   }
   /if (${DoBuffBot}) /varset MsgText cast ${MsgText}
   /if (${MsgText.Left[5].Equal[abort]}) {
      /if (${ACState}) {
         /varcalc ACState ${ACState}+2
         /call AdvCombat
      }
      /if (${Me.Casting.ID}) {
         /if (${Me.Class.ShortName.Equal[BRD]}) /squelch /stoptwist
         /interrupt
         /if (${Me.Class.ShortName.Equal[BRD]}) /squelch /stoptwist
      }
      /if (${Me.Mount.ID}) /dismount
      /varset DoMelee FALSE
      /varset DoDebuffs FALSE
      /varset DoHeals FALSE
      /if (!${Me.Class.ShortName.Equal[brd]}) /varset DoBuffs FALSE
      /bc Setting - DoHeals, DoBuffs, DoDebuffs & DoMelee FALSE and Following ${SenderName}
      /varset MsgText follow close
   }
   /if (${MsgText.Left[4].Equal[help]}) {
      /if (!${MsgText.Right[-4].Length}) {
         /echo Modbot command list.  Use "/mb help command" for specific help on each.
         /echo addbuff adddebuff buffup cast DoBuffs DoSongs DoDebuffs DoHeals DoEvents docombines DoMelee DoAFK DoSit DoForage DoLoot dolist DoPet DoMount DoFW doquest DoBuffBot follow give MBPause MBP MBWayRec MBWayPlay Offtank goto mana letsroll holdup attack stop sing exclude include sell script campout inviteall setinivar handin getbank PetShrink search DoAA DoRez 
      } else {
         /echo This feature is currently under construction.
      }
      /return
   }
   /if (${MsgText.Left[9].Equal[docommand]}) {
      /echo Processing command ${MsgText.Right[-10]}
      /docommand ${MsgText.Right[-10]}
      /return
   }
   /if (${MsgText.Left[6].Equal[search]}) {
      /declare SearchText string local ${MsgText.Right[-7]}
      /declare ReplyText string local ${FindItemCount[${SearchText}]} on me and ${FindItemBankCount[${SearchText}]} in my bank. 
      /declare Count1 int local ${FindItemCount[${SearchText}]}
      /declare BCount1 int local ${FindItemBankCount[${SearchText}]}
      /if (${Count1}>0 && ${FindItemCount[=${SearchText}]}!=${Count1} || ${BCount1}>0 && ${FindItemBankCount[=${SearchText}]}!=${BCount1}) {
         /varset ReplyText ${ReplyText} - possible different items.
      }
      /bc ${ReplyText}
      /return
   }
   /if (${MsgText.Left[7].Equal[addbuff]}) {
      /if (${MsgText.Arg[2].Equal[help]}) {
         /echo Format: /mb addbuff "<Spell Name>|<gem,alt,item,ect>" <Alias> <TarType> <TarCnt>
         /return
      }
      /if (${MsgText.Arg[2].Arg[1,|].Length} && ${MsgText.Arg[2].Arg[2,|].Length}) /varcalc ABCount ${ABCount}+1
      /ini "${IniFile}" AdvBuff ABCount ${ABCount}
      /echo ${MsgText.Right[-8]} : ABCount = ${ABCount}
      /if (${ABNewFormat}) {
         /ini "${IniFile}" AB${ABCount} Gem ${MsgText.Arg[2].Arg[2,|]}
         /ini "${IniFile}" AB${ABCount} Spell "${MsgText.Arg[2].Arg[1,|]}"
         /if (${MsgText.Arg[3].Length}) /ini "${IniFile}" AB${ABCount} SpellAlias "${MsgText.Arg[3]}"
         /if (${MsgText.Arg[4].Length}) /ini "${IniFile}" AB${ABCount} TarType "${MsgText.Arg[4]}"
         /if (${MsgText.Arg[5].Length}) /ini "${IniFile}" AB${ABCount} TarCnt "${MsgText.Arg[5]}"
      } else {
         /ini "${IniFile}" AdvBuff ABGem${ABCount} ${MsgText.Arg[2].Arg[2,|]}
         /ini "${IniFile}" AdvBuff ABSpell${ABCount} "${MsgText.Arg[2].Arg[1,|]}"
         /if (${MsgText.Arg[3].Length}) /ini "${IniFile}" AdvBuff ABSpellAlias${ABCount} "${MsgText.Arg[3]}"
         /if (${MsgText.Arg[4].Length}) /ini "${IniFile}" AdvBuff ABTarType${ABCount} "${MsgText.Arg[4]}"
         /if (${MsgText.Arg[5].Length}) /ini "${IniFile}" AdvBuff ABTarCnt${ABCount} "${MsgText.Arg[5]}"
      }
      /bc Restart to add to memory.
      /return
   }  
   /if (${MsgText.Left[9].Equal[adddebuff]}) {
      /if (${MsgText.Arg[2].Equal[help]}) {
         /echo Format: /mb adddebuff "<Spell Name>|<gem,alt,item,ect>" <Alias> <TarType> <TarCnt>
         /return
      }
      /if (${MsgText.Arg[2].Arg[1,|].Length} && ${MsgText.Arg[2].Arg[2,|].Length} || ${MsgText.Arg[2].Find[gem]}) /varcalc ADCount ${ADCount}+1
      /ini "${IniFile}" AdvDebuff ADCount ${ADCount}
      /echo ${MsgText.Right[-8]} : ADCount = ${ADCount}
      /declare dspell string local ${MsgText.Arg[2].Arg[1,|]}
      /declare dgem string local ${MsgText.Arg[2].Arg[2,|]}
      /if (${dgem.Find[gem]}) /varset dgem ${dgem.Right[-3]}
      /if (${ADNewFormat}) {
         /ini "${IniFile}" AD${ADCount} Gem ${dgem}
         /if (${dspell.Find[gem]}) {
            /ini "${IniFile}" AD${ADCount} Spell "${Me.Gem[${dgem}]}"
         } else {
            /ini "${IniFile}" AD${ADCount} Spell "${MsgText.Arg[2].Arg[1,|]}"
         }
         /if (${MsgText.Arg[3].Length}) /ini "${IniFile}" AD${ADCount} SpellAlias "${MsgText.Arg[3]}"
         /if (${MsgText.Arg[4].Length}) /ini "${IniFile}" AD${ADCount} TarType "${MsgText.Arg[4]}"
         /if (${MsgText.Arg[5].Length}) /ini "${IniFile}" AD${ADCount} TarCnt "${MsgText.Arg[5]}"
      } else {
         /ini "${IniFile}" AdvDebuff ADGem${ADCount} ${dgem}
         /if (${dspell.Find[gem]}) {
            /ini "${IniFile}" Advdebuff ADSpell${ADCount} "${Me.Gem[${dgem}]}"
         } else {
            /ini "${IniFile}" Advdebuff ADSpell${ADCount} "${MsgText.Arg[2].Arg[1,|]}"
         }
         /if (${MsgText.Arg[3].Length}) /ini "${IniFile}" Advdebuff ADSpellAlias${ADCount} "${MsgText.Arg[3]}"
         /if (${MsgText.Arg[4].Length}) /ini "${IniFile}" Advdebuff ADTarType${ADCount} "${MsgText.Arg[4]}"
         /if (${MsgText.Arg[5].Length}) /ini "${IniFile}" Advdebuff ADTarCnt${ADCount} "${MsgText.Arg[5]}"
      }
      /bc Restart to add to memory.
      /return
   }  
   /if (${MsgText.Left[7].Equal[DoBuffs]} || ${MsgText.Left[7].Equal[DoSongs]}) {
      /varset DoBuffs ${If[(${MsgText.Find[on]} || ${MsgText.Find[true]}),TRUE,${If[(${MsgText.Find[off]} || ${MsgText.Find[false]}),FALSE,${If[${DoBuffs},FALSE,TRUE]}]}]}
      /bc Setting - DoBuffs ${DoBuffs}
      /return
   }
   /if (${MsgText.Left[9].Equal[DoDebuffs]}) {
      /varset DoDebuffs ${If[(${MsgText.Find[on]} || ${MsgText.Find[true]}),TRUE,${If[(${MsgText.Find[off]} || ${MsgText.Find[false]}),FALSE,${If[${DoDebuffs},FALSE,TRUE]}]}]}
      /bc Setting - DoDebuffs ${DoDebuffs}
      /return
   }
   /if (${MsgText.Left[7].Equal[DoHeals]}) {
      /varset DoHeals ${If[(${MsgText.Find[on]} || ${MsgText.Find[true]}),TRUE,${If[(${MsgText.Find[off]} || ${MsgText.Find[false]}),FALSE,${If[${DoHeals},FALSE,TRUE]}]}]}
      /bc Setting - DoHeals ${DoHeals}
      /return
   }
   /if (${MsgText.Left[7].Equal[DoCures]}) {
      /varset DoCures ${If[(${MsgText.Find[on]} || ${MsgText.Find[true]}),TRUE,${If[(${MsgText.Find[off]} || ${MsgText.Find[false]}),FALSE,${If[${DoCures},FALSE,TRUE]}]}]}
      /bc Setting - DoCures ${DoCures}
      /return
   }
   /if (${MsgText.Left[8].Equal[DoEvents]}) {
      /varset DoEvents ${If[(${MsgText.Find[on]} || ${MsgText.Find[true]}),TRUE,${If[(${MsgText.Find[off]} || ${MsgText.Find[false]}),FALSE,${If[${DoEvents},FALSE,TRUE]}]}]}
      /bc Setting - DoEvents ${DoEvents}
      /return
   }
   /if (${MsgText.Left[7].Equal[DoMelee]}) {
      /varset DoMelee ${If[(${MsgText.Find[on]} || ${MsgText.Find[true]}),TRUE,${If[(${MsgText.Find[off]} || ${MsgText.Find[false]}),FALSE,${If[${DoMelee},FALSE,TRUE]}]}]}
      /bc Setting - DoMelee ${DoMelee}
      /return
   }
   /if (${MsgText.Left[5].Equal[DoAfk]}) {
      /varset DoAfk ${If[(${MsgText.Find[on]} || ${MsgText.Find[true]}),TRUE,${If[(${MsgText.Find[off]} || ${MsgText.Find[false]}),FALSE,${If[${DoAfk},FALSE,TRUE]}]}]}    
      /bc Setting - DoAfk ${DoAfk}
      /return
   }
   /if (${MsgText.Left[5].Equal[DoSit]}) {
      /varset DoSit ${If[(${MsgText.Find[on]} || ${MsgText.Find[true]}),TRUE,${If[(${MsgText.Find[off]} || ${MsgText.Find[false]}),FALSE,${If[${DoSit},FALSE,TRUE]}]}]}
      /bc Setting - DoSit ${DoSit}
      /return
   }
   /if (${MsgText.Left[8].Equal[DoForage]}) {
      /varset DoForage ${If[(${MsgText.Find[on]} || ${MsgText.Find[true]}),TRUE,${If[(${MsgText.Find[off]} || ${MsgText.Find[false]}),FALSE,${If[${DoForage},FALSE,TRUE]}]}]}
      /bc Setting - DoForage ${DoForage}
      /return
   }
   /if (${MsgText.Left[6].Equal[DoLoot]}) {
      /varset DoLoot ${If[(${MsgText.Find[on]} || ${MsgText.Find[true]}),TRUE,${If[(${MsgText.Find[off]} || ${MsgText.Find[false]}),FALSE,${If[${DoLoot},FALSE,TRUE]}]}]}
      /bc Setting - DoLoot ${DoLoot}
      /return
   }
   /if (${MsgText.Left[5].Equal[DoPet]}) {
      /varset DoPet ${If[(${MsgText.Find[on]} || ${MsgText.Find[true]}),TRUE,${If[(${MsgText.Find[off]} || ${MsgText.Find[false]}),FALSE,${If[${DoPet},FALSE,TRUE]}]}]}
      /bc Setting - DoPet ${DoPet}
      /return
   }
   /if (${MsgText.Left[7].Equal[DoMount]}) {
      /varset DoMount ${If[(${MsgText.Find[on]} || ${MsgText.Find[true]}),TRUE,${If[(${MsgText.Find[off]} || ${MsgText.Find[false]}),FALSE,${If[${DoMount},FALSE,TRUE]}]}]}
      /bc Setting - DoMount ${DoMount}
      /return
   }
   /if (${MsgText.Left[4].Equal[DoFW]}) {
      /varset DoFW ${If[(${MsgText.Find[on]} || ${MsgText.Find[true]}),TRUE,${If[(${MsgText.Find[off]} || ${MsgText.Find[false]}),FALSE,${If[${DoFW},FALSE,TRUE]}]}]}
      /bc Setting - DoFW ${DoFW}
      /return
   }
   /if (${MsgText.Left[5].Equal[DoRez]}) {
      /varset DoRez ${If[(${MsgText.Find[on]} || ${MsgText.Find[true]}),TRUE,${If[(${MsgText.Find[off]} || ${MsgText.Find[false]}),FALSE,${If[${DoRez},FALSE,TRUE]}]}]}
      /bc Setting - DoRez ${DoRez}
      /return
   }
   /if (${MsgText.Left[4].Equal[DoAA]}) {
      /if (${MsgText.Right[-5].Equal[force]}) {
         /bc Forcing an AA purchase Check
         /call AAPicker force
      } else {
         /varset DoAA ${If[(${MsgText.Find[on]} || ${MsgText.Find[true]}),TRUE,${If[(${MsgText.Find[off]} || ${MsgText.Find[false]}),FALSE,${If[${DoAA},FALSE,TRUE]}]}]}
         /bc Setting - DoAA ${DoAA}
      }
      /return
   }
   /if (${MsgText.Left[3].Equal[MBP]} || ${MsgText.Left[7].Equal[MBPause]}) {
      /if ((${MsgText.Find[on]} || ${MsgText.Find[true]}) && ${ACState}) {
         /varcalc ACState ${ACState}+2
         /call AdvCombat
      }
      /varset MBPause ${If[(${MsgText.Find[on]} || ${MsgText.Find[true]}),TRUE,${If[(${MsgText.Find[off]} || ${MsgText.Find[false]}),FALSE,${If[${MBPause},FALSE,TRUE]}]}]}
      /bc Setting - MBPause ${MBPause}
      /if (${MBPause} && ${CampStatus}) /makecamp off
      /if (${MBPause} && ${Me.Class.ShortName.Equal[brd]}) {
         /if (${Twist}) /twist off
      }
      /return
   }
   /if (${MsgText.Left[8].Equal[MBWayRec]}) {
      /call MBWayRecord ${MsgText.Right[-9]}
      /return
   }
   /if (${MsgText.Left[9].Equal[MBWayPlay]}) {
      /call MBWayPlay ${MsgText.Right[-10]}
      /if (${Macro.Return.Equal[false]}) /bc Problem with playback
      /return
   }
   /if (${MsgText.Left[9].Equal[DoBuffBot]}) {
      /varset DoBuffBot ${If[(${MsgText.Find[on]} || ${MsgText.Find[true]}),TRUE,${If[(${MsgText.Find[off]} || ${MsgText.Find[false]}),FALSE,${If[${DoBuffBot},FALSE,TRUE]}]}]}
      /bc Setting - DoBuffBot ${DoBuffBot}
      /return
   }
   /if (${MsgText.Left[7].Equal[OffTank]}) {
      /varset OffTank ${If[(${MsgText.Find[on]} || ${MsgText.Find[true]}),TRUE,${If[(${MsgText.Arg[2].Equal[off]} || ${MsgText.Find[false]}),FALSE,${If[${OffTank},FALSE,TRUE]}]}]}
      /bc Setting - OffTank ${OffTank}
|     /if (${OffTank}) {
|        /varset ACMATarget 0
|        /varset ACTarID 0
|     }
      /return
   }
   /if (${MsgText.Left[4].Equal[goto]}) {
      /if (!${MsgText.Arg[2].Length} || !${MsgText.Arg[3].Length}) /return
      /if (!${Me.Standing}) /stand
      /moveto loc ${MsgText.Arg[2]} ${MsgText.Arg[3]}
      /return
   }
   /if (${MsgText.Left[6].Equal[dolist]}) {
      /bc DoBuffs ${DoBuffs}
      /bc DoDebuffs ${DoDebuffs}
      /bc DoHeals ${DoHeals}
      /bc DoEvents ${DoEvents}
      /bc DoMelee ${DoMelee}
      /bc DoAfk ${DoAfk}
      /bc DoSit ${DoSit}
      /bc DoLoot ${DoLoot}
      /bc DoForage ${DoForage}
      /bc DoFW ${DoFW}
      /return
   }
   /if (${MsgText.Left[4].Equal[mana]}) {
      /bc ${Me.PctMana}%
      /return
   }
   /if (${MsgText.Left[8].Equal[letsroll]}) {
      /bc Setting - DoCures,DoBuffs,DoHeals,DoMelee,DoEvents,DoDebuffs TRUE
      /varset DoDebuffs TRUE
      /varset DoHeals TRUE
      /varset DoMelee TRUE
      /varset DoEvents TRUE
      /varset DoBuffs TRUE
      /varset DoCures TRUE
      /varset MakeCampD ${Me.Heading.DegreesCCW}
      /if (${MsgText.Equal[letsroll unpause]}) {
         /bc Setting - MBPause FALSE
         /varset MBPause FALSE
      }
      /return
   }
   /if (${MsgText.Left[6].Equal[holdup]}) {
      /bc Setting - DoBuffs,DoDebuffs,DoEvents,DoMelee,DoCures,DoLoot,DoPull FALSE
      /varset DoDebuffs FALSE
      /varset DoEvents FALSE
      /varset DoHeals TRUE
      /if (!${Me.Class.ShortName.Equal[brd]}) /varset DoBuffs FALSE
      /varset DoMelee FALSE
      /varset DoCures FALSE
      /varset DoPull FALSE
      /varset DoLoot FALSE
      /return
   }
   /if (${MsgText.Left[6].Equal[buffup]}) {
      /bc Setting - DoBuffs & DoEvents & DoHeals TRUE
      /varset DoEvents TRUE
      /varset DoHeals TRUE
      /varset DoBuffs TRUE
      /return
   }
   /if (${MsgText.Left[7].Find[attack]}) {
      /bc Setting - DoMelee TRUE & Attacking
      /varset ACState 100
      /varset DoMelee TRUE
      /if (!${MsgText.Arg[2].Length}) /varset MsgText ${MsgText} ${Me.CleanName}
      /call AdvCombat ${If[${MsgText.Arg[2].Length},${MsgText.Arg[2]},]} ${If[${MsgText.Arg[2].Length} && ${MsgText.Arg[3].Length},${MsgText.Arg[3]},]} ${If[${MsgText.Arg[2].Length} && ${MsgText.Arg[3].Length} && ${MsgText.Arg[4].Length},${MsgText.Arg[4]},]}
      /return
   }
   /if (${MsgText.Left[4].Equal[stop]}) {
      /if (${Select[${ACState},2,4]}) /varset ACState 1
      /if (${AdvPath.Following} || ${AdvPath.Paused}) {
         /call AdvPlugCall off
      } else {
         /squelch /stick off
         /if (${CampStatus}==1) {
            /varset MakeCampX ${Me.X}
            /varset MakeCampY ${Me.Y}
            /varset MakeCampD ${Me.Heading.DegreesCCW}
            /makecamp on
         } else {
            /makecamp off
         }
      }
      /varset FollowFlag 0
      /varset FollowID 0
      /varset FollowName
      /varset ACStartY ${Me.Y}
      /varset ACStartX ${Me.X}
      /varset ACStartHeading ${Me.Heading.DegreesCCW}
      /if (!${AdvPath.Following} || ${Stick.Status.Equal[off]}) /bc No longer on AF
      /return
   }
   /if (${MsgText.Left[8].Equal[makecamp]}) {
      /squelch /stick off
      /if (${AdvPath.Following}) /afollow off
      /varset FollowFlag 0
      /makecamp on
      /varset MakeCampX ${Me.X}
      /varset MakeCampY ${Me.Y}
      /varset MakeCampD ${Me.Heading.DegreesCCW}
      /varset CampStatus 1
      /bc Hanging out
      /return
   }
   /if (${MsgText.Left[5].Equal[cast ]} || (${Me.Class.ShortName.Equal[brd]} && (${MsgText.Left[4].Equal[sing]} || ${MsgText.Left[4].Equal[sung]})) ) {
      /call CastCall "${SenderName}" "${MsgText}"
      /return
   }
   /if (${MsgText.Left[7].Equal[exclude]}) {
      /if (${MsgText.Right[-8].Equal[save]}) {
         /bc Saving Exclude List
         /call ProcessExcludeList save
      } else {
         /bc Excluding ${MsgText.Right[-8]}
         /if (!${ExcludeList.Find[|${MsgText.Right[-8]}|]}) /varset ExcludeList ${ExcludeList}${MsgText.Right[-8]}|
         /call ProcessExcludeList
      }
      /return
   }
   /if (${MsgText.Left[4].Equal[sell]} && !${MsgText.Left[7].Equal[selling]}) {
      /if (!${MsgText.Right[-5].Length} && ${Select[${Target.Class},Merchant]}) {
         /declare MerchName string local ${Target.CleanName}
      } else /if (${MsgText.Right[-5].Length}) {
         /declare MerchName string local ${MsgText.Right[-5]}
      } else /if (${Spawn[Merchant].Distance} < 20) {
         /declare MerchName string local ${Spawn[Merchant].Name}
      }
      /bc Selling items to ${MerchName}
      /call SellCheck "${MerchName}"
   }
   /if (${MsgText.Left[4].Equal[give]} && ${MsgText.Right[-5].Length}) {
      /bc Giving items to ${MsgText.Arg[2]}
      /call GiveCheck ${MsgText.Right[-5]}
      /return
   }
   /if (${MsgText.Left[7].Equal[include]}) {
      /bc Including ${MsgText.Right[-8]}
      /if (${ExcludeList.Find[${MsgText.Right[-8]}]}) {
         /declare sVarM string local ${MsgText.Right[-8]}|
         /if (${ExcludeList.Find[#${MsgText.Right[-8]}]}) /varset sVarM #${sVarM}
         /declare sVarL string local ${ExcludeList.Mid[1,${Math.Calc[${ExcludeList.Find[${sVarM}]}-1]}]}
         /declare sVarR string local ${ExcludeList.Mid[${Math.Calc[${sVarL.Length}+${sVarM.Length}+1]},${ExcludeList.Length}]}
         /varset ExcludeList ${sVarL}${sVarR}
      }
      /call ProcessExcludeList
      /return
   }
   /if (${MsgText.Left[6].Equal[follow]}) {
      /squelch /makecamp off
      /varset CampStatus 0
      /varset ACStartHeading 0
      /varset FollowID ${Spawn[pc ${SenderName}].ID}
      /if (!${FollowID}) /return
      /if (${MoveTo.Moving}) /moveto off
      /if (${MsgText.Equal[follow me]} || ${MsgText.Equal[follow]} || ${MsgText.Find[follow close]}) {
         /if (${AdvPath.Active} && !${MsgText.Find[follow close]}) {
            /if (${Stick.Status.Equal[ON]}) /squelch /stick off
            /call AdvPlugCall "${SenderName}"
         }
      } else {
         /varset FollowID ${Spawn[${MsgText.Right[-7]}].ID}
         /if (${AdvPath.Active}) /call AdvPlugCall ${Spawn[${MsgText.Right[-7]}].CleanName}
      }
      /varset FollowName ${Spawn[${FollowID}].Name}
      /varset FollowFlag ${If[!${FollowID} || ${Spawn[${FollowID}].Type.Equal[corpse]},0,1]}
      /call AdvPathCall
      /if (${MsgText.Equal[follow close]} && ${FollowID}) {
         /if (${AdvPath.Active}) /afollow off
         /if (${Target.ID}!=${FollowID}) {
            /tar id ${FollowID}
            /delay 5 ${Target.ID}==${FollowID}
         }
         /squelch /stick 5 uw
      }
      /if (${Stick.Status.Equal[on]} || ${AdvPath.Following}) /bc AFing ${Spawn[${FollowID}].CleanName}
      /return
   }
   /if (${MsgText.Left[6].Equal[Script]}) {
     /call MBScript "${MsgText.Right[-7]}" ${MsgFrom}
     /return
   }
   /if (${MsgText.Left[7].Equal[campout]}) {
      /if (${AdvPath.Following}) /squelch /afollow off
      /if (${Stick.Status.Equal[on]}) /stick off
      /if (${Me.Casting.ID}) /delay 5s !${Me.Casting.ID}
      /if (${Me.Mount.ID}) {
         /dismount
         /delay 1s
      }
      /if (${Me.AltAbilityReady[Companion's Suspension]} && ${Me.Pet.ID} && ${PetCast.Arg[3,|].Find[sm]}) {
         /casting "Companion's Suspension|alt" -maxtries|5
         /delay 5s !${Me.Pet.ID}
         /docommand ${If[${Me.Pet.ID},/bc hmm pet didn't cache,/bc Cached the pet]}
      }
      /if (${MsgText.Equal[campout char]}) {
         /bc Camping to character select
         /docommand /camp
         /delay 3s
         /docommand /endmacro
      }
      /if (${MsgText.Right[-7].Length}>3) {
         /if (${MsgText.Right[-7].Find[trader]}) {
            /bc Camping and selecting this account's trader to login
            /call Login "${TraderName}"
         } else {
            /bc Camping and selecting ${MsgText.Right[-7]} to login
            /call Login ${MsgText.Right[-7]}
         }
         /bc We had a problem Houston, ending mac
         /delay 3s
         /docommand /endmacro
      }
      /bc Camping to desktop
      /docommand /camp desktop
      /delay 3s
      /docommand /endmacro
   }
   /if (${MsgText.Left[6].Equal[dopull]}) {
      /varset DoPull ${If[(${MsgText.Find[on]} || ${MsgText.Find[true]}),TRUE,${If[(${MsgText.Find[off]} || ${MsgText.Find[false]}),FALSE,${If[${DoPull},FALSE,TRUE]}]}]}
      /bc Setting - DoPull ${DoPull}
      /if (!${DoPull}) /return
      /if (${MsgText.Arg[2].Length} && !${Select[${MsgText.Arg[2]},on,off]}) {
         /varset APPath ${MsgText.Arg[2]}
         /bc APPath set to [ ${MsgText.Arg[2]} ]
      }
      /if (${TankName.Equal[${Me.Name}]}) /varset ACAssistPct 110
      /if (!${CampStatus}) /mb makecamp
      /return
   }
   /if (${MsgText.Left[7].Equal[DoTells]}) {
      /varset DoTells ${If[(${MsgText.Find[on]} || ${MsgText.Find[true]}),TRUE,${If[(${MsgText.Find[off]} || ${MsgText.Find[false]}),FALSE,${If[${DoTells},FALSE,TRUE]}]}]}
      /bc Setting - DoTells ${DoTells}
      /return
   }
   /if (${MsgText.Left[6].Equal[recast]}) {
      /declare b int local 0
      /declare c int local 0
      /for b 1 to ${ABCount}
         /for c 1 to ${Math.Calc[${ABSpellAlias[${b}].Count[|]}+1]}
            /if (${MsgText.Arg[2].Equal[${ABSpellAlias[${b}].Arg[${c},|]}]}) {
               /if (!${ABRecast[${b}]}) {
                  /varset ABRecast[${b}] TRUE
                  /bc ${ABSpell[${b}]} will now recast when worn off.
               }
            }
         /next c
      /next b
      /return
   }
   /if (${MsgText.Left[9].Equal[inviteall]}) {
      /declare i int local
      /if (${MsgText.Find[ ]}) {
         /echo Starting modbot on channel - ${MsgText.Arg[2]}
         /bct ${MsgText.Arg[2]} //mac modbot
      } else {
         /echo Starting modbot and inviting all clients.
         /noparse /bca //if (!${Zone.Name.Equal[The Bazaar]}) /mac modbot
      }
      /delay 15s
      /for i 1 to ${Math.Calc[${NetBots.Counts}]}
         /if (${NetBots.Client.Arg[${i}].NotEqual[${Me.Name}]} && !${Group.Member[${NetBots.Client.Arg[${i}]}].Index}) { 
            /invite ${NetBots.Client.Arg[${i}]}
            /delay ${Math.Rand[20]}
         }
         /if (${i}==6) /break
      /next i
   }
   /if (${MsgText.Left[7].Equal[doquest]}) {
      /if (${Select[${MsgText.Arg[2]},on,off,true,false]} || !${MsgText.Arg[2].Length}) {
         /varset DoQuest ${If[(${MsgText.Find[on]} || ${MsgText.Find[true]}),TRUE,${If[(${MsgText.Find[off]} || ${MsgText.Find[false]}),FALSE,${If[${DoQuest},FALSE,TRUE]}]}]}
         /bc Setting - DoQuest ${DoQuest}
      }
      /if (${Cursor.ID} && !${Target.ID} && !${Select[${MsgText.Arg[2]},on,off,true,false]} && ${MsgText.Arg[3].Length}) {
         /declare qName string local
         /declare i int local
         /for i 3 to ${Math.Calc[${MsgText.Count[ ]}+1]}
            /varset qName ${qName} ${MsgText.Arg[${i}]}
         /next i
         /ini "${LootIni}" ${Cursor.Name.Left[1]} "${Cursor.Name}" "Quest|${MsgText.Arg[2]}|${qName}|"
         /echo Added - "Quest|${MsgText.Arg[2]}|${qName}|" - to loot.ini for ${Cursor.Name}
      }
   }
   /if (${MsgText.Left[9].Equal[docombine]}) {
      /varset DoCombines ${If[(${MsgText.Find[on]} || ${MsgText.Find[true]}),TRUE,${If[(${MsgText.Find[off]} || ${MsgText.Find[false]}),FALSE,${If[${DoCombines},FALSE,TRUE]}]}]}
      /bc Setting - DoCombines ${DoCombines}
      /return
   }
   /if (${MsgText.Left[9].Equal[setinivar]}) {
      /if (!${MsgText.Arg[2].Length} || !${MsgText.Arg[3].Length}) {
         /echo Usage: /mb setinivar <Varname> <Setting>  (use quotes around multiple word settings)
         /return
      }
      /declare IniSection string local
      /declare b int local
      /declare IniVar string local ${MsgText.Arg[2]}
      /declare IniSetting string local ${MsgText.Arg[3]}
      /declare VarNumber int local
      /declare tSection string local Settings Melee AdvHeal AdvDebuff AdvBuff AdvEvent AdvCure AdvPull AdvCure
      /declare tClass string local
      /declare tVar string local
      /declare tNumCnt int local
      /if (${IniVar.Right[1].Equal[]]} && ${IniVar.Find[[]}) {
         /varset VarNumber ${MsgText.Arg[2].Arg[2,[].Left[-1]}
         /varset IniVar ${MsgText.Arg[2].Arg[1,[]}${VarNumber}
      }
      /if (${MsgText.Arg[4].Length}) {
         /for b 4 to ${Math.Calc[${MsgText.Count[ ]}+1]}
            /varset IniSetting ${IniSetting} ${MsgText.Arg[${b}]}
         /next b
      }
      /for a 1 to 9
         /varset IniSection ${tSection.Arg[${a}]}
         /if (!${IniSection.Length}) /return
         /echo ${IniFile},${IniSection}
         /for b 1 to ${Ini["${IniFile}",${IniSection}].Count[|]}+1
            /if (${Ini["${IniFile}",${IniSection}].Arg[${b},|].Equal[${IniVar}]}) {
               /ini "${IniFile}" "${IniSection}" "${IniVar}" "${IniSetting}"
               /echo You're setting for ${MsgText.Arg[2]} is now ${Ini["${IniFile}",${IniSection},${IniVar}]} in your .ini file.
               /if (${Defined[${MsgText.Arg[2]}]}) {
                  /varset ${MsgText.Arg[2]} ${IniSetting}
               } else {
                  /varset IniVar ${MsgText.Arg[2].Arg[1,[]}
                  /if (!${Defined[${IniVar}]}) {
                     /echo Cannot set variable in memory.  Defined variable does not match. -Restart required to set it.
                     /return
                  }
                  /varset ${MsgText.Arg[2]} ${IniSetting}
               }
               /return
            }
         /next b
      /next a
      /varset tClass ${IniVar.Left[2]}
      /if (${Select[${tClass},AH,AD,AB,AE,AQ]} && ${${tClass}NewFormat} && ${Select[${IniVar.Right[1]},1,2,3,4,5,6,7,8,9,0]}) {
         /varset tNumCnt 1
:IniNextNum
         /varcalc a ${IniVar.Length}-${tNumCnt}
         /if (${Select[${IniVar.Mid[${a},1]},1,2,3,4,5,6,7,8,9,0]}) {
            /varcalc tNumCnt ${tNumCnt}+1
            /goto :IniNextNum
         }
         /varset tVar ${IniVar.Left[-${tNumCnt}]}
         /varset tVar ${tVar.Right[-2]}
         /varset IniSection ${tClass}${IniVar.Right[${tNumCnt}]}
         /echo ${IniFile},${IniSection}
         /for b 1 to ${Ini["${IniFile}",${IniSection}].Count[|]}+1
            /if (${Ini["${IniFile}",${IniSection}].Arg[${b},|].Equal[${tVar}]}) {
               /ini "${IniFile}" "${IniSection}" "${tVar}" "${IniSetting}"
               /echo You're setting for ${MsgText.Arg[2]} is now ${Ini["${IniFile}",${IniSection},${tVar}]} in your .ini file.
               /if (${Defined[${MsgText.Arg[2]}]}) {
                  /varset ${MsgText.Arg[2]} ${IniSetting}
               } else {
                  /varset IniVar ${MsgText.Arg[2].Arg[1,[]}
                  /if (!${Defined[${IniVar}]}) {
                     /echo Cannot set variable in memory.  Defined variable does not match. -Restart required to set it.
                     /return
                  }
                  /varset ${MsgText.Arg[2]} ${IniSetting}
               }
               /return
            }
         /next b
      }   
:IniNoF
      /echo Variable not found
:IniVarDone
      /return
   }
   /if (${MsgText.Left[6].Equal[handin]}) {
      /declare tTarget string local ${Target.CleanName}
      /declare cItem string local ${Cursor.Name}
      /if (${MsgText.Arg[2].Length}) /varset tTarget ${MsgText.Arg[2]}
      /if (${MsgText.Arg[3].Length}) /varset cItem ${MsgText.Arg[3]}
      /if (!${tTarget.Length} || !${cItem.Length}) /echo Incorrect usage.  /mb handin "TargetName" "ItemName" or hold item and target..
      /echo Handing in all ${Cursor.Name}'s to ${Target.CleanName}
      /call GiveCheck "${tTarget}" "${cItem}" FALSE ${Math.Calc[${FindItemCount[=${Cursor.Name}]}+${Cursor.Stack}]}
      /return
   }
   /if (${MsgText.Left[7].Equal[getbank]}) {
      /if (!${MsgText.Right[-7].Length} && !${Cursor.ID}) /echo Usage - Hold Item to get on Cursor or getbank <ItemName> <InStacks> <Quantity>
      /if (${Cursor.ID}) {
         /call GetBankItem "${Cursor.Name}" ${MsgText.Right[-7]}
      } else {
         /call GetBankItem ${MsgText.Right[-7]}
      }
      /return
   }
   /if (${MsgText.Left[9].Equal[PetShrink]}) {
      /varset PetShrink ${If[(${MsgText.Find[on]} || ${MsgText.Find[true]}),TRUE,${If[(${MsgText.Find[off]} || ${MsgText.Find[false]}),FALSE,${If[${PetShrink},FALSE,TRUE]}]}]}    
      /bc Setting - PetShrink ${PetShrink}
      /return
   }  
   /if (${MsgText.Left[8].Equal[autobank]}) {
      /call autobank ${MsgText.Right[-9]}
   }
   /if (${MsgText.Left[9].Equal[saytarget]}) {
      /if (${NetBots[${SenderName}].TargetID}) {
         /declare saytarget int local ${NetBots[${SenderName}].TargetID}
         /target id ${saytarget}
         /delay 1s ${Target.ID}==${saytarget}
         /if (${Target.Distance} > 10) /stick 5
         /delay 1s ${Target.Distance} < 10
         /stick off
         /say ${MsgText.Right[-10]}
      }
   }
/return


Sub LoadINIVar(IniSection,IniVar,IniValue,MacroVar,MyIni,VarType)
   /if (!${Defined[${MacroVar}]} && ${VarType.Length}) /declare ${MacroVar} ${VarType} outer
   /declare IniString string local ${Ini["${MyIni}",${IniSection},${IniVar},NOTFOUND]}
   /varset ${MacroVar} ${IniString}
   /if (${IniString.Equal["NOTFOUND"]}) {
      /varset ${MacroVar} ${IniValue}
      /ini "${MyIni}" "${IniSection}" "${IniVar}" "${${MacroVar}}"
   }
/return


Sub Event_CastRst
   /varset CastResult RESISTED
/return


Sub Event_CastStn
   /varset CastResult STUNNED
/return


Sub Event_CastImm(Line)
   /if (${Line.Find[to the stun portion]}) /return
   /varset CastResult IMMUNE
/return


Sub Event_CharmBroke
   /bc LOST PET - LOST PET
   /if (!${PetCast.Find[charm ]} || !${DoPet}) /return
   /if (${PetAssist}) /varset PetAssist 1
   /if (!${Me.Mount.ID} && ${Me.Sitting}) /sit off
   /varset ADCheckTimer 0
   /call ADDebuffCheck
/return


Sub PMCheck(PM)
   /if (${MBPause}) /return
   /if (${MBDebug}) /echo Macro.RunTime:${Macro.RunTime} - Sub:PMCheck:${PM}
   /declare i int local
   /if (!${Defined[PetCastSM]}) /declare PetCastSM bool outer FALSE
   /declare WaitTimer timer local
   /if (${PM.Equal[m]}&&!${Me.Mount.ID}&&!${Me.FreeBuffSlots}) /return
   /if (!${PM.Length}) /return FALSE
   /declare PMCast string local ${If[${PM.Equal[p]},${PetCast},${If[${PM.Equal[m]},${MountCast},]}]}
   /if (${PMCast.Length}<5) /return FALSE
   /declare tPSpellID int local
   /declare sGem string local ${PMCast.Arg[2,|]}
   /if (${sGem.Find[-invis]}) {
      /if (${Me.Invis}) /return
      /varset sGem ${PMCast.Arg[2,|].Arg[1]}
   }
   /declare tGem int local ${If[${Select[${PMCast.Arg[2,|].Mid[4,${Math.Calc[${sGem.Length}-3]}]},1,2,3,4,5,6,7,8,9,10,11,12,13]},${PMCast.Arg[2,|].Mid[4,${Math.Calc[${sGem.Length}-3]}]},0]}
   /declare tSpellID int local ${If[${tGem},${Spell[${PMCast.Arg[1,|]}].ID},${If[${sGem.Equal[item]},${FindItem[=${PMCast.Arg[1,|]}].Spell.ID},${If[${sGem.Equal[alt]},${Me.AltAbility[${PMCast.Arg[1,|]}].Spell.ID},0]}]}]}
   /if (${PM.Equal[p]} && ${PMCast.Arg[3,|].Find[sm]} && ${Me.AltAbilityReady[Companion's Suspension]} && !${PetCastSM}) {
      /varset PetCastSM TRUE
      /casting "Companion's Suspension|alt" -maxtries|5
      /delay 10s ${Me.Pet.ID}
      /if (${Me.Pet.ID}) /goto :tJump
      /if (!${Me.Pet.ID}) {
         /bc Problem using minion AA, will attempt to create a new pet
         /varset PMCast ${PMCast.Arg[1,|]}|${PMCast.Arg[2,|]}
      }
   }
   /if (!${tSpellID} || !${sGem.Length}) /return
   /if (${tGem} && ${Me.CurrentMana}<${Spell[${tSpellID}].Mana}) /return
   /bc ${If[${PM.Equal[p]},Making Pet,${If[${PM.Equal[m]},Getting Mount,]}]}, could be a minute, please wait...
   /if (${Me.Casting.ID}) /delay 3s !${Me.Casting.ID}
   /if (${tGem} && ${Me.Gem[${tGem}].ID}!=${tSpellID}) {
      /varset tPSpellID ${Spell[${Me.Gem[${tGem}]}].ID}
      /memorize "${PMCast.Arg[1,|]}|gem${tGem}"
   }
   /if (${PMCast.Arg[2,|].Find[gem]} && !${Me.Gem[${PMCast.Arg[1,|]}]}) /delay 4s ${Me.Gem[${PMCast.Arg[1,|]}]}
   /varset WaitTimer 250
:waitloop
   /if (${MBPause} || ${Me.State.Equal[feign]}) /return
   /if (${PMCast.Arg[2,|].Find[gem]} && !${Me.Gem[${PMCast.Arg[1,|]}]}) /return
   /if (${DoHeals} && ${AHCount}) /call AHHealCheck
   /doevents
   /if ((!${Spawn[${NetBots[${TankName}].TargetID}].Type.Equal[npc]} && ${ACState} || ${Spawn[${NetBots[${TankName}].TargetID}].Type.Equal[npc]} && !${ACState})) /call AdvCombat
   /if (${DoDebuffs} && ${ADCount} && !${ADCheckTimer} && (${ADMobCount}||${APTargetID})) /call ADDebuffCheck
   /if (${WaitTimer} && (!${Cast.Ready[${PMCast.Arg[1,|]}|${PMCast.Arg[2,|]}]} && !${Me.Class.ShortName.Equal[brd]} || ${Me.Class.ShortName.Equal[brd]} && !${Me.Gem[${PMCast.Arg[1,|]}]})) /goto :waitloop

   /varset MBCurrentSpell PM|1|${Me.ID}
   /if (${Cast.Ready}) /call MQ2Cast "${PMCast.Arg[1,|]}" ${sGem} 10s CastCheck -targetid|${Me.ID} -maxtries|2 ${If[${PM.Equal[p]} && ${PetFoci.Length}>2,-bandolier|${PetFoci},]}
   /delay 30s ${If[${PM.Equal[p]},${Me.Pet.ID},${If[${PM.Equal[m]},${Me.Mount.ID},0]}]} || ${PM.Equal[m]} && !${Me.Casting.ID} && ${Cast.Result.Equal[CAST_OUTDOORS]}
:tJump
   /if (${Cast.Result.Equal[CAST_OUTDOORS]} && !${Ini["${MBCommon}",RestrictedList,${Zone.ID}].Find[mount|]} && ${PM.Equal[m]}) /ini "${MBCommon}" RestrictedList ${Zone.ID} ${If[${Ini["${MBCommon}",RestrictedList,${Zone.ID}].Length},${Ini["${MBCommon}",RestrictedList,${Zone.ID}]},]}mount|
   /if (!${Me.Pet.ID} && ${PM.Equal[p]} || !${Me.Mount.ID} && ${PM.Equal[m]}) {
      /bc Must have been a problem with ${If[${PM.Equal[p]},making the pet,${If[${PM.Equal[m]},getting the mount,]}]}, setting it to FALSE
      /docommand ${If[${PM.Equal[p]},/varset DoPet FALSE,${If[${PM.Equal[m]},/varset DoMount FALSE,/echo PMCheck problem]}]}
   }
   /if (${PM.Equal[p]} && ${Me.Pet.ID}) {
      /if (${Select[${Me.Class.ShortName},wiz]}) { 
         /bc Being a Wizard, I'm dumping pet, setting DoPet FALSE
         /pet get lost
         /varset DoPet FALSE
      }
      /if (${Me.Pet.ID} && !${PetAggro}) /pet taunt off
   }
   /varset MBCurrentSpell
   /if (${tPSpellID}) {
      /delay 3s ${Cast.Status.Equal[i]}
      /memorize "${Spell[${tPSpellID}]}|gem${tGem}"
      /delay 5s ${Me.Gem[${tGem}].ID}==${tPSpellID}
   }
/return


Sub AFK
   /docommand ${If[(${AFKMessage.Length} && !${AFKMessage.Equal[AFK Message]}),/afk ${AFKMessage},/afk]}
/return


Sub Event_Broke
   /if (${Defined[Broke]}) /varset Broke TRUE
   /echo Error: You are out of money!
   /docommand /${BeepCommand}
/return TRUE


Sub Event_SummonBoth
   /if (!${DoFW} || (${Me.Invis} && !${Me.Class.ShortName.Equal[rog]})) /return
   /call Event_SummonFood
   /autoinv
   /call Event_SummonDrink
   /autoinv
/return


Sub Event_SummonDrink
   /if (!${DoFW} || ${SumDrink.Length}<3 || ${MBPause} || (${Me.Invis} && !${Me.Class.ShortName.Equal[rog]})) /return
   /if (${Me.Casting.ID}) /delay 5s !${Me.Casting.ID}
   /call MQ2Cast "${SumDrink.Arg[1,|]}" ${SumDrink.Arg[2,|]} 5s CastCheck
   /delay 10s ${Cursor.ID}
   /call ClearCursor
/return


Sub Event_SummonFood
   /if (!${DoFW} || ${SumFood.Length}<3 || ${MBPause} || (${Me.Invis} && !${Me.Class.ShortName.Equal[rog]})) /return
   /if (${Me.Casting.ID}) /delay 5s !${Me.Casting.ID}
   /call MQ2Cast "${SumFood.Arg[1,|]}" ${SumFood.Arg[2,|]} 5s CastCheck
   /delay 10s ${Cursor.ID}
   /call ClearCursor
/return


Sub Event_Invite((string eLine,string eType)
   /if (!${Select[${eType},group,raid]}) /return
   /if (${Select[${eLine.Arg[1]},${MasterList}]} || ${NetBots[${eLine.Arg[1]}].ID}) {
      /if (!${Me.Class.ShortName.Equal[brd]} && ${Me.Casting.ID}) /delay 10s !${Me.Casting.ID}
      /invite
      /delay 1s
      /if (${Window[ConfirmationDialogBox].Open}) /notify ConfirmationDialogBox CD_Yes_Button leftmouseup
   }
/return


Sub Event_DelayOnZone(ZoneText)
   /if (${ZoneText.Find[Drunken Monkey]} || ${ZoneText.Find[effects]}) /return
   /echo Zoning Detected.
   /declare a int local
   /declare b int local
   /varset DoDebuffs FALSE
   /varset DoMelee FALSE
   /varset DoPull FALSE
   /if (${FollowFlag}) {
      /varset FollowFlag 10
      /stick off
   }
   /if (${Select[${MakeCamp},on]} || ${CampStatus}) {
      /makecamp off
      /varset CampStatus 0
   }
   /if (${PetCast.Find[charm ]}) /if (${ADCharmID}) /varset ADCharmID 0
   /if (${PetAssist}) /varset PetAssist 1
   /if (${Melee.Combat}) /melee reset
   /if (${Defined[ADMobTimer]}) {
      /for a 1 to ${ADMobCount}
         /varset ADMobList[${a}] 0
         /for b 1 to ${ADCount}
            /varset ADMDL[${a},${b}] 0
         /next b
      /next a
      /varset ADMobCount 0
      /varset ACMATarget 0
      /varset ACState 0
   }
   /delay 600s ${Me.ID}
   /delay 5s
   /if (${DoAfk} && !${Me.AFK}) /call AFK
   /varset ACStartY ${Me.Y}
   /varset ACStartX ${Me.X}
   /varset ACState 0
   /varset ACMATarget 0
   /varset ADMobCount 0
   /call ProcessExcludeList zone
   /varset ZoneName ${Zone.Name}
   /if (${ADCount}) {
      /for a 1 to ${ADCount}
         /if (${Ini["${MBCommon}","${Spell[${ADSpell[${a}]}].Name}","${Zone.ShortName}Immune"].Find[|]}) {
            /call LoadINIVar "${Spell[${ADSpell[${a}]}].Name}" "${Zone.ShortName}Immune" " " "Imm${Spell[${ADSpell[${a}]}].ID}" "${MBCommon}" string
         } else /if (${Defined[Imm${Spell[${ADSpell[${a}]}].ID}]}) {
            /if (${Imm${Spell[${ADSpell[${a}]}].ID}.Length}) /varset Imm${Spell[${ADSpell[${a}]}].ID}
         }
      /next a
   }
   /doevents flush
   /squelch /alert clear 25
/return


Sub Event_Slain
   /bc - DEATH DETECTED, putting all functions on hold. -
   /bc - You must issue the appropriate command to restart each function or restart the macro. -
   /varset DoDebuffs FALSE
   /varset DoEvents FALSE
   /varset DoHeals FALSE
   /varset DoMelee FALSE
   /varset DoBuffs FALSE
   /varset DoCures FALSE
   /varset DoPull FALSE
   /consent group
   /delay 3s
   /consent raid
   /delay 3s
   /consent guild
   /stick off
   /varset FollowFlag 0
   /keypress left
   /keypress right
   /doevents flush
   /call Wait4Rez
   /delay 600s ${Me.ID}
   /delay 6s
   /delay 20s ${Cast.Status.Equal[i]}
/return


Sub Wait4Rez
   /declare rezPct int local
   /declare callFlag bool local
   /declare RezWaitTimer timer ${RezGiveUpSeconds}s
   /stick off
   /makecamp off
   /delay 5s
   /if (!${DoRez}) {
      /bc DoRez is FALSE. Taking no actions.
      /return
   }
   /if (${RezMinPct}>100 && !${RezAcceptCall}) {
      /bc Rez settings do not allow for auto-accepting of rez/call. Taking no actions.
      /return
   }
   /echo Awaiting rez
   /if (${RezMinPct}<101) /echo Auto-accepting rez of ${RezMinPct}% and above
   /if (${RezAcceptCall}) /echo Auto-accepting Call to Corpse for rez later
:checkforrez 
   /if (!${Window[ConfirmationDialogBox].Open} && !${RezWaitTimer}) {
      /bc Rez timer expired. Camping to give time to rez me later.
      /ini "${IniFile}" "Settings" "DeathSlot" "TRUE"
      /camp desk
      /delay 5s
      /end
   }
   /delay 10s ${Window[ConfirmationDialogBox].Open} 
   /if (!${Window[ConfirmationDialogBox].Open}) /goto :checkforrez
   /delay 1s
   /if (${Bool[${Window[ConfirmationDialogBox].Child[CD_TextOutput].Text.Find["(0 percent)"]}]}) {
       /varset rezPct 0
       /varset callFlag FALSE
   } else /if (${Bool[${Window[ConfirmationDialogBox].Child[CD_TextOutput].Text.Find["(10 percent)"]}]}) { 
       /varset rezPct 10
       /varset callFlag FALSE
   } else /if (${Bool[${Window[ConfirmationDialogBox].Child[CD_TextOutput].Text.Find["(20 percent)"]}]}) { 
       /varset rezPct 20
       /varset callFlag FALSE
   } else /if (${Bool[${Window[ConfirmationDialogBox].Child[CD_TextOutput].Text.Find["(35 percent)"]}]}) { 
       /varset rezPct 35
       /varset callFlag FALSE
   } else /if (${Bool[${Window[ConfirmationDialogBox].Child[CD_TextOutput].Text.Find["(50 percent)"]}]}) { 
       /varset rezPct 50
       /varset callFlag FALSE
   } else /if (${Bool[${Window[ConfirmationDialogBox].Child[CD_TextOutput].Text.Find["(60 percent)"]}]}) { 
       /varset rezPct 60
       /varset callFlag FALSE
   } else /if (${Bool[${Window[ConfirmationDialogBox].Child[CD_TextOutput].Text.Find["(75 percent)"]}]}) { 
       /varset rezPct 75
       /varset callFlag FALSE
   } else /if (${Bool[${Window[ConfirmationDialogBox].Child[CD_TextOutput].Text.Find["(90 percent)"]}]}) { 
       /varset rezPct 90
       /varset callFlag FALSE
   } else /if (${Bool[${Window[ConfirmationDialogBox].Child[CD_TextOutput].Text.Find["(96 percent)"]}]}) { 
       /varset rezPct 96
       /varset callFlag FALSE
   } else /if (${Bool[${Window[ConfirmationDialogBox].Child[CD_TextOutput].Text.Find["(100 percent)"]}]}) {
       /varset rezPct 100
       /varset callFlag FALSE
   } else /if (${Bool[${Window[ConfirmationDialogBox].Child[CD_TextOutput].Text.Find["you will still be able to get a resurrection later"]}]}) {
       /varset rezPct 0
       /varset callFlag TRUE
   } else {
       /varset rezPct 0
       /varset callFlag FALSE
   }
   /if (${rezPct}>=${RezMinPct} || (${RezAcceptCall} && ${callFlag})) /goto :acceptrez
   /if (${callFlag}) {
      /echo Declining call to corpse. Continuing to wait.
   } else {
      /echo Declining ${rezPct}% rez. Continuing to wait.
   }
   /nomodkey /notify ConfirmationDialogBox No_Button leftmouseup	
   /goto :checkforrez
:acceptrez
   /if (${callFlag}) {
      /echo Accepting call to corpse.
   } else {
      /echo Accepting ${rezPct}% rez.
   }
   /nomodkey /notify ConfirmationDialogBox Yes_Button leftmouseup 
   /delay 2s 
   /if (${Window[RespawnWnd].Open}) /nomodkey /notify RespawnWnd RW_OptionsList listselect 2  
   /delay 1s 
   /nomodkey /notify RespawnWnd RW_SelectButton leftmouseup 
   /delay 2s 
   /ini "${IniFile}" "Settings" "DeathSlot" "FALSE"
   /delay 3s !${Window[RespawnWnd].Open}
/return


Sub ClearCursor
|   /if (${Window[InventoryWindow].Open} && !${DoCombines}) /return
   /if (${Window[InventoryWindow].Open} && !${DoCombines} && ${Foreground} ) /return   
|   /if (${Window[InventoryWindow].Open} && !${DoCombines}) {
|      /if (${Bool[${Plugin[MQ2FPS]}]} {
|	     /if (${Foreground}) /return
|   } 
   /declare x int local
   /declare tTime int local ${Macro.RunTime}
:auto_inv
   /if (${Cursor.ID}) {
      /if (${Cursor.Container}) {
         /for x 1 to 10
            /if (!${InvSlot[pack${x}].Item.ID}) /nomodkey /itemnotify pack${x} leftmouseup
         /next x
         /if (${Cursor.Container}) /return
      } else {
         /call ItemSort
      }
      /if (${Macro.RunTime}<${tTime}+10) /goto :auto_inv
   }
/return

Sub Buy(string itemToBuy, int amount, string vendor)
   /if (!${Defined[Broke]}) /declare Broke bool outer FALSE
   /if (!${vendor.Length} || !${itemToBuy.Length} || ${amount}==0 || ${Broke}) /return FALSE
   /echo Buying ${itemToBuy} from ${vendor}!
   /declare QTY int local 0
   /declare l2 int local 0
   /declare mItems int local 0
   /tar npc ${vendor}
   /delay 1s ${Target.ID}==${Spawn[Merchant].ID}
   /nomodkey /click right target
   /delay 5s ${Window[MerchantWnd].Open}
   /varset QTY ${Math.Calc[${amount}-${FindItemCount[=${itemToBuy}]}]}
   /if (${FindItemCount[=${itemToBuy}]}>= ${amount}) {
      /echo Done!
      /return TRUE
   }
:itemCheck
   /varset mItems ${Merchant.Items}
   /delay 2
   /if (${mItems}<${Merchant.Items}) /goto :itemCheck 
 	
   /if (!${Window[MerchantWnd].Open}) /return FALSE
   /varset l2 ${Window[MerchantWnd].Child[itemList].List[=${itemToBuy},2]}
   /if (!${l2}) {
      /echo couldn't find ${itemToBuy}
      /return FALSE
   } else {
      /notify MerchantWnd itemList listselect ${l2}
      /notify MerchantWnd itemList leftmouse ${l2}
      /delay 2
   }
   /echo Buying ${itemToBuy} Till I get ${amount}
:Loop
   /if (${Broke}) /return FALSE
   /if (${QTY}>19) {
      /buyitem 20
      /delay 1s
      /doevents
      /varset QTY ${Math.Calc[${amount}-${FindItemCount[=${itemToBuy}]}]}
      /goto :Loop
   } else {
      /if (${QTY}>0 && ${QTY}<20) {
         /buyitem ${QTY}
         /delay 1s
         /doevents
         /varset QTY ${Math.Calc[${amount}-${FindItemCount[=${itemToBuy}]}]}
      }
   }
   /if (${QTY}==0) {
      /nomodkey /notify MerchantWnd MW_Done_Button leftmouseup
      /echo Done!
      /return TRUE
   }
   /goto :Loop
/return TRUE


Sub Sell(ItemToSell)
   /declare OldMoney int local
   /call ClearCursor
   /echo Selling - ${ItemToSell} 
:tFinding
   /if (!${ItemToSell.Length} || !${Window[MerchantWnd].Open}) /return
   /if (${FindItemCount[=${ItemToSell}]} ) {
      /nomodkey /itemnotify ${FindItem[=${ItemToSell}].InvSlot} leftmouseup
      /if (${SelectedItem.ID}==${FindItem[=${ItemToSell}].ID} && ${SelectedItem.Value}) {
         /delay 1s
         /varset OldMoney ${Me.Cash}
         /nomodkey /shift /notify MerchantWnd MW_Sell_Button leftmouseup
         /delay 2s (${Me.Cash}!=${OldMoney})
      }
      /goto :tFinding
   }
/return


Sub SellCheck(VendorName)
   /call ClearCursor
   /if (!${VendorName.Length}) /return FALSE
   /squelch /target ${VendorName}
   /delay 3s ${Target.Name.Find[${VendorName}]} || ${Target.CleanName.Find[${VendorName}]}
   /if (!${Target.Name.Find[${VendorName}]} && !${Target.CleanName.Find[${VendorName}]} || ${Spawn[${Target.ID}].Distance3D}>30) /return FALSE
   /echo selling
|  /squelch /face
   /click right target
   /delay 2s ${Window[MerchantWnd].Open}
   /declare tPack int local
   /declare tItem int local
   /declare mItem int local
   /declare mItemName string local
   /declare sItemName string local
   /nomodkey /keypress OPEN_INV_BAGS
:loop
   /if (${Window[TradeSkillWnd].Open}) /notify TradeSkillWnd ExperimentButton leftmouseup
   /delay 4 !${Window[TradeSkillWnd].Open}
   /if (${Window[TradeSkillWnd].Open}) /goto :loop
   /varset mItem ${Merchant.Items}
   /delay 2s
   /if (${Merchant.Items}!=${mItem}) /goto :loop
   /for tPack 1 to 10
      /if (${Me.Inventory[pack${tPack}].Container}) {
         /for tItem 1 to ${Me.Inventory[pack${tPack}].Container}
            /varset sItemName ${Me.Inventory[pack${tPack}].Item[${tItem}].Name}
            /if (${sItemName.NotEqual[NULL]} && ${Ini["${LootIni}","${sItemName.Left[1]}","${sItemName}"].Equal[Sell]}) /call Sell "${sItemName}"
         /next tItem
         /if (${Window[Pack${tPack}].Open}) /nomodkey /itemnotify pack${tPack} rightmouseup
         /delay 3s !${Window[Pack${tPack}].Open}
      } else {
         /varset sItemName ${Me.Inventory[pack${tPack}].Name}
         /if (${sItemName.NotEqual[NULL]} && ${Ini["${LootIni}","${sItemName.Left[1]}","${sItemName}"].Equal[Sell]}) /call Sell "${sItemName}"
      }
   /next tPack
   /for mItem 1 to ${Merchant.Items}
       /varset mItemName ${Merchant.Item[${mItem}].Name}
       /if (${Ini["${LootIni}","${mItemName.Left[1]}","${mItemName}"].EqualCS[KEEP]}) /bc ${mItemName} found on ${VendorName}
   /next mItem
   /nomodkey /notify MerchantWnd MW_Done_Button leftmouseup
   /squelch /target clear
/return TRUE


Sub GetBankItem(ItemName,InStacks,int Quant)
   /if (!${ItemName.Length} || !${Window[BankWnd].Open} && !${Window[BigBankWnd].Open}) {
      /echo Error! Usage - Bank Window must be Open - getbank <ItemName> <Quantity> <InStacks>
      /return
   }
   /if (!${InStacks.Length}) /varset InStacks TRUE
:CursorCheck
   /if (${Cursor.ID}) /autoinv
   /if (${Cursor.ID}) /goto :CursorCheck
   /declare BankSlots int local
   /declare i int local
   /declare j int local
   /declare Gathered int local
   /if (${Window[BankWnd].Open}) {    
      /varset BankSlots 16 
   } else { 
      /if (${Window[BigBankWnd].Open}) { 
         /varset BankSlots 24 
      }
   }
   /if (${Category.Find[${ItemName}]}) /echo ${ItemName} is a Category
   /if (!${FindItemBankCount[=${ItemName}]} && !${Category.Find[${ItemName}]}) { 
      /echo Can't Find ${ItemName} in Bank.
      /return
   }
   /call OpenAllBankContainers ${BankSlots}
   /for i 1 to ${BankSlots}
      /for j 1 to ${Me.Bank[${i}].Container}
         /if ((${Me.Bank[${i}].Item[${j}].Name.Equal[${ItemName}]} || ${Category.Find[${ItemName}]} && ${Ini["${LootIni}",${Me.Bank[${i}].Item[${j}].Name.Left[1]},${Me.Bank[${i}].Item[${j}].Name}].Find[${ItemName}]}) && !${FindItem[=${Me.Bank[${i}].Item[${j}].Name}].FreeStack} && !${Me.FreeInventory}) /echo No Room for ${Me.Bank[${i}].Item[${j}].Stack} - ${Me.Bank[${i}].Item[${j}].Name}
         /if ((${Me.Bank[${i}].Item[${j}].Name.Equal[${ItemName}]} || ${Category.Find[${ItemName}]} && ${Ini["${LootIni}",${Me.Bank[${i}].Item[${j}].Name.Left[1]},${Me.Bank[${i}].Item[${j}].Name}].Find[${ItemName}]}) && (${FindItem[=${Me.Bank[${i}].Item[${j}].Name}].FreeStack} || ${Me.FreeInventory})) {
            /echo Getting ${Me.Bank[${i}].Item[${j}].Stack} - ${Me.Bank[${i}].Item[${j}].Name} From Bank
            /if (${Bool[${InStacks}]}) {
               /shiftkey /itemnotify in bank${i} ${j} leftmouseup
            } else {
               /ctrlkey /itemnotify in bank${i} ${j} leftmouseup
            }
            /delay 2s ${Cursor.ID}		 		
            /call ClearCursor
:CursorClear
            /if (${Cursor.ID}) /autoinv
            /if (${Cursor.ID}) /goto :CursorClear
            /varset Gathered Gathered+1
            /if (${Quant}>0) { 
               /if (${Gathered}>=${Quant}) {
                  /echo Got ${Gathered} - ${ItemName}'s from Bank.
                  /return
               }
            }
         }
      /next j
   /next i
   /echo Done getting Items  with names matching ${ItemName} from Bank.
/return


Sub OpenAllBankContainers(int BankSlots)
   /declare i int local 1
   /for i 1 to ${BankSlots} 
      /if (${Int[${Me.Bank[${i}].Container}]}) { 
         /if (!${Window[bank${i}].Open}) { 
            /nomodkey /itemnotify bank${i} rightmouseup 
            /delay 5s ${Window[bank${i}].Open} 
         } 
         /if (!${Window[bank${i}].Open}) /echo ERROR -- Possible problem opening bank container at # ${i}. 
         /delay 2 
      } 
   /next i 
/return 1 


|woobs - This needs to be rewritten
Sub autobank(string ItemsToBank)
   /echo AutoBanking - ${ItemsToBank}
   /declare a int local 0
   /declare PackNum int local 1
   /declare SlotNum int local 1
   /declare iCount int local 0
   /declare tItem string local
   /declare tItemID int local 0
   /declare slotid int local 0

|woobs - these nested /next's may be a problem
   /keypress OPEN_INV_BAGS
   /delay 1s ${Window[Pack8].Open}
   /for a 1 to ${Math.Calc[${ItemsToBank.Count[|]}+1]}
       /if (${Category.Find[${ItemsToBank.Arg[${a},|]}]}) {
| Search for items marked as Category in your loot.ini file...
          /echo Searching inventory for items marked ${ItemsToBank.Arg[${a},|]} in your loot file.
          /for PackNum 1 to 8
             /for SlotNum 0 to ${InvSlot[pack${PackNum}].Item.Container}
                /if (!${Ini["${LootIni}",${InvSlot[pack${PackNum}].Item.Item[${SlotNum}].Name.Left[1]},"${InvSlot[pack${PackNum}].Item.Item[${SlotNum}].Name}"].Find[${ItemsToBank.Arg[${a},|]}]}) {
                   /next SlotNum
                   /next PackNum
                   /next a
                } else /if (${Ini["${LootIni}",${InvSlot[pack${PackNum}].Item.Item[${SlotNum}].Name.Left[1]},"${InvSlot[pack${PackNum}].Item.Item[${SlotNum}].Name}"].Find[${ItemsToBank.Arg[${a},|]}]}) {
                   /echo Found matching item at pack ${PackNum} slot ${SlotNum}
                   /varset tItemID ${InvSlot[pack${PackNum}].Item.Item[${SlotNum}].ID}
                   /varset tItem ${InvSlot[pack${PackNum}].Item.Item[${SlotNum}].Name}
                   /echo Item is ${tItemID} : ${tItem}
                }
                /if (!${tItemID}) /next a
                /varset slotid ${FindItem[${tItem}].InvSlot}
                /if (!${slotid}) /next a 
                /call bankit ${slotid}
             /next SlotNum
          /next PackNum	  
       } else {
:ItemFind
          /varset tItemID ${FindItem[=${ItemsToBank.Arg[${a},|]}].ID}
          /varset tItem ${FindItem[=${ItemsToBank.Arg[${a},|]}].Name}
          /if (!${tItemID}) /next a
          /varset slotid ${FindItem[${tItem}].InvSlot}
          /if (!${slotid}) /next a 
          /call bankit ${slotid}
          /if (${FindItemCount[=${ItemsToBank.Arg[${a},|]}]}>0) /goto :ItemFind
          /next a
       }
       /echo Done moving items to bank.
   /next a
/return


Sub bankit(int slotid)
   /declare tItemID int local ${InvSlot[${slotid}].Item.ID}
    
   /nomodkey /shiftkey /itemnotify ${slotid} leftmouseup 
   /delay 10 ${Cursor.ID}==${tItemID}
:AutoIt
   /if (${Window[BankWnd].Open}) {    
      /notify BankWnd bw_autobutton leftmouseup
   } else { 
      /if (${Window[BigBankWnd].Open}) { 
         /notify BigBankWnd bigb_autobutton leftmouseup 
      }
   } 
   /delay 5 !${Cursor.ID}
   /if (${Cursor.ID}) /goto :AutoIt
/return


Sub GiveCheck(TargetName,ItemNames,bool InStacks,ItemCount)
|  /echo ${TargetName} : ${ItemNames} : ${InStacks} : ${ItemCount}
   /if (!${TargetName.Length} || !${Spawn[${TargetName}].ID} || !${ItemNames.Length}) /return FALSE
   /declare tID int local ${Spawn[${TargetName}].ID}
   /declare a int local
   /declare b int local
   /declare NumItems int local 1
   /declare x int local ${Me.X}
   /declare y int local ${Me.Y}
   /declare tGC bool local FALSE
   /declare tPack int local
   /declare tItem int local
   /declare tItemID int local
   /declare tItemName string local
   /declare sItemName string local
   /declare iCount int local
   /call ClearCursor
:Cursorloop
   /if (${Cursor.ID}) /autoinv
   /if (${Cursor.ID}) /goto :CursorLoop
|  /if (${Spawn[${TargetName}].Type.Equal[npc]}) /varset InStacks FALSE
   /if (${Target.ID}!=${tID}) {
      /squelch /target id ${tID}
      /delay 1s ${Target.ID}==${tID}
   }
|woobs - These goto's are bad
   /if (!${Target.ID}) /goto :tEnd
   /if (${Target.Distance3D}>12) {
      /call MBMoveTo ${Target.Y} ${Target.X}
      /if (!${Macro.Return}) {
         /echo problem getting to ${TargetName}, Sub GiveCheck
         /goto :tEnd
      }
   }
   /nomodkey /keypress OPEN_INV_BAGS
   /delay 1s ${Window[Pack10].Open}
   /for a 1 to ${Math.Calc[${ItemNames.Count[|]}+1]}
      /if (${ItemCount.Arg[${a},|].Find[all]}) {
         /varset NumItems ${FindItemCount[=${ItemNames.Arg[${a},|]}]}
      }
      /if (${ItemCount.Arg[${a},|].Length} && !${ItemCount.Arg[${a},|].Find[all]}) /varset NumItems ${ItemCount.Arg[${a},|]}
      /for b 1 to ${NumItems}
         /if (${Category.Find[${ItemNames.Arg[${a},|]}]}) {
| Search for items marked as Category in your loot.ini file...
            /echo Searching inventory for items marked ${ItemNames.Arg[${a},|]} in your loot file.
            /for tPack 1 to 10
               /if (${Me.Inventory[pack${tPack}].Container}) {
                  /for tItem 1 to ${Me.Inventory[pack${tPack}].Container}
                     /varset tItemName ${Me.Inventory[pack${tPack}].Item[${tItem}].Name}
                     /if (${tItemName.NotEqual[NULL]} && ${Ini["${LootIni}","${tItemName.Left[1]}","${tItemName}"].Find[${ItemNames.Arg[${a},|]}]}) {
                        /echo Found matching item - ${tItemID} : ${tItemName}
                        /varset tItemID ${Me.Inventory[pack${tPack}].Item[${tItem}].ID}
                        /goto :founditem
                     }
                  /next tItem
               } else {
                  /varset tItemName ${Me.Inventory[pack${tPack}].Name}
                  /if (${tItemName.NotEqual[NULL]} && ${Ini["${LootIni}","${tItemName.Left[1]}","${tItemName}"].Find[${ItemNames.Arg[${a},|]}]}) {
                     /echo Found matching item - ${tItemID} : ${tItemName}
                     /varset tItemID ${Me.Inventory[pack${tPack}].Item[${tItem}].ID}
                     /goto :founditem
                  }
               } 
            /next tPack        
         } else {
            /varset tItemID ${FindItem[=${ItemNames.Arg[${a},|]}].ID}
            /varset tItemName ${FindItem[=${ItemNames.Arg[${a},|]}].Name}
         }
         /if (!${tItemID}) {
            /echo Can't find ${ItemNames.Arg[${a},|]} to give
            /goto :tNextItem
         }
:founditem
         /varset MiscCheckTimer 15s
:loop
         /if (!${MiscCheckTimer}) {
            /echo Problem with GiveCheck - MiscCheckTimer
            /goto :tEnd
         }
|        /if (!${tItemID}) /goto :tEnd
         /if (${InStacks} && ${FindItem[=${tItemName}].InvSlot}) {
            /nomodkey /shift /itemnotify ${FindItem[=${tItemName}].InvSlot} leftmouseup
         } else /if (!${InStacks} && ${FindItem[=${tItemName}].InvSlot}) {
            /nomodkey /ctrl /itemnotify ${FindItem[=${tItemName}].InvSlot} leftmouseup
         }
         /delay 2s ${Cursor.ID}==${tItemID}
         /if (${Cursor.ID}!=${tItemID}) /goto :loop
         /if (${Target.ID}) /click left target
         /delay 10s (${Window[GiveWnd].Open} || ${Window[TradeWnd].Open})
         /if (!${Target.Type.Equal[pc]} && !${Window[GiveWnd].Open} || ${Target.Type.Equal[pc]} && !${Window[TradeWnd].Open}) {
            /echo failure
            /varset MiscCheckTimer 0
            /goto :loop
         }
:tEnd
         /varcalc iCount ${iCount}+1
         /if (${Window[GiveWnd].Open}) {
            /if (${iCount}<4) /goto :tNextItem 
            /notify GiveWnd GVW_Give_Button LeftMouseUp
            /delay 5s !${Window[GiveWnd].Open}
            /if (${Target.ID}!=${tID}) {
               /squelch /target id ${tID}
               /delay 1s ${Target.ID}==${tID}
            }
         }
         /if (${Window[TradeWnd].Open}) {
            /if (${iCount}<8) /goto :tNextItem   	
            /if (!${Window[TradeWnd].MyTradeReady}) {
               /delay 2
               /notify TradeWnd TRDW_Trade_Button LeftMouseUp
            }
            /delay 15s !${Window[TradeWnd].Open}
         } else {
            /if (${Window[GiveWnd].Open}) {
               /delay 2
               /notify GiveWnd GVW_Give_Button LeftMouseUp
            }
            /delay 5s !${Window[GiveWnd].Open}
         }
         /varset iCount 0
:tNextItem
      /next b
   /next a
   /if (${iCount}) {
      /if (${Window[GiveWnd].Open}) {
         /notify GiveWnd GVW_Give_Button LeftMouseUp
         /delay 5s !${Window[GiveWnd].Open}
         /if (${Target.ID}!=${tID}) {
            /squelch /target id ${tID}
            /delay 1s ${Target.ID}==${tID}
         }
      }
      /if (${Window[TradeWnd].Open}) {
         /if (!${Window[TradeWnd].MyTradeReady}) {
            /delay 2
            /notify TradeWnd TRDW_Trade_Button LeftMouseUp
         }
         /delay 15s !${Window[TradeWnd].Open}
      } else {
         /if (${Window[GiveWnd].Open}) {
            /delay 2
            /notify GiveWnd GVW_Give_Button LeftMouseUp
         }
         /delay 5s !${Window[GiveWnd].Open}
      } 
   }
   /keypress CLOSE_INV_BAGS
   /keypress esc
   /delay 1s
   /call MBMoveTo ${y} ${x}
/return ${tGC}


Sub MBScript(P0,P1,P2)
|  /echo ${P0} ${P1} ${P2}
  /if (${MBDebug}) /echo Macro.RunTime:${Macro.RunTime} - Sub:MBScript:${P0}
  /declare a int local
  /declare b int local
  /declare c int local
  /declare Timer timer local
  /declare sVarL string local
  /declare sVarC string local
  /declare sVarM string local
  /declare ScriptINI string local ${IniFile}
  /if (!${P0.Length}) {
    /echo Syntax Error
    /return
  }
  /if (${Select[${P0.Arg[1,-]},net,network]}) {
    /varset ScriptINI ${NetworkINI}
    /if (!${P0.Find[ ]}) {
      /varset P0 ${P0.Right[${Math.Calc[${P0.Length}-${P0.Find[-]}]}]}
    } else /varset P0 ${P0.Arg[2]}
  }
  /if (!${Defined[${P0}C]}) {
    /declare ${P0}C int outer ${Ini["${ScriptINI}",Script-${P0},Commands]}
    /if (!${${P0}C}) {
       /if (${P0.Equal[Defense]}) /return
       /declare IniString string local ${Ini["${ScriptINI}",Script-${P0},Commands,NOTFOUND]}
       /if (${IniString.Equal[NOTFOUND]}) {
         /ini "${ScriptINI}" "Script-${P0}" "Commands" "0"
         /ini "${ScriptINI}" "Script-${P0}" "C1" "/return"
         /if (${P0.NotEqual[MBStartup]}) /echo MBScript - INI Entry Made for: Script-${P0}
       }
       /return
    }
|   /if (${Ini["${ScriptINI}",Script-${P0},Commands]}==0) /return
    /declare ${P0}[${${P0}C}] string outer
    /for a 1 to ${${P0}C}
      /varset ${P0}[${a}] ${Ini["${ScriptINI}",Script-${P0},C${a}]}
    /next a
  }
| /if (${Ini["${ScriptINI}",Script-${P0},Commands]}==0) /return
| woobs - replace above if with below if to help not re-reading ini file
  /if (!${${P0}C}) /return 
  /varset Timer 15s
:Top
  /for a 1 to ${${P0}C}
    /varset sVarC
    /for b 1 to ${${P0}[${a}].Count[{]}
      /varset sVarL ${${P0}[${a}].Arg[${b},{]}
      /varset sVarC ${sVarC}${sVarL}${
    }
    /next b
    /varset sVarC ${sVarC}${${P0}[${a}].Arg[${b},{]}
    /if (!${${P0}[${a}].Count[{]}) /varset sVarC ${${P0}[${a}]}
    /varset sVarM
    /for b 1 to ${sVarC.Count[']}
      /if (${sVarC.Find['']}) {
        /varset sVarL ${sVarC.Mid[1,${Math.Calc[${sVarC.Find['']}-1]}]}"
| "
        /varset sVarC ${sVarC.Mid[${Math.Calc[${sVarC.Find['']}+2]},${sVarC.Length}]}
        /varset sVarM ${sVarM}${sVarL}
      }
    /next b
    /varset sVarM ${sVarM} ${sVarC}
    /if (${sVarM.Length}) /varset sVarC ${sVarM}
    /if (${EchoScripts}) /echo /docommand ${sVarC}
    /docommand ${sVarC}
    /if (!${Timer}) {
      /echo Script [${P0}] took too long, exiting
      /return
    }
  /next a
/return


Sub CastCall(CCMsgFrom,CCMsgTxt)
   /if (${MBDebug}) /echo Macro.RunTime:${Macro.RunTime} - Sub:CastCall:${CCMsgFrom}-${CCMsgTxt}
|return <cast_???> <AH,AD,AC,AB,AE,AQ> <Count Number 1-??Count> <spellid> <targetid>
|  /echo ${CCMsgFrom}-${CCMsgTxt}
   /declare a int local 1
   /declare b int local 1
   /declare c int local 1
   /declare d int local 1
   /declare e int local 1
   /if (${CCMsgTxt.Arg[1].Equal[coresist]}) {
      /declare tCastClass string local AD AH AB AQ AE
   } else {
      /declare tCastClass string local AH AD AB AQ AE
   } 
   /declare tCastCnt int local 0
   /declare tGrpMbr int local 0
   /declare sGem string local
   /declare sReturn string local
   /declare tGem int local 0
   /declare tSpell string local
   /declare tSpellID int local 0
   /declare tSpellRange int local 0
   /declare tCastTime timer local 0
   /declare tCT int local
   /declare tTargetID int local 0
   /declare CCtTargetID int local 0
   /declare tCCMsgTxt string local
   /declare ProjectIllu bool local FALSE
   /declare tFoci string local
   /declare pGem int local 0
   /declare CastCount int local 0
   /declare tCCMsgArg1 string local ${CCMsgTxt.Arg[1]}
   /declare tTwistGems string local

   /if (${Me.Class.ShortName.Equal[brd]}) {
      /varset tTwistGems ${Twist.List}
   }
   /declare sNBClnt string local ${NetBots.Client}
:AliasLoop
   /for a 1 to 5
      /if (${${tCastClass.Arg[${a}]}Count}) {
         /varset tCastCnt ${${tCastClass.Arg[${a}]}Count}
         /for b 1 to ${tCastCnt}
            /for c 1 to ${Math.Calc[${${tCastClass.Arg[${a}]}SpellAlias[${b}].Count[|]}+1]}
               /if (${CCMsgTxt.Arg[2].Equal[${${tCastClass.Arg[${a}]}SpellAlias[${b}].Arg[${c},|]}]}) {
                  /varset sReturn ${tCastClass.Arg[${a}]} ${b}
                  /varset tSpell ${${tCastClass.Arg[${a}]}Spell[${b}]}
                  /varset sGem ${${tCastClass.Arg[${a}]}Gem[${b}]}
                  /varset tFoci ${${tCastClass.Arg[${a}]}SpellFoci[${b}]}
                  /if (${tCastClass.Arg[${a}].NotEqual[ab]} && !${MasterList.Find[${CCMsgFrom}]} && ${CCMsgFrom.NotEqual[${Me.CleanName}]} && (!${MasterList.Equal[Netbots]} && ${NetBots.Client.Find[${CCMsgFrom}]})) /return
                  /if (${CCMsgTxt.Arg[3].Equal[off]}) {
                     /varset ${tCastClass.Arg[${a}]}TarCnt[${b}] 0
                     /bc ${CCMsgTxt.Arg[2]}-${tSpell} is Off
                     /if (${b}<${tCastCnt}) {
                        /varcalc CastCount ${CastCount}+1
                        /goto :AliasReturn
                     }
                     /if (${Me.Class.ShortName.NotEqual[brd]}) /return SUCCESS
                     /varset CCMsgTxt sung ${CCMsgTxt.Arg[2]}
                  }
                  /if (${CCMsgTxt.Arg[3].Equal[on]} && (${MasterList.Find[${CCMsgFrom}]} || ${MasterList.Find[Netbots]} && ${NetBots[${CCMsgFrom}].ID} || ${CCMsgFrom.Equal[${Me.CleanName}]})) {
                     /if (${${tCastClass.Arg[${a}]}NewFormat}) {
                        /call LoadINIVar ${tCastClass.Arg[${a}]}${b} TarCnt 0 ${tCastClass.Arg[${a}]}TarCnt[${b}] ${IniFile}
                     } else {
                        /call LoadINIVar ${If[${tCastClass.Arg[${a}].Equal[ah]},AdvHeal,${If[${tCastClass.Arg[${a}].Equal[ab]},AdvBuff,${If[${tCastClass.Arg[${a}].Equal[ad]},AdvDebuff,${If[${tCastClass.Arg[${a}].Equal[ae]},AdvEvent,${If[${tCastClass.Arg[${a}].Equal[aq]},AdvCure,NOTFOUND]}]}]}]}]} ${tCastClass.Arg[${a}]}TarCnt${b} 0 ${tCastClass.Arg[${a}]}TarCnt[${b}] ${IniFile}
                     }
                     /if (${${tCastClass.Arg[${a}]}TarCnt[${b}]}==0) /varset ${tCastClass.Arg[${a}]}TarCnt[${b}] 1
                     /bc ${CCMsgTxt.Arg[2]}-${tSpell} is On
                     /if (${b}<${tCastCnt}) {
                        /varcalc CastCount ${CastCount}+1
                        /goto :AliasReturn
                     }
                     /return SUCCESS
                  }
                  /if (${Select[${sGem},1,2,3,4,5,6,7,8,9,10,11,12,13]}) {
|/echo was Spell
                     /docommand ${If[${Me.Book[${Me.Book[${tSpell} Rk. III]}].ID},/varset tSpellID ${Me.Book[${Me.Book[${tSpell} Rk. III]}].ID},${If[${Me.Book[${Me.Book[${tSpell} Rk. II]}].ID},/varset tSpellID ${Me.Book[${Me.Book[${tSpell} Rk. II]}].ID},${If[${Me.Book[${Me.Book[${tSpell}]}].ID},/varset tSpellID ${Me.Book[${Me.Book[${tSpell}]}].ID},/multiline ; /bc Spell ${tSpell} Not found in your book.  Check your .ini settings.;/goto :CantCast]}]}]}
                     /if (${Me.Book[${Me.Book[${tSpell}]}].ID}!=${tSpellID}) /varset tSpell ${Spell[${tSpellID}].Name}
                     /if (${Me.CurrentMana}<${Spell[${tSpellID}].Mana}) /goto :CantCast
                     /varset tGem ${sGem}
                     /varcalc tCT ${Spell[${tSpellID}].MyCastTime}/100+40
                     /if (${Me.Casting.ID} && ${Me.Class.ShortName.NotEqual[BRD]}) /delay 2s !${Me.Casting.ID}
                     /if (${Me.Gem[${tGem}].ID}!=${tSpellID}) {
                        /if (${Me.Class.ShortName.Equal[BRD]}) /varset pGem ${Me.Gem[${tGem}].ID}
                        /squelch /memorize "${Spell[${tSpellID}]}|gem${tGem}"
                        /delay 5s ${Me.Gem[${tGem}].ID}==${tSpellID}
                        /if (${Me.Gem[${tGem}].ID}!=${tSpellID}) {
                           /bc CAST - FAILED MEMORIZE
                           /goto :CantCast
                        }
                        /varcalc tCT ${Spell[${tSpellID}].RecastTime}/100+${tCT}
                     } else {
                        /if (!${Cast.Ready[${tSpellID}|gem${tGem}]}) /varcalc tCT ${Spell[${tSpellID}].RecastTime}/100+${tCT}
                     }
                     /if (${Me.Class.ShortName.Equal[brd]}) {
                        /if (!${CCMsgTxt.Find[sing]} && !${CCMsgTxt.Find[sung]} && (${MBPause} || !${tTwistGems.Length})) {
                           /varset tTwistGems ${tGem}
                        } else {
                           /if (${CCMsgTxt.Find[sing]}) {
                              /varset ${sReturn.Arg[1]}TarCnt[${sReturn.Arg[2]}] 1
                              /if (${BardRestGems.Find[${tGem}]} || ${BardCombatGems.Find[${tGem}]}) /return
                              /docommand ${If[${Spell[${tSpellID}].SpellType.Equal[detrimental]},${If[!${BardCombatGems.Find[${SGem}]},/varset BardCombatGems ${BardCombatGems} ${sGem},]},${If[!${BardRestGems.Find[${SGem}]},/varset BardRestGems ${BardRestGems} ${sGem},]}]}
                           } else /if (${CCMsgTxt.Find[sung]}) {
                              /varset ${sReturn.Arg[1]}TarCnt[${sReturn.Arg[2]}] 0
                              /if (!${BardRestGems.Find[${tGem}]} && !${BardCombatGems.Find[${tGem}]}) /return
                              /if (${BardRestGems.Find[${tGem}]}) {
                                 /varset sGem R ${BardRestGems}
                                 /varset tSpell
                                 /for e 1 to ${sGem.Count[ ]}+1
                                    /if (!${sGem.Arg[${e}].Equal[${tGem}]} && ${e}>1) /varset tSpell ${tSpell} ${sGem.Arg[${e}]}
                                 /next e
                              }
                              /if (${sGem.Arg[1].Equal[r]}) /varset BardRestGems ${tSpell}
                              /if (${BardCombatGems.Find[${tGem}]}) {
                                 /varset sGem C ${BardCombatGems}
                                 /varset tSpell
                                 /for e 1 to ${sGem.Count[ ]}+1
                                    /if (!${sGem.Arg[${e}].Equal[${tGem}]} && ${e}>1) /varset tSpell ${tSpell} ${sGem.Arg[${e}]}
                                 /next e
                              }
                              /if (${sGem.Arg[1].Equal[c]}) /varset BardCombatGems ${tSpell}
                              /varset tTwistGems ${tSpell}
                              /varset tSpell Removed [${Spell[${tSpellID}].Name}] from list : TarCnt == ${${sReturn.Arg[1]}TarCnt[${sReturn.Arg[2]}]}
                              /varcalc tCT 0
                              /goto :tContCast
                           }
                        }
                     }
                  } else /if (${sGem.Equal[item]} && ${Bool[${FindItem[=${tSpell}]}]}) {
|/echo was Item
                     /varset tSpellID ${FindItem[=${tSpell}].Spell.ID}
                     /if (${FindItem[=${tSpell}].Timer}) /goto :CantCast
                     /varset tGem 20
                     /varcalc tCT ${FindItem[=${tSpell}].CastTime}*10+30
                     /if (${tSpell.Equal[Fellowship Registration Insignia]}) /makemevisible
                  } else /if (${sGem.Equal[alt]} && ${Me.AltAbility[${tSpell}].ID}) {
|/echo was Alt
                     /varset tSpellID ${Me.AltAbility[${tSpell}].Spell.ID}
                     /if (!${Me.AltAbilityReady[${tSpell}]}) /goto :CantCast
                     /varset tGem 30
                     /varcalc tCT ${Me.AltAbility[${tSpell}].Spell.CastTime}/100+20
                  } else /if (${sGem.Equal[script]}) {
|/echo was Script
                     /call MBScript "${tSpell}"
                     /return
                  } else /if (${sGem.Equal[disc]} && ${Me.CombatAbility[${tSpell}]}) {
|/echo was disc
                     /varset tSpellID ${Spell[${tSpell}].ID}
                     /if (${Me.CurrentEndurance} < ${Spell[${tSpell}].EnduranceCost}) || !${Me.CombatAbilityReady[${tSpell}]}) /goto :CantCast
                     /varset tGem 50
                     /varcalc tCT ${Spell[${tSpellID}].MyCastTime}/100+20
                  }

                  /varset tSpellRange ${Spell[${tSpellID}].Range}
                  /varset tTargetID 0

                  /if (${CCMsgTxt.Arg[3].Length}) {
                     /if (${CCMsgTxt.Arg[3].Find[%t]} && ${NetBots[${CCMsgFrom}].TargetID}) {
                        /varset tTargetID ${NetBots[${CCMsgFrom}].TargetID}
                     } else /if (${CCMsgTxt.Arg[3].Find[self]}) {
                        /varset tTargetID ${Me.ID}
                     } else /if (${sNBClnt.Find[${CCMsgTxt.Arg[3]}]}) {
                        /varset tTargetID ${Spawn[pc ${CCMsgTxt.Arg[3]}].ID}
                     } else /if (${CCMsgTxt.Arg[4].Equal[warder]} && ${Spawn[${CCMsgTxt.Arg[3].Left[-2]}].Pet.Name.Find[Warder]}) {
                        /varset tTargetID ${Spawn[pc ${CCMsgTxt.Arg[3].Left[-2]}].Pet.ID}
                     } else /if (${Spawn[${CCMsgTxt.Arg[3]}].ID} && ${CCMsgTxt.Arg[3].Equal[${Spawn[${CCMsgTxt.Arg[3]}].Name}]}) {
                        /varset tTargetID ${Spawn[${CCMsgTxt.Arg[3]}].ID}
                     } else /if (${Spawn[${CCMsgTxt.Arg[3]}].ID} && ${CCMsgTxt.Arg[3].Equal[${Spawn[${CCMsgTxt.Arg[3]}].ID}]}) {
                        /varset tTargetID ${Spawn[${CCMsgTxt.Arg[3]}].ID}
                     } else /if (${CCMsgTxt.Arg[3].Find[pet]} && ${Spawn[pc ${CCMsgFrom}].Pet.ID}) {
                        /varset tTargetID ${Spawn[pc ${CCMsgFrom}].Pet.ID}
                     }
                  }
  
                  /if (!${tTargetID}) /docommand ${If[${Spell[${tSpellID}].SpellType.Equal[detrimental]},/varset tTargetID ${NetBots[${CCMsgFrom}].TargetID},${If[${NetBots[${CCMsgFrom}].ID},/varset tTargetID ${NetBots[${CCMsgFrom}].ID},/varset tTargetID ${Spawn[${CCMsgFrom}].ID}]}]}
                  /if (!${tTargetID} && !${NetBots[${CCMsgFrom}].ID} && (${Spell[${tSpellID}].SpellType.Equal[detrimental]} || ${CCMsgTxt.Arg[3].Find[%t]})) {
                     /target clear
                     /assist ${CCMsgFrom}
                     /delay 2s ${Target.ID}
                     /varset tTargetID ${Target.ID}
                  }
:tPrepBuff
                  /if (${CCMsgTxt.Arg[3].Equal[grp]}) {
                     /varcalc tGrpMbr ${tGrpMbr}+1
                     /if (${Group.Member[${tGrpMbr}].ID}) {
                        /varset tTargetID ${Group.Member[${tGrpMbr}].ID}
                        /if (!${Spawn[${tTargetID}].ID} || ${Spawn[${tTargetID}].Distance}>${Spell[${tSpellID}].MyRange}) {
                           /bc Cast canceled for ${Spawn[${tTargetID}].Name}.  Not in range!
                           /goto :tPrepBuff
                        }
                     } else {
                        /varset tTargetID ${Me.ID}
                     }
                  }
                  /if (${Select[${Spell[${tSpellID}].Name},Gate,Mass Group Buff]} || (${Spell[${tSpellID}].TargetType.Equal[self]} && !${Me.AltAbility[Project Illusion]})) {
                     /varset tTargetID ${Me.ID}
                  } else /if (${Spell[${tSpellID}].TargetType.Equal[self]} && ${Me.AltAbility[Project Illusion]} && (${Spawn[${tTargetID}].ID} && ${tTargetID}!=${Me.ID} || ${CCMsgTxt.Arg[3].Find[%t]} || ${Spawn[${CCMsgTxt.Arg[3]}].ID})) {
                     /varset ProjectIllu TRUE
                     /echo Projecting
                  }
                  /if (${Spell[${tSpellID}].TargetType.Equal[pet]}) /varset tTargetID ${Me.Pet.ID}
                  /if (!${tTargetID} || ${Spawn[${tTargetID}].Distance3D}>${tSpellRange} && ${Spell[${tSpellID}].TargetType.Find[single]}) {
                     /echo Dude 10
                     /goto :CantCast
                  }
                  /if (${tGem}==50) {
                     /varset CCtTargetID ${tTargetID}
                     /if (${Target.ID}!=${CCtTargetID} && ${CCtTargetID} && (${tTargetID}!=${Me.ID} && ${Spell[${tSpellID}].TargetType.NotEqual[self]})) {
                        /squelch /tar id ${CCtTargetID}
                        /delay 1s ${Target.ID}==${CCtTargetID}
                     }
                     /disc ${tSpellID}
                     /goto :CastDone
                  }
                  /varset CCtTargetID 0
                  /varset tCastTime ${tCT}
:tCasting
                  /if (${CCtTargetID}) {
                     /doevents ImHit
                     /if (${APAggro}) /goto :tContCast
                     /if (${Me.Class.ShortName.Equal[BRD]}) {
                        /doevents CastRst
                        /doevents CastStn
                        /doevents CastImm
                        /if (!${CastResult.Equal[pending]}) {
                           /if (${Select[${CastResult},success,immune,resisted,reset]}) /goto :tContCast
                           /if (${Select[${CastResult},stunned]}) {
                              /varset CCtTargetID 0
                              /goto :tCasting
                           }
                        }
                        /delay 1s
                        /if (!${tCastTime} && !${Cast.Timing}) /varset CastResult SUCCESS
                     }
                     /if (${Cast.Timing} && !${Cast.Status.Equal[i]} || ${Me.Class.ShortName.Equal[BRD]}) /goto :tCasting
                     /if (${Cast.Status.Equal[i]}) {
                        /if (${Select[${Cast.Result},cast_fizzle,cast_recover]}) {
                           /varset tCastTime ${tCT}
                           /varset CCtTargetID 0
                           /goto :tCasting
                        }
                        /if (${Select[${Cast.Result},cast_immune,cast_takehold]}) {
                           /bc Target Immune or wont Take Hold
                           /goto :tContCast
                        }
                        /if (${Select[${Cast.Result},cast_outofrange,cast_cannotsee,cast_distracted,cast_notarget,cast_interrupted]}) /goto :CantCast
                        /if (${Cast.Result.Equal[cast_success]}) /goto :tContCast
                     }
                  } else /if (${Me.Class.ShortName.Equal[BRD]} || ${Cast.Ready[${tSpell}|${sGem}]} && !${Spell[tSpellID].TargetType.Equal[self]}) {
                     /varset CCtTargetID ${tTargetID}
                     /if (${Target.ID}!=${CCtTargetID} && ${CCtTargetID} && (${tTargetID}!=${Me.ID} && ${Spell[${tSpellID}].TargetType.NotEqual[self]})) {
                        /squelch /tar id ${CCtTargetID}
                        /delay 1s ${Target.ID}==${CCtTargetID}
                     }
                     /if (${Me.Class.ShortName.Equal[BRD]}) {
                        /if (${Twist}) /twist off
                        /varset tCastTime 40
                        /varset CastResult PENDING
                        /docommand ${If[${tGem}<20,/squelch /twist ${sGem},/casting "${tSpell}|${sGem}" ${If[${tTargetID},-targetid|${tTargetID},]}]}
                     } else {
                        /if (${ProjectIllu}) {
                           /casting "Project Illusion|alt" -targetid|${tTargetID}
                           /delay 1s
                        }
                        /varset MBCurrentSpell CC|1|${tTargetID}
                        /docommand ${If[${tGem}>19,/call MQ2Cast "${tSpell}" ${sGem} ${Spell[${tSpell}].MyCastTime.Float}s -targetid|${tTargetID} -bandolier|${tFoci},/call MQ2Cast "${Spell[${tSpellID}]}" gem${tGem} ${Spell[${tSpell}].MyCastTime.Float}s -targetid|${tTargetID} -bandolier|${tFoci}]}
                        /if (${Macro.Return.Equal[cast_success]}) /goto :tContCast
                     }
                  }
                  /if (${tCastTime} || ${Cast.Status.NotEqual[i]}) /goto :tCasting
                  /echo WTF - Cast
                  /goto :CantCast
:tContCast
                  /varset sReturn ${Cast.Result} ${sReturn} ${tSpellID} ${CCtTargetID}
                  /if (${Me.Class.ShortName.Equal[BRD]}) {
                     /if (${Twist.List.NotEqual[${tTwistGems}]} && ${tTwistGems.Length}>0) {
                        /squelch /twist ${tTwistGems}
                     } else {
                        /if (${Twist.Twisting} && ${tTwistGems.Length} < 1) /twist off
                     }
                  } 
                  /if (!${Select[${Cast.Result},cast_immune,cast_takehold]} && ${tCCMsgArg1.NotEqual[qcast]}) /docommand ${If[!${tSpell.Find[from list]},/bc Casted: -[ ${Spawn[${CCtTargetID}].CleanName} ]- with ${tSpell},/bc Casted: -[ Bard ]- ${tSpell}]}
                  /if (${Cursor.ID}) /call ClearCursor
                  /doevents eqbc
                  /if (${CCMsgTxt.Arg[3].Equal[grp]} && ${Group.Member[${tGrpMbr}].ID}) {
                     /varcalc tCT ${tCT}+40
                     /goto :tPrepBuff
                  }
                  /varcalc CastCount ${CastCount}+1
                  /if (${CCMsgTxt.Arg[1].Equal[coresist]}) /goto :CastDone
               }
:AliasReturn            
            /next c
         /next b
      }
   /next a
   /if (${CastCount}>0 && (${CCMsgTxt.Arg[3].Equal[on]} || ${CCMsgTxt.Arg[3].Equal[off]})) /return SUCCESS
   /if (${CastCount}) {
      /varset CastCount 0    
      /goto :AliasDone
   }
   /if (${NetBots[${CCMsgFrom}].ID} && !${CastCount}) /echo Couldn't find Alias [${CCMsgTxt.Arg[2, ]}]
   /return NOALIASFND

:AliasDone
   /docommand ${If[${CCMsgTxt.Arg[3].Equal[%t]} || ${CCMsgTxt.Arg[3].Equal[grp]} || ${sNBClnt.Find[${CCMsgTxt.Arg[3]}]} || ${CCMsgTxt.Arg[3].Equal[self]} || ${CCMsgTxt.Arg[3].Equal[${Spawn[${CCMsgTxt.Arg[3]}].ID}]},/varset d 4,/varset d 3]}
   /varset tCCMsgTxt
   /varset tGrpMbr 0

:tArgloop
   /if (${CCMsgTxt.Arg[${d}].Length}) {
      /if (${CCMsgTxt.Arg[${Math.Calc[${d}+1]}].Equal[warder]}) /varcalc d ${d} + 2
      /varset tCCMsgTxt ${tCCMsgTxt} ${CCMsgTxt.Arg[${d}]}
      /varcalc d ${d}+1
      /goto :tArgloop
   } else {
      /if (${tCCMsgTxt.Length} > 0) {
         /varset CCMsgTxt ${tCCMsgArg1} ${tCCMsgTxt}
         /varset b 1
         /varset c 1
         /varset a 1
         /goto :AliasLoop
      }
   }
:CastDone
   /if (!${Me.ID}) /call Event_DelayOnZone
   /varset MBCurrentSpell
   /if (${Me.Class.ShortName.Equal[BRD]} && !${MBPause}) {
      /if (${Twist.List.NotEqual[${tTwistGems}]} && ${tTwistGems.Length}>0) /squelch /twist ${tTwistGems}
   }
   /return ${sReturn}

:CantCast
   /if (${Me.Class.ShortName.Equal[BRD]} && !${MBPause}) {
      /if (${Twist.List.NotEqual[${tTwistGems}]} && ${tTwistGems.Length}>0) /squelch /twist ${tTwistGems}
   }
   /if (!(${Spell[${tSpellID}].SpellType.Equal[detrimental]} && (${CCtTargetID} && (${Spawn[${CCtTargetID}].Type.Equal[corpse]} || !${Spawn[${CCtTargetID}].ID})) || (!${CCtTargetID} && ${tTargetID} && (${Spawn[${tTargetID}].Type.Equal[corpse]} || !${Spawn[${tTargetID}].ID})))) /bc There was a problem and I can't cast ${tSpell} right now
/return FAIL ${sReturn}


Sub ProcessExcludeList(P0)
   /if (${MBDebug}) /echo Macro.RunTime:${Macro.RunTime} Sub:ProcessExcludeList : ${P0} : ${ZoneName} - ${Zone.Name}
   /if ((${P0.Length}) || !${Zone.Name.Equal[${ZoneName}]}) {
       /if (${P0.Equal[save]}) {
          /ini "${MBCommon}" "ExcludeList" "${Zone.ShortName}" "${ExcludeList}"
          /return
       } else /if (${P0.Equal[zone]} || !${Zone.Name.Equal[${ZoneName}]}) {
          /varset ExcludeList ${If[${Select[${Ini["${MBCommon}","ExcludeList","${Zone.ShortName}",NOTFOUND]},NOTFOUND,NULL]},|,${Ini["${MBCommon}","ExcludeList","${Zone.ShortName}",NOTFOUND]}]}
|         /echo /varset ExcludeList ${If[${Select[${Ini["${MBCommon}","ExcludeList","${Zone.ShortName}",NOTFOUND]},NOTFOUND,NULL]},|,${Ini["${MBCommon}","ExcludeList","${Zone.ShortName}",NOTFOUND]}]}
|         /echo List set
       }
   }
   /if (!${ExcludeList.Left[1].Equal[|]}) /varset ExcludeList |${ExcludeList}
|  /echo ${ExcludeList} 
   /declare a int local
   /squelch /alert clear ${AlertList}
   /squelch /alert add ${AlertList} NPC "eye of "
   /if (${ExcludeList.Length}<=1) /return
   /for a 1 to ${ExcludeList.Count[|]}
      /if (${ExcludeList.Arg[${a},|].Find[#]}==1) {
         /squelch /alert add ${AlertList} NPC "${ExcludeList.Arg[${a},|].Right[-1]}"
      } else {
         /squelch /alert add ${AlertList} NPC "${ExcludeList.Arg[${a},|]}"
      }
   /next a
/return


Sub Login(string name,..)
|  /echo ${name.Arg[2, ]}
|  /echo ${MacToRun}
   /if (!${name.Length}) {
      /echo Syntax error
      /return
   }
   /declare toon int local
   /if (${Defined[MacToRun]}) /deletevar MacToRun
   /if (${name.Find[|]}) {
      /declare MacToRun string global ${name.Arg[2,|]}
   }
   /bc Logging into ${name.Arg[1,|]}
   /camp
   /delay 10m ${MacroQuest.GameState.Equal[CHARSELECT]}
   /varset toon ${Window[CharacterListWnd].Child[CLW_LeftScreen].Child[CLW_CharactersScreen].Child[CLW_Character_List].List[${name.Arg[1,|]},3]}
   /notify CharacterListWnd CLW_Character_List listselect ${toon}
   /notify CharacterListWnd CLW_Character_List leftmouse ${toon}
   /notify CharacterListWnd CLW_Play_Button leftmouseup
   /delay 10m ${MacroQuest.GameState.Equal[INGAME]}
|  /delay 3m ${Me.ID}
   /if (${Zone.ShortName.Equal[Bazaar]} && ${Me.Name.Equal[${TraderName.Arg[1,|]}]} && !${Defined[MacToRun]}) {
      /echo Trader logged in.
      /if (!${Window[BazaarWnd]}) /timed 10 /trader
      /timed 30 /if (${Window[BazaarWnd].Child[BZW_Start_Button]}) /nomodkey /notify BazaarWnd BZW_Start_Button leftmouseup
      /endmacro
   } else /if (${Zone.ShortName.Equal[Bazaar]} && ${Me.Name.Equal[${TraderName.Arg[1,|]}]} && ${Defined[MacToRun]}) {
      /echo Trader logged in.  Starting ${MacToRun} macro.
      /timed 100 /mac ${MacToRun}
      /timed 150 /deletevar MacToRun
      /endmacro
   } else {
      /mqp
      /timed 100 /macro modbot
   }
/return


Sub Startup
   /if (${MBDebug}) /echo Macro.RunTime:${Macro.RunTime} - Sub:Startup

   /if (!${Plugin[MQ2Eqbc].Name.Length}) {
      /squelch /plugin MQ2Eqbc noauto
      /if (!${Plugin[MQ2Eqbc].Name.Length}) {
         /echo You must have MQ2EQBC plugin to use this macro!!
         /endmacro
      }
   }

   /if (!${Plugin[MQ2Netbots].Name.Length}) {
      /squelch /plugin MQ2Netbots noauto
      /if (!${Plugin[MQ2Netbots].Name.Length}) {
         /echo You must have MQ2Netbots plugin to use this macro!!
         /endmacro
      }
   }
   /netbots on send=on grab=on

   /if (!${Plugin[MQ2Melee].Name.Length}) {
      /squelch /plugin MQ2Melee noauto
      /if (!${Plugin[MQ2Melee].Name.Length}) {
         /echo You must have MQ2Melee plugin to use this macro!!
         /endmacro
      }
      /bc You should configure MQ2Melee if you want to melee
   }

   /if (!${Plugin[MQ2Cast].Name.Length}) {
      /squelch /plugin MQ2Cast noauto
      /if (!${Plugin[MQ2Cast].Name.Length}) {
         /echo You must have MQ2Cast plugin to use this macro!!
         /endmacro
      }
   }

   /if (!${Plugin[MQ2Exchange].Name.Length}) {
      /squelch /plugin MQ2Exchange noauto
      /if (!${Plugin[MQ2Exchange].Name.Length}) {
         /echo You must have MQ2Exchange plugin to use this macro!!
         /endmacro
      }
   }

   /if (!${Plugin[MQ2AdvPath].Name.Length}) {
      /squelch /plugin MQ2AdvPath noauto
      /if (!${Plugin[MQ2AdvPath].Name.Length}) {
         /echo You must have MQ2AdvPath plugin to use this macro!!
         /endmacro
      }
   }

   /if (!${Plugin[MQ2MoveUtils].Name.Length}) {
      /squelch /plugin MQ2MoveUtils noauto
      /if (!${Plugin[MQ2MoveUtils].Name.Length}) {
         /echo You must have MQ2MoveUtils plugin to use this macro!!
         /endmacro
      }
   }

   /if (${Me.Class.ShortName.Equal[BRD]}) {
      /if (!${Plugin[MQ2Twist].Name.Length} ) {
         /squelch /plugin MQ2Twist noauto
         /if (!${Plugin[MQ2Twist].Name.Length} ) {
            /echo Bards must have MQ2Twist plugin to use this macro!!
            /endmacro
         }
      }
   }

   /squelch /stick off
   /declare CurMerchant int outer 0
   /declare CurrentEQBCCommand string outer
   /declare AlertList int outer 20
   /declare MiscCheckTimer timer outer
   /declare SitCheckTimer timer outer
   /declare AACheckTimer timer outer
   /declare SenderName string outer
   /declare FollowFlagZoneDelay int outer 20
   /declare DoBuffBot bool outer
   /declare CastResult string outer UNKNOWN
   /declare CampStatus int outer
   /declare MakeCampX int outer
   /declare MakeCampY int outer
   /declare MakeCampD int outer
   /declare MBPause bool outer
   /declare QueueCount int outer
   /declare DoBuffQueue[25,2] int outer
   /declare MBCurrentSpell string outer
   /declare DoQuest bool outer TRUE
   /declare DoCombines bool outer FALSE
   /declare TankName string outer
   /declare PullerName string outer
   /declare EchoScripts bool outer False
   /declare NamedID int outer
   /declare BardCombatGems string outer
   /declare BardRestGems string outer
   /declare ZoneName string outer
   /declare interruptFlag bool outer FALSE
   /declare AAIniFile string outer AA_${Me.Name}.ini 

   /call LoadINIVar Settings DoMelee FALSE DoMelee "${IniFile}" bool
   /call LoadINIVar Settings DoHeals TRUE DoHeals "${IniFile}" bool
   /call LoadINIVar Settings DoBuffs FALSE DoBuffs "${IniFile}" bool
   /call LoadINIVar Settings DoDebuffs FALSE DoDebuffs "${IniFile}" bool
   /call LoadINIVar Settings DoEvents FALSE DoEvents "${IniFile}" bool
   /call LoadINIVar Settings DoCures FALSE DoCures "${IniFile}" bool
   /call LoadINIVar Settings DoPull FALSE DoPull "${IniFile}" bool
   /call LoadINIVar Settings DoPet FALSE DoPet "${IniFile}" bool
   /call LoadINIVar Settings DoSit FALSE DoSit "${IniFile}" bool
   /call LoadINIVar Settings DoLoot FALSE DoLoot "${IniFile}" bool
   /call LoadINIVar Settings DoFW FALSE DoFW "${IniFile}" bool
   /call LoadINIVar Settings DoForage FALSE DoForage "${IniFile}" bool
   /call LoadINIVar Settings ForageIni "forage.ini" ForageIni "${IniFile}" string
   /call LoadINIVar Settings DoAfk FALSE DoAfk "${IniFile}" bool
   /call LoadINIVar Settings DoRez TRUE DoRez "${IniFile}" bool
   /call LoadINIVar Settings DoMount FALSE DoMount "${IniFile}" bool
   /call LoadINIVar Settings MountCast " " MountCast "${IniFile}" string
   /call LoadINIVar Settings MasterList "Netbots" MasterList "${IniFile}" string
   /call LoadINIVar Settings TankName " " AllTankName "${IniFile}" string
   /if (${AllTankName.NotEqual[manual]}) /varset TankName ${AllTankName.Arg[1,,]}
   /call LoadINIVar Settings PullerName " " AllPullerName "${IniFile}" string
   /if (${AllPullerName.NotEqual[manual]}) /varset PullerName ${AllPullerName.Arg[1,,]}
   /declare ExcludeList string outer ${If[${Ini["${MBCommon}","ExcludeList","${Zone.ShortName}",NOTFOUND].NotEqual[NOTFOUND]},${Ini["${MBCommon}","ExcludeList","${Zone.ShortName}",NOTFOUND]},|]}
   /call LoadINIVar Settings Radius 100 Radius "${IniFile}" int
   /call LoadINIVar Settings ZRadius 100 ZRadius "${IniFile}" int
   /call LoadINIVar Settings SitAggroRadiusCheck 75 SitAggroRadiusCheck "${IniFile}" int
   /if (${Me.MaxMana}==0) { 
      /call LoadINIVar Settings SitManaPct 0 SitManaPct "${IniFile}" int
   } else {
      /call LoadINIVar Settings SitManaPct 95 SitManaPct "${IniFile}" int
   }
   /if (${Select[${Me.Class.ShortName},CLR,DRU,SHM,ENC,NEC,MAG,WIZ,BRD]}) {
      /call LoadINIVar Settings SitEndurPct 0 SitEndurPct "${IniFile}" int
   } else {
      /call LoadINIVar Settings SitEndurPct 95 SitEndurPct "${IniFile}" int
   }
   /call LoadINIVar Settings AfkMessage "Not now, thanks" AFKMessage "${IniFile}" string
   /call LoadINIVar Settings RezMinPct 96 RezMinPct "${IniFile}" int
   /call LoadINIVar Settings RezAcceptCall FALSE RezAcceptCall "${IniFile}" bool
   /call LoadINIVar Settings RezGiveUpSeconds 6000 RezGiveUpSeconds "${IniFile}" int
   /call LoadINIVar Settings DeathSlot "FALSE" DeathSlot "${IniFile}" string
   /if (!${Select[${DeathSlot},TRUE,FALSE]}) /ini "${IniFile}" "Settings" "DeathSlot" "FALSE"
   /call LoadINIVar Settings NetworkINI " " NetworkINI "${IniFile}" string
   /call LoadINIVar Settings TraderName " " TraderName "${IniFile}" string
   /if (!${Defined[DoTells]}) /declare DoTells bool outer TRUE
   /squelch /makecamp leash 200
   /squelch /makecamp radius 10
   /if (!${MakeCamp.Leash}) /squelch /makecamp leash
   /makecamp off
   /call LoadINIVar Settings FollowDistance 20 FollowDistance "${IniFile}" string
   /if (!${Defined[FollowFlag]}) /declare FollowFlag int outer
   /if (!${Defined[PauseFlag]}) /declare PauseFlag int outer 0
   /if (!${Defined[FollowID]}) /declare FollowID int outer
   /if (!${Defined[FollowName]}) /declare FollowName string outer
   /call LoadINIVar Settings FollowStick "${FollowDistance} hold uw" FollowStick "${IniFile}" string
   /call LoadINIVar Settings PetCast " " PetCast "${IniFile}" string
   /call LoadINIVar Settings PetAggro FALSE PetAggro "${IniFile}" bool
   /call LoadINIVar Settings PetAssist 0 PetAssist "${IniFile}" int
   /call LoadINIVar Settings PetFoci " " PetFoci "${IniFile}" string
   /call LoadINIVar Settings PetShrink TRUE PetShrink "${IniFile}" bool
   /call LoadINIVar Settings PetShrinkSpell " " PetShrinkSpell "${IniFile}" string
   /call LoadINIVar Settings SummonFood " " SumFood "${IniFile}" string
   /call LoadINIVar Settings SummonDrink " " SumDrink "${IniFile}" string
   /call LoadINIVar Settings DoAA FALSE DoAA "${IniFile}" bool
   /call LoadINIVar Settings AAAnnounce " " AAAnnounce "${IniFile}" string
   /call LoadINIVar Settings AAtoNormal FALSE AAtoNormal "${IniFile}" bool
   /call LoadINIVar Settings AAWarning 90 AAWarning "${IniFile}" int
   /call LoadINIVar Settings AABank 0 AABank "${IniFile}" int
   /call LoadINIVar Settings AAVerbose FALSE AAVerbose "${IniFile}" bool
   /if (${Me.Class.ShortName.Equal[BRD]}) {
      /squelch /stoptwist
      /squelch /twist reset
      /squelch /twist reload
   }
   /call LoadINIVar Settings LoadAlias 1 LoadAlias "${MBCommon}" int
   /if (${LoadAlias}) {
      /alias /mb /echo MB-
      /ini "${MBCommon}" "Settings" "LoadAlias" 0
   }
   /call LoadINIVar Settings BeepCommand "beep ding.wav" BeepCommand "${MBCommon}" string
   /call LoadINIVar Settings GoMNuke " " GoMNuke "${IniFile}" string
   /call ProcessExcludeList zone
   /varset ZoneName ${Zone.Name}
   /tgb on
   /call MBScript MBStartup
   /doevents flush
/return


Sub ItemSort
   /delay 5
   /declare DoubleCheck local
   /varset DoubleCheck ${Cursor.ID}
   /if (!${Ini["${ForageIni}",ForageList,${Cursor.Name}].Length}) {
      /ini "${ForageIni}" "ForageList" "${Cursor.Name}" "-1"
   } else {
      /if (${Ini["${ForageIni}",ForageList,${Cursor.Name}]}) {
         /if (${Ini["${ForageIni}",ForageList,${Cursor.Name}].Equal[Ignore]}) /return
         /if (${FindItemCount["=${Cursor.Name}"]}>=${Ini["${ForageIni}",ForageList,${Cursor.Name}]} && ${Ini["${ForageIni}",ForageList,${Cursor.Name}]}!=-1 && ${Ini["${ForageIni}",ForageList,${Cursor.Name}].NotEqual[Ignore]}) /goto :Destroy
      } else {
:Destroy
         /if (${Cursor.ID} == ${DoubleCheck}) { 
            /echo Destroying ${Cursor.Name} 
            /destroy 
         } 
         /return
      }
   }
:LootIt
   /echo Keeping ${Cursor.Name}
   /autoinventory
   /delay 5
/return


Sub Event_ResetMelee
   /melee reset
/return


Sub Event_WornOff(Line,SpellName,Dude)
   /if (${SpellName.Equal[${ADCharmSpell}]}) {
      /call Event_CharmBroke
      /return
   }
   /if (${NetBots[${Dude}].ID}) /return
   /declare i int local
   /declare spwnType string local
   /if ((${Dude.Find['s warder]} || ${Dude.Find['s Pet]}) && ${Spawn[Pet ${Dude}].ID}) {
      /varset spwnType Pet
   } else {
      /varset spwnType ${Spawn[${Dude}].Type}
   }
   /if (${ABCount} && ${Select[${Spawn[${Dude}].Type},PC,Pet]}) {
      /for i 1 to ${ABCount}
          /if (${SpellName.Equal[${Spell[${ABSpell[${i}]}].Name}]} && ${ABRecast[${i}]}) /call AddToQueue ${Spawn[${spwnType} ${Dude}].ID} ${i}
      /next i
   }
/return


Sub DoBuffEvents
   /if (${MBPause}) /return
   /declare a int local
   /declare TempVar int local ${Math.Calc[${ABCount}+1]}
   /declare TempGuy int local 0
   /for a 1 to 25
      /if (${DoBuffQueue[${a},2]}<${TempVar} && ${Spawn[${DoBuffQueue[${a},1]}].ID} && ${Spawn[${DoBuffQueue[${a},1]}].Type.NotEqual[corpse]} && (${Spawn[${DoBuffQueue[${a},1]}].Distance}<${Me.Book[${Me.Book[${ABSpell[${DoBuffQueue[${a},2]}]}]}].MyRange} || ${Spawn[${DoBuffQueue[${a},1]}].Distance}<${Me.Book[${Me.Book[${ABSpell[${DoBuffQueue[${a},2]}]}]}].AERange})) {
         /varset TempGuy ${DoBuffQueue[${a},1]}
         /varset TempVar ${DoBuffQueue[${a},2]}
      }
|     /if (!${Spawn[${DoBuffQueue[${a},1]}].ID}) /call RemoveFromQueue ${DoBuffQueue[${a},1]} ${DoBuffQueue[${a},2]}
   /next a
   /if (!${ACState} && ${DoMelee} && ${ADMobCount}) /return
   /if (!${SpawnCount[npc radius ${Radius} zradius ${ZRadius} noalert ${AlertList}]} || ${ABTarType[${TempVar}].Find[cbt]} || ${Select[${ABGem[${TempVar}]},item,alt]} || ${Me.Gem[${ABSpell[${TempVar}]}]}) {
      /if (${TempGuy}) /if (((${Me.SpellReady[${ABSpell[${TempVar}]}]} || ${Spell[${ABSpell[${TempVar}]}].RecastTime.TotalSeconds}<30) && ${Me.CurrentMana}>${Spell[${ABSpell[${TempVar}]}].Mana} && ${Me.PctMana}>=${ABSpellMinMana[${TempVar}]}) || (${ABGem[${TempVar}].Equal[item]} && ${FindItem[${ABSpell[${TempVar}]}].TimerReady}<30) || (${ABGem[${TempVar}].Equal[alt]} && ${Me.AltAbilityReady[${ABSpell[${TempVar}]}]}<30)) {
         /call MQ2Cast "${ABSpell[${TempVar}]}" ${If[!${Select[${ABGem[${TempVar}]},item,alt]},gem,]}${ABGem[${TempVar}]} 10s CastCheck -targetid|${TempGuy}
         /if (${castReturn.Equal[CAST_TAKEHOLD]} || ${castReturn.Equal[CAST_SUCCESS]} || ${Cast.Return.Equal[CAST_OUTDOORS]}) {
            /call RemoveFromQueue ${TempGuy} ${TempVar}
|           /if (${ABAnnounce[${TempVar}].Length}) /docommand ${ABAnnounce[${a}]} Recast: -[ ${Spawn[${ABtTargetID}].CleanName} ]- with ${ABSpell[${a}]}
         }
      }
   }
/return


Sub AddToQueue(int Buffee,int BuffNumber)
   /declare a int local
|  /if (!${RestrictedList.Find[|${DoBuffID[${BuffNumber}]}|]} && ${QueueCount}<25 && ${Spawn[${Buffee}].Type.NotEqual[corpse]} && ${Spawn[${Buffee}].ID}) {
   /if (${QueueCount}<25 && ${Spawn[${Buffee}].Type.NotEqual[corpse]} && ${Spawn[${Buffee}].ID}) {
      /for a 1 to 25
         /if (!${DoBuffQueue[${a},1]}) {
            /varset DoBuffQueue[${a},1] ${Buffee}
            /varset DoBuffQueue[${a},2] ${BuffNumber}
            /varcalc QueueCount ${QueueCount}+1
            /return
         }
      /next a
   }
/return


Sub RemoveFromQueue(int Buffee,int BuffNumber)
   /doevents IRC
   /doevents Chat
   /declare a int local
   /declare tABMob int local
   /for a 1 to 25
      /if (((${Spawn[${DoBuffQueue[${a},1]}].State.Equal["DEAD"]} || !${Spawn[${DoBuffQueue[${a},1]}].ID}) || ((${DoBuffQueue[${a},1]}==${Buffee} || (!${Buffee} && (${Group.Member[${Spawn[${DoBuffQueue[${a},1]}].CleanName}].Index} || ${Group.Member[${Spawn[${DoBuffQueue[${a},1]}].Master.CleanName}].Index}))) && (${DoBuffID[${DoBuffQueue[${a},2]}]}==${BuffNumber} || ${DoBuffQueue[${a},2]}==${BuffNumber}))) && ${DoBuffQueue[${a},1]}) {
         /varset DoBuffQueue[${a},1] 0
         /varset DoBuffQueue[${a},2] 0
         /varcalc QueueCount ${QueueCount}-1
      }
   /next a
/return


Sub TellEvent(string From, string MsgText)
   /if (!${DoTells}) /return
   /bc ${Time} [+g+]Tell Recieved from[+x+] ${From} : ${MsgText}
   /docommand /${BeepCommand}
/return


Sub Event_Camping
   /echo Campout detected. Ending macro.
   /call MainExit
   /endmacro
/return


Sub CastCheck
   /if (${MBDebug}) /echo Macro.RunTime:${Macro.RunTime} ${Time}- Sub:CastCheck
   /call AHHealCheck
   /if (${Me.Mount.ID} && !${AHAllowDismount} || ${Cast.Status.Equal[I]}) /return
   /if (${Window[RespawnWnd].Open} || ${NetBots[${Me.CleanName}].Charmed}) {
      /varset interruptFlag TRUE
      /return
   }
   /declare a int local 0
   /if ((${Target.Type.Equal[corpse]} || !${Target.ID}) && ${Spell[${Me.Casting.ID}].TargetType.Find[Single]}) {
      /call interrupt
      /return
   }
   /if (${MBCurrentSpell.Arg[1,|].Equal[AD]}) {
      /if ((${Select[${ADTarType[${MBCurrentSpell.Arg[2,|]}]},1,11]} && ${NetBots[${TankName}].TargetID}!=${MBCurrentSpell.Arg[3,|]} || ${Select[${ADTarType[${MBCurrentSpell.Arg[2,|]}]},2,12]} && ${NetBots[${TankName}].TargetID}==${MBCurrentSpell.Arg[3,|]}) && !${Me.Mount.ID}) /call interrupt
      /if ((${Select[${ADTarType[${MBCurrentSpell.Arg[2,|]}]},1,11]} && ${NetBots[${TankName}].TargetID}!=${MBCurrentSpell.Arg[3,|]} || ${Select[${ADTarType[${MBCurrentSpell.Arg[2,|]}]},2,12]} && ${NetBots[${TankName}].TargetID}==${MBCurrentSpell.Arg[3,|]}) && ${Me.Mount.ID}) {
         /varset interruptFlag TRUE
         /return
      }
      /if (${Spawn[${Target.ID}].Type.Equal[corpse]} || ${Spell[${Me.Casting.ID}].TargetType.Equal[single]} && !${Target.ID}) /call interrupt
      /return
   }
   /if (${MBCurrentSpell.Arg[1,|].Equal[AB]}) {
      /if (${ABTarType[${MBCurrentSpell.Arg[2,|]}].Find[cbt]} && !${ABTarType[${MBCurrentSpell.Arg[2,|]}].Find[idle]} && ${ADMobCount} && ${SpawnCount[npc los radius ${Radius} zradius ${ZRadius} noalert ${AlertList}]}<=${SpawnCount[npc untargetable los radius ${Radius} zradius ${ZRadius} noalert ${AlertList}]} && !${Me.Mount.ID}) {
         /call interrupt
         /return
      }
      /if (${ADCount} && ${ADMobCount} && ${DoDebuffs} && !${ABTarType[${MBCurrentSpell.Arg[2,|]}].Find[cbt]}) {
         /for a 1 to ${ADMobMax}
|            /docommand ${If[${ADMobList[${a}]} && !${MLDebuffed[${a}]},/call interrupt,/next a]}
             /if (${ADMobList[${a}]} && !${MLDebuffed[${a}]}) {
                 /call interrupt
                 /return
             }
         /next a
      }
   }
/return


Sub ParseAnnounce(string AnnounceText, string tTarget, string tSpell)
   /declare ATlft string local
   /declare ATrght string local
   /if (${AnnounceText.Length}<5 || !${tTarget.Length}) /return
   /if (${AnnounceText.Find[,]}) {
      /docommand ${AnnounceText}
      /return
   }
   /if (${AnnounceText.Find[%t]} && ${tTarget.Length}) {
      /varset ATlft ${AnnounceText.Left[${Math.Calc[${AnnounceText.Find[%t]}-1]}]}
      /varset ATrght ${AnnounceText.Right[${Math.Calc[${AnnounceText.Length}-${AnnounceText.Left[${Math.Calc[${AnnounceText.Find[%t]}-1]}].Length}-2]}]}
      /varset AnnounceText ${ATlft} ${tTarget} ${ATrght}
   }
   /if (${AnnounceText.Find[%s]} && ${tSpell.Length}) /varset AnnounceText ${AnnounceText.Replace[%s,${tSpell}]}
   /docommand ${AnnounceText}
/return


Sub IniCommand(string IniText)
| /echo IniCommand: Text = ${IniText}
  /if (${IniText.Length}<3) /return
  /declare b int local 1
  /declare sVarC string local
  /declare sVarL string local
  /declare sVarM string local
  /for b 1 to ${IniText.Count[{]}
    /varset sVarL ${IniText.Arg[${b},{]}
    /varset sVarC ${sVarC}${sVarL}${
      }
  /next b
  /varset sVarC ${sVarC}${IniText.Arg[${b},{]}
  /if (!${IniText.Count[{]}) /varset sVarC ${IniText}
  /varset sVarM
  /for b 1 to ${sVarC.Count[']}
    /if (${sVarC.Find['']}) {
      /varset sVarL ${sVarC.Mid[1,${Math.Calc[${sVarC.Find['']}-1]}]}"
| "
      /varset sVarC ${sVarC.Mid[${Math.Calc[${sVarC.Find['']}+2]},${sVarC.Length}]}
      /varset sVarM ${sVarM}${sVarL}
    }
  /next b
  /varset sVarM ${sVarM} ${sVarC}
  /if (${sVarM.Length}) /varset sVarC ${sVarM}
| /echo "${sVarC}"
  /docommand ${sVarC}
/return FALSE


Sub Event_GoM(string eLine,string eSkip,string eMax)
   /if (${MBPause} || !${GoMNuke.Length} || (!${Me.Song[Gift of Mana (${eMax})].ID} && !${Me.Song[Gracious Gift of Mana (${eMax})].ID})) /return
   /declare x int local 0
   /declare IniSpell string local
   /for x 1 to ${Math.Calc[${GoMNuke.Count[|]}+1]}
      /call FindAlias ${GoMNuke.Arg[${x},|]}
      /if (!${Macro.Return.Length} || ${Macro.Return.Equal[NULL]}) /return
      /varset IniSpell ${Macro.Return}
      /if ((${Spell[${${IniSpell.Arg[1,|]}Spell[${IniSpell.Arg[2,|]}]}].SpellType.Equal[detrimental]} && (!${Spawn[${ACMATarget}].ID} || ${Spawn[${ACMATarget}].Type.Equal[corpse]})) || ${Spell[${${IniSpell.Arg[1,|]}Spell[${IniSpell.Arg[2,|]}]}].Level}>${Int[${eMax}]}) {
         /if (${GoMNuke.Find[|]} && ${Math.Calc[${x}-1]}<${GoMNuke.Count[|]}) /goto :nextGOM
         /return
      }
      /delay 2s ${Cast.Ready[${${IniSpell.Arg[1,|]}Spell[${IniSpell.Arg[2,|]}]}|gem${${IniSpell.Arg[1,|]}Gem[${IniSpell.Arg[2,|]}]}]}
      /if (!${Cast.Ready[${${IniSpell.Arg[1,|]}Spell[${IniSpell.Arg[2,|]}]}|gem${${IniSpell.Arg[1,|]}Gem[${IniSpell.Arg[2,|]}]}]} && ${GoMNuke.Find[|]} && ${Math.Calc[${x}-1]}<${GoMNuke.Count[|]}) /goto :nextGOM
      /goto :castGOM
:nextGOM
   /next x
   /return
:castGOM
   /call MQ2Cast "${${IniSpell.Arg[1,|]}Spell[${IniSpell.Arg[2,|]}]}" gem${${IniSpell.Arg[1,|]}Gem[${IniSpell.Arg[2,|]}]} 3s CastCheck ${If[${Spell[${${IniSpell.Arg[1,|]}Spell[${IniSpell.Arg[2,|]}]}].SpellType.Equal[detrimental]},-targetid|${ACMATarget},-targetid|${Spawn[${TankName}].ID}]}
   /echo GoMNuke : ${${IniSpell.Arg[1,|]}Spell[${IniSpell.Arg[2,|]}]}
/return


Sub FindAlias(SpellAlias)
   /declare tCastClass string local AH AD AB AE AQ
   /declare d int local
   /declare x int local
   /declare y int local
   /declare tSpell string local
   /for d 1 to ${tCastClass.Count[ ]}
      /if (${${tCastClass.Arg[${d}]}Count}) {
          /for x 1 to ${${tCastClass.Arg[${d}]}Count}
             /for y 1 to ${${tCastClass.Arg[${d}]}SpellAlias[${x}].Count[|]}
                /if (${${tCastClass.Arg[${d}]}SpellAlias[${x}].Arg[${y},|].Equal[${SpellAlias}]}) {
                   /varset tSpell ${tSpell}${tCastClass.Arg[${d}]}|${x}|
                }
             /next y
          /next x
      }
   /next d
|  /echo tSpell - ${tSpell}
|  AHSpell5 would return as AH|5|
/return ${tSpell}


Sub Event_OutOfStuff
   /if (${DoCombines}) {
      /echo Out of materials.  Setting DoCombines FALSE
      /varset DoCombines FALSE
   }
/return


Sub IsNumber(Index)
   /if (!${Index.Length}) /return
   /declare x int local
   /for x 1 to ${Index.Length}
      /if (!${Select[${Index.Mid[${x},1]},0,1,2,3,4,5,6,7,8,9,.]}) {
         /echo IsNumber - FALSE - Index was ${Index}
         /return FALSE
      }
   /next x
/return TRUE


Sub SearchVendor
|  /declare CurMerchant int outer 0 - Need to set this to the ID of merchant window.  Grab name from window.
   /declare ICount int local
   /declare mItem int local 0
   /declare mItemName string local
   /if (!${Window[MerchantWnd].Open}) {
      /varset CurMerchant 0
      /return
   }
   /declare VendorName string local ${Window[MerchantWnd].Child[MW_MerchantName].Text}
:MerchPopulate
   /varset ICount ${Merchant.Items}
   /delay 1s
   /if (${ICount}!=${Merchant.Items}) /goto :MerchPopulate
   /varset CurMerchant ${Spawn[merchant ${Window[MerchantWnd].Child[MW_MerchantName].Text}].ID}
   /for mItem 1 to ${Merchant.Items}
      /varset mItemName ${Merchant.Item[${mItem}].Name}
      /if (${Ini["${LootIni}","${mItemName.Left[1]}","${mItemName}"].Find[Buy]}) {
         /echo ${mItemName} found on ${VendorName}
         /echo ${Ini["${LootIni}","${mItemName.Left[1]}","${mItemName}"]}
      }
   /next mItem
/return


| AAPurchasing Revamped - 11/01/2017
| Codebase from AAPurchase.inc - customized, restructured, and integrated into ModBot.

Sub AAInit(IniSection) 
   /declare ListNum string outer 
   /declare a int local 
   /if (!${IniSection.Length}) /varset IniSection AAtoBuy 
   /if (!${Ini["${AAIniFile}","${IniSection}"].Length} ) {
      /echo Creating AA_${Me.Name}.ini file
      /call AADump
   }
   /call LoadINIVar ${IniSection} AACount 0 AACount ${AAIniFile} int 
   /if (${AACount}) { 
      /declare AA[${AACount}] string outer 
      /for a 1 to ${AACount} 
         /call LoadINIVar ${IniSection} AA${a} " " AA[${a}] ${AAIniFile} 
      /next a 
      /if (${DoAA}) /call AAPicker force
   } 
/return 


Sub AAPicker(string aaParm)
   /if (${MBPause} || !${AACount} || ${Me.AAPoints}<=0) /return
| If not Fast-AA purchase, and Confirmation box is open from something else, skip AA checking for the moment so as not to accidentally click it
   /if (!${Window[OptionsWindow].Child[OptionsGeneralPage].Child[OGP_AANoConfirmCheckbox].Checked} && ${Window[ConfirmationDialogBox].Open}) /return
   /varset AACheckTimer 5m

   /declare x int local 
   /declare Cost int local 
   /declare AALevelStr string local 
   /declare curAALevel int local 
   /declare maxAALevel int local 
   /declare maxPurchaseLevel int local 
   /declare nameAA string local 
   /declare requiredLevel int local 
   /declare AACategory string local 
   /declare maxCounter int local 0 
   /declare myMax int local ${Math.Calc[${Me.Level}*2]}
   /declare AAtoSpend int local
   /declare lvlStart int local
   /declare lvlEnd int local
   /declare wantSomething bool local FALSE
   /declare wantExpendable bool local FALSE
   /declare doAnnounce string local 
   /declare doVerbose bool local
   /declare newAAstr string local 

   /if (${aaParm.Equal[force]}) {
      /if (${AAAnnounce.Length}) {
         /varset doAnnounce ${AAAnnounce}
      } else {
         /varset doAnnounce /bc
      }
      /varset doVerbose TRUE
   } else {
      /varset doAnnounce ${AAAnnounce} 
      /varset doVerbose ${AAVerbose}
   } 

   /varcalc AAtoSpend ${Me.AAPoints}-${AABank}
   /if (${AAtoSpend}<=0) {
      /if (${doAnnounce.Length} && ${doVerbose}) /docommand ${doAnnounce} Unable to purchase AA's at this time. You have no spendable AA.
      /return
   }

   /for x 1 to ${AACount} 
      /if (!${AA[${x}].Arg[2,"|"].Length} || ${AA[${x}].Arg[2,"|"].Equal[X]}) /goto :nextAA
      /varset nameAA ${AA[${x}].Arg[1,"|"]} 
| Sel ListNum to the page the AA is on. 
      /varset ListNum List${AltAbility[${nameAA}].Type} 
:BuyAdditional
| Clear the Can Purchase button and Filters
      /nomodkey /notify AAWindow ResetFilter leftmouseup
| Check if the skill is available to you. 
      /if (${AltAbility[${nameAA}].ID}) { 
         /varset AALevelStr ${Window[AAWindow].Child[${ListNum}].List[${Window[AAWindow].Child[${ListNum}].List[=${nameAA}]}, 2]} 
         /varset curAALevel ${AALevelStr.Arg[1,"/"]} 
         /varset maxAALevel ${AALevelStr.Arg[2,"/"]} 
         /if (${AA[${x}].Arg[2,"|"].Equal[M]}) {
            /varset maxPurchaseLevel ${maxAALevel}
            /if (${curAALevel}>=${maxPurchaseLevel}) {
               /varset newAAstr ${AA[${x}].Arg[1,"|"]}|X
               /if (${AA[${x}].Arg[3,"|"].Length}) /varset newAAstr ${newAAstr}|${AA[${x}].Arg[3,"|"]}
               /ini "${AAIniFile}" AAtoBuy AA${x} "${newAAstr}" 
               /goto :nextAA
            }
         } else { 
            /varset maxPurchaseLevel ${AA[${x}].Arg[2,"|"]} 
            /if (${maxPurchaseLevel}<=0) /goto :nextAA
            /if (${maxPurchaseLevel}>${maxAALevel}) /varset maxPurchaseLevel ${maxAALevel}
         }
         /varset AACategory ${Window[AAWindow].Child[${ListNum}].List[${Window[AAWindow].Child[${ListNum}].List[=${nameAA}]}, 5]}
         /if (${AACategory.Equal[Expendable]}) {
            /varset wantExpendable TRUE  
            /varset AACheckTimer 1m
         }
         /if (${curAALevel}<${maxPurchaseLevel}) {
            /varset wantSomething TRUE 
            /varset Cost ${Window[AAWindow].Child[${ListNum}].List[${Window[AAWindow].Child[${ListNum}].List[=${nameAA}]}, 3]} 
| Click the Can Purchase button. 
            /if (!${Window[AAWindow].Child[CanPurchaseFilter].Checked}) /nomodkey /notify AAWindow CanPurchaseFilter leftmouseup 
| If the Cost of the ability is less than or equal to your spendable AA and if the AA still shows in the list, purchase it. 
            /if (${Cost}<=${AAtoSpend} && !${Window[AAWindow].Child[${ListNum}].List[=${nameAA}]}==NULL) {
               /call AASelect "${nameAA}"   
               /call AAPurchase 
               /varcalc curAALevel ${curAALevel}+1 
               /if (${doAnnounce.Length}) /docommand ${doAnnounce} ${Time} Purchased level ${curAALevel}/${maxAALevel} of ${nameAA} for ${Cost} points. 
               /delay 2s 
| If I still have points after purchasing try to purchase again. 
               /if (${Me.AAPoints}>0 && ${Me.AAPoints}>${AABank}) {
                  /varcalc AAtoSpend ${Me.AAPoints}-${AABank}
                  /if (${AAtoSpend}>0) /goto :BuyAdditional
               }
               /return
            }
| Check if Cost is greater than spendable AA. 
            /if (${Cost}>${AAtoSpend}) { 
               /if (${doAnnounce.Length} && ${doVerbose}) /docommand ${doAnnounce} Unable to purchase ${nameAA} at this time. You have ${AAtoSpend} spendable AA and need ${Cost} AA. 
               /if (${AA[${x}].Arg[3,"|"].Equal[S]}) {
                  /if (${doAnnounce.Length} && ${doVerbose}) /docommand ${doAnnounce} Checking if other AA are available.
                  /goto :nextAA
               }
               /return
            } 
| If we aren't announcing or not verbose, we're done with this AA.
            /if (!${doAnnounce.Length} || !${doVerbose}) /goto :nextAA
| The following is only for Verbose announcements
| Check if you are not the right level.
            /call AASelect "${nameAA}"  
            /varset lvlStart ${Math.Calc[${Window[AAWindow].FirstChild.Next.Text.Find[Level:]} + 7]}
            /if (${lvlStart}) {
               /varset lvlEnd ${Window[AAWindow].FirstChild.Next.Text.Right[${Math.Calc[-${lvlStart}+1]}].Find[,]}
               /if (${lvlEnd}>1) {
                  /varset requiredLevel ${Window[AAWindow].FirstChild.Next.Text.Mid[${lvlStart},${Math.Calc[${lvlEnd}-1]}]}
                  /if (${requiredLevel}>${Me.Level}) { 
                     /docommand ${doAnnounce} You are Level ${Me.Level} and ${nameAA} requires Level ${requiredLevel}. Skipping.
                     /goto :nextAA
                  }
               }
            }
| You must not have the expansion or other pre-requisites
            /if (${Window[AAWindow].Child[AAW_ExpansionFilter].List[AACategory]}) {  
               /docommand ${doAnnounce} You have ${AAtoSpend} AA to spend and ${nameAA} costs ${Cost} AA. You must not have ${AACategory} or you have not met the prerequisites. Skipping.
            } else {
               /docommand ${doAnnounce} You have ${AAtoSpend} AA to spend and ${nameAA} costs ${Cost} AA. You must not have met the prerequisites. Skipping.
            }
| Else, the skill is maxed. 
         } else { 
            /if (${doAnnounce.Length} && ${doVerbose} && ${curAALevel}==${maxAALevel} && ${maxPurchaseLevel}==${maxAALevel}) /docommand ${doAnnounce} ${nameAA} is Maxed 
         } 
| Else, that skill is not available. 
      } else { 
         /if (${doAnnounce.Length}) /docommand ${doAnnounce} ${nameAA} is not a valid ${Me.Class} Ability. 
      } 
:nextAA 
   /next x 
| If all skills are maxed
   /if (!${wantSomething} && !${wantExpendable}) { 
      /if (${DoAA}) {
         /if (${doAnnounce.Length}) /docommand ${doAnnounce} No defined skills or all defined skills maxed. Setting DoAA FALSE.
         /varset DoAA FALSE
      } else {
         /if (${doAnnounce.Length}) /docommand ${doAnnounce} No defined skills or all defined skills maxed.
      }
| Else, if not, wait for next round. 
   } else { 
      /if (${doAnnounce.Length} && ${doVerbose}) /docommand ${doAnnounce} Cycled through all AA and nothing to purchase this round. 
   } 
| If banked AA is greater than or equal to warning level and less than max level, issue a warning. 
   /if (${Me.AAPoints}>=${AAWarning} && ${Me.AAPoints}<${myMax} && ${doAnnounce.Length}) /docommand ${doAnnounce} Warning! You are at ${Me.AAPoints} banked AA. 
| If banked AA equals max, turn off alt exp (as requested). 
   /if (${Me.AAPoints}>=${myMax}) { 
      /if (${doAnnounce.Length} && ${doVerbose}) /docommand ${doAnnounce} You have maxed your AAs and are unable to purchase any of your defined AAs.
      /if (${AAtoNormal}) {
         /if (${doAnnounce.Length}) /docommand ${doAnnounce} Changing you to full normal XP.
         /alt off
      } 
   } 
/return 


Sub AASelect(string AAName)
   /nomodkey /notify AAWindow ResetFilter leftmouseup
   /nomodkey /notify AAWindow AAW_Subwindows tabselect ${AltAbility[${AAName}].Type}
   /nomodkey /notify AAWindow ${ListNum} listselect ${Window[AAWindow].Child[${ListNum}].List[=${AAName}]}
   /nomodkey /notify AAWindow ${ListNum} leftmouse ${Window[AAWindow].Child[${ListNum}].List[=${AAName}]} 
/return 


| NextIndex doesn't always work, so we will stick with clicking the button
|Sub AAPurchase(string AAName)
|   /if (${AltAbility[${AAName}].NextIndex}>0) {
|      /alt buy ${AltAbility[${AAName}].NextIndex}
|   } else {
|      /alt buy ${AltAbility[${AAName}].ID}
|   }
|   /delay 2
|/return
 
Sub AAPurchase 
   /nomodkey /notify AAWindow TrainButton leftmouseup 
| If fast AA purchase is not on, accept the purchase. 
   /if (!${Window[OptionsWindow].Child[OptionsGeneralPage].Child[OGP_AANoConfirmCheckbox].Checked}) { 
      /delay 10s ${Window[ConfirmationDialogBox].Open}
      /if (!${Window[ConfirmationDialogBox].Open}) {
         /echo AA confirmation box not showing. Your AA purchase may not have been made.
         /return
      }
      /delay 2 
      /nomodkey /notify ConfirmationDialogBox Yes_Button leftmouseup 
   }
   /delay 2 
/return 

Sub Event_AAEarned
   /varset AACheckTimer
/return 


Sub AADump
   /declare a int local 
   /declare b int local
   /declare c int local
   /declare AAcnt int local 1 
   /ini ${AAIniFile} AAtoBuy AACount 0 
   /declare AALevelStr string local
   /declare AACostStr string local
   /declare AACategory string local
   /declare AATabs string local 35214 

   /nomodkey /notify AAWindow ResetFilter leftmouseup
   /delay 2

   /for c 1 to 5
      /varset a 1
      /varset b ${AATabs.Mid[${c},1]} 
:Loop 
      /if (${Window[AAWindow].Child[List${b}].List[${a}].Length}) { 
         /varset AALevelStr ${Window[AAWindow].Child[List${b}].List[${a}, 2]} 
         /varset AACostStr ${Window[AAWindow].Child[List${b}].List[${a}, 3]} 
         /varset AACategory ${Window[AAWindow].Child[List${b}].List[${a}, 5]} 
         /if (${AACategory.Equal[Expendable]} || (${AALevelStr.Arg[1,"/"]}<${AALevelStr.Arg[2,"/"]} && ${AACostStr.Length} && ${AACostStr.NotEqual[0]})) { 
            /ini "${AAIniFile}" AAtoBuy AA${AAcnt} "${Window[AAWindow].Child[List${b}].List[${a}]}|" 
            /varcalc AAcnt ${AAcnt}+1 
         } 
         /varcalc a ${a}+1 
         /goto :Loop 
      } 
   /next c
   /ini "${AAIniFile}" AAtoBuy AACount ${Int[${Math.Calc[${AAcnt} -1]}]} 
   /echo Finished dump to file: ${AAIniFile} 
/return