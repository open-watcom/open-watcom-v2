/****************************************************************************
*
*                            Open Watcom Project
*
*  Copyright (c) 2004-2010 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  wgml path and separator char definitions, Environment variables
*               for includes, ...
*
****************************************************************************/


#ifndef GTYPE_H_INCLUDED
#define GTYPE_H_INCLUDED

#if defined(__UNIX__)
    #define FNAMECMPSTR      strcmp      /* for case sensitive file systems */
#else
    #define FNAMECMPSTR      stricmp     /* for case insensitive file systems */
#endif

//================= Some global defines ========================
#define MAX_NESTING         32          // max nesting of option files
#define MAX_PASSES          10          // max no of document passes
#define MAX_INC_DEPTH       255         // max include level depth
#define BUF_SIZE            512         // default buffersize for filecb e.a.
#define MAX_FILE_ATTR       15          // max size for fileattr (T:xxxx)
#define SCR_KW_LENGTH       2           // script control word length
#define FUN_NAME_LENGTH     11          // &'function name max length
#define MAX_PAREN           50          // max parenthesis nesting in &'functions

#define TAG_NAME_LENGTH     15          // :tag name length
#define ATT_NAME_LENGTH     9           // :tag attr name length
#define VAL_LENGTH          10          // max length for attribute value
                                        // longer strings will be allocated
#define NUM2STR_LENGTH      12          // int to string conversion buffer length sNNNNNNNNNN+'\0'

#define ID_LEN              15          // length of refids wgml 4 gives a warning
                                        // for lengths > 7 but processes it
                                        // we try with truncating at 15

#define SYM_NAME_LENGTH     10          // symbol name length
#define MAC_NAME_LENGTH     8           // macro name length
#define LABEL_NAME_LENGTH   8           // label name length

#define MAX_MAC_PARMS       32          // maximum macro parm count
                                        // arbitrary value, not found in docu!!!
#define MAC_STAR_NAME       "_"         // local variable name for &*

#define MAX_FUN_PARMS       7           // max parmcount found in documentation

#define MAX_IF_LEVEL        10          // maximum nested .if s

#define MAX_CPI             100         // not documented, assumed
#define MAX_LPI             100         // not documented, assumed
#define MAX_COL             9           // .CD limits number of columns to 9 (1..9)

#define TAB_COUNT           16          // used with tab_list

#define TEXT_CHARS_DEF      16          // default text_chars text length if allocated or reallocated

/* default filename extensions */
#define DEF_EXT             "def"
#define ERR_EXT             "err"
#define GML_EXT             "gml"
#define LAY_EXT             "lay"
#define OPT_EXT             "opt"
#define COP_EXT             "cop"
#define PCD_EXT             "pcd"
#define FON_EXT             "fon"

#define CONT_CHAR_DEFAULT   0x03        // cont character
#define GML_CHAR_DEFAULT    ':'         // start of GML tag
#define SCR_CHAR_DEFAULT    '.'         // start of Script keyword
#define CW_SEP_CHAR_DEFAULT ';'         // script control line seperator

#define CPI10               10          // 10 as a CPI value
#define FONT0               0           // 0 as a font number
#define LPI6                6           // 6 as an LPI value

/* string start / end characters Possible incomplete list*/
#define d_q                 '\"'        // change also is_quote_char()
#define s_q                 '\''        // in gargutil.c
#define cent                '\x9b'      // if list is extended
#define excl                '!'
#define not_c               '^'
#define slash               '/'
#define vbar1               '|'
#define vbar2               '\xdd'
#define l_q                 '`'

#define my_tolower( p )     tolower( (unsigned char)(p) )
#define my_toupper( p )     toupper( (unsigned char)(p) )

#define my_isalpha( p )     isalpha( (unsigned char)(p) )
#define my_isalnum( p )     isalnum( (unsigned char)(p) )
#define my_isdigit( p )     isdigit( (unsigned char)(p) )
#define my_isxdigit( p )    isxdigit( (unsigned char)(p) )
#define my_isspace( p )     isspace( (unsigned char)(p) )

#define SkipSpaces( p )     while( *(p) == ' ' ) (p)++
#define SkipNonSpaces( p )  while( *(p) != '\0' && *(p) != ' ' ) (p)++
#define SkipDot( p )        if( *(p) == '.' ) (p)++
#define SkipSpacesTabs( p ) while( is_space_tab_char( *(p) ) ) (p)++

typedef unsigned    line_number;
typedef uint8_t     text_space;
typedef uint32_t    units_space;

/***************************************************************************/
/*  Space units Horiz + Vert              to be redesigned      TBD        */
/***************************************************************************/

typedef enum space_units {
    SU_undefined   = 0,                 // don't care = value zero
    SU_chars_lines = 16,                // undimensioned value
    SU_chars       = 1,                 // chars horizontal
    SU_lines       = 1,                 // lines vertical
    SU_cicero,                          // C   Cicero
    SU_cm,                              // cm  centimeter
    SU_dv,                              // dv  device units
    SU_ems,                             // M   Ems
    SU_inch,                            // i   inch
    SU_mm,                              // mm  millimeter
    SU_pica,                            // p   pica

    SU_lay_left    = 100,    // special values used for layout definition
    SU_lay_centre,           // ... :BANREGION indent, hoffset and width attr
    SU_lay_right,
    SU_lay_extend            // additions before SU_lay_left please
} space_units;

#define MAX_SU_CHAR     12            // length of space units in char format

typedef struct {
    char        su_txt[MAX_SU_CHAR];    // space unit as entered in chars
    int32_t     su_whole;               // integer part
    int32_t     su_dec;                 // decimal part (if any)
    int32_t     su_inch;                // value in inch with 4 decimals
    int32_t     su_mm;                  // value in mm with 4 decimals
    bool        su_relative;            // + - sign found
    space_units su_u;                   // unit
} su;

/***************************************************************************/
/*  Symbolic variables related                                             */
/***************************************************************************/

typedef enum {
    min_subscript = -1000000,           // smallest valid subscript
    max_subscript =  1000000,           // largest  valid subscript
    // the rest must be outside of range min_subscript,max_subscript
    no_subscript  = 0x11223344,         // value if not subscripted
    neg_subscript = 0x22334454,         // negative subscripts (*-)
    all_subscript = 0x22334455,         // all subscripts (*)
    pos_subscript = 0x22334456          // positive subscripts (*+)
} sub_index;

typedef enum {
    local_var   = 0x0001,
    subscripted = 0x0002,
    auto_inc    = 0x0004,
    predefined  = 0x0008,               // predefined at startup
    ro          = 0x0010,               // value not changable
    no_free     = 0x0020,               // symbol is defined at compile time
//    type_long   = 0x0040,
//    type_str    = 0x0080,
//    type_char   = 0x0100,
    access_fun  = 0x0200,               // get value via function call
    is_AMP      = 0x0400,               // identify predefined symbol AMP
    deleted     = 0x1000
} symbol_flags;

/***************************************************************************/
/*  entry for a (subscripted) symbolic variable                            */
/***************************************************************************/

typedef struct symsub {
    struct symsub   *   next;           // next subscript entry
    struct symvar   *   base;           // the base symvar
    sub_index           subscript;      // the subscript
    char            *   value;          // the value ptr
} symsub;

/***************************************************************************/
/*  Symbolic variable base entry                                           */
/***************************************************************************/

typedef struct symvar {
    struct symvar   *   next;           // next base entry
    char                name[SYM_NAME_LENGTH + 2];
    int                 last_auto_inc;  // last autoincremented subscript value
    int                 subscript_used; // count of used subscripts
    symsub          *   subscripts;     // subscript entries
    symsub          *   sub_0;          // special subscript 0 entry
    void                (*varfunc)( struct symvar * e );// access function
    symbol_flags        flags;
} symvar;


/***************************************************************************/
/*  Symbolic variable dictionary                                           */
/***************************************************************************/

typedef struct symdict *symdict_hdl;

/***************************************************************************/
/*  Flags for sym_list                                                     */
/***************************************************************************/

typedef enum {
    sl_none     = 0,    // for initialization
    sl_attrib   = 1,    // single-letter attribute function ("&e'")
    sl_funct    = 2,    // multi-letter function ("&'upper")
    sl_text     = 3,    // treat as text ("& " and malformed items)
    sl_symbol   = 4,    // symbol ("&<text>" and "&<text>.") -- but not one of the above items
    sl_split    = 5,    // symbol value requires line be split
} slflags;


/***************************************************************************/
/*  Symbol list entry                                                      */
/***************************************************************************/

typedef struct sym_list_entry {
    struct sym_list_entry   *   prev;                   // next entry
    char                        value[BUF_SIZE];        // value of attribute, function, or symbol
    char                    *   start;                  // position of "&" in original buffer
    char                    *   end;                    // position of byte after item in original buffer
    slflags                     type;
} sym_list_entry;


/***************************************************************************/
/*  Flags for filecb and macrocb                                           */
/***************************************************************************/

typedef enum {
    FF_clear        = 0x0000,           // clear all flags
    FF_eof          = 0x0002,           // eof
    FF_err          = 0x0004,           // file error
    FF_crlf         = 0x0008,           // delete trailing CR and / or LF
    FF_macro        = 0x0010,           // entry is macro not file
    FF_tag          = 0x0030,           // entry is macro via tag
    FF_open         = 0x8000            // file is open
} fflags;

/***************************************************************************/
/*  Flags for input                                                        */
/***************************************************************************/

typedef enum {
    II_none     = 0x00,                 // neutral entry (inp_line only)
    II_file     = 0x01,                 // inputcb is file (inputcb only)
    II_macro    = 0x02,                 // inputcb is macro (inputcb only)
    II_tag      = 0x04,                 // inputcb is macro via tag (inputcb only)
    II_tag_mac  = II_tag | II_macro,    // input is tag or macro (inputcb only)
    II_input    = II_file | II_macro | II_tag, // all input types (inputcb only)
    II_research = 0x08,                 // research mode (for file only) (inputcb only)
    II_eof      = 0x10,                 // end of file (input) (inputcb only)
} i_flags;

/***************************************************************************/
/*  List of (defined macro / input) lines                                  */
/*    also used for in_buf_pool in this case fixed length buf_size         */
/***************************************************************************/

typedef struct inp_line {
    struct inp_line *   next;           // next line
    i_flags             fmflags;        // II_none only
    bool                fm_symbol;      // hidden_head is from a symbol substition
    bool                hh_tag;         // hidden_head is from a tag
    bool                ip_start;       // hidden_head is from an input phrase start tag
    bool                sym_space;      // symbol substitution was preceeded by a space
    char                value[1];       // line content variable length
} inp_line;

/***************************************************************************/
/*  label control block                                                    */
/***************************************************************************/

typedef struct labelcb {
    struct labelcb  *   prev;
    fpos_t              pos;            // file position for label if file
    line_number         lineno;         // lineno of label
    char                label_name[LABEL_NAME_LENGTH + 1];
} labelcb;

/***************************************************************************/
/*  Macro  dictionary                                                      */
/***************************************************************************/

typedef void    mac_dict;

/***************************************************************************/
/*  macro definition entry  for macro dictionary                           */
/***************************************************************************/

typedef struct mac_entry {
    struct mac_entry    *   next;
    inp_line            *   macline;    // macro definition lines
    line_number             lineno;     // lineno start of macro definition
    labelcb             *   label_cb;   // controlling label definitions
    char                *   mac_file_name;  // file name macro definition
    char                    name[MAC_NAME_LENGTH + 1];  // macro name
} mac_entry;

/***************************************************************************/
/*  entry for an (included) input file                                     */
/***************************************************************************/

typedef struct filecb {
    FILE        *   fp;                 // FILE ptr
    line_number     lineno;             // current line number
    line_number     linemin;            // first line number to process
    line_number     linemax;            // last line number to process
    size_t          usedlen;            // used data of filebuf
    fpos_t          pos;                // position for reopen
    labelcb     *   label_cb;           // controlling label definitions
    char        *   filename;           // full filename
    fflags          flags;
    char            fileattr[MAX_FILE_ATTR + 1];// T:xxxx
} filecb;

/***************************************************************************/
/*  parameter structure for macro call                                     */
/***************************************************************************/

typedef struct mac_parms {
    char        *   star;               // &*  complete parmline
    int             star0;              // &*0 parmcount
    inp_line    *   starx;              // &*1 - &*x parms
} mac_parms;

/***************************************************************************/
/*  Entry for an included macro                                            */
/***************************************************************************/

typedef struct  macrocb {
    line_number         lineno;         // current macro line number
    inp_line        *   macline;        // list of macro lines
    mac_entry       *   mac;            // macro definition entry
    struct gtentry  *   tag;            // tag entry if macro called via tag
    fflags              flags;
    bool                ix_seen;        // set when index tag/cw seen (even if indexing is off)
} macrocb;

/***************************************************************************/
/*  Stack for .if .th .el .do processing                                   */
/***************************************************************************/

typedef struct ifflags {

    unsigned    iflast  : 1;            // .if was last line
    unsigned    iftrue  : 1;            // last .if was true
    unsigned    iffalse : 1;            // last .if was false

    unsigned    ifthen  : 1;            // processing object of then
    unsigned    ifelse  : 1;            // processing object of else
    unsigned    ifdo    : 1;            // processing object of do group

    unsigned    ifcwte  : 1;            // .th or .el control word
    unsigned    ifcwdo  : 1;            // .do control word
    unsigned    ifcwif  : 1;            // .if control word

    unsigned    ifindo  : 1;            // .do begin done (not cleared by do end)


} ifflags;

typedef struct ifcb {
    int             if_level;           // nesting level
    ifflags         if_flags[MAX_IF_LEVEL + 1]; // index 0 not used
} ifcb;

/***************************************************************************/
/*  for .pe processing                                                     */
/***************************************************************************/

typedef struct pecb {                   // for .pe control
    char *  line;                       // line to perform n times
    int     ll;                         // length of line
    int     count;                      // value of .pe n  active if > 0
} pecb;

/***************************************************************************/
/*  input stack for files and macros                                       */
/***************************************************************************/

typedef struct  inputcb {
    struct inputcb  *   prev;
    inp_line        *   hidden_head;    // manage lines split at ; or :
    inp_line        *   hidden_tail;    // manage lines split at ; or :
    symdict_hdl         local_dict;     // local symbol dictionary
    ifcb            *   if_cb;          // for controlling .if .th .el
    pecb                pe_cb;          // for controlling .pe perform
    union  {
        filecb      *   f;              // used if input is from file
        macrocb     *   m;              // used if input is from macro/tag
    } s;
    i_flags             fmflags;
    bool                fm_hh;          // logical input record is from a hidden_head
    bool                fm_symbol;      // logical input record is from a symbol substition
    bool                hh_tag;         // hidden_head is indeed from a tag
    bool                sym_space;      // symbol substitution was preceeded by a space
} inputcb;

/***************************************************************************/
/*  scr keywords                                                           */
/***************************************************************************/

typedef enum {
    cw_break    = 1,           // control word causes break, ie. flush output
    cw_o_t                              // control word produces output text
} scrflags;

typedef struct scrtag {
    char            tagname[SCR_KW_LENGTH + 2];
    void            (*tagproc)( void );
    scrflags        cwflags;
} scrtag;

/***************************************************************************/
/*  GML tags    predefined                                                 */
/***************************************************************************/

typedef enum {
    tag_no_flag     = 0,                // none of the below
    tag_only        = 1,                // tag without any attribute
    tag_basic       = 2,                // basic elements possible on tag line.
    tag_text        = 4,                // text line possible
    etag_req        = 8,                // eTAG required
    etag_opt        = 16,               // eTAG optional
    tag_is_general  = 32,               // general tag
    tag_layout      = 64,               // tag valid in layout
    tag_out_txt     = 128,              // tag produces output text
} gmlflags;

/************************************************************************/
/*  These are used in two places:                                       */
/*    In rs_loc, they indicate that a restricted area has been entered  */
/*    In gmltag.taglocs, they indicate that this tag is allowed in that */
/*      restricted area                                                 */
/************************************************************************/

typedef enum {
    restricted_tag  = 0,                // tag is not allowed in any restricted section
    titlep_tag      = 1,                // tag allowed in TITLEP section
    address_tag     = 2,                // tag allowed in ADDRESS section
    figcap_tag      = 4,                // tag allowed after FIGCAP
    banner_tag      = 8,                // tag allowed in BANNER (BANREGION, eBANNER)
    banreg_tag      = 16,               // tag allowed in BANREGION (eBANREGION)
} locflags;

/************************************************************************/
/*  Tag classes are used to identify certain tags as belonging to       */
/*  certain classes. These classes are then used in processing.         */
/************************************************************************/

typedef enum {
    no_class        = 0,                // tag is not assigned a class
    def_tag         = 1,                // marks DDHD, DD, GD
    index_tag       = 2,                // marks I1, I2, I3, IH1, IH2, IH3
    ip_start_tag    = 4,                // marks CIT, HP0, HP1, HP2, HP3, SF, Q
    ip_end_tag      = 8,                // marks eCIT, eHP0, eHP1, eHP2, eHP3, eSF, eQ
    li_lp_tag       = 16,               // marks LI LP
    list_tag        = 32,               // marks DTHD, DDHD, DT, DD, GT, GD, LI, LP
} classflags;

typedef struct gmltag {
   char             tagname[TAG_NAME_LENGTH + 1];
   size_t           taglen;
   void             (*gmlproc)( const struct gmltag * entry );
   gmlflags         tagflags;
   locflags         taglocs;
   classflags       tagclass;
} gmltag;

/***************************************************************************/
/*  GML tags   user defined                                                */
/*  i.e.  via .gt and .ga script control words                             */
/*  enum values have to be single bits 2**x                                */
/***************************************************************************/

typedef enum gavalflags {
    val_def     = 1,                    // value is default
    val_any     = 2,                    // any value allowed
    val_length  = 4,                    // max length of value
    val_range   = 8,                    // allowed range (numeric)
    val_value   = 16,                   // allowed value stored in union
    val_valptr  = 32,                   // allowed value allocated
    val_auto    = 64,                   // automatic (not used / implemented)
    val_reset   = 128                   // reset (not used / implemented)
} gavalflags;

/***************************************************************************/
/*  options B   from .ga control word                                      */
/***************************************************************************/

typedef struct gavalentry {
    struct gavalentry   *   next;
    union a {
       size_t   length;                 // possible max length of (character) value
       int      range[4];               // min, max, default omitted, default without value
       char     value[VAL_LENGTH + 1];  // string value if short enough
       char *   valptr;                 // ... else allocated
    } a;
    gavalflags              valflags;
} gavalentry;

/***************************************************************************/
/*  options A   from .ga control word                                      */
/*  enum values have to be single bits 2**x                                */
/*  exception are the att_proc_xxx values                                  */
/***************************************************************************/

typedef enum {
    att_def         = 0x0001,           // attribute has default value
    att_range       = 0x0002,           // attribute has range
    att_auto        = 0x0004,           // attribute is automatic
    att_any         = 0x0008,           // attribute any value allowed
    att_req         = 0x0010,           // attribute required
    att_upper       = 0x0020,           // translate to upper
    att_off         = 0x0040,           // attribute is inactive

    att_proc_all    = 0x0f00,           // mask for processing flags

    att_proc_req    = 0x0100,           // req attr not yet seen
    att_proc_auto   = 0x0200,           // auto attr cannot be specified
    att_proc_seen   = 0x0400,           // attr specified
    att_proc_val    = 0x0800            // ... with value specified
} gaflags;

/***************************************************************************/
/*  entry from .ga control word                                            */
/***************************************************************************/

typedef struct gaentry {
    struct gaentry  *   next;
    gavalentry      *   vals;
    char                name[ATT_NAME_LENGTH + 1];
    gaflags             attflags;
} gaentry;

/***************************************************************************/
/*  GML tag options from the .gt Control word                              */
/*  enum values have to be single bits, powers of 2                        */
/***************************************************************************/

typedef enum {
    tag_attr     = 1,                   // tag has attributes
    tag_cont     = 2,                   // CONTinue specified
    tag_nocont   = 4,                   // NOCONTinue specified
    tag_csoff    = 8,                   // CSOFF specified
    tag_next     = 16,                  // TAGnext specified
    tag_textdef  = 32,                  // TEXTDef specified
    tag_texterr  = 64,                  // TEXTError specified
    tag_textline = 128,                 // TEXTLine specified
    tag_textreq  = 256,                 // TEXTRequired specified
    tag_off      = 512                  // tag OFF specified
} gtflags;

/***************************************************************************/
/*  Tag entry  from .gt control word                                       */
/***************************************************************************/

typedef struct gtentry {
    struct gtentry  *   next;
    gaentry         *   attribs;        // list of attributes
    unsigned            usecount;
    size_t              namelen;        // actual length of name
    char                name[TAG_NAME_LENGTH + 1];
    char                macname[MAC_NAME_LENGTH + 2];   // macro to call
    gtflags             tagflags;
    bool                overload;       // user tag has same name as predefined tag
} gtentry;

/***************************************************************************/
/*  condcode  returncode for several conditions during parameterchecking   */
/*            loosely adapted from wgml 88.1 IBM S/360 ASM code            */
/***************************************************************************/

typedef enum condcode {            // return code for some scanning functions
    zero            = 0,
    omit            = 1,                // argument omitted
    pos             = 2,                // argument affirmative
    neg             = 4,                // argument negative
    quotes          = 8,                // argument within quotes
    quotes0         = 16,               // only quotes
    no              = 32,               // argument undefined
    notnum          = 32                // value not numeric / overflow
}  condcode;

/***************************************************************************/
/*  scr string functions                                                   */
/***************************************************************************/

typedef struct parm {
    char    *       a;                  // start of parm ptr
    char    *       e;                  // end of parm ptr
    bool            redo;               // parm starts with "&"
} parm;

typedef struct scrfunc {
    const   char    fname[FUN_NAME_LENGTH + 1];   // function name
    const   size_t  length;             // actual length of fname
    const   size_t  parm_cnt;           // mandatory parms
    const   size_t  opt_parm_cnt;       // optional parms
    condcode        (*fun)( parm parms[MAX_FUN_PARMS], size_t parm_count,
                            char * * ppval, int32_t valsize );
} scrfunc;

/***************************************************************************/
/*  definitions for getnum routine  to be reworked  TBD                    */
/***************************************************************************/

typedef enum {
    selfdef     = 4,
    aritherr    = 8,
    ilorder     = 12,
    illchar     = 16,
    mnyerr      = 20,
    operr       = 24,
    parerr      = 28,
    enderr      = 32
} getnumrc;

typedef struct getnum_block {
    int         ignore_blanks;          // 1 if blanks are ignored
    char        *argstart;
    char        *argstop;
    char        *errstart;
    char        *first;
    int         length;
    int         result;                 // result as long
    char        resultstr[NUM2STR_LENGTH];  // result in char format (32-bit number)
    getnumrc    error;
    char        num_sign;               // remember absolute or relative value
    condcode    cc;
} getnum_block;

/***************************************************************************/
/*  struct used to hold parameters of option FONT                          */
/***************************************************************************/

typedef struct opt_font {
    struct opt_font *   nxt;
    char *              name;
    char *              style;
    uint32_t            space;
    uint32_t            height;
    font_number         font;
} opt_font;

/***************************************************************************/
/*  message numbers  + severities                                          */
/***************************************************************************/

typedef enum msg_ids  {
    #include "wgmlmsge.gh"              // as lowercase enums
} msg_ids;

#include "wgmlmsgs.gh"                  // as uppercase defines

typedef enum {
    SEV_BANNER,
    SEV_DEBUG,
    SEV_INFO,
    SEV_WARNING,
    SEV_ERROR,
    SEV_FATAL_ERR
} severity;

/***************************************************************************/
/* enum for generated document sections                                    */
/***************************************************************************/

typedef enum {
    gs_none     = 0,    // document specification includes neither FIGLIST nor TOC
    gs_figlist  = 1,    // document specification includes FIGLIST
    gs_toc      = 2,    // document specification includes TOC
} gen_sect;

/***************************************************************************/
/* enum for sections with page number styles and conversion struct         */
/***************************************************************************/

typedef enum {
    pns_abstract,
    pns_preface,
    pns_body,
    pns_appendix,
    pns_backm,
    pns_index,
    pns_max             // for pgnum_style array sizing
} pgnum_sect;

/***************************************************************************/
/* enum for document sections  sequence is important, don't change         */
/***************************************************************************/

typedef enum doc_section {
    doc_sect_none,                      // nothing so far
    doc_sect_gdoc,                      // gdoc
    doc_sect_frontm,                    // front matter
    doc_sect_titlep,                    // title page
    doc_sect_etitlep,                   // end title page
    doc_sect_abstract,                  // abstract
    doc_sect_preface,                   // preface
    doc_sect_toc,                       // table of contents
    doc_sect_figlist,                   // figure list
    doc_sect_body,                      // body
    doc_sect_appendix,                  // appendix
    doc_sect_backm,                     // back matter
    doc_sect_index,                     // index
    doc_sect_toce,                      // table of contents (at end of file)
    doc_sect_figliste,                  // figure list (at end of file)
    doc_sect_egdoc                      // egdoc  has to be last
} doc_section;

/***************************************************************************/
/* enum for justify values                                                 */
/***************************************************************************/

typedef enum ju_enum {                  // for .ju(stify)
    ju_off,                             // ju_off must have lowest value
    ju_on,                              // ju_on next
    ju_half,
    ju_left,
    ju_right,
    ju_centre,
    ju_center = ju_centre,
    ju_inside,
    ju_outside
} ju_enum;

/***************************************************************************/
/*  enums for layout tags with attributes  (and ebanregion)                */
/*  the order is as shown by WGML 4.0 :convert output                      */
/***************************************************************************/

typedef enum lay_sub {
    el_zero     = 0,                    // dummy to make 0 invalid
    el_page     = 1,
    el_default,
    el_widow,
    el_fn,
    el_fnref,
    el_p,
    el_pc,
    el_fig,
    el_xmp,
    el_note,
    el_h0,
    el_h1,
    el_h2,
    el_h3,
    el_h4,
    el_h5,
    el_h6,
    el_heading,
    el_lq,
    el_dt,
    el_gt,
    el_dthd,
    el_cit,
    el_figcap,
    el_figdesc,
    el_dd,
    el_gd,
    el_ddhd,
    el_abstract,
    el_preface,
    el_body,
    el_backm,
    el_lp,
    el_index,
    el_ixpgnum,
    el_ixmajor,
    el_ixhead,
    el_i1,
    el_i2,
    el_i3,
    el_toc,
    el_tocpgnum,
    el_toch0,
    el_toch1,
    el_toch2,
    el_toch3,
    el_toch4,
    el_toch5,
    el_toch6,
    el_figlist,
    el_flpgnum,
    el_titlep,
    el_title,
    el_docnum,
    el_date,
    el_author,
    el_address,
    el_aline,
    el_from,
    el_to,
    el_attn,
    el_subject,
    el_letdate,
    el_open,
    el_close,
    el_eclose,
    el_distrib,
    el_appendix,
    el_sl,
    el_ol,
    el_ul,
    el_dl,
    el_gl,
    el_banner,
    el_banregion,
    el_ebanregion
} lay_sub;

/****************************************************************************/
/*  definitions for function codes inserted into input buffer               */
/*  originally intended for subscript and superscript and similar items     */
/*    function start should be an even value                                */
/*    function end the following odd value                                  */
/*    0xfd 0x02 subscripted text 0xfd 0x03     example for subscripted text */
/*                                                                          */
/*  incomplete will change                                      TBD         */
/****************************************************************************/

typedef enum functs {
    function_escape         = '\xfd',
    function_end            = '\x01',

    function_subscript      = '\x02',
    function_sub_end        = '\x03',

    function_superscript    = '\x04',
    function_sup_end        = '\x05',
} functs;

/***************************************************************************/
/*  tags and controlwords as enums for distinction during processing       */
/***************************************************************************/

typedef enum e_tags {
    t_NONE,
    #define pickg( name, length, routine, gmlflags, locflags, classname )  t_##name,
    #include "gtags.h"
    #undef pickg
//    #define picklab( name, routine, flags )  t_label,
//    #define picks( name, routine, flags )  t_##name,
//    #include "gscrcws.h" TBD
//    #undef picks
//    #undef picklab
    t_MAX                               // the last one for range check
} e_tags;

/***************************************************************************/
/*  nesting stack for open tags even if input file is not active any more  */
/***************************************************************************/

typedef struct nest_stack {
    struct  nest_stack  * prev;

    line_number         lineno;         // lineno of :xl, :HPx :SF call
    union {
        char        *   filename;       // file name of :xl, :HPx :SF call
        struct mt {
            gtentry     *   tag_m;      // for usertag / macro
            mac_entry   *   m;          // macro entry of :xl, :HPx :SF call
        } mt;
    } s;
    i_flags             nest_flag;      // for selecting the union
} nest_stack;

/***************************************************************************/
/* Stack of margins and other values for nested tags                       */
/*                                                                         */
/* Originally for tag/etag pairs, to catch mismatches and report the error */
/* Inevitably, other item were added, some of them very tag-specific       */
/* "on entry" indicates the field is intended to save the value found      */
/* when the start tag is processed so it can be restored by the end tag    */
/* "current" fields are for values pertaining to the current tag/block     */
/* the lists depend heavily on this stack                                  */
/***************************************************************************/

struct dl_lay_level;    // avoids include circularity with gtypelay.h
struct gl_lay_level;    // avoids include circularity with gtypelay.h
struct ol_lay_level;    // avoids include circularity with gtypelay.h
struct sl_lay_level;    // avoids include circularity with gtypelay.h
struct ul_lay_level;    // avoids include circularity with gtypelay.h

typedef struct tag_cb {
    struct  tag_cb      *   prev;           // open tag chain
    nest_stack          *   p_stack;        // calling chain for this tag
    union {     // these will be for the specific level in effect
        struct  dl_lay_level    *   dl_layout;  // current DL LAYOUT record
        struct  gl_lay_level    *   gl_layout;  // current GL LAYOUT record
        struct  ol_lay_level    *   ol_layout;  // current OL LAYOUT record
        struct  sl_lay_level    *   sl_layout;  // current SL LAYOUT record
        struct  ul_lay_level    *   ul_layout;  // current UL LAYOUT record
    } u;
    uint32_t                li_number;      // current list item no
    uint32_t                lm;             // left margin on entry
    uint32_t                rm;             // max width on entry
    int32_t                 align;          // current attribute value
    int32_t                 left_indent;    // current attribute value
    int32_t                 right_indent;   // current attribute value
    uint32_t                post_skip;      // current attribute value
    uint32_t                tsize;          // current attribute value
    uint32_t                xl_pre_skip;    // parent list pre_skip value (used by LP)
    text_space              spacing;        // spacing on entry
    uint8_t                 headhi;         // current attribute value
    uint8_t                 termhi;         // current attribute value
    font_number             font;           // font on entry
    bool                    compact  : 1;   // current attribute value
    bool                    dl_break : 1;   // current attribute value
    bool                    in_list  : 1;   // true if inside a list, including current tag
    e_tags                  c_tag;          // enum of tag
} tag_cb;

/***************************************************************************/
/*  for constructing a filename stack                                      */
/***************************************************************************/

typedef struct fnstack {
    struct  fnstack * prev;
    char    fn[1];                      // var length file name
} fnstack;

/***************************************************************************/
/*  for cmdline specified layout files stack (FIFO)                        */
/***************************************************************************/

typedef struct laystack {
    struct  laystack * next;
    char    layfn[1];                   // var length file name
} laystack;

/***************************************************************************/
/*  box column definition for use with control word BX                     */
/***************************************************************************/

#define BOXCOL_COUNT 16

typedef enum {  // see Wiki for column type definitions
    bx_v_both,  // current box "both" column
    bx_v_down,  // current box "down" column
    bx_v_hid,   // current box "hid" column
    bx_v_new,   // current box "new" column
    bx_v_out,   // current box "out" column
    bx_v_split, // current box "split" column
    bx_v_up,    // current box "up" column
} bx_v_ind;

typedef struct {
            uint32_t        col;
            uint32_t        depth;
            bx_v_ind        v_ind;
            bool            rebreak;
} box_col_spec;

typedef struct box_col_set {
    struct  box_col_set     *   next;
            uint32_t            current;
            uint32_t            length;
            box_col_spec    *   cols;
} box_col_set;

typedef struct box_col_stack {
    struct  box_col_stack   *   next;
            box_col_set     *   first;
            bool                had_cols;
            bool                inner_box;
} box_col_stack;

/***************************************************************************/
/*  a single tab stop and an array of tab stops                            */
/***************************************************************************/

typedef enum {
    al_center,
    al_left,
    al_right,
} alignment;

typedef struct {
    uint32_t            column;
    alignment           alignment;
    uint8_t             fill_char;
} tab_stop;

typedef struct {
    uint16_t            current;
    uint16_t            length;
    tab_stop       *    tabs;
} tab_list;

/***************************************************************************/
/*  word, line, page, column, element items                                */
/***************************************************************************/

typedef enum {
    tx_norm     = 0,    // normal text
    tx_sup      = 1,    // superscript text
    tx_sub      = 2,    // subscript text
    tx_figcap   = 4,    // FIGCAP prefix ending (only used with WHELP)
} text_type;

typedef enum {
    tt_none,
    tt_def,
    tt_user
} tab_type;

/***************************************************************************/
/*  Character device markers                                               */
/*  These run the functions defined by the first LINEPROCS instance in     */
/*  the FONTSTYLE block (in the DRIVER)                                    */
/*  These functions are not part of the font switch sequence as such       */
/*  Some tags allow multiple markers to be condensed into one              */
/*                                                                         */
/*  Note: Although the code does not distinguish, WHELP is the device      */
/*        these markers produce visible effects with                       */
/*  Note: PS markers are fs_norm text_chars with no text                   */
/***************************************************************************/

typedef enum {
    fs_norm,        // this is not a marker; it is a normal text_chars instance
    fs_full,        // this is a full marker: both startvalue and endvalue functions are run
    fs_from,        // this is a half-marker which runs the endvalue function only
    fs_from2,       // allow one half-marker to do the work of two fs_from half-markers
    fs_to,          // this is a half-marker which runs the startvalue function only
    fs_to2          // allow one half-marker to do the work of two fs_to half-markers
} fontswitch_type;

typedef struct text_chars {                 // tabbing-related fields have comments
    struct  text_chars  *   next;
    struct  text_chars  *   prev;
            uint32_t        x_address;
            uint32_t        width;
            uint32_t        ts_width;       // tab space width (expected to be needed with al_center and al_right)
            uint16_t        count;
            uint16_t        length;
            alignment       tab_align;      // tab alignment
            bool            pre_gap;        // true if original text had preceding space
            bool            post_ix;        // record state of ProcFlags.post_ix
            font_number     font;
            font_number     phrase_font;    // actual SF font, even if too large
            fontswitch_type f_switch;       // font switch type (bx-related)
            i_flags         fmflags;        // flags from inputcb
            tab_type        tab_pos;        // if not tt_none, text_chars was positioned by a tab character
            text_type       type;
            char            text[1];
} text_chars;

struct eol_ix;              // forward declaration

typedef struct text_line {
    struct  text_line   *   next;
            uint32_t        line_height;
            units_space     units_spacing;
            uint32_t        y_address;
    struct  eol_ix      *   eol_index;
            text_chars  *   first;
            text_chars  *   last;
} text_line;

typedef enum {
    el_binc,        // BINCLUDE element
    el_dbox,        // DBOX element
    el_graph,       // GRAPHIC element
    el_hline,       // HLINE element
    el_text,        // text element
    el_vline,       // VLINE element
    el_vspace,      // vertical space element (blank lines and SP)
} element_type;

// struct doc_element;    // forward declaration (uncomment if ever needed)

typedef struct {
            uint32_t        cur_left;
            uint32_t        depth;
            uint32_t        y_address;
            bool            at_top;
            bool            force_FONT0;
            bool            has_rec_type;
    struct  eol_ix          *eol_index;
            FILE            *fp;
            char            *file;
} binclude_element;

typedef struct {
            uint32_t        h_start;
            uint32_t        v_start;
            uint32_t        h_len;
            uint32_t        v_len;
    struct  eol_ix      *   eol_index;
} dbox_element;

typedef struct {
            uint32_t        cur_left;
            uint32_t        depth;
            uint32_t        scale;
            uint32_t        width;
            uint32_t        y_address;
            int32_t         xoff;
            int32_t         yoff;
            bool            at_top;
    struct  eol_ix          *eol_index;
            FILE            *fp;
            font_number     next_font;
            char            *short_name;
            char            *file;
} graphic_element;

typedef struct {
            uint32_t        h_start;
            uint32_t        v_start;
            uint32_t        h_len;
            uint32_t        o_subs_skip;
            uint32_t        o_top_skip;
    struct  eol_ix      *   eol_index;
            bool            ban_adjust; // hline is first line in an outer box at effective top of page
} hline_element;

typedef struct {
    struct  doc_element *   prev;       // only used with headings
    struct  ffh_entry   *   entry;      // only used with headings
    struct  ref_entry   *   ref;        // only used with headings
            text_line   *   first;
            bool            bx_h_done;  // bx has added ascenders to the line
            bool            force_op;   // forces overprint at top of page
            bool            overprint;  // placement avoids padding warning
            bool            vspace_next;// next element is a vspace_element (XMP/eXMP blocks only)
} text_element;

typedef struct {
            uint32_t        h_start;
            uint32_t        v_start;
            uint32_t        v_len;
    struct  eol_ix      *   eol_index;
            bool            twice;
} vline_element;

typedef struct {
    font_number             font;
    struct  eol_ix      *   eol_index;
} vspace_element;

typedef struct doc_element {
    struct  doc_element     *   next;
            uint32_t            blank_lines;
            uint32_t            depth;
            uint32_t            subs_skip;
            uint32_t            top_skip;
            uint32_t            h_pos;      // used by multicolumn support only
            uint32_t            v_pos;      // used by multicolumn support only
    union {
            binclude_element    binc;
            dbox_element        dbox;
            graphic_element     graph;
            hline_element       hline;
            text_element        text;
            vline_element       vline;
            vspace_element      vspace;
    } element;
            element_type        type;       // placement avoids padding warning
            bool                do_split;   // split the group with this element starting the new group
            bool                in_xmp;     // true is element was inside an XMP/eXMP block
            bool                op_co_on;   // true if concatenation was on when element was processed
} doc_element;

/********************************************************************/
/* Tags and control words that use blocks of doc_elements           */
/* None of these can occur inside of itself (except BX, in the      */
/*   sense that BX lines can be inside a box formed by BX)          */
/* Except for ADDRESS and BX, none of these can occur inside any of */
/*   the others; ADDRESS allows none of the tags and the control    */
/*   words FB and FK have no apparent effect                        */
/* In addition, CC and CP cannot occur inside any of these except   */
/*   ADDRESS                                                        */
/* wgml 4.0 does not recognize cc or cp blocks, so they are not     */
/*   assigned tags here                                             */
/* control word FN has not been implemented, so no tag is assigned  */
/*   it may, of course, turn out to be possible to use gt_fn        */
/* Control word BX can be used to draw a box around any block or    */
/*   inside any block, or both with the same block                  */
/* Thus, the stack intermingles BX groups and other groups, but can */
/*   only contain one of the other types of groups at any time      */
/* The queues used with FB and FK can, of course, contain multiple  */
/*   instances of those blocks                                      */
/********************************************************************/

typedef enum {
    gt_none,    // no doc_el_group in use
    gt_address, // tag ADDRESS
    gt_bx,      // control word BX
    gt_co,      // control word CO
    gt_fn,      // tag FN
    gt_fig,     // tag FIG
    gt_fb,      // control word FB
    gt_fk,      // control work FK
    gt_hx,      // tags H0--H6
    gt_xmp,     // tag XMP
} group_type;

typedef struct doc_el_group {
    struct  doc_el_group    *   next;
            uint32_t            depth;
            uint32_t            post_skip;  // figure or heading at top of column
            doc_element     *   first;
            doc_element     *   last;
            group_type          owner;      // tag or control word using this instance
            font_number         block_font; // used by CO OFF/CO ON blocks
} doc_el_group;

typedef struct {
            uint32_t        main_top;       // top of page for main
            uint32_t        fig_top;        // top of page for bot_fig
            uint32_t        fn_top;         // top of page for footnote
            uint32_t        col_left;       // column left margin
            doc_element *   col_width;
            doc_element *   main;
            doc_element *   bot_fig;
            doc_element *   footnote;
} doc_column;

typedef struct doc_pane {
    struct  doc_pane        *   next;
            uint32_t            page_width_top; // top of page for page_width
            uint32_t            col_width_top;  // top of page for col_width
            uint32_t            col_count;      // number of columns
            uint32_t            col_width;      // width of each column
            uint32_t            cur_col;        // column currently in use
            doc_element     *   page_width;
            doc_column          cols[MAX_COL];
} doc_pane;

struct banner_lay_tag;  // avoids include circularity with gtypelay.h

/***************************************************************************/
/*  This will be changing, possibly quite a bit, as multicolumn support    */
/*  is added; fields added for multicolumn support are commented           */
/*  The "last" pointers are used to keep track of where to add each new    */
/*  doc_element as it appears, both in doc_page and doc_next_page below    */
/*  NOTE: g_page_top is currently used to hold the top of page_width       */
/*  NOTE: page sections without "last" pointers do not need them because   */
/*        they can only hold one item (heading or figure)                  */
/***************************************************************************/

typedef struct {
            uint32_t            page_top;       // top of page for top banner
            uint32_t            panes_top;      // top of page for first pane
            uint32_t            bot_ban_top;    // top of page for bottom banner
            uint32_t            max_depth;
            uint32_t            cur_depth;
            uint32_t            page_left;      // page left margin
            uint32_t            page_width;     // page right margin
            uint32_t            max_width;      // maximum width (page or column)
            int32_t             cur_left;       // net adjustment to left margin (can be negative)
            int32_t             cur_width;      // current width
            doc_pane        *   last_pane;
            doc_column      *   cur_col;        // quick access to t_page.last_pane->cols[t_page.last_pane->cur_col]
            doc_element     *   last_col_main;
            doc_element     *   last_col_fn;
    struct  banner_lay_tag  *   top_banner;
    struct  banner_lay_tag  *   bottom_banner;
            symsub          *   topheadsub;     // ptr to $TOPHEAD symvar entry
            symsub          *   botheadsub;     // ptr to $BOTHEAD symvar entry
            doc_element     *   top_ban;
            doc_pane        *   panes;
            doc_element     *   bot_ban;
    struct  eol_ix          *   eol_index;
} doc_page;

typedef struct {
            doc_el_group    *   last_fk_queue;
            doc_el_group    *   last_page_width;
            doc_el_group    *   last_col_width;
            doc_element     *   last_col_main;
            doc_el_group    *   last_col_bot;
            doc_el_group    *   last_col_fn;
            doc_el_group    *   fk_queue;       // pending FK blocks
            doc_el_group    *   page_width;     // page_width
            doc_el_group    *   col_width;      // cols->col_width
            doc_element     *   col_main;       // cols->main
            doc_el_group    *   col_bot;        // cols->bot
            doc_el_group    *   col_fn;         // cols->fn
            uint32_t            prev_pg_depth;
} doc_next_page;

/***************************************************************************/
/*  moved from the :BANREGION attribute values in gtypelay.h for use here  */
/***************************************************************************/

typedef enum content_enum {
    no_content          =  0,
    author_content,
    bothead_content,
    date_content,
    docnum_content,
    head0_content,
    head1_content,
    head2_content,
    head3_content,
    head4_content,
    head5_content,
    head6_content,
    headnum0_content,
    headnum1_content,
    headnum2_content,
    headnum3_content,
    headnum4_content,
    headnum5_content,
    headnum6_content,
    headtext0_content,
    headtext1_content,
    headtext2_content,
    headtext3_content,
    headtext4_content,
    headtext5_content,
    headtext6_content,
    pgnuma_content,
    pgnumad_content,
    pgnumr_content,
    pgnumrd_content,
    pgnumc_content,
    pgnumcd_content,
    rule_content,
    sec_content,
    stitle_content,
    title_content,
    string_content,
    time_content,
    tophead_content,
    max_content                         // keep as last entry
} content_enum;

/***************************************************************************/
/*  definitions for number style                                           */
/*  moved from gtypelay.h to avoid inclusion circularity                   */
/***************************************************************************/

typedef enum num_style {
    h_style     = 0x0001,               // hindu-arabic
    a_style     = 0x0002,               // lowercase alphabetic
    b_style     = 0x0004,               // uppercase alphabetic
    c_style     = 0x0080,               // uppercase roman
    r_style     = 0x0010,               // lowercase roman
    char1_style = a_style | b_style | c_style | h_style | r_style,
    xd_style    = 0x0100,               // decimal point follows
    xp_style    = 0x0600,               // in parenthesis
    xpa_style   = 0x0200,               // only left parenthesis
    xpb_style   = 0x0400                // only right parenthesis
} num_style;

/***************************************************************************/
/*  Structures for storing index information from IX control word          */
/*  and In IHn IREF tags                                                   */
/***************************************************************************/

typedef enum {          // definition order is important
    pgnone,             // nothing (In, IHn, IREF)
    pgpageno,           // numeric page number (IX default)(In)
    pgmajor,            // major page reference (IX)(In, IREF)
    pgstart,            // start page (In, IREF)
    pgend,              // end page (In, IREF)
    pgstring,           // pg string (In, IREF) / reference (ix)
    pgmajorstring,      // major string reference (IX)
    pgsee,              // see string or seeid item (IHn, IREF)
} ereftyp;

typedef struct ix_e_blk {                   // index entry for pagenos / text
    struct  ix_e_blk    *   next;           // next entry
            char        *   prt_text;       // used with seeid when ref has prt_text
    union {
        struct {
            char        *   page_text;      // reference is text (IX ref, pg string, see/seeid string)
            size_t          page_text_len;  // reference text length
        } pageref;
        struct {
            uint32_t        page_no;        // pageno is number
            num_style       style;          // page number style defined by banner, if any
        } pagenum;
    } u;
            ereftyp          entry_typ;     // selects page_no or page_text (IX, In), or no reference (IHn)
} ix_e_blk;

/* These are used in the order given, and each is kept in sorted order */

typedef struct {
    ix_e_blk    *   major_pgnum;    // first major page number ix entry block
    ix_e_blk    *   major_string;   // first major string ix entry block
    ix_e_blk    *   normal_pgnum;   // first normal page number ix entry block
    ix_e_blk    *   normal_string;  // first normal string ix entry block
    ix_e_blk    *   see_string;     // first see/seeid string ix entry block
} entry_list;

typedef struct ix_h_blk {                   // index header with index term text
    struct  ix_h_blk    *   next;           // next ix header block same level
    struct  ix_h_blk    *   lower;          // first ix hdr block next lower level
            entry_list  *   entry;          // set of pointers to ix entry block
            uint32_t        ix_lvl;         // index level 1 - 3
            size_t          ix_term_len;    // index term length
            char        *   ix_term;        // index term
            size_t          prt_term_len;   // display text length
            char        *   prt_term;       // display text (NULL -> use index term)
} ix_h_blk;

typedef struct eol_ix {                     // type for eol_index list
    struct  eol_ix      *   next;
    struct  ix_h_blk    *   ixh;            // the index header block to reference
            ereftyp         type;           // type of entry (pgmajor, pgnum, pgbegin, pgend)
} eol_ix;

/***************************************************************************/
/*  Enum for distinguishing the source of a heading.                       */
/*  For use with gen_heading() in ghx.c                                    */
/*  Note: the order of the tags matters, please do not reorganize them     */
/*  Note: "heading" here refers to something that appears in the TOC       */
/*  Note: allows control of differences, such as which layout to use       */
/***************************************************************************/

typedef enum {
    hds_h0,         // H0 tag
    hds_h1,         // H1 tag
    hds_h2,         // H2 tag
    hds_h3,         // H3 tag
    hds_h4,         // H4 tag
    hds_h5,         // H5 tag
    hds_h6,         // H6 tag
    hds_appendix,   // APPENDIX (also for sizing the hierarchy-related arrays)
    hds_abstract,   // ABSTRACT (also used for sizing the hx_head_lay_tag array)
    hds_preface,    // PREFACE
    hds_body,       // BODY
    hds_backm,      // BACKM
    hds_index,      // INDEX
    hds_max         // used for sizing the hx_sect_lay_tag array
} hdsrc;

/***************************************************************************/
/*  Structure for storing information used to actually process and output  */
/*  (if appropriate) headings, whether from an Hn tag or a secion tag.     */
/*  IN PROGRESS */
/***************************************************************************/

typedef struct {
    int32_t         headn;              // current heading number (numeric)
    char            hnumstr[64];        // current heading number (text)
    symsub      *   headsub;            // ptr to $HEADx symvar entry
    symsub      *   hnumsub;            // ptr to $HNUMx symvar entry
    symsub      *   htextsub;           // ptr to $HTEXTx symvar entry
    char            tag[3];             // "H0" ... "H6", used for Hx tags only
} hd_num_data;

typedef enum page_pos {         // needed here to avoid gtypelay.h circularity
    pos_left,
    pos_right,
    pos_center,
    pos_centre = pos_center
} page_pos;

/***************************************************************************/
/*  Structure for storing figure/heading information from :FIG, :FIGCAP,   */
/*  :Fn, :Hn tags                                                          */
/*  Note: footnote and heading-related control words are not implemented   */
/***************************************************************************/

typedef enum {
    ffh_fig         =    1, // FIG entry
    ffh_fn          =    2, // FN entry
    ffh_hn          =    4, // heading entry
    ffh_figcap      =    8, // FIGCAP used -- even if there is no text
    ffh_prefix      =   16, // use heading prefix
} ffhflags;

typedef struct ffh_entry {
    struct ffh_entry     *  next;
    uint32_t                pageno; // output page
    uint32_t                number; // figure/footnote number or heading level
    char                *   prefix; // figcap/heading generated text
    char                *   text;   // figcap/heading text line
    num_style               style;  // figcap/heading number format (based on section)
    ffhflags                flags;
    bool                    abs_pre;// first heading in ABSTRACT or PREFACE
} ffh_entry;

/***************************************************************************/
/*  Structure for building reference dictionaries for tags :FIG, :FN, :Hn, */
/*  :In, and :IHn and for using refids to find the associated entry by     */
/*  tags :FIGREF, FNREF, HDREF, and :IREF                                  */
/*  Note: Although attribute refid is always required, attribute id is     */
/*  never required: not even a footnote is required to have an id. Thus,   */
/*  the complete list of entries for FIGLIST, INDEX, and TOC are found in  */
/*  fig_dict, hd_dict, and index_dict, and is provided in fn_dict;         */
/*  fig_dict, fn_dict, and hd_dict are also used in multipass processing   */
/***************************************************************************/

typedef enum {
    rf_ffh          =   1,  // :FIG, FN, or :Hx entry
    rf_ix           =   2,  // :Ix :IHx entry
} refflags;

typedef struct ref_entry {
    struct ref_entry    *   next;
    char                    id[ID_LEN+1];   // reference id
    refflags                flags;
    union {
        struct {
            ffh_entry   *   entry;          // detail for FIG, FN, or Hx entry
        } ffh;
        struct {
            ix_h_blk    *   hblk;           // detail for IX, In, IHn, or IREF entry
            ix_h_blk    *   base;           // attachment point for hblk, if any
        } ix;
    } u;
} ref_entry;

/***************************************************************************/
/*  forward reference / undefined id / page change                         */
/*   used for :FIG, :FN, :Hx, :FIGREF, :FNREF, :HDREF                      */
/***************************************************************************/

typedef struct fwd_ref {
    struct fwd_ref  *   next;
    char                id[ID_LEN+1];   // reference id
} fwd_ref;

/********************************************************************************/
/*  enum and struct for use with script styles                                  */
/*   used for .BD, .BI, .UL, .US and others as they are implemented             */
/*   related functions &'bold() etc may also need to use them, if implemented   */
/********************************************************************************/

typedef enum {
    SCT_none    = 0,    // none in effect
    SCT_bd      = 1,    // .BD in effect
    SCT_us      = 2,    // .US in effect (also .UL, which has the same effect)
    SCT_bi      = 4,    // .BI in effect
} style_cw_type;

typedef enum {
    SCS_none,   // not in use
    SCS_line,   // text on same line as control word
    SCS_on,     // used with ON
    SCS_count,  // used with line count
} style_cw_scope;

typedef struct {
    uint32_t        count;  // number of lines, if scope is SCS_count
    style_cw_scope  scope;  // scope enum
} style_cw_info;

typedef struct {
    style_cw_info   cw_bd;  // info for .BD
    style_cw_info   cw_us;  // info for .US (or .UL, which behaves the same way)
    font_number     font;   // font to restore when all scope fields are SCS_none
    style_cw_type   style;  // the current script style to use
} script_style_info;

/***************************************************************************/
/*  Bit-flag structs                                                       */
/***************************************************************************/

/***************************************************************************/
/*  Mostly command-line option flags, plus pass control                    */
/***************************************************************************/

typedef struct global_flags {
    unsigned        quiet         : 1;  // suppress product info
    unsigned        bannerprinted : 1;  // product info shown
    unsigned        wscript       : 1;  // enable WATCOM script extension
    unsigned        firstpass     : 1;  // first or only pass
    unsigned        lastpass      : 1;  // last or only pass
    unsigned        inclist       : 1;  // show included files
    unsigned        warning       : 1;  // show warnings
    unsigned        statistics    : 1;  // output statistics at end

    unsigned        index         : 1;  // index option
    unsigned        free9         : 1;
    unsigned        freea         : 1;
    unsigned        freeb         : 1;
    unsigned        freec         : 1;
    unsigned        freed         : 1;
    unsigned        freee         : 1;
    unsigned        research      : 1;  // -r global research mode output
} global_flags;                         // Global flags

/***************************************************************************/
/*  Processing flags, some of which are not bit flags at all               */
/***************************************************************************/

typedef struct proc_flags {
    doc_section     doc_sect;               // which part are we in (FRONTM, BODY, ...
    doc_section     doc_sect_nxt;           // next section (tag already seen)
    unsigned        frontm_seen         : 1;// FRONTM tag seen
    unsigned        start_section       : 1;// start section call done

    unsigned        researchfile        : 1;// research for one file ( -r filename )

    unsigned        fb_document_done    : 1;// true if fb_document() called
    unsigned        fb_position_done    : 1;// first positioning on new page done
    unsigned        page_ejected        : 1;// a page was deliberately ejected (headings)
    unsigned        page_started        : 1;// we have something for the curr page
    unsigned        col_started         : 1;// we have something for the curr page
    unsigned        line_started        : 1;// we have something for current line
    unsigned        just_override       : 1;// current line is to be justified

    unsigned        author_tag_seen     : 1;// remember first :AUTHOR tag
    unsigned        date_tag_seen       : 1;// :DATE is allowed only once
    unsigned        docnum_tag_seen     : 1;// :DOCNUM is allowed only once
    unsigned        index_tag_cw_seen   : 1;// .IX, :I1-3, :IH1-3, :IREF seen
    unsigned        stitle_seen         : 1;// remember first stitle value
    unsigned        title_tag_top       : 1;// :TITLE pre_top_skip used
    unsigned        title_text_seen     : 1;// remember first :TITLE tag text

    unsigned        heading_banner      : 1;// banner replaced for heading (Hn)
    unsigned        goto_active         : 1;// processing .go label
    unsigned        newLevelFile        : 1;// start new include Level (file)
    unsigned        gml_tag             : 1;// input buf starts with GML_char
    unsigned        scr_cw              : 1;// input buf starts with SCR_char
    unsigned        if_cond             : 1;// symbol substitution in if condition
    unsigned        macro_ignore        : 1;// .. in col 1-2
    unsigned        CW_force_sep        : 1;// scr cw line was indented and separator must be recognized
    unsigned        CW_noblank          : 1;// no blank between CW/macro and first operand
    unsigned        CW_sep_ignore       : 1;// ignore scr cw separator
    unsigned        indented_text       : 1;// text was indented
    unsigned        in_macro_define     : 1;// macro definition active
    unsigned        suppress_msg        : 1;// suppress error msg (during scanning)
    unsigned        blanks_allowed      : 1;// blanks allowed (during scanning)
    unsigned        keep_ifstate        : 1;// leave ifstack unchanged for next line
    unsigned        substituted         : 1;// variable substituted in current line
    unsigned        unresolved          : 1;// variable found, but not yet resolved
    unsigned        literal             : 1;// .li is active
    unsigned        concat              : 1;// .co ON if set
    unsigned        cont_char           : 1;// continue char found at end of line
    unsigned        ct                  : 1;// .ct continue text is active
    unsigned        pre_fsp             : 1;// activate fsp if line ended in a space from substitution
    unsigned        fsp                 : 1;// force space in spite of .ct
    unsigned        co_on_indent        : 1;// indent line if line started with a space from substitution
    unsigned        zsp                 : 1;// force no space (used when start position of next text_chars is already set)
    unsigned        as_text_line        : 1;// process text as <text line>
    unsigned        in_figlist_toc      : 1;// process FIGLIST/TOC text as <text line>
    unsigned        force_pc            : 1;// use PC tag processing on text not preceded by a tag or control word
    unsigned        utc                 : 1;// user tag with "continue" is active
    unsigned        in_trans            : 1;// esc char is specified (.ti set x)
    unsigned        reprocess_line      : 1;// unget for current input line
    unsigned        sk_2nd              : 1;// .sk follows blank lines of .sp
    unsigned        sk_co               : 1;// CO OFF and .sk -1 or .sk n, n > 0 done
    unsigned        tag_end_found       : 1;// '.' ending tag found
    unsigned        skips_valid         : 1;// controls set_skip_vars() useage
    unsigned        new_pagenr          : 1;// FIG/heading page number changed
    unsigned        first_hdr           : 1;// first header done
    unsigned        box_cols_cur        : 1;// current BX line had column list
    unsigned        bx_set_done         : 1;// BX SET was done last before current BX line
    unsigned        draw_v_line         : 1;// vertical lines are to be drawn for this BX line
    unsigned        in_bx_box           : 1;// identifies first BX line
    unsigned        no_bx_hline         : 1;// determines if a horizontal line is to be emitted or not
    unsigned        top_line            : 1;// determines if current line is at top of page
    unsigned        vline_done          : 1;// determines if a vertical line was done
    unsigned        keep_left_margin    : 1;// for indent NOTE tag paragraph
    unsigned        skip_blank_line     : 1;// for XMP/eXMP blocks in macros
    unsigned        in_reduced          : 1;// position resulting from IN reduced to left edge of device page
    unsigned        dd_starting         : 1;// DD after break had no text (in next scr_process_break())
    unsigned        para_starting       : 1;// :LP, :P or :PC had no text (in scr_process_break())
    unsigned        para_has_text       : 1;// :LP, :P, :PB or :PC had text (used by PB)
    unsigned        titlep_starting     : 1;// AUTHOR or TITLE had no text (in scr_process_break())
    unsigned        space_fnd           : 1;// last input record ended with a space character
    unsigned        force_op            : 1;// force overprint (used with BX CAN/BX DEL)
    unsigned        op_done             : 1;// overprint done (used for heading page adjustment)
    unsigned        overprint           : 1;// .sk -1 active or not

    unsigned        ix_seen             : 1;// index tag/cw preceded current text (indexing on or not)
    unsigned        post_ix             : 1;// index tag/cw preceded current text (indexing on)

    unsigned        br_done             : 1;// break done after einl_in_inlp set: set font to FONT0 in process_text()
    unsigned        einl_in_inlp        : 1;// inline end tag inside inline phrase
    unsigned        inl_text            : 1;// text just processed was inside an inline phrase
    unsigned        xmp_ut_sf           : 1;// SF in user-defined tag inside XMP block

    unsigned        cc_cp_done          : 1;// CC or CP done; apply current inset to first line only
    unsigned        dd_break_done       : 1;// DD break done (first line of text only)
    unsigned        dd_macro            : 1;// DT/DD were invoked inside a macro
    unsigned        dt_space            : 1;// insert one space after DT text
    unsigned        null_value          : 1;// current symbol has "" or equivalent as its value

    unsigned        dd_space            : 1;// insert one space before DD text
    unsigned        need_dd             : 1;// DT seen; DD must be next tag
    unsigned        need_ddhd           : 1;// DTHD seen; DDHD must be next tag
    unsigned        need_gd             : 1;// GT seen; GD must be next tag
    unsigned        need_li_lp          : 1;// top of list/need LI/LP (OL,SL,UL)
    unsigned        need_tag            : 1;// need tag now, not text
    unsigned        need_text           : 1;// need text now, not tag or cw/macro
    unsigned        no_var_impl_err     : 1;// suppress err_var_not_impl msg
    unsigned        tophead_done        : 1;// tophead symbol set
    unsigned        wrap_indent         : 1;// for index item/reference indent when line breaks

    unsigned        has_aa_block        : 1;// true if device defined :ABSOLUTEADDRESS
    unsigned        ps_device           : 1;// true if device is PS (PostScript)
    unsigned        wh_device           : 1;// true if device is WHELP (help file)

    unsigned        layout              : 1;// within :layout tag and sub tags
    unsigned        lay_specified       : 1;// LAYOUT option or :LAYOUT tag seen
    unsigned        banner              : 1;// within layout banner definition
    unsigned        banregion           : 1;// within layout banregion definition
    unsigned        hx_level            : 3;// 0 - 6  active Hx :layout sub tag
    lay_sub         lay_xxx             : 8;// active :layout sub tag

    ju_enum         justify             : 8;// .ju on half off ...

} proc_flags;                           // processing flags

/***************************************************************************/
/*  Attribute flags; used to catch duplicate/missing required attributes   */
/*  Note: must be zeroed before each use                                   */
/***************************************************************************/

typedef struct attr_flags {
    unsigned    align               : 1;
    unsigned    abstract_string     : 1;
    unsigned    appendix_string     : 1;
    unsigned    binding             : 1;
    unsigned    backm_string        : 1;
    unsigned    body_string         : 1;
    unsigned    bullet              : 1;
    unsigned    bullet_translate    : 1;
    unsigned    bullet_font         : 1;
    unsigned    case_a              : 1;    // using just "case" causes compiler problems
    unsigned    columns             : 1;
    unsigned    contents            : 1;
    unsigned    date_form           : 1;
    unsigned    default_frame       : 1;
    unsigned    default_place       : 1;
    unsigned    delim               : 1;
    unsigned    depth               : 1;
    unsigned    display_heading     : 1;
    unsigned    display_in_toc      : 1;
    unsigned    docnum_string       : 1;
    unsigned    docsect             : 1;
    unsigned    figcap_string       : 1;
    unsigned    file                : 1;
    unsigned    fill_string         : 1;
    unsigned    font                : 1;
    unsigned    frame               : 1;
    unsigned    group               : 1;
    unsigned    gutter              : 1;
    unsigned    hoffset             : 1;
    unsigned    header              : 1;
    unsigned    indent              : 1;
    unsigned    index_delim         : 1;
    unsigned    index_string        : 1;
    unsigned    input_esc           : 1;
    unsigned    ixhead_frame        : 1;
    unsigned    justify             : 1;
    unsigned    left_adjust         : 1;
    unsigned    left_indent         : 1;
    unsigned    left_margin         : 1;
    unsigned    level               : 1;
    unsigned    line_break          : 1;
    unsigned    line_indent         : 1;
    unsigned    line_left           : 1;
    unsigned    max_group           : 1;
    unsigned    note_string         : 1;
    unsigned    number_font         : 1;
    unsigned    number_form         : 1;
    unsigned    number_reset        : 1;
    unsigned    number_style        : 1;
    unsigned    page_eject          : 1;
    unsigned    page_position       : 1;
    unsigned    page_reset          : 1;
    unsigned    para_indent         : 1;
    unsigned    place               : 1;
    unsigned    post_skip           : 1;
    unsigned    pouring             : 1;
    unsigned    pre_lines           : 1;
    unsigned    pre_skip            : 1;
    unsigned    pre_top_skip        : 1;
    unsigned    preface_string      : 1;
    unsigned    refdoc              : 1;
    unsigned    refnum              : 1;
    unsigned    refplace            : 1;
    unsigned    region_position     : 1;
    unsigned    reposition          : 1;
    unsigned    right_adjust        : 1;
    unsigned    right_indent        : 1;
    unsigned    right_margin        : 1;
    unsigned    script_format       : 1;
    unsigned    section_eject       : 1;
    unsigned    see_string          : 1;
    unsigned    see_also_string     : 1;
    unsigned    size                : 1;
    unsigned    skip                : 1;
    unsigned    spacing             : 1;
    unsigned    stop_eject          : 1;
    unsigned    string_font         : 1;
    unsigned    threshold           : 1;
    unsigned    toc_levels          : 1;
    unsigned    top_margin          : 1;
    unsigned    voffset             : 1;
    unsigned    width               : 1;
    unsigned    wrap_indent         : 1;
} attr_flags;                           // attribute flags

/***************************************************************************/
/*  structure for parsed tag attribute names and values                    */
/***************************************************************************/

typedef struct tag_att_val {
    char        *   att_name;
    uint32_t        att_len;
    char        *   val_name;
    uint32_t        val_len;
    bool            val_quoted;
} tag_att_val;

#endif                                  // GTYPE_H_INCLUDED
