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


#ifndef _GLOBALS_INCLUDED
#define _GLOBALS_INCLUDED

/* strings */
extern char near TITLE[];
extern char near AUTHOR[];
extern char near VERSION[];
extern char near DATESTAMP_T[];
extern char near DATESTAMP_D[];
extern char near MSG_CHARACTERS[];
extern char near MSG_LINES[];
extern char near MSG_PRESSANYKEY[];
extern char near MSG_DELETEDINTOBUFFER[];
extern char near CONFIG_FILE[];
extern char near MEMORIZE_MODE[];
extern char near SingleBlank[];
extern char near SingleSlash[];
extern char near *near EditOpts[];
extern char near * near BoolStr[];
extern int NumEditOpts;

/* mouse data */
extern int MouseRow;
extern int MouseCol;
extern int MouseSpeed;
extern int MouseStatus;
extern int MouseDoubleClickSpeed;
extern int LastMouseEvent;
extern int MouseRepeatStartDelay;
extern int MouseRepeatDelay;

/* generic editing globals */
extern char *FileEndString;
extern char *StatusString;
extern int MaxPush;
extern int Radix;
extern char *WordDefn;
extern char *WordAltDefn;
extern int AutoSaveInterval;
extern int Language;
extern long NextAutoSave;
extern int HalfPageLines;
extern int PageLinesExposed;
extern char *GrepDefault;
extern int LastEvent;
extern int StackK;
extern char SpinCount;
extern char near SpinData[];
extern char ExitAttr,VideoPage;
extern char *Majick,*BndMemory,*EXEName,*HistoryFile,*TagFileName;
extern char *StaticBuffer;
extern int CurrFIgnore;
extern char *FIgnore;
extern int FcbBlocksInUse;
extern char * near MatchData[ MAX_SEARCH_STRINGS*2 ];
extern int MatchCount;
extern mark *MarkList;
extern fcb *FcbThreadHead,*FcbThreadTail;
extern info *InfoHead,*InfoTail,*CurrentInfo;
extern file *CurrentFile;
extern fcb *CurrentFcb;
extern line *WorkLine;
extern line *CurrentLine;
extern linenum CurrentLineNumber,TopOfPage;
extern int CurrentColumn,LeftColumn;
extern int ColumnDesired;
extern window_id CurrentWindow,MessageWindow,StatusWindow,CurrNumWindow;
extern window_id MenuWindow;
extern int LastEvent;
extern char *Comspec;
extern select_rgn SelRgn;
extern int CursorBlinkRate;

/* historys */
extern history_data FilterHist;
extern history_data CLHist;
extern history_data FindHist;
extern history_data LastFilesHist;

/* keymap data */
extern int CurrentKeyMapCount;
extern vi_key *CurrentKeyMap;
extern key_map *KeyMaps,*InputKeyMaps;

/* savebuf data */
extern int CurrentSavebuf;
extern int SavebufNumber;
extern char LastSavebuf;
extern savebuf near Savebufs[ MAX_SAVEBUFS ];
extern savebuf near SpecialSavebufs[ MAX_SPECIAL_SAVEBUFS+1 ];
extern savebuf *WorkSavebuf;

/* undo data */
extern int MaxUndoStack;
extern undo_stack *UndoStack,*UndoUndoStack;

/* bound key data */
extern event _NEAR EventList[];
extern int EventCount;
extern int MaxKeysBound;
extern int _NEAR SavebufBound[];
extern char *ScriptBound;
extern char *InputScriptBound;

/* directory info */
extern direct_ent * near DirFiles[MAX_FILES];
extern int DirFileCount;

/* window info */
extern window_info editw_info, messagew_info, statusw_info;
extern window_info cmdlinew_info, dirw_info;
extern window_info setw_info, filelistw_info, setvalw_info;
extern window_info linenumw_info, filecw_info;
extern window_info repcntw_info, menubarw_info, menuw_info;
extern window_info extraw_info, defaultw_info;
extern window_info activemenu_info, greyedmenu_info, activegreyedmenu_info;

/* misc data */
extern int WrapMargin;
extern short *StatusSections;
extern int NumStatusSections;
extern int SystemRC;
extern char *CommandBuffer;
extern int LastRetCode;
extern long LastRC;
extern long MaxMemFree,MemoryLeft,MaxMemFreeAfterInit;
extern cursor_type OverstrikeCursorType,InsertCursorType,NormalCursorType;
extern int RegExpError;
extern eflags EditFlags;
extern char near crlf[];
extern int LastError,TabAmount,ShiftWidth,HardTab;
extern int LineNumWinWidth;
extern int maxdotbuffer;
extern vi_key *DotBuffer,*DotCmd,*AltDotBuffer;
extern int DotDigits,DotCount,DotCmdCount,AltDotDigits,AltDotCount;
extern volatile long ClockTicks;
extern int RepeatDigits;
extern bool NoRepeatInfo;
extern char near RepeatString[MAX_REPEAT_STRING];
extern int SourceErrCount;
extern char near SpawnPrompt[];

/* file io globals */
extern int SwapBlocksInUse;
extern int MaxLine,MaxLinem1;
extern int SwapFileHandle,MaxSwapBlocks,SwapBlockArraySize,XMemBlockArraySize;
extern char *ReadBuffer,*WriteBuffer,*SwapBlocks,*XMemBlocks;
extern char *HomeDirectory,*CurrentDirectory,*TmpDir;
extern int TotalEMSBlocks,MaxEMSBlocks,EMSBlocksInUse;
extern int TotalXMSBlocks,MaxXMSBlocks,XMSBlocksInUse;

/* windowing globals */
extern char *TileColors;
extern int MaxTileColors;
extern int MoveColor,ResizeColor,MaxWindowTileX,MaxWindowTileY;
extern int SelectColor;
extern int InactiveWindowColor;
extern char ScrollBarChar;
extern int ScrollBarCharColor;
extern int ClockX,ClockY;
extern int SpinX,SpinY;
extern char WindMaxWidth,WindMaxHeight;
extern int CurrentStatusColumn;
extern char EndOfLineChar;
extern int GreyedMenuColour;

/* parse constants */
extern int MaxColorTokens,ColorTokensSize;
extern char near ParseClTokens[];
extern char near SetTokens1[];
extern char near SetTokens2[];
extern char near SetTokens2a[];

/* Toolbar constants */
extern int ToolBarButtonHeight;
extern int ToolBarButtonWidth;
extern int ToolBarColor;

#endif
