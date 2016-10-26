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


#include <stdio.h>
#include "wresall.h"
#include "dmpdiag.h"
#include "wresdefn.h"
#include "rcrtns.h"

static void PrintDialogBoxHeader( DialogBoxHeader * head )
/********************************************************/
{
    puts( "Dialog Header:" );
    printf("\tStyle: 0x%08lx  No. of items: %d\n",
                head->Style, head->NumOfItems );
    printf( "\tx: %d   y: %d   w: %d   h: %d\n", head->Size.x, head->Size.y,
                head->Size.width, head->Size.height );
    printf( "\tMenu Name: %-10.10s    ClassName: %-10.10s\n",
                head->MenuName, head->ClassName );
    printf( "\tCaption: %-.40\n", head->Caption );
    if (head->Style & DS_SETFONT) {
        printf( "\tPoint Size: %d   FontName: %-.20s\n", head->PointSize,
                head->FontName );
    }
}

static void PrintControlClass( ControlClass * class )
/***************************************************/
{
    if (class->Class & 0x80) {
        switch (class->Class) {
        case CLASS_BUTTON:
            fputs( "\tClass: 0x80 button        ", stdout);
            break;
        case CLASS_EDIT:
            fputs( "\tClass: 0x81 edit           ", stdout);
            break;
        case CLASS_STATIC:
            fputs( "\tClass: 0x82 static         ", stdout);
            break;
        case CLASS_LISTBOX:
            fputs( "\tClass: 0x83 listbox        ", stdout);
            break;
        case CLASS_SCROLLBAR:
            fputs( "\tClass: 0x84 scrollbar      ", stdout);
            break;
        case CLASS_COMBOBOX:
            fputs( "\tClass: 0x85 combobox       ", stdout);
            break;
        default:
            printf( "\tClass: 0x%02x                ", class->Class );
        }
    } else {
        printf( "\tClass: %-15.15s     ", class->ClassName );
    }
}

static void PrintResNameOrOrd( ResNameOrOrdinal * name )
/******************************************************/
{
    if (name->ord.fFlag == 0xff) {
        printf( "%5d\n", name->ord.wOrdinalID );
    } else {
        printf( "%-40.40s\n", name->name );
    }
}

static void PrintDialogBoxControl( DialogBoxControl * control )
/*************************************************************/
{
    printf( "x: %d   y: %d   w: %d   h: %d\n",
            control->Size.x, control->Size.y, control->Size.width,
            control->Size.height );
    printf( "\tId: %d   Style: 0x%08lx\n", control->ID, control->Style );
    PrintControlClass( control->ClassID );
    printf( "Extra bytes: %d\n", control->ExtraBytes );
    fputs( "\tText: ", stdout );
    PrintResNameOrOrd( control->Text );
}

bool DumpDialog( uint_32 offset, uint_32 length, WResFileID handle )
/******************************************************************/
{
    bool                error;
    int                 prevpos;
    int                 itemnum;
    DialogBoxHeader     head;
    DialogBoxControl    control;

    length = length;
    head.NumOfItems = 0;
    prevpos = RCSEEK( handle, offset, SEEK_SET );
    error = ( prevpos == -1 );

    if( !error ) {
        error = ResReadDialogBoxHeader( &head, handle );
    }
    if( !error ) {
        PrintDialogBoxHeader( &head );
    }

    puts( "Controls:" );

    for( itemnum = 0; itemnum < head.NumOfItems && !error; itemnum++ ) {
        error = ResReadDialogBoxControl( &control, handle );
        if( !error ) {
            printf( "   %3d. ", itemnum + 1 );
            PrintDialogBoxControl( &control );
            if( control.ClassID != NULL ) {
                RCFREE( control.ClassID );
            }
            if( control.Text != NULL ) {
                RCFREE( control.Text );
            }
        }
    }

    ResFreeDialogBoxHeaderPtrs( &head );

    RCSEEK( handle, prevpos, SEEK_SET );

    return( error );
}
