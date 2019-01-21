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


/*    name                          varname              initval enum  */
PICK( "AUTOIndent",           "AI", AutoIndent,          true,   AUTOINDENT )
PICK( "AUTOMESSAGECLear",     "AC", AutoMessageClear,    false,  AUTOMESSAGECLEAR )
PICK( "BEEPflag",             "BF", BeepFlag,            false,  BEEPFLAG )
PICK( "CASEIGnore",           "CI", CaseIgnore,          true,   CASEIGNORE )
PICK( "CASESHift",            "CS", CaseShift,           true,   CASESHIFT )
PICK( "CHangelikevi",         "CV", ChangeLikeVI,        false,  CHANGELIKEVI )
PICK( "CLock",                "CL", Clock,               false,  CLOCK )
PICK( "CMode",                "CM", CMode,               false,  CMODE )
PICK( "COLORBar",             "CB", Colorbar,            false,  COLORBAR )
PICK( "COLUMNINFILESTatus",   "CF", ColumnInFileStatus,  false,  COLUMNINFILESTATUS )
PICK( "CRLFAutodetect",       "CA", CRLFAutoDetect,      true,   CRLFAUTODETECT )
PICK( "CURRENTSTATUS",        "CT", CurrentStatus,       true,   CURRENTSTATUS )
PICK( "DRAWTildes",           "DT", DrawTildes,          true,   DRAWTILDES )
PICK( "DISPLAYSeconds",       "DS", DisplaySeconds,      false,  DISPLAYSECONDS )
PICK( "EIGHTbits",            "EB", EightBits,           true,   EIGHTBITS )
PICK( "ESCAPEMESSage",        "EM", EscapeMessage,       true,   ESCAPEMESSAGE )
PICK( "EXTENDEDMEMory",       "XM", ExtendedMemory,      true,   EXTENDEDMEMORY )
PICK( "FONTBar",              "FB", Fontbar,             false,  FONTBAR )
PICK( "IGNORECTRLZ",          "IZ", IgnoreCtrlZ,         false,  IGNORECTRLZ )
PICK( "IGNORETagcase",        "IT", IgnoreTagCase,       false,  IGNORETAGCASE )
PICK( "JUMPYScroll",          "JS", JumpyScroll,         false,  JUMPYSCROLL )
#ifdef __WIN__
PICK( "LASTEOL",              "LE", LastEOL,             false,  LASTEOL )
#else
PICK( "LASTEOL",              "LE", LastEOL,             true,   LASTEOL )
#endif
PICK( "LEFTHANDMouse",        "LM", LeftHandMouse,       false,  LEFTHANDMOUSE )
PICK( "LINEBased",            "LB", LineBased,           true,   LINEBASED )
PICK( "LINENUMbers",          "LN", LineNumbers,         false,  LINENUMBERS )
PICK( "LINENUMSONRight",      "LR", LineNumsOnRight,     false,  LINENUMSONRIGHT )
PICK( "MAgic",                "MA", Magic,               false,  MAGIC )
PICK( "MARKLonglines",        "ML", MarkLongLines,       true,   MARKLONGLINES )
PICK( "MEnus",                "ME", Menus,               false,  MENUS )
PICK( "MODELess",             "MO", Modeless,            false,  MODELESS )
PICK( "PAUSEONSpawnerr",      "PE", PauseOnSpawnErr,     true,   PAUSEONSPAWNERR )
PICK( "PPKeywordonly",        "PP", PPKeywordOnly,       false,  PPKEYWORDONLY )
PICK( "QUiet",                "QU", Quiet,               false,  QUIET )
PICK( "QUITMovesforward",     "QF", QuitMovesForward,    true,   QUITMOVESFORWARD )
PICK( "QUITATLASTFILEEXIT",   "QE", QuitAtLastFileExit,  true,   QUITATLASTFILEEXIT )
PICK( "READENTIREFIle",       "RF", ReadEntireFile,      true,   READENTIREFILE )
PICK( "READONLYCHeck",        "RC", ReadOnlyCheck,       true,   READONLYCHECK )
PICK( "REALTABS",             "RT", RealTabs,            true,   REALTABS )
PICK( "REGSUBMagic",          "RM", RegSubMagic,         true,   REGSUBMAGIC )
PICK( "REMOVESpaceTrailing",  "RS", RemoveSpaceTrailing, false,  REMOVESPACETRAILING )
PICK( "REPEATInfo",           "RI", RepeatInfo,          false,  REPEATINFO )
PICK( "SAMEFILECHeck",        "SC", SameFileCheck,       false,  SAMEFILECHECK )
PICK( "SAVECONfig",           "SN", SaveConfig,          false,  SAVECONFIG )
PICK( "SAVEOnBuild",          "SB", SaveOnBuild,         true,   SAVEONBUILD )
PICK( "SAVEPOSition",         "SO", SavePosition,        true,   SAVEPOSITION )
PICK( "SEARCHFORTAGFILE",     "ST", SearchForTagfile,    false,  SEARCHFORTAGFILE )
PICK( "SEARCHWrap",           "SW", SearchWrap,          true,   SEARCHWRAP )
PICK( "SHOWMatch",            "SM", ShowMatch,           false,  SHOWMATCH )
PICK( "SPinning",             "SP", Spinning,            false,  SPINNING )
PICK( "SSBar",                "SS", SSbar,               false,  SSBAR )
PICK( "STATUSInfo",           "SI", StatusInfo,          false,  STATUSINFO )
PICK( "TAGPROMPT",            "TP", TagPrompt,           true,   TAGPROMPT )
PICK( "TOOLBar",              "TB", Toolbar,             false,  TOOLBAR )
PICK( "UNdo",                 "UN", Undo,                true,   UNDO )
PICK( "USEMouse",             "UM", UseMouse,            true,   USEMOUSE )
PICK( "USENOName",            "UA", UseNoName,           true,   USENONAME )
PICK( "VErbose",              "VE", Verbose,             true,   VERBOSE )
PICK( "WINDOWGAdgets",        "WG", WindowGadgets,       false,  WINDOWGADGETS )
PICK( "WORDWRAP",             "WW", WordWrap,            true,   WORDWRAP )
PICK( "WRAPBACKSpace",        "WS", WrapBackSpace,       true,   WRAPBACKSPACE )
PICK( "WRITECRlf",            "WL", WriteCRLF,           true,   WRITECRLF )
PICK( "ZAPcolorsatexit",      "ZC", ZapColors,           false,  ZAPCOLORS )
#if 0
PICKL( "AUTOIndent",            "AI",   AutoIndent,                 true,  AUTOINDENT )
PICKL( "CMode",                 "CM",   CMode,                      false, CMODE )
PICKL( "CRLFAutodetect",        "CA",   CRLFAutoDetect,             true,  CRLFAUTODETECT )
PICKL( "EIGHTbits",             "EB",   EightBits,                  true,  EIGHTBITS )
PICKL( "IGNORECTRLZ",           "IZ",   IgnoreCtrlZ,                false, IGNORECTRLZ )
PICKL( "IGNORETagcase",         "IT",   IgnoreTagCase,              false, IGNORETAGCASE )
PICKL( "PPKeywordonly",         "PP",   PPKeywordOnly,              false, PPKEYWORDONLY )
PICKL( "READENTIREFIle",        "RF",   ReadEntireFile,             true,  READENTIREFILE )
PICKL( "READONLYCHeck",         "RC",   ReadOnlyCheck,              true,  READONLYCHECK )
PICKL( "REALTABS",              "RT",   RealTabs,                   true,  REALTABS )
PICKL( "SHOWMatch",             "SM",   ShowMatch,                  false, SHOWMATCH )
PICKL( "TAGPROMPT",             "TP",   TagPrompt,                  true,  TAGPROMPT )
PICKL( "WRITECRlf",             "WL",   WriteCRLF,                  true,  WRITECRLF )
#endif

