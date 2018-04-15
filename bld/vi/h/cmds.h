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


PICK( "!",                          EDIT_DMT )
PICK( "Edit\0",                     EDIT )
PICK( "!",                          WRITE_DMT )
PICK( "Write\0",                    WRITE )
PICK( "Read\0",                     READ )
PICK( "Delete\0",                   DELETE )
PICK( "Substitute\0",               SUBSTITUTE )
PICK( "!",                          QUIT_DMT )
PICK( "Quit\0",                     QUIT )
PICK( "SETCOLOR\0",                 SETCOLOR )
PICK( "Files\0",                    FILES )
PICK( "Next\0",                     NEXT )
PICK( "Yank\0",                     YANK )
PICK( ">\0",                        SHOVE )
PICK( "<\0",                        SUCK )
PICK( "DAte\0",                     DATE )
PICK( "CD\0",                       CD )
PICK( "!\0",                        SYSTEM )
PICK( "COMPRess\0",                 COMPRESS )
PICK( "RESize\0",                   RESIZE )
PICK( "Xit\0",                      SAVEANDEXIT )
PICK( "!",                          GLOBAL_DMT )
PICK( "Global\0",                   GLOBAL )
PICK( "SOurce\0",                   SOURCE )
PICK( "SHell\0",                    SHELL )
PICK( "TILE\0",                     TILE )
PICK( "MOVEWin\0",                  MOVEWIN )
PICK( "TAG\0",                      TAG )
PICK( "FGRep\0",                    FGREP )
PICK( "SIZE\0",                     SIZE )
PICK( "!",                          MAP_DMT )
PICK( "MAP\0",                      MAP )
PICK( "EGRep\0",                    EGREP )
PICK( "!",                          VIEW_DMT )
PICK( "VIEw\0",                     VIEW )
PICK( "ALIas\0",                    ALIAS )
PICK( "MATCH\0",                    MATCH )
PICK( "COMMANDWindow\0",            COMMANDWINDOW )
PICK( "COUNTWindow\0",              COUNTWINDOW )
PICK( "MENUBARWindow\0",            MENUBARWINDOW )
PICK( "MENUWindow\0",               MENUWINDOW )
PICK( "DIRWindow\0",                DIRWINDOW )
PICK( "EDITWindow\0",               EDITWINDOW )
PICK( "EXTRAINFOWindow\0",          EXTRAINFOWINDOW )
PICK( "FILECWindow\0",              FILECWINDOW )
PICK( "FILEWindow\0",               FILEWINDOW )
PICK( "HILIGHT\0",                  HILIGHT )
PICK( "LINENUMBERWindow\0",         LINENUMBERWINDOW )
PICK( "MESSAGEWindow\0",            MESSAGEWINDOW )
PICK( "SETVALWindow\0",             SETVALWINDOW )
PICK( "SETWindow\0",                SETWINDOW )
PICK( "STATUSWindow\0",             STATUSWINDOW )
PICK( "TEXT\0",                     TEXT )
PICK( "BORDER\0",                   BORDER )
PICK( "DIMENSION\0",                DIMENSION )
PICK( "ENDWindow\0",                ENDWINDOW )
PICK( "SET\0",                      SET )
PICK( "COMPile\0",                  COMPILE )
PICK( "ECHO\0",                     ECHO )
PICK( "Prev\0",                     PREV )
PICK( "LOAD\0",                     LOAD )
PICK( "!",                          UNMAP_DMT )
PICK( "UNMAP\0",                    UNMAP )
PICK( "ABbrev\0",                   ABBREV )
PICK( "UNABbrev\0",                 UNABBREV )
PICK( "UNALias\0",                  UNALIAS )
PICK( "!",                          PUT_DMT )
PICK( "PUt\0",                      PUT )
PICK( "!",                          HIDE_DMT )
PICK( "Hide\0",                     HIDE )
PICK( "DEFAULTWindow\0",            DEFAULTWINDOW )
PICK( "EVAL\0",                     EVAL )
PICK( "KEYAdd\0",                   KEYADD )
PICK( "CASCade\0",                  CASCADE )
PICK( "MENU\0",                     MENU )
PICK( "MENUITEM\0",                 MENUITEM )
PICK( "ENDMENU\0",                  ENDMENU )
PICK( "QUITALL\0",                  QUITALL )
PICK( "MINimize\0",                 MINIMIZE )
PICK( "MAXimize\0",                 MAXIMIZE )
PICK( "DELETEMENU\0",               DELETEMENU )
PICK( "ADDMENUItem\0",              ADDMENUITEM )
PICK( "DELETEMENUItem\0",           DELETEMENUITEM )
PICK( "Open\0",                     OPEN )
PICK( "WQ\0",                       WRITEQUIT )
PICK( "HELP\0",                     HELP )
PICK( "EXECute\0",                  EXECUTE )
PICK( "!",                          MAPBASE_DMT )
PICK( "MAPBASE\0",                  MAPBASE )
PICK( "FLOATMenu\0",                FLOATMENU )
PICK( "PUSH\0",                     PUSH )
PICK( "POP\0",                      POP )
PICK( "EXPAND\0",                   EXPAND )
PICK( "GENCONFIG\0",                GENCONFIG )
PICK( "SETFONT\0",                  SETFONT )
PICK( "EXITALL\0",                  EXITALL )
PICK( "EXITFILESONLY\0",            EXITFILESONLY )
PICK( "ADDTOOLBARITEM\0",           ADDTOOLBARITEM )
PICK( "ADDTOOLBARITEM2\0",          ADDTOOLBARITEM2 )
PICK( "DELETETOOLBARITEM\0",        DELETETOOLBARITEM )
PICK( "MENUFILELIST\0",             MENUFILELIST )
PICK( "ABOUT\0",                    ABOUT )
PICK( "MENULASTFILES\0",            MENULASTFILES )
PICK( "ACTIVEMENUWINDOW\0",         ACTIVEMENUWINDOW )
PICK( "GREYEDMENUWINDOW\0",         GREYEDMENUWINDOW )
PICK( "ACTIVEGREYEDMENUWINDOW\0",   ACTIVEGREYEDMENUWINDOW )
PICK( "WHITESPACE\0",               WHITESPACE )
PICK( "SELECTION\0",                SELECTION )
PICK( "EOFTEXT\0",                  EOFTEXT )
PICK( "KEYWORD\0",                  KEYWORD )
PICK( "OCTAL\0",                    OCTAL )
PICK( "HEX\0",                      HEX )
PICK( "INTEGER\0",                  INTEGER )
PICK( "CHAR\0",                     CHAR )
PICK( "PREPROCESSOR\0",             PREPROCESSOR )
PICK( "SYMBOL\0",                   SYMBOL )
PICK( "INVALIDTEXT\0",              INVALIDTEXT )
PICK( "IDENTIFIER\0",               IDENTIFIER )
PICK( "JUMPLABEL\0",                JUMPLABEL )
PICK( "COMMENT\0",                  COMMENT )
PICK( "FLOAT\0",                    FLOAT )
PICK( "STRING\0",                   STRING )
PICK( "VARIABLE\0",                 VARIABLE )
PICK( "REGEXP\0",                   REGEXP )
PICK( "FILETYPESOURCE\0",           FILETYPESOURCE )
PICK( "ENDFILETYPESOURCE\0",        ENDFILETYPESOURCE )
PICK( "LOCATE\0",                   LOCATE )
PICK( "ACTIVATEIDE\0",              ACTIVATEIDE )
PICK( "CHECKOUT\0",                 CHECKOUT )
PICK( "CHECKIN\0",                  CHECKIN )
