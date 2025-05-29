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

    if( dict == global_dict
      || dict == sys_dict ) {
        dict->htbl = mem_alloc( sizeof( void * ) * SYM_HASH_SIZE );
        memset( dict->htbl, 0, sizeof( void * ) * SYM_HASH_SIZE );
        dict->local = false;
    }

    return;
}

static void free_value( symsub *ws )
{
    if( ws->size > 0 )
        mem_free( ws->value );
    mem_free( ws );
}

static void free_sym_chain( symvar *wk )
{
    symvar          *wkn;
    symsub          *ws;

    for( ; wk != NULL; wk = wkn ) {
        wkn = wk->next;
        if( (wk->flags & SF_no_free) == 0 ) {
            while( (ws = wk->subscripts) != NULL ) {
                wk->subscripts = ws->next;
                free_value( ws );
            }
            free_value( wk->sub_0 );
            mem_free( wk );
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
    if( dict->compares > 1000
      || dict->symbols > 25
      || dict->lookups > 200 ) {
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


/***************************************************************************
 * resize_and_copy_value
 *  resize value buffer to hold new value
 *  and copy data into it
 */
static void resize_value( symsub *val, unsigned size )
{
    if( val->size < size ) {// need more room
        if( val->size > 0 ) {
            val->value = mem_realloc( val->value, size + 1 );
        } else {
            val->value = mem_alloc( size + 1 );
        }
        val->size = size;
    }
}

void resize_and_copy_value( symsub *val, const char *src )
{
    resize_value( val, strlen( src ) );
    strcpy( val->value, src );
}

static void resize_and_copy_value_len( symsub *val, const char *src, unsigned size )
{
    resize_value( val, size );
    strncpy( val->value, src, size );
    val->value[size] = '\0';
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
                 wk->flags & SF_deleted ? "deleted " : "",
                 wk->flags & SF_local_var ? "local " : "",
                 wk->flags & SF_auto_inc ? "auto_inc " : "",
                 wk->flags & SF_predefined ? "predefined " : "",
                 wk->flags & SF_ro ? "RO " : "",
                 wk->flags & SF_no_free ? "no_free " : "",
                 wk->flags & SF_access_fun ? "access_fun " : "",
                 wk->subscript_used );
    } else {
        out_msg( "Variable='%s'%s flags=%s%s%s%s%s%s%s ",
                 wk->name, &fill[len],
                 wk->flags & SF_deleted ? "deleted " : "",
                 wk->flags & SF_local_var ? "local " : "",
                 wk->flags & SF_auto_inc ? "auto_inc " : "",
                 wk->flags & SF_predefined ? "predefined " : "",
                 wk->flags & SF_ro ? "RO " : "",
                 wk->flags & SF_no_free ? "no_free " : "",
                 wk->flags & SF_access_fun ? "access_fun " : "");
    }
    if( wk->flags & SF_subscripted ) {
        (*symsubcnt)++;
        out_msg( "\n" );
        for( ws = wk->subscripts; ws != NULL; ws = ws->next ) {
            out_msg( "   subscript= %8ld value='%s'\n", ws->subscript, ws->value );
        }
    } else {
        if( (wk->flags & SF_access_fun)
          && (wk->varfunc != NULL) ) {
            (wk->varfunc)( wk );    // get current value
        }
        if( wk->sub_0->value == NULL) { // oops not initialized
            out_msg( "\n" );
        } else {
            out_msg("value='%s'\n", wk->sub_0->value );
        }
        (*symcnt)++;
    }
    ProcFlags.no_var_impl_err = saveflag;
}


/***************************************************************************/
/*  search symbol and subscript entry in specified  dictionary             */
/*  fills symsub structure pointer if found                                */
/*                                                                         */
/***************************************************************************/

int find_symvar( symdict_hdl dict, const char *name, sub_index subscript, symsub **symsubval )
{
    symvar      *wk;
    symsub      *ws;
    int         rc = 0;

    if( (*name == '$')
      && (dict != sys_dict) ) {         // for sysxxx try system dict first
        rc = find_symvar( sys_dict, name, subscript, symsubval );// recursion
        if( rc ) {
            return( rc );               // found predefined systemvariable
        }
    }

    *symsubval = NULL;
    dict->lookups++;

    if( dict->local ) {
        wk = dict->first;
    } else {
        int     hash;

        hash = sym_hash( name );
        wk = dict->htbl[hash];
    }
    for( ; wk != NULL; wk = wk->next ) {
        dict->compares++;
        if( strcmp( wk->name, name ) == 0 ) {
            if( wk->flags & SF_deleted ) {
                break;                  // symbol name is deleted
            }
            rc = 1;                     // symbol name found
            break;
        }
    }

    if( rc ) {
        *symsubval = wk->sub_0;         // return at least sub 0
        if( wk->flags & SF_subscripted ) {
            if( (subscript == SI_no_subscript)
              || (subscript == SI_none) ) {
                rc = 2;                 // subscript found
            } else {
                rc = 1;                 // name is found
                for( ws = wk->subscripts; ws != NULL; ws = ws->next ) {
                    if( subscript == ws->subscript ) {
                        *symsubval = ws;// subscript found
                        rc = 2;
                        break;
                    }
                }
            }
        } else {
            if( (wk->flags & SF_access_fun)
              && (wk->varfunc != NULL) ) {
                (wk->varfunc)( wk );    // get value via special function
            }
            rc = 2;                     // not subscripted -> all found
        }
    }
    if( *symsubval != NULL ) {
        if( (*symsubval)->value == NULL ) {
            rc = 0;                     // temporarily set not found for uninitialized
        }
    }
    return( rc );
}

/***************************************************************************/
/*  find_symvar_sym                                                        */
/*          find symbolic variable (local or global)                       */
/*          if the dictionary is the local dict then                       */
/*          search up thru the local dictionaries up to the first file     */
/*          unless the symbol looks like an auto symbol (all numeric)      */
/***************************************************************************/

int find_symvar_sym( symvar *sym, sub_index subscript, symsub **symsubval )
{
    const char      *p;
    inputcb         *incbs;
    int             rc;
    symdict_hdl     dict;

    if( sym->flags & SF_local_var ) {  // lookup var in dict
        dict = input_cbs->local_dict;
    } else {
        dict = global_dict;
    }
    rc = find_symvar( dict, sym->name, subscript, symsubval );
    if( rc
      || ( sym->name[0] == *MAC_STAR_NAME
      && sym->name[1] == '\0' ) ) {
        return( rc );                   // found variable in specified dict or is *
    }

    p = sym->name;                           // see if symbol name consists entirely of digits
    while( my_isdigit( *p ) ) {
        p++;
    }

    if( *p != '\0'
      && (dict == input_cbs->local_dict) ) {   // not auto symbol and current dict is local dict

        /* search upwards thru all local dicts through first file local dict encountered */

        for( incbs = input_cbs->prev; incbs != NULL; incbs = incbs->prev ) {
            if( incbs->local_dict != NULL ) {
                rc = find_symvar( incbs->local_dict, sym->name, subscript, symsubval );
                if( rc ) {
                    break;              // found variable
                }
                if( incbs->fmflags & II_file ) {
                    break;   // stop at first file
                }
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

static  int find_symvar_del( symdict_hdl dict, const char *name, sub_index subscript,
                             symsub **symsubval, symvar **delentry )
{
    symvar      *wk;
    symsub      *ws;
    int         rc = 0;

    if( (*name == '$')
      && (dict != sys_dict) ) {// for sysxxx try system dict first
        rc = find_symvar_del( sys_dict, name, subscript, symsubval, delentry );   // recursion
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
    for( ; wk != NULL; wk = wk->next ) {
        dict->compares++;
        if( strcmp( wk->name, name ) == 0 ) {
            if( wk->flags & SF_deleted ) {
                *delentry = wk;
                rc = -1;                // deleted symbol found
            } else {
                rc = 1;                 // symbol name found
            }
            break;
        }
    }
    if( rc > 0 ) {                      // non deleted symbol found
        *symsubval = wk->sub_0;         // return at least sub 0
        if( wk->flags & SF_subscripted ) {
            if( (subscript == SI_no_subscript)
              || (subscript == SI_none) ) {
                rc = 2;                 // subscript found
            } else {
                rc = 1;                 // name is found
                for( ws = wk->subscripts; ws != NULL; ws = ws->next ) {
                    if( subscript == ws->subscript ) {
                        *symsubval = ws;// subscript found
                        rc = 2;
                        break;
                    }
                }
            }
        } else {
            if( subscript == SI_none ) {
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

static bool check_subscript( sub_index subscript )
{
    if( subscript != SI_no_subscript ) {
        if( (subscript < SI_min_subscript)
          || (subscript > SI_max_subscript) ) {
            // SC--076 Subscript index must be between -1000000 and 1000000
            char    linestr[NUM2STR_LENGTH + 1];

            sprintf( linestr, "%d", subscript );
            xx_line_err_exit_c( ERR_SUB_OUT_OF_RANGE, linestr );
            /* never return */
        }
    }
    return( true );
}

/***************************************************************************/
/*  add_symvar_sub add value and subscript to base symbol entry            */
/*           the subscripts are added in ascending order                   */
/***************************************************************************/

static bool add_symvar_sub( symvar *var, const char *val, unsigned len, sub_index subscript, symsub **nsub )
{
    symsub  *   newsub;
    symsub  *   ws;
    symsub  *   wsv;
//  char        sub_cnt[NUM2STR_LENGTH + 1];

    if( var->flags & SF_ro ) {          // value readonly
        return( true );                 // pretend success as wgml 4.0 does
    }
    if( subscript != SI_no_subscript ) {
        if( !check_subscript( subscript ) ) {
            return( false );
        }
        var->subscript_used++;
        /* update special sub 0 entry */
#if 0
        sprintf( sub_cnt, "%d", var->subscript_used );
        resize_and_copy_value( var->sub_0, sub_cnt );
#else
        sprintf( var->sub_0->value, "%d", var->subscript_used );  // TBD
#endif
        var->flags |= SF_subscripted;
        if( (var->flags & SF_auto_inc)
          && (subscript == var->last_auto_inc + 1) ) {
            var->last_auto_inc++;
        }
        newsub            = mem_alloc( sizeof( symsub ) );
        newsub->next      = NULL;
        newsub->base      = var;
        newsub->subscript = subscript;
        newsub->size      = len;
        newsub->value     = mem_alloc( len + 1 );
        strncpy( newsub->value, val, len );
        newsub->value[len] = '\0';

/*
 * insert subscript in ascending sort order
 */
        ws  = var->subscripts;
        if( ws == NULL
          || (subscript < ws->subscript) ) {
            newsub->next    = var->subscripts;
            var->subscripts = newsub;
        } else {
            while( (ws != NULL) ) {
                if( subscript > ws->subscript ) {
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
        resize_and_copy_value_len( newsub, val, len );
    }
    *nsub = newsub;
    return( true );
}

/***************************************************************************/
/*  link_sym    add existing symbol to dictionary                          */
/***************************************************************************/

void link_sym( symdict_hdl dict, symvar *sym )
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

static symvar *add_symsym( symdict_hdl dict, const char *name, symbol_flags f )
{
    symvar      *new;
    symsub      *newsub;

    new = mem_alloc( sizeof( symvar ) );
    new->next = NULL;
    strcpy( new->name, name );
    new->last_auto_inc  = 0;
    new->subscript_used = 0;
    new->subscripts = NULL;
    new->flags = f & ~SF_deleted;

    newsub = mem_alloc( sizeof( symsub ) );
    new->sub_0 = newsub;
    newsub->next      = NULL;
    newsub->base      = new;
    newsub->subscript = 0;
    newsub->size      = NUM2STR_LENGTH;
    newsub->value     = mem_alloc( NUM2STR_LENGTH + 1 );
    newsub->value[0]  = '0';
    newsub->value[1]  = '\0';

    link_sym( dict, new );

    return( new );
}


/***************************************************************************/
/*  add_symvar_addr  add symbol with subscript and value                   */
/*  with    returning ptr to symsub entry                                  */
/***************************************************************************/

int add_symvar_addr( symdict_hdl dict, const char *name, const char *val, unsigned len,
                     sub_index subscript, symbol_flags f, symsub **sub )
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
        case -1: // deleted symbol found
            new->flags &= ~SF_deleted;     // reset deleted switch
            ok = add_symvar_sub( new, val, len, subscript, sub );
            if( !ok ) {
                rc = 3;
            } else {
                if( GlobalFlags.firstpass
                  && (input_cbs->fmflags & II_research) ) {
                    if( rc < 3 ) {
                        print_sym_entry( new, &dummyi, &dummyi );
                    }
                }
            }
            break;
        case 0: // nothing found
            new = add_symsym( dict, name, f );
            ok = add_symvar_sub( new, val, len, subscript, sub );
            if( !ok ) {
                rc = 3;
            } else {
                if( GlobalFlags.firstpass
                  && (input_cbs->fmflags & II_research) ) {
                    if( rc < 3 ) {
                        print_sym_entry( new, &dummyi, &dummyi );
                    }
                }
            }
            break;
        case 1: // symbol found, but not subscript
            newsub->base->flags &= ~SF_deleted;// reset deleted switch
            newsub->base->flags |= f;   // use flags given
            ok = add_symvar_sub( newsub->base, val, len, subscript, sub );
            if( !ok ) {
                rc = 3;
            } else {
                if( GlobalFlags.firstpass
                  && (input_cbs->fmflags & II_research) ) {
                    if( rc < 3 ) {
                        print_sym_entry( newsub->base, &dummyi, &dummyi );
                    }
                }
            }
            break;
        case 2: // symbol + subscript found, or not subscripted
            newsub->base->flags &= ~SF_deleted;// reset deleted switch
            newsub->base->flags |= f;   // use flags given
            if( (newsub->base->flags & SF_ro)
              || strncmp( newsub->value, val, len ) == 0 ) {
                /*
                 * do nothing var is readonly or value is unchanged
                 */
            } else {
                resize_and_copy_value_len( newsub, val, len );
            }
            *sub = newsub;
            if( GlobalFlags.firstpass
              && (input_cbs->fmflags & II_research) ) {
                if( rc < 3 ) {
                    print_sym_entry( newsub->base, &dummyi, &dummyi );
                }
            }
            break;
        default:
            xx_line_err_exit_c( ERR_LOGIC_ERR, __FILE__ );
            /* never return */
        }
    }
    return( rc );
}


/***************************************************************************/
/*  add_symvar  add symbol with subscript and value                        */
/*  without returning ptr to symsub entry                                  */
/***************************************************************************/

int add_symvar( symdict_hdl dict, const char *name, const char *val, unsigned len, sub_index subscript, symbol_flags f )
{
    symsub          *newsub;

    return( add_symvar_addr( dict, name, val, len, subscript, f, &newsub ) );
}

int add_symvar_sym( symvar *sym, const char *val, unsigned len, sub_index subscript, symbol_flags f )
{
    symsub          *newsub;
    symdict_hdl     dict;

    if( sym->flags & SF_local_var ) {
        dict = input_cbs->local_dict;
    } else {
        dict = global_dict;
    }
    return( add_symvar_addr( dict, sym->name, val, len, subscript, f, &newsub ) );
}

static void reset_auto_inc_chain( symvar * wk )
{
    symsub          *ws;

    for( ; wk != NULL; wk = wk->next ) {

        if( wk->flags & SF_auto_inc ) {

            wk->sub_0->value[0] = '0';
            wk->sub_0->value[1] = '\0';

            wk->flags |= SF_deleted;
            wk->subscript_used = 0;
            while( (ws = wk->subscripts) != NULL ) {
                wk->subscripts = ws->next;
                free_value( ws );
            }
        }
        wk->last_auto_inc = 0;
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
    for( wk = dict->first; wk != NULL; wk = wk->next ) {
        print_sym_entry( wk, &symcnt, &symsubcnt );
    }
    out_msg( "\nUnsubscripted symbols defined: %d\n", symcnt );
    out_msg( "Subscripted   symbols defined: %d\n", symsubcnt );
    return;
}

