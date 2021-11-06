/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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


#ifndef _SOURCE_INCLUDED
#define _SOURCE_INCLUDED

/*
 * constants
 */
#define MAX_SRC_LINE            512
#define MAX_SRC_LABELS          512
#define MAX_SRC_FILES           9
#define MAX_SRC_CLVARS          9

typedef enum {
    EXPR_EQ,
    EXPR_PLUSEQ,
    EXPR_MINUSEQ,
    EXPR_TIMESEQ,
    EXPR_DIVIDEEQ
} expr_oper;

enum {
    #define PICK(a,b) b,
    #include "srckeys.h"
    #undef PICK
    SRC_T_NULL
};

#define IS_LOCALVAR(n)      (n[0] < 'A' || n[0] > 'Z')

#define GLOBVAR_COLUMN          "C"
#define GLOBVAR_ROW             "R"
#define GLOBVAR_FILEHOME        "H"
#define GLOBVAR_FILEFULLNAME    "F"
#define GLOBVAR_FILEDRIVE       "D"
#define GLOBVAR_FILEPATH        "P"
#define GLOBVAR_FILENAME        "N"
#define GLOBVAR_FILEEXT         "E"
#define GLOBVAR_FILEMODIFIED    "M"
#define GLOBVAR_USER_FILEDRIVE  "D1"
#define GLOBVAR_USER_FILEPATH   "P1"

#define GLOBVAR_COMMAND_BUFFER  "Com"
#define GLOBVAR_INIPATH         "IniPath"
#define GLOBVAR_LINELEN         "LineLen"
#define GLOBVAR_OS              "OS"
#define GLOBVAR_OS386           "OS386"
#define GLOBVAR_OSX64           "OSX64"
#define GLOBVAR_WINDOW_HEIGHT   "SH"
#define GLOBVAR_SYSTEM_RETCODE  "Sysrc"
#define GLOBVAR_WINDOW_WIDTH    "SW"

#define SRCHOOK_DEFS \
    pick( SRC_HOOK_WRITE,           0x0001, "Wrhook"      ) \
    pick( SRC_HOOK_READ,            0x0002, "Rdhook"      ) \
    pick( SRC_HOOK_BUFFIN,          0x0004, "Buffinhook"  ) \
    pick( SRC_HOOK_BUFFOUT,         0x0008, "Buffouthook" ) \
    pick( SRC_HOOK_COMMAND,         0x0010, "Cmdhook"     ) \
    pick( SRC_HOOK_MODIFIED,        0x0020, "Modhook"     ) \
    pick( SRC_HOOK_MENU,            0x0040, "Menuhook"    ) \
    pick( SRC_HOOK_MOUSE_LINESEL,   0x0080, "MLselhook"   ) \
    pick( SRC_HOOK_MOUSE_CHARSEL,   0x0100, "MCselhook"   ) \
    pick( SRC_HOOK_DDE,             0x0200, "DDEhook"     )

typedef enum hooktype {
    SRC_HOOK_NONE           = 0,
    #define pick(e,m,t)     e = m,
    SRCHOOK_DEFS
    #undef pick
} hooktype;

typedef char        *label;
typedef unsigned    srcline;

typedef enum {
    CS_IF,          /* an if/elseif block */
    CS_ELSE,        /* an else block */
    CS_LOOP,        /* a looping structure */
    CS_EOS          /* end of stack */
} cstype;

typedef enum {
    STR_T_SUBSTR,
    STR_T_STRLEN,
    STR_T_STRCHR
} strtype;

/*
 * structures
 */
/* control stack */
typedef struct cs_entry {
    struct cs_entry     *next;
    label               top;
    label               alt;
    label               end;
    srcline             sline;
    cstype              type;
} cs_entry;

struct sfile;

typedef struct labels {
    char            **name;
    struct sfile    **pos;
    unsigned short  cnt;
} labels;

typedef struct vars {
    struct vars *next, *prev;
    char        *value;
    size_t      len;
    char        name[1];
} vars;

typedef struct vars_list {
    vars    *head, *tail;
} vars_list;

typedef enum {
    SRCFILE_NONE = 0,
    SRCFILE_FILE,
    SRCFILE_BUFF
} ftype;

typedef struct files {
    union {
        FILE *f[MAX_SRC_FILES];
        struct {
            info        *cinfo;
            linenum     line;
        } buffer[MAX_SRC_FILES];
    } u;
    ftype       ft[MAX_SRC_FILES];
} files;

typedef enum branch_cond {
    COND_FALSE,
    COND_TRUE,
    COND_JMP
} branch_cond;

typedef struct sfile {
    struct sfile    *next;
    struct sfile    *prev;
    char            *arg1;
    char            *arg2;
    int             token;
    char            *data;
    srcline         sline;
    branch_cond     branchcond;
    union {
        branch_cond branchres;
        expr_oper   oper;
    } u;
    bool            hasvar;
} sfile;

typedef struct resident {
    struct resident *next;
    char            *fn;
    sfile           *sf;
    labels          lab;
    bool            scriptcomp;
} resident;

/*
 * external defs (from srcdata.c)
 */
extern const char _NEAR   StrTokens[];
extern const char _NEAR   SourceTokens[];
extern long         CurrentSrcLabel;
extern srcline      CurrentSrcLine;
extern int          CurrentSrcToken;

/*
 * function prototypes
 */

/* srcassgn.c */
extern vi_rc    SrcAssign( const char *data, vars_list *vl );

/* srccs.c */
extern void     CSInit( void );
extern vi_rc    CSFini( void );
extern void     CSIf( const char *data );
extern void     CSElseIf( const char *data );
extern void     CSElse( void );
extern void     CSEndif( void );
extern void     CSWhile( const char *data );
extern void     CSLoop( void );
extern void     CSEndLoop( void );
extern void     CSUntil( const char *data );
extern void     CSBreak( void );
extern void     CSContinue( void );
extern void     CSQuif( const char *data );

/* srcexpnd.c */
extern char     *Expand( char *, const char *data, vars_list *vl );

/* srcexpr.c */
extern vi_rc    SrcExpr( sfile *, vars_list *vl );

/* srcfile.c */
extern vi_rc    SrcOpen( sfile *, files *, const char *data, vars_list *vl );
extern vi_rc    SrcRead( sfile *, files *, const char *data, vars_list *vl );
extern vi_rc    SrcWrite( sfile *, files *, const char *data, vars_list *vl );
extern vi_rc    SrcClose( sfile *, files *, const char *data, vars_list *vl );

/* srcgen.c */
extern vi_rc    PreProcess( const char *, sfile **, labels * );
extern void     GenJmpIf( branch_cond, const char *lbl );
extern void     GenJmp( const char *lbl );
extern void     GenLabel( char *lbl );
extern void     GenTestCond( const char *data );
extern label    NewLabel( void );
extern void     AbortGen( vi_rc );

/* srcgoto.c */
extern vi_rc    SrcGoTo( sfile **, const char *lbl, labels * );

/* srclabel.c */
extern vi_rc    AddLabel( sfile *, labels *, const char *lbl );
extern int      FindLabel( labels *labs, const char *lbl );

/* srchook.c */
extern vi_rc    SourceHook( hooktype, vi_rc );
extern vars     *GetHookVar( hooktype num );
extern vi_rc    SourceHookWithData( hooktype num, char *data );
extern void     HookScriptCheck( void );
extern vi_rc    InvokeColSelHook( int sc, int ec );
extern vi_rc    InvokeLineSelHook( linenum s, linenum e );
extern vi_rc    InvokeMenuHook( int menunum, int line );

/* srcif.c */
extern vi_rc    SrcIf( sfile **, vars_list *vl );

/* srcinp.c */
extern vi_rc    SrcInput( const char *data, vars_list *vl );
extern void     SrcGet( const char *data, vars_list *vl );

/* srcnextw.c */
extern vi_rc    SrcNextWord( const char *data, vars_list * );

/* srcvar.c */
extern void     GlobVarAddStr( const char *name, const char *value );
extern void     GlobVarAddLong( const char *name, long value );
extern void     GlobVarAddRowAndCol( void );
extern void     GlobVarFini( void );
extern vars     *GlobVarFind( const char *name );
extern vars     *VarFind( const char *name, vars_list *vl );
extern void     VarAddStr( const char *name, const char *value, vars_list *vl );
extern void     VarAddLong( const char *name, long value, vars_list *vl );
extern void     VarListDelete( vars_list *vl );
extern bool     VarName( char *name, const char *data, vars_list *vl );
extern bool     ReadVarName( const char **data, char *name, vars_list *vl );

extern bool     RunWindowsCommand( const char *data, vi_rc *rc, vars_list *vl );

#endif
