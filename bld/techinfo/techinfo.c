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
* Description:  Techhelp utility. This program was originally designed to
*               help the oh so overworked technical support department at
*               WATCOM. It helps to resolve any ambiguities that always seem
*               to be present when getting information from an user about
*               the environment that he/she runs in.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <conio.h>
#include <time.h>
#include <process.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef __OS2__
#define INCL_DOSINFOSEG
#define INCL_DOSMEMMGR
#define INCL_DOSDEVICES
#include <os2.h>
#include <i86.h>
#else
#include <dos.h>
#endif
#include <fcntl.h>
#if defined(__QNX__)
#include <unistd.h>
#else
#include <io.h>
#endif

extern int PType( void );
extern int PBus( void );
extern int PPrefix( void );
extern int NDPType( void );
#ifdef __DOS__
extern int monint( int print );
#endif

#define BANNER "Watcom's Techinfo Utility, Version 1.4"

/* The following define for LEVEL was borrowed from definition in BPATCH */
#define LEVEL           "WATCOM patch level .xxx"
#define LEVEL_HEAD_SIZE (sizeof(LEVEL)-5)       /* w/o ".xxx\0" */

/****************************************************************************/
/*   define the environment variables relevant to WATCOM                    */
/****************************************************************************/

char *WATCOMC_EnvVars[] = {
    "WATCOM",
    "EDPATH",
    "INCLUDE",
    "DOS_INCLUDE",
    "NT_INCLUDE",
    "OS2_INCLUDE",
    "QNX_INCLUDE",
    "WINDOWS_INCLUDE",
    "FINCLUDE",
    "FORCE",
    "INC386",
    "LIB",
    "LIB286",
    "LIB386",
    "LIBDOS",
    "LIBNOV",
    "LIBOS2",
    "LIBOS2FLAT",
    "LIBPHAR",
    "LIBWIN",
    "NO87",
    "PATH",
    "TMP",
    "WCGMEMORY",
    "WCC",
    "WCC386",
    "WCL",
    "WCL386",
    "WD",
    "WDW",
    "WFC",
    "WFC386",
    "WFL",
    "WFL386",
    "WPP",
    "WPP386",
    "WVIDEO",
    "BOOKSHELF",    /* OS/2 */
    "HELP",         /* OS/2 */
    NULL };

char *WSQL_EnvVars[] = {
    "SQLPATH",
    "SQLSTART",
    "SQLCONNECT",
    "SQLLOGON",
    "WSQL",
    "SQLANY",
    NULL };


char *WATCOMC_Tools[] = {
    // This list does not include all of the tools, just the major ones
    "ide",
    "watfor77",
    "watfor87",
    "wbrw",
    "wd",
    "wcc",
    "wcc386",
    "wf386",
    "wf387",
    "wmake",
    "wpp",
    "wpp386",
//    "wccp",
//    "wcc386p",
    "wfc",
    "wfc386",
//    "wfcp",
//    "wfc386p",
    "wvideo",
    "wlink",
    "wlib",
    NULL };

char *WSQL_Tools[] = {
    // This list does not include all of the tools, just the major ones
//    "acme",
//    "acme32",
    "dbeng50",
    "dbeng40",
    "dbclient",
    "dbsrv50",
    "dbsrv40",
//    "dbstart",
    "isql",
//    "rtacme",
//    "rtacme32",
    "rtdsk50",
    "rtdsk40",
    "rtsql",
    "sqlpp",
//    "acmew",
/*  Windows tools */
    "dbeng50w",
    "dbeng40w",
    "dbclienw",
    "dbsrv50w",
    "dbsrv40w",
    "isqlw",
    "rtdsk50w",
    "rtdsk40w",
//    "rtacmew",
    "rtsqlw",
    NULL };

char *WATCOMC_Dirs[] = {
    "\\bin",
    "\\binb",
    "\\binp",
    "\\binw",
    "\\binnt",
    NULL };

char *WSQL_Dirs[] = {
    "\\dos",
    "\\win",
    "\\win32",
    "\\nt",
    "\\os2",
    "\\nlm",
    "\\qnx",
    "\\mac",
    NULL };

char *ProcessorType[2][8][2] =
{
    {
        { "8086",   "8088"  },
        { "80186",  "80188" },
        { "80286",  "unknown" },
        { "80386",  "unknown" },
        { "80486",  "unknown" },
        { "Intel Pentium",  "unknown" },
        { "Intel Pentium Pro/II/III",  "unknown" },
        { "Intel Pentium 4",  "unknown" }
    },

    {
        { "NEC V30",      "NEC V20" },
        { "unknown",      "unknown" },
        { "unknown",      "unknown" },
        { "unknown",      "unknown" },
        { "unknown",      "unknown" },
        { "unknown",      "unknown" },
        { "unknown",      "unknown" },
        { "unknown",      "unknown" }
    }
};

char *CoProcessorType[8] =
{
    "No",
    "8087",
    "80287",
    "80387",
    "80486 DX processor or 80487 SX",
    "Pentium",
    "Pentium Pro/II/III",
    "Pentium 4"
};

char *IsNotIs[2] =
      { "is NOT",
        "IS" };

//char BPATCHPATH[256];
char WATCOMPATH[256];
int  LineCount = 0;
FILE *TechOutFile;

void Usage( void )
/****************/
{
    printf( BANNER "\n" );
#if defined( __OS2__ )
    printf( "<Usage>:  techinfo\n" );
#else
    printf( "<Usage>:  techinfo [-v]\n" );
#endif
}

void techoutput( char *format, ... )
{

//  int vprintf( const char *format, va_list arg );
//  int vfprintf( FILE *fp, const char *format, va_list arg );

    va_list     args;

    va_start( args, format );
    vprintf( format, args );
    va_end( args );

    va_start( args, format );
    vfprintf( TechOutFile, format, args );
    va_end( args );

    ++LineCount;
    if( (format[0] == '\n') && (strlen(format) != 1) ) ++LineCount;
    if( LineCount > 23 ) {
        printf( "Press [Enter] to continue..." );
        fflush( stdout );
        getch();
        putchar( '\n' );
        LineCount = 0;
    }
}

void do_company_header( void )
/****************************/
{
    time_t              time_of_day;

    time_of_day = time( NULL );
    techoutput( BANNER "\n" );
    techoutput( "Current Time: %s", ctime( &time_of_day) );
    techoutput( "Visit http://www.openwatcom.org/\n" );
    techoutput( "\n" );
}

void print_env_var( const char * env_var )
/****************************************/
{
    char        *path;

    path = getenv( env_var );
    if( path != NULL ) {
        if( (strlen( env_var ) + strlen( path )) > 76 ) LineCount++;
        techoutput( "%s=<%s>\n", env_var, path );
    }
}

static char CurrLevel[ sizeof( LEVEL )  ];

void GetLevel( char *name )
{
    int         io;
    char        buffer[ sizeof( LEVEL ) ];

    CurrLevel[0] = '\0';
    io = open( name, O_BINARY+O_RDONLY );
    if( lseek( io, -(long)sizeof( LEVEL ), SEEK_END ) != -1L &&
        read( io, buffer, sizeof( LEVEL ) ) == sizeof( LEVEL ) &&
        memcmp( buffer, LEVEL, LEVEL_HEAD_SIZE ) == 0 ) {
        strcpy( CurrLevel, buffer + LEVEL_HEAD_SIZE );
    }
    close( io );
}

void patch_tool( char *tool, char **dirs )
{
    char            toolbuff[159];
    int             i;
    unsigned        rc;
    struct stat     info;

    for( i = 0; dirs[i] != NULL; ++i ) {
        strcpy( toolbuff, WATCOMPATH );
        strcat( toolbuff, dirs[i] );
        strcat( toolbuff, "\\" );
        strcat( toolbuff, tool );
        strcat( toolbuff, ".exe" );
        rc = stat( toolbuff, &info );
        if( rc == 0 ){
#if 0
            spawnl( P_WAIT, BPATCHPATH, BPATCHPATH, "-q", toolbuff, NULL );
#else
            GetLevel( toolbuff );
            if( CurrLevel[ 0 ] == '\0' ) {
                techoutput( "File '%s' has not been patched\n", toolbuff );
            } else {
                techoutput( "File '%s' has been patched to level '%s'\n",
                        toolbuff, CurrLevel );
            }
#endif
        }
    }
}


void get_compiler_patch( void )
/*****************************/
{
/***************************************************************************/
/*   This function first dumps out all relevant WATCOM environment vars    */
/*   It then uses bpatch to find patch levels of compiler executables.     */
/***************************************************************************/
    char *              path;
    unsigned            i;

    techoutput( "------------------Watcom C Environment Variables ----------------\n" );
    for( i = 0; WATCOMC_EnvVars[i] != NULL; ++i ) {
        print_env_var( WATCOMC_EnvVars[i] );
    }

    path = getenv( "WATCOM" );
    if( path != NULL ) {
        strcpy( WATCOMPATH, path );
        fflush( stdout );
        for( i = 0; WATCOMC_Tools[i] != NULL; ++i ) {
            patch_tool( WATCOMC_Tools[i], WATCOMC_Dirs );
        }
    } else {
        techoutput( "... ERROR...WATCOM environment variable not set.\n" );
    }
}

void get_wsql_patch( void )
/*************************/
{
/***************************************************************************/
/*   This function first dumps out all relevant WATCOM environment vars    */
/*   It then uses bpatch to find patch levels of compiler executables.     */
/***************************************************************************/
    char *              path;
    unsigned            i;

    techoutput( "------------------Watcom SQL Environment Variables ----------------\n" );
    for( i = 0; WSQL_EnvVars[i] != NULL; ++i ) {
        print_env_var( WSQL_EnvVars[i] );
    }

    path = getenv( "SQLANY" );
    if( path == NULL ) {
        path = getenv( "WSQL" );
    }
    if( path != NULL ) {
        strcpy( WATCOMPATH, path );
        fflush( stdout );
        for( i = 0; WSQL_Tools[i] != NULL; ++i ) {
            patch_tool( WSQL_Tools[i], WSQL_Dirs );
        }
    } else {
        techoutput( "... ERROR...SQLANY environment variable not set.\n" );
    }
}

void get_mem_info( void )
/***********************/
{
#ifdef __OS2__
    ULONG       mem;

    DosMemAvail( &mem );
    techoutput( "\nAmount of free memory %ld bytes\n", (ULONG) mem );
#else
    union REGS          regs;
    unsigned long       mem;

    regs.h.ah = 0x88;
    int86( 0x15, &regs, &regs );
    techoutput( "\nAmount of extended memory is: %dK\n", regs.x.ax );
    int86( 0x12, &regs, &regs );
    techoutput( "Amount of base memory is: %dK\n", regs.x.ax );
    regs.h.ah = 0x48;
    regs.x.bx = 0xffff;
    int86( 0x21, &regs, &regs );
    if( regs.w.cflag != 0 ) {
        mem = regs.x.bx;
        mem *= 16;
        techoutput( "Amount of free base memory is: %l bytes\n", mem );
    }
#endif
}

void dump_files( char drive_name )
/********************************/
{
    char               filename[17];
    FILE               *fp;
    char               buffer[256];

    filename[0] = drive_name;
    strcpy( &filename[1], ":\\CONFIG.SYS" );
    techoutput( "\n------------%s-------------\n", filename );
    fp = fopen( filename, "r" );
    if( fp != NULL ) {
        while( fgets( buffer, 256, fp ) != NULL ) {
            if( strlen( buffer ) > 80 ) LineCount++;
            techoutput( "%s", buffer );
        }
        fclose( fp );
    } else {
        techoutput( "Cannot find %s\n", filename );
    }

    strcpy( &filename[1], ":\\AUTOEXEC.BAT" );
    techoutput( "\n------------%s-------------\n", filename );
    fp = fopen( filename, "r" );
    if( fp != NULL ) {
        while( fgets( buffer, 256, fp ) != NULL ) {
            if( strlen( buffer ) > 80 ) LineCount++;
            techoutput( "%s", buffer );
        }
        fclose( fp );
    } else {
        techoutput( "Cannot find %s\n", filename );
    }
}


void get_config_files( void )
/***************************/
{
    int                 drive_name;
#ifdef __OS2__
    SEL                 gbl_seg;
    SEL                 lcl_seg;
    PGINFOSEG           gbl;

    DosGetInfoSeg( &gbl_seg, &lcl_seg );
    gbl = MK_FP( gbl_seg, 0 );
    techoutput( "OS/2 Version %d.%d%c\n", gbl->uchMajorVersion,
                gbl->uchMinorVersion, gbl->chRevisionLetter);
    drive_name = gbl->bootdrive + 'A' - 1;
#else
    union REGS          regs;
    char                *comspec;

    regs.h.ah = 0x30;
    intdos( &regs, &regs );
    techoutput( "DOS Version %d.%d\n", regs.h.al, regs.h.ah );
    /*   Get Drive that user booted off of   */
    if( regs.h.al >= 0x04 ) {
        regs.w.ax = 0x3305;
        intdos( &regs, &regs );
        drive_name = regs.h.dl + 'A' - 1;
    } else {
        comspec = getenv( "COMSPEC" );
        if( comspec != NULL ) {
            drive_name = toupper( *comspec );
        } else {
            return;
        }
    }
#endif
    dump_files( drive_name );
}

#ifndef __OS2__
int NDPEquip( void )
{
    union REGS          regs;

    int86( 0x11, &regs, &regs );
    return( (regs.x.ax >> 1) & 0x0001 );
}
#endif

void machine_type( int print )
/****************************/
{
    int manuf, plevel, buswidth, ndptype, ndp;

    techoutput( "\n------------------------------------------------------\n");
    manuf = 0;
    buswidth = 0;
    plevel = PType();
    if( plevel < 2 ) buswidth = PBus();
    if( plevel < 1 ) manuf = PPrefix();
    ndptype = NDPType();
    if( (plevel ==  4) && (ndptype == 3) ) ndptype = 4;
    if( (plevel ==  5) && (ndptype == 3) ) ndptype = 5;
    if( (plevel ==  6) && (ndptype == 3) ) ndptype = 6;
    if( (plevel ==  7) && (ndptype == 3) ) ndptype = 7;
    techoutput( "An %s processor is installed in this system.\n",
                ProcessorType[manuf][plevel][buswidth] );

#ifdef __OS2__
    DosDevConfig( &ndp, 3, 0 );
    if( ndp != 0 ) ndp = 1;
#else
    ndp = NDPEquip();
#endif
    techoutput( "%s math coprocessor (NDP) is present\n",
                CoProcessorType[ ndptype ] );
    techoutput( "\tand Equipment Flags say math coprocessor %s present.\n",
                IsNotIs[ ndp ] );
#ifdef __OS2__
    techoutput( "Skipping math coprocessor interrupt check since this is not DOS.\n" );
    print = print; /* stop the warning message */
#else
    if( ndptype != 0 ) {
        LineCount++;
        techoutput( "The next test may cause the system to hang if math\n"
                "\tcoprocessor (NDP) interrupts are not handled properly.\n" );
        techoutput( "%s (NDP) interrupts are %s.\n",
                    CoProcessorType[ ndptype ],
                    monint( print ) ?
                        "properly enabled" :
                        "not enabled (math exceptions will not be detected!)"
                  );
    } else {
        techoutput( "Skipping math coprocessor interrupt check.\n" );
    }
#endif
}

#ifndef __OS2__
void check_dos_comm( void )
/*************************/
{
    /*   Check for the share and append utilities    */
    union REGS          regs;

    techoutput( "\n------------------------------------------------------\n");
    regs.w.ax = 0xB700;
    int86( 0x2F, &regs, &regs );
    if( regs.h.al == 0xFF )
        techoutput( "APPEND                  INSTALLED\n" );
    else
        techoutput( "APPEND                  NOT INSTALLED\n" );
}
#endif

int main( int argc, char *argv[] )
{
    char    *opt;
    int     print;

    print = 0;
    if( argc > 1 ) {
        opt = argv[1];
        if( *opt == '/' || *opt == '-' ) {
            if( opt[1] == 'v' || opt[1] == 'V' ) {
                print = 1;
            }
        }
        if( print == 0 ) {
            Usage();
            exit(0);
        }
    }
    TechOutFile = fopen( "techinfo.out", "w" );
    do_company_header();
    get_compiler_patch();
    get_wsql_patch();
    get_mem_info();
    get_config_files();
    machine_type( print );
#ifndef __OS2__
    check_dos_comm();
#endif
    fclose( TechOutFile );
    return( 0 );
}
