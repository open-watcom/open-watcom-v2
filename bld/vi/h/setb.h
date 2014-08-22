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
PICK( "AUTOIndent",           "AI", AutoIndent,          true,   SETFLAG_T_AUTOINDENT )
PICK( "AUTOMESSAGECLear",     "AC", AutoMessageClear,    false,  SETFLAG_T_AUTOMESSAGECLEAR )
PICK( "BEEPflag",             "BF", BeepFlag,            false,  SETFLAG_T_BEEPFLAG )
PICK( "CASEIGnore",           "CI", CaseIgnore,          true,   SETFLAG_T_CASEIGNORE )
PICK( "CASESHift",            "CS", CaseShift,           true,   SETFLAG_T_CASESHIFT )
PICK( "CHangelikevi",         "CV", ChangeLikeVI,        false,  SETFLAG_T_CHANGELIKEVI )
PICK( "CLock",                "CL", Clock,               false,  SETFLAG_T_CLOCK )
PICK( "CMode",                "CM", CMode,               false,  SETFLAG_T_CMODE )
PICK( "COLORBar",             "CB", Colorbar,            false,  SETFLAG_T_COLORBAR )
PICK( "COLUMNINFILESTatus",   "CF", ColumnInFileStatus,  false,  SETFLAG_T_COLUMNINFILESTATUS )
PICK( "CRLFAutodetect",       "CA", CRLFAutoDetect,      true,   SETFLAG_T_CRLFAUTODETECT )
PICK( "CURRENTSTATUS",        "CT", CurrentStatus,       true,   SETFLAG_T_CURRENTSTATUS )
PICK( "DRAWTildes",           "DT", DrawTildes,          true,   SETFLAG_T_DRAWTILDES )
PICK( "DISPLAYSeconds",       "DS", DisplaySeconds,      false,  SETFLAG_T_DISPLAYSECONDS )
PICK( "EIGHTbits",            "EB", EightBits,           true,   SETFLAG_T_EIGHTBITS )
PICK( "ESCAPEMESSage",        "EM", EscapeMessage,       true,   SETFLAG_T_ESCAPEMESSAGE )
PICK( "EXTENDEDMEMory",       "XM", ExtendedMemory,      true,   SETFLAG_T_EXTENDEDMEMORY )
PICK( "FONTBar",              "FB", Fontbar,             false,  SETFLAG_T_FONTBAR )
PICK( "IGNORECTRLZ",          "IZ", IgnoreCtrlZ,         false,  SETFLAG_T_IGNORECTRLZ )
PICK( "IGNORETagcase",        "IT", IgnoreTagCase,       false,  SETFLAG_T_IGNORETAGCASE )
PICK( "JUMPYScroll",          "JS", JumpyScroll,         false,  SETFLAG_T_JUMPYSCROLL )
#ifdef __WIN__
PICK( "LASTEOL",              "LE", LastEOL,             false,  SETFLAG_T_LASTEOL )
#else
PICK( "LASTEOL",              "LE", LastEOL,             true,   SETFLAG_T_LASTEOL )
#endif
PICK( "LEFTHANDMouse",        "LM", LeftHandMouse,       false,  SETFLAG_T_LEFTHANDMOUSE )
PICK( "LINEBased",            "LB", LineBased,           true,   SETFLAG_T_LINEBASED )
PICK( "LINENUMbers",          "LN", LineNumbers,         false,  SETFLAG_T_LINENUMBERS )
PICK( "LINENUMSONRight",      "LR", LineNumsOnRight,     false,  SETFLAG_T_LINENUMSONRIGHT )
PICK( "MAgic",                "MA", Magic,               false,  SETFLAG_T_MAGIC )
PICK( "MARKLonglines",        "ML", MarkLongLines,       true,   SETFLAG_T_MARKLONGLINES )
PICK( "MEnus",                "ME", Menus,               false,  SETFLAG_T_MENUS )
PICK( "MODELess",             "MO", Modeless,            false,  SETFLAG_T_MODELESS )
PICK( "PAUSEONSpawnerr",      "PE", PauseOnSpawnErr,     true,   SETFLAG_T_PAUSEONSPAWNERR )
PICK( "PPKeywordonly",        "PP", PPKeywordOnly,       false,  SETFLAG_T_PPKEYWORDONLY )
PICK( "QUiet",                "QU", Quiet,               false,  SETFLAG_T_QUIET )
PICK( "QUITMovesforward",     "QF", QuitMovesForward,    true,   SETFLAG_T_QUITMOVESFORWARD )
PICK( "QUITATLASTFILEEXIT",   "QE", QuitAtLastFileExit,  true,   SETFLAG_T_QUITATLASTFILEEXIT )
PICK( "READENTIREFIle",       "RF", ReadEntireFile,      true,   SETFLAG_T_READENTIREFILE )
PICK( "READONLYCHeck",        "RC", ReadOnlyCheck,       true,   SETFLAG_T_READONLYCHECK )
PICK( "REALTABS",             "RT", RealTabs,            true,   SETFLAG_T_REALTABS )
PICK( "REGSUBMagic",          "RM", RegSubMagic,         true,   SETFLAG_T_REGSUBMAGIC )
PICK( "REMOVESpaceTrailing",  "RS", RemoveSpaceTrailing, false,  SETFLAG_T_REMOVESPACETRAILING )
PICK( "REPEATInfo",           "RI", RepeatInfo,          false,  SETFLAG_T_REPEATINFO )
PICK( "SAMEFILECHeck",        "SC", SameFileCheck,       false,  SETFLAG_T_SAMEFILECHECK )
PICK( "SAVECONfig",           "SN", SaveConfig,          false,  SETFLAG_T_SAVECONFIG )
PICK( "SAVEOnBuild",          "SB", SaveOnBuild,         true,   SETFLAG_T_SAVEONBUILD )
PICK( "SAVEPOSition",         "SO", SavePosition,        true,   SETFLAG_T_SAVEPOSITION )
PICK( "SEARCHFORTAGFILE",     "ST", SearchForTagfile,    false,  SETFLAG_T_SEARCHFORTAGFILE )
PICK( "SEARCHWrap",           "SW", SearchWrap,          true,   SETFLAG_T_SEARCHWRAP )
PICK( "SHOWMatch",            "SM", ShowMatch,           false,  SETFLAG_T_SHOWMATCH )
PICK( "SPinning",             "SP", Spinning,            false,  SETFLAG_T_SPINNING )
PICK( "SSBar",                "SS", SSbar,               false,  SETFLAG_T_SSBAR )
PICK( "STATUSInfo",           "SI", StatusInfo,          false,  SETFLAG_T_STATUSINFO )
PICK( "TAGPROMPT",            "TP", TagPrompt,           true,   SETFLAG_T_TAGPROMPT )
PICK( "TOOLBar",              "TB", Toolbar,             false,  SETFLAG_T_TOOLBAR )
PICK( "UNdo",                 "UN", Undo,                true,   SETFLAG_T_UNDO )
PICK( "USEMouse",             "UM", UseMouse,            true,   SETFLAG_T_USEMOUSE )
PICK( "USENOName",            "UA", UseNoName,           true,   SETFLAG_T_USENONAME )
PICK( "VErbose",              "VE", Verbose,             true,   SETFLAG_T_VERBOSE )
PICK( "WINDOWGAdgets",        "WG", WindowGadgets,       false,  SETFLAG_T_WINDOWGADGETS )
PICK( "WORDWRAP",             "WW", WordWrap,            true,   SETFLAG_T_WORDWRAP )
PICK( "WRAPBACKSpace",        "WS", WrapBackSpace,       true,   SETFLAG_T_WRAPBACKSPACE )
PICK( "WRITECRlf",            "WL", WriteCRLF,           true,   SETFLAG_T_WRITECRLF )
PICK( "ZAPcolorsatexit",      "ZC", ZapColors,           false,  SETFLAG_T_ZAPCOLORS )
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

