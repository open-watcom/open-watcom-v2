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


#ifndef NDEBUG

#include "plusplus.h"

#include "vbuf.h"
#include "ring.h"
#include "fmtsym.h"
#include "fmttype.h"
#include "fnovload.h"
#include "dbg.h"

static const char *rankNames[] = {
#include "fnovrank.h"
        "OV_RANK_INVALID"
};
static const char *resultNames[] = {
#include "fnovrslt.h"
        "FNOV_INVALID"
};

typedef enum
{
    PRINT_THIS, PRINT_RETURN, PRINT_DEFAULT
} PRINT_RANK_FORMAT;


static void printControl( FNOV_CONTROL control )
/**********************************************/
// pretty display of scalar ranking information
{
    if( control != FNC_DEFAULT ) {
        printf( "  control=( " );
        if( control & FNC_EXCLUDE_ELLIPSIS ) {
            printf( "exclude_ellipsis " );
        }
        if( control & FNC_EXCLUDE_UDCONV ) {
            printf( "exclude_udconv " );
        }
        if( control & FNC_RANK_RETURN ) {
            printf( "rank_return " );
        }
        if( control & FNC_DISTINCT_CHECK ) {
            printf( "distinct_check " );
        }
        if( control & FNC_MEMBER ) {
            printf( "member " );
        }
        if( control & FNC_ONLY_SYM ) {
            printf( "only_sym " );
        }
        if( control & FNC_TEMPLATE ) {
            printf( "template " );
        }
        if( control & FNC_STDOPS ) {
            printf( "stdops " );
        }
        if( control & FNC_EXCLUDE_CONV ) {
            printf( "exclude_conv " );
        }
        printf( ")" );
    }
    printf( "\n" );
}

static void printScalar( FNOV_SCALAR *scalar, bool is_udc )
/*********************************************************/
// pretty display of scalar ranking information
{
    if( is_udc && scalar->not_exact ) {
        printf( "Not Exact(%d) ", scalar->not_exact );
    }
    if( scalar->trivial ) {
        printf( "Trivial(%d) ", scalar->trivial );
    }
    if( scalar->promotion ) {
        printf( "Promotion(%d) ", scalar->promotion );
    }
    if( scalar->standard ) {
        printf( "Standard(%d) ", scalar->standard );
    }
    if( scalar->udcnv > 0 ) {
        printf( "UDC(%d) ", scalar->udcnv );
    }
    if( is_udc ) {
        printf( "\n" );
    }
}

static void printArgs( arg_list *args )
/*************************************/
{
    int i;
    VBUF prefix, suffix, flags;

    FormatTypeModFlags( args->qualifier, &flags );
    printf( "    'this qualifier': '%s'\n", VbufString( &flags ) );
    VbufFree( &flags );
    for( i = 0 ; i < args->num_args ; i++ ) {
        FormatType( args->type_list[i], &prefix, &suffix );
        printf( "    [%d]: '%s<id> %s'\n", i+1, VbufString( &prefix ), VbufString( &suffix ) );
        VbufFree( &prefix );
        VbufFree( &suffix );
    }
}

static void printRank( int num_args, FNOV_RANK *rank, PRINT_RANK_FORMAT fmt )
/***************************************************************************/
{
    int i;

    for( i = 0 ; i < num_args ; i++ ) {
        if( fmt == PRINT_THIS ) {
            printf( "    'this': %s ", rankNames[rank->rank] );
        } else if( fmt == PRINT_RETURN ) {
            printf( "    Return: %s ", rankNames[rank->rank] );
        } else {
            printf( "    [%d]: %s ", i+1, rankNames[rank->rank] );
        }
        if( rank->userdef ) {
            printControl( rank->control );
            printf( "\tUsrDef Input: " );
            printScalar( &rank->u.ud.in, TRUE );
            printf( "\tUsrDef Output: " );
            printScalar( &rank->u.ud.out, TRUE );
        } else {
            printScalar( &rank->u.no_ud, FALSE );
            printControl( rank->control );
        }
        rank++;
    }
}

void PrintFnovList( FNOV_LIST *root )
/***********************************/
// pretty display of list of overloaded function rankings
{
    int         i;
    FNOV_LIST   *entry;
    VBUF        name;

    i = 1;
    RingIterBeg( root, entry ) {
        printf( "[%d]: '%s'", i++, FormatSym( entry->sym, &name ) );
        VbufFree( &name );
        if( entry->member || entry->stdops ) {
            printf( " flags=( " );
            if( entry->member ) {
                printf( "member " );
            }
            if( entry->stdops ) {
                printf( "stdops " );
            }
            printf( ")" );
        }
        printf( "\n" );
        if( SymIsThisFuncMember( entry->sym ) ) {
            printRank( 1, &entry->thisrank, PRINT_THIS );
        }
        if( entry->rankvector == NULL ) {
            printf( "    (no rank available)\n" );
        } else {
            if( entry->rankvector->control & FNC_RANK_RETURN ) {
                printRank( 1, entry->rankvector, PRINT_RETURN );
            } else {
                printRank( entry->alist->num_args
                         , entry->rankvector
                         , PRINT_DEFAULT );
            }
        }
    } RingIterEnd( entry )
}

void PrintFnovResolution( FNOV_RESULT result, arg_list *args,
/***********************************************************/
    FNOV_LIST *match, FNOV_LIST *reject, SYMBOL sym )
// pseudo pretty display of overloaded ranking information
{
    int         length;
    VBUF        name;

    if( sym != NULL ) {
        FormatSym( sym, &name );
    } else if( match != NULL ) {
        FormatSym( match->sym, &name );
    } else if( reject != NULL ) {
        FormatSym( reject->sym, &name );
    } else {
        VbufInit( &name );
    }
    printf( " Result: %s\n", resultNames[result] );
    if( VbufLen( &name ) > 0 ) {
        printf( "Symbol: '%s'", VbufString( &name ) );
        VbufFree( &name );
        length = RingCount( match ) + RingCount( reject );
        if( length > 0 ) {
            printf( " occurs %d time%s",
                    length,
                    (length != 1 ? "s " : " " ) );
        }
    }
    printf( "\n" );

    if( sym == NULL && args != NULL ) {
        printf( "Ranked Arguments:\n" );
        printArgs( args );
    }
    if( match != NULL ) {
        printf( "Matching Functions:\n" );
        PrintFnovList( match );
    }
    if( reject != NULL ) {
        printf( "Rejected Functions:\n" );
        PrintFnovList( reject );
    }
}

#endif
