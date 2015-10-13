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
* Description:  Perform binary patching.
*
****************************************************************************/


#include "bdiff.h"
#include "bool.h"
#include "msg.h"
#include "newfile.h"
#include "oldfile.h"
#include "patchio.h"
#include "myio.h"
#include "msg.h"
#include "installp.h"

#ifdef BDIFF
    #define PatchName ""

    #define OpenPatch()                 (pat=PatchFile,PATCH_RET_OKAY)
    #define InPatch( type )             (tmp=pat,pat+=sizeof(type),*(type*)tmp)
    #define ClosePatch()

    #define InNew( offset )             (*(hole*)(dest+offset))
    #define OutNew( offset, x, type )   *(type*)(dest+(offset))=(x)

    #define FindOld( name )             NULL
    #define SetOld( name )              NULL
    #define OpenOld( len, prompt, newsize, newsum ) PATCH_RET_OKAY
    #define InOld( offset )             (*(byte*)(OldFile+offset))
    #define CloseOld( havenew, dobackup )

    #define Dump( x )
    #define DOPROMPT                    true
    #define DOBACKUP                    true

#else  /* Not BDIFF */

  #if defined(__386__)
    #if defined(_WPATCH) || defined( INSTALL_PROGRAM )

    #define InNew( offset )             ( Input( &NewFile, tmp, offset, sizeof(hole)), *(hole*)tmp )
    #define OutNew( off, x, type )      *(type*)tmp = (x); Output( &NewFile, tmp, off, sizeof( type ) );

    #else

    #define OutNew( off, x, type )      *(type*)(NewFile+off) = (x);
    #define InNew( off )                *(hole*)(NewFile+off)

    #endif

    #define Dump( x )
    #define DOPROMPT                    true
    #define DOBACKUP                    DoBackup

  #elif defined(BDUMP)

    #define InNew( offset )             1
    #define OutNew( off, x, type )      //( x )

    #define Dump( x )                   printf x
    #define DOPROMPT                    false
    #define DOBACKUP                    false

  #else

    #define InNew( offset )             ( Input( &NewFile, tmp, offset, sizeof(hole)), *(hole*)tmp )
    #define OutNew( off, x, type )      *(type*)tmp = (x); Output( &NewFile, tmp, off, sizeof( type ) );

    #define Dump( x )
    #define DOPROMPT                    true
    #define DOBACKUP                    DoBackup

  #endif

    #define InPatch( type )             ( InputPatch( tmp, sizeof( type ) ), *(type*)tmp )

#endif

static char CurrLevel[sizeof( PATCH_LEVEL )];

#if defined( BDIFF )

static byte     *pat;

#else

const char      *PatchName;
const char      *NewName;
bool            DoPrompt;
bool            DoBackup;
bool            PrintLevel;

#endif

#if !defined(BDIFF) && !defined(__386__) && !defined(BDUMP)

/* Real mode version. Buffer up the holes and apply in sorted order */

typedef struct {
    foff        offset;
    foff        diff;
} save_hole;

static int         NumHoles;
static save_hole   *HoleArray = NULL;
static int         HoleArraySize;

static PATCH_RET_CODE InitHoles( void )
{
    NumHoles = 0;
    HoleArraySize = (64*1024L) / sizeof( save_hole ) - 1;
    for( ;; ) {
        HoleArray = bdiff_malloc( HoleArraySize*sizeof(save_hole) );
        if( HoleArray != NULL )
            break;
        HoleArraySize /= 2;
        if( HoleArraySize < 100 ) {
            PatchError( ERR_MEMORY_OUT );
            return( PATCH_NO_MEMORY );
        }
    }
    return( PATCH_RET_OKAY );
}

static void FreeHoleArray( void )
{
    if( HoleArray != NULL ) {
        bdiff_free( HoleArray );
        HoleArray = NULL;
    }
}

static int HoleCompare( const void *_h1, const void *_h2 )
{
    const save_hole *h1 = _h1;
    const save_hole *h2 = _h2;

    if( h1->offset < h2->offset )
        return( -1 );
    if( h1->offset > h2->offset )
        return( 1 );
    return( 0 );
}

static void FlushHoles( void )
{
    save_hole       *end;
    save_hole       *curr;
    hole            tmp;

    if( NumHoles != 0 ) {
        qsort( HoleArray, NumHoles, sizeof( save_hole ), HoleCompare );
        end = HoleArray + NumHoles;
        for( curr = HoleArray; curr < end; ++curr ) {
            Input( &NewFile, &tmp, curr->offset, sizeof(hole) );
            tmp += curr->diff;
            Output( &NewFile, &tmp, curr->offset, sizeof( hole ) );
        }
        NumHoles = 0;
    }
}

static void AddHole( foff offset, foff diff )
{
    if( NumHoles == HoleArraySize ) {
        FlushHoles();
    }
    HoleArray[NumHoles].offset = offset;
    HoleArray[NumHoles].diff = diff;
    NumHoles++;
}

#else

#define InitHoles()
#define AddHole( off, diff ) OutNew( (off), InNew( (off) ) + (diff), hole );
#define FlushHoles()
#define FreeHoleArray()

#endif

void GetLevel( const char *name )
{
    int         io;
    char        buffer[sizeof( PATCH_LEVEL )];

    CurrLevel[0] = '\0';
    io = open( name, O_BINARY+O_RDONLY );
    if( io == -1 )
        return;
    if( lseek( io, -(long)sizeof( PATCH_LEVEL ), SEEK_END ) != -1L &&
        read( io, buffer, sizeof( PATCH_LEVEL ) ) == sizeof( PATCH_LEVEL ) &&
        memcmp( buffer, PATCH_LEVEL, PATCH_LEVEL_HEAD_SIZE ) == 0 ) {
        strcpy( CurrLevel, buffer + PATCH_LEVEL_HEAD_SIZE );
    }
    close( io );
}

void FileCheck( int fd, const char *name )
{
    if( fd == -1 ) {
        FilePatchError( ERR_CANT_OPEN, name );
    }
}

void SeekCheck( long pos, const char *name )
{
    if( pos == -1 ) {
        FilePatchError( ERR_IO_ERROR, name );
    }
}

static PATCH_RET_CODE InitPatch( char **target_given )
{
    char            *p;
    bool            compare_sig;
    char            target[FILENAME_MAX];
    char            ch;
    char            *temp;
    PATCH_RET_CODE  ret;
#ifdef BDIFF
    byte            *tmp;
#else
    byte            tmp[4];
#endif
    ret = OpenPatch();
    if( ret != PATCH_RET_OKAY ) {
        return( ret );
    }
    p = PATCH_SIGNATURE;
    compare_sig = true;
    while( (ch = InPatch( char )) != EOF_CHAR ) {
        if( compare_sig ) {
            if( ch != *p ) {
                PatchError( ERR_NOT_PATCHFILE, PatchName );
                return( PATCH_RET_OKAY );
            }
            ++p;
            if( ch == END_SIG_CHAR ) {
                compare_sig = false;
            }
        }
    }
    p = target;
    while( (*p++ = InPatch( char )) != '\0' )
        ;
    if( (*target_given) != NULL ) {
        temp = SetOld( (*target_given) );
    } else {
        temp = FindOld( target );
    }
    if( temp ) {
        *target_given = temp;
        return( PATCH_RET_OKAY );
    } else {
        *target_given = NULL;
        ClosePatch();
        return( PATCH_CANT_OPEN_FILE );
    }
}

#ifdef BDIFF

PATCH_RET_CODE Execute( byte *dest )
{
    byte            *tmp;

#else

PATCH_RET_CODE Execute( void )
{
    byte            tmp[4];

#endif

    patch_cmd       cmd;
    byte            next;
    hole            diff;
    foff            size;
    foff            incr;
    foff            iters;
    foff            old_size;
    foff            new_size;
    foff            checksum;
    foff            new_offset;
    foff            old_offset;
    char            ch;
    bool            havenew;
    PATCH_RET_CODE  ret;
    PATCH_RET_CODE  ret2;
#ifdef BDIFF
    char            *dummy = NULL;
#endif

    havenew = true;
#ifdef BDIFF
    InitPatch( &dummy );
#endif
    old_size = InPatch( foff );
    new_size = InPatch( foff );
    checksum = InPatch( foff );
    ret = OpenOld( old_size, DOPROMPT, new_size, checksum );
    if( ret != PATCH_RET_OKAY )
        goto error1;
    ret = OpenNew( new_size );
    if( ret != PATCH_RET_OKAY )
        goto error2;
    InitHoles();
    for( ;; ) {
#if defined( INSTALL_PROGRAM )
    #if defined( __NT__ ) || defined( __WINDOWS__ ) || defined( __OS2__ )
        if( PatchStatusCancelled() ) {
            ret = PATCH_RET_CANCEL;
            goto error3;
        }
    #endif
#endif
        cmd = InPatch( patch_cmd );
        if( cmd == CMD_DONE )
            break;
        switch( cmd ) {
        case CMD_DIFFS:
            new_offset = InPatch( foff );
            size = InPatch( foff );
            Dump(( "Different  new-%8.8lx size-%8.8lx\n", new_offset, size ));
            while( size != 0 ) {
                OutNew( new_offset, InPatch( byte ), byte );
                ++new_offset;
                --size;
            }
            break;
        case CMD_SAMES:
            new_offset = InPatch( foff );
            old_offset = InPatch( foff );
            size = InPatch( foff );
            Dump(( "Similar    new-%8.8lx old-%8.8lx size-%8.8lx\n",
                   new_offset, old_offset, size));
            while( size != 0 ) {
                OutNew( new_offset, InOld( old_offset ), byte );
                ++new_offset;
                ++old_offset;
                --size;
            }
            break;
        case CMD_ITER_HOLES:
            new_offset = InPatch( foff );
            diff = InPatch( hole );
            iters = InPatch( foff );
            incr = InPatch( foff );
            ch = InPatch( byte );
            Dump(( "IterHole   new-%8.8lx diff-%8.8lx iter-%8.8lx inc-%8.8lx\n",
                   new_offset, diff, iters, incr ));
            while( iters != 0 ) {
                AddHole( new_offset, diff );
                new_offset += incr;
                --iters;
            }
            break;
        case CMD_HOLES:
            new_offset = InPatch( foff );
            diff = InPatch( hole );
            for( ;; ) {
                Dump(( "Hole       new-%8.8lx diff-%8.8lx\n",new_offset,diff));
                AddHole( new_offset, diff );
                next = InPatch( byte );
                if( next == 0 )
                    break;
                if( ( next & 0x80 ) == 0  ) {
                    new_offset += (foff)next & 0x7f;
                } else if( ( next & 0x40 ) == 0 ) {
                    new_offset += ( (foff)next & 0x3f ) << 8;
                    new_offset += (foff)InPatch( byte );
                } else {
                    new_offset += ( (foff)next & 0x3f ) << 16;
                    new_offset += (foff)InPatch(byte) << 8;
                    new_offset += (foff)InPatch(byte);
                }
            }
            break;
        default:
            PatchError( ERR_BAD_PATCHFILE, PatchName );
            ret = PATCH_BAD_PATCH_FILE;
            goto error3;
        }
    }
    ret = PATCH_RET_OKAY;
    FlushHoles();
error3:
    FreeHoleArray();
    ret2 = CloseNew( new_size, checksum, &havenew );
    if( ret == PATCH_RET_OKAY ) {
        ret = ret2;
    }
error2:
    CloseOld( havenew && DOPROMPT, DOBACKUP );
error1:
    ClosePatch();
    return( ret );
}

#if !defined( BDIFF )

PATCH_RET_CODE DoPatch(
    const char  *patchname,
    bool        doprompt,
    bool        dobackup,
    bool        printlevel,
    char        *outfilename )
{
    char            buffer[sizeof( PATCH_LEVEL )];
  #ifndef _WPATCH
    char            *target = NULL;
  #endif
    PATCH_RET_CODE  ret;

    outfilename=outfilename;
    PatchName = patchname;
    DoPrompt = doprompt;
    DoBackup = dobackup;
    PrintLevel = printlevel;
    NewName = tmpnam( NULL );
  #ifndef _WPATCH
    if( access( PatchName, R_OK ) != 0 ) {
        PatchError( ERR_CANT_FIND, PatchName );
        return( PATCH_CANT_FIND_PATCH );
    }
  #endif
  #if !defined( INSTALL_PROGRAM )
    if( PrintLevel ) {
        GetLevel( PatchName );
        if( CurrLevel[0] == '\0' ) {
            Message( MSG_NOT_PATCHED, PatchName );
        } else {
            Message( MSG_PATCHED_TO_LEVEL, PatchName, CurrLevel );
        }
        return( PATCH_RET_OKAY );
    }
  #endif
    _splitpath( PatchName, NULL, NULL, NULL, buffer );
  #ifndef _WPATCH
    ret = InitPatch( &target );
  #else
    ret = InitPatch( &outfilename );
  #endif
  #if defined( INSTALL_PROGRAM )
    if( ret != PATCH_RET_OKAY ) {
        return( ret );
    }
    if( outfilename != NULL ) {
        strcpy( outfilename, target );
        PatchingFileStatusShow( PatchName, outfilename );
    }
  #endif
  #ifndef _WPATCH
    GetLevel( target );
    if( stricmp( buffer, CurrLevel ) <= 0 ) {
        ClosePatch();
    #if !defined( INSTALL_PROGRAM )
        Message( MSG_ALREADY_PATCHED, target, CurrLevel );
    #endif
        return( PATCH_ALREADY_PATCHED );
    } else {
  #endif
        ret = Execute();
        if( ret != PATCH_RET_OKAY ) {
            return( ret );
        }
  #ifndef _WPATCH
    }
  #endif
  #if !defined( INSTALL_PROGRAM ) && !defined( _WPATCH )
    Message( MSG_SUCCESSFULLY_PATCHED, target, buffer );
  #endif
    return( PATCH_RET_OKAY );
}

#endif
