/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#define CM  ,

/*     name                           vartype           varname                initval             enum  */
PICK(  "AUTOSAVEINTerval",      "SF", int,              AutoSaveInterval,      30,                 AUTOSAVEINTERVAL )
PICK(  "BUTTONHeight",          "SF", int,              ToolBarButtonHeight,   24,                 TOOLBARBUTTONHEIGHT )
PICK(  "BUTTONWidth",           "SF", int,              ToolBarButtonWidth,    24,                 TOOLBARBUTTONWIDTH )
PICK(  "CLOCKX",                "SF", windim,           ClockX,                74,                 CLOCKX )
PICK(  "CLOCKY",                "SF", windim,           ClockY,                0,                  CLOCKY )
PICK(  "COMMANDCURSORTYpe",     "SF", cursor_type,      NormalCursorType,      {7 CM 0},           COMMANDCURSORTYPE )
PICK(  "CURRENTSTATUSColumn",   "SF", int,              CurrentStatusColumn,   56,                 CURRENTSTATUSCOLUMN )
PICK(  "CURSORBLINKRATE",       "SF", int,              CursorBlinkRate,       -1,                 CURSORBLINKRATE )
PICK(  "ENDOFLinechar",         "SF", char,             EndOfLineChar,         0,                  ENDOFLINECHAR )
PICK(  "EXITATTR",              "SF", viattr_t,         ExitAttr,              7,                  EXITATTR )
PICK(  "FIGNORE",               "SF", char *,           FIgnore,               NULL,               FIGNORE )
PICK(  "FILEENDString",         "SF", char *,           FileEndString,         NULL,               FILEENDSTRING )
#ifndef INITVARS
PICK(  "FILEName",              "SF", ,                 ,                      ,                   FILENAME )
#endif
PICK(  "GADGETSTRing",          "SF", char *,           GadgetString,          NULL,               GADGETSTRING )
PICK(  "GREPDefault",           "SF", char *,           GrepDefault,           NULL,               GREPDEFAULT )
PICK(  "HARDTab",               "SF", int,              HardTab,               8,                  HARDTAB )
PICK(  "HISTORYFile",           "SF", char *,           HistoryFile,           NULL,               HISTORYFILE )
PICK(  "INACTIVEWINDOWColor",   "SF", vi_color,         InactiveWindowColor,   WHITE,              INACTIVEWINDOWCOLOR )
PICK(  "INSERTCURSORType",      "SF", cursor_type,      InsertCursorType,      {50 CM 0},          INSERTCURSORTYPE )
#ifndef INITVARS
PICK(  "LANGUAGE",              "SF", ,                 ,                      ,                   LANGUAGE )
#endif
PICK(  "LINENUMWINWIdth",       "SF", int,              LineNumWinWidth,       8,                  LINENUMWINWIDTH )
PICK(  "MAGICSTRing",           "SF", char *,           Majick,                NULL,               MAGICSTRING )
#ifndef INITVARS
PICK(  "MAXCLHistory",          "SF", ,                 ,                      ,                   MAXCLHISTORY )
#endif
PICK(  "MAXEMSK",               "SF", int,              MaxEMSBlocks,          2048,               MAXEMSK )
#ifndef INITVARS
PICK(  "MAXFILTERHistory",      "SF", ,                 ,                      ,                   MAXFILTERHISTORY )
PICK(  "MAXFINDHistory",        "SF", ,                 ,                      ,                   MAXFINDHISTORY )
PICK(  "MAXLASTFILESHistory",   "SF", ,                 ,                      ,                   MAXLASTFILESHISTORY )
#endif
PICK(  "MAXLINElen",            "SF", int,              MaxLine,               512,                MAXLINELEN )
PICK(  "MAXPUSH",               "SF", int,              MaxPush,               15,                 MAXPUSH )
PICK(  "MAXSWAPK",              "SF", int,              MaxSwapBlocks,         2048,               MAXSWAPK )
PICK(  "MAXTILECOLORS",         "SF", int,              MaxTileColors,         10,                 MAXTILECOLORS )
PICK(  "MAXWINDOWTILEX",        "SF", int,              MaxWindowTileX,        3,                  MAXWINDOWTILEX )
PICK(  "MAXWINDOWTILEY",        "SF", int,              MaxWindowTileY,        1,                  MAXWINDOWTILEY )
PICK(  "MAXXMSK",               "SF", int,              MaxXMSBlocks,          2048,               MAXXMSK )
PICK(  "MOUSEDCLICKspeed",      "SF", int,              MouseDoubleClickSpeed, 3,                  MOUSEDCLICKSPEED )
PICK(  "MOUSEREPEATdelay",      "SF", int,              MouseRepeatDelay,      0,                  MOUSEREPEATDELAY )
PICK(  "MOUSESPeed",            "SF", int,              MouseSpeed,            4,                  MOUSESPEED )
PICK(  "MOVEColor",             "SF", vi_color,         MoveColor,             CYAN,               MOVECOLOR )
PICK(  "OVERSTRIKECURSORType",  "SF", cursor_type,      OverstrikeCursorType,  {100 CM 100},       OVERSTRIKECURSORTYPE )
PICK(  "PAGELinesexposed",      "SF", int,              PageLinesExposed,      1,                  PAGELINESEXPOSED )
PICK(  "RADix",                 "SF", int,              Radix,                 10,                 RADIX )
PICK(  "RESIZEColor",           "SF", vi_color,         ResizeColor,           YELLOW,             RESIZECOLOR )
PICK(  "SHELLPrompt",           "SF", char *,           SpawnPrompt,           NULL,               SHELLPROMPT )
PICK(  "SHIFTWIDth",            "SF", int,              ShiftWidth,            4,                  SHIFTWIDTH )
PICK(  "SPINX",                 "SF", windim,           SpinX,                 68,                 SPINX )
PICK(  "SPINY",                 "SF", windim,           SpinY,                 0,                  SPINY )
PICK(  "STACKk",                "SF", int,              StackK,                MIN_STACK_K,        STACKK )
PICK(  "STATUSSECtions",        "SF", unsigned short *, StatusSections,        NULL,               STATUSSECTIONS )
PICK(  "STATUSSTRing",          "SF", char *,           StatusString,          NULL,               STATUSSTRING )
PICK(  "TABamount",             "SF", int,              TabAmount,             8,                  TABAMOUNT )
PICK(  "TAGFILEname",           "SF", char *,           TagFileName,           NULL,               TAGFILENAME )
PICK(  "TILECOLOR",             "SF", type_style *,     TileColors,            NULL,               TILECOLOR )
PICK(  "TMPDIR",                "SF", char *,           TmpDir,                NULL,               TMPDIR )
PICK(  "TOOLBARColor",          "SF", vi_color,         ToolBarColor,          BLUE,               TOOLBARCOLOR )
PICK(  "WORD",                  "SF", char *,           WordDefn,              NULL,               WORD )
PICK(  "WORDALT",               "SF", char *,           WordAltDefn,           NULL,               WORDALT )
PICK(  "WRAPMARGIN",            "SF", int,              WrapMargin,            0,                  WRAPMARGIN )
#if 0
PICKL( "HARDTab",               "SF", int,              HardTab,               8,                  HARDTAB )
PICKL( "SHIFTWIDth",            "SF", int,              ShiftWidth,            4,                  SHIFTWIDTH )
PICKL( "TABamount",             "SF", int,              TabAmount,             8,                  TABAMOUNT )
#endif
