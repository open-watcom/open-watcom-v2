rcinclude "os2.h"










































































STRINGTABLE
BEGIN
    
	
    
     
 

 
		
		1, "First" 
		
 

 
		
		2, "Change &Font ..." 

 
		
		3, "Change Font for this window" 

 

 
		
		4, "Floating Toolbar" 
 

 
		
		5, "&Fix Tool Bar" 

 
		
		6, "Make Tool Bar Fixed" 

 

 
		
		7, "Could Not Create Required Pen" 

 
		
		8, "Could Not Open Window" 

 

 
		
		9, "File Name:" 

 
		
		10, "Directories:" 

 
		
		11, "OK" 

 
		
                12, "Cancel" 

 
		
		13, "List Files of Type:" 

 
		
		14, "Drives:" 

 
		
		15, "\nThis file already exists.\n\nReplace existing file?\n" 

 
		
		16, "Save" 

 
		
		17, "Open" 

 

 
		
		18, "Restore" 

 
		
		19, "Move" 

 
		
		20, "Size" 

 
		
		21, "Minimize" 

 
		
		22, "Maximize" 

 
		
		23, "Close" 

 

 
		
		24, "Nex&t" 

 
		
		25, "&Close" 

 

 
		
		26, "&Cascade" 

 
		
		27, "Tile &Horizontally" 

 
		
		28, "Tile &Vertically" 

 
		
		29, "&Arrange Icons" 

 
		
		30, "&More Windows ..." 

 
		
		31, "Arrange all windows in a cascaded fashion" 

 
		
		32, "Arrange all windows in a horizontally tiled fashion" 

 
		
		33, "Arrange all windows in a vertically tiled fashion" 

 
		
		34, "Arrange all icons in rows" 

 
		
		35, "Show a list of all windows" 

 
		
		36, "Go to Window " 

 
		
		37, "Select Window" 

 

 
		
		38, "&Fix Toolbar" 

 
		
		39, "Make toolbar buttons into menus" 

 

 
		
		40, "&Abort" 

 
		
		41, "&Ignore" 

 
		
		42, "&No" 

 
		
		43, "&Retry" 

 
		
		44, "&Yes" 

 
		
		45, "Stop!" 

 
		
		46, "!" 


 

 
		
		47, "Colour" 

 
		
		48, "Black" 

 
		
		49, "Blue" 

 
		
		50, "Green" 

 
		
		51, "Cyan" 

 
		
		52, "Red" 

 
		
		53, "Magenta" 

 
		
		54, "Brown" 

 
		
		55, "White" 

 
		
		56, "Grey" 

 
		
		57, "Bright Blue" 

 
		
		58, "Bright Green" 

 
		
		59, "Bright Cyan" 

 
		
		60, "Bright Red" 

 
		
		61, "Bright Magenta" 

 
		
		62, "Bright Yellow" 

 
		
		63, "Bright White" 

 
		
		64, "table of contents" 
		

 

 
		
		65, "ApplIcon" 
		

 
		
		66, "" 


 
		
		67, " " 

 
		
		68, "Helv.8" 

 
		
		69, "System Proportional.10" 
		
 
 

 
		
		70, "Last" 

END



















































STRINGTABLE
BEGIN
    
    
    

    

 
	
	402,"Busy ..." 

 
	
	403,"Not Found" 

 
	
	404,"Search Wrapped" 

 
	
	405,"End of Window Reached" 

 
	
	406,"Top of Window Reached" 

 
	
	407,"Match Mode: " 

 
	
	408,"OK" 

 
	
	409,"Cancel" 


END



DLGTEMPLATE  501 
BEGIN
    DIALOG "Regular Expression",  501 , 16, 40, 150, 187, WS_VISIBLE,
		 FCF_TITLEBAR | WS_VISIBLE | FCF_SYSMENU
    PRESPARAMS PP_FONTNAMESIZE, "..\h\dlgrx.dlg"
    BEGIN
        CONTROL		"^", 0, 4, 165, 18, 11, WC_STATIC
		 , SS_TEXT | DT_LEFT | WS_VISIBLE | DT_MNEMONIC
        CONTROL		"~Start of Line",  100 , 23, 165, 122, 11, WC_BUTTON
		 , BS_AUTOCHECKBOX | WS_VISIBLE | WS_TABSTOP | DT_MNEMONIC
        CONTROL		"$", 0, 4, 153, 18, 11, WC_STATIC
		 , SS_TEXT | DT_LEFT | WS_VISIBLE | DT_MNEMONIC
        CONTROL		"~End of Line",  101 , 23, 153, 122, 11, WC_BUTTON
		 , BS_AUTOCHECKBOX | WS_VISIBLE | WS_TABSTOP | DT_MNEMONIC
        CONTROL		"\\", 0, 4, 141, 18, 11, WC_STATIC
		 , SS_TEXT | DT_LEFT | WS_VISIBLE | DT_MNEMONIC
        CONTROL		"~Toggle RX Character",  102 , 23, 141, 122, 11, WC_BUTTON
		 , BS_AUTOCHECKBOX | WS_VISIBLE | WS_TABSTOP | DT_MNEMONIC
        CONTROL		".", 0, 4, 130, 18, 11, WC_STATIC
		 , SS_TEXT | DT_LEFT | WS_VISIBLE | DT_MNEMONIC
        CONTROL		"~Any Character",  103 , 23, 130, 122, 11, WC_BUTTON
		 , BS_AUTOCHECKBOX | WS_VISIBLE | WS_TABSTOP | DT_MNEMONIC
        CONTROL		"[]", 0, 4, 118, 18, 11, WC_STATIC
		 , SS_TEXT | DT_LEFT | WS_VISIBLE | DT_MNEMONIC
        CONTROL		"~Character Group",  104 , 23, 118, 122, 11, WC_BUTTON
		 , BS_AUTOCHECKBOX | WS_VISIBLE | WS_TABSTOP | DT_MNEMONIC
        CONTROL		"()", 0, 4, 106, 18, 11, WC_STATIC
		 , SS_TEXT | DT_LEFT | WS_VISIBLE | DT_MNEMONIC
        CONTROL		"E~xpression Group",  105 , 23, 106, 122, 11, WC_BUTTON
		 , BS_AUTOCHECKBOX | WS_VISIBLE | WS_TABSTOP | DT_MNEMONIC
        CONTROL		"|", 0, 4, 94, 18, 11, WC_STATIC
		 , SS_TEXT | DT_LEFT | WS_VISIBLE | DT_MNEMONIC
        CONTROL		"A~lternate Expression",  106 , 23, 94, 122, 11, WC_BUTTON
		 , BS_AUTOCHECKBOX | WS_VISIBLE | WS_TABSTOP | DT_MNEMONIC
        CONTROL		"?", 0, 4, 83, 18, 11, WC_STATIC
		 , SS_TEXT | DT_LEFT | WS_VISIBLE | DT_MNEMONIC
        CONTROL		"~Zero or One Times",  107 , 23, 83, 122, 11, WC_BUTTON
		 , BS_AUTOCHECKBOX | WS_VISIBLE | WS_TABSTOP | DT_MNEMONIC
        CONTROL		"+", 0, 4, 71, 18, 11, WC_STATIC
		 , SS_TEXT | DT_LEFT | WS_VISIBLE | DT_MNEMONIC
        CONTROL		"~One or More Times",  108 , 23, 71, 122, 11, WC_BUTTON
		 , BS_AUTOCHECKBOX | WS_VISIBLE | WS_TABSTOP | DT_MNEMONIC
        CONTROL		"*", 0, 4, 59, 18, 11, WC_STATIC
		 , SS_TEXT | DT_LEFT | WS_VISIBLE | DT_MNEMONIC
        CONTROL		"Zero or ~More Times",  109 , 23, 59, 122, 11, WC_BUTTON
		 , BS_AUTOCHECKBOX | WS_VISIBLE | WS_TABSTOP | DT_MNEMONIC
        CONTROL		"~", 0, 4, 47, 18, 11, WC_STATIC
		 , SS_TEXT | DT_LEFT | WS_VISIBLE | DT_MNEMONIC
        CONTROL		"~Ignore case",  110 , 23, 47, 122, 11, WC_BUTTON
		 , BS_AUTOCHECKBOX | WS_VISIBLE | WS_TABSTOP | DT_MNEMONIC
        CONTROL		"@", 0, 4, 36, 18, 11, WC_STATIC
		 , SS_TEXT | DT_LEFT | WS_VISIBLE | DT_MNEMONIC
        CONTROL		"~Respect case",  111 , 23, 36, 122, 11, WC_BUTTON
		 , BS_AUTOCHECKBOX | WS_VISIBLE | WS_TABSTOP | DT_MNEMONIC
        CONTROL		"OK",  112 , 18, 12, 51, 11, WC_BUTTON
		 , BS_DEFAULT | WS_VISIBLE | WS_TABSTOP | DT_MNEMONIC
        CONTROL		"Cancel",  113 , 84, 12, 51, 11, WC_BUTTON
		 , BS_PUSHBUTTON | WS_VISIBLE | WS_TABSTOP | DT_MNEMONIC
    END
END



DLGTEMPLATE  502 
BEGIN
    DIALOG "Enter Search String",  502 , 4, 79, 197, 140, WS_VISIBLE
		, FCF_TITLEBAR
		 | WS_VISIBLE | FCF_SYSMENU
    PRESPARAMS PP_FONTNAMESIZE, "..\h\dlgsrch.dlg"
    BEGIN
        CONTROL		"",  103 , 9, 119, 183, 10, WC_ENTRYFIELD
		 , ES_AUTOSCROLL | ES_LEFT | WS_VISIBLE | WS_TABSTOP
        CONTROL		"",  104 , 9, 70, 183, 35, WC_LISTBOX
		 , WS_VISIBLE | WS_TABSTOP
        CONTROL		"Regular E~xpression",  106 , 4, 47, 122, 11, WC_BUTTON
		 , BS_AUTOCHECKBOX | WS_VISIBLE | WS_TABSTOP | DT_MNEMONIC
        CONTROL		"~Edit ...",  107 , 126, 47, 56, 11, WC_BUTTON
		 , BS_PUSHBUTTON | WS_VISIBLE | WS_TABSTOP | DT_MNEMONIC
        CONTROL		"~Ignore Case",  105 , 4, 36, 122, 11, WC_BUTTON
		 , BS_AUTOCHECKBOX | WS_VISIBLE | WS_TABSTOP | DT_MNEMONIC
        CONTROL		"~Next",  100 , 18, 12, 51, 11, WC_BUTTON
		 , BS_DEFAULT | WS_VISIBLE | WS_TABSTOP | DT_MNEMONIC
        CONTROL		"~Previous",  101 , 75, 12, 51, 11, WC_BUTTON
		 , BS_PUSHBUTTON | WS_VISIBLE | WS_TABSTOP | DT_MNEMONIC
        CONTROL		"Cancel",  102 , 131, 12, 51, 11, WC_BUTTON
		 , BS_PUSHBUTTON | WS_VISIBLE | WS_TABSTOP | DT_MNEMONIC
    END
END



DLGTEMPLATE  503 
BEGIN
    DIALOG "Enter Search String",  503 , 4, 79, 197, 140, WS_VISIBLE
		, FCF_TITLEBAR
		 | WS_VISIBLE | FCF_SYSMENU
    PRESPARAMS PP_FONTNAMESIZE, "..\h\dlgsrcha.dlg"
    BEGIN
        CONTROL		"",  103 , 9, 119, 183, 10, WC_ENTRYFIELD
		 , ES_AUTOSCROLL | ES_LEFT | WS_VISIBLE | WS_TABSTOP
        CONTROL		"",  104 , 9, 70, 183, 35, WC_LISTBOX
		 , WS_VISIBLE | WS_TABSTOP
        CONTROL		"Regular E~xpression",  106 , 4, 47, 122, 11, WC_BUTTON
		 , BS_AUTOCHECKBOX | WS_VISIBLE | WS_TABSTOP | DT_MNEMONIC
        CONTROL		"~Edit ...",  107 , 126, 47, 56, 11, WC_BUTTON
		 , BS_PUSHBUTTON | WS_VISIBLE | WS_TABSTOP | DT_MNEMONIC
        CONTROL		"~Ignore Case",  105 , 4, 36, 122, 11, WC_BUTTON
		 , BS_AUTOCHECKBOX | WS_VISIBLE | WS_TABSTOP | DT_MNEMONIC
        CONTROL		"~OK",  100 , 18, 12, 51, 11, WC_BUTTON
		 , BS_DEFAULT | WS_VISIBLE | WS_TABSTOP | DT_MNEMONIC
        CONTROL		"Cancel",  102 , 131, 12, 51, 11, WC_BUTTON
		 , BS_PUSHBUTTON | WS_VISIBLE | WS_TABSTOP | DT_MNEMONIC
    END
END






ICON 1          "../bitmap/os2/prof_i.ico"
ICON  1002  "../bitmap/os2/prof_i.ico"
