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
#include <stdlib.h>
#include <string.h>
#include "wio.h"

#include "bdiff.h"
#include "wpatchio.h"
#include "wpatch.h"

//#include "exeform.h"

#include "watcom.h"
#include "symtab.h"

#include "machtype.h"
#ifdef USE_DBGINFO
    #include "dbginfo.h"
#endif

typedef enum {
    ALG_NOTHING,        /* find differences without extra info */
    ALG_ONLY_NEW,       /* only the new exe's debugging info is available */
    ALG_BOTH,           /* both exe's have debugging info available */
} algorithm;

#define BUFF_SIZE       16384
#define NL              "\r\n"
#define NL_SIZE         2

typedef struct {
    fpos_t              last_offset;
} walker_data;

typedef struct exe_blk {
    struct exe_blk      *next;
    fpos_t              start;
    uint_32             length;
    uint_16             mod_offset;
} exe_blk;

typedef struct exe_mod {
    struct exe_blk      *blocks;
    unsigned_16         mod_offset;
    char                name[1];
} exe_mod;

#if 0
typedef struct exe_info {
    int                 fd;
    symbol_table        mods_by_offset;
    symbol_table        mods_by_name;
    symbol_table        blks;
    exe_form_t          form;
    struct {
        int             fd;
        fpos_t          start;          /* start of debugging info */
    } sym;
} exe_info;
#endif

char drive[_MAX_DRIVE];
char dir[_MAX_DIR];
char fname[_MAX_FNAME];
char ext[_MAX_EXT];

char buff[BUFF_SIZE];

byte *OldSymName;
byte *NewSymName;

//exe_info old;
//exe_info new;

byte *PatchFile;
foff    PatchSize;
byte *OldFile;
byte *NewFile;
int     AppendPatchLevel;
char    *SyncString = NULL;

extern void     Execute( byte * );
extern void     GetMsg( char *, int );
extern void     FileCheck( int fd, char *name );
extern void     SeekCheck( long pos, char *name );
extern void     MsgPrintf( int resourceid, va_list arglist );

#define MAX_DIFF        (1L<<17)
#define MIN_DIFF        (1L<<7)
#define MIN_EQUALITY    (1L<<6)

#if MIN_EQUALITY > MIN_DIFF / 2
    #error "No dice buddy. Try again!"
#endif

typedef struct region {
    struct region       *next;
    foff                old_start;
    foff                new_start;
    foff                size;
    hole                diff;
    int                 dependants;
} region;

static region   *SimilarRegions;
static region   *DiffRegions;
static region   *HoleRegions;
static region  *HoleArray;
static foff     SimilarSize;
static foff     NumHoles;
static foff     NumDiffs;
static foff     DiffSize;
static foff     HolesInRegion;
static foff     NumSimilarities;
static foff     HoleCount[3];
static foff     HoleHeaders;

static byte *OldCurr;
static byte *NewCurr;
static byte *TestOld;
static byte *TestNew;
static byte *SaveOld;
static byte *SaveNew;
static foff EndOld;
static int  OldCorrection;
static foff EndNew;
static int  NewCorrection;
static byte *CurrPatch;
static int Verbose;
static char *NewName;
static char *CommentFile;
static char LevelBuff[64];

static foff SyncOld = (foff)-1;
static foff SyncNew = (foff)-1;

/* Forward declarations */
void SortHoleArray( void );

/*
 * Utility routines
 * ================
 */

static void Err( int format, va_list args )
{
    char        msgbuf[80];

    GetMsg( msgbuf, MSG_ERROR );
    printf( msgbuf );
    MsgPrintf( format, args);
}

void PatchError( int format, ... )
{
    va_list     args;

    va_start( args, format );
    Err( format, args );
    printf( "\n" );
    va_end( args );
    MsgFini();
    exit( EXIT_FAILURE );
}

void FilePatchError( int format, ... )
{
    va_list     args;
    int         err;

    va_start( args, format );
    err = errno;
    Err( format, args );
    printf( ": %s\n", strerror( err ) );
    va_end( args );
    MsgFini();
    exit( EXIT_FAILURE );
}

void stats( char *format, ... )
{
    va_list     arg;

    if( Verbose ) {
        va_start( arg, format );
        vprintf( format, arg );
        fflush( stdout );
        va_end( arg );
    }
}


void NotNull( void *p, char *str )
{
    if( p == NULL ) {
        PatchError( ERR_NO_MEMORY, str );
    }
}


void Usage( char *name )
{
    char msgbuf[80];
    int i;

    i = MSG_USAGE_FIRST;
    GetMsg( msgbuf, i );
    printf( msgbuf, name );
    for( i = i + 1; i <= MSG_USAGE_LAST; i++ ) {
        GetMsg( msgbuf, i );
        if( msgbuf[0] == 0 ) break;
        puts( msgbuf );
    }
    MsgFini();
    exit( EXIT_FAILURE );
}

void *ReadIn( char *name, foff buff_size, foff read_size )
{
    int         fd;
    void        *buff;

    buff = _allocate( buff_size );
    NotNull( buff, "file buffer" );
    fd = open( name, O_RDONLY+O_BINARY, 0 );
    FileCheck( fd, name );
    if( read( fd, buff, read_size ) != read_size ) {
        FilePatchError( ERR_CANT_READ, name );
    }
    close( fd );
    return( buff );
}


foff FileSize( char *name, int *correction )
{
    foff        size;
    int         fd;
    char        buff[ sizeof( PATCH_LEVEL ) ];

    if( access( name, R_OK ) != 0 ) {
        PatchError( ERR_CANT_FIND, name );
    } else {
        fd = open( name, O_RDONLY+O_BINARY, 0 );
        FileCheck( fd, name );
        size = lseek( fd, 0, SEEK_END );
        SeekCheck( size, name );
        *correction = 0;
        if( size > sizeof( PATCH_LEVEL ) ) {
            SeekCheck( lseek( fd, -(long)sizeof( PATCH_LEVEL ), SEEK_END ), name );
            if( read( fd, buff, sizeof( PATCH_LEVEL ) ) != sizeof( PATCH_LEVEL ) ) {
                FilePatchError( ERR_CANT_READ, name );
            }
            if( memcmp( buff, PATCH_LEVEL, PATCH_LEVEL_HEAD_SIZE ) == 0 ) {
                size -= sizeof( PATCH_LEVEL ); /* lie about size */
                *correction = sizeof( PATCH_LEVEL );
            }
        }
        close( fd );
    }
    return( size );
}


/*
 * Routines to create "regions" and add them to their lists
 */

void AddRegion( region **owner, foff old_start, foff new_start, foff size )
{
    region      *reg;

    reg = _allocate( sizeof( region ) );
    NotNull( reg, "region" );
    reg->next = *owner;
    reg->old_start = old_start;
    reg->new_start = new_start;
    reg->size = size;
    *owner = reg;
}

void AddSimilar( foff old_start, foff new_start, foff size )
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

void AddDiff( foff new_start, foff size )
{
    if( size == 0 ) return;
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
    AddRegion( &DiffRegions, -1, new_start, size );
}


void AddSimilarDiff( foff old_start, foff new_start, foff size )
{
    if( SimilarRegions != NULL ) {
        if( old_start+size < EndOld && new_start+size < EndNew ) {
            if( SimilarRegions->new_start >= ( new_start + size ) ) {
                AddDiff( new_start + size,
                     SimilarRegions->new_start - ( new_start + size ) );
            }
        }
    }
    AddSimilar( old_start, new_start, size );
}



void AddHole( foff old_start, foff new_start )
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

int TheSameIgnoringHoles( void )
{
    int         i;
    byte        *end_old;
    byte        *end_new;

    /* sync two file pointers up to an identical byte */

    for( i = 0; ; ++i ) {
        if( i == sizeof(hole)+1 ) {
            return( 0 );
        }
        if( *TestOld == *TestNew ) {
            break;
        }
        ++TestOld;
        ++TestNew;
    }

    /* find out if TestOld and TestNew are similar for length MIN_EQUALITY */

    end_old = TestOld + MIN_EQUALITY;
    end_new = TestNew + MIN_EQUALITY;
    for(;;) {
        if( TestOld >= end_old ) return( 1 );
        if( TestNew >= end_new ) return( 1 );
        if( *TestOld != *TestNew ) {
            if( TestOld[sizeof(hole)] != TestNew[sizeof(hole)] ) {
                if( TestOld[2*sizeof(hole)] != TestNew[2*sizeof(hole)] ) {
                    if( TestOld[3*sizeof(hole)] != TestNew[3*sizeof(hole)] ) {
                        return( 0 );
                    }
                    TestOld += sizeof(hole);
                    TestNew += sizeof(hole);
                }
                TestOld += sizeof(hole);
                TestNew += sizeof(hole);
            }
            TestOld += sizeof(hole)-1;
            TestNew += sizeof(hole)-1;
        }
        ++TestOld;
        ++TestNew;
    }
}

int AreasAreSimilar( foff_diff adjust, foff_diff backup_amt )
{
    byte *lastold;
    byte *lastnew;

    /*
     * Find out if there is a similar area (the same ignoring holes)
     * if we move OldCurr and NewCurr back by "backup_amt", adjusting
     * NewCurr by adjust (the shift between the two areas)
     */

#define AddAndCheck( size ) if( TestOld + size >= OldCurr ) break; \
                            if( TestNew + size >= NewCurr ) break; \
                            TestOld += size;    \
                            TestNew += size;


    /* check boundary conditions */

    if( OldCurr < OldFile + backup_amt ) return( 0 );
    if( NewCurr < NewFile + backup_amt ) return( 0 );
    TestOld = OldCurr - backup_amt;
    TestNew = NewCurr - backup_amt;
    TestNew -= adjust;
    if( TestOld < OldFile || TestOld >= OldCurr ) return( 0 );
    if( TestNew < NewFile || TestNew >= NewCurr ) return( 0 );
    if( TheSameIgnoringHoles() ) {

        if( TestOld >= OldCurr ) return( 0 );
        if( TestNew >= NewCurr ) return( 0 );

        lastold = TestOld;
        lastnew = TestNew;

        /* Move forward as long as two files are similar */

        for(;;) {
            if( *TestOld == *TestNew ) {
                lastold = TestOld;
                lastnew = TestNew;
                AddAndCheck( 1 );
                continue;
            }
            if( TestOld[sizeof(hole)] == TestNew[sizeof(hole)] ) {
                AddAndCheck( sizeof( hole ) );
                continue;
            }
            if( TestOld[2*sizeof(hole)] == TestNew[2*sizeof(hole)] ) {
                AddAndCheck( 2*sizeof( hole ) );
                continue;
            }
            if( TestOld[3*sizeof(hole)] == TestNew[3*sizeof(hole)] ) {
                AddAndCheck( 3*sizeof( hole ) );
                continue;
            }
            break;
        }

        OldCurr = lastold;
        NewCurr = lastnew;

        return( 1 );
    }
    return( 0 );
}


static void CheckSyncPoint( void )
{
    if( SyncOld == (foff)-1 ) return;
    if( SaveOld >= OldFile+SyncOld && OldCurr < OldFile+SyncOld ||
        SaveNew >= NewFile+SyncNew && NewCurr < NewFile+SyncNew ) {
    OldCurr = OldFile+SyncOld;
    NewCurr = NewFile+SyncNew;
        stats( "\r\nJumped SyncPoint\r\n" );
    }
}

int ReSync( void )
{

    /* Try to resync the two file pointer by finding similar areas */

    foff_diff   i;
    foff_diff   backup;
    unsigned    j;
    char        *spin = "-\\|/";

    SaveOld = OldCurr; SaveNew = NewCurr;
    j = 0;
    for( backup = MIN_DIFF; backup <= MAX_DIFF; backup += backup ) {
        if( backup > OldCurr-OldFile || backup > NewCurr-NewFile ) return( 0 );
        stats( "\rResync %8.8lx:%8.8lx %c ", OldCurr-OldFile, NewCurr-NewFile, spin[j] );
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


int TryBackingUp( int backup )
{
    int         i;

    if( OldCurr-backup < OldFile ) return( 0 );
    if( NewCurr-backup < NewFile ) return( 0 );
    for( i = 0; ; ++i ) {
        if( i == sizeof( hole ) ) {
            return( 0 );
        }
        if( *(OldCurr-backup-i) == *(NewCurr-backup-i) ) break;
    }
    ++OldCurr;
    ++NewCurr;
    while( backup >= sizeof( hole ) ) {
        OldCurr -= sizeof( hole );
        NewCurr -= sizeof( hole );
        AddHole( OldCurr - OldFile, NewCurr - NewFile );
        backup -= sizeof( hole );
    }
    if( backup != 0 ) {
        OldCurr -= backup;
        NewCurr -= backup;
        AddDiff( NewCurr - NewFile, backup );
    }
    return( 1 );
}


void FindRegions( void )
{
    /*
     * classify the differences between the two files into regions;
     * similar, different, and hole
     */

    byte        *old_hi;

    OldCurr = OldFile + EndOld + 1;
    NewCurr = NewFile + EndNew + 1;
    AddSimilarDiff( EndOld + 1, EndNew + 1, 0 );
    OldCurr = OldFile + (EndOld-1);
    NewCurr = NewFile + (EndNew-1);
    old_hi = OldCurr;
    for(;;) {
        if( *OldCurr != *NewCurr ) {
            if( !TryBackingUp(sizeof(hole))
             && !TryBackingUp(2*sizeof(hole))
             && !TryBackingUp(3*sizeof(hole)) ) {
                ++OldCurr;
                ++NewCurr;
                AddSimilarDiff(OldCurr-OldFile, NewCurr-NewFile,
                           (old_hi-OldCurr)+1);
                if( !ReSync() ) break;
                stats( "%8.8lx:%8.8lx\n", OldCurr-OldFile, NewCurr-NewFile );
                old_hi = OldCurr;
                continue;
            }
        }
        if( OldCurr == OldFile || NewCurr == NewFile ) {
            AddSimilarDiff( OldCurr - OldFile,
                        NewCurr - NewFile, (old_hi-OldCurr) + 1 );
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

/***************************************************************
  Routines that use debugging information to guide the analysis
 ***************************************************************/

void printd( char *p )
{
    int i;

    for( i = *(p++); i != 0; --i ) {
        stats( "%c", *(p++) );
    }
}

void fatal( int p )
{
    char msgbuf[80];

    GetMsg( msgbuf, p );
    puts( msgbuf );
    MsgFini();
    exit( EXIT_FAILURE );
}

void IOError( char *file )
{
    char msgbuf[80];

    GetMsg( msgbuf, MSG_IO_ERROR );
    printf( msgbuf, file );
    MsgFini();
    exit( EXIT_FAILURE );
}

void xseek( int fd, fpos_t pos, int how )
{
    if( lseek( fd, pos, how ) < 0 ) {
        fatal( MSG_IO_ERR );
    }
}

void xread( int fd, void *p, size_t size )
{
    if( read( fd, p, size ) != size ) {
        fatal( MSG_IO_ERR );
    }
}

int cmp_blk( exe_blk *b1, exe_blk *b2 )
{
    if( b1->start == b2->start ) {
        return( 0 );
    } else if( b1->start < b2->start ) {
        return( -1 );
    }
    return( 1 );
}

int cmp_mod_offset( exe_mod *m1, exe_mod *m2 )
{
    if( m1->mod_offset == m2->mod_offset ) {
        return( 0 );
    } else if( m1->mod_offset < m2->mod_offset ) {
        return( -1 );
    }
    return( 1 );
}

int cmp_mod_name( exe_mod *m1, exe_mod *m2 )
{
    int comp;

    if( m1->name[0] == m2->name[0] ) {
        return( memcmp( &(m1->name[1]), &(m2->name[1]), m2->name[0] ) );
    } else if( m1->name[0] < m2->name[0] ) {
        comp = memcmp( &(m1->name[1]), &(m2->name[1]), m1->name[0] );
        if( comp == 0 ) {
            comp = -1;
        }
    } else {
        comp = memcmp( &(m1->name[1]), &(m2->name[1]), m2->name[0] );
        if( comp == 0 ) {
            comp = 1;
        }
    }
    return( comp );
}

int TestBlock( foff old_off, foff new_off, foff len )
{
    byte *pold;
    byte *pnew;
    byte *o;
    byte *n;
    byte *old_stop;
    byte *new_stop;
    foff matches;
    foff holes;

    pold = &OldFile[ old_off ];
    pnew = &NewFile[ new_off ];
    matches = 0;
    holes = 0;
    o = pold;
    n = pnew;
    old_stop = &pold[ len ];
    new_stop = &pnew[ len ];
    for(;;) {
        if( o >= old_stop || n >= new_stop ) break;
        if( *o == *n ) {
            ++o;
            ++n;
            ++matches;
            continue;
        }
        o += sizeof( hole );
        n += sizeof( hole );
        if( o >= old_stop || n >= new_stop ) break;
        if( *o == *n ) {
            ++holes;
        }
    }
    if( matches + holes * sizeof( hole ) > len / 2 ) {
        AddSimilar( old_off, new_off, len );
        o = pold;
        n = pnew;
        for(;;) {
            if( o >= old_stop || n >= new_stop ) break;
            if( *o == *n ) {
                ++o;
                ++n;
                continue;
            }
            o += sizeof( hole );
            n += sizeof( hole );
            if( o >= old_stop || n >= new_stop ) {
                n -= sizeof( hole );
                AddDiff( n - NewFile, new_stop - n );
                break;
            }
            AddHole( (o-sizeof(hole)) - OldFile, (n-sizeof(hole)) - NewFile );
        }
        return( 1 );
    }
    return( 0 );
}

int FindBlockInOld( foff new_off, foff len )
{
    byte *pold;
    byte *pnew;
    byte *o;
    byte *n;
    byte *old_stop;
    byte *new_stop;

    pnew = &NewFile[ new_off ];
    new_stop = &pnew[ len ];
    pold = &OldFile[ 0 ];
    old_stop = &OldFile[ EndOld ];
    for(;;) {
        if( pold >= old_stop ) break;
        o = memchr( pold, *pnew, old_stop - pold );
        if( o == NULL ) break;
        pold = o + 1;
        n = pnew;
        for(;;) {
            if( o >= old_stop || n >= new_stop ) {
                if( TestBlock( ( pold - 1 ) - OldFile, new_off, len ) ) {
                    return( 1 );
                }
                break;
            }
            if( *o == *n ) {
                ++o;
                ++n;
                continue;
            }
            o += sizeof( hole );
            n += sizeof( hole );
            if( o >= old_stop || n >= new_stop ) {
                if( TestBlock( ( pold - 1 ) - OldFile, new_off, len ) ) {
                    return( 1 );
                }
                break;
            }
            if( *o != *n ) break;
        }
    }
    AddDiff( new_off, len );
    return( 0 );
}

#ifdef USE_DBGINFO
int both_walker( exe_blk *new_blk, void *parm )
{
    fpos_t len;
    walker_data *last;
    exe_blk *old_blk;
    exe_mod *old_mod;
    exe_mod *new_mod;
    auto exe_mod tmp_mod;

    last = parm;
    tmp_mod.mod_offset = new_blk->mod_offset;
    new_mod = SymFind( new.mods_by_offset, &tmp_mod );
    if( new_mod == NULL ) {
        fatal( MSG_DEBUG_INFO );
    }
    if( last->last_offset != new_blk->start ) {
        len = new_blk->start - last->last_offset;
        stats( "[%lx-%lx) ???",last->last_offset, last->last_offset + len );
        if( FindBlockInOld( last->last_offset, len ) ) {
            stats( "\n" );
        } else {
            stats( " (NOT found)\n" );
        }
    }
    last->last_offset = new_blk->start + new_blk->length;
    stats( "[%lx-%lx) ", new_blk->start, last->last_offset );
    printd( new_mod->name );
    if( new_blk->start >= EndNew ) {
        stats( " (BSS block ignored)\n" );
        return( 0 );
    }
    old_mod = SymFind( old.mods_by_name, new_mod );
    if( old_mod == NULL ) {
        stats( " (NOT found)\n" );
        AddDiff( new_blk->start, new_blk->length );
        return( 0 );
    }
    /* find an exact match in length if possible */
    for( old_blk = old_mod->blocks; old_blk; old_blk = old_blk->next ) {
        if( old_blk->length == -1 ) {
            continue;
        }
        if( old_blk->length == new_blk->length ) {
            if( TestBlock( old_blk->start, new_blk->start, old_blk->length ) ) {
                old_blk->length = -1;   /* don't test this one again */
                stats( "\n" );
                return( 0 );
            }
        }
    }
    for( old_blk = old_mod->blocks; old_blk; old_blk = old_blk->next ) {
        if( old_blk->length == -1 || old_blk->length == new_blk->length ) {
            continue;
        }
        if( old_blk->length > new_blk->length ) {
            if( TestBlock( old_blk->start, new_blk->start, new_blk->length ) ) {
                AddDiff( new_blk->start + new_blk->length,
                         old_blk->length - new_blk->length );
                old_blk->length = -1;   /* don't test this one again */
                stats( "\n" );
                return( 0 );
            }
        } else {        /* old_blk->length < new_blk->length */
            if( TestBlock( old_blk->start, new_blk->start, old_blk->length ) ) {
                AddDiff( new_blk->start + old_blk->length,
                         new_blk->length - old_blk->length );
                old_blk->length = -1;   /* don't test this one again */
                stats( "\n" );
                return( 0 );
            }
        }
    }
    AddDiff( new_blk->start, new_blk->length );
    stats( " (changed)\n" );
    return( 0 );
}

int only_new_walker( exe_blk *new_blk, void *parm )
{
    fpos_t len;
    walker_data *last;
    exe_mod *new_mod;
    auto exe_mod tmp_mod;

    last = parm;
    tmp_mod.mod_offset = new_blk->mod_offset;
    new_mod = SymFind( new.mods_by_offset, &tmp_mod );
    if( new_mod == NULL ) {
        fatal( MSG_DEBUG_INFO );
    }
    if( last->last_offset != new_blk->start ) {
        len = new_blk->start - last->last_offset;
        stats( "[%lx-%lx) ???",last->last_offset, last->last_offset + len );
        if( FindBlockInOld( last->last_offset, len ) ) {
            stats( "\n" );
        } else {
            stats( " (NOT found)\n" );
        }
    }
    last->last_offset = new_blk->start + new_blk->length;
    stats( "[%lx-%lx) ", new_blk->start, last->last_offset );
    printd( new_mod->name );
    if( new_blk->start >= EndNew ) {
        stats( " (BSS block ignored)\n" );
        return( 0 );
    }
    if( FindBlockInOld( new_blk->start, new_blk->length ) ) {
        stats( "\n" );
    } else {
        stats( " (NOT found)\n" );
    }
    return( 0 );
}

fpos_t ExeOverlayAccess( exe_form_t exe, uint_16 section, uint_16 *seg )
{
    exe = exe;
    return( section - *seg );
}

//#ifdef USE_DBGINFO
void ProcessExe( char *name, char *sym_name, exe_info *exe )
{
    unsigned num_blks;
    exe_mod *new_mod;
    exe_mod *found_mod;
    exe_blk *new_blk;
    fpos_t first_section;
    fpos_t mod_list;
    fpos_t addr_list;
    fpos_t curr_offset;
    fpos_t debug_header;
    addr48_ptr seg_addr;
    auto master_dbg_header dbg_head;
    auto section_dbg_header section_head;
    auto seg_info seg_desc;
    auto mod_info mod_name;
    auto addr_info seg_chunk;
    auto exe_mod tmp_mod;
    char msgbuf[80];

    _splitpath( name, drive, dir, fname, ext );
    if( ext[0] == '\0' ) {
        strcpy( ext, ".exe" );
    }
    _makepath( buff, drive, dir, fname, ext );
    exe->fd = open( buff, O_RDONLY|O_BINARY );
    if( exe->fd == -1 ) {
        IOError( buff );
    }
    _splitpath( sym_name, drive, dir, fname, ext );
    if( ext[0] == '\0' ) {
        strcpy( ext, ".sym" );
    }
    _makepath( buff, drive, dir, fname, ext );
    exe->sym.fd = open( buff, O_RDONLY|O_BINARY );
    if( exe->sym.fd == -1 ) {
        IOError( buff );
    }
    xseek( exe->sym.fd, - sizeof( dbg_head ), SEEK_END );
    debug_header = lseek( exe->sym.fd, 0, SEEK_CUR );
    xread( exe->sym.fd, &dbg_head, sizeof( dbg_head ) );
    if( dbg_head.signature != VALID_SIGNATURE           ||
        dbg_head.exe_major_ver != EXE_MAJOR_VERSION     ||
        dbg_head.exe_minor_ver > EXE_MINOR_VERSION      ||
        dbg_head.obj_major_ver != OBJ_MAJOR_VERSION     ||
        dbg_head.obj_minor_ver > OBJ_MINOR_VERSION      ) {
        fatal( MSG_DEBUG_INFO );
    }
    exe->sym.start = lseek( exe->sym.fd, 0, SEEK_END ) - dbg_head.debug_size;
    /* get segment positions in executable */
    first_section = exe->sym.start + dbg_head.lang_size + dbg_head.segment_size;
    xseek( exe->sym.fd, first_section, SEEK_SET );
    xread( exe->sym.fd, &section_head, sizeof( section_head ) );
    if( section_head.mod_offset >= section_head.gbl_offset      ||
        section_head.gbl_offset >= section_head.addr_offset     ||
        section_head.addr_offset >= section_head.section_size ) {
        fatal( MSG_DEBUG_INFO );
    }
    exe->mods_by_offset = SymInit( cmp_mod_offset );
    exe->mods_by_name = SymInit( cmp_mod_name );
    mod_list = first_section + section_head.mod_offset;
    xseek( exe->sym.fd, mod_list, SEEK_SET );
    curr_offset = section_head.mod_offset;
    while( curr_offset != section_head.gbl_offset ) {
        xread( exe->sym.fd, &mod_name, sizeof( mod_name ) );
        new_mod = _allocate( sizeof( exe_mod ) + mod_name.name[0] );
        if( new_mod == NULL ) {
            GetMsg( msgbuf, ERR_MEMORY_OUT );
            puts( msgbuf );
        MsgFini();
            exit( EXIT_FAILURE );
        }
        new_mod->blocks = NULL;
        new_mod->mod_offset = curr_offset - section_head.mod_offset;
        new_mod->name[0] = mod_name.name[0];
        xread( exe->sym.fd, &new_mod->name[1], mod_name.name[0] );
        SymAdd( exe->mods_by_offset, new_mod );
        SymAdd( exe->mods_by_name, new_mod );
        curr_offset += sizeof( mod_name ) + mod_name.name[0];
    }
    if( exe == &new ) {
        new.blks = SymInit( cmp_blk );
    }
    if( first_section + section_head.section_size != debug_header ) {
        fatal( MSG_OVERLAYS );
    }
    exe->form = ExeForm( exe->fd, 0, exe );
    addr_list = first_section + section_head.addr_offset;
    xseek( exe->sym.fd, addr_list, SEEK_SET );
    curr_offset = section_head.addr_offset;
    while( curr_offset != section_head.section_size ) {
        xread( exe->sym.fd, &seg_desc, sizeof(seg_info) - sizeof(addr_info) );
        curr_offset += sizeof( seg_info ) - sizeof( addr_info );
        num_blks = seg_desc.num;
        seg_addr = seg_desc.base;
        while( num_blks != 0 ) {
            xread( exe->sym.fd, &seg_chunk, sizeof( seg_chunk ) );
            curr_offset += sizeof( seg_chunk );
            tmp_mod.mod_offset = seg_chunk.mod;
            found_mod = SymFind( exe->mods_by_offset, &tmp_mod );
            if( found_mod == NULL ) {
                fatal( MSG_DEBUG_INFO );
            }
            new_blk = _allocate( sizeof( *new_blk ) );
            if( new_blk == NULL ) {
                GetMsg( msgbuf, ERR_MEMORY_OUT );
                puts( msgbuf );
        MsgFini();
                exit( EXIT_FAILURE );
            }
            new_blk->next = found_mod->blocks;
            found_mod->blocks = new_blk;
            new_blk->start = ExeTransformAddr( exe->form,
                                seg_addr.segment, seg_addr.offset,
                                section_head.section_id );
            seg_addr.offset += seg_chunk.size;
            new_blk->length = seg_chunk.size;
            new_blk->mod_offset = found_mod->mod_offset;
            if( exe == &new ) {
                SymAdd( new.blks, new_blk );
            }
            --num_blks;
        }
    }
}

void SymbolicDiff( algorithm alg, char *old_exe, char *new_exe )
{
    auto walker_data data;

    data.last_offset = 0;
    ProcessExe( new_exe, NewSymName, &new );
    if( alg == ALG_BOTH ) {
        ProcessExe( old_exe, OldSymName, &old );
        SymWalk( new.blks, &data, both_walker );
        close( old.fd );
        close( old.sym.fd );
    } else {
        SymWalk( new.blks, &data, only_new_walker );
    }
    close( new.fd );
    close( new.sym.fd );
}
#endif


void VerifyCorrect( char *name )
{

    /* Try the patch file and ensure it produces new from old */

    char        *real_new;
    foff        offset;

    memset( NewFile, 0x00, EndNew );
    Execute( NewFile );
    _free( OldFile );
    real_new = ReadIn( name, EndNew, EndNew );
    if( real_new != NULL ) {
        if( memcmp( real_new, NewFile, EndNew ) != 0 ) {
            offset = 0;
            for( ;; ) {
                if( *real_new != *NewFile ) {
                    PatchError( ERR_PATCH_BUNGLED, offset, *real_new, *NewFile );
                }
                ++offset;
                if( offset >= EndNew ) break;
                ++real_new;
                ++NewFile;
            }
        }
        _free( real_new );
    }
}

int HoleCompare( const void *_h1, const void *_h2 )
{
    const region    *h1 = _h1;
    const region    *h2 = _h2;

    if( h1->diff < h2->diff ) return( -1 );
    if( h1->diff > h2->diff ) return( 1 );
    if( h1->new_start < h2->new_start ) return( -1 );
    if( h1->new_start > h2->new_start ) return( 1 );
    return( 0 );
}


void CheckPatch( int size )
{
    byte *oldpatch;

    if( CurrPatch - PatchFile + size >= PatchSize ) {
    oldpatch = PatchFile;
    PatchSize += 10*1024;
    PatchFile = _reallocate( PatchFile, PatchSize );
    NotNull( PatchFile, "patch file" );
    CurrPatch = PatchFile + ( CurrPatch - oldpatch );
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

int OutVar( foff value, int really )
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


void OutStr( char *str )
{
    for( ;; ) {
        if( *str == '\0' ) break;
        OutPatch( *str, char );
        ++str;
    }
}


#define MIN_ITERS (sizeof(patch_cmd)+sizeof(hole)+sizeof(foff)+sizeof(foff))

int FOffCompare( const void *_h1, const void *_h2 )
{
    const region    *h1 = _h1;
    const region    *h2 = _h2;

    if( h1->new_start < h2->new_start ) return( -1 );
    if( h1->new_start > h2->new_start ) return( 1 );
    return( 0 );
}

#define RUN_SIZE 5

long HolesToDiffs( void ) 
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

            for( ;; ) {
                if( curr >= end ) break;
                if( next->old_start - curr->old_start
                 != next->new_start - curr->new_start ) break;
                if( next->old_start - curr->old_start <= RUN_SIZE ) break;
                ++next;
                ++curr;
            }
            if( curr >= end ) break;

            /* extend this run while holes are the same small distance apart */

            run_start = curr;
            for( ;; ) {
                ++next;
                ++curr;
                if( curr >= end ) break;
                if( next->old_start - curr->old_start
                 != next->new_start - curr->new_start ) break;
                if( next->old_start - curr->old_start > RUN_SIZE ) break;
            }
            if( curr >= end ) break;

            /* convert to a difference region if it would make patch smaller */

            hole_size = 0;
            diff_size = 2*sizeof(foff)+sizeof(patch_cmd)
            + curr->new_start - run_start->new_start + sizeof(hole);
            for( run = run_start; run <= curr; ++run ) {
                if( run->dependants == 1 ) hole_size += run->size;
            }
            if( hole_size > diff_size ) {
                savings += hole_size - diff_size;
                AddRegion( &DiffRegions, run_start->old_start,
                           run_start->new_start,
                           curr->new_start-run_start->new_start+sizeof(hole) );
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


void ProcessHoleArray( int write_holes )
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
    int         first;
    foff        iters;
    foff        incr;
    int         size;

    if( NumHoles != 0 ) {
        end = HoleArray + NumHoles - 1;
        first = 1;
        for( curr = HoleArray; curr <= end; ++curr ) {
            diff = curr->diff;
            prev = curr;
            curr->dependants = 1;
            incr = 0;
            iters = 1;
            for( end_iters = curr + 1; end_iters <= end; ++end_iters ) {
                if( diff != end_iters->diff ) break;
                if( incr != 0
                 && incr != end_iters->new_start - prev->new_start ) break;
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
                first = 0;
                curr = end_iters - 1;
                curr_start = curr->new_start;
                curr_diff = 0;
                curr_header = curr;
            } else if( first || diff != curr_diff ) {
                if( write_holes ) {
                    if( !first ) {
                        OutPatch( 0, byte );
                    }
                    first = 0;
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
                    HoleCount[ size ]++;
                } else {
                    curr->size = size;
                    curr_header->dependants++;
                }
                curr_start = curr->new_start;
            }
        }
        if( write_holes ) OutPatch( 0, byte );
    }
}

void WriteSimilars( void ) 
{
    /* write similar regions out to the patch file */

    region      *curr;
    region      *junk;

    for( curr = SimilarRegions; curr != NULL; ) {
        OutPatch( CMD_SAMES, patch_cmd );
        OutPatch( curr->new_start, foff );
        OutPatch( curr->old_start, foff );
        OutPatch( curr->size, foff );
        junk = curr;
        curr = curr->next;
        _free( junk );
    }
}

void WriteDiffs( void )
{
    /* write difference regions out to the patch file */

    region      *curr;
    region      *junk;
    foff        curr_start;
    foff        size;

    for( curr = DiffRegions; curr != NULL; ) {
        curr_start = curr->new_start;
        size = curr->size;
        OutPatch( CMD_DIFFS, patch_cmd );
        OutPatch( curr_start, foff );
        OutPatch( size, foff );
        while( size != 0 ) {
            OutPatch( NewFile[ curr_start ], byte );
            --size;
            ++curr_start;
        }
        junk = curr;
        curr = curr->next;
        _free( junk );
    }
}


void AddLevel( char *name )
{
    memcpy( LevelBuff, PATCH_LEVEL, sizeof( PATCH_LEVEL ) );
    _splitpath( name, NULL, NULL, NULL, LevelBuff+PATCH_LEVEL_HEAD_SIZE );
    memcpy( NewFile + EndNew, LevelBuff, sizeof( PATCH_LEVEL ) );
}


void WriteLevel( void )
{
    char *buff;
    int         size;

    OutPatch( CMD_DIFFS, patch_cmd );
    OutPatch( EndNew, foff );
    OutPatch( sizeof( PATCH_LEVEL ), foff );
    size = sizeof( PATCH_LEVEL );
    buff = LevelBuff;
    while( size != 0 ) {
        OutPatch( *buff, char );
        --size;
        ++buff;
    }
}

foff Sum( void )
{
    foff        sum;
    foff        i;
    foff        end;

    sum = 0;
    end = EndNew;
    if( AppendPatchLevel ) end += sizeof( PATCH_LEVEL );
    for( i = 0; i != end; ++i ) {
        sum += NewFile[ i ];
    }
    return( sum );
}


void CopyComment( void )
{
    int         fd;
    foff        size;
    char        *comment;

    if( CommentFile != NULL ) {
        fd = open( CommentFile, O_RDONLY | O_BINARY, 0 );
        FileCheck( fd, CommentFile );
        size = lseek( fd, 0, SEEK_END );
        SeekCheck( size, CommentFile );
        SeekCheck( lseek( fd, 0, SEEK_SET ), CommentFile );
        comment = _allocate( size+1 );
        NotNull( comment, "comment file" );
        if( read( fd, comment, size ) != size ) {
            FilePatchError( ERR_CANT_READ, CommentFile );
        }
        close( fd );
        comment[ size ] = '\0';
        OutStr( comment );
    }
}

void WritePatchFile( char *name )
{

    foff        size;

    PatchSize = EndNew;
    PatchFile = _allocate( PatchSize );
    NotNull( PatchFile, "patch file" );
    CurrPatch = PatchFile;

    if( AppendPatchLevel ) AddLevel( name );

    OutStr( PATCH_SIGNATURE );
    CopyComment();

    OutPatch( EOF_CHAR, byte );
    OutStr( NewName );
    OutPatch( '\0', char );
    OutPatch( EndOld + OldCorrection, foff );
    size = EndNew;
    if( AppendPatchLevel ) size += sizeof( PATCH_LEVEL );
    OutPatch( size, foff );
    OutPatch( Sum(), foff );

    WriteSimilars();
    WriteDiffs();
    if( AppendPatchLevel ) WriteLevel();
    ProcessHoleArray( 1 );

    OutPatch( CMD_DONE, byte );

    size = CurrPatch - PatchFile;
    PatchWrite( PatchFile, size );
}


void MakeHoleArray( void )
{
    region      *reg;
    region      *new_hole;
    region      *curr;

    if( NumHoles == 0 ) return;
    HoleArray = _allocate( sizeof( region ) * NumHoles );
    NotNull( HoleArray, "sorted holes" );
    new_hole = HoleArray;
    for( reg = HoleRegions; reg != NULL; ) {
        curr = reg;
        reg = reg->next;
        *new_hole = *curr;
        _free( curr );
        ++new_hole;
    }
    HoleRegions = reg;
}

void SortHoleArray( void )
{
    qsort( HoleArray, NumHoles, sizeof( region ), HoleCompare );
}


void FreeHoleArray( void )
{
    if( NumHoles != 0 ) {
        _free( HoleArray );
    }
}

foff FindSyncString( byte *file, foff end )
{
    foff    i;
    int     len;

    len = strlen( SyncString );
    end -= len;
    for( i = 0; i < end; ++i ) {
    if( file[i] != SyncString[0] ) continue;
    if( memcmp( SyncString, file+i, len ) != 0 ) continue;
    return( i );
    }
    return( -1 );
}

void ScanSyncString( void )
{
    if( SyncString == NULL ) return;
    SyncOld = FindSyncString( OldFile, EndOld );
    SyncNew = FindSyncString( NewFile, EndNew );
    if( SyncOld == (foff)-1 || SyncNew == (foff)-1 ) {
    fatal( ERR_NO_SYNCSTRING );
    }
    while( OldFile[SyncOld] == NewFile[SyncNew] ) {
    ++SyncOld; ++SyncNew;
    if( SyncOld == EndOld ) break;
    if( SyncNew == EndNew ) break;
    }
}


int DoBdiff( char *srcPath, char *tgtPath, char *name )
{
    long       savings;
    foff        buffsize;
    foff        best_from_new;
    int         i;
//    if( !MsgInit() ) exit( EXIT_FAILURE );
    /* initialize static variables each time */
    SimilarRegions = NULL;
    DiffRegions = NULL;
    HoleRegions = NULL;
    SimilarSize = 0;
    NumHoles = 0;
    NumDiffs = 0;
    DiffSize = 0;
    HolesInRegion = 0;
    NumSimilarities = 0;
    HoleHeaders = 0;
    for ( i = 0; i < 3; i += 1 ) {
        HoleCount[ i ] = 0;
    }

    NewName = name;
    EndOld = FileSize( srcPath, &OldCorrection );
    EndNew = FileSize( tgtPath, &NewCorrection );

    buffsize = ( EndOld > EndNew ) ? ( EndOld ) : ( EndNew );
    buffsize += sizeof( PATCH_LEVEL );
    OldFile = ReadIn( srcPath, buffsize, EndOld );
    NewFile = ReadIn( tgtPath, buffsize, EndNew );

    ScanSyncString();

    FindRegions();

    if( NumHoles == 0 && DiffSize == 0 && EndOld == EndNew ) {
        printf( "Patch file not created - files are identical\n" );
    MsgFini();
        exit( EXIT_SUCCESS );
    }
    MakeHoleArray();
    SortHoleArray();
    ProcessHoleArray( 0 );
    savings = HolesToDiffs();
    WritePatchFile( "" );
    FreeHoleArray();
    VerifyCorrect( tgtPath );

    stats( "similar regions:    %8lu bytes (%lu chunks)\n",
            SimilarSize, NumSimilarities);
    stats( "different regions:  %8lu bytes (%lu chunks)\n",
            DiffSize, NumDiffs );
    stats( "hole->diff savings: %8lu bytes\n", savings );
    stats( "number of holes:    %8lu\n", NumHoles );
    stats( "(%lu headers + %lu single + %lu double + %lu triple)\n\n",
           HoleHeaders, HoleCount[0], HoleCount[1], HoleCount[2] );
    stats( "old file: %8lu bytes   new file: %8lu bytes\n", EndOld, EndNew );
    stats( "%lu%% of old executable referenced in patch file (largest amount is 100%%)\n",
           (SimilarSize*100) / EndOld );
    best_from_new = 0;
    if( EndNew > EndOld ) {
        best_from_new = EndNew - EndOld;
    }
    stats( "%lu%% of new executable output to patch file (least amount is %lu%%)\n",
           (DiffSize*100) / EndNew, (best_from_new*100) / EndNew );
    stats( "%lu total patch file size (%lu%%)\n", CurrPatch - PatchFile,
           ( ( CurrPatch - PatchFile ) * 100 ) / EndNew );

    MsgFini();
    return ( EXIT_SUCCESS );
}

/*
 * For debugging purposes only (In debugger issue "CALL DUMP" command)
 */

void dump( void )
{
    region  *reg;

    printf( "        Similarities\n"
            "        ============\n" );
    printf( "O Offset  N Offset      Size\n" );
    for( reg = SimilarRegions; reg; reg = reg->next ) {
        printf( "%8lx  %8lx  %8lx\n",reg->old_start,reg->new_start,reg->size);
    }
    printf( "    Differences\n"
            "    ===========\n" );
    printf( "N Offset      Size\n" );
    for( reg = DiffRegions; reg; reg = reg->next ) {
        printf( "%8lx  %8lx\n",reg->new_start,reg->size);
    }
    printf( "           Holes\n"
            "           =====\n" );
    printf( "O Offset  N Offset  Difference\n" );
    for( reg = HoleRegions; reg; reg = reg->next ) {
        printf( "%8lx  %8lx  %8lx\n",reg->old_start,reg->new_start,reg->diff);
    }
}
