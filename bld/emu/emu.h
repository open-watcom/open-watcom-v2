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


typedef struct ld {
        unsigned short x[4];
        unsigned short exponent;
} LD;
#pragma aux     __FLDA "*";
#pragma aux     __FLDS "*";
#pragma aux     __FLDC "*";
#pragma aux     __FLDM "*";
#pragma aux     __FLDD "*";
#pragma aux     __FLDN "*";
#pragma aux     __EmuFDLD "*" parm caller [ax bx cx dx] [si];
#pragma aux     __EmuLDFD "*";
#pragma aux     __I4LD "*";
#pragma aux     __U4LD "*";
#pragma aux     __LDI4 "*" parm caller [bx] value [dx ax];
#pragma aux     __LDU4 "*" parm caller [bx] value [dx ax];
#pragma aux     __EmuLDFS "*";
#pragma aux     __EmuFSLD "*";
#pragma aux     __sqrt "*";
#pragma aux     __log  "*";
#pragma aux     __sin  "*";
#pragma aux     __cos  "*";
#pragma aux     __tan  "*";
#pragma aux     __atan "*";
#pragma aux     __f2xm1 "*";
#pragma aux     __fyl2x "*";
#pragma aux     __fprem "*";
#pragma aux     __EvalPoly "*";
#pragma aux     __OddPoly  "*";
#pragma aux     __frndint "*" modify [bx cx];

void    __FLDA( LD *, LD *, LD * );     // add
void    __FLDS( LD *, LD *, LD * );     // subtract
void    __FLDM( LD *, LD *, LD * );     // multiply
void    __FLDD( LD *, LD *, LD * );     // divide
int     __FLDC( LD *, LD * );           // compare
void    __FLDN( LD * );                 // negate
void    __EmuFDLD( double, LD * );      // convert double to long double
double  __EmuLDFD( LD * );              // convert long double to double
void    __I4LD( long, LD * );           // convert long to long double
void    __U4LD( long, LD * );           // convert unsigned long to long double
long    __LDI4( LD * );                 // convert long double to long
unsigned long __LDU4( LD * );           // convert long double to unsigned long
float   __EmuLDFS( LD * );              // convert long double to float
void    __EmuFSLD( float, LD * );       // convert float to long double
int     __fprem( LD *, LD * );          // fprem
int     __fyl2x( LD *, LD * );          // fyl2x
void    __sqrt( LD * );                 // fsqrt
void    __log( LD * );                  // log
void    __sin( LD * );                  // sin
void    __cos( LD * );                  // cos
void    __tan( LD * );                  // tan
void    __atan( LD * );                 // atan
void    __f2xm1( LD * );                // 2**x -1
void    __frndint( LD *, int );         // frndint
