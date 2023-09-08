/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Mainline for binary diff utility.
*
****************************************************************************/


#include "bdiff.h"
#include "msg.h"
#include "symtab.h"
#include "diff.h"
#include "wdbginfo.h"
#include "pathgrp2.h"
#include "wpatch.h"

#include "clibext.h"


#define BUFF_SIZE       16384

#define MAX_DIFF        (1L<<17)
#define MIN_DIFF        (1L<<7)
#define MIN_EQUALITY    (1L<<6)

#if MIN_EQUALITY > MIN_DIFF / 2
    #error "No dice buddy. Try again!"
#endif

typedef struct exe_blk {
    struct exe_blk      *next;
    fpos_t              start;
    uint_32             length;
    uint_16             mod_offset;
} exe_blk;

typedef struct exe_mod {
    struct exe_blk      *blocks;
    uint_16             mod_offset;
    char                name[1];
} exe_mod;

byte            *PatchBuffer;
byte            *OldFile;

bool            AppendPatchLevel;
bool            Verbose;

char            *OldSymName;
char            *NewSymName;

char            *CommentFile;

static foff            DiffSize = 0;
static foff            NumHoles = 0;

static const char   *SyncString = NULL;

static int          OldCorrection;
static int          NewCorrection;
static foff         EndOld;
static foff         EndNew;
static byte         *NewFile;

static region       *SimilarRegions = NULL;
static region       *DiffRegions = NULL;
static region       *HoleRegions = NULL;
static region       *HoleArray;

static byte         *OldCurr;
static byte         *NewCurr;
static byte         *TestOld;
static byte         *TestNew;
static byte         *SaveOld;
static byte         *SaveNew;

static foff         HolesInRegion = 0;
static char         LevelBuff[64];

static foff SyncOld = (foff)-1;
static foff SyncNew = (foff)-1;

static foff         SimilarSize = 0;
static foff         NumSimilarities = 0;
static foff         HoleCount[3] = { 0, 0, 0 };
static foff         HoleHeaders = 0;
static foff         NumDiffs = 0;

static byte         *CurrPatch;

static foff         PatchSize;

/*
 * Utility routines
 * ================
 */

static void stats( const char *format, ... )
{
    va_list     args;

    if( Verbose ) {
        va_start( args, format );
        vprintf( format, args );
        fflush( stdout );
        va_end( args );
    }
}


static bool NotNull( void *p, char *str )
{
    bool    ok;

    ok = ( p != NULL );
    if( !ok ) {
        PatchError( ERR_NO_MEMORY, str );
    }
    return( ok );
}

static void *ReadIn( const char *name, size_t buff_size, size_t read_size )
{
    FILE        *fd;
    void        *buff;

    buff = bdiff_malloc( buff_size );
    if( NotNull( buff, "file buffer" ) ) {
        fd = fopen( name, "rb" );
        if( FileCheck( fd, name ) ) {
            if( fread( buff, 1, read_size, fd ) != read_size ) {
                FilePatchError( ERR_CANT_READ, name );
                bdiff_free( buff );
                buff = NULL;
            }
            fclose( fd );
            return( buff );
        }
        bdiff_free( buff );
    }
    return( NULL );
}


static foff FileSize( const char *name, int *correction )
{
    unsigned long   size;
    FILE            *fd;
    char            buff[sizeof( PATCH_LEVEL )];

    size = 0;
    if( access( name, R_OK ) != 0 ) {
        PatchError( ERR_CANT_FIND, name );
    } else {
        fd = fopen( name, "rb" );
        if( FileCheck( fd, name ) ) {
            if( SeekCheck( fseek( fd, 0, SEEK_END ), name ) ) {
                size = ftell( fd );
                *correction = 0;
                if( size > sizeof( PATCH_LEVEL ) ) {
                    if( SeekCheck( fseek( fd, -(long)sizeof( PATCH_LEVEL ), SEEK_END ), name ) ) {
                        if( fread( buff, 1, sizeof( PATCH_LEVEL ), fd ) != sizeof( PATCH_LEVEL ) ) {
                            FilePatchError( ERR_CANT_READ, name );
                            size = 0;
                        } else {
                            if( memcmp( buff, PATCH_LEVEL, PATCH_LEVEL_HEAD_SIZE ) == 0 ) {
                                size -= sizeof( PATCH_LEVEL ); /* lie about size */
                                *correction = sizeof( PATCH_LEVEL );
                            }
                        }
                    }
                }
            }
            fclose( fd );
        }
    }
    return( size );
}


/*
 * Routines to create "regions" and add them to their lists
 */

static void AddRegion( region **owner, foff old_start, foff new_start, foff size )
{
    region      *reg;

    reg = bdiff_malloc( sizeof( region ) );
    if( NotNull( reg, "region" ) ) {
        reg->next = *owner;
        reg->old_start = old_start;
        reg->new_start = new_start;
        reg->size = size;
        *owner = reg;
    }
}

static void AddSimilar( foff old_start, foff new_start, foff size )
{
    SimilarSize += size;
    if( SimilarRegions ) {
        foff last_old;
        foff last_new;

        last_old = SimilarRegions->old_start + SimilarRegions->size;
        last_new = SimilarRegions->new_start + SimilarRegions->size;
        if( last_old == old_start && last_new == new_start ) {
            SimilarRegions->size += size;
            return;
        }
    }
    HolesInRegion = 0;
    NumSimilarities++;
    AddRegion( &SimilarRegions, old_start, new_start, size );
}

static void AddDiff( foff new_start, foff size )
{
    if( size == 0 )
        return;
    DiffSize += size;
    if( DiffRegions ) {
        foff last_new;

        last_new = DiffRegions->new_start + DiffRegions->size;
        if( last_new == new_start ) {
            DiffRegions->size += size;
            return;
        }
    }
    NumDiffs++;
    AddRegion( &DiffRegions, (foff)-1, new_start, size );
}


static void AddSimilarDiff( foff old_start, foff new_start, foff size )
{
    if( SimilarRegions != NULL ) {
        if( old_start + size < EndOld || new_start + size < EndNew ) {
            if( SimilarRegions->new_start >= ( new_start + size ) ) {
                AddDiff( new_start + size,
                     SimilarRegions->new_start - ( new_start + size ) );
            }
        }
    }
    AddSimilar( old_start, new_start, size );
}


static void AddHole( foff old_start, foff new_start )
{
    NumHoles++;
    HolesInRegion++;
    AddRegion( &HoleRegions, old_start, new_start, sizeof( hole ) );
    HoleRegions->diff = *(hole*)( NewFile + HoleRegions->new_start )
                      - *(hole*)( OldFile + HoleRegions->old_start );
}

/*
 * =====================================================================
 * Routines to compare the files ignoring "holes". A hole is a 1-4 byte
 * difference in the two files. These are handled specially in the patch
 * file, since these are the changed relocation items caused by a shift
 * in the code within the executable.
 * =====================================================================
 */

#define _Check()               (*TestOld == *TestNew)
#define _CheckHole( count )    (TestOld[count * sizeof( hole )] == TestNew[count * sizeof( hole )])

#define _Next()                 ++TestOld;++TestNew
#define _NextHole( count )      TestOld += count * sizeof( hole );TestNew += count * sizeof( hole )

static int TheSameIgnoringHoles( void )
{
    int         i;
    byte        *end_old;
    byte        *end_new;

    /* sync two file pointers up to an identical byte */

    for( i = 0; i < sizeof( hole ) + 1; ++i ) {
        if( _Check() ) {
            /* find out if TestOld and TestNew are similar for length MIN_EQUALITY */
            end_old = TestOld + MIN_EQUALITY;
            end_new = TestNew + MIN_EQUALITY;
            while( TestOld < end_old && TestNew < end_new ) {
                if( _Check() ) {
                    _Next();
                } else if( _CheckHole( 1 ) ) {
                    _NextHole( 1 );
                } else if( _CheckHole( 2 ) ) {
                    _NextHole( 2 );
                } else if( _CheckHole( 3 ) ) {
                    _NextHole( 3 );
                } else {
                    return( 0 );
                }
            }
            return( 1 );
        }
        _Next();
    }
    return( 0 );
}

static int AreasAreSimilar( foff_diff adjust, foff_diff backup_amt )
{
    byte *lastold;
    byte *lastnew;

    /*
     * Find out if there is a similar area (the same ignoring holes)
     * if we move OldCurr and NewCurr back by "backup_amt", adjusting
     * NewCurr by adjust (the shift between the two areas)
     */

#define _AddAndCheck() \
    if( TestOld + 1 >= OldCurr ) break; \
    if( TestNew + 1 >= NewCurr ) break; \
    _Next()

#define _AddAndCheckHole( count ) \
    if( TestOld + count * sizeof( hole ) >= OldCurr ) break; \
    if( TestNew + count * sizeof( hole ) >= NewCurr ) break; \
    _NextHole( count )


    /* check boundary conditions */

    if( OldCurr < OldFile + backup_amt )
        return( 0 );
    if( NewCurr < NewFile + backup_amt )
        return( 0 );
    TestOld = OldCurr - backup_amt;
    TestNew = NewCurr - backup_amt;
    TestNew -= adjust;
    if( TestOld < OldFile || TestOld >= OldCurr )
        return( 0 );
    if( TestNew < NewFile || TestNew >= NewCurr )
        return( 0 );
    if( TheSameIgnoringHoles() ) {
        if( TestOld >= OldCurr )
            return( 0 );
        if( TestNew >= NewCurr )
            return( 0 );

        lastold = TestOld;
        lastnew = TestNew;

        /* Move forward as long as two files are similar */

        for( ;; ) {
            if( _Check() ) {
                lastold = TestOld;
                lastnew = TestNew;
                _AddAndCheck();
            } else if( _CheckHole( 1 ) ) {
                _AddAndCheckHole( 1 );
            } else if( _CheckHole( 2 ) ) {
                _AddAndCheckHole( 2 );
            } else if( _CheckHole( 3 ) ) {
                _AddAndCheckHole( 3 );
            } else {
                break;
            }
        }

        OldCurr = lastold;
        NewCurr = lastnew;

        return( 1 );
    }
    return( 0 );
}


static void CheckSyncPoint( void )
{
    if( SyncOld == (foff)-1 )
        return;
    if( SaveOld >= OldFile + SyncOld && OldCurr < OldFile + SyncOld ||
        SaveNew >= NewFile + SyncNew && NewCurr < NewFile + SyncNew ) {
        OldCurr = OldFile + SyncOld;
        NewCurr = NewFile + SyncNew;
        stats( "\r\nJumped SyncPoint\r\n" );
    }
}

static int ReSync( void )
{

    /* Try to resync the two file pointer by finding similar areas */

    foff_diff   i;
    foff_diff   backup;
    int         j;
    char        *spin = "-\\|/";

    SaveOld = OldCurr; SaveNew = NewCurr;
    j = 0;
    for( backup = MIN_DIFF; backup <= MAX_DIFF; backup += backup ) {
        if( backup > OldCurr - OldFile || backup > NewCurr - NewFile )
            return( 0 );
        stats( "\rResync %8.8lx:%8.8lx %c ", OldCurr - OldFile, NewCurr - NewFile, spin[j] );
        ++j; j &= 3;
        for( i = 0; i <= backup; ++i ) {
            if( AreasAreSimilar( i, backup ) ) {
                CheckSyncPoint();
                return( 1 );
            }
        }
        for( i = -1; i >= -backup; --i ) {
            if( AreasAreSimilar( i, backup ) ) {
                CheckSyncPoint();
                return( 1 );
            }
        }
    }
    return( 0 );
}


static int TryBackingUp( int backup )
{
    int         i;

    if( OldCurr - backup < OldFile )
        return( 0 );
    if( NewCurr - backup < NewFile )
        return( 0 );
    for( i = 0; i < sizeof( hole ); ++i ) {
        if( *(OldCurr - backup - i) == *(NewCurr - backup - i) ) {
            ++OldCurr;
            ++NewCurr;
            for( ; backup >= sizeof( hole ); backup -= sizeof( hole ) ) {
                OldCurr -= sizeof( hole );
                NewCurr -= sizeof( hole );
                AddHole( (foff)( OldCurr - OldFile ), (foff)( NewCurr - NewFile ) );
            }
            if( backup != 0 ) {
                OldCurr -= backup;
                NewCurr -= backup;
                AddDiff( (foff)( NewCurr - NewFile ), backup );
            }
            return( 1 );
        }
    }
    return( 0 );
}


static void FindRegions( void )
{
    /*
     * classify the differences between the two files into regions;
     * similar, different, and hole
     */

    byte        *old_hi;

    OldCurr = OldFile + EndOld + 1;
    NewCurr = NewFile + EndNew + 1;
    AddSimilarDiff( EndOld + 1, EndNew + 1, 0 );
    OldCurr = OldFile + ( EndOld - 1 );
    NewCurr = NewFile + ( EndNew - 1 );
    old_hi = OldCurr;
    for( ;; ) {
        if( *OldCurr != *NewCurr ) {
            if( !TryBackingUp( sizeof( hole ) )
             && !TryBackingUp( 2 * sizeof( hole ) )
             && !TryBackingUp( 3 * sizeof( hole ) ) ) {
                ++OldCurr;
                ++NewCurr;
                AddSimilarDiff( (foff)( OldCurr - OldFile ), (foff)( NewCurr - NewFile ), (foff)( old_hi - OldCurr ) + 1 );
                if( !ReSync() )
                    break;
                stats( "%8.8lx:%8.8lx\n", OldCurr - OldFile, NewCurr - NewFile );
                old_hi = OldCurr;
                continue;
            }
        }
        if( OldCurr == OldFile || NewCurr == NewFile ) {
            AddSimilarDiff( (foff)( OldCurr - OldFile ), (foff)( NewCurr - NewFile ), (foff)( old_hi - OldCurr ) + 1 );
            break;
        }
        --OldCurr;
        --NewCurr;
    }
    OldCurr = OldFile;
    NewCurr = NewFile;
    if( SimilarRegions->new_start != 0 ) {
        AddSimilarDiff( 0, 0, 0 );
    }
    putchar( '\n' );
}

static void fatal( int p )
{
    char msgbuf[MAX_RESOURCE_SIZE];

    GetMsg( msgbuf, p );
    puts( msgbuf );
}

static int VerifyCorrect( const char *name )
/*******************************************
 * Try the patch file and ensure it produces new from old
 */
{
    char            *real_new;
    foff            offset;
    PATCH_RET_CODE  rc;

    memset( NewFile, 0, EndNew );
    rc = Execute( NewFile );
    bdiff_free( OldFile );
    if( rc == PATCH_RET_OKAY ) {
        real_new = ReadIn( name, EndNew, EndNew );
        if( real_new != NULL ) {
            if( memcmp( real_new, NewFile, EndNew ) != 0 ) {
                offset = 0;
                for( ;; ) {
                    if( *real_new != *NewFile ) {
                        PatchError( ERR_PATCH_BUNGLED, offset, *real_new, *NewFile );
                        rc = PATCH_BAD_PATCH;
                    }
                    ++offset;
                    if( offset >= EndNew )
                        break;
                    ++real_new;
                    ++NewFile;
                }
            }
            bdiff_free( real_new );
        }
    }
    return( rc != PATCH_RET_OKAY );
}

static int HoleCompare( const void *_h1, const void *_h2 )
{
    const region *h1 = _h1;
    const region *h2 = _h2;

    if( h1->diff < h2->diff )
        return( -1 );
    if( h1->diff > h2->diff )
        return( 1 );
    if( h1->new_start < h2->new_start )
        return( -1 );
    if( h1->new_start > h2->new_start )
        return( 1 );
    return( 0 );
}


static void CheckPatch( int size )
{
    byte *oldpatch;

    if( CurrPatch - PatchBuffer + size >= PatchSize ) {
        oldpatch = PatchBuffer;
        PatchSize += 10 * 1024;
        PatchBuffer = bdiff_realloc( PatchBuffer, PatchSize );
        if( NotNull( PatchBuffer, "patch file" ) ) {
            CurrPatch = PatchBuffer + ( CurrPatch - oldpatch );
        }
    }
}

#define OutPatch( val, type ) {CheckPatch( sizeof(type) );*(type*)CurrPatch=(val);CurrPatch+=sizeof(type);}

/*
 * Output the offset to the next hole with the same difference value.
 * as follows:
 * 0xxxxxxx                     - value xxxxxxx                 ( 0 .. 127 )
 * 10xxxxxx yyyyyyyy            - value xxxxxxyyyyyyyy          ( 0 .. 16383 )
 * 11xxxxxx yyyyyyyy zzzzzzzz   - value xxxxxxyyyyyyyyzzzzzzzz  ( 0 .. 2**30-1 )
 */

static int OutVar( foff value, int really )
{
    if( value <= 0x7f ) {
        if( really ) {
            OutPatch( value & 0x7f, byte );     /* top bit off */
        }
        return( 0 );
    } else if( value <= 0x3fff ) {      /* top bit on, next bit off */
        if( really ) {
            OutPatch( 0x80 | ( ( value >> 8 ) & 0x3f ), byte );
            OutPatch( value & 0xff, byte );
        }
        return( 1 );
    } else {                            /* top two on */
        if( really ) {
            OutPatch( 0xC0 | ( ( value >> 16 ) & 0x3f ), byte );
            OutPatch( ( value >> 8 ) & 0xff, byte );
            OutPatch( value & 0xff, byte );
        }
        return( 2 );
    }
}


static void OutStr( const char *str )
{
    for( ; *str != '\0'; ++str ) {
        OutPatch( *str, char );
    }
}


#define MIN_ITERS (sizeof( patch_cmd ) + sizeof( hole ) + sizeof( foff ) + sizeof( foff ))

static int FOffCompare( const void *_h1, const void *_h2 )
{
    const region *h1 = _h1;
    const region *h2 = _h2;

    if( h1->new_start < h2->new_start )
        return( -1 );
    if( h1->new_start > h2->new_start )
        return( 1 );
    return( 0 );
}

static void SortHoleArray( void )
{
    qsort( HoleArray, NumHoles, sizeof( region ), HoleCompare );
}

#define RUN_SIZE 5

static long HolesToDiffs( void )
{
    /* Find runs of holes which would be cheaper to represent as differences */

    region      *curr;
    region      *next;
    region      *end;
    region      *run_start;
    region      *run;
    long        hole_size;
    long        diff_size;
    long        dead_holes;
    long        savings;

    savings = 0;
    if( NumHoles != 0 ) {

        /* sort by file offset */

        qsort( HoleArray, NumHoles, sizeof( region ), FOffCompare );

        end = HoleArray + NumHoles - 1;
        next = HoleArray+1;
        curr = HoleArray;
        dead_holes = 0;
        savings = 0;

        /* going through the array in adjacent pairs, (curr, next) ... */

        for( ;; ) {

            /* find two holes which are the same small amount apart */

            for( ; curr < end; ++curr ) {
                if( next->old_start - curr->old_start != next->new_start - curr->new_start )
                    break;
                if( next->old_start - curr->old_start <= RUN_SIZE )
                    break;
                ++next;
            }
            if( curr >= end )
                break;

            /* extend this run while holes are the same small distance apart */

            run_start = curr;
            for( ;; ) {
                ++next;
                ++curr;
                if( curr >= end )
                    break;
                if( next->old_start - curr->old_start != next->new_start - curr->new_start )
                    break;
                if( next->old_start - curr->old_start > RUN_SIZE ) {
                    break;
                }
            }
            if( curr >= end )
                break;

            /* convert to a difference region if it would make patch smaller */

            hole_size = 0;
            diff_size = 2 * sizeof( foff ) + sizeof( patch_cmd )
                + curr->new_start - run_start->new_start + sizeof( hole );
            for( run = run_start; run <= curr; ++run ) {
                if( run->dependants == 1 ) {
                    hole_size += run->size;
                }
            }
            if( hole_size > diff_size ) {
                savings += hole_size - diff_size;
                AddRegion( &DiffRegions, run_start->old_start,
                           run_start->new_start,
                           curr->new_start - run_start->new_start + sizeof( hole ) );
                for( run = run_start; run <= curr; ++run ) {
                    ++dead_holes;
                    run->diff = 0;
                    run->new_start = 0;
                }
                ++curr;
                next = curr+1;
            }
        }

        /* sort by difference amount. This moves dead holes to the front */

        SortHoleArray();

        /* Get rid of dead holes (Those converted to difference regions) */

        NumHoles -= dead_holes;
        memmove( HoleArray, HoleArray+dead_holes, NumHoles * sizeof( region ) );
    }
    return( savings );
}


static void ProcessHoleArray( int write_holes )
{

    /* write holes out to the patch file, or just calculate file size needed */

    region      *curr;
    region      *curr_header;
    region      *end_iters;
    region      *prev;
    region      *end;
    hole        curr_diff;
    hole        diff;
    foff        curr_start;
    bool        first;
    foff        iters;
    foff        incr;
    int         size;

    curr_start = 0;
    curr_diff = 0;
    curr_header = NULL;
    if( NumHoles != 0 ) {
        end = HoleArray + NumHoles - 1;
        first = true;
        for( curr = HoleArray; curr <= end; ++curr ) {
            diff = curr->diff;
            prev = curr;
            curr->dependants = 1;
            incr = 0;
            iters = 1;
            for( end_iters = curr + 1; end_iters <= end; ++end_iters ) {
                if( diff != end_iters->diff )
                    break;
                if( incr != 0 && incr != end_iters->new_start - prev->new_start )
                    break;
                incr = end_iters->new_start - prev->new_start;
                ++iters;
                prev = end_iters;
            }
            if( iters > MIN_ITERS ) {
                if( write_holes ) {
                    if( !first ) {
                        OutPatch( 0, byte );
                    }
                    HoleHeaders++;
                    OutPatch( CMD_ITER_HOLES, patch_cmd );
                    OutPatch( curr->new_start, foff );
                    OutPatch( diff, hole );
                    OutPatch( iters, foff );
                    OutPatch( incr, foff );
                }
                first = false;
                curr = end_iters - 1;
                curr_start = curr->new_start;
                curr_diff = 0;
                curr_header = curr;
            } else if( first || diff != curr_diff ) {
                if( write_holes ) {
                    if( !first ) {
                        OutPatch( 0, byte );
                    }
                    first = false;
                    HoleHeaders++;
                    OutPatch( CMD_HOLES, patch_cmd );
                    OutPatch( curr->new_start, foff );
                    OutPatch( diff, hole );
                }
                curr_start = curr->new_start;
                curr_diff = diff;
                curr->size = sizeof( hole ) + sizeof( foff )
                           + sizeof( patch_cmd ) + sizeof( byte );
                curr_header = curr;
            } else {
                size = OutVar( curr->new_start - curr_start, write_holes );
                if( write_holes ) {
                    HoleCount[size]++;
                } else {
                    curr->size = size;
                    curr_header->dependants++;
                }
                curr_start = curr->new_start;
            }
        }
        if( write_holes ) {
            OutPatch( 0, byte );
        }
    }
}

static void WriteSimilars( void )
{
    /* write similar regions out to the patch file */

    region      *curr;
    region      *next;

    for( curr = SimilarRegions; curr != NULL; curr = next ) {
        next = curr->next;
        OutPatch( CMD_SAMES, patch_cmd );
        OutPatch( curr->new_start, foff );
        OutPatch( curr->old_start, foff );
        OutPatch( curr->size, foff );
        bdiff_free( curr );
    }
}

static void WriteDiffs( void )
{
    /* write difference regions out to the patch file */

    region      *curr;
    region      *next;
    foff        curr_start;
    foff        size;

    for( curr = DiffRegions; curr != NULL; curr = next ) {
        next = curr->next;
        curr_start = curr->new_start;
        size = curr->size;
        OutPatch( CMD_DIFFS, patch_cmd );
        OutPatch( curr_start, foff );
        OutPatch( size, foff );
        for( ; size != 0; --size ) {
            OutPatch( NewFile[curr_start++], byte );
        }
        bdiff_free( curr );
    }
}


static void AddLevel( const char *name )
{
    pgroup2     pg;

    memcpy( LevelBuff, PATCH_LEVEL, sizeof( PATCH_LEVEL ) );
    _splitpath2( name, pg.buffer, NULL, NULL, NULL, &pg.ext );
    strcpy( LevelBuff + PATCH_LEVEL_HEAD_SIZE, pg.ext );
    memcpy( NewFile + EndNew, LevelBuff, sizeof( PATCH_LEVEL ) );
}


static void WriteLevel( void )
{
    char        *buff;
    int         size;

    OutPatch( CMD_DIFFS, patch_cmd );
    OutPatch( EndNew, foff );
    OutPatch( sizeof( PATCH_LEVEL ), foff );
    buff = LevelBuff;
    for( size = sizeof( PATCH_LEVEL ); size != 0; --size ) {
        OutPatch( *buff++, char );
    }
}

static foff Sum( void )
{
    foff        sum;
    foff        i;
    foff        end;

    sum = 0;
    end = EndNew;
    if( AppendPatchLevel )
        end += sizeof( PATCH_LEVEL );
    for( i = 0; i != end; ++i ) {
        sum += NewFile[i];
    }
    return( sum );
}


static void CopyComment( void )
{
    FILE            *fd;
    unsigned long   size;
    char            *comment;

    if( CommentFile != NULL ) {
        fd = fopen( CommentFile, "rb" );
        if( FileCheck( fd, CommentFile ) ) {
            if( SeekCheck( fseek( fd, 0, SEEK_END ), CommentFile ) ) {
                size = ftell( fd );
                if( SeekCheck( fseek( fd, 0, SEEK_SET ), CommentFile ) ) {
                    comment = bdiff_malloc( size + 1 );
                    if( NotNull( comment, "comment file" ) ) {
                        if( fread( comment, 1, size, fd ) != size ) {
                            FilePatchError( ERR_CANT_READ, CommentFile );
                        } else {
                            comment[size] = '\0';
                            OutStr( comment );
                        }
                        bdiff_free( comment );
                    }
                }
            }
            fclose( fd );
        }
    }
}

static void WritePatchFile( const char *name, const char *new_name )
{
    foff        len;
    size_t      size;
    FILE        *fd;

    PatchSize = 0;
    CurrPatch = NULL;
    PatchBuffer = bdiff_malloc( EndNew );
    if( NotNull( PatchBuffer, "patch file" ) ) {
        PatchSize = EndNew;
        CurrPatch = PatchBuffer;

        if( AppendPatchLevel )
            AddLevel( name );

        OutStr( PATCH_SIGNATURE );
        CopyComment();

        OutPatch( EOF_CHAR, byte );
        OutStr( new_name );
        OutPatch( '\0', char );
        OutPatch( EndOld + OldCorrection, foff );
        len = EndNew;
        if( AppendPatchLevel )
            len += sizeof( PATCH_LEVEL );
        OutPatch( len, foff );
        OutPatch( Sum(), foff );

        WriteSimilars();
        WriteDiffs();
        if( AppendPatchLevel )
            WriteLevel();
        ProcessHoleArray( 1 );

        OutPatch( CMD_DONE, byte );

        PatchSize = CurrPatch - PatchBuffer;
        fd = fopen( name, "wb" );
        if( FileCheck( fd, name ) ) {
            size = fwrite( PatchBuffer, 1, PatchSize, fd );
            if( PatchSize != size ) {
                FilePatchError( ERR_CANT_WRITE, name );
                PatchSize = size;
            }
            fclose( fd );
        }
        bdiff_free( PatchBuffer );
        PatchBuffer = NULL;
    }
}


static void MakeHoleArray( void )
{
    region      *new_hole;
    region      *curr;

    if( NumHoles == 0 )
        return;
    HoleArray = bdiff_malloc( sizeof( region ) * NumHoles );
    if( NotNull( HoleArray, "sorted holes" ) ) {
        new_hole = HoleArray;
        while( (curr = HoleRegions) != NULL ) {
            HoleRegions = curr->next;
            *new_hole++ = *curr;
            bdiff_free( curr );
        }
    }
}

static void FreeHoleArray( void )
{
    if( NumHoles != 0 ) {
        bdiff_free( HoleArray );
    }
}

static foff FindSyncString( byte *file, foff end, const char *syncString )
{
    foff        i;
    foff        len;

    len = (foff)strlen( syncString );
    end -= len;
    for( i = 0; i < end; ++i ) {
        if( file[i] != (byte)syncString[0] )
            continue;
        if( memcmp( syncString, file + i, len ) != 0 )
            continue;
        return( i );
    }
    return( (foff)-1 );
}

static bool ScanSyncString( const char *syncString )
{
    if( syncString != NULL ) {
        SyncOld = FindSyncString( OldFile, EndOld, syncString );
        SyncNew = FindSyncString( NewFile, EndNew, syncString );
        if( SyncOld == (foff)-1 || SyncNew == (foff)-1 ) {
            fatal( ERR_NO_SYNCSTRING );
            return( false );
        }
        while( OldFile[SyncOld] == NewFile[SyncNew] ) {
            ++SyncOld; ++SyncNew;
            if( SyncOld == EndOld )
                break;
            if( SyncNew == EndNew ) {
                break;
            }
        }
    }
    return( true );
}

static void print_stats( long savings )
{
    foff        best_from_new;

    best_from_new = 0;
    if( EndNew > EndOld ) {
        best_from_new = EndNew - EndOld;
    }
    stats( "similar regions:    %8lu bytes (%lu chunks)\n", SimilarSize, NumSimilarities);
    stats( "different regions:  %8lu bytes (%lu chunks)\n", DiffSize, NumDiffs );
    stats( "hole->diff savings: %8lu bytes\n", savings );
    stats( "number of holes:    %8lu\n", NumHoles );
    stats( "(%lu headers + %lu single + %lu double + %lu triple)\n\n", HoleHeaders, HoleCount[0], HoleCount[1], HoleCount[2] );
    stats( "old file: %8lu bytes   new file: %8lu bytes\n", EndOld, EndNew );
    stats( "%lu%% of old executable referenced in patch file (largest amount is 100%%)\n", ( SimilarSize * 100 ) / EndOld );
    stats( "%lu%% of new executable output to patch file (least amount is %lu%%)\n", ( DiffSize * 100 ) / EndNew, ( best_from_new * 100 ) / EndNew );
    stats( "%lu total patch file size (%lu%%)\n", PatchSize, ( PatchSize * 100 ) / EndNew );
}

/*
 * For debugging purposes only (In debugger issue "CALL DUMP" command)
 */

void dump( void )
{
    region *reg;

    puts( "        Similarities" );
    puts( "        ============" );
    puts( "O Offset  N Offset      Size" );
    for( reg = SimilarRegions; reg; reg = reg->next ) {
        printf( "%8lx  %8lx  %8lx\n",reg->old_start,reg->new_start,reg->size);
    }
    puts( "    Differences" );
    puts( "    ===========" );
    puts( "N Offset      Size" );
    for( reg = DiffRegions; reg; reg = reg->next ) {
        printf( "%8lx  %8lx\n",reg->new_start,reg->size);
    }
    puts( "           Holes" );
    puts( "           =====" );
    puts( "O Offset  N Offset  Difference" );
    for( reg = HoleRegions; reg; reg = reg->next ) {
        printf( "%8lx  %8lx  %8lx\n",reg->old_start,reg->new_start,reg->diff);
    }
}

int DoBdiff( const char *srcPath, const char *tgtPath, const char *new_name, const char *name, bool init )
{
    long        savings;
    foff        buffsize;
    int         i;

    if( init ) {
        /* initialize static variables each time */
        SimilarRegions = NULL;
        DiffRegions = NULL;
        HoleRegions = NULL;
        NumHoles = 0;
        HoleHeaders = 0;
        DiffSize = 0;
        NumDiffs = 0;
        NumSimilarities = 0;
        SimilarSize = 0;
        HolesInRegion = 0;
        for( i = 0; i < 3; i += 1 ) {
            HoleCount[i] = 0;
        }
    }

    EndOld = FileSize( srcPath, &OldCorrection );
    EndNew = FileSize( tgtPath, &NewCorrection );

    buffsize = ( EndOld > EndNew ) ? ( EndOld ) : ( EndNew );
    buffsize += sizeof( PATCH_LEVEL );
    OldFile = ReadIn( srcPath, buffsize, EndOld );
    NewFile = ReadIn( tgtPath, buffsize, EndNew );

    if( ScanSyncString( SyncString ) ) {
        FindRegions();

        if( NumHoles == 0 && DiffSize == 0 && EndOld == EndNew ) {
            puts( "Patch file not created - files are identical" );
            return( 1 );
        }
        MakeHoleArray();
        SortHoleArray();
        ProcessHoleArray( 0 );
        savings = HolesToDiffs();
        WritePatchFile( name, new_name );
        FreeHoleArray();
        i = VerifyCorrect( tgtPath );
        print_stats( savings );
        return( i );
    }
    return( 1 );
}
