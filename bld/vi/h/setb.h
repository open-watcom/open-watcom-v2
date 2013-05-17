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
PICK( "AUTOIndent",           "AI", AutoIndent,         TRUE,   SET2_T_AUTOINDENT )
PICK( "AUTOMESSAGECLear",     "AC", AutoMessageClear,   FALSE,  SET2_T_AUTOMESSAGECLEAR )
PICK( "BEEPflag",             "BF", BeepFlag,           FALSE,  SET2_T_BEEPFLAG )
PICK( "CASEIGnore",           "CI", CaseIgnore,         TRUE,   SET2_T_CASEIGNORE )
PICK( "CASESHift",            "CS", CaseShift,          TRUE,   SET2_T_CASESHIFT )
PICK( "CHangelikevi",         "CV", ChangeLikeVI,       FALSE,  SET2_T_CHANGELIKEVI )
PICK( "CLock",                "CL", Clock,              FALSE,  SET2_T_CLOCK )
PICK( "CMode",                "CM", CMode,              FALSE,  SET2_T_CMODE )
PICK( "COLORBar",             "CB", Colorbar,           FALSE,  SET2_T_COLORBAR )
PICK( "COLUMNINFILESTatus",   "CF", ColumnInFileStatus, FALSE,  SET2_T_COLUMNINFILESTATUS )
PICK( "CRLFAutodetect",       "CA", CRLFAutoDetect,     TRUE,   SET2_T_CRLFAUTODETECT )
PICK( "CURRENTSTATUS",        "CT", CurrentStatus,      TRUE,   SET2_T_CURRENTSTATUS )
PICK( "DRAWTildes",           "DT", DrawTildes,         TRUE,   SET2_T_DRAWTILDES )
PICK( "DISPLAYSeconds",       "DS", DisplaySeconds,     FALSE,  SET2_T_DISPLAYSECONDS )
PICK( "EIGHTbits",            "EB", EightBits,          TRUE,   SET2_T_EIGHTBITS )
PICK( "ESCAPEMESSage",        "EM", EscapeMessage,      TRUE,   SET2_T_ESCAPEMESSAGE )
PICK( "EXTENDEDMEMory",       "XM", ExtendedMemory,     TRUE,   SET2_T_EXTENDEDMEMORY )
PICK( "FONTBar",              "FB", Fontbar,            FALSE,  SET2_T_FONTBAR )
PICK( "IGNORECTRLZ",          "IZ", IgnoreCtrlZ,        FALSE,  SET2_T_IGNORECTRLZ )
PICK( "IGNORETagcase",        "IT", IgnoreTagCase,      FALSE,  SET2_T_IGNORETAGCASE )
PICK( "JUMPYScroll",          "JS", JumpyScroll,        FALSE,  SET2_T_JUMPYSCROLL )
#ifdef __WIN__
PICK( "LASTEOL",              "LE", LastEOL,            FALSE,  SET2_T_LASTEOL )
#else
PICK( "LASTEOL",              "LE", LastEOL,            TRUE,   SET2_T_LASTEOL )
#endif
PICK( "LEFTHANDMouse",        "LM", LeftHandMouse,      FALSE,  SET2_T_LEFTHANDMOUSE )
PICK( "LINEBased",            "LB", LineBased,          TRUE,   SET2_T_LINEBASED )
PICK( "LINENUMbers",          "LN", LineNumbers,        FALSE,  SET2_T_LINENUMBERS )
PICK( "LINENUMSONRight",      "LR", LineNumsOnRight,    FALSE,  SET2_T_LINENUMSONRIGHT )
PICK( "MAgic",                "MA", Magic,              FALSE,  SET2_T_MAGIC )
PICK( "MARKLonglines",        "ML", MarkLongLines,      TRUE,   SET2_T_MARKLONGLINES )
PICK( "MEnus",                "ME", Menus,              FALSE,  SET2_T_MENUS )
PICK( "MODELess",             "MO", Modeless,           FALSE,  SET2_T_MODELESS )
PICK( "PAUSEONSpawnerr",      "PE", PauseOnSpawnErr,    TRUE,   SET2_T_PAUSEONSPAWNERR )
PICK( "PPKeywordonly",        "PP", PPKeywordOnly,      FALSE,  SET2_T_PPKEYWORDONLY )
PICK( "QUiet",                "QU", Quiet,              FALSE,  SET2_T_QUIET )
PICK( "QUITMovesforward",     "QF", QuitMovesForward,   TRUE,   SET2_T_QUITMOVESFORWARD )
PICK( "QUITATLASTFILEEXIT",   "QE", QuitAtLastFileExit, TRUE,   SET2_T_QUITATLASTFILEEXIT )
PICK( "READENTIREFIle",       "RF", ReadEntireFile,     TRUE,   SET2_T_READENTIREFILE )
PICK( "READONLYCHeck",        "RC", ReadOnlyCheck,      TRUE,   SET2_T_READONLYCHECK )
PICK( "REALTABS",             "RT", RealTabs,           TRUE,   SET2_T_REALTABS )
PICK( "REGSUBMagic",          "RM", RegSubMagic,        TRUE,   SET2_T_REGSUBMAGIC )
PICK( "REPEATInfo",           "RI", RepeatInfo,         FALSE,  SET2_T_REPEATINFO )
PICK( "SAMEFILECHeck",        "SC", SameFileCheck,      FALSE,  SET2_T_SAMEFILECHECK )
PICK( "SAVECONfig",           "SN", SaveConfig,         FALSE,  SET2_T_SAVECONFIG )
PICK( "SAVEOnBuild",          "SB", SaveOnBuild,        TRUE,   SET2_T_SAVEONBUILD )
PICK( "SAVEPOSition",         "SO", SavePosition,       TRUE,   SET2_T_SAVEPOSITION )
PICK( "SEARCHFORTAGFILE",     "ST", SearchForTagfile,   FALSE,  SET2_T_SEARCHFORTAGFILE )
PICK( "SEARCHWrap",           "SW", SearchWrap,         TRUE,   SET2_T_SEARCHWRAP )
PICK( "SHOWMatch",            "SM", ShowMatch,          FALSE,  SET2_T_SHOWMATCH )
PICK( "SPinning",             "SP", Spinning,           FALSE,  SET2_T_SPINNING )
PICK( "SSBar",                "SS", SSbar,              FALSE,  SET2_T_SSBAR )
PICK( "STATUSInfo",           "SI", StatusInfo,         FALSE,  SET2_T_STATUSINFO )
PICK( "TAGPROMPT",            "TP", TagPrompt,          TRUE,   SET2_T_TAGPROMPT )
PICK( "TOOLBar",              "TB", Toolbar,            FALSE,  SET2_T_TOOLBAR )
PICK( "UNdo",                 "UN", Undo,               TRUE,   SET2_T_UNDO )
PICK( "USEMouse",             "UM", UseMouse,           TRUE,   SET2_T_USEMOUSE )
PICK( "USENOName",            "UA", UseNoName,          TRUE,   SET2_T_USENONAME )
PICK( "VErbose",              "VE", Verbose,            TRUE,   SET2_T_VERBOSE )
PICK( "WINDOWGAdgets",        "WG", WindowGadgets,      FALSE,  SET2_T_WINDOWGADGETS )
PICK( "WORDWRAP",             "WW", WordWrap,           TRUE,   SET2_T_WORDWRAP )
PICK( "WRAPBACKSpace",        "WS", WrapBackSpace,      TRUE,   SET2_T_WRAPBACKSPACE )
PICK( "WRITECRlf",            "WL", WriteCRLF,          TRUE,   SET2_T_WRITECRLF )
PICK( "ZAPcolorsatexit",      "ZC", ZapColors,          FALSE,  SET2_T_ZAPCOLORS )
#if 0
PICKL( "AUTOIndent",            "AI",   AutoIndent,                 true,  SET1_T_AUTOINDENT )
PICKL( "CMode",                 "CM",   CMode,                      false, SET1_T_CMODE )
PICKL( "CRLFAutodetect",        "CA",   CRLFAutoDetect,             true,  SET1_T_CRLFAUTODETECT )
PICKL( "EIGHTbits",             "EB",   EightBits,                  true,  SET1_T_EIGHTBITS )
PICKL( "IGNORECTRLZ",           "IZ",   IgnoreCtrlZ,                false, SET1_T_IGNORECTRLZ )
PICKL( "IGNORETagcase",         "IT",   IgnoreTagCase,              false, SET1_T_IGNORETAGCASE )
PICKL( "PPKeywordonly",         "PP",   PPKeywordOnly,              false, SET1_T_PPKEYWORDONLY )
PICKL( "READENTIREFIle",        "RF",   ReadEntireFile,             true,  SET1_T_READENTIREFILE )
PICKL( "READONLYCHeck",         "RC",   ReadOnlyCheck,              true,  SET1_T_READONLYCHECK )
PICKL( "REALTABS",              "RT",   RealTabs,                   true,  SET1_T_REALTABS )
PICKL( "SHOWMatch",             "SM",   ShowMatch,                  false, SET1_T_SHOWMATCH )
PICKL( "TAGPROMPT",             "TP",   TagPrompt,                  true,  SET1_T_TAGPROMPT )
PICKL( "WRITECRlf",             "WL",   WriteCRLF,                  true,  SET1_T_WRITECRLF )
#endif

