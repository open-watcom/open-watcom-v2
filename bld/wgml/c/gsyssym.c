/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Implements predefined system variables SYSxxx and more.
*               Most are read-only and some are calculated on access.
*
****************************************************************************/


#include "wgml.h"

#include "clibext.h"


#define sys(x)  sys##x                  // construct symvar varname
#define sysf(x) sys##x##fun             // construct access function name
#define sys0(x) sys##x##0               // construct subscript 0 name
#define syss(x) sys##x##str             // construct name for string value


/***************************************************************************/
/*  declare dictionary entries for system variables                        */
/***************************************************************************/

#define picka( var, flag )      pickk( var, flag )
#define pickc( var, flag )      pickk( var, flag )
#define pickl( var, flag )      pickk( var, flag )
#define pickk( var, flag ) static symvar sys( var );
#include "gsyssym.h"
#undef pickk
#undef pickl
#undef pickc
#undef picka

#define picka( var, flag )      pickk( var, flag )
#define pickc( var, flag )      pickk( var, flag )
#define pickl( var, flag )      pickk( var, flag )
#define pickk( var, flag ) static symsub sys0( var );
#include "gsyssym.h"
#undef pickk
#undef pickl
#undef pickc
#undef picka

/***************************************************************************/
/*  declare the access functions for system variables                      */
/***************************************************************************/

#define picka( var, flag )
#define pickc( var, flag )      pickl( var, flag )
#define pickk( var, flag )      pickl( var, flag )
#define pickl( var, flag ) static void sysf( var )( symvar *entry );
#include "gsyssym.h"
#undef pickl
#undef pickk
#undef pickc
#undef picka

/***************************************************************************/
/*  define char strings to hold the values of some system variables        */
/***************************************************************************/

#define picka( var, flag )
#define pickk( var, flag )
#define pickc( var, flag ) static char syss( var )[2];              // for single char values as string
#define pickl( var, flag ) static char syss( var )[NUM2STR_LENGTH + 1]; // for number as string and sysbxchar
#include "gsyssym.h"
#undef pickl
#undef pickc
#undef pickk
#undef picka

/***************************************************************************/
/*  define the dictionary for the system variables                         */
/*  The dictionary is built at compile time                                */
/*  picka:      no access function, no separate string value               */
/*  pickc:         access function,    separate string value 2 chars       */
/*  pickl:         access function,    separate string value 12 chars      */
/*  pickk:         access function, no separate string value               */
/*                                  or 2 predefined values  ON OFF         */
/***************************************************************************/

#define pickc( var, flag )              \
        static symvar sys( var ) = {    \
            NULL, "$" #var, 0L, 0L, NULL, &sys0( var ), sysf( var ), flag }; \
        static symsub sys0( var ) = { NULL, &sys( var ), SI_no_subscript, syss( var ), 1 };
#define pickl( var, flag )              \
        static symvar sys( var ) = {    \
            NULL, "$" #var, 0L, 0L, NULL, &sys0( var ), sysf( var ), flag }; \
        static symsub sys0( var ) = { NULL, &sys( var ), SI_no_subscript, syss( var ), NUM2STR_LENGTH };
#define picka( var, flag )              \
        static symvar sys( var ) = {    \
            NULL, "$" #var, 0L, 0L, NULL, &sys0( var ), NULL, flag }; \
        static symsub sys0( var ) = { NULL, &sys( var ), SI_no_subscript, NULL, 0 };
#define pickk( var, flag )              \
        static symvar sys( var ) = {    \
            NULL, "$" #var, 0L, 0L, NULL, &sys0( var ), sysf( var ), flag }; \
        static symsub sys0( var ) = { NULL, &sys( var ), SI_no_subscript, NULL, 0 };
#include "gsyssym.h"
#undef pickk
#undef picka
#undef pickl
#undef pickc


/***************************************************************************
 *  Define array with justify parameter strings
 *  used also for independent "OFF"/"ON" strings for other parameters
 */
static char     *just_str[] = {
    #define JUST_DEF(a,b) b,
    JUST_DEFS
    #undef JUST_DEF
};

#define CONST_OFF just_str[JUST_off]
#define CONST_ON  just_str[JUST_on]

static  char    dateval[20];
static  char    dayofmval[3];
static  char    dayofwval[2];
static  char    dayofyval[4];
static  char    hourval[3];
static  char    minuteval[3];
static  char    monthval[3];
static  char    pdayofwval[10];
static  char    pmonthval[12];
static  char    pyearval[5];
static  char    timeval[9];

/***************************************************************************/
/*  make new single char value known in dictionary                         */
/*  This is called from gsdccw.c and others                                */
/***************************************************************************/

void    add_to_sysdir( char *name, char char_val )
{
    symsub          *dictval;

    find_symvar( sys_dict, name, SI_no_subscript, &dictval);
    *(dictval->value) = char_val;
}

/***************************************************************************/
/*  error routine for referenced, but not yet implemented variables        */
/*  the msg is only output for the first reference of every variable       */
/***************************************************************************/

static void var_wng( char *varname, symvar *e )
{
    if( !ProcFlags.no_var_impl_err ) {  // for full dict print no err msg
        e->varfunc = NULL;              // deactivate after first warning
        xx_warn_c( ERR_VAR_NOT_IMPL, varname );
    }
    return;
}

/***************************************************************************/
/*  access routines for some system symbols  mostly still dummy   TBD      */
/*  only those with comments are 'real'                                    */
/***************************************************************************/

static void sysadfun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void sysadevenfun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void sysadoddfun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void sysapagefun( symvar *e )   // absolute page
{
    (void)e;

    sprintf( sysapagestr, "%d", g_apage + 1 );   // to match wgml 4.0
    return;
}

static void sysbefun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void sysbfontsfun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void sysbmfun( symvar *e )
{
    (void)e;

    sprintf( sysbmstr, "%d", g_bm );
    return;
}

static void sysbofun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void sysbxfun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void sysbxcharfun( symvar *e )  // box char always UNDefined TBD
{
    var_wng( e->name, e );
    return;
}

static void sysccfun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void syscccfun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void syscdfun( symvar *e )      // column count
{
    (void)e;

    sprintf( syscdstr, "%d", g_cd );
    return;
}

static void syscdcountfun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void syscharsfun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void sysclfun( symvar *e )      // column length
{
    (void)e;

    sprintf( sysclstr, "%d", g_cl );
    return;
}

static void syscofun( symvar *e )      // .co status
{
    (void)e;

    if( ProcFlags.concat ) {
        sysco0.value = CONST_ON;
    } else {
        sysco0.value = CONST_OFF;
    }
    return;
}

static void syscontfun( symvar *e )
{
    e->sub_0->value = syscontstr;
    return;
}

static void syscpfun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void syscpagesfun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void syscpcfun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void syscpifun( symvar *e )     // cpi chars per inch
{
    (void)e;

    sprintf( syscpistr, "%d", CPI );
    return;
}

static void sysdfontsfun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void sysdhsetfun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void sysdpagefun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void sysduplexfun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void sysenvfun( symvar *e )     // never seen another value TBD
{
    (void)e;

    sysenv0.value = "BODY";
    return;
}

static void sysfbfun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void sysfbcfun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void sysfbffun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void sysfilefun( symvar *e )    // name of current input file/macro
{
    (void)e;

    if( input_cbs == NULL ) {
        sysfile0.value = NULL;
    } else {
        if( input_cbs->fmflags & II_file ) {
            sysfile0.value = input_cbs->s.f->filename;
        } else {
            sysfile0.value = input_cbs->s.m->mac->name;
        }
    }
    return;
}

static void sysfkfun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void sysfkcfun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void sysflnestfun( symvar *e )  // include level file/macro
{
    (void)e;

    sprintf( sysflneststr, "%d", inc_level );
    return;
}

static void sysfmfun( symvar *e )
{
    (void)e;

    sprintf( sysfmstr, "%d", g_fm );
    return;
}

static void sysfnfun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void sysfnamfun( symvar *e )  // name of current input file not macro
{
    inputcb *wk;

    (void)e;

    if( input_cbs == NULL ) {
        sysfnam0.value = NULL;
    } else {
        wk = input_cbs;
        while( (wk != NULL) && (wk->fmflags & II_file) == 0 ) {
            wk = wk->prev;
        }
        if( wk == NULL ) {
            sysfnam0.value = NULL;
        } else {
            sysfnam0.value = wk->s.f->filename;
        }
    }
    return;
}

static void sysfncfun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void sysfnumfun( symvar *e )// lineno of current input file not macro
{
    inputcb     *wk;
    line_number l = 0;

    if( input_cbs != NULL ) {
        wk = input_cbs;
        while( (wk != NULL) && (wk->fmflags & II_file) == 0 ) {
            wk = wk->prev;
        }
        if( wk != NULL ) {
            l = wk->s.f->lineno;
        }
    }
    sprintf( e->sub_0->value, "%d", l );
    return;
}

static void sysfontsfun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void sysfsfun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void sysgutterfun( symvar *e )  // gutter
{
    (void)e;

    sprintf( sysgutterstr, "%d", g_gutter );
    return;
}

static void syshifun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void syshmfun( symvar *e )
{
    (void)e;

    sprintf( syshmstr, "%d", g_hm );
    return;
}

static void syshnfun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void syshncfun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void syshsfun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void syshyfun( symvar *e )
{
    (void)e;

    syshy0.value = CONST_OFF;     // will need adjustment if hyphenation is implemented
    return;
}

static void syshycfun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void syshyphfun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void sysinfun( symvar *e )      // .in indent value
{
    int     t_indent;                   // needed to make correction below

    (void)e;

    t_indent = (t_page.cur_left * CPI) / g_resh;
    if( ((t_page.cur_left * CPI) - (t_indent * g_resh)) > (g_resh / 2) ) {  // rounding check
        t_indent++;
    }
    sprintf( sysinstr, "%d", t_indent );
}

static void sysinrfun( symvar *e )     // .in indentr indent right value
{
    int     t_indent;                   // needed to make correction below

    (void)e;

    t_indent = (t_page.max_width * CPI) / g_resh;
    if( ((t_page.max_width * CPI) - (t_indent * g_resh)) > (g_resh / 2) ) {  // rounding check
        t_indent++;
    }
    sprintf( sysinrstr, "%d", t_indent );
}

static void sysirfun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void sysixjfun( symvar *e )
{
    (void)e;

    sysixj0.value = sysixjstr;
    return;
}

static void sysixreffun( symvar *e )   // $ixref
{
    (void)e;

    sysixref0.value = sysixrefstr;
    return;
}

static void sysjufun( symvar *e )      // .ju status
{
    (void)e;

    sysju0.value = just_str[ProcFlags.justify];
    return;
}

static void syslayoutfun( symvar *e ) // LAYOUT cmdline option or :LAYOUT tag seen
{
    (void)e;

    if( ProcFlags.lay_specified ) {
        syslayout0.value = CONST_ON;
    } else {
        syslayout0.value = CONST_OFF;
    }
    return;
}

static void syslcfun( symvar *e )      // remaining lines in column
{
    unsigned    column_lines;
    unsigned    net_depth;

    (void)e;

    net_depth = t_page.max_depth - t_page.cur_depth;
    if (t_line != NULL && (t_line->line_height <= net_depth) ) {
        net_depth -= t_line->line_height;
    }
    column_lines = (net_depth * LPI) / g_resv;
    sprintf( syslcstr, "%d", column_lines );
    return;
}

static void syslifun( symvar *e )      // SCRIPT control word start char
{
    (void)e;

    *syslistr = SCR_char;
    return;
}

static void syslinbfun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void syslinefun( symvar *e )    // current lineno on page
{
    (void)e;

    sprintf( syslinestr, "%d", g_line );
    return;
}

static void sysllfun( symvar *e )
{
    (void)e;

    sprintf( sysllstr, "%d", g_ll );
    return;
}

static void syslnumfun( symvar *e )  // lineno of current input file / macro
{
    line_number l;

    if( input_cbs == NULL ) {
        l = 0;
    } else {
        if( input_cbs->fmflags & II_file ) {
            l = input_cbs->s.f->lineno;
        } else {
            l = input_cbs->s.m->lineno;
        }
    }
    sprintf( e->sub_0->value, "%d", l );
    return;
}

static void syslsfun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void syslstfun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void sysmcfun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void sysmcsfun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void sysnodeidfun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void sysoffun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void sysofflinefun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void sysonlinefun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void sysoocfun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void sysoutfun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void syspagefun( symvar *e )    // pageno in body
{
    (void)e;

    sprintf( syspagestr, "%d", g_page );
    return;
}

static void syspagedfun( symvar *e )   // page depth
{
    (void)e;

    sprintf( syspagedstr, "%d", g_page_depth );
    return;
}

static void syspagelmfun( symvar *e )  // page left margin
{
    (void)e;

    sprintf( syspagelmstr, "%d", g_lm );
    return;
}

static void syspagermfun( symvar *e )  // page right margin
{
    (void)e;

    sprintf( syspagermstr, "%d", g_rm );
    return;
}

static void syspgnumafun( symvar *e )  // pagenumber
{
    (void)e;

    sprintf( syspgnumastr, "%d", g_page );
    return;
}

static void syspgnumadfun( symvar *e ) // pagenumber.
{
    (void)e;

    sprintf( syspgnumadstr, "%d.", g_page );
    return;
}

static void syspgnumcfun( symvar *e )  // roman page no (UPPER)
{
    (void)e;

    int_to_roman( g_page, syspgnumcstr, sizeof( syspgnumcstr ), true );
    return;
}

static void syspgnumcdfun( symvar *e ) // roman page no (UPPER).
{
    (void)e;

    int_to_roman( g_page, syspgnumcdstr, sizeof( syspgnumcdstr ), true );
    strcat( syspgnumcdstr, "." );
    return;
}

static void syspgnumrfun( symvar *e ) // roman page no
{
    (void)e;

    int_to_roman( g_page, syspgnumrstr, sizeof( syspgnumrstr ), false );
    return;
}

static void syspgnumrdfun( symvar *e ) // roman page no.
{
    (void)e;

    int_to_roman( g_page, syspgnumrdstr, sizeof( syspgnumrdstr ), false );
    strcat( syspgnumrdstr, "." );
    return;
}

static void syspifun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void syspixfun( symvar *e )
{
    (void)e;

    e->sub_0->value = syspixstr;
    return;
}

static void sysplfun( symvar *e )
{
    unsigned    column_lines;

    (void)e;

    column_lines = (t_page.max_depth * LPI) / g_resv;
    sprintf( sysplstr, "%d", column_lines );
    return;
}

static void sysplsfun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void syspnfun( symvar *e )      // page no
{
    (void)e;

    sprintf( syspnstr, "%d", g_page );
    return;
}

static void sysppagefun( symvar *e )   // page no
{
    (void)e;

    sprintf( sysppagestr, "%d", g_page );
    return;
}

static void sysprsfun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void sysprtfun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void syspsfun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void syspwfun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void sysquietfun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void sysrbfun( symvar *e )      // required blank
{
    (void)e;

    e->sub_0->value = sysrbstr;
    return;
}

static void sysrecnofun( symvar *e )   // recno current input file / macro
{                                       // make it the same as &syslnum
    syslnumfun( e );                    // TBD
    return;
}

static void sysreshfun( symvar *e )    // horiz base units
{
    (void)e;

    sprintf( sysreshstr, "%d", g_resh );
    return;
}

static void sysresvfun( symvar *e )    // vert base units
{
    (void)e;

    sprintf( sysresvstr, "%d", g_resv );
    return;
}

static void sysretfun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void sysrmnestfun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void sysscfun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void sysscreenfun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void sysseqnofun( symvar *e )   // seqno current input record
{                                       // take &syslnum
    line_number l;                      // TBD

    if( input_cbs == NULL ) {
        l = 0;
    } else {
        if( input_cbs->fmflags & II_file ) {
            l = input_cbs->s.f->lineno;
        } else {
            l = input_cbs->s.m->lineno;
        }
    }
    sprintf( e->sub_0->value, "%.8u", l );
    return;
}

static void sysskcondfun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void sysslfun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void sysspcondfun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void syssufun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void syssysfun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void systabfun( symvar *e )     // current user-defined tab character
{
    (void)e;

    *systabstr = tab_char;
    return;
}

static void systbfun( symvar *e )     // current user-defined tab character
{
    (void)e;

    *systbstr = tab_char;
    return;
}

static void systermtfun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void systmfun( symvar *e )
{
    (void)e;

    sprintf( systmstr, "%d", g_tm );
    return;
}

static void sysuseridfun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void syswdfun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

static void sysxtextfun( symvar *e )
{
    var_wng( e->name, e );
    return;
}

/***************************************************************************/
/*        dummy routines to avoid another picx macro                       */
/*        they deactivate themself at the first call                       */
/***************************************************************************/


static void sysampfun( symvar *e )     // dummy routine not needed
{
    e->varfunc = NULL;
    return;
}

static void sysbcfun( symvar *e )      // dummy routine not needed
{
    e->varfunc = NULL;
    return;
}

static void sysbsfun( symvar *e )      // dummy routine not needed
{
    e->varfunc = NULL;
    return;
}

static void syscwfun( symvar *e )      // dummy routine not needed
{
    e->varfunc = NULL;
    return;
}

static void sysgmlfun( symvar *e )     // dummy routine not needed
{
    e->varfunc = NULL;
    return;
}

static void syspassnofun( symvar *e )  // dummy routine not needed
{
    e->varfunc = NULL;
    return;
}

static void syspassoffun( symvar *e )  // dummy routine not needed
{
    e->varfunc = NULL;
    return;
}

static void systisetfun( symvar *e )   // dummy routine not needed
{
    e->varfunc = NULL;
    return;
}


/***************************************************************************/
/*  init_date_time  init several date / time global variables              */
/***************************************************************************/

static  void    init_date_time( void )
{
    time_t          now;
    struct tm       *tmbuf;
    char            *p;

    now = time( NULL );
    tmbuf = localtime( &now );
    strftime( dateval, sizeof( dateval ), "%B %d, %Y", tmbuf );

    p = strstr( dateval, " 0" );        // search for leading zero
    if( p != NULL ) {                   // 'September 02, 2009'
        p++;
        *p = ' ';
        while( *p != '\0' ) {           // becomes
            *p = *(p + 1);              // 'September 2, 2009'
            p++;
        }
    }
    sysdate0.value = dateval;
    add_symvar( global_dict, "date", dateval, strlen( dateval ), SI_no_subscript, SF_none );

    strftime( dayofmval, sizeof( dayofmval ), "%e", tmbuf );
    sysdayofm0.value = dayofmval;

    strftime( dayofwval, sizeof( dayofwval ), "%w", tmbuf );
    dayofwval[0] += 1;                  // make 0-6 sun-sat 1-7
    sysdayofw0.value = dayofwval;

    strftime( dayofyval, sizeof( dayofyval ), "%j", tmbuf );
    sysdayofy0.value = dayofyval;

    strftime( hourval, sizeof( hourval ), "%H", tmbuf );
    syshour0.value = hourval;

    strftime( minuteval, sizeof( minuteval ), "%M", tmbuf );
    sysminute0.value = minuteval;

    strftime( monthval, sizeof( monthval ), "%m", tmbuf );
    sysmonth0.value = monthval;

    strftime( pdayofwval, sizeof( pdayofwval ), "%A", tmbuf );
    syspdayofw0.value = pdayofwval;

    strftime( pmonthval, sizeof( pmonthval ), "%B", tmbuf );
    syspmonth0.value = pmonthval;

    strftime( pyearval, sizeof( pyearval ), "%Y", tmbuf );
    syspyear0.value = pyearval;
    sysyear0.value = &pyearval[2];      // year without century

    strftime( timeval, sizeof( timeval ), "%T", tmbuf );
    systime0.value = timeval;
    syssecond0.value = &timeval[6];

    add_symvar( global_dict, "time", timeval, strlen( timeval ), SI_no_subscript, SF_none );

}

/***************************************************************************/
/*  init_predefined_symbols     global symbols, not system                 */
/***************************************************************************/

static  void    init_predefined_symbols( void )
{
    char            wkstring[2];

    add_symvar( global_dict, "amp", "&", 1, SI_no_subscript, SF_is_AMP | SF_predefined );
    wkstring[0] = GML_CHAR_DEFAULT;
    wkstring[1] = '\0';
    add_symvar( global_dict, "gml", wkstring, 1, SI_no_subscript, SF_predefined );
}

/***************************************************************************/
/*  init_sysparm     this is separate as only known after cmdline          */
/*                   processing                                            */
/***************************************************************************/

void    init_sysparm( char *cmdline, char *banner )
{
    char            *p;

    p = strchr( cmdline, '(' );         // find parm start
    if( p == NULL ) {
        sysparm0.value = cmdline;       // empty cmdline
    } else {
        p++;
        SkipSpaces( p );                // over leading blanks
        sysparm0.value = p;

        p += strlen( p ) - 1;
        if( *p == ' ' ) {               // delete trailing blanks
            while( *p == ' ' ) {
                p--;
            }
            *++p = 0;                   // terminate string
        }
    }
    strupr( sysparm0.value );           // uppercase as wgml4 does

    syspdev0.value = g_dev_name;
    sysversion0.value = banner;
}


/***************************************************************************/
/*  init_sys_dict  initialize dictionary and some entries which do not     */
/*                 change very often                                       */
/***************************************************************************/

void    init_sys_dict( symdict_hdl *pdict )
{
    init_dict( pdict );

#define picka( var, flag )    pickl( var, flag )
#define pickc( var, flag )    pickl( var, flag )
#define pickk( var, flag )    pickl( var, flag )
#define pickl( var, flag )    link_sym( *pdict, &sys( var ) );
#include "gsyssym.h"
#undef pickl
#undef pickk
#undef pickc
#undef picka

#define SET_CHAR(p,v)   (p)[0]=v;(p)[1]='\0'
#define SET_STR(p,v)    strcpy((p),(v))

    init_date_time();                   // set up predefned global
    init_predefined_symbols();          // variables

    /***********************************************************************/
    /*  commented statements are perhaps  TBD                              */
    /***********************************************************************/

//  *sysadstr  =
//  *sysadevenstr  =
//  *sysadoddstr  =
    SET_CHAR( sysampstr, '&' );
    SET_CHAR( sysbcstr, 'Y' );
//  *sysbestr  =
    SET_CHAR( sysbsstr, 0x16 );
    SET_CHAR( sysbxstr, 'N' );
    SET_STR( sysbxcharstr, "UND" );
    SET_CHAR( sysccstr, 'N' );
//  *syscccstr =
    syschars0.value = CONST_OFF;
    sysco0.value    = CONST_ON;
    SET_CHAR( syscpstr, 'N' );
    SET_CHAR( syscontstr, 0x03 );
//  *syscpagesstr  =
//  *syscpcstr =
//  *syscpistr =
    SET_CHAR( syscwstr, CW_SEP_CHAR_DEFAULT );
//  *sysdfontsstr =
//  *sysdhsetstr =
//  *sysdpagestr =
    sysduplex0.value = CONST_OFF;
    SET_CHAR( sysfbstr, 'N' );
//  *sysfbcstr =
//  *sysfbfstr =
    SET_CHAR( sysfkstr, 'N' );
//  *sysfkcstr =
    SET_CHAR( sysfnstr, 'N' );
//  *sysfncstr =
//  *sysfontsstr =
//  *sysfsstr =
    SET_CHAR( sysgmlstr, GML_CHAR_DEFAULT );
//  *sysgutterstr =
//  *syshistr =
    SET_CHAR( syshnstr, 'N' );
//  *syshncstr =
//  *syshsstr =
    syshy0.value = CONST_OFF;
//  *syshycstr =
    SET_CHAR( syshyphstr, 'N' );        // hyphenation OFF is default; hyphenation ON not implemented
//  *sysinstr =
//  *sysinrstr =
//  *sysirstr =
    SET_CHAR( sysixjstr, '-' );
    SET_STR( sysixrefstr, ", " );
    sysju0.value = CONST_ON;
    syslayout0.value = CONST_OFF;
    SET_CHAR( syslistr, '.' );
    SET_CHAR( syslinbstr, ' ' );
//  *syslsstr =
//  *syslststr =
    SET_CHAR( sysmcstr, 'N' );
    SET_CHAR( sysmcsstr, '.' );
    sysmember0.value = NULL;            // member is never set
//  *sysnodeidstr =
//  *sysofstr =
    SET_CHAR( sysofflinestr, 'N' );
    SET_CHAR( sysonlinestr, 'Y' );
//  *sysoocstr =
//  *sysoutstr =
//  *syspagedstr =
//  *syspistr =
    SET_CHAR( syspixstr, '*' );
//  *sysplstr =
    SET_CHAR( sysplsstr, ',' );
//  *sysppagestr =
    SET_CHAR( sysprsstr, '-' );
    sysprt0.value = CONST_ON;
    SET_CHAR( syspsstr, '%' );
//  *syspwstr =
    sysquiet0.value = CONST_OFF;
    SET_CHAR( sysrbstr, ' ' );
//  *sysrecnostr =
//  *sysretstr  =
//  *sysrmneststr  =
    SET_CHAR( sysscstr, 'N' );
    SET_CHAR( sysscreenstr, 'N' );
//  *sysseqnostr =
//  *sysskcondstr =
//  *sysslstr =
//  *sysspcondstr =
    syssu0.value = CONST_ON;
#if defined( __DOS__ )
    syssys0.value = "DOS";
#elif defined( __OS2__ )
    syssys0.value = "OS2";
#elif defined( __NT__ )
    syssys0.value = "NT";
#elif defined( __UNIX__ )
  #if defined( __LINUX__ )
    syssys0.value = "LINUX";
  #elif defined( __OSX__ )
    syssys0.value = "OSX";
  #elif defined( __BSD__ )
    syssys0.value = "BSD";
  #endif
#endif
    SET_CHAR( systabstr, '\t' );
    SET_CHAR( systbstr, '\t' );
//  *systermtstr =
    SET_CHAR( systisetstr, ' ' );
//  *systitlestr =
//  *sysuseridstr =
//  *syswdstr =
//  *sysxtextstr =

    return;
}
