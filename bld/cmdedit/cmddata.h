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


GLOBAL char    OldLine[LINE_WIDTH];    // a saved copy of Line
GLOBAL int     OldMaxCursor;           // a saved copy of MaxCursor
GLOBAL int     OldCursor;              // a saved copy of Cursor
GLOBAL int     LineSaved;              // has the line ever been saved

GLOBAL int     More;           // do we still have another command? (from !)
GLOBAL int     BuffOne;        // is KBDChar a valid character or do we need to read?

GLOBAL int     Row;            // cursor row on screen
GLOBAL int     Col;            // cursor column on screen
GLOBAL int     ColOffset;      // distance from Col of our 'block' cursor
GLOBAL int     RowOffset;      // distance from Row of our 'block' cursor
GLOBAL int     StartCol;       // our original cursor column

GLOBAL int     Cursor;         // our cursor position within Line
GLOBAL int     MaxCursor;      // the length of Line
GLOBAL int     CursorDiff;     // amount to move the top of cursor to get insert cursor
GLOBAL int     Insert;         // are we in insert mode?
GLOBAL int     Overflow;       // The maximum MaxCursor
GLOBAL int     Edited;         // was the line Edited (or just RetrieveACommandd)
GLOBAL int     Base;           // where we start drawing (for left right scroll)
GLOBAL int     Draw;           // Do we need to redraw the line?
GLOBAL int     StartDraw;      // the first char that needs to be drawn (usually 0)
GLOBAL int     ImmedCommand;   // was this an Immediate PFKey command?
GLOBAL int     FirstNextOrPrev;// is this the first 'retrieval' key hit?
GLOBAL CURSOR  Cur;            // cursor information
GLOBAL KBDDESC Kbd;            // Keyboard information
GLOBAL KBDCHAR KbdChar;        // A character gotten from the keyboard
GLOBAL int     KbdHandle;      // The handle for kbd$ (OS/2)
GLOBAL char    far *PFChars;   // chars from a pfkey that we pretend are typed
GLOBAL char    Line[LINE_WIDTH];// the buffer for the command line

GLOBAL int     AlwaysSave;
GLOBAL int     FileLower;
GLOBAL int     AlwaysAlias;
GLOBAL int     StickyInsert;
GLOBAL int     StartInInsert;
GLOBAL int     FileUnique;
GLOBAL int     PrintAllFiles;
GLOBAL char    CmdSeparator;
GLOBAL int     WantAlias;
GLOBAL int     HideDirCmds;
GLOBAL int     AliasSize;
GLOBAL int     AppendSlash;

#ifdef DOS
    extern unsigned char far * far AliasList;
#else
    extern unsigned char far * AliasList;
    GLOBAL char AliasArea[ 2048 ]; /* The DLL seems to need static memory */
#endif

GLOBAL int      HaveDirent;
GLOBAL int      NextFileCalls;
GLOBAL char     PathBuff[LINE_WIDTH];
GLOBAL char far *PathCurr;
