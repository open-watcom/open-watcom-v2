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


PICK( "Edit\0",                 PCL_T_EDIT )
PICK( "Write\0",                PCL_T_WRITE )
PICK( "Read\0",                 PCL_T_READ )
PICK( "Delete\0",               PCL_T_DELETE )
PICK( "Substitute\0",           PCL_T_SUBSTITUTE )
PICK( "Quit\0",                 PCL_T_QUIT )
PICK( "SETCOLOR\0",             PCL_T_SETCOLOR )
PICK( "Files\0",                PCL_T_FILES )
PICK( "Next\0",                 PCL_T_NEXT )
PICK( "Yank\0",                 PCL_T_YANK )
PICK( ">\0",                    PCL_T_SHOVE )
PICK( "<\0",                    PCL_T_SUCK )
PICK( "DAte\0",                 PCL_T_DATE )
PICK( "CD\0",                   PCL_T_CD )
PICK( "!\0",                    PCL_T_SYSTEM )
PICK( "COMPRess\0",             PCL_T_COMPRESS )
PICK( "RESize\0",               PCL_T_RESIZE )
PICK( "Xit\0",                  PCL_T_SAVEANDEXIT )
PICK( "Global\0",               PCL_T_GLOBAL )
PICK( "SOurce\0",               PCL_T_SOURCE )
PICK( "SHell\0",                PCL_T_SHELL )
PICK( "TILE\0",                 PCL_T_TILE )
PICK( "MOVEWin\0",              PCL_T_MOVEWIN )
PICK( "TAG\0",                  PCL_T_TAG )
PICK( "FGRep\0",                PCL_T_FGREP )
PICK( "SIZE\0",                 PCL_T_SIZE )
PICK( "MAP\0",                  PCL_T_MAP )
PICK( "EGRep\0",                PCL_T_EGREP )
PICK( "VIEw\0",                 PCL_T_VIEW )
PICK( "ALIas\0",                PCL_T_ALIAS )
PICK( "MATCH\0",                PCL_T_MATCH )
PICK( "COMMANDWindow\0",        PCL_T_COMMANDWINDOW )
PICK( "COUNTWindow\0",          PCL_T_COUNTWINDOW )
PICK( "MENUBARWindow\0",        PCL_T_MENUBARWINDOW )
PICK( "MENUWindow\0",           PCL_T_MENUWINDOW )
PICK( "DIRWindow\0",            PCL_T_DIRWINDOW )
PICK( "EDITWindow\0",           PCL_T_EDITWINDOW )
PICK( "EXTRAINFOWindow\0",      PCL_T_EXTRAINFOWINDOW )
PICK( "FILECWindow\0",          PCL_T_FILECWINDOW )
PICK( "FILEWindow\0",           PCL_T_FILEWINDOW )
PICK( "HILIGHT\0",              PCL_T_HILIGHT )
PICK( "LINENUMBERWindow\0",     PCL_T_LINENUMBERWINDOW )
PICK( "MESSAGEWindow\0",        PCL_T_MESSAGEWINDOW )
PICK( "SETVALWindow\0",         PCL_T_SETVALWINDOW )
PICK( "SETWindow\0",            PCL_T_SETWINDOW )
PICK( "STATUSWindow\0",         PCL_T_STATUSWINDOW )
PICK( "TEXT\0",                 PCL_T_TEXT )
PICK( "BORDER\0",               PCL_T_BORDER )
PICK( "DIMENSION\0",            PCL_T_DIMENSION )
PICK( "ENDWindow\0",            PCL_T_ENDWINDOW )
PICK( "SET\0",                  PCL_T_SET )
PICK( "COMPile\0",              PCL_T_COMPILE )
PICK( "ECHO\0",                 PCL_T_ECHO )
PICK( "Prev\0",                 PCL_T_PREV )
PICK( "LOAD\0",                 PCL_T_LOAD )
PICK( "UNMAP\0",                PCL_T_UNMAP )
PICK( "ABbrev\0",               PCL_T_ABBREV )
PICK( "UNABbrev\0",             PCL_T_UNABBREV )
PICK( "UNALias\0",              PCL_T_UNALIAS )
PICK( "PUt\0",                  PCL_T_PUT )
PICK( "Hide\0",                 PCL_T_HIDE )
PICK( "DEFAULTWindow\0",        PCL_T_DEFAULTWINDOW )
PICK( "EVAL\0",                 PCL_T_EVAL )
PICK( "KEYAdd\0",               PCL_T_KEYADD )
PICK( "CASCade\0",              PCL_T_CASCADE )
PICK( "MENU\0",                 PCL_T_MENU )
PICK( "MENUITEM\0",             PCL_T_MENUITEM )
PICK( "ENDMENU\0",              PCL_T_ENDMENU )
PICK( "QUITALL\0",              PCL_T_QUITALL )
PICK( "MINimize\0",             PCL_T_MINIMIZE )
PICK( "MAXimize\0",             PCL_T_MAXIMIZE )
PICK( "DELETEMENU\0",           PCL_T_DELETEMENU )
PICK( "ADDMENUItem\0",          PCL_T_ADDMENUITEM )
PICK( "DELETEMENUItem\0",       PCL_T_DELETEMENUITEM )
PICK( "Open\0",                 PCL_T_OPEN )
PICK( "WQ\0",                   PCL_T_WRITEQUIT )
PICK( "HELP\0",                 PCL_T_HELP )
PICK( "EXECute\0",              PCL_T_EXECUTE )
PICK( "MAPBASE\0",              PCL_T_MAPBASE )
PICK( "FLOATMenu\0",            PCL_T_FLOATMENU )
PICK( "PUSH\0",                 PCL_T_PUSH )
PICK( "POP\0",                  PCL_T_POP )
PICK( "EXPAND\0",               PCL_T_EXPAND )
PICK( "GENCONFIG\0",            PCL_T_GENCONFIG )
PICK( "SETFONT\0",              PCL_T_SETFONT )
PICK( "EXITALL\0",              PCL_T_EXITALL )
PICK( "EXITFILESONLY\0",        PCL_T_EXITFILESONLY )
PICK( "ADDTOOLBARITEM\0",       PCL_T_ADDTOOLBARITEM )
PICK( "DELETETOOLBARITEM\0",    PCL_T_DELETETOOLBARITEM )
PICK( "MENUFILELIST\0",         PCL_T_MENUFILELIST )
PICK( "ABOUT\0",                PCL_T_ABOUT )
PICK( "MENULASTFILES\0",        PCL_T_MENULASTFILES )
PICK( "ACTIVEMENUWINDOW\0",     PCL_T_ACTIVEMENUWINDOW )
PICK( "GREYEDMENUWINDOW\0",     PCL_T_GREYEDMENUWINDOW )
PICK( "ACTIVEGREYEDMENUWINDOW\0",PCL_T_ACTIVEGREYEDMENUWINDOW )
PICK( "WHITESPACE\0",           PCL_T_WHITESPACE )
PICK( "SELECTION\0",            PCL_T_SELECTION )
PICK( "EOFTEXT\0",              PCL_T_EOFTEXT )
PICK( "KEYWORD\0",              PCL_T_KEYWORD )
PICK( "OCTAL\0",                PCL_T_OCTAL )
PICK( "HEX\0",                  PCL_T_HEX )
PICK( "INTEGER\0",              PCL_T_INTEGER )
PICK( "CHAR\0",                 PCL_T_CHAR )
PICK( "PREPROCESSOR\0",         PCL_T_PREPROCESSOR )
PICK( "SYMBOL\0",               PCL_T_SYMBOL )
PICK( "INVALIDTEXT\0",          PCL_T_INVALIDTEXT )
PICK( "IDENTIFIER\0",           PCL_T_IDENTIFIER )
PICK( "JUMPLABEL\0",            PCL_T_JUMPLABEL )
PICK( "COMMENT\0",              PCL_T_COMMENT )
PICK( "FLOAT\0",                PCL_T_FLOAT )
PICK( "STRING\0",               PCL_T_STRING )
PICK( "VARIABLE\0",             PCL_T_VARIABLE )
PICK( "REGEXP\0",               PCL_T_REGEXP )
PICK( "FILETYPESOURCE\0",       PCL_T_FILETYPESOURCE )
PICK( "ENDFILETYPESOURCE\0",    PCL_T_ENDFILETYPESOURCE )
PICK( "LOCATE\0",               PCL_T_LOCATE )
PICK( "ACTIVATEIDE\0",          PCL_T_ACTIVATE_IDE )
PICK( "CHECKOUT\0",             PCL_T_CHECKOUT )
PICK( "CHECKIN\0",              PCL_T_CHECKIN )

#undef PICK
