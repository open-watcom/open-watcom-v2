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


/*     name                         varname         initval enum  */
PICK( "AUTOIndent",           "AI", AutoIndent,          TRUE,   SETFLAG_T_AUTOINDENT )
PICK( "AUTOMESSAGECLear",     "AC", AutoMessageClear,    FALSE,  SETFLAG_T_AUTOMESSAGECLEAR )
PICK( "BEEPflag",             "BF", BeepFlag,            FALSE,  SETFLAG_T_BEEPFLAG )
PICK( "CASEIGnore",           "CI", CaseIgnore,          TRUE,   SETFLAG_T_CASEIGNORE )
PICK( "CASESHift",            "CS", CaseShift,           TRUE,   SETFLAG_T_CASESHIFT )
PICK( "CHangelikevi",         "CV", ChangeLikeVI,        FALSE,  SETFLAG_T_CHANGELIKEVI )
PICK( "CLock",                "CL", Clock,               FALSE,  SETFLAG_T_CLOCK )
PICK( "CMode",                "CM", CMode,               FALSE,  SETFLAG_T_CMODE )
PICK( "COLORBar",             "CB", Colorbar,            FALSE,  SETFLAG_T_COLORBAR )
PICK( "COLUMNINFILESTatus",   "CF", ColumnInFileStatus,  FALSE,  SETFLAG_T_COLUMNINFILESTATUS )
PICK( "CRLFAutodetect",       "CA", CRLFAutoDetect,      TRUE,   SETFLAG_T_CRLFAUTODETECT )
PICK( "CURRENTSTATUS",        "CT", CurrentStatus,       TRUE,   SETFLAG_T_CURRENTSTATUS )
PICK( "DRAWTildes",           "DT", DrawTildes,          TRUE,   SETFLAG_T_DRAWTILDES )
PICK( "DISPLAYSeconds",       "DS", DisplaySeconds,      FALSE,  SETFLAG_T_DISPLAYSECONDS )
PICK( "EIGHTbits",            "EB", EightBits,           TRUE,   SETFLAG_T_EIGHTBITS )
PICK( "ESCAPEMESSage",        "EM", EscapeMessage,       TRUE,   SETFLAG_T_ESCAPEMESSAGE )
PICK( "EXTENDEDMEMory",       "XM", ExtendedMemory,      TRUE,   SETFLAG_T_EXTENDEDMEMORY )
PICK( "FONTBar",              "FB", Fontbar,             FALSE,  SETFLAG_T_FONTBAR )
PICK( "IGNORECTRLZ",          "IZ", IgnoreCtrlZ,         FALSE,  SETFLAG_T_IGNORECTRLZ )
PICK( "IGNORETagcase",        "IT", IgnoreTagCase,       FALSE,  SETFLAG_T_IGNORETAGCASE )
PICK( "JUMPYScroll",          "JS", JumpyScroll,         FALSE,  SETFLAG_T_JUMPYSCROLL )
#ifdef __WIN__
PICK( "LASTEOL",              "LE", LastEOL,             FALSE,  SETFLAG_T_LASTEOL )
#else
PICK( "LASTEOL",              "LE", LastEOL,             TRUE,   SETFLAG_T_LASTEOL )
#endif
PICK( "LEFTHANDMouse",        "LM", LeftHandMouse,       FALSE,  SETFLAG_T_LEFTHANDMOUSE )
PICK( "LINEBased",            "LB", LineBased,           TRUE,   SETFLAG_T_LINEBASED )
PICK( "LINENUMbers",          "LN", LineNumbers,         FALSE,  SETFLAG_T_LINENUMBERS )
PICK( "LINENUMSONRight",      "LR", LineNumsOnRight,     FALSE,  SETFLAG_T_LINENUMSONRIGHT )
PICK( "MAgic",                "MA", Magic,               FALSE,  SETFLAG_T_MAGIC )
PICK( "MARKLonglines",        "ML", MarkLongLines,       TRUE,   SETFLAG_T_MARKLONGLINES )
PICK( "MEnus",                "ME", Menus,               FALSE,  SETFLAG_T_MENUS )
PICK( "MODELess",             "MO", Modeless,            FALSE,  SETFLAG_T_MODELESS )
PICK( "PAUSEONSpawnerr",      "PE", PauseOnSpawnErr,     TRUE,   SETFLAG_T_PAUSEONSPAWNERR )
PICK( "PPKeywordonly",        "PP", PPKeywordOnly,       FALSE,  SETFLAG_T_PPKEYWORDONLY )
PICK( "QUiet",                "QU", Quiet,               FALSE,  SETFLAG_T_QUIET )
PICK( "QUITMovesforward",     "QF", QuitMovesForward,    TRUE,   SETFLAG_T_QUITMOVESFORWARD )
PICK( "QUITATLASTFILEEXIT",   "QE", QuitAtLastFileExit,  TRUE,   SETFLAG_T_QUITATLASTFILEEXIT )
PICK( "READENTIREFIle",       "RF", ReadEntireFile,      TRUE,   SETFLAG_T_READENTIREFILE )
PICK( "READONLYCHeck",        "RC", ReadOnlyCheck,       TRUE,   SETFLAG_T_READONLYCHECK )
PICK( "REALTABS",             "RT", RealTabs,            TRUE,   SETFLAG_T_REALTABS )
PICK( "REGSUBMagic",          "RM", RegSubMagic,         TRUE,   SETFLAG_T_REGSUBMAGIC )
PICK( "REMOVESpaceTrailing",  "RS", RemoveSpaceTrailing, TRUE,   SETFLAG_T_REMOVESPACETRAILING )
PICK( "REPEATInfo",           "RI", RepeatInfo,          FALSE,  SETFLAG_T_REPEATINFO )
PICK( "SAMEFILECHeck",        "SC", SameFileCheck,       FALSE,  SETFLAG_T_SAMEFILECHECK )
PICK( "SAVECONfig",           "SN", SaveConfig,          FALSE,  SETFLAG_T_SAVECONFIG )
PICK( "SAVEOnBuild",          "SB", SaveOnBuild,         TRUE,   SETFLAG_T_SAVEONBUILD )
PICK( "SAVEPOSition",         "SO", SavePosition,        TRUE,   SETFLAG_T_SAVEPOSITION )
PICK( "SEARCHFORTAGFILE",     "ST", SearchForTagfile,    FALSE,  SETFLAG_T_SEARCHFORTAGFILE )
PICK( "SEARCHWrap",           "SW", SearchWrap,          TRUE,   SETFLAG_T_SEARCHWRAP )
PICK( "SHOWMatch",            "SM", ShowMatch,           FALSE,  SETFLAG_T_SHOWMATCH )
PICK( "SPinning",             "SP", Spinning,            FALSE,  SETFLAG_T_SPINNING )
PICK( "SSBar",                "SS", SSbar,               FALSE,  SETFLAG_T_SSBAR )
PICK( "STATUSInfo",           "SI", StatusInfo,          FALSE,  SETFLAG_T_STATUSINFO )
PICK( "TAGPROMPT",            "TP", TagPrompt,           TRUE,   SETFLAG_T_TAGPROMPT )
PICK( "TOOLBar",              "TB", Toolbar,             FALSE,  SETFLAG_T_TOOLBAR )
PICK( "UNdo",                 "UN", Undo,                TRUE,   SETFLAG_T_UNDO )
PICK( "USEMouse",             "UM", UseMouse,            TRUE,   SETFLAG_T_USEMOUSE )
PICK( "USENOName",            "UA", UseNoName,           TRUE,   SETFLAG_T_USENONAME )
PICK( "VErbose",              "VE", Verbose,             TRUE,   SETFLAG_T_VERBOSE )
PICK( "WINDOWGAdgets",        "WG", WindowGadgets,       FALSE,  SETFLAG_T_WINDOWGADGETS )
PICK( "WORDWRAP",             "WW", WordWrap,            TRUE,   SETFLAG_T_WORDWRAP )
PICK( "WRAPBACKSpace",        "WS", WrapBackSpace,       TRUE,   SETFLAG_T_WRAPBACKSPACE )
PICK( "WRITECRlf",            "WL", WriteCRLF,           TRUE,   SETFLAG_T_WRITECRLF )
PICK( "ZAPcolorsatexit",      "ZC", ZapColors,           FALSE,  SETFLAG_T_ZAPCOLORS )
#if 0
PICKL( "AUTOIndent",            "AI",   AutoIndent,                 true,  SETFLAG_T_AUTOINDENT )
PICKL( "CMode",                 "CM",   CMode,                      false, SETFLAG_T_CMODE )
PICKL( "CRLFAutodetect",        "CA",   CRLFAutoDetect,             true,  SETFLAG_T_CRLFAUTODETECT )
PICKL( "EIGHTbits",             "EB",   EightBits,                  true,  SETFLAG_T_EIGHTBITS )
PICKL( "IGNORECTRLZ",           "IZ",   IgnoreCtrlZ,                false, SETFLAG_T_IGNORECTRLZ )
PICKL( "IGNORETagcase",         "IT",   IgnoreTagCase,              false, SETFLAG_T_IGNORETAGCASE )
PICKL( "PPKeywordonly",         "PP",   PPKeywordOnly,              false, SETFLAG_T_PPKEYWORDONLY )
PICKL( "READENTIREFIle",        "RF",   ReadEntireFile,             true,  SETFLAG_T_READENTIREFILE )
PICKL( "READONLYCHeck",         "RC",   ReadOnlyCheck,              true,  SETFLAG_T_READONLYCHECK )
PICKL( "REALTABS",              "RT",   RealTabs,                   true,  SETFLAG_T_REALTABS )
PICKL( "SHOWMatch",             "SM",   ShowMatch,                  false, SETFLAG_T_SHOWMATCH )
PICKL( "TAGPROMPT",             "TP",   TagPrompt,                  true,  SETFLAG_T_TAGPROMPT )
PICKL( "WRITECRlf",             "WL",   WriteCRLF,                  true,  SETFLAG_T_WRITECRLF )
#endif

