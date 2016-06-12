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
#include "sampinfo.h"
#include "msg.h"
#include "myassert.h"
#include "support.h"

//#include "dipinter.def"
//#include "support.def"
//#include "msg.def"


extern char *           WProfDips;
extern sio_data         *CurrSIOData;

STATIC char *       errMsgText( dip_status );
STATIC bool         loadDIP( char *, bool, bool );

STATIC dip_status   DIPStatus;



void DIGCLIENT DIPCliImageUnload( mod_handle mh )
/***********************************************/
{
    mh=mh;
}



void DIGCLIENT DIPCliMapAddr( addr_ptr * addr, void * d )
/*******************************************************/
{
    MapAddressToActual( (image_info *)d, addr );
}



imp_sym_handle * DIGCLIENT DIPCliSymCreate( imp_image_handle *ih, void *d )
/*************************************************************************/
{
    ih=ih;
    d=d;
    return( NULL );
}



dip_status DIGCLIENT DIPCliItemLocation( location_context * lc,
                            context_item ci, location_list * ll )
/***************************************************************/
{
    lc=lc;
    ci=ci;
    ll=ll;
    return( DS_ERR|DS_NO_CONTEXT );
}



dip_status DIGCLIENT DIPCliAssignLocation( location_list * dst,
                        location_list * src, unsigned long size )
/***************************************************************/
{
    dst=dst;
    src=src;
    size=size;
    return( DS_ERR );
}



dip_status DIGCLIENT DIPCliSameAddrSpace( address a, address b )
/**************************************************************/
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



void DIGCLIENT DIPCliAddrSection( address * addr )
/************************************************/
{
    MapAddressIntoSection( addr );
}



void DIGCLIENT DIPCliStatus( dip_status status )
/**********************************************/
{
    DIPStatus = status;
}

dig_mad DIGCLIENT DIPCliCurrMAD( void )
/*************************************/
{
    return( CurrSIOData->config.mad );
}



/*
 * Profiler dip interface routines
 * ===============================
 */



extern void WPDipInit( void )
/***************************/
{
    char        *dip_name;
    unsigned    dip_count;
    dip_status  dip_stat;

    dip_stat = DIPInit();
    if( dip_stat != DS_OK ) {
        fatal( LIT( Dip_Init_Failed ), dip_stat&~DS_ERR );
    }
    dip_count = 0;
    if( WProfDips == NULL ) {
        dip_name = DIPDefaults;
    } else {
        dip_name = WProfDips;
    }
    while( *dip_name != NULLCHAR ) {
        if( loadDIP( dip_name, true, true ) ) {
            dip_count++;
        }
        dip_name += strlen( dip_name ) + 1;
    }
    if( dip_count == 0 ) {
        DIPFini();
        fatal( LIT( Dip_Find_Failed  ));
    }
}



extern process_info *WPDipProc( void )
/************************************/
{
    return( DIPCreateProcess() );
}



extern void WPDipDestroyProc( process_info *dip_proc )
/****************************************************/
{
    if( dip_proc != NULL ) {
        DIPDestroyProcess( dip_proc );
    }
}



extern void WPDipSetProc( process_info *dip_proc )
/************************************************/
{
    DIPSetProcess( dip_proc );
}



extern mod_handle WPDipLoadInfo( int f_handle, char * f_name, void * image,
                       int image_size, unsigned dip_start, unsigned dip_end )
/***************************************************************************/
{
    unsigned    prio;
    mod_handle  dip_module;

    dip_module = NO_MOD;
    prio = dip_start;
    for( ;; ) {
        prio = DIPPriority( prio );
        if( prio == 0 || prio > dip_end ) break;
        DIPStatus = DS_OK;
        dip_module = DIPLoadInfo( f_handle, image_size, prio );
        if( dip_module != NO_MOD ) {
            *(void **)ImageExtra( dip_module ) = image;
            DIPMapInfo( dip_module, image );
            break;
        }
        if( DIPStatus & DS_ERR ) {
            ErrorMsg( LIT( Dip_Info_Failed ), f_name, errMsgText( DIPStatus ) );
        }
    }
    return( dip_module );
}



extern void WPDipFini( void )
/***************************/
{
    DIPFini();
}



STATIC bool loadDIP( char *dip, bool defaults, bool fail_big )
/************************************************************/
{
    dip_status  ret;

    ret = DIPLoad( dip );
    if( ret != DS_OK ) {
        if( defaults && (ret == (DS_ERR|DS_FOPEN_FAILED)) ) {
            return( false );
        }
        if( fail_big ) {
            fatal( LIT( Dip_Load_Failed ), dip, errMsgText( ret ) );
        }
        ErrorMsg( LIT( Dip_Load_Failed ), dip, errMsgText( ret ) );
        return( false );
    }
    return( true );
}



STATIC char * errMsgText( dip_status status )
/*******************************************/
{
    static char * WPDIPText[] = {
        "Ok",                           /* DS_OK */
        "Failed!",                      /* DS_FAIL */
        "Too many DIPs",                /* DS_TOO_MANY_DIPS */
        "Invalid DIP version number",   /* DS_INVALID_DIP_VERSION */
        "Out of memory",                /* DS_NO_MEM */
        "Not debugging anything",       /* DS_NO_PROCESS */
        "Too many images",              /* DS_TOO_MANY_IMAGES */
        "Too many pointer types",       /* DS_TOO_MANY_POINTERS */
        "Incorrect type for operation", /* DS_IMPROPER_TYPE */
        "File open failed",             /* DS_FOPEN_FAILED */
        "File read failed",             /* DS_FREAD_FAILED */
        "File write failed",            /* DS_FWRITE_FAILED */
        "File seek failed",             /* DS_FSEEK_FAILED */
        "Invalid DIP file",             /* DS_INVALID_DIP */
        "Incorrect parameter",          /* DS_BAD_PARM */
        "Wrapped",                      /* DS_WRAPPED */
        "Invalid debugging information",/* DS_INFO_INVALID */
        "Incorrect debugging information version", /* DS_INFO_BAD_VERSION */
        "No parameter",                 /* DS_NO_PARM */
        "Incorrect location",           /* DS_BAD_LOCATION */
        "No context for location expression", /* DS_NO_CONTEXT */
        "Do not have correct register value", /* DS_CONTEXT_ITEM_INVALID */
        "Can not read memory location", /* DS_NO_READ_MEM */
        "Can not write memory location",/* DS_NO_WRITE_MEM */
        "Invalid operator token"        /* DS_INVALID_OPERATOR */
    };
/*myassert( DS_LAST == 25 );*/
    status &= ~DS_ERR;
    if( status > DS_INVALID_OPERATOR ) {
        status = DS_FAIL;
    }
    return( WPDIPText[ status ] );
}
