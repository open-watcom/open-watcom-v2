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


PICK( "AUTOIndent\0",           "AI\0", AutoIndent,     TRUE,   SET2_T_AUTOINDENT )
PICK( "AUTOMESSAGECLear\0",     "AC\0", AutoMessageClear,FALSE, SET2_T_AUTOMESSAGECLEAR )
PICK( "BEEPflag\0",             "BF\0", BeepFlag,       FALSE,  SET2_T_BEEPFLAG )
PICK( "CASEIGnore\0",           "CI\0", CaseIgnore,     TRUE,   SET2_T_CASEIGNORE )
PICK( "CASESHift\0",            "CS\0", CaseShift,      TRUE,   SET2_T_CASESHIFT )
PICK( "CHangelikevi\0",         "CV\0", ChangeLikeVI,   FALSE,  SET2_T_CHANGELIKEVI )
PICK( "CLock\0",                "CL\0", Clock,          FALSE,  SET2_T_CLOCK )
PICK( "CMode\0",                "CM\0", CMode,          FALSE,  SET2_T_CMODE )
PICK( "COLORBar\0",             "CB\0", Colorbar,       FALSE,  SET2_T_COLORBAR )
PICK( "COLUMNINFILESTatus\0",   "CF\0", ColumnInFileStatus,FALSE,SET2_T_COLUMNINFILESTATUS )
PICK( "CRLFAutodetect\0",       "CA\0", CRLFAutoDetect, TRUE,   SET2_T_CRLFAUTODETECT )
PICK( "CURRENTSTATUS\0",        "CT\0", CurrentStatus,  TRUE,   SET2_T_CURRENTSTATUS )
PICK( "DRAWTildes\0",           "DT\0", DrawTildes,     TRUE,   SET2_T_DRAWTILDES )
PICK( "DISPLAYSeconds\0",       "DS\0", DisplaySeconds, FALSE,  SET2_T_DISPLAYSECONDS )
PICK( "EIGHTbits\0",            "EB\0", EightBits,      TRUE,   SET2_T_EIGHTBITS )
PICK( "ESCAPEMESSage\0",        "EM\0", EscapeMessage,  TRUE,   SET2_T_ESCAPEMESSAGE )
PICK( "EXTENDEDMEMory\0",       "XM\0", ExtendedMemory, TRUE,   SET2_T_EXTENDEDMEMORY )
PICK( "FONTBar\0",              "FB\0", Fontbar,        FALSE,  SET2_T_FONTBAR )
PICK( "IGNORECTRLZ\0",          "IZ\0", IgnoreCtrlZ,    FALSE,  SET2_T_IGNORECTRLZ )
PICK( "IGNORETagcase\0",        "IT\0", IgnoreTagCase,  FALSE,  SET2_T_IGNORETAGCASE )
PICK( "JUMPYScroll\0",          "JS\0", JumpyScroll,    FALSE,  SET2_T_JUMPYSCROLL )
PICK( "LEFTHANDMouse\0",        "LM\0", LeftHandMouse,  FALSE,  SET2_T_LEFTHANDMOUSE )
PICK( "LINEBased\0",            "LB\0", LineBased,      TRUE,   SET2_T_LINEBASED )
PICK( "LINENUMbers\0",          "LN\0", LineNumbers,    FALSE,  SET2_T_LINENUMBERS )
PICK( "LINENUMSONRight\0",      "LR\0", LineNumsOnRight,FALSE,  SET2_T_LINENUMSONRIGHT )
PICK( "MAgic\0",                "MA\0", Magic,          FALSE,  SET2_T_MAGIC )
PICK( "MARKLonglines\0",        "ML\0", MarkLongLines,  TRUE,   SET2_T_MARKLONGLINES )
PICK( "MEnus\0",                "ME\0", Menus,          FALSE,  SET2_T_MENUS )
PICK( "MODELess\0",             "MO\0", Modeless,       FALSE,  SET2_T_MODELESS )
PICK( "PAUSEONSpawnerr\0",      "PE\0", PauseOnSpawnErr,TRUE,   SET2_T_PAUSEONSPAWNERR )
PICK( "PPKeywordonly\0",        "PP\0", PPKeywordOnly,  FALSE,  SET2_T_PPKEYWORDONLY )
PICK( "QUiet\0",                "QU\0", Quiet,          FALSE,  SET2_T_QUIET )
PICK( "QUITMovesforward\0",     "QF\0", QuitMovesForward,TRUE,  SET2_T_QUITMOVESFORWARD )
PICK( "QUITATLASTFILEEXIT\0",   "QE\0", QuitAtLastFileExit,TRUE,SET2_T_QUITATLASTFILEEXIT )
PICK( "READENTIREFIle\0",       "RF\0", ReadEntireFile, TRUE,   SET2_T_READENTIREFILE )
PICK( "READONLYCHeck\0",        "RC\0", ReadOnlyCheck,  TRUE,   SET2_T_READONLYCHECK )
PICK( "REALTABS\0",             "RT\0", RealTabs,       TRUE,   SET2_T_REALTABS )
PICK( "REGSUBMagic\0",          "RM\0", RegSubMagic,    TRUE,   SET2_T_REGSUBMAGIC )
PICK( "REPEATInfo\0",           "RI\0", RepeatInfo,     FALSE,  SET2_T_REPEATINFO )
PICK( "SAMEFILECHeck\0",        "SC\0", SameFileCheck,  FALSE,  SET2_T_SAMEFILECHECK )
PICK( "SAVECONfig\0",           "SN\0", SaveConfig,     FALSE,  SET2_T_SAVECONFIG )
PICK( "SAVEOnBuild\0",          "SB\0", SaveOnBuild,    TRUE,   SET2_T_SAVEONBUILD )
PICK( "SAVEPOSition\0",         "SO\0", SavePosition,   TRUE,   SET2_T_SAVEPOSITION )
PICK( "SEARCHFORTAGFILE\0",     "ST\0", SearchForTagfile,FALSE, SET2_T_SEARCHFORTAGFILE )
PICK( "SEARCHWrap\0",           "SW\0", SearchWrap,     TRUE,   SET2_T_SEARCHWRAP )
PICK( "SHOWMatch\0",            "SM\0", ShowMatch,      FALSE,  SET2_T_SHOWMATCH )
PICK( "SPinning\0",             "SP\0", Spinning,       FALSE,  SET2_T_SPINNING )
PICK( "SSBar\0",                "SS\0", SSbar,          FALSE,  SET2_T_SSBAR )
PICK( "STATUSInfo\0",           "SI\0", StatusInfo,     FALSE,  SET2_T_STATUSINFO )
PICK( "TAGPROMPT\0",            "TP\0", TagPrompt,      TRUE,   SET2_T_TAGPROMPT )
PICK( "TOOLBar\0",              "TB\0", Toolbar,        FALSE,  SET2_T_TOOLBAR )
PICK( "UNdo\0",                 "UN\0", Undo,           TRUE,   SET2_T_UNDO )
PICK( "USEMouse\0",             "UM\0", UseMouse,       TRUE,   SET2_T_USEMOUSE )
PICK( "USENOName\0",            "UA\0", UseNoName,      TRUE,   SET2_T_USENONAME )
PICK( "VErbose\0",              "VE\0", Verbose,        TRUE,   SET2_T_VERBOSE )
PICK( "WINDOWGAdgets\0",        "WG\0", WindowGadgets,  FALSE,  SET2_T_WINDOWGADGETS )
PICK( "WORDWRAP\0",             "WW\0", WordWrap,       TRUE,   SET2_T_WORDWRAP )
PICK( "WRAPBACKSpace\0",        "WS\0", WrapBackSpace,  TRUE,   SET2_T_WRAPBACKSPACE )
PICK( "WRITECRlf\0",            "WL\0", WriteCRLF,      TRUE,   SET2_T_WRITECRLF )
PICK( "ZAPcolorsatexit\0",      "ZC\0", ZapColors,      FALSE,  SET2_T_ZAPCOLORS )

#undef PICK
