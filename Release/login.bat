cls
@ECHO off
CD C:\Program Files (x86)\WinEQ2\
timeout /t 5
@ECHO Launching MY SK
START WinEQ2.exe /plugin:WinEQ2-EQ.dll "MYSK"
timeout /t 20
@ECHO Launching MY Cleric
START WinEQ2.exe /plugin:WinEQ2-EQ.dll "MYCleric"
timeout /t 20
@ECHO Launching MY Beastlord
START WinEQ2.exe /plugin:WinEQ2-EQ.dll "MYBeasty"
timeout /t 20
@ECHO Launching MY Mage
START WinEQ2.exe /plugin:WinEQ2-EQ.dll "MYMage"
timeout /t 20
@ECHO Launching MY Chanter
START WinEQ2.exe /plugin:WinEQ2-EQ.dll "MYChanter"
timeout /t 20
@ECHO Launching MY Wizard
START WinEQ2.exe /plugin:WinEQ2-EQ.dll "MYWiz1"
timeout /t 20
@ECHO Launching MY Wizard
START WinEQ2.exe /plugin:WinEQ2-EQ.dll "MYWiz2"
timeout /t 20
@ECHO Launching MY zerker
START WinEQ2.exe /plugin:WinEQ2-EQ.dll "MYZerker"