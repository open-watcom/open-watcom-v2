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


#ifndef _STRUCT_INCLUDED
#define _STRUCT_INCLUDED

typedef unsigned short ushort;
typedef unsigned long ulong;
typedef unsigned short vi_key;

typedef struct {
    char        height;
    char        width;
} cursor_type;

typedef struct {
    char        case_ignore:1;
    char        use_regexp:1;
    char        search_forward:1;
    char        search_wrap:1;
    char        prompt:1;
    char        selection:1;
    char        spare:2;
    char        *find;
    int         findlen;
    char        *replace;
    int         replacelen;
    char        *ext;
    int         extlen;
    char        *path;
    int         pathlen;
} fancy_find;

typedef struct {
    int         max;
    int         curr;
    char        **data;
} history_data;

typedef struct {
    char        inuse:1;
    char        is_base:1;
    char        was_inuse:1;
    char        no_input_window:1;
    char        fill5:1,fill6:1,fill7:1,fill8:1;
    vi_key      *data;
} key_map;

/* command structure */
typedef struct {
    char        len;
    char        cmd[255];
} cmd_struct;

/*
 * alias type
 */
typedef struct alias_list {
    struct alias_list   *next,*prev;
    char                *alias;
    char                *expand;
} alias_list;

/*
 * line type
 */
typedef long linenum;

/*
 * file stack
 */
typedef struct file_stack {
    linenum             lineno;
    int                 col;
    char                fname[1];
} file_stack;

/*
 * directory entry
 */
typedef struct {
    unsigned    sec:5;
    unsigned    min:6;
    unsigned    hour:5;
} time_struct;

typedef struct {
    unsigned    day:5;
    unsigned    month:4;
    unsigned    year:7;
} date_struct;

typedef struct {
    char                attr;
    long                fsize;
#ifndef __QNX__
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
    short       x1,y1,x2,y2;
} windim;

/*
 * window id
 */
#ifdef __WIN__
    #include "winhdr.h"
    #ifdef HAS_HWND
        typedef HWND window_id;
    #else
    #ifdef __WINDOWS_386__
        typedef unsigned short window_id;
    #else
        typedef const void near * window_id;
    #endif
#endif
#else
    typedef int window_id;
#endif

/*
 * info for a single text file line
 */
typedef struct linedata {
    ushort      mark:5;         // first mark on the line
    ushort      globmatch:1;    // global command matched this line
    ushort      nolinedata:1;   // no data associated with this line (WorkLine
                                // has the data instead)
    ushort      hidden:1;       // line is hidden (NYI)
    ushort      hilite:1;       // line need hiliting
    ushort      fill10:1,fill11:1,fill12:1,fill13:1,fill14:1,fill15:1,fill16:1;
} linedata;

typedef struct line {
    struct line *next,*prev;    // links for other lines
    short       len;            // length of line
    union {
        linedata        ld;
        short           word;
    } inf;
    char        data[1];        // actual string for line
} line;
#define LINE_SIZE sizeof( line )

/*
 * info for a file control block in a file
 */
struct file;
typedef struct fcb {
    struct fcb  *next,*prev;            // links fcbs in a file
    struct fcb  *thread_next,*thread_prev;// links all fcbs created
    struct file *f;                     // file associated with fcb
    line        *line_head,*line_tail;  // linked list of lines
    linenum     start_line,end_line;    // starting/ending line number
    short       byte_cnt;               // number of bytes in lines
    long        offset;                 // offset in swap file
    long        last_swap;              // time fcb was last swapped
    ushort      swapped:1;              // fcb is swapped
    ushort      in_memory:1;            // fcb is in memory
    ushort      on_display:1;           // lines in fcb are displayed
    ushort      non_swappable:1;        // fcb is not swappable
    ushort      dead:1;                 // fcb is dead (obsolete)
    ushort      was_on_display:1;       // fcb was on display (used to save
                                        // display state when switching files)
    ushort      in_extended_memory:1;   // fcb is in extended memory
    ushort      in_xms_memory:1;        // fcb is in XMS memory
    ushort      in_ems_memory:1;        // fcb is in EMS memory
    ushort      nullfcb:1;              // fcb is a special one that has no
                                        // lines associated with it
    ushort      globalmatch:1;          // a global command matched at least
                                        // one line in this fcb
    ushort      flag12:1;
    ushort      flag13:1,flag14:1,flag15:1,flag16:1;
    long        xmemaddr;               // address of fcb in extended memory
} fcb;
#define FCB_SIZE sizeof( fcb )

/*
 * info for an entire file
 */
typedef struct file {
    char        *name;                  // file name
    char        *home;                  // home directory of file
    fcb         *fcb_head,*fcb_tail;    // linked list of fcbs
    long        curr_pos;               // current offset in file on disk
    ushort      modified:1;             // file has been modified
    ushort      bytes_pending:1;        // there are still bytes to be read
                                        // off the disk for the file
    ushort      viewonly:1;             // file is view only
    ushort      read_only:1;            // file is read only
    ushort      check_readonly:1;       // file needs its read-only status
                                        // checked against the file on disk
    ushort      dup_count:4;            // number of duplicate views on the
                                        // file that have been opened
    ushort      been_autosaved:1;       // file has been autosaved
    ushort      need_autosave:1;        // file needs to be autosaved
    ushort      is_stdio:1;             // file is a "stdio" file (reads from
                                        // stdin and writes to stdout)
    ushort      needs_display:1;        // file needs to be displayed
    ushort      check_for_crlf:1;       // check file system when we write it out
    ushort      fill15:1,fill16:1;
    long        size;                   // size of file in bytes
    int         handle;                 // file handle (if entire file is not
                                        // read, will be an open file handle)
#ifdef __QNX__
    short       attr;
#endif
    char        as_name[TMP_NAME_LEN];  // name that file was auto-saved as
} file;
#define FILE_SIZE sizeof( file )

/*
 * mark setting
 */
typedef struct {
    linenum     lineno;         // line number that mark is on
    char        next;           // pointer to next mark on the same line
    ushort      col:12;         // column that mark is on
    ushort      inuse:1;        // mark is being used
    ushort      spare:3;
} mark;
#define MARK_SIZE sizeof( mark )

typedef struct i_mark {
    linenum     line;
    int         column;
} i_mark;

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
    i_mark      hi_start;
    i_mark      hi_end;
    char        highlight:1;
    char        line_based:1;
/*
 * Double ACK! Some times we need to treat a range differently depending
 * on whether an operator or a move is using it. This tells us if we should
 * include the last character or not (compare "d/foo" to "/foo"). Puke.
 */
    char        fix_range:1;
    char        spare:5;
} range;

typedef int (*insert_rtn)( void );
typedef int (*move_rtn)( range *, long count );
typedef int (*op_rtn)( range * );
typedef int (*misc_rtn)( long count );
typedef int (*old_rtn)( void );
typedef int (*alias_rtn)( void **, void ** );

typedef union command_rtn {
    insert_rtn  ins;
    move_rtn    move;
    op_rtn      op;
    misc_rtn    misc;
    old_rtn     old;
    alias_rtn   alias;
    void        *ptr;
} command_rtn;

typedef struct {
    char        type:3;                 // type of event (list in const.h)
    char        keep_selection:1;       // keep selection after this event?
    char        is_number:1;            // is event a digit? (repeat count)
    char        fix_range:1;            // see comment in doMove (editmain.c)
    char        is_op_alias:1;          // event is an aliased operator
    char        keep_selection_maybe:1; // keep selection if already selecting
    char        modifies:1;             // does this event modify the file?
    char        spare:7;
} event_bits;

typedef struct {
    command_rtn alt_rtn;
    insert_rtn  ins;
    command_rtn rtn;
    event_bits  b;
    event_bits  alt_b;
} event;

/* used to cast a base event (as in keys.h) to a char */
#define EVENT_CHAR( e ) ( (char)e )

/*
 * structure to define all the goop needed to display text
 */
typedef struct {
    short       foreground;
    short       background;
    short       font;
} type_style;

/*
 * window init info
 */
typedef struct {
    bool        has_border;
    short       border_color1,border_color2;
    type_style  text;
    type_style  hilight;
    short       x1,y1,x2,y2;
} window_info;

/*
 * undo info
 */
typedef struct {
    linenum     start,end;
} undo_insert;

typedef struct {
    fcb *fcb_head,*fcb_tail;
} undo_delete;

typedef struct {
    linenum     line,top;
    short       col,depth;
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
    ushort      selected:1;
    ushort      lines:1;
    ushort      dragging:1;
    ushort      empty:13;
    linenum     start_line;
    linenum     end_line;
    int         start_col;
    int         end_col;
    int         start_col_v;
} select_rgn;

/*
 * all info for a file being edited
 */
typedef struct info {
    struct info *next,*prev;
    file        *CurrentFile;
    linenum     CurrentLineNumber,TopOfPage;
    int         CurrentColumn,LeftColumn;
    undo_stack  *UndoStack, *UndoUndoStack;
    int         CurrentUndoItem,CurrentUndoUndoItem;
    window_id   CurrNumWindow;
    mark        *MarkList;
    bool        linenumflag;
    window_id   CurrentWindow;
    int         ColumnDesired;
    bool        CMode;
    bool        WriteCRLF;
    select_rgn  SelRgn;
    bool        IsColumnRegion;
    ushort      DuplicateID;
    void        *dc;
    int         dc_size;
    int         Language;
    bool     RealTabs;
    bool     ReadEntireFile;
    bool     ReadOnlyCheck;
    bool     EightBits;
    int      TabAmount;
    int      HardTab;
    int      ShiftWidth;
    int      AutoIndent;
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
        char    *data;
        fcb     *fcb_head;
    } first;
    fcb         *fcb_tail;
} savebuf;
#define SAVEBUF_SIZE sizeof( savebuf )

/*
 * color settings
 */
typedef struct {
    char        red,green,blue;
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
    int         *retevents;     // events that simulate pressing enter
    int         event;          // event that caused a return
    bool        show_lineno;    // show lines in top-right corner
    linenum     cln;            // current line to display
    window_id   eiw;            // alternate window to accept events in (like
                                // the options window after fgrep...)
    bool        is_menu:1;      // is a menu we are showing
    bool        spare:7;
} selectitem;

/*
 * SelectLineInFile data structure
 */
typedef struct {
    file        *f;             // file with data for lines
    char        **vals;         // values associated with each line
    int         valoff;         // offset to display values beside line data
    window_info *wi;            // info describing window to create
    linenum     sl;             // selected line
    char        *title;         // title of window
    int         (*checkres)(char *, char *, int * ); // check if selected
                                // change is valid
    int         *allow_rl;      // allow cursor right/left (for menu bar)
    char        **hilite;       // chars to highlight
    bool        show_lineno;    // show lines in top-right corner
    int         *retevents;     // events that simulate pressing enter
    int         event;          // event that caused a return
    linenum     cln;            // current line to display
    window_id   eiw;            // alternate window to accept events in (like
                                // the options window after fgrep...)
    bool        is_menu:1;      // select list is a menu
    bool        has_scroll_gadgets:1; // list has scroll gadgets
    bool        spare:6;
} selflinedata;

/*
 * special file (used to process bound data)
 */
typedef struct {
    int length;
    int maxlines,currline;
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
    #define PICK( a,b,c,d,e ) bool c;
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
