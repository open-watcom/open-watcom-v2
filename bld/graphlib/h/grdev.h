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


#pragma pack(push, 1);
typedef struct graphics_device {
    short           ( *init ) ();               // initialization
    void            ( *finish ) ();             // finish up device
    void            ( *set ) ();                // set device
    void            ( *reset ) ();              // reset device
    void            ( *setup ) ();              // setup routine
    void pascal     ( near *up ) ();            // move up routine (ASM)
    void pascal     ( near *left ) ();          // move left routine
    void pascal     ( near *down ) ();          // move down routine
    void pascal     ( near *right ) ();         // move right routine
    void pascal     ( near *plot[4] ) ();       // plot-replace
    short pascal    ( near *getdot ) ();        // get pixel color
    void pascal     ( near *zap ) ();           // zap routine
    void pascal     ( near *fill ) ();          // fill style routine
    void pascal     ( near *pixcopy ) ();       // copy pixels
    void pascal     ( near *readrow ) ();       // read a row of pixels
    short pascal    ( near *scanleft ) ();      // left scan in paint
    short pascal    ( near *scanright ) ();     // right scan in paint
} gr_device;
#pragma pack (pop);
