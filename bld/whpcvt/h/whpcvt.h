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
* Description:  Global WHPCVT include file. Contains system independent
*               structures required across all help platforms.
*
****************************************************************************/


#ifndef WHPCVT_H
#define WHPCVT_H

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <malloc.h>
#include <setjmp.h>
#include <string.h>
#include <ctype.h>
#ifndef __WATCOMC__
  #include "clibext.h"
#endif

/* for testing */
//#define static

/**** Useful Macros and base types ****/
typedef unsigned char bool;

#ifndef TRUE
#define TRUE    1
#define FALSE   0
#endif

#define _min( a, b ) ( a > b ) b ? a
#define _max( a, b ) ( a > b ) a ? b
#define _new( ptr, size ) ( ptr = check_alloc( sizeof( *(ptr) ) * (size) ) )
#define _renew( ptr, size ) \
        ( ptr = check_realloc( ptr, sizeof( *(ptr) ) * (size) ) )
#define _free( x ) free( x )
#define _alloc( size ) check_alloc( size )
#define _realloc( ptr, size ) check_realloc( ptr, size )


/**** Typedefs for converter ****/

typedef struct browse_ctx {
    struct browse_ctx           *next;
    struct ctx_def              *ctx;
} browse_ctx;

typedef struct browse_def {
    struct browse_def           *next;
    char                        *browse_name;
    browse_ctx                  *ctx_list;
} browse_def;

typedef struct section_def {
    struct section_def          *next;
    bool                        blank_order;
    int                         order_num;
    char                        *section_text;
    int                         section_size;
} section_def;

enum {
    TITLE_FMT_DEFAULT,
    TITLE_FMT_LINE,
    TITLE_FMT_NOLINE
};

typedef struct keyword_def {
    struct keyword_def          *next;
    char                        *keyword;
    struct ctx_def              *defined_ctx;
    bool                        duplicate;
    int                         id;
    int                         ctx_list_size;
    int                         ctx_list_alloc;
    struct ctx_def              **ctx_list;
} keyword_def;

typedef struct keylist_def {
    struct keylist_def          *next;
    keyword_def                 *key;
} keylist_def;

typedef struct ctx_def {
    struct ctx_def              *next;
    char                        *ctx_name;
    char                        *title;
    browse_def                  *browse;
    int                         browse_num;
    section_def                 *section_list;
    int                         ctx_id;
    int                         title_fmt;
    struct ctx_def              *up_ctx;
    int                         head_level;
    bool                        empty;
    bool                        req_by_link;
    keylist_def                 *keylist;
} ctx_def;

typedef struct link_def {
    struct link_def             *next;
    char                        *link_name;
    int                         line_num;
} link_def;


/**** special characters *****/
#define CH_EXCLUDE_ON           195
#define CH_EXCLUDE_OFF          180
#define CH_CTX_DEF              247
#define CH_HLINK                127
#define CH_DFN                  234
#define CH_TOPIC                238
#define CH_LIST_START           210
#define CH_LIST_ITEM            211
#define CH_DLIST_DESC           185
#define CH_LIST_END             212
#define CH_OLIST_START          200
#define CH_OLIST_END            201
#define CH_DLIST_START          202
#define CH_DLIST_END            203
#define CH_DLIST_TERM           204
#define CH_CTX_KW               236
#define CH_SLIST_START          213
#define CH_SLIST_END            214
#define CH_TOPIC_LN             167
#define CH_TOPIC_NOLN           248
#define CH_PAR_RESET            158
#define CH_EXCLUDE_OFF_BLANK    159
#define CH_UP_TOPIC             160
#define CH_BMP                  176
#define CH_BOX_ON               219
#define CH_BOX_OFF              254
#define CH_FONTSTYLE_START      239
#define CH_FONTSTYLE_END        240
#define CH_FONTTYPE             157
#define CH_TABXMP               170
#define CH_FLINK                232


/**** a few InfoBench specific chars ****/
//(defined here 'cause WHPCVT.C needs to know about them )
#define IB_HLINK_L_CHAR         224
#define IB_HLINK_R_CHAR         224
#define IB_BRACE_L_CHAR         '<'
#define IB_BRACE_R_CHAR         '>'

/**** various file extensions ****/
#define EXT_INPUT_FILE      ".whp"
#define EXT_OUTRTF_FILE     ".rtf"
#define EXT_OUTIPF_FILE     ".ipf"
#define EXT_OUTHTML_FILE    ".htm"
#ifdef __DOS__
  #define EXT_OUTWIKI_FILE  ".wik"
#else
  #define EXT_OUTWIKI_FILE  ".wiki"
#endif
#define EXT_OUTIB_FILE      ".ib"
#define EXT_IDX_FILE        ".idx"
#define EXT_KW_FILE         ".kw"
#define EXT_BLIST_FILE      ".blt"
#define EXT_DEF_FILE        ".h"
#define EXT_HDEF_FILE       ".hh"
#define EXT_HLP_FILE        ".hlp"
#define EXT_TBL_FILE        ".tbl"

/**** Globals ****/
#ifndef WHPCVT_GBL
#define WHPCVT_GBL extern
#endif
WHPCVT_GBL      FILE            *In_file;
WHPCVT_GBL      FILE            *Out_file;
WHPCVT_GBL      FILE            *Idx_file;
WHPCVT_GBL      FILE            *KW_file;
WHPCVT_GBL      FILE            *Blist_file;
WHPCVT_GBL      FILE            *Def_file;
WHPCVT_GBL      FILE            *Hdef_file;
WHPCVT_GBL      FILE            *Contents_file;

WHPCVT_GBL      int             Line_num;
WHPCVT_GBL      char            *Line_buf;

WHPCVT_GBL      ctx_def         *Ctx_list;
WHPCVT_GBL      ctx_def         *Curr_ctx;
WHPCVT_GBL      browse_def      *Browse_list;
WHPCVT_GBL      link_def        *Link_list;

WHPCVT_GBL      int             Start_inc_ul;
WHPCVT_GBL      int             Start_inc_ol;
WHPCVT_GBL      int             Start_inc_sl;
WHPCVT_GBL      int             Start_inc_dl;

WHPCVT_GBL      bool            Do_up;
WHPCVT_GBL      bool            Do_kw_button;
WHPCVT_GBL      bool            Keep_titles;
WHPCVT_GBL      bool            Break_link;
WHPCVT_GBL      bool            Remove_empty;
WHPCVT_GBL      bool            Real_ipf_font;
WHPCVT_GBL      bool            Keep_link_topics;
WHPCVT_GBL      bool            Exclude_special;

WHPCVT_GBL      int             Right_Margin;
WHPCVT_GBL      int             Text_Indent;
WHPCVT_GBL      int             Hyper_Brace_L;
WHPCVT_GBL      int             Hyper_Brace_R;
WHPCVT_GBL      bool            Do_browse;
WHPCVT_GBL      bool            Do_tc_button;
WHPCVT_GBL      bool            Do_idx_button;
WHPCVT_GBL      char            Header_File[];
WHPCVT_GBL      char            Footer_File[];
WHPCVT_GBL      bool            Tab_xmp;
WHPCVT_GBL      char            Tab_xmp_char;

WHPCVT_GBL      char            Help_fname[];

WHPCVT_GBL      char            Fonttype_roman[];
WHPCVT_GBL      char            Fonttype_symbol[];
WHPCVT_GBL      char            Fonttype_helv[];
WHPCVT_GBL      char            Fonttype_courier[];

WHPCVT_GBL      char            *Ipf_title;

WHPCVT_GBL      char            *IB_def_topic;
WHPCVT_GBL      char            *IB_help_desc;


/**** Error constants ****/
enum {
    ERR_NO_TOPIC,
    ERR_CTX_EXISTS,
    ERR_NO_TITLE,
    ERR_NO_MEMORY,
    ERR_BAD_LINK_DFN,
    ERR_MAX_LISTS,
    ERR_UNDEF_LINK,
    ERR_EMPTY_LINK,
    ERR_BAD_ARGS
};


/**** misc. stuff ****/
#define INDENT_INC              360     // for RTF only
#define NUM_TAB_STOPS           15      // for RTF only

/**** externed functions ****/
extern void error_str(
    char                *err_str
);
extern void error(
    int                 err,
    bool                line_num
);

extern void *check_alloc(
    size_t              size
);

extern void *check_realloc(
    void                *ptr,
    size_t              size
);

extern void whp_fprintf(
    FILE                *file,
    char                *fmt,
    ...
);

extern void whp_fwrite(
    void                *buf,
    int                 el_size,
    int                 num_el,
    FILE                *f
);

extern bool read_line(
    void
);

extern int trans_add_char(
    int                 ch,
    section_def         *section,
    int                 *alloc_size
);

extern int trans_add_str(
    char                *str,
    section_def         *section,
    int                 *alloc_size
);

extern int trans_add_nobreak_str(
    char                *str,
    section_def         *section,
    int                 *alloc_size
);

extern bool find_keyword(
    ctx_def             *ctx,
    char                *keyword
);

extern keyword_def *find_keyword_all(
    char                *keyword
);

extern void add_ctx_keyword(
    ctx_def             *ctx,
    char                *keyword
);

extern char *skip_blank(
    char                *ptr
);

extern int rtf_trans_line(
    section_def         *section,
    int                 alloc_size
);

extern void rtf_output_file(
    void
);

extern int ipf_trans_line(
    section_def         *section,
    int                 alloc_size
);

extern void ipf_output_file(
    void
);

extern int ib_trans_line(
    section_def         *section,
    int                 alloc_size
);

extern void ib_output_file(
    void
);

extern int html_trans_line(
    section_def         *section,
    int                 alloc_size
);

extern void html_output_file(
    void
);

extern int wiki_trans_line(
    section_def         *section,
    int                 alloc_size
);

extern void wiki_output_file(
    void
);

extern void add_link(
    char                *link_name
);

extern ctx_def *find_ctx(
    char                *ctx_name
);

extern char *whole_keyword_line(
    char                *ptr
);

extern bool is_special_topic(
    ctx_def             *ctx,
    bool                dump_popup
);

extern void rtf_topic_init( void );
extern void ipf_topic_init( void );
extern void ib_topic_init( void );
extern void html_topic_init( void );
extern void wiki_topic_init( void );

#endif
