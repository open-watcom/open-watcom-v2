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


#include "os.h"
#include "intrptr.h"

/*
  The timer interrupt (0x08) is special in that the getvect and setvect
  DOS calls will put an intermediate interrupt function ahead of your
  interrupt function and thus you will always get the same cs:ip value.
  **********************************************************************
  We want to get the cs:ip value that the program was executing at
  so we must bend the rules a bit and modify the interrupt vector table
  directly using INT_LOCATE.
  This treatement is not necessary for int 21h and 28h but is used instead
  of getvec() and setvec() following some law of programming inertia.
  **********************************************************************
*/
    /*  FAR_PTR pointer stored at n-th interrupt vector location */

#define INT_LOCATE( nr )    ( *((void interrupt (* FAR_PTR *)())  \
                               ((unsigned long)(unsigned)nr*4)) )
#define MX_INTXX        8

void interrupt                  (*old_int03)();

/*
    located in INTRC.ASM
*/
extern void interrupt   int28_handler( union INTPACK r );
extern void interrupt   int21_handler( union INTPACK r );
extern void interrupt   int13_handler( union INTPACK r );
extern void interrupt   int03_handler( union INTPACK r );

extern void interrupt   (* FAR_PTR old_int13)();
extern void interrupt   (* FAR_PTR old_int21)();
extern void interrupt   (* FAR_PTR old_int28)();

extern void interrupt   intx0_handler( union INTPACK r );
extern void interrupt   intx1_handler( union INTPACK r );
extern void interrupt   intx2_handler( union INTPACK r );
extern void interrupt   intx3_handler( union INTPACK r );
extern void interrupt   intx4_handler( union INTPACK r );
extern void interrupt   intx5_handler( union INTPACK r );
extern void interrupt   intx6_handler( union INTPACK r );
extern void interrupt   intx7_handler( union INTPACK r );

extern void interrupt   (* FAR_PTR old_intx0)();
extern void interrupt   (* FAR_PTR old_intx1)();
extern void interrupt   (* FAR_PTR old_intx2)();
extern void interrupt   (* FAR_PTR old_intx3)();
extern void interrupt   (* FAR_PTR old_intx4)();
extern void interrupt   (* FAR_PTR old_intx5)();
extern void interrupt   (* FAR_PTR old_intx6)();
extern void interrupt   (* FAR_PTR old_intx7)();

static void interrupt   (* intxx_handlers[MX_INTXX])() =
{ intx0_handler, intx1_handler, intx2_handler, intx3_handler,
  intx4_handler, intx5_handler, intx6_handler, intx7_handler };
static void interrupt   (* FAR_PTR * old_intxx_handlers[MX_INTXX])() =
{ &old_intx0, &old_intx1, &old_intx2, &old_intx3,
  &old_intx4, &old_intx5, &old_intx6, &old_intx7 };

static unsigned char intr_list[MX_INTXX];
static unsigned next_intr;

intrptr HookTimer( intrptr new_int08 )
{
    intrptr     old_int08;

    _disable();  /* ...disable hardware interrupts: a protective measure */
    old_int08 = INT_LOCATE( 0x08 ); /* i.e. timer interrupt 08h location */
    INT_LOCATE( 0x08 ) = new_int08;
    _enable();
    return( old_int08 );
}

/*
    Will replace addresses of usual INT 21H and 28H interrupts with our custom
    tailored ones (defined in INTERC.ASM module).
*/

void InstallDOSIntercepts( void )
{
    int i;

    for( i = 0; i < next_intr; ++i ) {
        _disable();
        *old_intxx_handlers[i] = INT_LOCATE( intr_list[i] );
        INT_LOCATE( intr_list[i] ) = intxx_handlers[i];
        _enable();
    }
    _disable();

    old_int28 = INT_LOCATE( 0x28 );
    INT_LOCATE( 0x28 ) = &int28_handler;

    old_int21 = INT_LOCATE( 0x21 );
    INT_LOCATE( 0x21 ) = &int21_handler;

    old_int13 = INT_LOCATE( 0x13 );
    INT_LOCATE( 0x13 ) = &int13_handler;

    old_int03 = INT_LOCATE( 0x03 );
    INT_LOCATE( 0x03 ) = &int03_handler;

    _enable();
}


void RemoveDOSIntercepts( void )    /* will undo the above */
{
    int i;

    for( i = 0; i < next_intr; ++i ) {
        _disable();
        INT_LOCATE( intr_list[i] ) = *old_intxx_handlers[i];
        _enable();
    }
    _disable();
    INT_LOCATE( 0x21 ) = old_int21;
    INT_LOCATE( 0x28 ) = old_int28;
    INT_LOCATE( 0x13 ) = old_int13;
    INT_LOCATE( 0x03 ) = old_int03;
    _enable();
}

int AddInterrupt( unsigned num )
{
    if( next_intr >= MX_INTXX ) {
        return( -1 );
    }
    intr_list[next_intr++] = num;
    return( 0 );
}
