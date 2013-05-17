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
* Description:  Declaration of editor global variables.
*
****************************************************************************/


#ifndef _GLOBALS_INCLUDED
#define _GLOBALS_INCLUDED

/* strings */
extern char _NEAR   TITLE[];
extern char _NEAR   AUTHOR[];
extern char _NEAR   VERSIONT[];
extern char _NEAR   DATESTAMP_T[];
extern char _NEAR   DATESTAMP_D[];
extern char _NEAR   MSG_CHARACTERS[];
extern char _NEAR   MSG_LINES[];
extern char _NEAR   MSG_PRESSANYKEY[];
extern char _NEAR   MSG_DELETEDINTOBUFFER[];
extern char _NEAR   CONFIG_FILE[];
extern char _NEAR   MEMORIZE_MODE[];
extern char _NEAR   SingleBlank[];
extern char _NEAR   SingleSlash[];
extern char _NEAR   SingleQuote[];
extern char _NEAR   * _NEAR EditOpts[];
extern char         * _NEAR BoolStr[];
extern int          NumEditOpts;

/* mouse data */
#if defined( __LINUX__ )        /* compatible with the ui lib */
extern unsigned short   MouseRow;
extern unsigned short   MouseCol;
extern unsigned short   MouseStatus;
#else
extern int              MouseRow;
extern int              MouseCol;
extern int              MouseStatus;
#endif
extern vi_mouse_event   LastMouseEvent;

/* generic editing globals */
extern long         NextAutoSave;
extern int          HalfPageLines;
extern vi_key       LastEvent;
extern int          SpinCount;
extern char _NEAR   SpinData[];
extern char         VideoPage;
extern char         *BndMemory, *EXEName;
extern int          FcbBlocksInUse;
extern char * _NEAR MatchData[MAX_SEARCH_STRINGS * 2];
extern int          MatchCount;
extern mark         *MarkList;
extern fcb          *FcbThreadHead, *FcbThreadTail;
extern info         *InfoHead, *InfoTail, *CurrentInfo;
extern file         *CurrentFile;
extern fcb          *CurrentFcb;
extern line         *WorkLine;
extern line         *CurrentLine;
extern i_mark       CurrentPos;
extern i_mark       LeftTopPos;
extern int          VirtualColumnDesired;
extern window_id    CurrentWindow, MessageWindow, StatusWindow, CurrNumWindow;
extern window_id    MenuWindow;
extern char         *Comspec;
extern select_rgn   SelRgn;

/* historys */

/* keymap data */
extern int      CurrentKeyMapCount;
extern vi_key   *CurrentKeyMap;
extern key_map  *KeyMaps, *InputKeyMaps;

/* savebuf data */
extern int              CurrentSavebuf;
extern int              SavebufNumber;
extern char             LastSavebuf;
extern savebuf _NEAR    Savebufs[MAX_SAVEBUFS];
extern savebuf _NEAR    SpecialSavebufs[MAX_SPECIAL_SAVEBUFS + 1];
extern savebuf          *WorkSavebuf;

/* undo data */
extern int          MaxUndoStack;
extern undo_stack   *UndoStack, *UndoUndoStack;

/* bound key data */
extern event _NEAR  EventList[];
extern int          MaxKeysBound;
extern vi_key _NEAR SavebufBound[];

/* directory info */
extern direct_ent * _NEAR   DirFiles[MAX_FILES];
extern int                  DirFileCount;

/* window info */
extern window_info  editw_info, messagew_info, statusw_info;
extern window_info  cmdlinew_info, dirw_info;
extern window_info  setw_info, filelistw_info, setvalw_info;
extern window_info  linenumw_info, filecw_info;
extern window_info  repcntw_info, menubarw_info, menuw_info;
extern window_info  extraw_info, defaultw_info;
extern window_info  activemenu_info, greyedmenu_info, activegreyedmenu_info;

/* misc data */
extern long             SystemRC;
extern char             *CommandBuffer;
extern vi_rc            LastRetCode;
extern vi_rc            LastRC;
extern long             MaxMemFree, MemoryLeft, MaxMemFreeAfterInit;
extern int              RegExpError;
extern eflags           EditFlags;
extern evars            EditVars;
extern vi_rc            LastError;
extern int              maxdotbuffer;
extern vi_key           *DotBuffer, *DotCmd, *AltDotBuffer;
extern int              DotDigits, DotCount, DotCmdCount, AltDotDigits, AltDotCount;
extern volatile long    ClockTicks;
extern int              RepeatDigits;
extern bool             NoRepeatInfo;
extern char _NEAR       RepeatString[MAX_REPEAT_STRING];
extern int              SourceErrCount;
extern bool             BoundData;

/* file io globals */
extern int              SwapBlocksInUse;
extern int              SwapFileHandle;
extern int              SwapBlockArraySize, XMemBlockArraySize;
extern char             *ReadBuffer, *WriteBuffer;
extern unsigned char    *XMemBlocks;
extern unsigned char    *SwapBlocks;
extern char             *HomeDirectory, *CurrentDirectory;
extern int              TotalEMSBlocks;
extern int              EMSBlocksInUse;
extern int              TotalXMSBlocks;
extern int              XMSBlocksInUse;

/* windowing globals */
extern char         ScrollBarChar;
extern int          ScrollBarCharColor;

/* parse constants */
extern int          MaxColorTokens, ColorTokensSize;
extern char _NEAR   ParseClTokens[];
extern char _NEAR   SetTokens1[];
extern char _NEAR   SetTokens2[];
extern char _NEAR   SetTokens2a[];

/* Toolbar constants */

#endif
