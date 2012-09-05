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
    SRC_T_EXPR,
    SRC_T_LABEL,
    SRC_T_IF,
    SRC_T_QUIF,
    SRC_T_ELSEIF,
    SRC_T_ELSE,
    SRC_T_ENDIF,
    SRC_T_LOOP,
    SRC_T_ENDLOOP,
    SRC_T_ENDWHILE,
    SRC_T_WHILE,
    SRC_T_UNTIL,
    SRC_T_BREAK,
    SRC_T_CONTINUE,
    SRC_T_GOTO,
    SRC_T_ASSIGN,
    SRC_T_RETURN,
    SRC_T_OPEN,
    SRC_T_READ,
    SRC_T_WRITE,
    SRC_T_CLOSE,
    SRC_T_INPUT,
    SRC_T_ATOMIC,
    SRC_T_GET,
    SRC_T_VBJ__,
    SRC_T_NEXTWORD,
    SRC_T_NULL
};

#define SRC_HOOK_WRITE          0x0001
#define SRC_HOOK_READ           0x0002
#define SRC_HOOK_BUFFIN         0x0004
#define SRC_HOOK_BUFFOUT        0x0008
#define SRC_HOOK_COMMAND        0x0010
#define SRC_HOOK_MODIFIED       0x0020
#define SRC_HOOK_MENU           0x0040
#define SRC_HOOK_MOUSE_LINESEL  0x0080
#define SRC_HOOK_MOUSE_CHARSEL  0x0100
#define SRC_HOOK_DDE            0x0200

typedef int     hooktype;
typedef char    *label;

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
    cstype              type;
    short               srcline;
} cs_entry;

struct sfile;

typedef struct labels {
    short           cnt;
    char            **name;
    struct sfile    **pos;
} labels;

typedef struct vars {
    struct vars *next, *prev;
    short       len;
    char        *value;
    char        name[1];
} vars;

typedef struct vlist {
    vars    *head, *tail;
} vlist;

typedef enum {
    SRCFILE_NONE = 0,
    SRCFILE_FILE,
    SRCFILE_BUFF
} ftype;

typedef struct files {
    ftype       ft[MAX_SRC_FILES];
    union {
        FILE *f[MAX_SRC_FILES];
        struct {
            info        *cinfo;
            linenum     line;
        } buffer[MAX_SRC_FILES];
    } u;
} files;

typedef enum branch_cond {
    COND_FALSE,
    COND_TRUE,
    COND_JMP
} branch_cond;

typedef struct sfile {
    struct sfile    *next, *prev;
    char            *arg1, *arg2;
    int             token;
    branch_cond     branchcond;
    union {
        branch_cond branchres;
        expr_oper   oper;
    } u;
    char            hasvar;
    int             line;
    char            *data;
} sfile;

typedef struct resident {
    struct resident *next;
    bool            scriptcomp;
    char            *fn;
    sfile           *sf;
    labels          lab;
} resident;

/*
 * external defs (from srcdata.c)
 */
extern char _NEAR   StrTokens[];
extern char _NEAR   SourceTokens[];
extern char         *ErrorTokens;
extern int          *ErrorValues;
extern vars         *VarHead, *VarTail;
extern long         CurrentSrcLabel;
extern int          CurrentSrcLine, CurrentSrcToken;
extern char         *CurrentSrcData;

/*
 * function prototypes
 */
 
/* srcassgn.c */
extern vi_rc    SrcAssign( char *, vlist * );

/* srccs.c */
extern void     CSInit( void );
extern vi_rc    CSFini( void );
extern void     CSIf( void );
extern void     CSElseIf( void );
extern void     CSElse( void );
extern void     CSEndif( void );
extern void     CSWhile( void );
extern void     CSLoop( void );
extern void     CSEndLoop( void );
extern void     CSUntil( void );
extern void     CSBreak( void );
extern void     CSContinue( void );
extern void     CSQuif( void );

/* srcexpnd.c */
extern void     Expand( char *, vlist * );

/* srcexpr.c */
extern vi_rc    SrcExpr( sfile *, vlist * );

/* srcfile.c */
extern vi_rc    SrcOpen( sfile *, vlist *, files *, char * );
extern vi_rc    SrcRead( sfile *, files *, char *, vlist * );
extern vi_rc    SrcWrite( sfile *, files *, char *, vlist * );
extern vi_rc    SrcClose( sfile *, vlist *, files *, char * );

/* srcgen.c */
extern vi_rc    PreProcess( const char *, sfile **, labels * );
extern void     GenJmpIf( branch_cond, label );
extern void     GenJmp( label );
extern void     GenLabel( label );
extern void     GenTestCond( void );
extern label    NewLabel( void );
extern void     AbortGen( vi_rc );

/* srcgoto.c */
extern vi_rc    SrcGoTo( sfile **, label, labels * );

/* srclabel.c */
extern vi_rc    AddLabel( sfile *, labels *, label );
extern int      FindLabel( labels *labs, label lbl );

/* srchook.c */
extern vi_rc    SourceHook( hooktype, vi_rc );
extern vars     *GetHookVar( hooktype num );
extern vi_rc    SourceHookData( hooktype num, char *data );
extern void     HookScriptCheck( void );
extern vi_rc    InvokeColSelHook( int sc, int ec );
extern vi_rc    InvokeLineSelHook( linenum s, linenum e );
extern vi_rc    InvokeMenuHook( int menunum, int line );

/* srcif.c */
extern vi_rc    SrcIf( sfile **, vlist * );
extern vi_rc    GetErrorTokenValue( int *, char * );
extern vi_rc    ReadErrorTokens( void );

/* srcinp.c */
extern vi_rc    SrcInput( char *, vlist * );
extern void     SrcGet( char *, vlist * );

/* srcnextw.c */
extern vi_rc    SrcNextWord( char *, vlist * );

/* srcvar.c */
extern void     VarAddGlobalStr( char *, char * );
extern void     VarAddRandC( void );
extern void     VarAddGlobalLong( char *, long );
extern void     VarAddStr( char *, char *, vlist * );
extern void     VarListDelete( vlist * );
extern bool     VarName( char *, vlist * );
extern vars     *VarFind( char *, vlist * );

extern bool     RunWindowsCommand( char *, vi_rc *, vlist * );

#endif
