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

/*     name                              vartype       varname                initval             enum  */
PICK(  "AUTOSAVEINTerval",      "SF",   int,          AutoSaveInterval,      30,                 SET1_T_AUTOSAVEINTERVAL )
PICK(  "BUTTONHeight",          "SF",   int,          ToolBarButtonHeight,   24,                 SET1_T_TOOLBARBUTTONHEIGHT )
PICK(  "BUTTONWidth",           "SF",   int,          ToolBarButtonWidth,    24,                 SET1_T_TOOLBARBUTTONWIDTH )
PICK(  "CLOCKX",                "SF",   int,          ClockX,                74,                 SET1_T_CLOCKX )
PICK(  "CLOCKY",                "SF",   int,          ClockY,                0,                  SET1_T_CLOCKY )
PICK(  "COMMANDCURSORTYpe",     "SF",   cursor_type,  NormalCursorType,      {7 CM 0},           SET1_T_COMMANDCURSORTYPE )
PICK(  "CURRENTSTATUSColumn",   "SF",   int,          CurrentStatusColumn,   56,                 SET1_T_CURRENTSTATUSCOLUMN )
PICK(  "CURSORBLINKRATE",       "SF",   int,          CursorBlinkRate,       -1,                 SET1_T_CURSORBLINKRATE )
PICK(  "ENDOFLinechar",         "SF",   char,         EndOfLineChar,         0,                  SET1_T_ENDOFLINECHAR )
PICK(  "EXITATTR",              "SF",   viattr_t,     ExitAttr,              7,                  SET1_T_EXITATTR )
PICK(  "FIGNORE",               "SF",   char *,       FIgnore,               NULL,               SET1_T_FIGNORE )
PICK(  "FILEENDString",         "SF",   char *,       FileEndString,         NULL,               SET1_T_FILEENDSTRING )
#ifndef INITVARS
PICK(  "FILEName",              "SF",   ,             ,                      ,                   SET1_T_FILENAME )
#endif
PICK(  "GADGETSTRing",          "SF",   char *,       GadgetString,          NULL,               SET1_T_GADGETSTRING )
PICK(  "GREPDefault",           "SF",   char *,       GrepDefault,           NULL,               SET1_T_GREPDEFAULT )
PICK(  "HARDTab",               "SF",   int,          HardTab,               8,                  SET1_T_HARDTAB )
PICK(  "HISTORYFile",           "SF",   char *,       HistoryFile,           NULL,               SET1_T_HISTORYFILE )
PICK(  "INACTIVEWINDOWColor",   "SF",   vi_color,     InactiveWindowColor,   WHITE,              SET1_T_INACTIVEWINDOWCOLOR )
PICK(  "INSERTCURSORType",      "SF",   cursor_type,  InsertCursorType,      {50 CM 0},          SET1_T_INSERTCURSORTYPE )
#ifndef INITVARS
PICK(  "LANGUAGE",              "SF",   ,             ,                      ,                   SET1_T_LANGUAGE )
#endif
PICK(  "LINENUMWINWIdth",       "SF",   int,          LineNumWinWidth,       8,                  SET1_T_LINENUMWINWIDTH )
PICK(  "MAGICSTRing",           "SF",   char *,       Majick,                NULL,               SET1_T_MAGICSTRING )
PICK(  "MAXCLHistory",          "SF",   history_data, CLHist,                {10 CM 0 CM NULL},  SET1_T_MAXCLHISTORY )
PICK(  "MAXEMSK",               "SF",   int,          MaxEMSBlocks,          2048,               SET1_T_MAXEMSK )
PICK(  "MAXFILTERHistory",      "SF",   history_data, FilterHist,            {10 CM 0 CM NULL},  SET1_T_MAXFILTERHISTORY )
PICK(  "MAXFINDHistory",        "SF",   history_data, FindHist,              {10 CM 0 CM NULL},  SET1_T_MAXFINDHISTORY )
PICK(  "MAXLASTFILESHistory",   "SF",   history_data, LastFilesHist,         {4 CM 0 CM NULL},   SET1_T_MAXLASTFILESHISTORY )
PICK(  "MAXLINElen",            "SF",   int,          MaxLine,               512,                SET1_T_MAXLINELEN )
PICK(  "MAXPUSH",               "SF",   int,          MaxPush,               15,                 SET1_T_MAXPUSH )
PICK(  "MAXSWAPK",              "SF",   int,          MaxSwapBlocks,         2048,               SET1_T_MAXSWAPK )
PICK(  "MAXTILECOLORS",         "SF",   int,          MaxTileColors,         10,                 SET1_T_MAXTILECOLORS )
PICK(  "MAXWINDOWTILEX",        "SF",   int,          MaxWindowTileX,        3,                  SET1_T_MAXWINDOWTILEX )
PICK(  "MAXWINDOWTILEY",        "SF",   int,          MaxWindowTileY,        1,                  SET1_T_MAXWINDOWTILEY )
PICK(  "MAXXMSK",               "SF",   int,          MaxXMSBlocks,          2048,               SET1_T_MAXXMSK )
PICK(  "MOUSEDCLICKspeed",      "SF",   int,          MouseDoubleClickSpeed, 3,                  SET1_T_MOUSEDCLICKSPEED )
PICK(  "MOUSEREPEATdelay",      "SF",   int,          MouseRepeatDelay,      0,                  SET1_T_MOUSEREPEATDELAY )
PICK(  "MOUSESPeed",            "SF",   int,          MouseSpeed,            4,                  SET1_T_MOUSESPEED )
PICK(  "MOVEColor",             "SF",   vi_color,     MoveColor,             CYAN,               SET1_T_MOVECOLOR )
PICK(  "OVERSTRIKECURSORType",  "SF",   cursor_type,  OverstrikeCursorType,  {100 CM 100},       SET1_T_OVERSTRIKECURSORTYPE )
PICK(  "PAGELinesexposed",      "SF",   int,          PageLinesExposed,      1,                  SET1_T_PAGELINESEXPOSED )
PICK(  "RADix",                 "SF",   int,          Radix,                 10,                 SET1_T_RADIX )
PICK(  "RESIZEColor",           "SF",   vi_color,     ResizeColor,           YELLOW,             SET1_T_RESIZECOLOR )
PICK(  "SHELLPrompt",           "SF",   char *,       SpawnPrompt,           NULL,               SET1_T_SHELLPROMPT )
PICK(  "SHIFTWIDth",            "SF",   int,          ShiftWidth,            4,                  SET1_T_SHIFTWIDTH )
PICK(  "SPINX",                 "SF",   int,          SpinX,                 68,                 SET1_T_SPINX )
PICK(  "SPINY",                 "SF",   int,          SpinY,                 0,                  SET1_T_SPINY )
PICK(  "STACKk",                "SF",   int,          StackK,                MIN_STACK_K,        SET1_T_STACKK )
PICK(  "STATUSSECtions",        "SF",   short *,      StatusSections,        NULL,               SET1_T_STATUSSECTIONS )
PICK(  "STATUSSTRing",          "SF",   char *,       StatusString,          NULL,               SET1_T_STATUSSTRING )
PICK(  "TABamount",             "SF",   int,          TabAmount,             8,                  SET1_T_TABAMOUNT )
PICK(  "TAGFILEname",           "SF",   char *,       TagFileName,           NULL,               SET1_T_TAGFILENAME )
PICK(  "TILECOLOR",             "SF",   type_style *, TileColors,            NULL,               SET1_T_TILECOLOR )
PICK(  "TMPDIR",                "SF",   char *,       TmpDir,                NULL,               SET1_T_TMPDIR )
PICK(  "TOOLBARColor",          "SF",   vi_color,     ToolBarColor,          BLUE,               SET1_T_TOOLBARCOLOR )
PICK(  "WORD",                  "SF",   char *,       WordDefn,              NULL,               SET1_T_WORD )
PICK(  "WORDALT",               "SF",   char *,       WordAltDefn,           NULL,               SET1_T_WORDALT )
PICK(  "WRAPMARGIN",            "SF",   int,          WrapMargin,            0,                  SET1_T_WRAPMARGIN )
#if 0
PICKL( "HARDTab",               "SF",   int,          HardTab,               8,                  SET1_T_HARDTAB )
PICKL( "SHIFTWIDth",            "SF",   int,          ShiftWidth,            4,                  SET1_T_SHIFTWIDTH )
PICKL( "TABamount",             "SF",   int,          TabAmount,             8,                  SET1_T_TABAMOUNT )
#endif
