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
* Description:  Definition of editor's global variables.
*
****************************************************************************/


#include "vi.h"
#include "menu.h"
#include "ex.h"
#include "rxsupp.h"

/* strings */
char _NEAR      MSG_CHARACTERS[] = "characters";
char _NEAR      MSG_LINES[] = "lines";
char _NEAR      MSG_PRESSANYKEY[] = "Press any key";
char _NEAR      MSG_DELETEDINTOBUFFER[] = " deleted into buffer ";
char _NEAR      MEMORIZE_MODE[] = "Memorize Mode ";
char _NEAR      CONFIG_FILE[] = CFG_NAME;
char _NEAR      SingleBlank[] = " ";
char _NEAR      SingleSlash[] = "/";
char _NEAR      SingleQuote[] = "\"";
char            * _NEAR BoolStr[] = { "FALSE", "TRUE" };

/* edit options */
char _NEAR * _NEAR EditOpts[] =  {
    "<F1> Go To",
    "<F2> Edit",
    "<F3> Get All"
};
int NumEditOpts = sizeof( EditOpts ) / sizeof( char _NEAR * );

/* event data */
event _NEAR EventList[] = {
    #define vi_pick( enum, modeless, insert, command, nm_bits, bits ) \
        modeless, insert, command, nm_bits, bits,
    #include "events.h"
    #undef vi_pick
};

/* mouse data */
#if !defined( __UNIX__ )
int             MouseRow;
int             MouseCol;
int             MouseStatus;
#endif
vi_mouse_event  LastMouseEvent = MOUSE_NONE;

/* generic editing data */
long            NextAutoSave;
int             HalfPageLines;
vi_key          LastEvent;
int             SpinCount;
char            _NEAR SpinData[] = { '\xC4', '\\', '|', '/' };
char            VideoPage;
char            *EXEName;
char            *BndMemory;
long            SystemRC;
int             FcbBlocksInUse;
mark            *MarkList;
fcb             *FcbThreadHead;
fcb             *FcbThreadTail;
info            *InfoHead;
info            *InfoTail;
info            *CurrentInfo;
file            *CurrentFile;
fcb             *CurrentFcb;
line            *CurrentLine;
line            *WorkLine = NULL;
i_mark          CurrentPos = { 1, 1 };
i_mark          LeftTopPos = { 1, 0 };
int             VirtualColumnDesired = 1;
window_id       CurrentWindow = NO_WINDOW;
window_id       MessageWindow = NO_WINDOW;
window_id       StatusWindow = NO_WINDOW;
window_id       MenuWindow = NO_WINDOW;
window_id       CurrNumWindow = NO_WINDOW;
select_rgn      SelRgn;

/*
 * directory data
 */
direct_ent      * _NEAR DirFiles[MAX_FILES];
int             DirFileCount;

/*
 * key map data
 */
int             CurrentKeyMapCount;
vi_key          *CurrentKeyMap;
key_map         *KeyMaps;
key_map         *InputKeyMaps;

/*
 * savebuf data
 */
vi_key          _NEAR SavebufBound[MAX_SAVEBUFS] =
                        { VI_KEY( CTRL_F1 ), VI_KEY( CTRL_F2 ),
                          VI_KEY( CTRL_F3 ), VI_KEY( CTRL_F4 ),
                          VI_KEY( CTRL_F5 ), VI_KEY( CTRL_F6 ),
                          VI_KEY( CTRL_F7 ), VI_KEY( CTRL_F8 ),
                          VI_KEY( CTRL_F9 ) };
int             CurrentSavebuf = 0;     /* this is 0 based */
char            LastSavebuf;    /* this is 1 based - users see it */
savebuf         _NEAR Savebufs[MAX_SAVEBUFS];
savebuf         _NEAR SpecialSavebufs[MAX_SPECIAL_SAVEBUFS + 1];
savebuf         *WorkSavebuf = &SpecialSavebufs[MAX_SPECIAL_SAVEBUFS];
int             SavebufNumber = NO_SAVEBUF; /* this is 0 based */

/*
 * undo data
 */
undo_stack      *UndoStack;
undo_stack      *UndoUndoStack;

/*
 * windows data
 */
#ifndef __WIN__
window_info cmdlinew_info = { 0, WHITE, BLACK, { WHITE, BLACK, 0 },
    { BRIGHT_WHITE, BLACK, 0 }, 0, 24, 79, 24 };
window_info statusw_info = { 0, WHITE, BLACK, { WHITE, BLACK, 0 },
    { BRIGHT_WHITE, BLACK, 0 }, 69, 24, 79, 24 };
window_info repcntw_info = { 1, WHITE, BLACK, { WHITE, BLACK, 0 },
    { BRIGHT_WHITE, BLACK, 0 }, 28, 20, 43, 32 };
window_info editw_info = { 0, WHITE, BLACK, { WHITE, BLACK, 0 },
    { BRIGHT_WHITE, BLACK, 0 }, 0, 0, 79, 24 };
window_info extraw_info = { 1, WHITE, BLACK, { WHITE, BLACK, 0 },
    { BRIGHT_WHITE, BLACK, 0 }, 26, 2, 51, 18 };
window_info filecw_info = { 1, WHITE, BLACK, { WHITE, BLACK, 0 },
    { BRIGHT_WHITE, BLACK, 0 }, 4, 8, 75, 17 };
window_info linenumw_info = { 0, WHITE, BLACK, { WHITE, BLACK, 0 },
    { BRIGHT_WHITE, BLACK, 0 }, 1, 21, 8, 24 };
window_info dirw_info = { 1, WHITE, BLACK, { WHITE, BLACK, 0 },
    { BRIGHT_WHITE, BLACK, 0 }, 10, 2, 69, 17 };
window_info filelistw_info = { 1, WHITE, BLACK, { WHITE, BLACK, 0 },
    { BRIGHT_WHITE, BLACK, 0 }, 26, 2, 78, 18 };
window_info setw_info = { 1, WHITE, BLACK, { WHITE, BLACK, 0 },
    { BRIGHT_WHITE, BLACK, 0 }, 12, 2, 40, 21 };
window_info setvalw_info = { 1, WHITE, BLACK, { WHITE, BLACK, 0 },
    { BRIGHT_WHITE, BLACK, 0 }, 43, 6, 70, 9 };
window_info messagew_info = { 0, WHITE, BLACK, { WHITE, BLACK, 0 },
    { BRIGHT_WHITE, BLACK, 0 }, 0, 24, 79, 24 };
window_info menuw_info = { 1, WHITE, BLACK, { WHITE, BLACK, 0 },
    { BRIGHT_WHITE, BLACK, 0 }, 0, 1, 0, 0 };
window_info menubarw_info = { 0, WHITE, BLACK, { WHITE, BLACK, 0 },
    { BRIGHT_WHITE, BLACK, 0 }, 0, 0, 79, 0 };
window_info defaultw_info = { 0, WHITE, BLACK, { WHITE, BLACK, 0 },
    { BRIGHT_WHITE, BLACK, 0 }, 0, 1, 79, 22 };
window_info activemenu_info = { 0, WHITE, BLACK, { WHITE, BLACK, 0 },
    { BRIGHT_WHITE, BLACK, 0 }, 0, 0, 0, 0 };
window_info greyedmenu_info = { 0, WHITE, BLACK, { WHITE, BLACK, 0 },
    { BRIGHT_WHITE, BLACK, 0 }, 0, 0, 0, 0 };
window_info activegreyedmenu_info = { 0, WHITE, BLACK, { WHITE, BLACK, 0 },
    { BRIGHT_WHITE, BLACK, 0 }, 0, 0, 0, 0 };
#else
/* the ONE TRUE configuration - do not change this or nothing will work */
window_info cmdlinew_info = { 1, WHITE, BLACK, { WHITE, BLACK, 0 },
    { BRIGHT_WHITE, BLACK, 0 }, 0, 23, 69, 25 };
window_info statusw_info = { 0, WHITE, BLACK, { WHITE, BLACK, 0 },
    { BRIGHT_WHITE, BLACK, 0 }, 69, 24, 79, 24 };
window_info repcntw_info = { 1, WHITE, BLACK, { WHITE, BLACK, 0 },
    { BRIGHT_WHITE, BLACK, 0 }, 28, 20, 43, 32 };
window_info editw_info = { 0, WHITE, BLACK, { WHITE, BLACK, 0 },
    { BRIGHT_WHITE, BLACK, 0 }, 0, 0, 79, 23 };
window_info extraw_info = { 1, WHITE, BLACK, { WHITE, BLACK, 0 },
    { BRIGHT_WHITE, BLACK, 0 }, 26, 2, 51, 18 };
window_info filecw_info = { 1, WHITE, BLACK, { WHITE, BLACK, 0 },
    { BRIGHT_WHITE, BLACK, 0 }, 4, 8, 75, 17 };
window_info linenumw_info = { 0, WHITE, BLACK, { WHITE, BLACK, 0 },
    { BRIGHT_WHITE, BLACK, 0 }, 1, 21, 8, 24 };
window_info dirw_info = { 1, WHITE, BLACK, { WHITE, BLACK, 0 },
    { BRIGHT_WHITE, BLACK, 0 }, 10, 2, 69, 17 };
window_info filelistw_info = { 1, WHITE, BLACK, { WHITE, BLACK, 0 },
    { BRIGHT_WHITE, BLACK, 0 }, 26, 2, 78, 18 };
window_info setw_info = { 1, WHITE, BLACK, { WHITE, BLACK, 0 },
    { BRIGHT_WHITE, BLACK, 0 }, 12, 2, 40, 21 };
window_info setvalw_info = { 1, WHITE, BLACK, { WHITE, BLACK, 0 },
    { BRIGHT_WHITE, BLACK, 0 }, 43, 6, 70, 9 };
window_info messagew_info = { 0, WHITE, BLACK, { WHITE, BLACK, 0 },
    { BRIGHT_WHITE, BLACK, 0 }, 0, 25, 69, 25 };
window_info menuw_info = { 1, WHITE, BLACK, { WHITE, BLACK, 0 },
    { BRIGHT_WHITE, BLACK, 0 }, 0, 1, 0, 0 };
window_info menubarw_info = { 0, WHITE, BLACK, { WHITE, BLACK, 0 },
    { BRIGHT_WHITE, BLACK, 0 }, 0, 0, 79, 0 };
window_info defaultw_info = { 1, WHITE, BLACK, { WHITE, BLACK, 0 },
    { BRIGHT_WHITE, BLACK, 0 }, 0, 1, 79, 22 };
window_info activemenu_info = { 0, WHITE, BLACK, { WHITE, BLACK, 0 },
    { BRIGHT_WHITE, BLACK, 0 }, 0, 0, 0, 0 };
window_info greyedmenu_info = { 0, WHITE, BLACK, { WHITE, BLACK, 0 },
    { BRIGHT_WHITE, BLACK, 0 }, 0, 0, 0, 0 };
window_info activegreyedmenu_info = { 0, WHITE, BLACK, { WHITE, BLACK, 0 },
    { BRIGHT_WHITE, BLACK, 0 }, 0, 0, 0, 0 };
#endif

/*
 * file io data
 */
char            *CommandBuffer = NULL;
char            *CurrentDirectory = NULL;
char            *HomeDirectory = NULL;
char            *ReadBuffer = NULL;
char            *WriteBuffer = NULL;
unsigned char   *SwapBlocks = NULL;
int             SwapFileHandle = -1;
int             SwapBlockArraySize;
int             SwapBlocksInUse;
char            *Comspec = NULL;

/*
 * misc data
 */
vi_rc           LastRetCode;
vi_rc           LastRC;
long            MaxMemFree;
long            MaxMemFreeAfterInit;
int             RegExpError;
regexp          *CurrentRegularExpression = NULL;
char            * _NEAR MatchData[MAX_SEARCH_STRINGS * 2];
int             MatchCount = INITIAL_MATCH_COUNT;
vi_rc           LastError;
int             maxdotbuffer = 1024;
vi_key          *DotBuffer = NULL;
vi_key          *AltDotBuffer = NULL;
vi_key          *DotCmd = NULL;
int             DotDigits;
int             DotCount;
int             DotCmdCount;
int             AltDotDigits;
int             AltDotCount;
volatile long   ClockTicks;
int             RepeatDigits;
bool            NoRepeatInfo;
char            _NEAR RepeatString[MAX_REPEAT_STRING];
int             SourceErrCount;
bool            BoundData = false;

#define INITVARS
/*
 * edit flags
 */
eflags EditFlags = {
    #define PICK( a,b,c,d,e )   d,
    #include "setb.h"
    #undef PICK

    /*
     * internal booleans are here
     */
    #define PICK( a,b )         b,
    #include "setbi.h"
    #undef PICK
};

/*
 * edit vars
 */
evars EditVars = {
    #define PICK( a,b,c,d,e,f ) e,
    #include "setnb.h"
    #undef PICK

    /*
     * internal vars are here
     */
    #define PICK( a,b,c )       c,
    #include "setnbi.h"
    #undef PICK
};
#undef INITVARS
