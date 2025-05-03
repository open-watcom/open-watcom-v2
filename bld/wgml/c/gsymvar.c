/****************************************************************************
*
*                            Open Watcom Project
*
*  Copyright (c) 2004-2009 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Implements symbolic variables (tables and access routines).
*
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/* Practice shows that WGML uses two fundamentally different kinds of      */
/* symbol dictionaries.                                                    */
/*  Local dictionaries used for macros and tags tend to contain only a     */
/* small handful of symbols, sometimes even no symbols at all. Local       */
/* dictionaries typically see only a small number of lookups, often        */
/* just 1-2, and almost never more than 20.                                */
/*  The global dictionary, on the other hand, might contain well over      */
/* a hundred symbols, and needs to satisfy many thousands of lookups,      */
/* even several hundred thousand lookups for complex documents.            */
/*  This disparate usage pattern implies that the global dictionary must   */
/* use a hash table for any kind of satisfactory performance; but for the  */
/* local dictionaries, trivial linked lists may still be the best option.  */
/* The short lifetimes and infrequent access pattern seen with the local   */
/* dictionaries make the additonal setup and teardown overhead             */
/* unlikely to be amortized.                                               */
/*  Even for larger dictionaries, a hash table reduces the number of       */
/* string comparisons to well under twice the number of lookups (i.e.      */
/* there are perhaps 1.5 string comparisons per lookup). Without hash      */
/* tables, there can easily be 100 string comparisons per lookup for the   */
/* global dictionary.                                                      */
/***************************************************************************/


/***************************************************************************/
/*  Private symbol dictionary type                                         */
/***************************************************************************/
typedef struct symdict {
    symvar      *first;         // first symbol in chain
    symvar      **htbl;         // hash table
    int         lookups;        // lookup counter
    int         symbols;        // symbol counter
    int         compares;       // strcmp counter
    bool        local;          // local/global flag
} symdict;

#define SYM_HASH_SIZE       241

static unsigned hashpjw( const char *s )
{
    unsigned    h;
    char        c;

    h = *s++;
    if( h != 0 ) {
        c = *s++;
        if( c != '\0' ) {
            h = ( h << 4 ) + c;
            for( ;; ) {
                h &= 0x0fff;
                c = *s++;
                if( c == '\0' )
                    break;
                h = ( h << 4 ) + c;
                h = ( h ^ (h >> 12) ) & 0x0fff;
                c = *s++;
                if( c == '\0' )
                    break;
                h = ( h << 4 ) + c;
                h = h ^ (h >> 12);
            }
        }
    }
    return( h );
}

static int sym_hash( const char *name )
{
    unsigned    hash;

    hash = hashpjw( name );
    hash %= SYM_HASH_SIZE;
    return( hash );
}


/***************************************************************************/
/*  init_dict      initialize symbol dictionary                            */
/***************************************************************************/

void    init_dict( symdict_hdl *pdict )
{
    symdict_hdl dict;

    dict = mem_alloc( sizeof( symdict ) );
    dict->first    = NULL;
    dict->htbl     = NULL;
    dict->lookups  = 0;
    dict->symbols  = 0;
    dict->compares = 0;
    dict->local    = true;
    *pdict = dict;

    if( dict == global_dict || dict == sys_dict ) {
        dict->htbl = mem_alloc( sizeof( void * ) * SYM_HASH_SIZE );
        memset( dict->htbl, 0, sizeof( void * ) * SYM_HASH_SIZE );
        dict->local = false;
    }

    return;
}


static void free_sym_chain( symvar * wk )
{
    symvar  *   wkn;
    symsub  *   ws;
    symsub  *   wsn;

    while( wk != NULL ) {
        if( !(wk->flags & no_free) ) {
            ws = wk->subscripts;
            while( ws != NULL ) {
                mem_free( ws->value );
                wsn = ws->next;
                mem_free( ws );
                ws = wsn;
            }
            mem_free( wk->sub_0->value );
            mem_free( wk->sub_0 );
            wkn = wk->next;
            mem_free( wk );
            wk = wkn;
        } else {
            wk = wk->next;      // skip entire symbol
        }
    }
    return;
}

/***************************************************************************/
/*  free_dict   free all symbol dictionary entries                         */
/***************************************************************************/

void    free_dict( symdict_hdl *pdict )
{
    symdict_hdl dict;
    int         i;

    dict = *pdict;
#if 0
    if( dict->compares > 1000 || dict->symbols > 25 || dict->lookups > 200 ) {
        printf( "dict %p, symbols:%6d, lookups:%8d, compares: %12d\n",
                dict, dict->symbols, dict->lookups, dict->compares );
    }
#endif

    if( dict->local ) {
        free_sym_chain( dict->first );
    } else {
        for( i = 0; i < SYM_HASH_SIZE; ++i ) {
            free_sym_chain( dict->htbl[i] );
        }
        mem_free( dict->htbl );
    }
    mem_free( dict );
    *pdict = NULL;
    return;
}


/***************************************************************************/
/*  print_sym_entry  print symbol with walue                               */
/***************************************************************************/

static void print_sym_entry( symvar * wk, int * symcnt, int * symsubcnt )
{
    symsub          *   ws;
    int                 len;
    bool                saveflag;
    static const char   fill[11] = "          ";

    saveflag    = ProcFlags.no_var_impl_err;
    ProcFlags.no_var_impl_err = true;   // suppress err msg

    len = strlen( wk->name );
    if( wk->subscript_used > 0 ) {
        out_msg( "Variable='%s'%s flags=%s%s%s%s%s%s%s "
                 "subscript_used=%d", wk->name, &fill[len],
                 wk->flags & deleted ? "deleted " : "",
                 wk->flags & local_var ? "local " : "",
                 wk->flags & auto_inc ? "auto_inc " : "",
                 wk->flags & predefined ? "predefined " : "",
                 wk->flags & ro ? "RO " : "",
                 wk->flags & no_free ? "no_free " : "",
                 wk->flags & access_fun ? "access_fun " : "",
                 wk->subscript_used );
    } else {
        out_msg( "Variable='%s'%s flags=%s%s%s%s%s%s%s ",
                 wk->name, &fill[len],
                 wk->flags & deleted ? "deleted " : "",
                 wk->flags & local_var ? "local " : "",
                 wk->flags & auto_inc ? "auto_inc " : "",
                 wk->flags & predefined ? "predefined " : "",
                 wk->flags & ro ? "RO " : "",
                 wk->flags & no_free ? "no_free " : "",
                 wk->flags & access_fun ? "access_fun " : "");
    }
    ws = wk->subscripts;
    if( wk->flags & subscripted ) {
        (*symsubcnt)++;
        out_msg( "\n" );
    } else {
        if( (wk->flags & access_fun) && (wk->varfunc != NULL) ) {
            (wk->varfunc)( wk );    // get current value
        }
        if( wk->sub_0->value == NULL) { // oops not initialized
            out_msg( "\n" );
        } else {
            out_msg("value='%s'\n", wk->sub_0->value );
        }
        (*symcnt)++;
    }
    if( wk->flags & subscripted ) {
        while( ws != NULL ) {
            out_msg( "   subscript= %8ld value='%s'\n",
                    ws->subscript, ws->value );
            ws = ws->next;
        }
    }
    ProcFlags.no_var_impl_err = saveflag;
}


/***************************************************************************/
/*  search symbol and subscript entry in specified  dictionary             */
/*  fills symsub structure pointer if found                                */
/*                                                                         */
/***************************************************************************/

int find_symvar( symdict_hdl dict, char * name, sub_index sub, symsub **symsubval )
{
    symvar      *wk;
    symsub      *ws;
    int         rc = 0;

    if( (*name == '$') && (dict != sys_dict) ) {// for sysxxx try system dict first
        rc = find_symvar( sys_dict, name, sub, symsubval );// recursion
        if( rc ) {
            return( rc );               // found predefined systemvariable
        }
    }
    if( dict->local ) {
        wk = dict->first;
    } else {
        int     hash;

        hash = sym_hash( name );
        wk = dict->htbl[hash];
    }

    *symsubval = NULL;
    dict->lookups++;

    while( wk != NULL) {
        dict->compares++;
        if( strcmp( wk->name, name ) == 0 ) {
            if( wk->flags & deleted ) {
                break;                  // symbol name is deleted
            }
            rc = 1;                     // symbol name found
            break;
        }
        wk = wk->next;
    }
    if( rc ) {
        *symsubval = wk->sub_0;         // return at least sub 0
        if( wk->flags & subscripted ) {
            if( (sub == no_subscript) || (sub == 0) ) {
                rc = 2;                 // subscript found
            } else {
                rc = 1;                 // name is found
                ws = wk->subscripts;
                while( ws != NULL ) {
                    if( sub == ws->subscript ) {
                        *symsubval = ws;// subscript found
                        rc = 2;
                        break;
                    }
                    ws = ws->next;
                }
            }
        } else {
            if( (wk->flags & access_fun) && (wk->varfunc != NULL) ) {
                (wk->varfunc)( wk );    // get value via special function
            }
            rc = 2;                     // not subscripted -> all found
        }
    }
    if( *symsubval != NULL ) {
        if( (*symsubval)->value == NULL ) {
            rc = 0;            // temporarily set not found for uninitialized
        }
    }
    return( rc );
}

/***************************************************************************/
/*  find_symvar_lcl     find local symbolic variable                       */
/*          if the dictionary is the local dict then                       */
/*          search up thru the local dictionaries up to the first file     */
/*          unless the symbol looks like an auto symbol (all numeric)      */
/***************************************************************************/

int find_symvar_lcl( symdict_hdl dict, char * name, sub_index sub, symsub **symsubval )
{
    char        *p;
    inputcb     *incbs;
    int         rc;
    symdict_hdl wk;

    rc = find_symvar( dict, name, sub, symsubval );
    if( rc || (strlen( name ) == 1) && (*name == *MAC_STAR_NAME) ) {
        return( rc );                   // found variable in specified dict or is *
    }

    p = name;                           // see if symbol name consists entirely of digits
    while( my_isdigit( *p ) ) {
        p++;
    }

    if( *p && (dict == input_cbs->local_dict) ) {   // not auto symbol and current dict is local dict

        /* search upwards thru all local dicts through first file local dict encountered */

        for( incbs = input_cbs->prev; incbs != NULL; incbs = incbs->prev ) {
            if( incbs->local_dict != NULL ) {
                wk = incbs->local_dict;
                rc = find_symvar( wk, name, sub, symsubval );
                if( rc ) {
                    break;              // found variable
                }
            if( incbs->fmflags & II_file ) break;   // stop at first file
            }
        }
    }
    return( rc );
}


/***************************************************************************/
/*  search symbol and subscript entry in specified  dictionary             */
/*  fills symsub structure pointer if found                                */
/*  finds deleted variables too internal routine                           */
/***************************************************************************/

static  int find_symvar_del( symdict_hdl dict, char *name, sub_index sub,
                             symsub **symsubval, symvar **delentry )
{
    symvar      *wk;
    symsub      *ws;
    int         rc = 0;

    if( (*name == '$') && (dict != sys_dict) ) {// for sysxxx try system dict first
        rc = find_symvar_del( sys_dict, name, sub, symsubval, delentry );   // recursion
        if( rc ) {
            return( rc );               // found predefined systemvariable
        }
    }
    *symsubval = NULL;

    if( dict->local ) {
        wk = dict->first;
    } else {
        int     hash;

        hash = sym_hash( name );
        wk = dict->htbl[hash];
    }

    while( wk != NULL) {
        dict->compares++;
        if( strcmp( wk->name, name ) == 0 ) {
            if( wk->flags & deleted ) {
                *delentry = wk;
                rc = -1;                // deleted symbol found
            } else {
                rc = 1;                 // symbol name found
            }
            break;
        }
        wk = wk->next;
    }
    if( rc > 0 ) {                      // non deleted symbol found

        *symsubval = wk->sub_0;         // return at least sub 0
        if( wk->flags & subscripted ) {
            if( (sub == no_subscript) || (sub == 0) ) {
                rc = 2;                 // subscript found
            } else {
                rc = 1;                 // name is found
                ws = wk->subscripts;
                while( ws != NULL ) {
                    if( sub == ws->subscript ) {
                        *symsubval = ws;// subscript found
                        rc = 2;
                        break;
                    }
                    ws = ws->next;
                }
            }
        } else {
            if( sub == 0 ) {
                rc = 2;                     // no subscripting -> all found
            } else {
                rc = 1;                     // name found but not subscript
            }
        }
    }
    return( rc );
}

/***************************************************************************/
/*  check subscript for allowed range                                      */
/***************************************************************************/

static bool check_subscript( sub_index sub )
{
    if( sub != no_subscript ) {
        if( (sub < min_subscript) || (sub > max_subscript) ) {
            // SC--076 Subscript index must be between -1000000 and 1000000
            char    linestr[NUM2STR_LENGTH];

            sprintf( linestr, "%d", sub );
            xx_line_err_c( err_sub_out_of_range, linestr );
        }
    }
    return( true );
}

/***************************************************************************/
/*  add_symvar_sub add value and subscript to base symbol entry            */
/*           the subscripts are added in ascending order                   */
/***************************************************************************/

static bool add_symvar_sub( symvar * var, char * val, sub_index sub, symsub * * nsub )
{
    symsub  *   newsub;
    symsub  *   ws;
    symsub  *   wsv;
//  char        sub_cnt[NUM2STR_LENGTH];

    if( var->flags & ro ) {             // value readonly
        return( true );                 // pretend success as wgml 4.0 does
    }
    if( sub != no_subscript ) {
        if( !check_subscript( sub ) ) {
            return( false );
        } else {
            var->subscript_used++;
                                             /* update special sub 0 entry */
#if 0
            sprintf( sub_cnt, "%d", var->subscript_used );
            if( strlen( var->sub_0->value ) < strlen( sub_cnt ) ) {// need more room
                var->sub_0->value = mem_realloc( var->sub_0->value,
                                                 strlen( sub_cnt ) + 1 );
            }
            strcpy( var->sub_0->value, sub_cnt );
#else
            sprintf( var->sub_0->value, "%d", var->subscript_used );  // TBD
#endif

            var->flags |= subscripted;
            if( (var->flags & auto_inc) && (sub == var->last_auto_inc + 1) ) {
                var->last_auto_inc++;
            }
        }

        newsub            = mem_alloc( sizeof( symsub ) );
        newsub->next      = NULL;
        newsub->base      = var;
        newsub->subscript = sub;
        newsub->value     = mem_alloc( strlen( val ) + 1 );
        strcpy( newsub->value, val );

/*
 * insert subscript in ascending sort order
 */
        ws  = var->subscripts;
        if( ws == NULL || (sub < ws->subscript) ) {
            newsub->next    = var->subscripts;
            var->subscripts = newsub;
        } else {
            while( (ws != NULL) ) {
                if( sub > ws->subscript ) {
                    wsv = ws;
                    ws  = ws->next;
                } else {
                    break;
                }
            }
            newsub->next = ws;
            wsv->next    = newsub;
        }
    } else {                            // unsubscripted variable
        newsub = var->sub_0;
        if( strlen( newsub->value ) < strlen( val ) ) { // need more room
            newsub->value = mem_realloc( newsub->value, strlen( val ) + 1 );
        }
        strcpy( newsub->value, val );
    }
    *nsub = newsub;
    return( true );
}

/***************************************************************************/
/*  link_sym    add existing symbol to dictionary                          */
/***************************************************************************/

void link_sym( symdict_hdl dict, symvar * sym )
{
    if( dict->local ) {
        sym->next   = dict->first;
        dict->first = sym;
    } else {
        int     hash;

        hash = sym_hash( sym->name );
        sym->next = dict->htbl[hash];
        dict->htbl[hash] = sym;
    }

    dict->symbols++;
    return;
}


/***************************************************************************/
/*  add_symsym  add symbol base entry and prepare subscript 0 entry        */
/***************************************************************************/

static void add_symsym( symdict_hdl dict, char * name, symbol_flags f, symvar * * n )
{
    symvar  *   new;
    symsub  *   newsub;
    int         k;

    new = mem_alloc( sizeof( symvar ) );

    for( k = 0; k < SYM_NAME_LENGTH; k++ ) {
       new->name[k] = name[k];
       if( !name[k] ) {
          break;
       }
    }
    for( ; k <= SYM_NAME_LENGTH; k++ ) {
       new->name[k] = '\0';
    }
    new->next = NULL;
    new->last_auto_inc  = 0;
    new->subscript_used = 0;
    new->subscripts = NULL;
    new->flags = f & ~deleted;

    newsub = mem_alloc( sizeof( symsub ) );
    new->sub_0 = newsub;
    newsub->next      = NULL;
    newsub->base      = new;
    newsub->subscript = 0;
    newsub->value     = mem_alloc( 12 + 1 );// for min subscript as string
                                            // -1000000
    *(newsub->value)  = '0';
    *(newsub->value + 1) = '\0';

    *n = new;
    link_sym( dict, new );

    return;
}


/***************************************************************************/
/*  add_symvar_addr  add symbol with subscript and value                   */
/*  with    returning ptr to symsub entry                                  */
/***************************************************************************/

int add_symvar_addr( symdict_hdl dict, char * name, char * val,
                     sub_index subscript, symbol_flags f, symsub * * sub )
{
    symvar  *   new = NULL;
    symsub  *   newsub = NULL;
    int         dummyi = 0;
    int         rc;
    bool        ok;

    if( !check_subscript( subscript ) ) {
        rc = 3;
    } else {
        rc = find_symvar_del( dict, name, subscript, &newsub, &new );
        switch ( rc ) {
        case -1 :                       // deleted symbol found
            new->flags &= ~deleted;     // reset deleted switch
            ok = add_symvar_sub( new, val, subscript, sub );
            if( !ok ) {
                rc = 3;
            } else {
                if( GlobalFlags.firstpass && (input_cbs->fmflags & II_research) ) {
                    if( rc < 3 ) {
                        print_sym_entry( new, &dummyi, &dummyi );
                    }
                }
            }
            break;
        case 0 :                        // nothing found
            add_symsym( dict, name, f, &new );
            ok = add_symvar_sub( new, val, subscript, sub );
            if( !ok ) {
                rc = 3;
            } else {
                if( GlobalFlags.firstpass && (input_cbs->fmflags & II_research) ) {
                    if( rc < 3 ) {
                        print_sym_entry( new, &dummyi, &dummyi );
                    }
                }
            }
            break;
        case 1 :                        // symbol found, but not subscript
            newsub->base->flags &= ~deleted;// reset deleted switch
            newsub->base->flags |= f;   // use flags given
            ok = add_symvar_sub( newsub->base, val, subscript, sub );
            if( !ok ) {
                rc = 3;
            } else {
                if( GlobalFlags.firstpass && (input_cbs->fmflags & II_research) ) {
                    if( rc < 3 ) {
                        print_sym_entry( newsub->base, &dummyi, &dummyi );
                    }
                }
            }
            break;
        case 2 :              // symbol + subscript found, or not subscripted
            newsub->base->flags &= ~deleted;// reset deleted switch
            newsub->base->flags |= f;   // use flags given
            if( (newsub->base->flags & ro) || strcmp( newsub->value, val ) == 0 ) {
                ;             // do nothing var is readonly or value is unchanged
            } else {
                if( strlen( newsub->value ) < strlen( val ) ) { // need more room
                    newsub->value = mem_realloc( newsub->value, strlen( val ) + 1 );
                }
                strcpy( newsub->value, val );
            }
            *sub = newsub;
            if( GlobalFlags.firstpass && (input_cbs->fmflags & II_research) ) {
                if( rc < 3 ) {
                    print_sym_entry( newsub->base, &dummyi, &dummyi );
                }
            }
            break;
        default:
            xx_line_err_c( err_logic_err, __FILE__ );
        }
    }
    return( rc );
}


/***************************************************************************/
/*  add_symvar  add symbol with subscript and value                        */
/*  without returning ptr to symsub entry                                  */
/***************************************************************************/

int add_symvar( symdict_hdl dict, char * name, char * val, sub_index subscript, symbol_flags f )
{
    symsub  *   newsub;

    return( add_symvar_addr( dict, name, val, subscript, f, &newsub ) );
}

static void reset_auto_inc_chain( symvar * wk )
{
    symsub  *   ws;

    while( wk != NULL ) {

        if( wk->flags & auto_inc ) {

            wk->sub_0->value[0] = '0';
            wk->sub_0->value[1] = '\0';

            wk->flags |= deleted;
            wk->subscript_used = 0;
            while( (ws = wk->subscripts) != NULL ) {
                wk->subscripts = ws->next;
                mem_free( ws->value );
                mem_free( ws );
            }
        }
        wk->last_auto_inc = 0;
        wk = wk->next;
    }
    return;
}

/***************************************************************************/
/*  reset_auto_inc_dict  reset auto_inc value for passes 2 - n             */
/*  and set variable as deleted                                            */
/***************************************************************************/

void    reset_auto_inc_dict( symdict_hdl dict )
{
    int         i;

    if( dict->local ) {
        reset_auto_inc_chain( dict->first );
    } else {
        for( i = 0; i < SYM_HASH_SIZE; ++i ) {
            reset_auto_inc_chain( dict->htbl[i] );
        }
    }
    return;
}


/***************************************************************************/
/*  print_sym_dict  output all of the symbol dictionary                    */
/***************************************************************************/

void    print_sym_dict( symdict_hdl dict )
{
    symvar      *   wk;
    char        *   lgs;
    int             symcnt;
    int             symsubcnt;

    wk          = dict->first;
    symcnt      = 0;
    symsubcnt   = 0;

    if( dict == sys_dict ) {
        lgs = "System";
    } else if( dict == global_dict ) {
        lgs = "Global";
    } else {
        lgs = "Local";
    }
    out_msg( "\n%s list of symbolic variables:\n", lgs );
    while( wk != NULL ) {
        print_sym_entry( wk, &symcnt, &symsubcnt );
        wk = wk->next;
    }
    out_msg( "\nUnsubscripted symbols defined: %d\n", symcnt );
    out_msg( "Subscripted   symbols defined: %d\n", symsubcnt );
    return;
}

