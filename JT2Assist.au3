;; Setup tray icon
Opt("TrayOnEventMode", 1) ;; Enable event-based tray icon
Opt("TrayMenuMode", 3) ;; No default menu

;; Window find string
$lolWindow = "[CLASS:LeagueOfLegendsWindowClass]"

;; Create exec strings
$injectRunStr = 'winject /dllname:"' & @ScriptDir & '\jt2.dll" /exename:"league of legends.exe"'
$ejectRunStr = 'winject /dllname:"' & @ScriptDir & '\jt2.dll" /exename:"league of legends.exe" /eject'

If Not FileExists("winject.exe") Then
	MsgBox(4096+16,"JT2 Assist error", "Cannot find winject.exe. It needs to run jungle timer, it injects JT2 dll into game process. You can download it from http://www.cheat-project.com/cheats-hacks/1382/WinJect-1.7/ or http://www.ucdownloads.com/downloads/downloads.php?do=file&id=578 . Press Ctrl and C buttons to copy content of this message box, paste it into notepad, select URL from the text and paste it into you browser address bar!")
	Exit
EndIf

If Not FileExists("jt2.dll") Then
	MsgBox(4096+16,"JT2 Assist error", "Cannot find jt2.dll. Download or build latest version and place jt2.dll into this directory.")
	Exit
EndIf

Func WinjectInjectJT2()
    Local $res = RunWait($injectRunStr, @ScriptDir, @SW_HIDE)
	If $res == 0 Then
		TrayTip("JT2 Assist", "JT2 Loaded. Enjoy!", 1, 1)
	Else
		TrayTip("JT2 Assist", "Cannot load JT2. Exit code: " & $res, 1, 1)
	EndIf
EndFunc

Func WinjectEjectJT2()
    Local $res = RunWait($ejectRunStr, @ScriptDir, @SW_HIDE)
	If $res == 0 Then
		TrayTip("JT2 Assist", "JT2 Unloaded", 1, 1)
	Else
		TrayTip("JT2 Assist", "Cannot unload JT2. Exit code: " & $res, 1, 1)
	EndIf
EndFunc

TraySetState()
TraySetToolTip("JungleTime2 Assist")

;; Create menu items
$injectitem = TrayCreateItem("Start JT")
$ejectitem = TrayCreateItem("Unload JT")
TrayCreateItem("")
$exititem = TrayCreateItem("Exit")

;; Set callbacks
TrayItemSetOnEvent($injectitem, "OnInjectItem")
TrayItemSetOnEvent($ejectitem, "OnEjectItem")
TrayItemSetOnEvent($exititem, "OnExitItem")

;; Done...
TrayTip("JT2 Assist", "Loaded and waits for the LoL client", 1, 1)

;; Main loop
While 1
	LoLWinWait()
	TrayTip("JT2 Assist", "Game window found", 1, 1)
	Sleep(1000)
	WinjectInjectJT2()
	Sleep(1000)
	WinWaitClose($lolWindow)
	TrayTip("JT2 Assist", "Game window closed, waiting another one", 1, 1)
WEnd
TraySetState(2) ;; Destroys/Hides the tray icon


;;
;; FUNCTIONS
;;

;; Callbacks
Func OnInjectItem()
	If Not WinExists($lolWindow) Then
    	TrayTip("JT2 Assist", "Cannot inject! Game window not found.", 1, 1)
    Else
    	WinjectInjectJT2()
	EndIf
EndFunc

Func OnEjectItem()
    If Not WinExists($lolWindow) Then
    	TrayTip("JT2 Assist", "Cannot eject! Game window not found.", 1, 1)
    Else
    	WinjectEjectJT2()
	EndIf
EndFunc

Func OnExitItem()
	TraySetState(2) ;; Destroys/Hides the tray icon
    Exit
EndFunc

;; Other
Func LoLWinWait()
	WinWait($lolWindow)

	;; Small hack to avoid bugs(see issue 22)
	WinWaitClose("[CLASS:SplashScreenClassName]")
	Sleep(5000)
EndFunc