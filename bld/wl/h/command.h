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
* Description:  Command line parser enumerations, structures, constants and
*               prototypes, including CmdTble function call prototypes
*
****************************************************************************/


typedef enum {
    MIDST,
    ENDOFLINE,
    ENDOFFILE,
    ENDOFCMD
}                       place;

typedef enum {
    NONBUFFERED,
    COMMANDLINE,
    INTERACTIVE,
    BUFFERED,
    ENVIRONMENT,
    SYSTEM
}                       method;

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
}                       sep_type;

typedef enum {
    OK,
    REJECT
}                       status;

typedef enum {
    ST_IS_ORDINAL,
    ST_NOT_ORDINAL,
    ST_INVALID_ORDINAL
} ord_state;

typedef struct tok {
    char        *buff;
    size_t      len;
    char        *next;
    char        *this;
    unsigned_16 line;
    unsigned_16 where : 2;
    unsigned_16 how : 3;
    unsigned_16 thumb : 1;
    unsigned_16 locked : 1;
    unsigned_16 quoted : 1;     /* set true if token parsed as a quoted string*/
    unsigned_16 skipToNext : 1; /* set true if we need to skip to next token without a separator */
} tok;

typedef enum commandflag {
    CF_TO_STDOUT        = 0x00000001,
    CF_SET_SECTION      = 0x00000002,   // used for LIB/FIXEDLIB directives
    CF_NO_DEF_LIBS      = 0x00000004,
    CF_FILES_BEFORE_DBI = 0x00000008,
    CF_UNNAMED          = 0x00000010,
    CF_AUTO_SEG_FLAG    = 0x00000020,   // used in CMDOS2
    CF_MEMBER_ADDED     = 0x00000040,
    CF_SEPARATE_SYM     = 0x00000080,
    CF_AUTOSECTION      = 0x00000100,
    CF_SECTION_THERE    = 0x00000200,
    CF_HAVE_FILES       = 0x00000400,
    CF_HAVE_REALBREAK   = 0x00000800,
    CF_LANGUAGE_MASK    = 0x00003000,
    CF_LANGUAGE_ENGLISH = 0x00000000,
    CF_LANGUAGE_JAPANESE= 0x00001000,
    CF_LANGUAGE_CHINESE = 0x00002000,
    CF_LANGUAGE_KOREAN  = 0x00003000,
    CF_ANON_EXPORT      = 0x00004000,
    CF_AFTER_INC        = 0x00008000,  // option must be specd. after op inc
    CF_DOING_OPTLIB     = 0x00010000,
    CF_NO_EXTENSION     = 0x00020000    // don't put an extension on exe name
} commandflag;

typedef struct cmdfilelist {
    struct cmdfilelist *prev;
    struct cmdfilelist *next;
    f_handle            file;
    char                *symprefix;
    char                *name;
    tok                 token;
} cmdfilelist;

typedef struct parse_entry {
    char                *keyword;
    bool                (*rtn)( void );
    enum exe_format     format;
    commandflag         flags;
} parse_entry;

typedef struct sysblock {
    struct sysblock     *next;
    char                *name;
    char                commands[1];
} sysblock;

typedef enum {
    TOK_NORMAL          = 0x00,
    TOK_INCLUDE_DOT     = 0x01,
    TOK_IS_FILENAME     = 0x02
} tokcontrol;

/* command parse tables */

extern parse_entry PosDbgMods[];
extern parse_entry DbgMods[];
extern parse_entry SysBeginOptions[];
extern parse_entry SysDeleteOptions[];
extern parse_entry SysEndOptions[];
extern parse_entry SortOptions[];
extern parse_entry Directives[];
extern parse_entry MainOptions[];
extern parse_entry SysDirectives[];
extern parse_entry Models[];
extern parse_entry Languages[];
extern parse_entry EndLinkOpt[];
extern parse_entry RunOptions[];
extern parse_entry Strategies[];
extern parse_entry TransTypes[];
extern parse_entry QNXSegModel[];
extern parse_entry QNXSegDesc[];
extern parse_entry QNXFormats[];
extern parse_entry PharModels[];
extern parse_entry NovModels[];
extern parse_entry NovDBIOptions[];
extern parse_entry Sections[];
extern parse_entry SectOptions[];
extern parse_entry DosOptions[];
extern parse_entry SubFormats[];
extern parse_entry OS2FormatKeywords[];
extern parse_entry WindowsFormatKeywords[];
extern parse_entry NTFormatKeywords[];
extern parse_entry VXDFormatKeywords[];
extern parse_entry Init_Keywords[];
extern parse_entry Term_Keywords[];
extern parse_entry Exp_Keywords[];
extern parse_entry SegDesc[];
extern parse_entry SegTypeDesc[];
extern parse_entry SegModel[];
extern parse_entry CommitKeywords[];
extern parse_entry ELFFormatKeywords[];
extern parse_entry ZdosOptions[];
extern parse_entry RdosOptions[];
extern parse_entry RawOptions[];
extern parse_entry OrderOpts[];
extern parse_entry OrderClassOpts[];
extern parse_entry OrderSegOpts[];
extern parse_entry OutputOpts[];

/* handy globals */

extern file_defext  Extension;
extern file_list    **CurrFList;
extern tok          Token;
extern commandflag  CmdFlags;
extern char         *Name;
extern sysblock     *SysBlocks;
extern sysblock     *LinkCommands;
extern cmdfilelist *CmdFile;

/* routines used in command parser */

extern bool             ProcArgList( bool (*)( void ), tokcontrol );
extern bool             ProcArgListEx( bool (*)( void ), tokcontrol ,cmdfilelist * );
extern bool             ProcOne( parse_entry *, sep_type, bool );
extern bool             MatchOne( parse_entry *, sep_type, char *, unsigned );
extern ord_state        getatoi( unsigned_16 * );
extern ord_state        getatol( unsigned_32 * );
extern bool             HaveEquals( tokcontrol );
extern bool             GetLong( unsigned_32 * );
extern char             *tostring( void );
extern char             *totext( void );
extern bool             GetToken( sep_type, tokcontrol );
extern bool             GetTokenEx( sep_type, tokcontrol ,cmdfilelist *, bool * );
extern void             RestoreParser( void );
extern void             NewCommandSource( char *, char *, method );
extern void             SetCommandFile( f_handle, char * );
extern void             EatWhite( void );
extern char             *FileName( const char *, size_t, file_defext, bool );
extern void             RestoreCmdLine( void );
extern bool             IsSystemBlock( void );
extern void             BurnUtils( void );
extern outfilelist      *NewOutFile( char * );
extern section          *NewSection( void );
extern char             *GetFileName( char **, bool );
