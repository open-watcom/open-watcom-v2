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
#include <dos.h>
#include <float.h>
#include <string.h>

void techoutput( char *format, ... );
extern int LineCount;

short Control = (                               \
               ( IC_AFFINE | RC_NEAR | PC_53 )  \
               | ( MCW_EM & ~                   \
                   ( EM_INVALID                 \
                   | EM_DENORMAL                \
                   | EM_ZERODIVIDE              \
                   | EM_OVERFLOW                \
                   | EM_UNDERFLOW               \
                   | EM_PRECISION )             \
               ) );

volatile short Sample_On = 0;
volatile unsigned short NDP_Status;

volatile struct NDP_Environment {
    unsigned short control_word;
    unsigned short status_word;
    unsigned short tag_word;
    unsigned short instruction_ptr;
    unsigned short opcode;
    unsigned short operand_ptr;
    unsigned short operand_ptr_2;
} NDP_Env;

extern void _TEST_NDP( void );
#pragma aux _TEST_NDP = \
        "FINIT",                        /* initialize ND processor */ \
        "FLDCW   word ptr Control",     /* initialize control */ \
        "FENI",                         /* enable interrupts */ \
        "FLD1",                         /* load 1.0 */ \
        "FLDZ",                         /* load 0.0 */ \
        "FDIVP   ST(1),ST",             /* divide 1.0 by 0.0 */ \
        "FNOP"                          /* no-operation */

extern void _INIT_NDP( void );
#pragma aux _INIT_NDP = "FNINIT"        /* reinitialize NDP */;

extern  void    __fstenv( void );
#pragma aux     __fstenv = "FSTENV NDP_Env";

#define NUM_INTS 0x80

volatile short int_tick[NUM_INTS] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

#if defined(__386__)
#define FAR
#else
#define FAR __far
#endif

void (__interrupt FAR *prev_int[NUM_INTS])();

#define intx( N )                                   \
    void __interrupt FAR int_rtn_##N()              \
      {                                             \
        if( Sample_On ) ++int_tick[N];              \
        if( prev_int[N] != NULL )                   \
            _chain_intr( prev_int[N] );             \
      }

#define intx2( N )                                  \
    void __interrupt FAR int_rtn_##N()              \
      {                                             \
        if( Sample_On ) ++int_tick[N];              \
        __fstenv();                                 \
        if( prev_int[N] != NULL )                   \
            _chain_intr( prev_int[N] );             \
      }

intx( 0x00 )
intx( 0x01 )
intx2( 0x02 )
intx( 0x03 )
intx( 0x04 )
intx( 0x05 )
intx( 0x06 )
intx( 0x07 )
intx( 0x08 )
intx( 0x09 )
intx( 0x0A )
intx( 0x0B )
intx( 0x0C )
intx( 0x0D )
intx( 0x0E )
intx( 0x0F )
intx( 0x10 )
intx( 0x11 )
intx( 0x12 )
intx( 0x13 )
intx( 0x14 )
intx( 0x15 )
intx( 0x16 )
intx( 0x17 )
intx( 0x18 )
intx( 0x19 )
intx( 0x1A )
intx( 0x1B )
intx( 0x1C )
intx( 0x1D )
intx( 0x1E )
intx( 0x1F )
intx( 0x20 )
intx( 0x21 )
intx( 0x22 )
intx( 0x23 )
intx( 0x24 )
intx( 0x25 )
intx( 0x26 )
intx( 0x27 )
intx( 0x28 )
intx( 0x29 )
intx( 0x2A )
intx( 0x2B )
intx( 0x2C )
intx( 0x2D )
intx( 0x2E )
intx( 0x2F )
intx( 0x30 )
intx( 0x31 )
intx( 0x32 )
intx( 0x33 )
intx( 0x34 )
intx( 0x35 )
intx( 0x36 )
intx( 0x37 )
intx( 0x38 )
intx( 0x39 )
intx( 0x3A )
intx( 0x3B )
intx( 0x3C )
intx( 0x3D )
intx( 0x3E )
intx( 0x3F )
intx( 0x40 )
intx( 0x41 )
intx( 0x42 )
intx( 0x43 )
intx( 0x44 )
intx( 0x45 )
intx( 0x46 )
intx( 0x47 )
intx( 0x48 )
intx( 0x49 )
intx( 0x4A )
intx( 0x4B )
intx( 0x4C )
intx( 0x4D )
intx( 0x4E )
intx( 0x4F )
intx( 0x50 )
intx( 0x51 )
intx( 0x52 )
intx( 0x53 )
intx( 0x54 )
intx( 0x55 )
intx( 0x56 )
intx( 0x57 )
intx( 0x58 )
intx( 0x59 )
intx( 0x5A )
intx( 0x5B )
intx( 0x5C )
intx( 0x5D )
intx( 0x5E )
intx( 0x5F )
intx( 0x60 )
intx( 0x61 )
intx( 0x62 )
intx( 0x63 )
intx( 0x64 )
intx( 0x65 )
intx( 0x66 )
intx( 0x67 )
intx( 0x68 )
intx( 0x69 )
intx( 0x6A )
intx( 0x6B )
intx( 0x6C )
intx( 0x6D )
intx( 0x6E )
intx( 0x6F )
intx( 0x70 )
intx( 0x71 )
intx( 0x72 )
intx( 0x73 )
intx( 0x74 )
intx( 0x75 )
intx( 0x76 )
intx( 0x77 )
intx( 0x78 )
intx( 0x79 )
intx( 0x7A )
intx( 0x7B )
intx( 0x7C )
intx( 0x7D )
intx( 0x7E )
intx( 0x7F )

void (__interrupt FAR *new_int[NUM_INTS])() = {
    int_rtn_0x00, int_rtn_0x01, int_rtn_0x02, int_rtn_0x03,
    int_rtn_0x04, int_rtn_0x05, int_rtn_0x06, int_rtn_0x07,
    int_rtn_0x08, int_rtn_0x09, int_rtn_0x0A, int_rtn_0x0B,
    int_rtn_0x0C, int_rtn_0x0D, int_rtn_0x0E, int_rtn_0x0F,
    int_rtn_0x10, int_rtn_0x11, int_rtn_0x12, int_rtn_0x13,
    int_rtn_0x14, int_rtn_0x15, int_rtn_0x16, int_rtn_0x17,
    int_rtn_0x18, int_rtn_0x19, int_rtn_0x1A, int_rtn_0x1B,
    int_rtn_0x1C, int_rtn_0x1D, int_rtn_0x1E, int_rtn_0x1F,
    int_rtn_0x20, int_rtn_0x21, int_rtn_0x22, int_rtn_0x23,
    int_rtn_0x24, int_rtn_0x25, int_rtn_0x26, int_rtn_0x27,
    int_rtn_0x28, int_rtn_0x29, int_rtn_0x2A, int_rtn_0x2B,
    int_rtn_0x2C, int_rtn_0x2D, int_rtn_0x2E, int_rtn_0x2F,
    int_rtn_0x30, int_rtn_0x31, int_rtn_0x32, int_rtn_0x33,
    int_rtn_0x34, int_rtn_0x35, int_rtn_0x36, int_rtn_0x37,
    int_rtn_0x38, int_rtn_0x39, int_rtn_0x3A, int_rtn_0x3B,
    int_rtn_0x3C, int_rtn_0x3D, int_rtn_0x3E, int_rtn_0x3F,
    int_rtn_0x40, int_rtn_0x41, int_rtn_0x42, int_rtn_0x43,
    int_rtn_0x44, int_rtn_0x45, int_rtn_0x46, int_rtn_0x47,
    int_rtn_0x48, int_rtn_0x49, int_rtn_0x4A, int_rtn_0x4B,
    int_rtn_0x4C, int_rtn_0x4D, int_rtn_0x4E, int_rtn_0x4F,
    int_rtn_0x50, int_rtn_0x51, int_rtn_0x52, int_rtn_0x53,
    int_rtn_0x54, int_rtn_0x55, int_rtn_0x56, int_rtn_0x57,
    int_rtn_0x58, int_rtn_0x59, int_rtn_0x5A, int_rtn_0x5B,
    int_rtn_0x5C, int_rtn_0x5D, int_rtn_0x5E, int_rtn_0x5F,
    int_rtn_0x60, int_rtn_0x61, int_rtn_0x62, int_rtn_0x63,
    int_rtn_0x64, int_rtn_0x65, int_rtn_0x66, int_rtn_0x67,
    int_rtn_0x68, int_rtn_0x69, int_rtn_0x6A, int_rtn_0x6B,
    int_rtn_0x6C, int_rtn_0x6D, int_rtn_0x6E, int_rtn_0x6F,
    int_rtn_0x70, int_rtn_0x71, int_rtn_0x72, int_rtn_0x73,
    int_rtn_0x74, int_rtn_0x75, int_rtn_0x76, int_rtn_0x77,
    int_rtn_0x78, int_rtn_0x79, int_rtn_0x7A, int_rtn_0x7B,
    int_rtn_0x7C, int_rtn_0x7D, int_rtn_0x7E, int_rtn_0x7F
};

int monint( int print )
  {
    int i;

    for( i = 0; i < NUM_INTS; i++ ) {
        prev_int[i] = _dos_getvect( i );
    }

    if( print ) techoutput( "\nInstalling interrupt handlers\n" );
    for( i = NUM_INTS - 1 ; i >= 0; i-- ) {
        _dos_setvect( i, new_int[i] );
    }

    if( print ) {
        Sample_On = 1;
        while( int_tick[ 8 ] < 24 );
        Sample_On = 0;

        techoutput( "Normal system operation:\n" );
        int_summary();
        memset( int_tick, 0, sizeof( int_tick ) );

        techoutput( "Starting exception test...\n" );
    }
    Sample_On = 1;
    _TEST_NDP();
    while( int_tick[ 8 ] < 24 );
    Sample_On = 0;

    _INIT_NDP();

    NDP_Status = NDP_Env.status_word;

    if( print ) {
        techoutput( "System operation with NDP divide by zero exception:\n" );
        int_summary();
        techoutput( "Restoring interrupt handlers\n" );
    }
    for( i = NUM_INTS - 1 ; i >= 0; i-- ) {
        _dos_setvect( i, prev_int[i] );
    }

#define TEST_FPU(x,y) techoutput( "\t%s " y "\n", \
                ((NDP_Status & x) ? "  " : "No") )

    if( print ) {
        techoutput( "80x87 status:\n" );
        TEST_FPU( SW_INVALID, "invalid operation" );
        TEST_FPU( SW_DENORMAL, "denormalized operand" );
        TEST_FPU( SW_ZERODIVIDE, "divide by zero" );
        TEST_FPU( SW_OVERFLOW, "overflow" );
        TEST_FPU( SW_UNDERFLOW, "underflow" );
        TEST_FPU( SW_INEXACT, "inexact result" );
    }

    return( (int_tick[ 2 ] != 0) && ((NDP_Status & SW_ZERODIVIDE) != 0) );
  }

static void int_summary()
{
    int i;

    for( i = 0; i < NUM_INTS; i++ ) {
        if( int_tick[ i ] != 0 ) {
            LineCount--;
            techoutput( "\tint %02.2x ticks = %8d", i, int_tick[ i ] );
            if( i == 0x00 ) techoutput( " divide error interrupt" );
            if( i == 0x01 ) techoutput( " single-step interrupt" );
            if( i == 0x02 ) techoutput( " hardware NMI interrupt" );
            if( i == 0x03 ) techoutput( " break-point interrupt" );
            if( i == 0x04 ) techoutput( " overflow interrupt" );
            if( i == 0x05 ) techoutput( " print-screen interrupt" );
            if( i == 0x06 ) techoutput( " undefined opcode interrupt" );
            if( i == 0x07 ) techoutput( " no math unit interrupt" );
            if( i == 0x08 ) techoutput( " IRQ 0 timer interrupt" );
            if( i == 0x09 ) techoutput( " IRQ 1 keyboard interrupt" );
            if( i == 0x0a ) techoutput( " IRQ 2 EGA vert retrace interrupt" );
            if( i == 0x0b ) techoutput( " IRQ 3 COM2 interrupt" );
            if( i == 0x0c ) techoutput( " IRQ 4 COM1 interrupt" );
            if( i == 0x0d ) techoutput( " IRQ 5 fixed disk interrupt" );
            if( i == 0x0e ) techoutput( " IRQ 6 diskette interrupt" );
            if( i == 0x0f ) techoutput( " IRQ 7 printer interrupt" );
            if( i == 0x10 ) techoutput( " video interrupt" );
            if( i == 0x11 ) techoutput( " equip. determination" );
            if( i == 0x12 ) techoutput( " memory size" );
            if( i == 0x13 ) techoutput( " disk interrupt" );
            if( i == 0x14 ) techoutput( " serial i/o s/w interrupt" );
            if( i == 0x15 ) techoutput( " misc s/w interrupt" );
            if( i == 0x16 ) techoutput( " keyboard s/w interrupt" );
            if( i == 0x17 ) techoutput( " printer interrupt" );
            if( i == 0x18 ) techoutput( " xfer to ROM BASIC" );
            if( i == 0x19 ) techoutput( " disk boot" );
            if( i == 0x1a ) techoutput( " clock s/w interrupt" );
            if( i == 0x1b ) techoutput( " ctrl/break interrupt" );
            if( i == 0x1c ) techoutput( " clock tick interrupt" );
            if( i == 0x20 ) techoutput( " prog. termination" );
            if( i == 0x21 ) techoutput( " DOS s/w interrupt" );
            if( i == 0x29 ) techoutput( " fast putchar interrupt" );
            if( i == 0x2a ) techoutput( " network interrupt" );
            if( i == 0x33 ) techoutput( " mouse interrupt" );
            if( i == 0x70 ) techoutput( " IRQ 8 real time clock interrupt" );
            if( i == 0x71 ) techoutput( " IRQ 9 redirect cascade interrupt" );
            if( i == 0x72 ) techoutput( " IRQ10 reserved interrupt" );
            if( i == 0x73 ) techoutput( " IRQ11 reserved interrupt" );
            if( i == 0x74 ) techoutput( " IRQ12 mouse interrupt" );
            if( i == 0x75 ) techoutput( " IRQ13 math coprocessor exception interrupt" );
            if( i == 0x76 ) techoutput( " IRQ14 fixed disk interrupt" );
            if( i == 0x77 ) techoutput( " IRQ15 reserved interrupt" );
            LineCount--;
            techoutput( "\n" );
        }
    }
}
