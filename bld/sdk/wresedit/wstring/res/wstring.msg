// the following is the font used in the status bar of the window
pick(   W_STATUSFONT,
        "Helv.8",
        "Helv.8" )

// WSTAT.C
pick(   W_STATUSNOTCREATED,
        "Could not create status line.",
        "Could not create status line." )
pick(   W_READYMSG,
        "Ready.",
        "Ready." )

// WEDIT.C
pick(   W_CHANGEMODIFIEDMENUITEM,
        "This string has been modified.\nChange?",
        "This string has been modified.\nChange?" )

// WHINTS.C
pick(   W_STR_CLEAR,
        "Clear the string table.",
        "Clear the string table." );
pick(   W_STR_UPDATE,
        "Update the file with this string table.",
        "Update the file with this string table." )
pick(   W_STR_SAVE,
        "Save this string table.",
        "Save this string table." )
pick(   W_STR_SAVEAS,
        "Save this string table to a different file.",
        "Save this string table to a different file." )
pick(   W_STR_SAVEINTO,
        "Copy this string table into a different file.",
        "Copy this string table into a different file." )
pick(   W_STR_EXIT,
        "Exit the string table editor.",
        "Exit the string table editor." )
pick(   W_STR_CUT,
        "Cut the string table to the clipboard.",
        "Cut the string table to the clipboard." )
pick(   W_STR_COPY,
        "Copy the string table to the clipbord.",
        "Copy the string table to the clipbord." )
pick(   W_STR_PASTE,
        "Paste the string table from the clipboard.",
        "Paste the string table from the clipboard." )
pick(   W_STR_DELETE,
        "Delete a string from the string table.",
        "Delete a string from the string table." )
pick(   W_STR_SHOWRIBBON,
        "Show/hide the toolbar.",
        "Show/hide the toolbar." )
pick(   W_STR_SYMBOLS,
        "Edit Symbols.",
        "Edit Symbols." )
pick(   W_STR_LOAD_SYMBOLS,
        "Load Symbols.",
        "Load Symbols." )
pick(   W_STR_MEM_FLAGS,
        "Modify the string table memory options.",
        "Modify the string table memory options." )
pick(   W_STR_NEWITEM,
        "Insert a new string.",
        "Insert a new string." )
pick(   W_STR_ABOUT,
        "Display program information.",
        "Display program information." )
pick(   W_STR_HELP,
        "List Open Watcom String Editor help topics.",
        "List Open Watcom String Editor help topics." )
pick(   W_STR_HELP_SEARCH,
        "Search for help on a specific topic.",
        "Search for help on a specific topic." )
pick(   W_STR_HELP_ON_HELP,
        "Display information on how to use help.",
        "Display information on how to use help." )
pick(   W_STR_FILEMENU,
        "Clear or update the string table.",
        "Clear or update the string table." )
pick(   W_STR_EDITMENU,
        "Edit the string table and its items.",
        "Edit the string table and its items." )
pick(   W_STR_RESMENU,
        "Manipulate string table settings.",
        "Manipulate string table settings." )
pick(   W_STR_STRMENU,
        "Create string table items.",
        "Create string table items." )
pick(   W_STR_HELPMENU,
        "Display help or editor info.",
        "Display help or editor info." )

// WRIBBON.C
pick(   W_SHOWTOOLBAR,
        "Show Tool&bar",
        "Show Tool&bar" )
pick(   W_TOOLBARHIDDEN,
        "Toolbar Hidden",
        "Toolbar Hidden" )
pick(   W_HIDETOOLBAR,
        "Hide Tool&bar",
        "Hide Tool&bar" )
pick(   W_TOOLBARSHOWN,
        "Toolbar Shown",
        "Toolbar Shown" )
pick(   W_TIP_NEW,
        "New",
        "New" )
pick(   W_TIP_OPEN,
        "Open",
        "Open" )
pick(   W_TIP_SAVE,
        "Save",
        "Save" )
pick(   W_TIP_CLEAR,
        "Clear",
        "Clear" )
pick(   W_TIP_UPDATE,
        "Update",
        "Update" )
pick(   W_TIP_CUT,
        "Cut",
        "Cut" )
pick(   W_TIP_COPY,
        "Copy",
        "Copy" )
pick(   W_TIP_PASTE,
        "Paste",
        "Paste" )
pick(   W_TIP_NEWITEM,
        "Insert New Item",
        "Insert New Item" )
pick(   W_TIP_DELETE,
        "Delete",
        "Delete" )

// WTOOLBAR.C
pick(   W_SYSMENUMOVE,
        "Move",
        "Move" )
pick(   W_SYSMENUSIZE,
        "Size",
        "Size" )
pick(   W_SYSMENUHIDE,
        "Hide",
        "Hide" )

// WSVOBJ.C
pick(   W_SAVERESINTO,
        "Save String Table Into Resource",
        "Save String Table Into Resource" )
pick(   W_SAVERESAS,
        "Save String Table Resource",
        "Save String Table Resource" )
pick(   W_SAVERESFILTER,
        "String Files (*.STR)\t*.STR\tResource Files (*.RES)\t*.RES\tExecutable Files (*.EXE)\t*.EXE\tDLL Libraries (*.DLL)\t*.DLL\t",
        "String Files (*.STR)\t*.STR\tResource Files (*.RES)\t*.RES\tExecutable Files (*.EXE)\t*.EXE\tDLL Libraries (*.DLL)\t*.DLL\t" )
pick(   W_NORESOLVEDUPS,
        "Could not resolve duplicate string entries in the target file.",
        "Could not resolve duplicate string entries in the target file." )
pick(   W_STRINGTABLEISEMPTY,
        "The string table is empty.",
        "The string table is empty." )

// WMAIN.C
pick(   W_STRINGAPPTITLE,
        "Open Watcom String Editor",
        "Open Watcom String Editor" )
pick(   W_CHANGESTRINGMEMFLAGS,
        "Changing Memory Flags",
        "Changing Memory Flags" )
pick(   W_UPDATEMODIFIEDSTRING,
        "This string resource has been modified.\nSave?",
        "This string resource has been modified.\nSave?" )
pick(   W_STRINGCLEARWARNING,
        "Clear will delete ALL strings.\nClear?",
        "Clear will delete ALL strings.\nClear?" )
pick(   W_STRINGCLEARTITLE,
        "Clear String Table",
        "Clear String Table" )
pick(   W_STRINGCLEARMSG,
        "String table cleared",
        "String table cleared" )
pick(   W_STRINGNAMES,
        "Strings %d - %d",
        "Strings %d - %d" )
pick(   W_UPDATEMODIFIEDSYM,
        "The symbols have been modified.\nSave?",
        "The symbols have been modified.\nSave?" )
pick(   W_ABOUT_NAME,
        "Open Watcom String Editor for Windows" STR_BITNEES,
        "Open Watcom String Editor for Windows" STR_BITNEES )
pick(   W_ABOUT_VERSION,
        banner1p2( _RESEDIT_VERSION_ ),
        banner1p2( _RESEDIT_VERSION_ ) )
pick(   W_ABOUT_COPYRIGHT_YEAR,
        "2002",
        "2002" )
pick(   W_ABOUT_TITLE,
        "About Open Watcom String Editor",
        "About Open Watcom String Editor" )

// WNEWITEM.C
pick(   W_QUERYREPLACESTRING,
        "There is already a string with this id.\nDo you wish to replace it?",
        "There is already a string with this id.\nDo you wish to replace it?" )
pick(   W_INSERTNEWSTRING,
        "Insert new string",
        "Insert new string" )

#include "w.msg"
