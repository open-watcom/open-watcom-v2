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
* Description:  Tests the text file output functions for wgml.
*               In addition to main(), these global functions are implemented:
*                   print_banner()
*                   print_usage()
*               as well as these #define macros:
*                   TEXT_START
*               and these enums:
*                   oc_construct_type
*                   oc_element_type
*                   oc_position
*               and these structs:
*                   oc_bx_element
*                   oc_column
*                   oc_element
*                   oc_fig_element
*                   oc_hpos_list
*                   oc_hpos_t
*                   oc_page
*                   oc_text_element
*                   oc_text_phrase
*               and these variables (the text arrays are not listed):
*                   oc_break
*                   oc_concat
*                   oc_cur_page
*                   oc_cur_post_skip
*                   oc_document_start
*                   oc_element_pool
*                   oc_font_number
*                   oc_hyphen_width
*                   oc_indent
*                   oc_is_paragraph
*                   oc_line_height_zero
*                   oc_max_depth
*                   oc_new_element
*                   oc_new_page
*                   oc_new_section
*                   oc_next_column
*                   oc_old_post_skip
*                   oc_page_left
*                   oc_page_right
*                   oc_page_top
*                   oc_pre_skip
*                   oc_pre_top_skip
*                   oc_script
*                   oc_text_chars_pool
*                   oc_text_line_pool
*               and these local functions:
*                   emulate_input_source()
*                   emulate_layout_page()
*                   emulate_wgml()
*                   oc_add_text_chars_to_pool()
*                   oc_add_text_line_to_pool()
*                   oc_alloc_oc_element()
*                   oc_alloc_text_chars()
*                   oc_alloc_text_line()
*                   oc_intrans()
*                   oc_output_page()
*                   oc_process_line_full()
*                   oc_process_text()
*                   oc_split_text()
*                   oc_tab_position()
*                   oc_text_chars_width()
*                   oc_validate_text_line()
*                   oc_wgml_tabs()
*
* Notes:        The Wiki should be consulted for any term whose meaning is
*               not apparent. This should help in most cases.
*
*               This program uses/tests the binary device library to output
*               a text file. As such, all structs and field names refer to
*               those in "copfiles.h", not the research code.
****************************************************************************/
 
#define __STDC_WANT_LIB_EXT1__ 1
#include <conio.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wgml.h"
#include "banner.h"
#include "common.h"
#include "findfile.h"
#include "gvars.h"
#include "research.h"

/* This is used with a struct whose length can vary. */

#define TEXT_START 32   // text_chars

/* Enum definitions. */

typedef enum {
    oc_bx_box,
    oc_fig_box,
    oc_paragraph,
    oc_title_text,
} oc_construct_type;

typedef enum {
    oc_bx,
    oc_dbox,
    oc_fig,
    oc_hline,
    oc_redundant_aa,
    oc_text,
    oc_vline,
} oc_element_type;

typedef enum {
    oc_center,
    oc_left,
    oc_none,
    oc_right,
} oc_position;

/* Struct definitions. */

struct oc_element; // Forward declaration.

typedef struct {
            uint32_t            depth;
    struct  oc_element    *   first;
} oc_bx_element;

typedef struct {
            uint32_t            depth;
    struct  oc_element    *   first;
} oc_fig_element;

typedef struct {
    uint32_t            count;
    uint32_t            depth;
    text_line       *   first;
} oc_text_element;

typedef struct oc_element {
    struct  oc_element      *   next;
            uint32_t            indent;
            uint32_t            net_skip;
            oc_position         line_position;
            oc_element_type     type;
    union {
            oc_bx_element       oc_bx;
            oc_fig_element      oc_fig;
            oc_text_element     oc_text;
    } element;
} oc_element;

typedef struct {
    uint32_t            cur_depth;
    oc_element      *   first;
    oc_element      *   last;
} oc_column;

typedef struct {
    bool                new_section;
    oc_column           column;
} oc_page;

typedef uint32_t    oc_hpos_t;
  
typedef struct {
    uint16_t            current;
    uint16_t            length;
    oc_hpos_t       *   hpos;
} oc_hpos_list;

typedef struct {
    font_number         font;
    bool                new_record;
    char            *   text;
} oc_text_phrase;
 
/* Local variables. */
 
/* These are used by more than one function. */

static  bool                oc_break                = false;
static  bool                oc_concat               = false;
static  bool                oc_document_start       = false;
static  bool                oc_is_paragraph         = false;
static  bool                oc_new_page             = false;
static  bool                oc_new_section          = false;
static  bool                oc_script               = false;
static  oc_element      *   oc_element_pool         = NULL;
static  oc_element      *   oc_new_element          = NULL;
static  oc_column           oc_next_column          = { 0, NULL, NULL };
static  oc_page             oc_cur_page             = { false, { 0, NULL, NULL } };
static  text_chars      *   oc_text_chars_pool      = NULL;
static  text_line       *   oc_text_line_pool       = NULL;
static  uint8_t             oc_font_number          = 0;
static  uint32_t            oc_cur_post_skip        = 0;
static  uint32_t            oc_hyphen_width         = 0;
static  uint32_t            oc_indent               = 0;
static  uint32_t            oc_line_height_zero     = 0;
static  uint32_t            oc_max_depth            = 0;
static  uint32_t            oc_old_post_skip        = 0;
static  uint32_t            oc_page_left            = 0;
static  uint32_t            oc_page_right           = 0;
static  uint32_t            oc_page_top             = 0;
static  uint32_t            oc_pre_skip             = 0;
static  uint32_t            oc_pre_top_skip         = 0;
 
/* Load the usage text array. */
 
static  char    const   *   usage_text[] = {
"Usage:  outcheck defined-name",
"'defined-name' is the defined name for the device to use",
"'defined-name' must be enclosed in quotes if it contains spaces",
NULL
};
 
/* Load the document text arrays. */
 
static  oc_text_phrase         title[] = {
    { 3, true, "OUTCHECK~ Document " },
    { 0, true, NULL}
};

static  oc_text_phrase         intro_cosc[] = {
    { 0, true, "'script' and '.co on' are in effect. " },
    { 0, true, NULL}
};

static  oc_text_phrase         intro_cowsc[] = {
    { 0, true, "'wscript' and '.co on' are in effect. " },
    { 0, true, NULL}
};

static  oc_text_phrase         intro_xcosc[] = {
    { 0, true, "'script' and '.co off' are in effect. " },
    { 0, true, NULL}
};

static  oc_text_phrase         intro_xcowsc[] = {
    { 0, true, "'wscript' and '.co off' are in effect. " },
    { 0, true, NULL}
};

static  oc_text_phrase         para_font[] = {
    {0, true, "     Parts of this document were copied and modified from the Wiki. " },
    {0, true, "The reason for this was to make it less self-referential. " },
    {0, true, "Ironically, we start with some rather self-referential tests " },
    {0, true, "of font switching: the title was in available font 3; most of " },
    {0, true, "the text is in available font 0; " },
    {0, false, "here is a phrase in " },
    {1, true, "available font 1" },
    {0, false, " and here is a phrase in " },
    {2, true, "available font 2" },
    {0, false, ", neither including spaces before or behind " },
    {0, true, "(the usual situation). The rarer cases have " },
    {0, false, "the highlighted phrase include" },
    {1, true, " the space before" },
    {0, false, " or " },
    {2, false, "the space behind " },
    {0, true, "(which can affect layout if the space widths vary by font) " },
    {0, true, "and, very rarely, perhaps, to start in the mid" },
    {3, false, "dle" },
    {0, false, " of a word. " },
    {0, true, NULL }
};
 
static  oc_text_phrase         para_long[] = {
    {0, true, "These are tests of extremely long words. First, a solid multiline word: aaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbccccccccccccccccccccccccdddddddddddddddddddddddddddeeeeeeeeeeeeeeeeaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbccccccccccccccccccccccccdddddddddddddddddddddddddddeeeeeeeeeeeeeeee " },
    {0, true, "And now an early multi-part multiline word: aaaaaaaaaa" },
    {1, false, "aaaaaaaaaa" },
    {0, false, "bbbbbbbbbbbbbbbbbbbbbbbccccccccccccccccccccccccdddddddddddddddddddddddddddeeeeeeeeeeeeeeeeaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbccccccccccccccccccccccccdddddddddddddddddddddddddddeeeeeeeeeeeeeeee " },
    {0, true, "And now a late multi-part multiline word: aaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbccccc" },
    {1, false, "cccccccccc" },
    {0, false, "cccccccccdddddddddddddddddddddddddddeeeeeeeeeeeeeeeeaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbccccccccccccccccccccccccdddddddddddddddddddddddddddeeeeeeeeeeeeeeee " },
    {0, true, "Now with early tabbing: aaaaa>aaaaaaaaaa>aaaaabbbbbbbbbbbbbbbbbbbbbbbccccccccccccccccccccccccdddddddddddddddddddddddddddeeeeeeeeeeeeeeeeaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbccccccccccccccccccccccccdddddddddddddddddddddddddddeeeeeeeeeeeeeeee " },
    {0, true, "and: aaaaa>aaaaa" },
    {1, false, "aaaaa>aaaaa" },
    {0, false, "bbbbbbbbbbbbbbbbbbbbbbbccccccccccccccccccccccccdddddddddddddddddddddddddddeeeeeeeeeeeeeeeeaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbccccccccccccccccccccccccdddddddddddddddddddddddddddeeeeeeeeeeeeeeee " },
    {0, true, "Next, with late tabbing: aaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbccccccccccccccccccccccccdddddddddddddddddddddddddddeeeeeeeeeeeeeeee>aaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbccccccccccccccccccccccccdddddddddddddddddddddddddddeeeeeeeeeeeeeeee " },
    {0, true, "and: aaaaaaaaaa" },
    {1, false, "aaaaaaaaaa" },
    {0, false, "bbbbbbbbbbbbbbbbbbbbbbbccccccccccccccccccccccccdddddddddddddddddddddddddddeeeeeeeeeeeeeeee>aaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbccccccccccccccccccccccccdddddddddddddddddddddddddddeeeeeeeeeeeeeeee " },
    {0, true, NULL }
};
 
static  oc_text_phrase         para_stop[] = {
    {0, true, "Now we get to the test of the stops. Research suggests that " },
    {0, true, "there are four: '.', '!', '?' and '.', at least when the " },
    {0, true, "criterion is that wgml 4.0 puts two spaces after them when " },
    {0, true, "they are used with wscript/noscript in effect! This is surely " },
    {0, true, "worth examining, is it not? It is a bit surprising, however, " },
    {0, true, "to find that neither ',' nor ';' is a stop; still, what " },
    {0, true, "matters here is what wgml 4.0 does. And putting the stop before " },
    {0, true, "a ')' appears to cancel the effect. (Thus, this will " },
    {0, true, "not be followed by two spaces.) (Nor will this example!) (And " },
    {0, true, "this won't either, will it?) (This one is, of course, rather " },
    {0, true, "unlikely:) in practice. " },
    {0, true, NULL }
};

static  oc_text_phrase         para_subsup[] = {
    {0, true, "Next we test subscripts and superscripts. First we try some " },
    {0, true, "tricks with the single-letter functions: " },
    {0, false, "\xfe\x01subscript\xfe\xff should be subscripted, " },
    {0, true, " and \xfe\x02superscript\xfe\xff should be superscripted. These " },
    {0, true, "should produce an interesting effect: " },
    {0, false, "\xfe\x01subscript\xfe\xff\xfe\x02superscript\xfe\xff. These " },
    {0, true, "should not work: \xfe\x01\xfe\xff"},
    {1, false, "subscript" },
    {0, false, " and \xfe\x02\xfe\xff"},
    {1, false, "superscript" },
    {0, true, " but these should: " },
    {1, false, "\xfe\x01subscript\xfe\xff" },
    {0, false, " and " },
    {1, false, "\xfe\x02superscript\xfe\xff" },
    {0, false, ". The functions " },
    {0, true, "should do no better: \xfe\x01" },
    {1, false, "subscript text" },
    {0, false, "\xfe\xff, \xfe\x02" },
    {1, false, "superscript text" },
    {0, false, "\xfe\xff, \xfe\x01" },
    {1, false, "subscript" },
    {0, false, " text\xfe\xff, \xfe\x02" },
    {1, false, "superscript" },
    {0, false, " text\xfe\xff, \xfe\x01subscript " },
    {1, false, "text" },
    {0, false, "\xfe\xff, and \xfe\x02superscript " },
    {1, false, "text"},
    {0, false, "\xfe\xff. " },
    {0, true, "These should work as intended, however:" },
    {1, false, " \xfe\x01subscript text\xfe\xff" },
    {0, false, " and " },
    {1, false, "\xfe\x02superscript text\xfe\xff." },
    {0, true, NULL }
};

static  oc_text_phrase         para_tab[] = {
    {0, true, "And now we come to wgml tabs. This is a test line with no " },
    {0, false, "wgml tabs at all. " },
    {0, false, ">This>is>a>test>line>which>is>infested>with>wgml>tabs. " },
    {0, true, ">This>is>a>test>line>which>is>infested>with>wgml>tabs. " },
    {0, true, "And this line shows one>tab, two>>tabs, and " },
    {0, false, "three>>>tabs. " },
    {0, true, "Here are tabs>" },
    {1, false, "before and after" },
    {0, false, ">a highlighted phrase. " },
    {0, true, "Finally, here is a set of five     spaces." },
    {0, true, NULL }
};

#if 0 // Needed until the boxes are ready for testing
 
static  oc_text_phrase         para2[] = {
    {0, true, "This box was created using .bx (and several other control " },
    {0, false, "words) and illustrates one of the many possibilites available: " },
    {0, false, NULL }
};
 
static  oc_text_phrase         bx_box[] = {
    {0, true, "%binary(3)>the appropriate graphic appears" },
    {0, true, "%binary1(4)>the appropriate graphic appears" },
    {0, true, "%binary2(5)>the appropriate graphic appears" },
    {0, true, "%binary4(6)>the appropriate graphic appears" },
    {0, false, NULL }
};
 
static  oc_text_phrase         para3[] = {
    {0, true, "This box was created using :FIG., which is in some ways less " },
    {0, false, "flexible than .bx but which has its advantages: " },
    {0, false, NULL }
};

static  oc_text_phrase         fig_box[] = {
    {0, true, "%x_address() returns the current horizontal print position" },
    {0, true, "%y_address() returns the current vertical print position" },
    {0, false, NULL }
};
 
#endif // #if 0
 
/* Local function definitions. */

/* Function oc_add_text_chars_to_pool().
 * Returns the text_chars instances of in_line to the pool.
 *
 * Parameter:
 *      in_line points to the text_line to process.
 *
 * Notes:
 *      This function is currently a lightly-adapted version of function
 *          add_text_chars_to_pool() in gproctxt.c.
 *      This version requires the calling function to set in_line->first and
 *          in_line->last to NULL. It is not entirely clear that this is
 *          either good or necessary.
 */

void oc_add_text_chars_to_pool( text_line * in_line )
{
    /* Ensure in_line->last is not NULL when it is used below. */

    if( in_line->first == NULL ) return;
    if( in_line->last == NULL ) in_line->last = in_line->first;

    /* Return the text_chars instances to the pool */

    in_line->last->next = oc_text_chars_pool;
    oc_text_chars_pool = in_line->first;

    return;
}

/* Function oc_add_text_line_to_pool().
 * Returns the text_line instances of in_line to the pool. This function also
 * returns the text_chars instances in each text_line to the pool.
 *
 * Parameter:
 *      in_line points to the linked list of text_line instances to process.
 *
 * Note:
 *      This version requires the calling function to set whatever was
 *          pointing to these text_lines to NULL. This was done in case
 *          something other than a text oc_element is being used. It is
 *          not clear that this is necessary.
 */

void oc_add_text_line_to_pool( text_line * in_line )
{
    text_line      *   cur_line;

    if( in_line == NULL ) return;

    cur_line = in_line;
    while( cur_line->next != NULL ) {
        oc_add_text_chars_to_pool( cur_line );
        cur_line->first = NULL;
        cur_line->last = NULL;
        cur_line = cur_line->next;
    }
    oc_add_text_chars_to_pool( cur_line );
    cur_line->first = NULL;
    cur_line->last = NULL;

    cur_line->next = oc_text_line_pool;
    oc_text_line_pool = in_line;

    return;
}

/* Function oc_alloc_oc_element().
 * Gets a oc_element from oc_element_pool or allocates a new one; in
 * either case, initializes it. This function has no counterpart in wgml at
 * present.
 *
 * Parameter:
 *      el_type contains the type of oc_element to create.
 *
 * Returns:
 *      A pointer to the text_chars instance.
 */

static oc_element * oc_alloc_oc_element( oc_element_type el_type )
{
    oc_element    *   retval  = NULL;

    if( oc_element_pool == NULL ) {
        retval = mem_alloc( sizeof( oc_element ) );
    } else {
        retval = oc_element_pool;
        oc_element_pool = oc_element_pool->next;
    }
    retval->next = NULL;
    retval->indent = oc_indent;
    retval->net_skip = 0;
    retval->line_position = oc_none;
    retval->type = el_type;

    switch( el_type ) {
    case oc_text:
        retval->element.oc_text.count = 0;
        retval->element.oc_text.depth = 0;
        retval->element.oc_text.first = NULL;
        break;
    case oc_bx:
    case oc_fig:
    case oc_dbox:
    case oc_hline:
    case oc_vline:
    case oc_redundant_aa:
        break;
    default:
        out_msg( "Unknown oc_element type\n" );
        err_count++;
        g_suicide();
    }
    return( retval );
}

/* Function oc_alloc_text_chars().
 * Gets a text_chars from oc_text_chars_pool or allocates a new one; in either
 * case, initializes it. The parameters used are the same as seen in the wgml
 * version in gproctxt.c, although the details are different. In wgml, the
 * function is called from several different locations, which presumably
 * explains the parameters.
 *
 * Parameters:
 *      in_text points to the text to be controlled by the new instance
 *      count contains the number of characters in in_text
 *      font_number contains the font number for the text_chars
 *      type contains the type of processing for the text_chars
 *
 * Returns:
 *      A pointer to the text_chars instance.
 */

static text_chars *oc_alloc_text_chars( char *in_text, size_t count, font_number font, text_type type )
{
    uint32_t        size    = TEXT_START;
    text_chars  *   retval  = NULL;

    if( oc_text_chars_pool == NULL ) {
        if( count > TEXT_START ) {
            size = (( count / TEXT_START ) + 1) * TEXT_START;
        }
        retval = mem_alloc( sizeof( text_chars ) + size );
        retval->length = size;
    } else {
        retval  = oc_text_chars_pool;
        oc_text_chars_pool = oc_text_chars_pool->next;
        if( count > retval->length ) {
            size = (( count / TEXT_START ) + 1) * TEXT_START;
            retval = mem_realloc( retval, sizeof( text_chars ) + size );
            retval->length = size;
        }
    }
    retval->next = NULL;
    retval->prev = NULL;
    retval->font = font;
    retval->type = type;
    retval->x_address = 0;
    retval->width = 0;

    if( in_text != NULL ) {
        memcpy_s(retval->text, count, in_text, count );
        retval->count = count;
    } else {
        retval->count = 0;
        retval->text[0] = '\0';
    }

    return( retval);
}

/* Function oc_alloc_text_line().
 * Gets a text_line from oc_text_line_pool or allocates a new one; in either
 * case, initializes it.
 *
 * Returns:
 *      A pointer to the text_line instance.
 */

static text_line * oc_alloc_text_line( void )
{
    text_line   *   retval  = NULL;

    if( oc_text_line_pool == NULL ) {
        retval = mem_alloc( sizeof( text_line ) );
    } else {
        retval = oc_text_line_pool;
        oc_text_line_pool = oc_text_line_pool->next;
    }
    retval->next = NULL;
    retval->line_height = 0;
    retval->y_address = 0;
    retval->first = NULL;
    retval->last = NULL;

    return( retval);
}

/* Function oc_intrans()
 * Applies input translation, if specified, to the text.
 *
 * Parameters:
 *      data contains the text to process.
 *      len contains the number of characters in text.
 *      font contains the font number to use.
 *
 * Parameters Modified:
 *      text will contain the text with input translation applied.
 *      count will contain the number of characters in the translated text.
 *
 * Note:
 *      The function body is an unaltered copy of intrans() in gproctxt.c.
 */

static void oc_intrans( char * data, uint16_t *len, font_number font )
{
    char    *   ps;                     // source ptr
    char    *   pt;                     // target ptr
    uint32_t    k;

    if( !ProcFlags.in_trans ) {
        return;                         // input translation not active
    }
    k = *len;
    ps = data;
    pt = data;
    for( k = 0; k <= *len; k++ ) {
        if( *ps == in_esc ) {           // translate needed
            ps++;                       // skip escape char
            k++;                        // and count
            *pt = cop_in_trans( *ps, font );   // translate
            ps++;
            pt++;
        } else {
            *pt++ = *ps++;              // else copy byte
        }
    }

    if( pt < ps ) {                     // something translated
        *len -= (ps - pt);              // new length
        *pt = ' ';
    }

    return;
}

/* Function oc_output_page()
 * This function sends the current page to the output device. It also returns
 * the text_line and text_chars instances to their respective pools after they
 * have been sent to the output device.
 *
 * Parameters:
 *
 * Globals Used:
 *      oc_cur_page.column is sent to the output device.
 *
 * Globals Modified:
 *      oc_cur_page.column will be set correctly for the next page.
 *
 * Notes:
 *      This function is intended to provide some idea of how a page might
 *          be output in wgml; the actual code would probably need to be more
 *          complicated.
 *      This function currently computes the actual vertical position, which
 *          makes sense since it is only when a page is finished that this
 *          can be done without having to redo it if one or more lines must be
 *          moved to the next page. However, that also results in a lot of
 *          other processing being done here that could be done earlier if 
 *          each text_line is output as soon as it is finalized.
 */
 
static void oc_output_page( void )
{
    oc_element      *   cur_elem    = NULL;
    oc_element      *   next_elem   = NULL;
    text_line       *   cur_line    = NULL;
    uint32_t            cur_h_address;
    uint32_t            cur_v_address;
    uint32_t            start_depth;
    uint32_t            start_width;

    /* An empty document section can result in this function being invoked
     * when oc_cur_page.column has no contents. This is not an error; however,
     * starting a new document page would be inappropriate.
     */

    if( oc_cur_page.column.first == NULL ) return;

    cur_elem = oc_cur_page.column.first;
    switch( cur_elem->type ) {
    case oc_text:
        if( cur_elem->element.oc_text.first == NULL ) return;
        cur_line = cur_elem->element.oc_text.first;
        start_depth = (cur_elem->net_skip + 1) * cur_line->line_height;
        break;
    case oc_bx:
        /* Implementation: to be done. */
        break;
    case oc_fig:
        /* Implementation: to be done. */
        break;
    default:
        out_msg( "outcheck internal error: incorrect oc_element\n" );
        err_count++;
        g_suicide();
    }
    oc_new_page = true;

    /* Assign the vertical positions to the various elements. While this could
     * be done in the output loop at present, if more than one column is
     * present on a page both will have to have their vertical positions
     * computed because the text will be printed using text_lines which
     * extend across the entire width of the page and which are formed
     * ad-hoc from all columns taken together.
     */

    while( cur_elem != NULL ) {
        while( cur_line != NULL ) {
            if( oc_document_start ) {

                /* NOTE: This may need adjusting when element types other than
                 * text are in use.
                 */

                /* cur_line is the first line in the document. The vertical
                 * position is the same as top-of-page: pre_skip is ignored.
                 * However, fb_position() must be called, possibly twice, to
                 * finish initializing devfuncs.c.
                 * NOTE: the actual computation of start_depth is a guess at
                 * this point.
                 */

                cur_h_address = oc_page_left;
                if( bin_driver->y_positive == 0x00 ) {
                    cur_v_address = oc_page_top - start_depth;
                } else {
                    cur_v_address = oc_page_top + start_depth;
                }

                /* Set the position. */

                fb_position( cur_h_address, cur_v_address );

                /* If this is a :P. and the horizontal address of the first
                 * text_chars is not oc_page_left (that is, the left margin), so
                 * that an indent exists, call fb_position() to set the indent.
                 * NOTE: Non-:P. paragraphs might have indents, if a control
                 * word exists that produces one. This may result in broadening
                 * the meaning of oc_is_paragraph to include non-:P. paragraphs.
                 */

                start_width = cur_line->first->x_address;
                if( oc_is_paragraph && (start_width > oc_page_left) ) {
                    cur_h_address = start_width;
                    fb_position( cur_h_address, cur_v_address );
                }

            } else {

                if( oc_new_page ) {

                    /* cur_line is the first line on a page (which may or may
                     * not be the first page of a new section, but which is
                     * not the first page in the document).
                     * NOTE: This computation may need tweaking.
                     */

                    if( bin_driver->y_positive == 0x00 ) {
                        cur_v_address = oc_page_top - start_depth;
                    } else {
                        cur_v_address = oc_page_top + start_depth;
                    }

                    /* New section/new page processing. */

                    if( oc_cur_page.new_section ) {
                        oc_cur_page.new_section = false;
                        fb_new_section( cur_v_address );
                    } else {
                        fb_document_page();
                    }

                } else {

                    /* cur_line is not the first line on the page. */

                    if( bin_driver->y_positive == 0x00 ) {
                        cur_v_address -= start_depth;
                    } else {
                        cur_v_address += start_depth;
                    }

                }
            }
            oc_document_start = false;
            oc_new_page = false;
            cur_line->y_address = cur_v_address;
            cur_line = cur_line->next;
            if ( cur_line != NULL ) {
                start_depth = cur_line->line_height;
            }
        }

        cur_elem = cur_elem->next;
        if( cur_elem != NULL ) {
            switch( cur_elem->type ) {
            case oc_text:
                cur_line = cur_elem->element.oc_text.first;
                start_depth = (cur_elem->net_skip + 1) * cur_line->line_height;
                break;
            case oc_bx:
                /* Implementation: to be done. */
                break;
            case oc_fig:
                /* Implementation: to be done. */
                break;
            default:
                out_msg( "outcheck internal error: incorrect oc_element\n" );
                err_count++;
                g_suicide();
            }
        }
    }

    /* Now output the page. This is correct for a page with one column. */

    cur_elem = oc_cur_page.column.first;
    while( cur_elem != NULL ) {
        switch( cur_elem->type ) {
        case oc_text:

            cur_line = cur_elem->element.oc_text.first;
            while( cur_line != NULL ) {
                fb_output_textline( cur_line );
                cur_line = cur_line->next;
            }

            /* Return the text_lines to the pool. */

            oc_add_text_line_to_pool( cur_elem->element.oc_text.first );
            cur_elem->element.oc_text.first = NULL;

            break;
        case oc_bx:
            /* Implementation: to be done. */
            break;
        case oc_fig:
            /* Implementation: to be done. */
            break;
        default:
            out_msg( "outcheck internal error: incorrect oc_element\n" );
            err_count++;
            g_suicide();
        }

        /* Return the cur_elem to oc_element_pool. */

        next_elem = cur_elem->next;
        if( oc_element_pool == NULL ) {
            oc_element_pool = cur_elem;
            cur_elem->next = NULL;
        } else {
            cur_elem->next = oc_element_pool;
            oc_element_pool = cur_elem;
        }
        cur_elem = next_elem;
        oc_cur_page.column.first = cur_elem;
    }

    /* Reconfigure oc_cur_page.column as the next page. This must be done
     * every time a page is output, and is best done here.
     */

    oc_cur_page.column.cur_depth = oc_next_column.cur_depth;
    oc_cur_page.column.first = oc_next_column.first;
    oc_cur_page.column.last = oc_next_column.last;
    oc_next_column.cur_depth = 0;
    oc_next_column.first = NULL;
    oc_next_column.last = NULL;

    return;
}

/* Function oc_process_line_full().
 * This function processes lines that are full, either because they do not have
 * enough room for the next word in the input text or because they are followed
 * by a break. This versions will, when implemented, add them to a static
 * global text oc_element and perform other actions.
 *
 * Parameters:
 *      in_line points to the text_line to be processed.
 *      justify provides the ability to prohibit justification even if
 *          justification is on.
 *
 * Note:
 *      Function process_line_full() in gproctxt.c was used as a starting-
 *          point for this function, but this is not a copy.
 */

static void oc_process_line_full( text_line * in_line, bool justify )
{
    static  text_line   *   line_ptr    = NULL;

            oc_element  *   cur_elem    = NULL;
            oc_element  *   save_elem   = NULL;
            text_chars  *   cur_chars   = NULL;
            text_line   *   cur_line    = NULL;
            text_line   *   last_line   = NULL;
            uint32_t        cur_depth   = 0;
            uint32_t        line_count  = 0;
            uint32_t        offset      = 0;
            uint32_t        title_width = 0;

    /* This should be correct. */

    if( in_line == NULL ) return;

    /* As should this. */

    if( in_line->first == NULL ) return;

    cur_elem = oc_cur_page.column.last;

    /* oc_process_text() aligns text_lines as if oc_left were in effect.
     * If oc_left is not in effect, adjustments must be made.
     */

    switch( cur_elem->line_position ) {
    case oc_center:
        switch( cur_elem->type ) {
        case oc_text:

            /* The computation shown produces the same result as wgml 4.0, but
             * not the result described in the Reference Manual. 
             */

            /* Compute the total width of the line, that is, the width of
             * the text that will be output (internal spacing included).
             */

            cur_chars = in_line->last;
            title_width = cur_chars->x_address + cur_chars->width;
            title_width -= in_line->first->x_address;

            offset = (oc_page_right - title_width) / 2;
            offset -= oc_page_left;

            /* Update each text_chars instance. */

            cur_chars = in_line->first;
            while( cur_chars != NULL) {
                cur_chars->x_address += offset;
                cur_chars = cur_chars->next;
            }

            break;
        case oc_bx:
            /* Implementation: to be done. */
            break;
        case oc_fig:
            /* Implementation: to be done. */
            break;
        default:
            out_msg( "outcheck internal error: incorrect oc_element_type\n" );
            err_count++;
            g_suicide();
        }
        break;
    case oc_left:
        /* Nothing to do? see below. */
        break;
    case oc_none:

        /* Do nothing; the element does not have the relevent attribute. */

        break;
    case oc_right:
        /* Not implemented yet. */
        break;
    default:
        out_msg( "outcheck internal error: incorrect oc_position\n" );
        err_count++;
        g_suicide();
    }

    /* Note: it is not known how the various tags which have a page_position
     * :LAYOUT attribute interact with justification. It might be as easy as
     * setting justify to "false", or it might require adjustment of the
     * variables controlling how justification is done. 
     */

    /* outcheck.c does not attempt justification; however, it would occur
     * inside the following if() statement if parameter justify is true
     * and it is otherwise called for.
     */

    if( justify ) {}

    /* Add in_line to the current element. */

    if( line_ptr == NULL ) {
        switch( cur_elem->type ) {
        case oc_text:
            cur_elem->element.oc_text.first = in_line;
            break;
        case oc_bx:
            /* Implementation: to be done. */
            break;
        case oc_fig:
            /* Implementation: to be done. */
            break;
        default:
            out_msg( "outcheck internal error: incorrect oc_element\n" );
            err_count++;
            g_suicide();
        }
        line_ptr = in_line;
    } else {
        line_ptr->next = in_line;
        line_ptr = line_ptr->next;
    }
    cur_elem->element.oc_text.count++;
    cur_elem->element.oc_text.depth += in_line->line_height;

    /* Break processing. */

    cur_depth = oc_cur_page.column.cur_depth;
    if( oc_break ) {    

        /* Full page processing. */

        line_ptr = NULL;
        cur_depth += cur_elem->net_skip * oc_line_height_zero;
        if( (cur_depth + cur_elem->element.oc_text.depth) > oc_max_depth ) {

            /* Determine which line is the last to fit on this page. */

            cur_line = cur_elem->element.oc_text.first;

            /* cur_line cannot be NULL at this point because we cannot get here
             * unless in_line was non-NULL and was added to oc_cur_page.column.last.
             */

            /* This would be a good place to check for "widows". The
             * thought is that line_count and cur_depth can adjusted as
             * needed to move lines to the next page to prevent a "widow".
             */

            line_count = 0;
            while( cur_line != NULL ) {
                if( (cur_depth + cur_line->line_height) >
                                                oc_max_depth ) break;
                line_count++;
                cur_depth += cur_line->line_height;
                last_line = cur_line;
                cur_line = cur_line->next;
            }

            /* cur_line, as noted, should not be NULL. However, if the entire
             * element must be moved to next_column intact, then last_line
             * will be NULL.
             */

            if( cur_line == NULL ) {
                    
                /* If cur_line is NULL, then all lines will fit. While
                 * not an error as such, it should still never happen.
                 */

                out_msg( "All text_lines fit, unexpectedly. \n" );
            } else {

                if( last_line == NULL ) {

                    /* Transfer cur_elem intact to oc_next_column.
                     * First find the element before cur_elem and have
                     * save_elem point to it.
                     */

                    save_elem = oc_cur_page.column.first;

                    if( save_elem == cur_elem ) {

                        /* This is most unlikely -- at least with text-only. */

                        out_msg( "internal error: page finishing.\n" );
                        err_count++;
                        g_suicide();
                    }

                    while( save_elem->next != cur_elem ) {
                        save_elem = save_elem->next;
                    }
                    save_elem->next = NULL;

                    /* Add cur_elem to oc_next_column and set variables. */

                    if( oc_next_column.first == NULL ) {
                        oc_next_column.first = cur_elem;
                        oc_next_column.last = oc_next_column.first;
                    } else {
                        oc_next_column.last->next = cur_elem;
                        oc_next_column.last = oc_next_column.last->next;
                    }

                    if( oc_next_column.first == oc_next_column.last ) {

                        /* Finalize net_skip for cur_elem.
                         * Since cur_elem is the first element on the page,
                         * only oc_pre_top_skip is used.
                         * oc_old_post_skip is updated for possible use with the
                         * next element, but is not used in finalizing net_skip
                         * because it refers to an element which occurred at the
                         * end of the prior page.
                         * NOTE: this only works for now. When :FIG is done, and
                         * cur_elem may not be the first element on the new
                         * page, some means of recovering the appropriate
                         * post_skip will need to be found.
                         */

                        switch( cur_elem->type ) {
                        case oc_text:
                            cur_elem->net_skip = oc_pre_top_skip;
                            oc_old_post_skip = oc_cur_post_skip;
                            break;
                        case oc_bx:
                            /* Implementation: to be done. */
                            break;
                        case oc_fig:
                            /* Implementation: to be done. */
                            break;
                        default:
                            out_msg( "outcheck internal error: incorrect oc_element\n" );
                            err_count++;
                            g_suicide();
                        }
                    }

                } else {

                    /* Split cur_elem to fit part of it onto the page. */

                    /* Add a new text element to oc_next_column and set
                     * variables.
                     */

                    if( oc_next_column.first == NULL ) {
                        oc_next_column.first = oc_alloc_oc_element( oc_text );
                        oc_next_column.last = oc_next_column.first;
                    } else {
                        oc_next_column.last->next = oc_alloc_oc_element( oc_text );
                        oc_next_column.last = oc_next_column.last->next;
                    }
                    save_elem = oc_next_column.last;
                    cur_elem->next = NULL;
                    last_line->next = NULL;

                    if( oc_next_column.first == oc_next_column.last ) {

                        /* Finalize net_skip for save_elem.
                         * Since save_elem is the first element on the page,
                         * only oc_pre_top_skip is used.
                         * oc_old_post_skip is updated for possible use with the
                         * next element, but is not used in finalizing net_skip
                         * because it refers to an element which occurred at the
                         * end of the prior page.
                         * NOTE: this only works for now. When :FIG is done, and
                         * save_elem may not be the first element on the new
                         * page, some means of recovering the appropriate
                         * post_skip will need to be found.
                         */

                        switch( save_elem->type ) {
                        case oc_text:
                            save_elem->net_skip = oc_pre_top_skip;
                            oc_old_post_skip = oc_cur_post_skip;
                            break;
                        case oc_bx:
                            /* Implementation: to be done. */
                            break;
                        case oc_fig:
                            /* Implementation: to be done. */
                            break;
                        default:
                            out_msg( "outcheck internal error: incorrect oc_element\n" );
                            err_count++;
                            g_suicide();
                        }
                    }

                    /* Update oc_cur_elem to reflect its new contents. */

                    cur_elem->element.oc_text.count = line_count;
                    cur_elem->element.oc_text.depth = cur_depth;

                    /* Update save_elem to reflect its contents. cur_line
                     * still heads the linked list of text_line instances
                     * in save_elem.
                     */

                    save_elem->element.oc_text.first = cur_line;
                    while( cur_line != NULL) {
                        save_elem->element.oc_text.count++;
                        save_elem->element.oc_text.depth += cur_line->line_height;
                        cur_line = cur_line->next;
                    }
                    oc_next_column.cur_depth += save_elem->element.oc_text.depth;
                }

                /* Finalize the current page. */

                oc_cur_page.column.cur_depth += cur_elem->element.oc_text.depth;
            }

            /* Print the current page. This also reinitializes it with the
             * contents of oc_next_column.
             */

            oc_output_page();

        } else {
            oc_cur_page.column.cur_depth += cur_elem->net_skip * oc_line_height_zero;
            oc_cur_page.column.cur_depth += cur_elem->element.oc_text.depth;
        }
    }

    return;
}

/* Function oc_text_chars_width().
 * Returns the width of text up to the first tab stop.
 *
 * Parameters:
 *      text contains the text to process.
 *      count contains the number of characters in text.
 *      font contains the font to use in computing the width.
 *
 * Returns:
 *      the width of the text up to the first tab stop.
 *
 * Notes:
 *      This is a convenience function wrapping cop_text_width(), which
 *          ignores embedded tab stops and so can be used in contexts where
 *          tab stops are to be ignored, such as the w' and 'width functions.
 *      Testing shows that an initial tab stop, if any, is not expanded at
 *          this time; indeed, an initial tab stop appears to cause the token
 *          to be treated as having a width of "0".
 */

static uint32_t oc_text_chars_width( char *text, uint32_t count, font_number font )
{
    uint32_t    i;
    uint32_t    cur_count   = 0;
    uint32_t    retval      = 0;

    for( i = 0; i < count; i++ ) {
        if( (text[i] == '\t') || (text[i] == tab_char) ) {
            break;
        }
        cur_count++;   
    }

    if( cur_count != 0 ) {
        retval = cop_text_width( text, cur_count, font );
    }

    return( retval );
}

/* Function oc_tab_position().
 * Determines the position of a (default) wgml tab stop.
 *
 * Parameter:
 *      cur_pos contains the current horizontal position on the line.
 *
 * Returns:
 *      the horizontal position of the next tab stop after cur_pos.
 */

static uint32_t oc_tab_position( uint32_t cur_pos )
{
    int                 i;
    uint32_t            last_tab;
    uint32_t            req_count;
    uint32_t            req_length;
    uint32_t            req_width;
    uint32_t            retval      = 0;

    /* Resize tab_list if necessary. */

    last_tab = def_tabs.tabs[def_tabs.current - 1].column;
    if( cur_pos > last_tab ) {

        /* Compute the minimum number of additional tab stops needed. */

        req_width = cur_pos - last_tab;
        req_count = (req_width / inter_tab) + 1;

        /* See if additional space is needed. */

        if( (def_tabs.tabs[def_tabs.current - 1].column + req_count) > def_tabs.length ) {

            /* Compute the new size. The intent is to add enough tabs not
             * only for the current value, but for up to TAB_COUNT - 1 more.
             */

            req_count /= TAB_COUNT;
            req_count++;

            req_length = def_tabs.length + (req_count * TAB_COUNT);
            def_tabs.tabs = mem_realloc( def_tabs.tabs, req_length * sizeof( tab_stop ) );
            def_tabs.length = req_length;
        }

        /* Set tabs up to the last available position in the hpos array. */

        for( i = def_tabs.current; i < def_tabs.length; i++ ) {
            def_tabs.tabs[i].column = def_tabs.tabs[i - 1].column + inter_tab;
            def_tabs.tabs[i].fill_char = ' ';
            def_tabs.tabs[i].alignment = al_left;
        }
        def_tabs.current = def_tabs.length;
    }

    /* Skip all tab stops to the left of cur_pos. */

    for( i = 0; i < def_tabs.current; i++ ) {
        if( cur_pos < def_tabs.tabs[i].column ) {
            retval = def_tabs.tabs[i].column;
            break;
        }
    }

    /* This would be a good point to preserve the value of the tab position
     * in a variable that the justification function can use as the left
     * boundry of the text to be justified.
     */

    return( retval );
}

/* Function oc_wgml_tabs().
 * This function converts a text_chars instance into a doubly-linked list
 * of text_chars instances reflecting the presence of wgml tabs. 
 *
 * Parameter:
 *      in_chars contains a pointer to the text_chars instance.
 *
 * Returns:
 *      a pointer to the last text_chars instance in the doubly-linked list.
 *
 * Note:
 *      If in_chars contains no wgml tabs, it is not altered, and the
 *          pointer returned will be the same as in_chars.
 */

static text_chars * oc_wgml_tabs( text_chars * in_chars )
{
    uint32_t        i;
    text_chars      *cur_chars      = in_chars;
    text_chars      *retval         = in_chars;
    char            *cur_text       = in_chars->text;
    uint32_t        count           = in_chars->count;
    uint32_t        cur_count       = 0;
    uint32_t        cur_h_address   = in_chars->x_address;
    uint32_t        start;

    /* The first word is to be left in in_chars. */

    for( i = 0; i < count; i++ ) {
        if( (cur_text[i] == '\t') || (cur_text[i] == tab_char) ) {
            break;
        }
        cur_count++;
    }

    /* If cur_count == count, then there are no wgml tabs in in_chars. */

    if( cur_count == count ) {
        return( retval );
    }

    /* in_chars is to contain any text before the first wgml tab. If there
     * is none, then it will become an extra text_chars preserving the spacing
     * after the preceding text_chars, except at the start of line, in which
     * case it will contain the text after the first wgml tab (up to the next
     * wgml tab) or be an empty text_chars at that tab stop.
     * This is what wgml 4.0 does.
     * Note: in_chars->width must have been set by oc_text_chars_width().
     */

    if( (cur_h_address == oc_page_left) && (cur_count == 0) ) {

        /* in_chars is at the left margin and starts with the first wgml tab
         * stop. Find the positon of that tab, and update cur_h_address and
         * in_chars.
         */

        cur_h_address = oc_tab_position( cur_h_address - oc_page_left ) + oc_page_left;
        in_chars->x_address = cur_h_address;

        /* Move to the next character and check it. */

        i++;
        if( (cur_text[i] == '\t') || (cur_text[i] == tab_char) ) {

            /* If the second character is also a wgml tab, in_chars becomes
             * an empty text_chars.
             */

            in_chars->count = 0;

        } else {

            /* If the second character is not a wgml tab, then the word
             * following the first tab is controlled by in_chars.
             */

            start = i;
            for( ; i < count; i++ ) {
                if( (cur_text[i] == '\t') || (cur_text[i] == tab_char) ) {
                    break;                
                }
            }

            /* i is sitting on the next wgml tab, or one byte past the end of
             * the original text. start contains the starting position of the
             * text to be controlled by in_chars. The wgml tab character
             * itself must be removed from the front of the text.
             */

            cur_count = i - start;
            memmove_s( &in_chars->text[0], cur_count, &in_chars->text[start], cur_count);
            in_chars->count = cur_count;
            in_chars->width = cop_text_width( in_chars->text, in_chars->count, in_chars->font );
            cur_h_address += in_chars->width;
        }

    } else {

        /* Either this is not the start of the line or in_chars->text starts
         * with a non-tab character. Adjust in_chars to control the first
         * word. The x_address value is already correct; cur_h_address needs
         * to be updated if the text_chars is not empty.
         */

        in_chars->count = cur_count;
        if( in_chars->width != 0 ) {
            cur_h_address += in_chars->width;
        }
    }

    if( i < count ) {

        /* i is sitting on the first wgml tab following the first word:
         * update cur_h_address, increment i to the next character and save
         * the resulting position to process the rest of the original text.
         */

        cur_h_address = oc_tab_position( cur_h_address - oc_page_left ) + oc_page_left;
        i++;
        start = i;
        for( i; i < count; i++ ) {

            /* Get the next word. */

            for( i; i < count; i++ ) {
                if( (cur_text[i] == '\t') || (cur_text[i] == tab_char) ) {
                    break;
                }
            }                

            /* i is sitting on either the tab after the current word or one
             * position past the end of the original text, start contains the
             * starting position of the current word: move it to a new
             * text_chars instance and hook it into the list.
             */

            cur_count = i - start;
            retval = oc_alloc_text_chars( &cur_text[start], cur_count, in_chars->font, in_chars->type );
            retval->x_address = cur_h_address;
            cur_chars->next = retval;
            retval->prev = cur_chars;
            cur_chars = cur_chars->next;
            retval->width = cop_text_width( retval->text, retval->count, in_chars->font );
            cur_h_address += retval->width;

            /* Set up for the next word. */

            start = i + 1;
            cur_h_address = oc_tab_position( cur_h_address - oc_page_left ) + oc_page_left;
        }

        /* If the last character in the original text was a tab, add an empty
         * text_chars.
         */

        i = count - 1;
        if( (cur_text[i] == '\t') || (cur_text[i] == tab_char) ) {
            retval = oc_alloc_text_chars( NULL, 0, in_chars->font, in_chars->type );
            retval->x_address = cur_h_address;
            cur_chars->next = retval;
            retval->prev = cur_chars;
            cur_chars = cur_chars->next;
        }
    }

    return( retval );
}

/* Function oc_split_text().
 * Splits a text_chars instance into two in such a way that the text controlled
 * by the original instance ends with the last character that will be
 * entirely to the left of limit.
 *
 * Parameters:
 *      in_chars contains the text_chars instance to split.
 *      limit contains the right limit to split in_chars at.
 *
 * Parameter Modified:
 *      in_chars will now fit into the space specified.
 *
 * Returns:
 *      The number of characters in in_chars that will not fit.
 *
 * Notes:
 *      If the device has variable-width fonts, it is possible for there to
 *          be space at the end of the line but not enough space for even the
 *          first character in in_chars->text to fit.
 *      Also, if the test which determined that in_chars did not fit was done
 *          with a larger limit than the value passed (for example, when
 *          hyphenation is being done), then it is possible that only the
 *          hyphen would fit, even with a character device.
 *      The value returned is a count rather than a text_chars * because, if
 *          no characters will fit, the result of returning a text_chars *
 *          would be to make in_chars an empty text_chars instance which has
 *          no use and so is not needed. It is more efficient for the calling
 *          function to react to this situation by putting all of in_chars
 *          onto the next line.
 */

static uint32_t oc_split_text( text_chars * in_chars, uint32_t limit )
{
    uint32_t    retval;
    uint32_t    test_count;
    uint32_t    test_limit;
    uint32_t    test_width;

    if( limit < in_chars->x_address ) {

        /* It is not possible that any characters will fit. */

        retval = in_chars->count;

    } else {

        /* It is possible that some characters might fit. */

        test_limit = limit - in_chars->x_address;   
        if( in_chars->width > test_limit ) {

            /* A more exact computation is needed. */

            test_count = test_limit / wgml_fonts[in_chars->font].default_width;
            test_width = cop_text_width( in_chars->text, test_count, in_chars->font );

            if( test_width != test_limit ) {

                /* Not all characters will fit. */
                
                if( test_width < test_limit ) {
                    while( test_width < test_limit ) {
                        test_count++;
                        test_width = cop_text_width( in_chars->text, test_count, in_chars->font );
                    }
                } else {
                    while( test_width > test_limit ) {
                        test_count--;
                        test_width = cop_text_width( in_chars->text, test_count, in_chars->font );
                    }
                }

                /* Do not update in_chars if either no characters will fit in
                 * the space specified or if all characters will fit.
                 * Note: Testing showed that both conditions could occur.
                 */

                if( test_count == 0 ) {

                    /* No characters will fit. */

                    retval = in_chars->count;

                } else if( test_count == in_chars->count ) { 

                    /* All characters will fit. */

                    retval = 0;

                } else {

                    /* Some characters will fit: finalize retval and in_chars. */

                    retval = in_chars->count - test_count;
                    in_chars->count = test_count;
                    in_chars->width = test_width;
                }
            } else {

                /* test_count chars will fit exactly. */

                retval = in_chars->count - test_count;
                in_chars->count = test_count;
                in_chars->width = test_width;
            }
        } else {

            /* The entire in_chars->text will fit, with room to spare. */

            retval = 0;
        }
    }

    return( retval );
}

/* Function oc_validate_text_line.
 * This function validates a text_line instance in the sense that it tests
 * that the x_address and prev values in the text_chars instances are correct.
 *
 * Parameter:
 *      in_line contains the text_line instance to check.
 *
 * Returns:
 *      true if each successive text_chars is properly positioned.
 *      false if an improperly position text_chars is found.
 *
 * Notes:
 *      This is a diagnostic function, needed only because, by using
 *          oc_page instances, it is impossible to trace back from the
 *          code detecting this error in devfuncs.c to the line creating
 *          the problem here. It is done as a function to simplify its use.
 *      A "correct" x_address is defined as:
 *          in_line->first->x_address (any value);
 *          for any subsequent text_chars instance S:
 *              S->x_address >= S.prev->x_address + S.prev->width
 *          that is, S must start after S.prev ends, although there need not
 *              be any spaces in between.
 */

static bool oc_validate_text_line( text_line * in_line )
{
    bool            retval      = true;
    text_chars  *   cur_chars   = NULL;

    if( in_line != NULL ) {
        if( in_line->first != NULL ) {
            cur_chars = in_line->first;

            while( cur_chars->next != NULL ) {
                cur_chars = cur_chars->next;

                /* This actually occurred in testing! Since cur_chars is,
                 * at earliest, the_line->first->next, cur_chars->prev
                 * should never be NULL.
                 * In fact, this was the test that failed when this function
                 * was first used.
                 */

                if( cur_chars->prev == NULL ) {
                    retval = false;
                    break;
                }

                /* This is the test originally intended. */

                if( cur_chars->x_address < (cur_chars->prev->x_address + cur_chars->prev->width) ) {
                    retval = false;
                    break;
                }
            }
        }
    }

    return( retval );
}

/* Function oc_process_text().
 * This function produces text_line instances from a single input buffer. This
 * may be as simple as adding a few words to an existing text_line or finishing
 * the current text_line, calling oc_process_line_full(), and starting another.
 *
 * Parameters:
 *      input_text contains a pointer to the input buffer.
 *      font_number contains the font number to be used.
 *      type contains the processing type for the phrase.
 *
 * Globals Used:
 *      oc_break indicates whether or not a break occurred before the current
 *          buffer.
 *
 * Globals Changed:
 *      oc_break will be "false" on exit.
 *
 * Notes:
 *      This function is intended to explore methods for converting input text 
 *          which has been finalized except for input translation and tabbing
 *          to a text_line instance. It is intended to mimic features of
 *          parse_text() in gproctxt.c. 
 *      The first text_chars of the first text_line in each oc_element will be
 *          positioned at the location given by oc_page_left plus oc_indent.
 *          The first text_chars of subsequent text_lines in the same
 *          oc_element will be positioned at the location given by page_left.
 */

static void oc_process_text( char * input_text, font_number font )
{
    static  bool            tabbing         = false;
    static  size_t          increment       = 0;
    static  size_t          spaces          = 0;
    static  text_type       cur_type        = norm;
    static  font_number     old_font        = 0;
    static  uint32_t        cur_h_address   = 0;
    static  text_line   *   the_line        = NULL;

            bool            ws_spc_done     = false;
            char            ch;
            char        *   token_start     = 0;
            size_t          count           = 0;
            size_t          space_width     = 0;
            text_chars  *   hyphen_chars    = NULL;
            text_chars  *   next_chars      = NULL;
            text_chars  *   save_chars      = NULL;
            uint32_t        cur_height      = 0;
            uint32_t        offset          = 0;
            uint32_t        old_count       = 0;

    /* These invariants should hold on entry:
     * If oc_new_section is true:
     *      It is the start of new document section.
     *      the_line->last should be NULL.
     *      increment should be 0.
     *      spaces should be 0.
     *      tabbing should be false.
     * otherwise, this is the start of a new phrase:
     *      the_line->first points to a linked list of text_chars instances.
     *      the_line->last points to the last text_chars instance in the_line.
     *      for each text_chars which is part of the_line, prev is null if
     *          it is the first text_chars instance in the_line; otherwise, it
     *          points to the text_chars instance such that prev->next points
     *          to that text_chars.
     *      increment should contain the width of next_chars.
     *      spaces contains the number of spaces (if any) before this phrase.
     *      tabbing will be true if and only if the last character of the
     *          preceding phrase was a wgml tab.
     * If input_text is NULL, then oc_break should be true.
     */

    /* This will keep the compiler happy and, who can say, might be helpful. */

    if( !oc_validate_text_line( the_line ) ) out_msg( "Gotcha!\n");

    /* Make sure we have a text_line. */

    if( the_line == NULL ) the_line = oc_alloc_text_line();

    /* Detect the start of a new section. */

    if( oc_new_section == true ) {

        /* If there is no input_text, do nothing: this section will be done
         * when input_text is not NULL.
         */

        if( input_text == NULL ) return;

        /* Ensure proper starting values for the new section. */

        cur_h_address = oc_page_left + oc_indent;
        increment = 0;
        spaces = 0;
        tabbing = false;

        /* Enable new section processing in oc_output_page(). */

        oc_cur_page.new_section = true;
        oc_new_section = false;

        /* Finalize net_skip for oc_new_element.
         * Since this is the start of a page, only oc_pre_top_skip is used.
         * oc_old_post_skip is updated for possible use with the next element,
         * but is not used in finalizing net_skip because it refers to an
         * element which occurred at the end of the prior page.
         */

        switch( oc_new_element->type ) {
        case oc_text:
            oc_new_element->net_skip += oc_pre_top_skip;
            oc_old_post_skip = oc_cur_post_skip;
            break;
        case oc_bx:
            /* Implementation: to be done. */
            break;
        case oc_fig:
            /* Implementation: to be done. */
            break;
        default:
            out_msg( "outcheck internal error: incorrect oc_element\n" );
            err_count++;
            g_suicide();
        }

        /* Attach oc_new_element to oc_cur_page.column. */

        if( oc_cur_page.column.first == NULL ) {
            oc_cur_page.column.first = oc_new_element;
            oc_cur_page.column.last = oc_cur_page.column.first;
        } else {
            oc_cur_page.column.last->next = oc_new_element;
            oc_cur_page.column.last = oc_cur_page.column.last->next;
        }
        oc_new_element = NULL;

        /* Since the_line is empty, it follows that break processing is not
         * appropriate, even if oc_break is true.
         */

        oc_break = false;
    }

    /* Break processing. */

    if( oc_break ) {

        /* Submit the_line with justification suppressed.
         * Note: the_line is set to NULL here so that, at the end of document
         * output, a final text_line, which will never be used, will not be
         * created.
         */

        if( !oc_validate_text_line( the_line ) ) out_msg( "Gotcha!\n");
        oc_process_line_full( the_line, false );
        the_line = NULL;

        /* If there is no input_text, this was the last line on the last page
         * of a section (or document). There is, then, no point in continuing.
         * Note: if this is not the end of the document, the_line will be
         * initialized on the next call to this function.
         */

        if( input_text == NULL ) return;

        /* Allocate a new text_line. */

        the_line = oc_alloc_text_line();

        /* Reset variables for use with the new line. If the above line is
         * triggered, they will be reset when the function is called next
         * as a new section will be starting.
         */

        if( oc_is_paragraph ) {
            cur_h_address = oc_page_left + oc_cur_page.column.last->indent;
        } else {
            cur_h_address = oc_page_left;
        }
        increment = 0;
        spaces = 0;
        tabbing = false;

        /* Set the line_height to its initial value. */

        the_line->line_height = oc_line_height_zero;

        /* Finalize net_skip for oc_new_element.
         * Since this is not the start of a page, oc_old_post_skip, oc_pre_skip,
         * and oc_pre_top_skip are used.
         * oc_old_post_skip is updated for possible use with the next element.
         * NOTE: this element's net_skip may be reset later if it turns out to be
         * the first element on a new page.
         */

        switch( oc_new_element->type ) {
        case oc_text:

            /* This may need to be redone in future. */

            if( oc_concat || oc_is_paragraph ) {
                oc_new_element->net_skip += oc_old_post_skip;
                oc_new_element->net_skip += oc_pre_skip;
                oc_new_element->net_skip += oc_pre_top_skip;
            } else {
                oc_new_element->net_skip = 0;
            }    
            oc_old_post_skip = oc_cur_post_skip;
            break;
        case oc_bx:
            /* Implementation: to be done. */
            break;
        case oc_fig:
            /* Implementation: to be done. */
            break;
        default:
            out_msg( "outcheck internal error: incorrect oc_element\n" );
            err_count++;
            g_suicide();
        }

        /* Attach oc_new_element to oc_cur_page.column. */

        if( oc_cur_page.column.first == NULL ) {
            oc_cur_page.column.first = oc_new_element;
            oc_cur_page.column.last = oc_cur_page.column.first;
        } else {
            oc_cur_page.column.last->next = oc_new_element;
            oc_cur_page.column.last = oc_cur_page.column.last->next;
        }
        oc_new_element = NULL;

        /* Break processing is done. */

        oc_break = false;
    }

    /* This should not happen, but it is innocuous. */

    if( input_text == NULL ) return;

    /* Since spaces will only be greater than zero at this point if the
     * prior phrase ended with one or more spaces, this would be a good
     * place to reset spaces to zero if control word .ct has been found
     * following the last phrase.
     */

    /* Now add text_chars to the_line based on the contents of input_text. */

    cur_height = 0;
    while( *input_text != '\0' ) {

        /* These invariants should hold at the top of this loop:
         * the_line should never be NULL.
         * If the_line->first is NULL:
         *      It is the start of a new paragraph (possibly a new section).
         *      cur_chars should be NULL.
         *      next_chars should be NULL.
         *      pre_chars should be NULL.
         *      increment should be 0.
         *      spaces should be 0.
         *      tabbing should be false.
         * If the_line->first is not NULL:
         *      It is the start of a new phrase.
         *      the_line->last points to the last text_chars instance in
         *          the_line.
         *      for each text_chars which is part of the_line, prev is null if
         *          it is the first text_chars instance in the_line; otherwise,
         *          it points to the text_chars instance such that prev->next
         *          points to that text_chars.
         *      increment should be 0.
         *      spaces may be 0 or may be > 0.
         *      tabbing will be true if and only if the last character of the
         *          preceding phrase was a wgml tab.
         */

        if( spaces != 0 ) {

            /* Since the_line->first will be NULL only when a new section or
             * break was involved, and since spaces is zeroed in those cases,
             * the_line->first cannot be NULL.
             */

            if( oc_script ) {

                /* "script" is in effect: an empty text_chars will be needed
                 * if the font used by the previous phrase differs from that
                 * used by the current phrase.
                 */

                if( old_font != font ) {

                    /* Obtain the text_chars instance and link it in. */

                    the_line->last->next = oc_alloc_text_chars( NULL, 0, old_font, cur_type );
                    the_line->last->next->prev = the_line->last;
                    the_line->last = the_line->last->next;

                    /* update the horizontal position and use it to set
                     * the_line->last->x_address.
                     */

                    space_width = spaces * wgml_fonts[old_font].spc_width;
                    cur_h_address += space_width;
                    the_line->last->x_address = cur_h_address;

                    /* Reset spaces and adjust the line_height if necessary. */

                    spaces = 0;
                    cur_height = wgml_fonts[old_font].line_height;
                    if( the_line->line_height < cur_height ) {
                        the_line->line_height = cur_height;
                    }
                }

            } else {

                /* "wscript" (or "noscript") is in effect: compute space_width
                 * and set the related variables.
                 * NOTE: this uses old_font, whether it is the same as
                 * font_number or not.
                 */

                space_width = wgml_fonts[old_font].spc_width;

                ch = the_line->last->text[the_line->last->count - 1];
                if( ch == '.' || ch == '!' || ch == '?' || ch == ':' ) {
                    space_width *= 2;
                }

                spaces = 0;
                tabbing = false;
                ws_spc_done = true;
            }
        }

        /* Reset old_font for the next phrase. */

        old_font = font;

        /* Pass over nay spaces before the next token. If ws_spc_done is true,
         * skip them; otherwise, count them.
         */

        while( *input_text != '\0' ) {
            if( *input_text != ' ' ) break;
            if( !ws_spc_done ) {
                spaces++;
            }
            input_text++;
        }

        /* If the end of the phrase is next, then the phrase ended with
         * spaces and/or tabs. Phrase processing is done.
         */

        if( *input_text == '\0' ) break;

        /* Compute space_width and adjust cur_h_address. */

        if( !ws_spc_done ) {

            /* If ws_spc_done is true, there is nothing to do: space_width, 
             * spaces, and tabbing have been set properly.
             */

            if( spaces != 0 ) {

                /* Set tabbing to false since there was at least one space
                 * before next_chars.
                 */

                tabbing = false;

                if( !oc_script ) { 

                    if( the_line->last == NULL ) {

                        /* Spaces is correct if ".co off" is in effect. */

                        if( oc_concat ) {
                            spaces = 0;
                        }
                        space_width = spaces * wgml_fonts[font].spc_width;

                    } else {

                        /* "wscript" (or "noscript") is in effect: set up
                         * space_width and ws_spc_done and reset spaces. 
                         * NOTE: this uses font_number, not old_font.
                         */

                        space_width = wgml_fonts[font].spc_width;

                        ch = the_line->last->text[the_line->last->count - 1];
                        if( ch == '.' || ch == '!' || ch == '?' || ch == ':' ) {
                            space_width *= 2;
                        }
                    }
                } else {
                    space_width = spaces * wgml_fonts[font].spc_width;
                }
                spaces = 0;
            }
        }
        cur_h_address += space_width;
        ws_spc_done = false;

        /* Process any escape sequences. */

        while( *input_text == '\xFE' ) {
            input_text++;
            switch( *input_text ) {
            case 0xff:
                cur_type = norm;
                break;
            case 0x01:
                cur_type = sub;
                break;
            case 0x02:
                cur_type = sup;
                break;
            default:
                out_msg( "outcheck internal error: incorrect text_type\n" );
                err_count++;
                g_suicide();
            }
            input_text++;
        }

        /* If a space is next, go back to the top to count spaces. */

        if( *input_text == ' ' ) continue;

        /* If the end of the phrase is next, then the phrase ended with
         * an initial escape sequence. Phrase processing is done.
         */

        if( *input_text == '\0' ) break;

        /* At this point, next_chars will be NULL. It is time to get the
         * next token.
         */

        token_start = input_text;
        while( *input_text != '\0' ) {
            if( oc_concat && (*input_text == ' ') ) {
                if( ProcFlags.in_trans ) {
                    if( *(input_text - 1) != in_esc ) {
                        break;
                    }
                }
            } else {
                if( *input_text == '\xFE' ) {
                    break;
                }
            }
            input_text++;
        }

        /* If .co is "off", shorten the token to exclude any space characters
         * at the end. Since input_text is a character string, it will end
         * with a '\0'.
         * NOTE: token_start is known not to point to a space character.
         * NOTE: input_text should point to the first position after the
         * last non-space non-null character. Omitting the final increment
         * produced endless looping.
         */

        if( !oc_concat ) {
            while( input_text > token_start ) {
                if( (*input_text == '\0') || (*input_text == ' ') ) {
                    input_text--;
                } else {
                    break;
                }
            }

            /* If the token was halted by an escape sequence, then the first
             * byte of that escape sequence must not be skipped.
             */

            if( *input_text != '\xFE' ) {
                input_text++;
            }
        }
        count = input_text - token_start;

        /* Initialize next_chars to contain the token. */

        next_chars = oc_alloc_text_chars( token_start, count, font, cur_type );
        next_chars->x_address = cur_h_address;
        oc_intrans( (char *)next_chars->text, &next_chars->count, font );

        if( next_chars->count == 0 ) {
            next_chars->width = 0;
        } else {
            next_chars->width = oc_text_chars_width( next_chars->text, next_chars->count, next_chars->font );
        }
        increment = next_chars->width;

        /* Set old_count to the current number of characters controlled by
         * next_chars. This will be used, if needed, to catch the case where
         * none of the characters in next_chars can, in fact, fit on the current
         * line. 
         */ 

        old_count = next_chars->count;

        /* Update text_line. */

        if( oc_concat ) {

            /* Determine if next_chars will fit on the_line.
             * Note: this test is not affected by the width of a hyphen.
             */

            while( !tabbing && ((cur_h_address + increment) > oc_page_right ) ) {

                /* Start with save_chars. */

                save_chars = the_line->last;

                /* Find any and all text_chars instances in the_line that
                 * form a single word with next_chars (the initial value
                 * of cur_h_address is the starting position of next_chars).
                 * Notes: save_chars will point to the first text_chars which
                 *            is not part of a such a word.
                 *        if save_chars is NULL, then either the_line is empty
                 *            or the entire line is part of a single word.
                 */

                if( save_chars != NULL ) {
                    while( cur_h_address == (save_chars->x_address + save_chars->width) ) {
                        cur_h_address = save_chars->x_address;
                        save_chars = save_chars->prev;
                        if( save_chars == NULL ) break;
                    }
                }            

                /* Identify when save_chars must be moved to a new line. */

                if( save_chars != NULL ) {
                    if( ((save_chars != the_line->last) && !((oc_page_left + increment) > oc_page_right))
                        || (save_chars->count == 0) ) {

                        /* the_line ends in either a multi-text_chars word or
                         * an empty text_chars. A multi-text_chars word starts
                         * with save_chars->next unless save_chars is empty, in
                         * which case it starts with save_chars.
                         * Note: In either case, this has the effect of ensuring
                         *       that the_line->last is not empty.
                         */

                        if( save_chars->count == 0 ) {

                            /* Detach the entire save_chars list. */

                            the_line->last = save_chars->prev;
                            the_line->last->next = NULL;
                            save_chars->prev = NULL;

                        } else {

                            /* Detach save_chars->next and the rest of the list,
                             * but leave save_chars itself at the end of
                             * the_line.
                             */

                            the_line->last = save_chars;
                            save_chars = save_chars->next;
                            save_chars->prev = NULL;
                            the_line->last->next = NULL;
                        }
                    } else {

                        /* If save_chars does not need to be moved, then it
                         * must be set to NULL. This prevents looping caused
                         * by never splitting next_chars.
                         */

                        save_chars = NULL;
                    }
                }

                /* At this point:
                 *  if save_chars is NULL, then next_chars needs to be
                 *      appended to the_line and split.
                 *  if save_chars is not NULL, the the_line is ready to be
                 *      output and the fate of next_chars will be decided
                 *      on the next pass through the loop.   
                 */

                if( save_chars == NULL ) {

                    /* A split is possible. It is only appropriate, however,
                     * if the_line is empty or if next_chars will not fit
                     * on its own line, unless it is part of a multi-text_chars
                     * word, and if at least one character in next_chars->text
                     * will fit on the current line.
                     */

                    if( (the_line->last == NULL) ||
                            ((the_line->last->x_address + the_line->last->width) == next_chars->x_address) ||
                            ((oc_page_left + increment) > oc_page_right) ) {

                        /* Here we see how much of next_chars->text will fit
                         * when the width of a hyphen is taken into account.
                         */

                        count = oc_split_text( next_chars, oc_page_right - oc_hyphen_width );

                        /* This is unlikely, but why take chances? */

                        if( count == 0 ) {

                            /* next_chars fits entirely. */

                            save_chars = NULL;
                            break;
                        }

                        /* count can equal old_count, but, in that case, there
                         * is nothing to do.
                         */ 

                        if( count != old_count ) {

                            /* next_chars is split, and hyphenation is needed.
                             * Start by attaching next_chars to the_line.
                             */

                            if( the_line->first == NULL ) {
                                the_line->first = next_chars;
                            } else {
                                next_chars->prev = the_line->last;
                                the_line->last->next = next_chars;
                            }
                            the_line->last = next_chars;
                            next_chars = NULL;

                            /* Create next_chars to contain the rest of the
                             * split text.
                             */

                            next_chars = oc_alloc_text_chars( &the_line->last->text[the_line->last->count],
                                        count, the_line->last->font, the_line->last->type );

                            /* Finalize the_line->last. */

                            the_line->last = oc_wgml_tabs( the_line->last );

                            /* Adjust the line_height, if appropriate. */

                            cur_height = wgml_fonts[font].line_height;
                            if( the_line->line_height < cur_height ) {
                                the_line->line_height = cur_height;
                            }

                            /* Create and attach hyphen_chars to the_line.
                             * Note: Each text_line which is hyphenated will
                             *       have its own hyphen_chars since, if they
                             *       were shared, then the common text_chars
                             *       would eventually be freed multiple times, and
                             *       the coding to prevent that would be very
                             *       complicated.
                             */


                            hyphen_chars = oc_alloc_text_chars( "-", 1, 0, norm );
                            hyphen_chars->width = oc_hyphen_width;

                            hyphen_chars->x_address = the_line->last->x_address + the_line->last->width;
                            hyphen_chars->prev = the_line->last;
                            the_line->last->next = hyphen_chars;
                            the_line->last = hyphen_chars;
                            hyphen_chars = NULL;

                            /* Adjust the line_height, if appropriate. */

                            cur_height = oc_line_height_zero;
                            if( the_line->line_height < cur_height ) {
                                the_line->line_height = cur_height;
                            }
                        }
                    }
                }

                /* At this point:
                 *   the_line is either empty or ready for output.
                 *   save_chars is either NULL or contains the start
                 *       of the new line.
                 *   next_chars needs to have x_address updated.
                 *   cur_h_address needs to be updated.
                 */

                if( the_line->first != NULL ) {

                    /* Process the full text_line and get a new one. */

                    if( !oc_validate_text_line( the_line ) ) out_msg( "Gotcha!\n");
                    oc_process_line_full( the_line, true );
                    the_line = oc_alloc_text_line();

                }

                /* Update cur_h_address to the start of the new line. */

                cur_h_address = oc_page_left;

                /* save_chars processing. */

                if( save_chars == NULL ) {

                    /* Reset next_chars to the left margin. */

                    next_chars->x_address = cur_h_address;

                } else {
                                
                    /* Attach save_chars to the_line. */

                    the_line->first = save_chars;
                    the_line->last = save_chars;

                    /* save_chars is a doubly-linked list of one or more
                     * text_chars instances. The first must be located at
                     * cur_h_address, and the others must be repositioned
                     * to the same relative position using offset.
                     */

                    offset = save_chars->x_address - cur_h_address;
                    save_chars->x_address = cur_h_address;

                    /* Adjust the line_height, if appropriate. */

                    cur_height = wgml_fonts[save_chars->font].line_height;
                    if( the_line->line_height < cur_height ) {
                        the_line->line_height = cur_height;
                    }

                    save_chars = save_chars->next;

                    /* Now reposition any following text_chars instances,
                     * keeping cur_h_address and the_line->last updated to
                     * the start of each instance.
                     */

                    while( save_chars != NULL ) {
                        the_line->last = save_chars;
                        save_chars->x_address -= offset;
                        cur_h_address = save_chars->x_address;

                        /* Adjust the line_height, if appropriate. */

                        cur_height = wgml_fonts[save_chars->font].line_height;
                        if( the_line->line_height < cur_height ) {
                            the_line->line_height = cur_height;
                        }

                        save_chars = save_chars->next;
                    }

                    /* At this point, next_chars->x_address is set to the proper
                     * value for the end of the prior text_line. Reset it using
                     * offset and update cur_h_address. 
                     */

                    next_chars->x_address -= offset;
                    cur_h_address = next_chars->x_address;
                }

                /* Update next_chars and increment so that, at the top
                 * of the loop, they will be correct for the next pass.
                 */

                next_chars->width = oc_text_chars_width( next_chars->text, next_chars->count, next_chars->font );
                increment = next_chars->width;
            }

        } else {
                
            /* When ".co off" is in effect, the_line can be empty at this
             * point. If it is empty, then there is nothing to do unless
             * "wcript/noscript" is in effect.
             * NOTE: if "script" is in effect, next_chars will be added
             * to the_line below.
             */

            if( !oc_script ) {

                /* "wscript/noscript" in effect: split next_chars.
                 * NOTE: if/when columns are supported, the second 
                 * parameter may depend on the current column.
                 */

                count = oc_split_text( next_chars, oc_page_right);

                /* The loop is needed so that the text is split over as many
                 * lines as necessary.
                 */

                while( count > 0 ) {

                    /* count can equal old_count, but, in that case, there
                     * is nothing to do.
                     */ 


                    if( count != old_count ) {

                        /* Attach next_chars to the_line. */

                        if( the_line->first == NULL ) {
                            the_line->first = next_chars;
                        } else {
                            next_chars->prev = the_line->last;
                            the_line->last->next = next_chars;
                        }
                        the_line->last = next_chars;
                        next_chars = NULL;

                        /* Create next_chars to contain the rest of the split
                         * text.
                         */

                        next_chars = oc_alloc_text_chars( &the_line->last->text[the_line->last->count],
                                        count, the_line->last->font, the_line->last->type );
                    }

                    /* Finalize the_line->last. */

                    the_line->last = oc_wgml_tabs( the_line->last );

                    /* Adjust the line_height, if appropriate. */

                    cur_height = wgml_fonts[font].line_height;
                    if( the_line->line_height < cur_height ) {
                        the_line->line_height = cur_height;
                    }

                    /* Process the full text_line and get a new one. */

                    if( !oc_validate_text_line( the_line ) ) out_msg( "Gotcha!\n");
                    oc_process_line_full( the_line, false );
                    the_line = oc_alloc_text_line();
    
                    /* Reset cur_h_address. */

                    cur_h_address = oc_page_left;

                    /* Reset next_chars so it can be processed again, in case
                     * the text in save_chars is so wide that it, also, must
                     * be cut off and moved to a new text_line.
                     */

                    next_chars->x_address = cur_h_address;
                    next_chars->width = oc_text_chars_width( next_chars->text, next_chars->count, next_chars->font );

                    /* See if next_chars is still too long. */

                    count = oc_split_text( next_chars, oc_page_right);
                }
            }
        }

        /* Add next_chars to the_line. */

        if( the_line->first == NULL ) {
            the_line->first = next_chars;
        } else {
            next_chars->prev = the_line->last;
            the_line->last->next = next_chars;
        }
        the_line->last = oc_wgml_tabs( next_chars );

        /* Set tabbing to true if next_chars contained at least one wgml tab
         * character. 
         * Note: if wgml tab alignment or fill characters affects how tabbing
         *           is done, something more elaborate may be needed here.
         */

        if( next_chars != the_line->last ) {
            tabbing = true;
        }
        next_chars = NULL;

        /* Update cur_h_address to the total length of the text in
         * the_line->last.
         */

        cur_h_address = the_line->last->x_address + the_line->last->width;

        /* Adjust the line_height, if appropriate. */

        cur_height = wgml_fonts[font].line_height;
        if( the_line->line_height < cur_height ) {
            the_line->line_height = cur_height;
        }

        /* Reset spaces. */

        spaces = 0;
    }
    return;
}

/* Function emulate_input_source()
 * This function passes known text buffers to oc_process_text() after setting
 * the font number and break flag. 
 *
 * Parameters:
 *      in_text points to the oc_text_phrase array to be processed.
 *
 * Global Changed:
 *      oc_break is set to the correct value.
 *      oc_font_number is set to the font_number of the last oc_text_phrase
 *          processed.
 *
 * Notes:
 *      This function is intended to emulate the last stage of text
 *          processing in our wgml. 
 */
 
static void emulate_input_source( oc_text_phrase * in_text )
{
    oc_text_phrase *   cur_phrase  = NULL;

    /* Process the first oc_text_phrase, which is always considered to cause a
     * break in this simplified emulation.
     */

    cur_phrase = in_text;
    if( cur_phrase->font >= wgml_font_cnt ) {
        cur_phrase->font = 0;
    }
    oc_font_number = cur_phrase->font;
    oc_break = true;
    oc_process_text( cur_phrase->text, oc_font_number );
    cur_phrase++;

    /* Process the remaining oc_text_phrases in in_text. */

    while( cur_phrase->text != NULL ) {
        if( cur_phrase->font >= wgml_font_cnt )
            cur_phrase->font = 0;
        oc_font_number = cur_phrase->font;
        if( oc_concat ) {

            /* In this emulation, there are no breaks inside a paragraph if
             * ".co on" is in effect.
             */

            oc_break = false;

        } else {

            /* In this emulation, there are is a break inside a paragraph if
             * ".co off" is in effect and cur_phrase->new_record is "true".
             */

            oc_break = cur_phrase->new_record;

            if( oc_break ) {

                /* If there is an internal break, a new oc_element will need
                 * to be provided if oc_current_oc_element is NULL.
                 */

                if( oc_new_element == NULL ) {
                    oc_is_paragraph = false;
                    oc_new_element = oc_alloc_oc_element( oc_text );
                }
            }
        }
        oc_process_text( cur_phrase->text, oc_font_number );
        cur_phrase++;
    }
    return;
}

/* Function emulate_layout_page()
 * This function emulates "the rest of wgml", setting various flags and
 * globals depending on the current element type and sending any text buffers
 * to emulate_input_source() to handle the break flag and the font number and
 * get each text buffer processed. 
 *
 * Parameters:
 *      construct encodes the type of construct to be emulated.
 *      input_text points to the oc_text_phrase array to be processed.
 */
 
static void emulate_layout_page( oc_text_phrase * input_text, oc_construct_type construct )
{
    switch( construct ) {
    case oc_title_text:

        /* Set the globals. This emulates a pre_top_skip of 15 and an indent
         * of 0.
         */

        oc_pre_top_skip = 15;
        oc_pre_skip = 0;
        oc_old_post_skip = oc_cur_post_skip;
        oc_cur_post_skip = 0;
        oc_indent = 0;

        /* Initalize oc_new_element. */

        oc_new_element = oc_alloc_oc_element( oc_text );

        /* Emulate setting up the element per the :LAYOUT :TITLEP :TITLE info. */

        oc_new_element->line_position = oc_center;

        /* Output the Title. */

        oc_is_paragraph = false;
        emulate_input_source( input_text );

        break;

    case oc_paragraph:

        /* Set the globals. This emulates a pre_skip of 1 and an indent of 
         * '0.5i'.
         */

        oc_pre_top_skip = 0;
        oc_pre_skip = 1;
        oc_old_post_skip = oc_cur_post_skip;
        oc_cur_post_skip = 0;
        oc_indent = bin_device->horizontal_base_units / 2;

        /* Initalize oc_new_element. */

        oc_new_element = oc_alloc_oc_element( oc_text );

        /* Emulate setting up the element per the :LAYOUT :P :TITLE info. */

        oc_new_element->line_position = oc_none;

        /* Put input_text into cur_elem. */

        oc_is_paragraph = true;
        emulate_input_source( input_text );
        break;
    case oc_bx_box:
        /* Implementation: to be done. */
        break;
    case oc_fig_box:
        /* Implementation: to be done. */
        break;
    default:
        out_msg( "outcheck internal error: incorrect element type\n" );
        err_count++;
        g_suicide();
    }
    return;
}

/* Function emulate_wgml().
 * This function loads the binary device library using the same code as wgml,
 * and then produces an text image output file using the device specified on
 * the command line. No parsing is done: the text is entirely static and the
 * assignment of words to lines etc is done manually; only the actual output
 * is being tested. 
 *
 * Notes:
 *      This code only allows for the :PAGEADDRESS block y_positive to have
 *          different values in different drivers. No known drivers have any
 *          value for x_positive other than "1" ("yes"). 
 */
 
static void emulate_wgml( void )
{
    int                 i;
    oc_element      *   te_pool_ptr = NULL;
    text_chars      *   tc_pool_ptr = NULL;
    text_line       *   tl_pool_ptr = NULL;
    uint32_t            max_char_width;
    uint32_t            oc_h_len; // needed?
    uint32_t            oc_v_len; // needed?

    /* Set the static globals. */

    /* Initialize the oc_element pool to hold ten instances. These are
     * oc_text_elements because that is the oc_element used most often.
     */

    /* Initialize the text_chars pool to hold 20 instances. */

    oc_text_chars_pool = mem_alloc( sizeof( text_chars ) + TEXT_START );
    oc_text_chars_pool->next = NULL;
    oc_text_chars_pool->font = 0;
    oc_text_chars_pool->x_address = 0;
    oc_text_chars_pool->width = 0;
    oc_text_chars_pool->count = 0;
    oc_text_chars_pool->length = TEXT_START;
    tc_pool_ptr = oc_text_chars_pool;
    for( i = 0; i < 19; i++ ) {
        tc_pool_ptr->next = mem_alloc( sizeof( text_chars ) + TEXT_START );
        tc_pool_ptr = tc_pool_ptr->next;
        tc_pool_ptr->next = NULL;
        tc_pool_ptr->font = 0;
        tc_pool_ptr->x_address = 0;
        tc_pool_ptr->width = 0;
        tc_pool_ptr->count = 0;
        tc_pool_ptr->length = TEXT_START;
    }

    /* Initialize the text_line pool to hold ten instances. */

    oc_text_line_pool = mem_alloc( sizeof( text_line ) );
    oc_text_line_pool->next = NULL;
    oc_text_line_pool->line_height = 0;
    oc_text_line_pool->y_address = 0;
    oc_text_line_pool->first = 0;
    tl_pool_ptr = oc_text_line_pool;
    for( i = 0; i < 9; i++ ) {
        tl_pool_ptr->next = mem_alloc( sizeof( text_line ) );
        tl_pool_ptr = tl_pool_ptr->next;
        tl_pool_ptr->next = NULL;
        tl_pool_ptr->line_height = 0;
        tl_pool_ptr->y_address = 0;
        tl_pool_ptr->first = NULL;
    }

    oc_element_pool = mem_alloc( sizeof( oc_element ) );
    oc_element_pool->next = NULL;
    oc_element_pool->type = oc_text;
    oc_element_pool->element.oc_text.count = 0;
    oc_element_pool->element.oc_text.depth = 0;
    oc_element_pool->element.oc_text.first = NULL;
    te_pool_ptr = oc_element_pool;
    for( i = 0; i < 9; i++ ) {
        te_pool_ptr->next = mem_alloc( sizeof( oc_element ) );
        te_pool_ptr = te_pool_ptr->next;
        te_pool_ptr->next = NULL;
        te_pool_ptr->type = oc_text;
        te_pool_ptr->element.oc_text.count = 0;
        te_pool_ptr->element.oc_text.depth = 0;
        te_pool_ptr->element.oc_text.first = NULL;
    }

    /* These values are currently being tuned to match wgml 4.0.
     * The constants used with oc_max_depth match the values used by
     * wgml 4.0 for a page depth of 9.66i converted to vertical base
     * units and rounded up if the result is not an integer. The use of
     * y_offset shown comes from our wgml code. 
     */

    if( bin_driver->y_positive == 0 ) {
        oc_page_top = bin_device->y_start;
        oc_max_depth = 9660 - bin_device->y_offset;
    } else {
        oc_page_top = bin_device->y_start;
        if( bin_device->y_start > 0 ) {
            oc_page_top--;
        }
        oc_max_depth = 58 - bin_device->y_offset;
    }

    /* The "missing" value, oc_page_bottom, was dropped because no use was
     * found for it in computing the page layout. The corresponding variable
     * may, of course, be useful in our wgml.
     */

    /* The values used set up a left margin of '1i' and a right margin of
     * '7i', expressed in horizontal base units, both from the position
     * specificed in the :PAGESTART block. 
     */

    oc_page_left = bin_device->horizontal_base_units + bin_device->x_start - bin_device->x_offset;
    oc_page_right = 7 * bin_device->horizontal_base_units + bin_device->x_start - bin_device->x_offset;

    /* This was originally created to be used in outcheck.c but not necessarily
     * in our wgml to simplify situations where the line height is either not
     * known or not well defined. However, it is also used with hyphenation,
     * since the hyphen used is always in the default font.
     */

    oc_line_height_zero = wgml_fonts[0].line_height;

    /* This is used in hyphenating extremely long words. */

    oc_hyphen_width = wgml_fonts[0].width_table['-'];

    /* Set the variables. */

    max_char_width = 0;

    for( i = 0; i < wgml_font_cnt; i++ ) {
        if( max_char_width < wgml_fonts[i].default_width ) {
            max_char_width = wgml_fonts[i].default_width;
        }
    }

    /* The OUTCHECK Test Document. */

    /* Start in "script" mode with concatenation on. */

    oc_concat = true;
    oc_script = true;

    /* Allow input translation tests. */

    cop_ti_table( "set ~" );

    /* First pass processing. */
    /* START processing.*/

    fb_start();

    /* DOCUMENT processing.*/

    fb_document();

    /* Only one document pass is emulated. */

    oc_document_start = true;
    oc_new_section = true;

    /* Title page. */ 

    emulate_layout_page( title, oc_title_text );

    /* The title page is done. Finish the page and force it out. */

    oc_break = true;
    oc_process_text( NULL, oc_font_number );
    oc_output_page();

    /* Document Section. */

    oc_new_section = true;

    /* Output the text paragraphs. */

    /* Set both system variables, just as ".dc tb" would do. */

    tab_char = tab_char = '>';
    add_to_sysdir( "$tb", tab_char );
    add_to_sysdir( "$tab", tab_char );

    /* We are still in "script" mode with concatenation on. */

    emulate_layout_page( intro_cosc, oc_paragraph );
    emulate_layout_page( para_font, oc_paragraph );
    emulate_layout_page( para_long, oc_paragraph );
    emulate_layout_page( para_stop, oc_paragraph );
    emulate_layout_page( para_subsup, oc_paragraph );
    emulate_layout_page( para_tab, oc_paragraph );

    /* Now switch to "wscript/noscript" mode with concatenation on. */

    oc_script = false;
    emulate_layout_page( intro_cowsc, oc_paragraph );
    emulate_layout_page( para_font, oc_paragraph );
    emulate_layout_page( para_long, oc_paragraph );
    emulate_layout_page( para_stop, oc_paragraph );
    emulate_layout_page( para_subsup, oc_paragraph );
    emulate_layout_page( para_tab, oc_paragraph );

    /* Now switch to "script" mode with concatenation off. */

    oc_concat = false;
    oc_script = true;
    emulate_layout_page( intro_xcosc, oc_paragraph );
    emulate_layout_page( para_font, oc_paragraph );
    emulate_layout_page( para_long, oc_paragraph );
    emulate_layout_page( para_stop, oc_paragraph );
    emulate_layout_page( para_subsup, oc_paragraph );
    emulate_layout_page( para_tab, oc_paragraph );

    /* Now switch to "wscript/noscript" mode with concatenation off. */

    oc_concat = false;
    oc_script = false;
    emulate_layout_page( intro_xcowsc, oc_paragraph );
    emulate_layout_page( para_font, oc_paragraph );
    emulate_layout_page( para_long, oc_paragraph );
    emulate_layout_page( para_stop, oc_paragraph );
    emulate_layout_page( para_subsup, oc_paragraph );
    emulate_layout_page( para_tab, oc_paragraph );

    /* Reset the tab-related system symbols. */

    tab_char = tab_char = 0x09;
    add_to_sysdir( "$tb", tab_char );
    add_to_sysdir( "$tab", tab_char );

#if 0 // restore when boxing is done again and blank-line-insertion is handled
    /* A blank line is placed between paragraphs. */

    /* Output the second paragraph. */

//    oc_h_start = oc_page_left + (bin_device->horizontal_base_units / 2);
    if( bin_driver->y_positive == 0x00 ) {
//        oc_v_start -= 2 * line_height_zero;
    } else {
//        oc_v_start += 2 * line_height_zero;
    }
    emulate_layout_page( para2, oc_paragraph );
#endif
    /* First box. */

    oc_h_len = bin_device->horizontal_base_units;
//    oc_h_start = bin_device->horizontal_base_units;
    oc_v_len = 2 * max_char_width;
#if 0 // move to within the bx_element including the above three lines
    if( bin_driver->y_positive == 0x00 ) {
//        oc_v_start -= 2 * line_height_zero;
    } else {
//        oc_v_start += 2 * line_height_zero;
    }

    if( has_aa_block ) {
        if( bin_driver->y_positive == 0x00 ) {
            if( bin_driver->hline.text != NULL ) {
//                fb_hline( oc_h_start, oc_v_start, oc_h_len );
//                fb_hline( oc_h_start, oc_v_start - oc_v_len, oc_h_len );
            }
            if( bin_driver->vline.text != NULL ) {
//                oc_v_start -= oc_v_len;
//                fb_vline( oc_h_start, oc_v_start, oc_v_len );
//                fb_vline( oc_h_start + oc_h_len, oc_v_start, oc_v_len );
//                oc_v_start += oc_v_len;
            }
        } else {
            if( bin_driver->hline.text != NULL ) {
//                fb_hline( oc_h_start, oc_v_start, oc_h_len );
//                fb_hline( oc_h_start, oc_v_start + oc_v_len, oc_h_len );
            }
            if( bin_driver->vline.text != NULL ) {
//                oc_v_start += oc_v_len;
//                fb_vline( oc_h_start, oc_v_start, oc_v_len );
//                fb_vline( oc_h_start + oc_h_len, oc_v_start, oc_v_len );
//                oc_v_start -= oc_v_len;
            }
        }
    }
#endif
    /* Output the third paragraph. */

#if 0 // restore when boxing is done again
//    oc_h_start = oc_page_left + (bin_device->horizontal_base_units / 2);
    if( bin_driver->y_positive == 0x00 ) {
//        oc_v_start = oc_page_top - line_height_zero;
    } else {
//        oc_v_start = line_height_zero;
    }
    emulate_layout_page( para3, oc_text );

    /* Second box. */

    if( bin_driver->y_positive == 0x00 ) {
//        oc_v_start -= 2 * line_height_zero;
    } else {
//        oc_v_start += 2 * line_height_zero;
    }

//    oc_h_start = bin_device->horizontal_base_units;
    if( has_aa_block != NULL ) {
        if( bin_driver->y_positive == 0x00 ) {
            if( bin_driver->dbox.text != NULL ) {
//                fb_dbox( oc_h_start, oc_v_start - oc_v_len, oc_h_len, oc_v_len );
            }
        } else {
            if( bin_driver->dbox.text != NULL ) {
//                fb_dbox( oc_h_start, oc_v_start + oc_v_len, oc_h_len, oc_v_len );
            }
        }
    }
#endif

    /* The document is processed. Finish the page and force it out. */

    oc_break = true;
    oc_process_text( NULL, oc_font_number );
    oc_output_page();

    /* :FINISH block. */

    fb_finish();

    /* Free allocated memory. */

    /* Free the memory held by the oc_element pool. */

    if( oc_element_pool != NULL ) {
        while( oc_element_pool->next != NULL ) {
            te_pool_ptr = oc_element_pool;
            oc_element_pool = oc_element_pool->next;
            switch( te_pool_ptr->type ) {
            case oc_text:

                if( te_pool_ptr->element.oc_text.first != NULL ) {

                    /* Return any text_line instances to the text_line pool. */

                    out_msg( "oc_text_element in pool not empty!\n" );
                    oc_add_text_line_to_pool( te_pool_ptr->element.oc_text.first );
                    te_pool_ptr->element.oc_text.first = NULL;
                }
            case oc_bx:
                /* Implementation: to be done. */
                break;
            case oc_fig:
                /* Implementation: to be done. */
                break;
            default:
                out_msg( "outcheck internal error: incorrect oc_element\n" );
                err_count++;
                g_suicide();
            }
            mem_free( te_pool_ptr );
        }
        mem_free( oc_element_pool );
        oc_element_pool = NULL;
        te_pool_ptr = NULL;
    }

    /* Free the memory held by the text_line pool. */

    if( oc_text_line_pool != NULL ) {
        while( oc_text_line_pool->next != NULL ) {
            tl_pool_ptr = oc_text_line_pool;
            oc_text_line_pool = oc_text_line_pool->next;
            if( tl_pool_ptr->first != NULL ) {

                /* Return any text_chars instances to the text_chars pool. */

                out_msg( "text_line in pool not empty!\n" );
                oc_add_text_chars_to_pool( tl_pool_ptr );
                tl_pool_ptr->first = NULL;
                tl_pool_ptr->last = NULL;
            }
            mem_free( tl_pool_ptr );
        }
        mem_free( oc_text_line_pool );
        oc_text_line_pool = NULL;
        tl_pool_ptr = NULL;
    }

    /* Free the memory held by the text_chars pool. */

    if( oc_text_chars_pool != NULL ) {
        while( oc_text_chars_pool->next != NULL ) {
            tc_pool_ptr = oc_text_chars_pool;
            oc_text_chars_pool = oc_text_chars_pool->next;
            mem_free( tc_pool_ptr );
        }
        mem_free( oc_text_chars_pool );
        oc_text_chars_pool = NULL;
        tc_pool_ptr = NULL;
    }

    return;
}

/* Global function definitions. */

/* Function print_banner().
 * Print the banner to the screen.
 */

extern void print_banner( void )
{
    puts( banner1w( "Document Output Test Program", _RESEARCH_VERSION_ ) );
    puts( banner2 );
    puts( banner3 );
    puts( banner3a );
}

/* Function print_usage().
 * Print the usage information to the screen.
 */

extern void print_usage( void )
{
    char const * *  list;

    list = usage_text;
    while( *list ) {
        puts( *list );
        ++list;
    }
}

/* Function main().
 * Given a valid defined name, verify that it is a valid .COP file and parse 
 * it if it is.
 *
 * The actual parsing is performed in the function parse_defined_name(); main()
 * is concerned with overall program architecture, not details.
 *
 * Returns:
 *      EXIT_FAILURE or EXIT_SUCCESS, as appropriate.
 */

int main()
{
    char    *   cmdline = NULL;
    int         retval;
    jmp_buf     env;
    size_t      cmdlen  = 0;

null_buffer();
start_heapcheck( "main" );

    /* For compatibility with wgml modules. */

    environment = &env;
    if( setjmp( env ) ) {               // if fatal error has occurred
        my_exit( 16 );
    }

    /* Display the banner. */

    print_banner();

    /* Display the usage information if the command line is empty. */

    cmdlen = _bgetcmd( NULL, 0 );
    if( cmdlen == 0 ) {
        print_usage();
        return( EXIT_FAILURE );
    }

    /* Include space for the terminating null character. */

    cmdlen++;

    /* Get the command line. */

    cmdline = malloc( cmdlen );
    if( cmdline == NULL ) {
        return( EXIT_FAILURE );
    }

    cmdlen = _bgetcmd( cmdline, cmdlen );

    /* Initialize the globals. */

    initialize_globals();
    res_initialize_globals();
    init_global_vars();         // wgml globals

    /* Parse the command line: allocates and sets tgt_path. */

    retval = parse_cmdline( cmdline );
    if( retval == FAILURE ) {
        free( cmdline );
        return( EXIT_FAILURE );
    }

    /* Free the memory held by cmdline and reset it. */

    free( cmdline );
    cmdline = NULL;

    dev_name = tgt_path;
    master_fname = "outcheck";
    out_file = NULL;
    out_file_attr = NULL;

    /* Initialize the binary device library. */

    ff_setup();
    cop_setup();

    /* Create the output file. */

    emulate_wgml();

    /* Release the memory allocated. */

    cop_teardown();
    ff_teardown();

    mem_free(tgt_path);
    tgt_path = NULL;

    free_some_mem();            // wgml globals

end_heapcheck( "main" );
//display_heap( "pre" );
//display_heap( "post" );

    return( EXIT_SUCCESS );
}

