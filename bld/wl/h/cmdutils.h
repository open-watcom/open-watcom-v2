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
* Description:  Command line parser enumerations, structures, constants and
*               prototypes, including CmdTble function call prototypes
*
****************************************************************************/


typedef enum {
    MIDST,
    ENDOFLINE,
    ENDOFFILE,
    ENDOFCMD
} place;

typedef enum {
    NONBUFFERED,
    COMMANDLINE,
    INTERACTIVE,
    BUFFERED,
    ENVIRONMENT,
    SYSTEM
} method;

typedef enum {
    SEP_NO,
    SEP_COMMA,
    SEP_EQUALS,
    SEP_PERIOD,
    SEP_END,
    SEP_QUOTE,
    SEP_PAREN,
    SEP_SPACE,
    SEP_PERCENT,
    SEP_DOT_EXT,
    SEP_LCURLY,
    SEP_RCURLY
} sep_type;

typedef enum {
    ST_IS_ORDINAL,
    ST_NOT_ORDINAL,
    ST_INVALID_ORDINAL
} ord_state;

typedef enum {
    CF_TO_STDOUT            = CONSTU32( 0x00000001 ),
    CF_SET_SECTION          = CONSTU32( 0x00000002 ),   // used for LIB/FIXEDLIB directives
    CF_NO_DEF_LIBS          = CONSTU32( 0x00000004 ),
    CF_FILES_BEFORE_DBI     = CONSTU32( 0x00000008 ),
    CF_UNNAMED              = CONSTU32( 0x00000010 ),
    CF_AUTO_SEG_FLAG        = CONSTU32( 0x00000020 ),   // used in CMDOS2
    CF_MEMBER_ADDED         = CONSTU32( 0x00000040 ),
    CF_SEPARATE_SYM         = CONSTU32( 0x00000080 ),
    CF_AUTOSECTION          = CONSTU32( 0x00000100 ),
    CF_SECTION_THERE        = CONSTU32( 0x00000200 ),
    CF_HAVE_FILES           = CONSTU32( 0x00000400 ),
    CF_HAVE_REALBREAK       = CONSTU32( 0x00000800 ),
    CF_LANGUAGE_ENGLISH     = CONSTU32( 0x00000000 ),
    CF_LANGUAGE_JAPANESE    = CONSTU32( 0x00001000 ),
    CF_LANGUAGE_CHINESE     = CONSTU32( 0x00002000 ),
    CF_LANGUAGE_KOREAN      = CONSTU32( 0x00003000 ),
    CF_ANON_EXPORT          = CONSTU32( 0x00004000 ),
    CF_AFTER_INC            = CONSTU32( 0x00008000 ),   // option must be specd. after op inc
    CF_DOING_OPTLIB         = CONSTU32( 0x00010000 ),
    CF_NO_EXTENSION         = CONSTU32( 0x00020000 ),   // don't put an extension on exe name
    CF_SUBSET               = CONSTU32( 0x00040000 ),
} commandflag;

#define CF_LANGUAGE_MASK    (CF_LANGUAGE_ENGLISH | CF_LANGUAGE_JAPANESE | CF_LANGUAGE_CHINESE | CF_LANGUAGE_KOREAN)

typedef enum {
    TOK_NORMAL          = 0x00,
    TOK_INCLUDE_DOT     = 0x01,
    TOK_IS_FILENAME     = 0x02
} tokcontrol;

typedef enum {
    GENVER_ERROR        = 0,
    GENVER_MAJOR        = 0x01,
    GENVER_MINOR        = 0x02,
    GENVER_REVISION     = 0x04
} version_state;

typedef struct version_block {
    unsigned_32 major;
    unsigned_32 minor;
    unsigned_32 revision;
    const char  *message;
} version_block;

typedef struct {
    char        *buff;
    size_t      len;
    const char  *next;
    const char  *this;
    place       where;
    method      how;
    boolbit     thumb       : 1;
    boolbit     locked      : 1;
    boolbit     quoted      : 1;    /* set true if token parsed as a quoted string*/
    boolbit     skipToNext  : 1;    /* set true if we need to skip to next token without a separator */
    unsigned_16 line;
} tok;

typedef struct cmdfilelist {
    struct cmdfilelist *prev;
    struct cmdfilelist *next;
    f_handle            file;
    char                *symprefix;
    char                *name;
    tok                 token;
} cmdfilelist;

typedef struct {
    char                *keyword;
    bool                (*rtn)( void );
    exe_format          format;
    commandflag         flags;
} parse_entry;

/* handy globals */

extern file_defext      Extension;
extern file_list        **CurrFList;
extern tok              Token;
extern commandflag      CmdFlags;
extern char             *Name;
extern cmdfilelist      *CmdFile;

/* routines used in command parser */

extern bool             ProcArgList( bool (*)( void ), tokcontrol );
extern bool             ProcArgListEx( bool (*)( void ), tokcontrol ,cmdfilelist * );
extern bool             ProcOne( parse_entry *entry, sep_type req );
extern bool             ProcOneSubset( parse_entry *entry, sep_type req );
extern bool             ProcOneSuicide( parse_entry *entry, sep_type req );
extern bool             MatchOne( parse_entry *, sep_type, const char *, size_t );
extern ord_state        getatoi( unsigned_16 * );
extern ord_state        getatol( unsigned_32 * );
extern bool             HaveEquals( tokcontrol );
extern bool             GetLong( unsigned_32 * );
extern char             *tostring( void );
extern char             *totext( void );
extern bool             GetToken( sep_type, tokcontrol );
extern bool             GetTokenEx( sep_type, tokcontrol ,cmdfilelist *, bool * );
extern void             RestoreParser( void );
extern void             NewCommandSource( const char *, const char *, method );
extern void             SetCommandFile( f_handle, const char * );
extern void             EatWhite( void );
extern void             RestoreCmdLine( void );
extern bool             IsSystemBlock( void );
extern void             BurnUtils( void );
extern outfilelist      *NewOutFile( char * );
extern char             *GetFileName( char **, bool );
extern version_state    GetGenVersion( version_block *vb, version_state enq, bool novell_revision );
