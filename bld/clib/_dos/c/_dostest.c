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
* Description:  Non-exhaustive test of the C library DOS functions.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <fcntl.h>
#include <share.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#define MAX_OPEN                10
#ifdef __386__
    #ifdef __PHARLAP__
        #define NUM_PARA                63
        #define SIZE_PARA               4096
    #else
        #define NUM_PARA                2047
        #define SIZE_PARA               16
    #endif
#else
    #define NUM_PARA            2047
    #define SIZE_PARA           16
#endif
#define INTERRUPT_TIME          1       // seconds
#define TICK_COUNT              (INTERRUPT_TIME * 18)
#ifdef __SW_BW
    #include <wdefwin.h>
    #define PROG_ABORT( num )   { printf( "Line: %d\n"                      \
                                          "Abnormal termination.\n", num ); \
                                  exit( -1 ); }
#else
    #define PROG_ABORT( num )   { printf( "Line: %d\n", num ); exit(-1); }
#endif
#define YEAR(t)                 (((t & 0xFE00) >> 9) + 1980 )
#define MONTH(t)                ((t & 0x01E0) >> 5)
#define DAY(t)                  (t & 0x001F)
#define HOUR(t)                 ((t & 0xF800) >> 11)
#define MINUTE(t)               ((t & 0x07E0) >> 5)
#define SECOND(t)               ((t & 0x001F) << 1 )

#define LFN_FNAME               "Long File Name.with.long.extension"

#if defined(__OS2__) \
 || defined(__NT__) \
 || defined(__WINDOWS__) \
 || defined(__WINDOWS_386__) \
 || defined(__PHARLAP__)
#undef DO_INTERRUPT
#else
#define DO_INTERRUPT
#endif

#ifdef DO_INTERRUPT
    volatile int clock_ticks = 0;
    void (__interrupt __far *prev_int_1c)();
    volatile int delay_flag = 1;
#endif

struct dosdate_t        ddate;
struct dostime_t        dtime;
int verbose = 0;
char *runtime;

void NowTest( char *funcname )
{
    if( verbose ) {
        printf( "Testing %s...\n", funcname );
        fflush( stdout );
    }
}

void TestDateTimeOperations( void )
{
    struct dosdate_t    newdate;
    struct dostime_t    newtime;

    NowTest( "_dos_getdate(), and _dos_gettime()" );
    _dos_getdate( &ddate );
    _dos_gettime( &dtime );
    if( verbose ) {
        printf( "The date (MM-DD-YYYY) is: %.2d-%.2d-%.4d\n",
            ddate.month, ddate.day, ddate.year );
        printf( "The time (HH:MM:SS) is: %.2d:%.2d:%.2d\n",
            dtime.hour, dtime.minute, dtime.second );
    }
    // run286 doesn't support the DosSetDateTime call
    if( stricmp( runtime, "run286" ) != 0 ) {
        NowTest( "_dos_setdate(), and _dos_settime()" );
        newdate.year = 1999;
        newdate.month = 12;
        newdate.day = 31;
        newtime.hour = 19;
        newtime.minute = 19;
        newtime.second = 30;
        _dos_setdate( &newdate );
        _dos_settime( &newtime );
        newdate.year = 1980;
        newdate.month = 1;
        newdate.day = 1;
        newtime.hour = 0;
        newtime.minute = 0;
        newtime.second = 0;
        _dos_getdate( &newdate );
        _dos_gettime( &newtime );
        if( ( newdate.year != 1999 ) || ( newdate.month != 12 ) ||
            ( newdate.day != 31 ) || ( newtime.hour != 19 ) ||
            ( newtime.minute != 19 ) ) {
            printf( "FAIL: getting/setting date/time functions.\n" );
            printf( "Note: date/time info might have been corrupted.\n" );
            PROG_ABORT( __LINE__ );
        }
        _dos_setdate( &ddate );
        _dos_settime( &dtime );
    }
}

#if !( defined(__WINDOWS__) || defined(__WINDOWS_386__) )
void TestMemoryOperations( void )
{
    #if defined(__NT__) || (defined(__OS2__) && (defined(__386__)||defined(__PPC__)))
    void                *segment;
    char                *cptr;
    #else
    unsigned            segment;
    char __far          *cptr;
    #endif
    unsigned            ctr;

    NowTest( "_dos_allocmem(), and _dos_freemem()" );
    if( _dos_allocmem( NUM_PARA, &segment ) != 0 ) {
        printf( "_dos_allocmem() failed. " );
        printf( "Only %u paragraphs available.\n", segment );
        PROG_ABORT( __LINE__ );
    }
    // try to access it
    #if defined(__NT__) || (defined(__OS2__) && (defined(__386__)||defined(__PPC__)))
    cptr = (char *) segment;
    #else
    cptr = (char __far *) MK_FP(segment,0);
    #endif
    for( ctr = 0; ctr < NUM_PARA * SIZE_PARA; ctr += SIZE_PARA ) {
        cptr[ctr] = '#';
    }
    #if !( defined(__OS2__) || defined(__NT__) || defined(__DOS4G__) )
    NowTest( "_dos_setblock()" );
    if( _dos_setblock( NUM_PARA * 2, segment, &ctr ) != 0 ) {
        printf( "_dos_setblock() failed. " );
        printf( "Can only resize to %u paragraphs.\n", ctr );
        PROG_ABORT( __LINE__ );
    }
    // try to access it
    for( ctr = 0; ctr < NUM_PARA * SIZE_PARA * 2; ctr += SIZE_PARA ) {
        cptr[ctr] = '^';
    }
    #endif
    if( _dos_freemem( segment ) != 0 ) {
        printf( "_dos_freemem() failed.\n" );
        PROG_ABORT( __LINE__ );
    }
}
#endif

void TestFileOperations( void )
{
    int                 fh[10];
    int                 ctr;
    unsigned            rc;
    char                str[20];
    struct find_t       fileinfo;
    unsigned long       flag;
    unsigned            attribute;
    unsigned            date,time;
    char                buffer[] = "This is a test for _dos_write().";
    char                tmpbuff[80];
    unsigned            len;

    NowTest( "_dos_creatnew()" );
    if( _dos_creatnew( "tmpfile", _A_RDONLY, &fh[0] ) != 0 ) {
        printf( "Couldn't create the file \"tmpfile\"\n" );
        PROG_ABORT( __LINE__ );
    } else {
        if( verbose ) printf( "Created \"tmpfile\"\n" );
        if( _dos_creatnew( "tmpfile", _A_NORMAL, &fh[1] ) == 0 ) {
            printf( "Error: _dos_creatnew() succeeded in creating " );
            printf( "an existing file\n" );
        }
        _dos_close( fh[0] );
    }
    NowTest( "_dos_setfileattr()" ) ;
    if( _dos_getfileattr( "not_exst.fil", &attribute ) == 0 ) {
        printf( "_dos_getfileattr() failed.\n" );
        PROG_ABORT( __LINE__ );
    }
    if( _dos_getfileattr( "tmpfile", &attribute ) != 0 ) {
        printf( "_dos_getfileattr() failed.\n" );
        PROG_ABORT( __LINE__ );
    }
    if( _dos_setfileattr( "tmpfile", attribute & ~_A_ARCH ) != 0 ) {
        printf( "_dos_setfileattr() failed.\n" );
        PROG_ABORT( __LINE__ );
    }
    if( _dos_getfileattr( "tmpfile", &attribute ) != 0 ) {
        printf( "_dos_getfileattr() failed.\n" );
        PROG_ABORT( __LINE__ );
    }
    if( ( attribute & _A_ARCH ) == _A_ARCH ) {
        printf( "_dos_setfileattr() failed.\n" );
        PROG_ABORT( __LINE__ );
    }
    if( verbose ) printf( "Creating %u temporary files...", MAX_OPEN );
    strcpy( str, "_tmpfile." );
    for( ctr = 0; ctr < MAX_OPEN; ++ctr ) {
        itoa( 999 - ctr, &str[9], 10 );
        if( _dos_creat( str, _A_NORMAL, &fh[ctr] ) != 0 ) {
            printf( "failed when creating the file \"%s\"\n", str );
            while( --ctr >= 0 ) _dos_close( fh[ctr] );
            PROG_ABORT( __LINE__ );
        }
    }
    if( verbose ) printf( "done.\n" );
    for( ctr = 0; ctr < MAX_OPEN; ++ctr ) {
        if( _dos_close( fh[ctr] ) != 0 ) {
            printf( "Failed to close all the opened files\n" );
            PROG_ABORT( __LINE__ );
        }
    }
    // Now we have 20 tmpfiles. Use them to test _dos_find...()
    NowTest( "_dos_findfirst(), _dos_findnext, and _dos_findclose()" );
    rc = _dos_findfirst( "_tmpfile.*", _A_NORMAL, &fileinfo );
    flag = 0;
    while( rc == 0 ) {
        strcpy( str, "_tmpfile." );
        for( ctr = 0; ctr < 9; ++ctr ) {
            if( str[ctr] != tolower(fileinfo.name[ctr]) ) {
                printf( "Incorrect filename retrieved. " );
                printf( "Filename '%s'\n", fileinfo.name );
                PROG_ABORT( __LINE__ );
            }
        }
        ctr = 999 - atoi( &fileinfo.name[9] );
        if( ctr >= MAX_OPEN || ctr < 0 ) {
            printf( "Incorrect filename retrieved\n" );
            PROG_ABORT( __LINE__ );
        }
        flag |= ( 1 << ctr );
        rc = _dos_findnext( &fileinfo );
    }
    _dos_findclose( &fileinfo );
    for( ctr = 0; ctr < MAX_OPEN; ++ctr ) {
        if( ( flag >> ctr ) & 1 != 1 ) {
            printf( "Couldn't find all the files created\n" );
            PROG_ABORT( __LINE__ );
        }
        flag &= ~(1 << ctr);
    }
    if( flag != 0 ) {
        printf( "There are more than the expected number of files!\n" );
        PROG_ABORT( __LINE__ );
    }
    NowTest( "_dos_getfileattr()" ) ;
    if( _dos_getfileattr( "tmpfile", &attribute ) != 0 ) {
        printf( "_dos_getfileattr() failed.\n" );
        PROG_ABORT( __LINE__ );
    }
    if( ( attribute & _A_RDONLY ) != _A_RDONLY ) {
        printf( "_dos_getfileattr() failed.\n" );
        PROG_ABORT( __LINE__ );
    }
    if( _dos_getfileattr( "_tmpfile.999", &attribute ) != 0 ) {
        printf( "_dos_getfileattr() failed.\n" );
        PROG_ABORT( __LINE__ );
    }
    if( ( attribute & _A_RDONLY ) == _A_RDONLY ) {
        printf( "_dos_getfileattr() failed.\n" );
        PROG_ABORT( __LINE__ );
    }
    NowTest( "_dos_setfileattr()" ) ;
    if( _dos_setfileattr( "tmpfile", _A_NORMAL ) != 0 ) {
        printf( "_dos_setfileattr() failed.\n" );
        PROG_ABORT( __LINE__ );
    }
    NowTest( "_dos_open()" );
    if( _dos_open( "tmpfile", O_WRONLY, &fh[0] ) != 0 ) {
        printf( "_dos_open() failed.\n" );
        PROG_ABORT( __LINE__ );
    }
    NowTest( "_dos_write()" );
    if( _dos_write( fh[0], buffer, sizeof(buffer), &len ) != 0 ||
        len != sizeof(buffer) ) {
        printf( "_dos_write() failed.\n" );
        PROG_ABORT( __LINE__ );
    }
    _dos_close( fh[0] );
    NowTest( "_dos_read()" );
    if( _dos_open( "tmpfile", O_RDWR, &fh[0] ) != 0 ) {
        printf( "_dos_open() failed.\n" );
        PROG_ABORT( __LINE__ );
    }
    if( _dos_read( fh[0], tmpbuff, 80, &len ) != 0 ||
        len != sizeof(buffer) || strcmp( buffer, tmpbuff ) != 0 ) {
        printf( "_dos_read() failed.\n" );
        PROG_ABORT( __LINE__ );
    }
    NowTest( "_dos_getftime()" );
    _dos_getftime( fh[0], &date, &time );
    /* add a 8 hour change allowance for the fact that TNT doesn't */
    /* handle the switch from system time to local time properly */
    if( ddate.month != MONTH( date ) ||
        ddate.day != DAY( date ) ||
        ddate.year != YEAR( date ) ||
        (dtime.hour != HOUR( time ) && dtime.hour != (HOUR( time ) + 8) ) ) {
        printf( "Incorrect time stamp retrieved.\n" );
        PROG_ABORT( __LINE__ );
    }
    NowTest( "_dos_setftime()" );
    date = (0 << 9) + (4 << 5) + 1;     // apr-1-1980
    time = (12 << 11) + (34 << 5) + 28; // 12:34:56
    _dos_setftime( fh[0], date, time );
    _dos_getftime( fh[0], &date, &time );
    if( YEAR( date ) != 1980 || MONTH( date ) != 4 || DAY( date ) != 1 ||
        HOUR( time ) != 12 || MINUTE( time ) != 34 || SECOND( time ) != 56 ) {
        printf( "_dos_setftime() failed.\n" );
        PROG_ABORT( __LINE__ );
    }
    NowTest( "_dos_close()" );
    if( _dos_close( fh[0] ) != 0) {
        printf( "_dos_close() failed.\n" );
        PROG_ABORT( __LINE__ );
    }
    if( unlink( "tmpfile" ) ) {
        printf( "Error: unable to remove \"tmpfile\"\n" );
    }
    strcpy( str, "_tmpfile." );
    for( ctr = 0; ctr < MAX_OPEN; ++ctr ) {
        itoa( 999 - ctr, &str[9], 10 );
        if( unlink( str ) ) {
            printf( "Couldn't remove temporary file \"%s\"\n", str );
        }
    }
}

void TestDiskOperations( void )
{
    unsigned            drive;

    #if !( defined(__OS2__) )
    struct diskfree_t   disk_data;

    NowTest( "_dos_getdiskfree()" );
    if( verbose ) printf( "Getting information about drive C:\n" );
    if( _dos_getdiskfree( 3, &disk_data ) != 0 ) {
        printf( "C: is reported to be invalid.\n" );
        PROG_ABORT( __LINE__ );
    } else {
        if( verbose ) {
            printf( "total clusters    : %u\n", disk_data.total_clusters );
            printf( "available clusters: %u\n", disk_data.avail_clusters );
            printf( "sectors / cluster : %u\n", disk_data.sectors_per_cluster );
            printf( "bytes / sector    : %u\n", disk_data.bytes_per_sector );
        }
    }
    #endif
    NowTest( "_dos_getdrive()" );
    _dos_getdrive( &drive );
    if( verbose ) printf( "the current drive is %c:\n", 'A' + drive - 1 );
}

void TestLFNAndDOSInterrupts( void )
{
    char            upname[_MAX_FNAME];
    struct find_t   findresult; /* Struct to hold findfirst information */
    int             handle;     /* File handle for _dos_open */
    unsigned        finderror;  /* The return value of findfirst/next */
    unsigned        attr;       /* File attributes */
#ifdef __WATCOM_LFN__
    int             handle2;    /* Handle for the Long Filename */

    _dos_creat( LFN_FNAME, _A_NORMAL, &handle2 );   /* Create a LFN */
    _dos_setfileattr( LFN_FNAME, _A_HIDDEN );       /* Give Hidden attr */
#endif
    finderror = _dos_findfirst( "*.*", _A_RDONLY | _A_HIDDEN | _A_SYSTEM |
                    _A_SUBDIR | _A_ARCH, &findresult );
    while( finderror == 0 ) {
        if( verbose ) {
            printf( "Found file: %s", findresult.name );
            strupr( strcpy( upname, findresult.name ) );
            if( strchr( findresult.name, ' ' ) != NULL ||
                strlen( findresult.name ) > 12 ||
                strcmp( findresult.name, upname ) != 0) {
                printf( ", a long filename" );
            }
            printf( "\n" );
        }
        /* Don't attempt to open "." and "..", it won't work */
        if( !strcmp( findresult.name, "." ) && !strcmp( findresult.name, ".." ) ) {
            if( _dos_open( findresult.name, O_RDONLY, &handle ) != 0 ) {
                printf( "_dos_open() failed for %s\n", findresult.name );
                PROG_ABORT( __LINE__ );
            }
        }
        if( _dos_getfileattr( findresult.name, &attr ) != 0 ) {
            printf( "Could not get attributes for %s\n", findresult.name );
            PROG_ABORT( __LINE__ );
        } else if( verbose ) {
            printf( "The attributes for %s are:\n", findresult.name );
            if( attr & _A_NORMAL ) printf( "Normal\n" );
            if( attr & _A_RDONLY ) printf( "Read Only\n" );
            if( attr & _A_HIDDEN ) printf( "Hidden\n" );
            if( attr & _A_SYSTEM ) printf( "System\n" );
            if( attr & _A_SUBDIR ) printf( "Directory\n" );
            if( attr & _A_ARCH   ) printf( "Archive\n" );
        }
        finderror = _dos_findnext( &findresult );
    }
    _dos_findclose( &findresult );
#ifdef __WATCOM_LFN__
    _dos_close( handle2 );
    unlink( LFN_FNAME );
#endif
}

#ifdef DO_INTERRUPT
void __interrupt __far timer_rtn( void )
{
    ++clock_ticks;
    if( clock_ticks >= TICK_COUNT ) {
        if( verbose ) printf( "Interrupt!\n" );
        clock_ticks -= TICK_COUNT;
        delay_flag = 0;
    }
    _chain_intr( prev_int_1c );
}

void TestInterruptOperations( void )
{
    NowTest( "_dos_getvect(), _dos_setvect(), and _chain_intr()" );
    if( verbose ) {
        printf( "Looping until the interrupt occurs in " );
        printf( "%d second(s).\n", INTERRUPT_TIME );
    }
    fflush( stdout );
    prev_int_1c = _dos_getvect( 0x1c );
    _dos_setvect( 0x1c, timer_rtn );
    while( delay_flag ) delay_flag *= 1;
    _dos_setvect( 0x1c, prev_int_1c );
}
#endif

int main( int argc, char *argv[] )
{
#ifdef __SW_BW
    FILE *my_stdout;
    my_stdout = freopen( "tmp.log", "a", stdout );
    if( my_stdout == NULL ) {
        fprintf( stderr, "Unable to redirect stdout\n" );
        exit( -1 );
    }
#endif
    if( argc > 1 )
        verbose = 1;
    runtime = getenv( "RUNTIME" );
    if( runtime == 0 )
        runtime = "";
    TestDateTimeOperations();   // This should go first
    TestFileOperations();
#if !( defined(__WINDOWS__) || defined(__WINDOWS_386__) )
    TestMemoryOperations();
#endif
    TestDiskOperations();
    TestLFNAndDOSInterrupts();
#ifdef DO_INTERRUPT
    TestInterruptOperations();
#endif
    printf( "Tests completed (%s).\n", strlwr( argv[0] ) );
#ifdef __SW_BW
    fprintf( stderr, "Tests completed (%s).\n", strlwr( argv[0] ) );
    fclose( my_stdout );
    _dwShutDown();
#endif
    return( 0 );
}
