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


#include <string.h>
#include <stdio.h>

#include "common.h"
#include "aui.h"
#include "wpaui.h"
#include "dip.h"
#include "sampinfo.h"

//#include "wpcnvt.def"
//#include "dlgcnvt.def"
extern void DlgGetConvert(a_window *wnd);

extern sio_data *       SIOData;
extern FILE *           ConvertFile;
extern bint             OptDIFFormat;
//extern bint             OptCommaFormat;

typedef void (DUMPRTNS)( char *, char *, char *, char *, int );

STATIC int  convertEntryCount;
STATIC int  convertEntrySize[4];

STATIC void countDIFData( char *, char *, char *, char *, int );
STATIC void initDIFData();
STATIC void finiDIFData();
STATIC void dumpDIFData( char *, char *, char *, char *, int );
STATIC void dumpCommaData( char *, char *, char *, char *, int );
STATIC void doConvert( a_window *, pointer, int );
STATIC void dumpSampleImages( sio_data *, pointer );
STATIC void dumpImage( image_info *, DUMPRTNS * );
STATIC void dumpModule( image_info *, mod_info *, DUMPRTNS * );



extern void WPConvert( a_window * wnd, int convert_select )
/*********************************************************/
{
    DlgGetConvert( wnd );
    if( ConvertFile == NULL ) return;
    if( OptDIFFormat ) {
        convertEntryCount = 0;
        memset( convertEntrySize, 0, sizeof( convertEntrySize ) );
        doConvert( wnd, &countDIFData, convert_select );
        initDIFData();
        doConvert( wnd, &dumpDIFData, convert_select );
    } else {
        doConvert( wnd, &dumpCommaData, convert_select );
    }
    if( OptDIFFormat ) {
        finiDIFData();
    }
    fclose( ConvertFile );
}



STATIC void doConvert( a_window * wnd, DUMPRTNS * dump_rtn, int convert_select )
/******************************************************************************/
{
    sio_data *      curr_sio;

    curr_sio = WndExtra( wnd );
    if( curr_sio->curr_image == NULL ) {
        curr_sio->curr_image = curr_sio->images[0];
    }
    if( curr_sio->curr_mod == NULL ) {
        curr_sio->curr_mod = curr_sio->curr_image->module[0];
    }
    if( convert_select == MENU_CONVERT_ALL ) {
        dumpSampleImages( curr_sio, dump_rtn );
    } else if( convert_select == MENU_CONVERT_IMAGE ) {
        dumpImage( curr_sio->curr_image, dump_rtn );
    } else if( convert_select == MENU_CONVERT_MODULE ) {
        dumpModule( curr_sio->curr_image, curr_sio->curr_mod, dump_rtn );
    }
}



STATIC void dumpSampleImages( sio_data * curr_sio, DUMPRTNS * dump_rtn )
/**********************************************************************/
{
    image_info *    curr_image;
    int             image_index;

    image_index = 0;
    while( image_index < curr_sio->image_count ) {
        curr_image = curr_sio->images[image_index];
        dumpImage( curr_image, dump_rtn );
        image_index++;
    }
}



STATIC void dumpImage( image_info * curr_image, DUMPRTNS * dump_rtn )
/*******************************************************************/
{
    mod_info *      curr_mod;
    int             mod_count;

    mod_count = 0;
    while( mod_count < curr_image->mod_count ) {
        curr_mod = curr_image->module[mod_count];
        dumpModule( curr_image, curr_mod, dump_rtn );
        mod_count++;
    }
}



STATIC void dumpModule( image_info * curr_image, mod_info * curr_mod,
                                                  DUMPRTNS * dump_rtn )
/*********************************************************************/
{
    file_info *     curr_file;
    rtn_info *      curr_rtn;
    int             file_count;
    int             rtn_count;

    file_count = 0;
    while( file_count < curr_mod->file_count ) {
        curr_file = curr_mod->mod_file[file_count];
        rtn_count = 0;
        while( rtn_count < curr_file->rtn_count ) {
            curr_rtn = curr_file->routine[rtn_count];
            if( curr_rtn->tick_count != 0
             || (!curr_rtn->gather_routine && !curr_rtn->unknown_routine) ) {
                dump_rtn( curr_image->name, curr_mod->name, curr_file->name,
                          curr_rtn->name, curr_rtn->tick_count );
            }
            rtn_count++;
        }
        file_count++;
    }
}



STATIC void countDIFData( char * image, char * module, char * file,
                                          char * routine, int count )
/*******************************************************************/
{
    count=count;
    convertEntryCount++;
    convertEntrySize[0] = max( convertEntrySize[0], strlen( image ) );
    convertEntrySize[1] = max( convertEntrySize[1], strlen( module ) );
    convertEntrySize[2] = max( convertEntrySize[2], strlen( file ) );
    convertEntrySize[3] = max( convertEntrySize[3], strlen( routine ) );
}



STATIC void initDIFData()
/***********************/
{
    fprintf( ConvertFile, "TABLE\n0,1\n\"WProf\"\n" );
    fprintf( ConvertFile, "VECTORS\n0,5\n\"\"\n" );
    fprintf( ConvertFile, "TUPLES\n0,%d\n\"\"\n", convertEntryCount++ );
    fprintf( ConvertFile, "LABEL\n1,0\n\"Image\"\n" );
    fprintf( ConvertFile, "SIZE\n1,%d\n\"\"\n", convertEntrySize[0] );
    fprintf( ConvertFile, "LABEL\n2,0\n\"Module\"\n" );
    fprintf( ConvertFile, "SIZE\n2,%d\n\"\"\n", convertEntrySize[1] );
    fprintf( ConvertFile, "LABEL\n3,0\n\"File\"\n" );
    fprintf( ConvertFile, "SIZE\n3,%d\n\"\"\n", convertEntrySize[2] );
    fprintf( ConvertFile, "LABEL\n4,0\n\"Routine\"\n" );
    fprintf( ConvertFile, "SIZE\n4,%d\n\"\"\n", convertEntrySize[3] );
    fprintf( ConvertFile, "LABEL\n5,0\n\"Count\"\n" );
    fprintf( ConvertFile, "SIZE\n5,10\n\"\"\n" );
    fprintf( ConvertFile, "DATA\n0,0\n\"\"\n" );
}



STATIC void finiDIFData()
/***********************/
{
    fprintf( ConvertFile, "-1,0\nEOD\n" );
}



STATIC void dumpDIFData( char * image, char * module, char * file,
                                         char * routine, int count )
/******************************************************************/
{
    fprintf( ConvertFile, "-1,0\nBOT\n" );
    fprintf( ConvertFile, "1,0\n\"%s\"\n", image );
    fprintf( ConvertFile, "1,0\n\"%s\"\n", module );
    fprintf( ConvertFile, "1,0\n\"%s\"\n", file );
    fprintf( ConvertFile, "1,0\n\"%s\"\n", routine );
    fprintf( ConvertFile, "0,%d\nV\n", count );
}



STATIC void dumpCommaData( char * image, char * module, char * file,
                                           char * routine, int count )
/********************************************************************/
{
    fprintf( ConvertFile, "\"%s\",\"%s\",\"%s\",\"%s\",\"%d\"\n", image,
             module, file, routine, count );
}
