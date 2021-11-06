/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  DIG interface client callback routines.
*
****************************************************************************/


#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include "wio.h"
#include "common.h"
#include "dip.h"
#include "dipimp.h"
#include "dipcli.h"
#include "aui.h"
#include "sampinfo.h"
#include "msg.h"
#include "myassert.h"
#include "support.h"
#include "dipinter.h"
#include "wpstart.h"
#include "wpdata.h"


//#include "support.def"
//#include "msg.def"

STATIC char         *errMsgText( dip_status );
STATIC bool         loadDIP( const char *, bool, bool );

STATIC dip_status   DIPStatus;



void DIPCLIENTRY( ImageUnload )( mod_handle mh )
/**********************************************/
{
    /* unused parameters */ (void)mh;
}



void DIPCLIENTRY( MapAddr )( addr_ptr * addr, void * d )
/******************************************************/
{
    MapAddressToActual( (image_info *)d, addr );
}



imp_sym_handle *DIPCLIENTRY( SymCreate )( imp_image_handle *iih, void *d )
/************************************************************************/
{
    /* unused parameters */ (void)iih; (void)d;

    return( NULL );
}



dip_status DIPCLIENTRY( ItemLocation )( location_context * lc,
                            context_item ci, location_list * ll )
/***************************************************************/
{
    /* unused parameters */ (void)lc; (void)ci; (void)ll;

    return( DS_ERR | DS_NO_CONTEXT );
}



dip_status DIPCLIENTRY( AssignLocation )( location_list * dst,
                    const location_list * src, unsigned long size )
/*****************************************************************/
{
    /* unused parameters */ (void)dst; (void)src; (void)size;

    return( DS_ERR );
}



dip_status DIPCLIENTRY( SameAddrSpace )( address a, address b )
/*************************************************************/
{
    if( a.sect_id == 0 ) {
        a.sect_id = b.sect_id;
    } else if( b.sect_id == 0 ) {
        b.sect_id = a.sect_id;
    }
    if( a.sect_id != b.sect_id )
        return( DS_FAIL );
    if( a.mach.segment != b.mach.segment )
        return( DS_FAIL );
    return( DS_OK );
}



void DIPCLIENTRY( AddrSection )( address * addr )
/***********************************************/
{
    MapAddressIntoSection( addr );
}



void DIPCLIENTRY( Status )( dip_status ds )
/*****************************************/
{
    DIPStatus = ds;
}

dig_arch DIPCLIENTRY( CurrArch )( void )
/**************************************/
{
    return( CurrSIOData->config.arch );
}



/*
 * Profiler dip interface routines
 * ===============================
 */



void WPDipInit( void )
/********************/
{
    char        *dip_name;
    unsigned    dip_count;
    dip_status  ds;

    ds = DIPInit();
    if( ds != DS_OK ) {
        fatal( LIT( Dip_Init_Failed ), ds & ~DS_ERR );
    }
    dip_count = 0;
    if( WProfDips == NULL ) {
        dip_name = DIPDefaults;
    } else {
        dip_name = WProfDips;
    }
    for( ; *dip_name != NULLCHAR; dip_name += strlen( dip_name ) + 1 ) {
        if( loadDIP( dip_name, true, true ) ) {
            dip_count++;
        }
    }
    if( dip_count == 0 ) {
        DIPFini();
        fatal( LIT( Dip_Find_Failed  ));
    }
}



process_info *WPDipProc( void )
/*****************************/
{
    return( DIPCreateProcess() );
}



void WPDipDestroyProc( process_info *dip_proc )
/*********************************************/
{
    if( dip_proc != NULL ) {
        DIPDestroyProcess( dip_proc );
    }
}



void WPDipSetProc( process_info *dip_proc )
/*****************************************/
{
    DIPSetProcess( dip_proc );
}



mod_handle WPDipLoadInfo( FILE *fp, const char *f_name, void *image,
                   unsigned image_size, dip_priority start, dip_priority end )
/****************************************************************************/
{
    dip_priority    priority;
    mod_handle      dip_module;

    dip_module = NO_MOD;
    for( priority = start - 1; (priority = DIPPriority( priority )) != 0; ) {
        if( priority > end )
            break;
        DIPStatus = DS_OK;
        dip_module = DIPLoadInfo( fp, image_size, priority );
        if( dip_module != NO_MOD ) {
            *(void **)DIPImageExtra( dip_module ) = image;
            break;
        }
        if( DIPStatus & DS_ERR ) {
            ErrorMsg( LIT( Dip_Info_Failed ), f_name, errMsgText( DIPStatus ) );
        }
    }
    return( dip_module );
}



void WPDipFini( void )
/********************/
{
    DIPFini();
}



STATIC bool loadDIP( const char *dip, bool defaults, bool fail_big )
/******************************************************************/
{
    dip_status  ds;

    ds = DIPLoad( dip );
    if( ds != DS_OK ) {
        if( defaults && ( ds == (DS_ERR | DS_FOPEN_FAILED) ) ) {
            return( false );
        }
        if( fail_big ) {
            fatal( LIT( Dip_Load_Failed ), dip, errMsgText( ds ) );
        }
        ErrorMsg( LIT( Dip_Load_Failed ), dip, errMsgText( ds ) );
        return( false );
    }
    return( true );
}



STATIC char * errMsgText( dip_status ds )
/***************************************/
{
    static char * WPDIPText[] = {
        "Ok",                                   /* DS_OK */
        "Failed!",                              /* DS_FAIL */
        "Too many DIPs",                        /* DS_TOO_MANY_DIPS */
        "Invalid DIP version number",           /* DS_INVALID_DIP_VERSION */
        "Out of memory",                        /* DS_NO_MEM */
        "Not debugging anything",               /* DS_NO_PROCESS */
        "Too many images",                      /* DS_TOO_MANY_IMAGES */
        "Too many pointer types",               /* DS_TOO_MANY_POINTERS */
        "Incorrect type for operation",         /* DS_IMPROPER_TYPE */
        "File open failed",                     /* DS_FOPEN_FAILED */
        "File read failed",                     /* DS_FREAD_FAILED */
        "File write failed",                    /* DS_FWRITE_FAILED */
        "File seek failed",                     /* DS_FSEEK_FAILED */
        "Invalid DIP file",                     /* DS_INVALID_DIP */
        "Incorrect parameter",                  /* DS_BAD_PARM */
        "Wrapped",                              /* DS_WRAPPED */
        "Invalid debugging information",        /* DS_INFO_INVALID */
        "Incorrect debugging information version", /* DS_INFO_BAD_VERSION */
        "No parameter",                         /* DS_NO_PARM */
        "Incorrect location",                   /* DS_BAD_LOCATION */
        "No context for location expression",   /* DS_NO_CONTEXT */
        "Do not have correct register value",   /* DS_CONTEXT_ITEM_INVALID */
        "Can not read memory location",         /* DS_NO_READ_MEM */
        "Can not write memory location",        /* DS_NO_WRITE_MEM */
        "Invalid operator token"                /* DS_INVALID_OPERATOR */
    };
    /*myassert( DS_LAST == 25 );*/
    ds &= ~DS_ERR;
    if( ds > DS_INVALID_OPERATOR ) {
        ds = DS_FAIL;
    }
    return( WPDIPText[ds] );
}
