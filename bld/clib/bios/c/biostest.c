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


/*
 *  BIOSTEST.C
 *  Currently, this program only tests the _bios functions.
 *  They now decide at run-time whether or not they're on an IBM or a NEC
 *  machine; this program is to ensure they still work.
 *
 */

#include <bios.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define VERIFY( exp )   if( !(exp) ) {                                      \
                            printf( "%s: ***FAILURE*** at line %d of %s.\n",\
                                    ProgramName, __LINE__,                  \
                                    strlwr(__FILE__) );                     \
                            NumErrors++;                                    \
                        }

//#define HARDWARE_ATTACHED
#define COM_PORT                1
#define PRINTER_PORT            0


struct EquipBits {
    unsigned bootsFromDisk      : 1;
    unsigned hasCoPro           : 1;
    unsigned ramSize            : 2;
    unsigned initialVideoMode   : 2;
    unsigned numDisketteDrives  : 2;
    unsigned noDma              : 1;
    unsigned numSerialPorts     : 3;
    unsigned hasGamePort        : 1;
    unsigned hasSerialPrinter   : 1;
    unsigned numParPrinters     : 2;
};


extern int      __NonIBM;

char    ProgramName[128];                       /* executable filename */
int     NumErrors = 0;                          /* number of errors */

#if defined(__DOS__) || defined(__WINDOWS__)
    #define DO_TESTING
#endif


#ifdef DO_TESTING


void TestDisk( void )
/*******************/
{
    struct diskinfo_t   di;
    unsigned short      rc;

    di.drive = 3 | 0x80;
    di.head = di.track = di.sector = 0;
    di.nsectors = 1;
    di.buffer = NULL;
    if( !__NonIBM ) {
        rc = _bios_disk( _DISK_RESET, &di );
        VERIFY( rc == 0 );

        rc = _bios_disk( _DISK_STATUS, &di );
        VERIFY( rc == 0 );

        rc = _bios_disk( _DISK_VERIFY, &di );
        VERIFY( rc == 0 );
    } else {
        rc = _bios_disk( _DISK_VERIFY, &di );
        VERIFY( rc == 0x01 );
    }
}


void TestEquiplist( void )
/************************/
{
    unsigned short      rc;
    struct EquipBits    bits;

    rc = _bios_equiplist();
    memcpy( &bits, &rc, sizeof(rc) );
    printf( "\nInstalled equipment:\n"
            "\tBoots from disk:             %u\n"
            "\tHas math coprocessor:        %u\n"
            "\tMotherboard RAM size:        %u\n"
            "\tInitial video mode:          %u\n"
            "\tNumber of diskette drives:   %u\n"
            "\tHas no DMA:                  %u\n"
            "\tNumber of serial ports:      %u\n"
            "\tHas game port:               %u\n"
            "\tHas serial printer:          %u\n"
            "\tNumber of parallel printers: %u\n\n",
            bits.bootsFromDisk, bits.hasCoPro, bits.ramSize,
            bits.initialVideoMode, bits.numDisketteDrives, bits.noDma,
            bits.numSerialPorts, bits.hasGamePort, bits.hasSerialPrinter,
            bits.numParPrinters
          );
}


void TestKeybrd( void )
/*********************/
{
    unsigned            status;
    int                 alive = 1;
    char                str[80];
    char                ch;

    printf( "Press a key to continue..." );
    fflush( stdout );
    while( !_bios_keybrd( _KEYBRD_READY ) )  ;
    printf( " ('%c' pressed)\n", _bios_keybrd( _KEYBRD_READ ) );

    printf( "Press 'q' to stop status display.\n" );
    while( alive ) {
        status = _bios_keybrd( _KEYBRD_SHIFTSTATUS );
        strcpy( str, "Active: " );
        if( status & 0x01 )  strcat( str, "RightShift " );
        if( status & 0x02 )  strcat( str, "LeftShift " );
        if( status & 0x04 )  strcat( str, "Ctrl " );
        if( status & 0x08 )  strcat( str, "Alt " );
        if( status & 0x10 )  strcat( str, "ScrollLock " );
        if( status & 0x20 )  strcat( str, "NumLock " );
        if( status & 0x40 )  strcat( str, "CapsLock " );
        if( status & 0x80 )  strcat( str, "Insert " );
        memset( str+strlen(str), ' ', 79-strlen(str) );
        str[79] = '\0';
        printf( "%s\r", str );
        if( _bios_keybrd( _KEYBRD_READY ) ) {
            ch = _bios_keybrd( _KEYBRD_READ ) & 0x00FF;
            if( ch == 'q'  ||  ch == 'Q' )  alive = 0;
        }
    }
}


void TestMemsize( void )
/**********************/
{
    printf( "\n\nMemory installed: %d kb\n\n", _bios_memsize() );
}


void TestPrinter( void )
/**********************/
{
    unsigned short      rc;

    rc = _bios_printer( _PRINTER_INIT, PRINTER_PORT, 0 );
    printf( "INIT: %04X\n", rc );

    rc = _bios_printer( _PRINTER_STATUS, PRINTER_PORT, 0 );
    printf( "STATUS: %04X\n", rc );

    #ifdef HARDWARE_ATTACHED
        rc = _bios_printer( _PRINTER_WRITE, PRINTER_PORT, '!' );
        VERIFY( (rc&0x08) == 0 );
        rc = _bios_printer( _PRINTER_WRITE, PRINTER_PORT, '\r' );
        VERIFY( (rc&0x08) == 0 );
    #endif
}


void TestSerialcom( void )
/************************/
{
    #ifdef HARDWARE_ATTACHED
        char *          outStr = "foo";
        int             count;
        unsigned short  rc;

        rc = _bios_serialcom( _COM_INIT, COM_PORT,
                        _COM_2400 | _COM_NOPARITY | _COM_CHR8 | _COM_STOP1 );
        VERIFY( (rc&0x8E00) == 0 );
        rc = _bios_serialcom( _COM_STATUS, COM_PORT,
                        _COM_2400 | _COM_NOPARITY | _COM_CHR8 | _COM_STOP1 );
        VERIFY( (rc&0x8E00) == 0 );

        for( count=0; count<strlen(outStr); count++ ) {
            rc = _bios_serialcom( _COM_SEND, COM_PORT, outStr[count] );
            VERIFY( rc == outStr[count] );
        }
    #endif
}


void TestTimeofday( void )
/************************/
{
    long                ticks;
    unsigned            hours, minutes, seconds;

    VERIFY( _bios_timeofday( _TIME_GETCLOCK, &ticks )  ==  0 );
    ticks = (ticks * 10) / 182;
    hours = ticks / 3600;
    minutes = (ticks%3600) / 60;
    seconds = ticks % 60;
    printf( "Old time is %ld seconds past midnight (%02u:%02u:%02u)\n",
            ticks, hours, minutes, seconds );

    ticks = (ticks * 182) / 10;
    VERIFY( _bios_timeofday( _TIME_SETCLOCK, &ticks )  ==  0 );

    VERIFY( _bios_timeofday( _TIME_GETCLOCK, &ticks )  ==  0 );
    ticks = (ticks * 10) / 182;
    hours = ticks / 3600;
    minutes = (ticks%3600) / 60;
    seconds = ticks % 60;
    printf( "New time is %ld seconds past midnight (%02u:%02u:%02u)\n\n",
            ticks, hours, minutes, seconds );
}

#endif


int main( int argc, char *argv[] )
/********************************/
{
    /*** Initialize ***/
    strcpy( ProgramName, strlwr(argv[0]) );
    printf( "%s: Machine type is %s.\n", ProgramName, __NonIBM ? "NEC" : "IBM" );

    /*** Test stuff ***/
    #ifdef DO_TESTING
        TestDisk();
        TestEquiplist();
        TestKeybrd();
        TestMemsize();
        TestPrinter();
        TestSerialcom();
        TestTimeofday();
    #endif

    /*** Print a pass/fail message and quit ***/
    if( NumErrors==0 ) {
        printf( "%s: SUCCESS.\n", ProgramName );
        return( EXIT_SUCCESS );
    } else {
        printf( "%s: FAILURE (%d errors).\n", ProgramName, NumErrors );
        return( EXIT_FAILURE );
    }
}
