#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef __UNIX__
#include <dirent.h>
#else
#include <direct.h>
#define mkdir(a,b) mkdir(a)
#endif

#define BASEPATH "../include"
#define BASEPATHLEN ( sizeof( BASEPATH ) )

#define MAX_PATH_NAME 512

// global variable to maintain what
// file currently being written
FILE *makefile;

//--------------------------------------------------------------------------
// defs of funtions at tail of program
int CompareMask( char *mask, char *name, int keepcase );

#define SFF_SUBCURSE        1 // go into subdirectories
#define SFF_DIRECTORIES 2 // return directory names also
#define SFF_NAMEONLY        4 // don't concatenate base with filename to result.

// flags sent to Process when called with a matching name
#define SFF_DIRECTORY   1 // is a directory...
#define SFF_DRIVE           2 // this is a drive...
int ScanFiles( char *base
             , char *mask
             , void **pInfo
             , void Process( char *name, int flags )
             , int flags
             );


//--------------------------------------------------------------------------

void WriteTargetList( char *filename, int flags )
{
    if( flags & SFF_DIRECTORY )
    {
    }
    else
    {
        fprintf( makefile, "             %s&\n", filename + BASEPATHLEN );
    }
}

//--------------------------------------------------------------------------

void WriteDeps( char *filename, int flags )
{
    filename += BASEPATHLEN;
    if( flags & SFF_DIRECTORY )
    {

    }
    else
    {
        char outname[64];
        char *ext = strrchr( filename, '.' );
        snprintf( outname, sizeof( outname ), "../watcom/%*.*s.mh", ext-filename, ext-filename, filename );
        fprintf( makefile, "%-20s: %s ../readonly.sp\n", filename, outname );
    }
}

//--------------------------------------------------------------------------

void WriteMHFiles( char *filename, int flags )
{
    filename += BASEPATHLEN;
    if( flags & SFF_DIRECTORY )
    {
        char outpath[MAX_PATH_NAME];
        snprintf( outpath, MAX_PATH_NAME, "watcom/%s", filename );
        mkdir( outpath, S_IRWXU | S_IRWXG | S_IRWXO );    // make the path...
        snprintf( outpath, MAX_PATH_NAME, "win/%s", filename );
        mkdir( outpath, S_IRWXU | S_IRWXG | S_IRWXO );
    }
    else
    {
        char *ext = strrchr( filename, '.' );
        if( ext )
        {
            FILE *out;
            char outname[MAX_PATH_NAME];
            snprintf( outname, MAX_PATH_NAME, "watcom/%*.*s.mh"
                     , ext-filename
                     , ext-filename
                     , filename );
            out = fopen( outname, "wt" );
            if( out )
            {
                fprintf( out, ":include ../include/%s\n", filename );
                fprintf( out, ":include readonly.sp\n" );
                fclose( out );
            }
        }
    }
}

//--------------------------------------------------------------------------

void ScanHeaders( void (*proc)(char *filename, int flags) )
{
    void *info = NULL;
    while( ScanFiles( "../include"
                         , "*.h"
                         , &info
                         , proc
                         , SFF_DIRECTORIES | SFF_SUBCURSE ) );
}

//--------------------------------------------------------------------------

int main( void )
{
    mkdir( "watcom", S_IRWXU | S_IRWXG | S_IRWXO );  // make the path...
    mkdir( "win", S_IRWXU | S_IRWXG | S_IRWXO );

    makefile = fopen( "master.mif", "wt" );
    if( makefile )
    {
        fprintf( makefile, "proj_name=w32api\n" );
        fprintf( makefile, "host_CPU=386\n" );
        fprintf( makefile, "host_OS=nt\n" );
        fprintf( makefile, "!include cproj.mif\n" );
        fprintf( makefile, "splice = wsplice -i.. -i../../include -k$(system) $(options) ../common.sp $[@ $^@\n" );
        fprintf( makefile, ".EXTENSIONS\n" );
        fprintf( makefile, ".EXTENSIONS : .h .hpp .mh .mhp\n" );
        fprintf( makefile, "common: &\n" );
        ScanHeaders( WriteTargetList );
        fprintf( makefile, "\n" );
        fprintf( makefile, "WIN_files = &\n" );
        fprintf( makefile, "                  $(common)\n" );
        fprintf( makefile, "\n" );
        fprintf( makefile, "all : $($(system)_files) .symbolic\n" );
        fprintf( makefile, "        @%%null\n" );
        fprintf( makefile, ".mh:$(path)\n" );
        fprintf( makefile, ".mhp:$(path)\n" );
        fprintf( makefile, ".mhp.hpp:\n" );
        fprintf( makefile, "             $(splice)\n" );
        fprintf( makefile, ".mh.h:\n" );
        fprintf( makefile, "             $(splice)\n" );
        fprintf( makefile, "!include ../deps.mif\n" );
        fprintf( makefile, "clean: .SYMBOLIC\n" );
        fprintf( makefile, "          rm -f *.h sys/*.h *.hpp common.cnv\n" );
        fclose( makefile );
    }
    makefile=fopen( "deps.mif", "wt" );
    if( makefile )
    {
        ScanHeaders( WriteDeps );
        fclose( makefile );
    }

    makefile=fopen( "win/makefile", "wt" );
    if( makefile )
    {
        fprintf( makefile, "#pmake: all build os_win os_nt \n" );
        fprintf( makefile, "path=../watcom;../watcom/gl\n" );
        fprintf( makefile, "options=-kUNICODE\n" );
        fprintf( makefile, "system=WIN\n" );
        fprintf( makefile, "!include ../master.mif\n" );
        fclose( makefile );
    }
    ScanHeaders( WriteMHFiles );

    return 0;
}


int CompareMask( char *mask, char *name, int keepcase )
{
    int m = 0, n = 0;
    int anymatch;
    int wasanymatch, wasmaskmatch;
    int matchone;
    char namech, maskch;
    if( !mask )
        return 1;
    if( !name )
        return 0;
try_mask:
    anymatch = 0;
    wasanymatch = 0;
    wasmaskmatch = 0;
    matchone = 0;
    do
    {
        if( mask[m] == '\t' || mask[m] == '|' )
        {
            //Log1( "Found mask seperator - skipping to next mask :%s", name );
            n = 0;
            m++;
            continue;
        }
        while( mask[m] == '*' )
        {
            anymatch = 1;
            m++;
        }
        while( mask[m] == '?' )
        {
            matchone++;
            m++;
        }
        if( !keepcase && name[n]>= 'a' && name[n] <= 'z' )
            namech = name[n] - ('a' - 'A');
        else
            namech = name[n];
        if( !keepcase && mask[m]>= 'a' && mask[m] <= 'z' )
            maskch = mask[m] - ('a' - 'A');
        else
            maskch = mask[m];
        if( matchone )
        {
            matchone--;
            n++;
        }
        else if( maskch == namech )
        {
            if( anymatch )
            {
                wasanymatch = n;
                wasmaskmatch = m;
                anymatch = 0;
            }
            n++;
            m++;
        }
        else if( anymatch )
        {
            n++;
        }
        else if( wasanymatch )
        {
            n = wasanymatch;
            m = wasmaskmatch;
            anymatch = 1;
            n++;
        }
        else
        {
            break;
        }
    }while( name[n] );
    if( mask[m] && ( mask[m] != '\t' && mask[m] != '|' ) )
    {
        int mask_m = m;
        while( mask[m] )
        {
            if( mask[m] == '\t' || mask[m] == '|' )
            {
                n = 0;
                m++;
                break;
            }
            m++;
        }
        if( mask[m] )
            goto try_mask;
        m = mask_m;
    }
    // match ???? will not match abc
    // a??? abc not match
    if( !matchone && (!mask[m] || mask[m] == '\t' || mask[m] == '|' ) && !name[n] )
        return 1;
    return  0;
}

typedef struct myfinddata {
    DIR *handle;
    struct dirent *fd;
    char buffer[MAX_PATH_NAME];
} MFD, *PMFD;

#define findhandle(pInfo) ( ((PMFD)(*pInfo))->handle)
#define finddata(pInfo) ( ((PMFD)(*pInfo))->fd)
#define findbuffer(pInfo) ( ((PMFD)(*pInfo))->buffer)

int ScanFiles( char *base
             , char *mask
             , void **pInfo
             , void Process( char *name, int flags )
             , int flags
             )
{
    int sendflags;
    struct stat st;
    if( !*pInfo )
    {
        char findmask[256];
        sprintf( findmask, "%s/*", base );
        *pInfo = malloc( sizeof( MFD ) );
        findhandle(pInfo) = opendir( base );
        if( findhandle(pInfo) == NULL )
        {
            free( *pInfo );
            *pInfo = NULL;
            return 0;
        }
    }
    else
    {
    getnext:
        if( ( finddata( pInfo ) = readdir( findhandle(pInfo) ) ) == NULL )
        {
            closedir( findhandle(pInfo) );
            free( *pInfo );
            *pInfo = NULL;
            return 0;
        }
    }
    if( !strcmp( ".", finddata(pInfo)->d_name ) ||
         !strcmp( "..", finddata(pInfo)->d_name ) )
        goto getnext;
    if( flags & SFF_NAMEONLY )
        strncpy( findbuffer( pInfo ), finddata(pInfo)->d_name, MAX_PATH_NAME );
    else
        snprintf( findbuffer( pInfo ), MAX_PATH_NAME, "%s/%s", base, finddata(pInfo)->d_name );
    findbuffer( pInfo )[MAX_PATH_NAME-1] = 0; // force nul termination...
    stat( findbuffer( pInfo ), &st );
    if( ( flags & (SFF_DIRECTORIES|SFF_SUBCURSE) )
         && S_ISDIR(st.st_mode) )
    {
        if( flags & SFF_SUBCURSE  )
        {
            void *data = NULL;
            if( flags & SFF_DIRECTORIES )
                if( Process )
                    Process( findbuffer( pInfo ), SFF_DIRECTORY );
            if( flags & SFF_NAMEONLY ) // if nameonly - have to rebuild the correct name.
                snprintf( findbuffer( pInfo ), MAX_PATH_NAME, "%s/%s", base, finddata(pInfo)->d_name );
            while( ScanFiles( findbuffer(pInfo), mask, &data, Process, flags ) );
        }
        goto getnext;
    }

    if( ( sendflags = SFF_DIRECTORY, ( ( flags & SFF_DIRECTORIES )
            && ( S_ISDIR( st.st_mode ) ) ) )
         || ( sendflags = 0, CompareMask( mask, finddata(pInfo)->d_name, 0 ) ) )
    {
        if( Process )
            Process( findbuffer( pInfo ), sendflags );
        return 1;
    }
    return 1;
}

