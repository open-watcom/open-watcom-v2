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
* Description:  Definition if internal editor structures. 
*
****************************************************************************/


#ifndef _STRUCT_INCLUDED
#define _STRUCT_INCLUDED

#include <stdio.h>

typedef unsigned short  vi_ushort;

typedef struct ss {
    struct ss   *next, *prev;
} ss;

typedef struct {
    char        height;
    char        width;
} cursor_type;

typedef struct {
    unsigned char   case_ignore     : 1;
    unsigned char   use_regexp      : 1;
    unsigned char   search_forward  : 1;
    unsigned char   search_wrap     : 1;
    unsigned char   prompt          : 1;
    unsigned char   selection       : 1;
    unsigned char   spare           : 2;
    int             posx;
    int             posy;
    char            *find;
    int             findlen;
    char            *replace;
    int             replacelen;
    char            *ext;
    int             extlen;
    char            *path;
    int             pathlen;
} fancy_find;

typedef struct {
    int         max;
    int         curr;
    char        **data;
} history_data;

typedef struct {
    unsigned char   inuse           : 1;
    unsigned char   is_base         : 1;
    unsigned char   was_inuse       : 1;
    unsigned char   no_input_window : 1;
    unsigned char   fill5           : 1;
    unsigned char   fill6           : 1;
    unsigned char   fill7           : 1;
    unsigned char   fill8           : 1;
    vi_key          *data;
} key_map;

/* command structure */
typedef struct {
    unsigned char       len;
    char                cmd[255];
} cmd_struct;

/*
 * alias type
 */
typedef struct alias_list {
    struct alias_list   *next, *prev;
    char                *alias;
    char                *expand;
} alias_list;

/*
 * line type
 */
typedef long linenum;

typedef struct i_mark {
    linenum     line;
    int         column;
} i_mark;

/*
 * file stack
 */
typedef struct file_stack {
    i_mark      p;
    char        fname[1];
} file_stack;

/*
 * directory entry
 */
typedef struct {
    unsigned    sec     : 5;
    unsigned    min     : 6;
    unsigned    hour    : 5;
} time_struct;

typedef struct {
    unsigned    day     : 5;
    unsigned    month   : 4;
    unsigned    year    : 7;
} date_struct;

typedef struct {
    char                attr;
    long                fsize;
#ifndef __UNIX__
    time_struct         time;
    date_struct         date;
#else
    unsigned long       time;
    unsigned short      st_mode;
#endif
    char                name[1];
} direct_ent;

/*
 * window dimension
 */
typedef struct {
    short       x1, y1, x2, y2;
} windim;

/*
 * window id
 */
#ifdef __WIN__
    #include "winhdr.h"
    #ifdef HAS_HWND
        typedef HWND window_id;
    #elif defined( __WINDOWS_386__ )
        typedef unsigned short window_id;
    #else
        typedef const void _NEAR * window_id;
    #endif
#else
    typedef int window_id;
#endif

/*
 * info for a single text file line
 */
typedef struct linedata {
    vi_ushort   mark        : 5;    // first mark on the line
    vi_ushort   globmatch   : 1;    // global command matched this line
    vi_ushort   nolinedata  : 1;    // no data associated with this line (WorkLine
                                    // has the data instead)
    vi_ushort   hidden      : 1;    // line is hidden (NYI)
    vi_ushort   hilite      : 1;    // line need hiliting
    vi_ushort   fill10      : 1;
    vi_ushort   fill11      : 1;
    vi_ushort   fill12      : 1;
    vi_ushort   fill13      : 1;
    vi_ushort   fill14      : 1;
    vi_ushort   fill15      : 1;
    vi_ushort   fill16      : 1;
} linedata;

typedef struct line {
    struct line *next, *prev;   // links for other lines
    short       len;            // length of line
    union {
        linedata        ld;
        short           word;
    } inf;
    char        data[1];        // actual string for line
} line;

typedef struct {
    line    *head, *tail;
} line_list;

/*
 * info for a file control block in a file
 */
struct file;
typedef struct fcb {
    struct fcb  *next, *prev;               // links fcbs in a file
    struct fcb  *thread_next, *thread_prev; // links all fcbs created
    struct file *f;                         // file associated with fcb
    line_list   lines;                      // linked list of lines
    linenum     start_line, end_line;       // starting/ending line number
    short       byte_cnt;                   // number of bytes in lines
    long        offset;                     // offset in swap file
    long        last_swap;                  // time fcb was last swapped
    vi_ushort   swapped             : 1;    // fcb is swapped
    vi_ushort   in_memory           : 1;    // fcb is in memory
    vi_ushort   on_display          : 1;    // lines in fcb are displayed
    vi_ushort   non_swappable       : 1;    // fcb is not swappable
    vi_ushort   dead                : 1;    // fcb is dead (obsolete)
    vi_ushort   was_on_display      : 1;    // fcb was on display (used to save
                                            // display state when switching files)
    vi_ushort   in_extended_memory  : 1;    // fcb is in extended memory
    vi_ushort   in_xms_memory       : 1;    // fcb is in XMS memory
    vi_ushort   in_ems_memory       : 1;    // fcb is in EMS memory
    vi_ushort   nullfcb             : 1;    // fcb is a special one that has no
                                            // lines associated with it
    vi_ushort   globalmatch         : 1;    // a global command matched at least
                                            // one line in this fcb
    vi_ushort   flag12              : 1;
    vi_ushort   flag13              : 1;
    vi_ushort   flag14              : 1;
    vi_ushort   flag15              : 1;
    vi_ushort   flag16              : 1;
    long        xmemaddr;                   // address of fcb in extended memory
} fcb;
#define FCB_SIZE sizeof( fcb )

typedef struct {
    fcb         *head, *tail;
} fcb_list;

/*
 * info for an entire file
 */
typedef struct file {
    char        *name;                  // file name
    char        *home;                  // home directory of file
    fcb_list    fcbs;                   // linked list of fcbs
    long        curr_pos;               // current offset in file on disk
    vi_ushort   modified        : 1;    // file has been modified
    vi_ushort   bytes_pending   : 1;    // there are still bytes to be read
                                        // off the disk for the file
    vi_ushort   viewonly        : 1;    // file is view only
    vi_ushort   read_only       : 1;    // file is read only
    vi_ushort   check_readonly  : 1;    // file needs its read-only status
                                        // checked against the file on disk
    vi_ushort   dup_count       : 4;    // number of duplicate views on the
                                        // file that have been opened
    vi_ushort   been_autosaved  : 1;    // file has been autosaved
    vi_ushort   need_autosave   : 1;    // file needs to be autosaved
    vi_ushort   is_stdio        : 1;    // file is a "stdio" file (reads from
                                        // stdin and writes to stdout)
    vi_ushort   needs_display   : 1;    // file needs to be displayed
    vi_ushort   check_for_crlf  : 1;    // check file system when we write it out
    vi_ushort   fill15          : 1;
    vi_ushort   fill16          : 1;
    long        size;                   // size of file in bytes
    int         handle;                 // file handle (if entire file is not
                                        // read, will be an open file handle)
#ifdef __UNIX__
    short       attr;
#endif
    char        as_name[TMP_NAME_LEN];  // name that file was auto-saved as
} file;
#define FILE_SIZE sizeof( file )

/*
 * mark setting
 */
typedef struct {
    i_mark      p;              // line number and column number that mark is on
    char        next;           // pointer to next mark on the same line
    vi_ushort   inuse   : 1;    // mark is being used
    vi_ushort   spare   : 3;
} mark;
#define MARK_SIZE sizeof( mark )

typedef struct range {
    i_mark              start;
    i_mark              end;
/*
 * ACK! Here we have a lovely little wart on the operator/movement
 * system. Some commands want to highlight a region of text after they
 * have moved; to support this we add these marks which tell us where
 * to begin and end highlighting, as well as a flag to decide whether to
 * actually do the highlighting. Puke.
 */
    i_mark          hi_start;
    i_mark          hi_end;
    unsigned char   highlight   : 1;
    unsigned char   line_based  : 1;
/*
 * Double ACK! Some times we need to treat a range differently depending
 * on whether an operator or a move is using it. This tells us if we should
 * include the last character or not (compare "d/foo" to "/foo"). Puke.
 */
    unsigned char   fix_range   : 1;
    unsigned char   selected    : 1;
    unsigned char   spare       : 4;
} range;

typedef vi_rc (*insert_rtn)( void );
typedef vi_rc (*move_rtn)( range *, long count );
typedef vi_rc (*op_rtn)( range * );
typedef vi_rc (*misc_rtn)( long count );
typedef vi_rc (*old_rtn)( void );
typedef vi_rc (*alias_rtn)( void *, void * );

typedef union command_rtn {
    vi_rc       (*dummy)(); /* Must go first to avoid parameter type mismatches */
    insert_rtn  ins;
    move_rtn    move;
    op_rtn      op;
    misc_rtn    misc;
    old_rtn     old;
    alias_rtn   alias;
} command_rtn;

typedef struct {
    unsigned    type                    : 3;    // type of event (list in const.h)
    unsigned    keep_selection          : 1;    // keep selection after this event?
    unsigned    is_number               : 1;    // is event a digit? (repeat count)
    unsigned    fix_range               : 1;    // see comment in doMove (editmain.c)
    unsigned    is_op_alias             : 1;    // event is an aliased operator
    unsigned    keep_selection_maybe    : 1;    // keep selection if already selecting
    unsigned    modifies                : 1;    // does this event modify the file?
    unsigned    spare                   : 7;
} event_bits;

typedef struct {
    command_rtn alt_rtn;
    insert_rtn  ins;
    command_rtn rtn;
    event_bits  b;
    event_bits  alt_b;
} event;

/* used to cast a base event (as in keys.h) to a char */
#define EVENT_CHAR( e ) ((char) e)

/*
 * structure to define all the goop needed to display text
 */
typedef struct {
    vi_color    foreground;
    vi_color    background;
    font_type   font;
} type_style;

/*
 * window init info
 */
typedef struct {
    bool        has_border;
    vi_color    border_color1, border_color2;
    type_style  text;
    type_style  hilight;
    short       x1, y1, x2, y2;
} window_info;

/*
 * undo info
 */
typedef struct {
    linenum     start, end;
} undo_insert;

typedef fcb_list undo_delete;

typedef struct {
    i_mark      p;
    linenum     top;
    short       depth;
    long        time_stamp;
} undo_start;

typedef union {
   undo_start   sdata;
   undo_insert  del_range;
   undo_delete  fcbs;
} undo_data;

typedef struct undo {
    struct undo *next;
    char        type;
    undo_data   data;
} undo;
#define UNDO_SIZE sizeof( undo )

typedef struct undo_stack {
    int         current;                // current depth of undo stack
    int         OpenUndo;               // number of "StartUndoGroups" open
                                        // on stack
    undo        **stack;                // stack data
    bool        rolled;                 // stack has been rolled (i.e., data
                                        // has rolled off the end)
} undo_stack;

typedef struct select_rgn {
    i_mark      start;
    i_mark      end;
    int         start_col_v;
    vi_ushort   selected    : 1;
    vi_ushort   lines       : 1;
    vi_ushort   dragging    : 1;
    vi_ushort   empty       : 13;
} select_rgn;

/*
 * all info for a file being edited
 */
typedef struct info {
    struct info *next, *prev;
    file        *CurrentFile;
    i_mark      CurrentPos;
    i_mark      LeftTopPos;
    undo_stack  *UndoStack, *UndoUndoStack;
    int         CurrentUndoItem, CurrentUndoUndoItem;
    window_id   CurrNumWindow;
    mark        *MarkList;
    bool        linenumflag;
    window_id   CurrentWindow;
    int         VirtualColumnDesired;
    bool        CMode;
    bool        WriteCRLF;
    select_rgn  SelRgn;
    bool        IsColumnRegion;
    vi_ushort   DuplicateID;
    void        *dc;
    int         dc_size;
    lang_t      Language;
    bool        RealTabs;
    bool        ReadEntireFile;
    bool        ReadOnlyCheck;
    bool        EightBits;
    int         TabAmount;
    int         HardTab;
    int         ShiftWidth;
    int         AutoIndent;
#ifdef __WIN__
    long        VScrollBarScale;
    int         HScrollBarScale;
#endif
} info;
#define INFO_SIZE sizeof( info )

/*
 * save buffer (for yanking/moving text)
 */
typedef struct savebuf {
    char        type;
    union {
        char        *data;
        fcb_list    fcbs;
    } u;
} savebuf;
#define SAVEBUF_SIZE sizeof( savebuf )

/*
 * color settings
 */
typedef struct {
    char        red, green, blue;
} rgb;

/*
 * SelectItem data
 */
typedef struct {
    window_info *wi;            // info describing window to create
    char        *title;         // title of window
    char        **list;         // lines to display
    int         maxlist;        // number of lines in list
    char        *result;        // where to copy the data for the picked line
    int         num;            // number of the picked line
    int         *allowrl;       // allow cursor right/left (for menu bar)
    char        **hilite;       // chars to highlight
    vi_key      *retevents;     // events that simulate pressing enter
    vi_key      event;          // event that caused a return
    bool        show_lineno;    // show lines in top-right corner
    linenum     cln;            // current line to display
    window_id   eiw;            // alternate window to accept events in (like
                                // the options window after fgrep...)
    bool        is_menu : 1;    // is a menu we are showing
    bool        spare   : 7;
} selectitem;

/*
 * SelectLineInFile data structure
 */
typedef struct {
    file        *f;                 // file with data for lines
    char        **vals;             // values associated with each line
    int         valoff;             // offset to display values beside line data
    window_info *wi;                // info describing window to create
    linenum     sl;                 // selected line
    char        *title;             // title of window
    vi_rc       (*checkres)(char *, char *, int * ); // check if selected
                                    // change is valid
    int         *allow_rl;          // allow cursor right/left (for menu bar)
    char        **hilite;           // chars to highlight
    bool        show_lineno;        // show lines in top-right corner
    vi_key      *retevents;         // events that simulate pressing enter
    vi_key      event;              // event that caused a return
    linenum     cln;                // current line to display
    window_id   eiw;                // alternate window to accept events in (like
                                    // the options window after fgrep...)
    bool        is_menu             : 1; // select list is a menu
    bool        has_scroll_gadgets  : 1; // list has scroll gadgets
    bool        spare               : 6;
} selflinedata;

/*
 * special file (used to process bound data)
 */
typedef struct {
    int length;
    int maxlines, currline;
} gfa;

typedef struct {
    struct line *cline;
    struct fcb  *cfcb;
} gfb;
typedef struct {
    union {
        FILE            *f;
        int             handle;
        char            *pos;
        struct file     *cfile;
    } data;
    union {
        gfa             a;
        gfb             b;
    } gf;
    gftype type;
} GENERIC_FILE;

typedef struct {
    /*
     * set booleans are here
     */
    #define PICK( a, b, c, d, e ) bool c;
    #include "setb.h"

    /*
     * internal booleans are here
     */
    bool DisplayHold;
    bool Starting;
    bool DotMode;
    bool Dotable;
    bool KeyMapMode;
    bool ClockActive;
    bool KeyOverride;
    bool ViewOnly;
    bool NewFile;
    bool SourceScriptActive;
    bool InputKeyMapMode;
    bool LineWrap;
    bool Monocolor;
    bool BlackAndWhite;
    bool Color;
    bool KeyMapInProgress;
    bool ResetDisplayLine;
    bool GlobalInProgress;
    bool ExtendedKeyboard;
    bool BreakPressed;
    bool AllowRegSubNewline;
    bool BoundData;
    bool SpinningOurWheels;
    bool ReadOnlyError;
    bool WindowsStarted;
    bool CompileScript;
    bool ScriptIsCompiled;
    bool CompileAssignments;
    bool OpeningFileToCompile;
    bool InsertModeActive;
    bool WatchForBreak;
    bool EchoOn;
    bool LoadResidentScript;
    bool CompileAssignmentsDammit;
    bool ExMode;
    bool Appending;
    bool LineDisplay;
    bool NoSetCursor;
    bool NoInputWindow;
    bool ResizeableWindow;
    bool BndMemoryLocked;
    bool MemorizeMode;
    bool DuplicateFile;
    bool NoReplaceSearchString;
    bool LastSearchWasForward;
    bool UndoLost;
    bool NoAddToDotBuffer;
    bool Dragging;
    bool NoCapsLock;
    bool RecoverLostFiles;
    bool IgnoreLostFiles;
    bool UseIDE;
    bool HasSystemMouse;
    bool UndoInProg;
    bool StdIOMode;
    bool NoInitialFileLoad;
    bool WasOverstrike;
    bool ReturnToInsertMode;
    bool AltMemorizeMode;
    bool AltDotMode;
    bool EscapedInsertChar;
    bool HoldEverything;
    bool IsWindowedConsole;
    bool ModeInStatusLine;
    bool IsChangeWord;
    bool OperatorWantsMove;
    bool ScrollCommand;
    bool FileTypeSource;
} eflags;               // don't forget to give default in globals.c

#endif
