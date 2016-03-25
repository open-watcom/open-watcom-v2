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
* Description:  Mainline for binary diff utility.
*
****************************************************************************/


#include "bdiff.h"
#include "msg.h"
#include "symtab.h"
#include "diff.h"
#ifdef USE_DBGINFO
#include "dbginfo.h"
#endif


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

#ifdef USE_DBGINFO
typedef long            exe_form_t;

typedef struct {
    fpos_t              last_offset;
} walker_data;

typedef struct exe_info {
    FILE                *fd;
    symbol_table        mods_by_offset;
    symbol_table        mods_by_name;
    symbol_table        blks;
    exe_form_t          form;
    struct {
        FILE            *fd;
        fpos_t          start;          /* start of debugging info */
    } sym;
} exe_info;

extern foff     ExeForm( FILE *, void *, exe_info * );
extern foff     ExeTransformAddr( exe_form_t, addr_seg, addr48_off, word );
extern fpos_t   ExeOverlayAccess( exe_form_t exe, uint_16 section, uint_16 *seg );
#endif

foff         DiffSize = 0;
foff         EndOld;
foff         EndNew;
foff         NumHoles = 0;

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

#ifdef USE_DBGINFO
static exe_info old;
static exe_info new;

static char drive[_MAX_DRIVE];
static char dir[_MAX_DIR];
static char fname[_MAX_FNAME];
static char ext[_MAX_EXT];
#endif

/*
 * Utility routines
 * ================
 */

static void stats( const char *format, ... )
{
    va_list     arg;

    if( Verbose ) {
        va_start( arg, format );
        vprintf( format, arg );
        fflush( stdout );
        va_end( arg );
    }
}


static void NotNull( void *p, char *str )
{
    if( p == NULL ) {
        PatchError( ERR_NO_MEMORY, str );
    }
}

void *ReadIn( const char *name, size_t buff_size, size_t read_size )
{
    FILE        *fd;
    void        *buff;

    buff = bdiff_malloc( buff_size );
    NotNull( buff, "file buffer" );
    fd = fopen( name, "rb" );
    FileCheck( fd, name );
    if( fread( buff, 1, read_size, fd ) != read_size ) {
        FilePatchError( ERR_CANT_READ, name );
    }
    fclose( fd );
    return( buff );
}


foff FileSize( const char *name, int *correction )
{
    unsigned long   size;
    FILE            *fd;
    char            buff[sizeof( PATCH_LEVEL )];

    size = 0;
    if( access( name, R_OK ) != 0 ) {
        PatchError( ERR_CANT_FIND, name );
    } else {
        fd = fopen( name, "rb" );
        FileCheck( fd, name );
        SeekCheck( fseek( fd, 0, SEEK_END ), name );
        size = ftell( fd );
        *correction = 0;
        if( size > sizeof( PATCH_LEVEL ) ) {
            SeekCheck( fseek( fd, -(long)sizeof( PATCH_LEVEL ), SEEK_END ), name );
            if( fread( buff, 1, sizeof( PATCH_LEVEL ), fd ) != sizeof( PATCH_LEVEL ) ) {
                FilePatchError( ERR_CANT_READ, name );
            }
            if( memcmp( buff, PATCH_LEVEL, PATCH_LEVEL_HEAD_SIZE ) == 0 ) {
                size -= sizeof( PATCH_LEVEL ); /* lie about size */
                *correction = sizeof( PATCH_LEVEL );
            }
        }
        fclose( fd );
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
    NotNull( reg, "region" );
    reg->next = *owner;
    reg->old_start = old_start;
    reg->new_start = new_start;
    reg->size = size;
    *owner = reg;
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
                AddHole( OldCurr - OldFile, NewCurr - NewFile );
            }
            if( backup != 0 ) {
                OldCurr -= backup;
                NewCurr -= backup;
                AddDiff( NewCurr - NewFile, backup );
            }
            return( 1 );
        }
    }
    return( 0 );
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
                AddSimilarDiff( OldCurr - OldFile, NewCurr - NewFile, ( old_hi - OldCurr ) + 1 );
                if( !ReSync() )
                    break;
                stats( "%8.8lx:%8.8lx\n", OldCurr - OldFile, NewCurr - NewFile );
                old_hi = OldCurr;
                continue;
            }
        }
        if( OldCurr == OldFile || NewCurr == NewFile ) {
            AddSimilarDiff( OldCurr - OldFile, NewCurr - NewFile, ( old_hi - OldCurr ) + 1 );
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

static void fatal( int p )
{
    char msgbuf[MAX_RESOURCE_SIZE];

    GetMsg( msgbuf, p );
    puts( msgbuf );
    MsgFini();
    exit( EXIT_FAILURE );
}

#ifdef USE_DBGINFO
static void ReadCheck( size_t rc, size_t size, const char *name )
{
    if( rc != size ) {
        FilePatchError( ERR_IO_ERROR, name );
    }
}

static void printd( char *p )
{
    int len;

    for( len = (byte)(*p++); len > 0; --len ) {
        stats( "%c", *p++ );
    }
}

static int cmp_blk( void *b1, void *b2 )
{
    if( ((exe_blk *)b1)->start == ((exe_blk *)b2)->start ) {
        return( 0 );
    } else if( ((exe_blk *)b1)->start < ((exe_blk *)b2)->start ) {
        return( -1 );
    }
    return( 1 );
}

static int cmp_mod_offset( void *m1, void *m2 )
{
    if( ((exe_mod *)m1)->mod_offset == ((exe_mod *)m2)->mod_offset ) {
        return( 0 );
    } else if( ((exe_mod *)m1)->mod_offset < ((exe_mod *)m2)->mod_offset ) {
        return( -1 );
    }
    return( 1 );
}

static int cmp_mod_name( void *m1, void *m2 )
{
    int comp;

    if( ((exe_mod *)m1)->name[0] == ((exe_mod *)m2)->name[0] ) {
        return( memcmp( &(((exe_mod *)m1)->name[1]), &(((exe_mod *)m2)->name[1]), (byte)((exe_mod *)m2)->name[0] ) );
    } else if( (byte)((exe_mod *)m1)->name[0] < (byte)((exe_mod *)m2)->name[0] ) {
        comp = memcmp( &(((exe_mod *)m1)->name[1]), &(((exe_mod *)m2)->name[1]), (byte)((exe_mod *)m1)->name[0] );
        if( comp == 0 ) {
            comp = -1;
        }
    } else {
        comp = memcmp( &(((exe_mod *)m1)->name[1]), &(((exe_mod *)m2)->name[1]), (byte)((exe_mod *)m2)->name[0] );
        if( comp == 0 ) {
            comp = 1;
        }
    }
    return( comp );
}

static int TestBlock( foff old_off, foff new_off, foff len )
{
    byte *pold;
    byte *pnew;
    byte *o;
    byte *n;
    byte *old_stop;
    byte *new_stop;
    foff matches;
    foff holes;

    pold = &OldFile[old_off];
    pnew = &NewFile[new_off];
    matches = 0;
    holes = 0;
    o = pold;
    n = pnew;
    old_stop = &pold[len];
    new_stop = &pnew[len];
    for( ;; ) {
        if( o >= old_stop || n >= new_stop )
            break;
        if( *o == *n ) {
            ++o;
            ++n;
            ++matches;
            continue;
        }
        o += sizeof( hole );
        n += sizeof( hole );
        if( o >= old_stop || n >= new_stop )
            break;
        if( *o == *n ) {
            ++holes;
        }
    }
    if( matches + holes * sizeof( hole ) > len / 2 ) {
        AddSimilar( old_off, new_off, len );
        o = pold;
        n = pnew;
        for( ;; ) {
            if( o >= old_stop || n >= new_stop )
                break;
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
            AddHole( ( o - sizeof( hole ) ) - OldFile, ( n - sizeof( hole ) ) - NewFile );
        }
        return( 1 );
    }
    return( 0 );
}

static int FindBlockInOld( foff new_off, foff len )
{
    byte *pold;
    byte *pnew;
    byte *o;
    byte *n;
    byte *old_stop;
    byte *new_stop;

    pnew = &NewFile[new_off];
    new_stop = &pnew[len];
    pold = &OldFile[0];
    old_stop = &OldFile[EndOld];
    for( ;; ) {
        if( pold >= old_stop )
            break;
        o = memchr( pold, *pnew, old_stop - pold );
        if( o == NULL )
            break;
        pold = o + 1;
        n = pnew;
        for( ;; ) {
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
            if( *o != *n ) {
                break;
            }
        }
    }
    AddDiff( new_off, len );
    return( 0 );
}

static int both_walker( void *_new_blk, void *parm )
{
    fpos_t len;
    walker_data *last;
    exe_blk *old_blk;
    exe_mod *old_mod;
    exe_mod *new_mod;
    auto exe_mod tmp_mod;
    exe_blk *new_blk = _new_blk;

    last = parm;
    tmp_mod.mod_offset = new_blk->mod_offset;
    new_mod = SymFind( new.mods_by_offset, &tmp_mod );
    if( new_mod == NULL ) {
        fatal( MSG_DEBUG_INFO );
    }
    if( last->last_offset != new_blk->start ) {
        len = new_blk->start - last->last_offset;
        stats( "[%lx-%lx) ???", last->last_offset, last->last_offset + len );
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
    for( old_blk = old_mod->blocks; old_blk != NULL; old_blk = old_blk->next ) {
        if( old_blk->length == (uint_32)-1 ) {
            continue;
        }
        if( old_blk->length == new_blk->length ) {
            if( TestBlock( old_blk->start, new_blk->start, old_blk->length ) ) {
                old_blk->length = (uint_32)-1;   /* don't test this one again */
                stats( "\n" );
                return( 0 );
            }
        }
    }
    for( old_blk = old_mod->blocks; old_blk != NULL; old_blk = old_blk->next ) {
        if( old_blk->length == (uint_32)-1 || old_blk->length == new_blk->length ) {
            continue;
        }
        if( old_blk->length > new_blk->length ) {
            if( TestBlock( old_blk->start, new_blk->start, new_blk->length ) ) {
                AddDiff( new_blk->start + new_blk->length,
                         old_blk->length - new_blk->length );
                old_blk->length = (uint_32)-1;   /* don't test this one again */
                stats( "\n" );
                return( 0 );
            }
        } else {        /* old_blk->length < new_blk->length */
            if( TestBlock( old_blk->start, new_blk->start, old_blk->length ) ) {
                AddDiff( new_blk->start + old_blk->length,
                         new_blk->length - old_blk->length );
                old_blk->length = (uint_32)-1;   /* don't test this one again */
                stats( "\n" );
                return( 0 );
            }
        }
    }
    AddDiff( new_blk->start, new_blk->length );
    stats( " (changed)\n" );
    return( 0 );
}

static int only_new_walker( void *_new_blk, void *parm )
{
    fpos_t len;
    walker_data *last;
    exe_mod *new_mod;
    auto exe_mod tmp_mod;
    exe_blk *new_blk = _new_blk;

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

static void ProcessExe( const char *name, char *sym_name, exe_info *exe )
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
    char file_name[_MAX_PATH];

    _splitpath( name, drive, dir, fname, ext );
    if( ext[0] == '\0' ) {
        strcpy( ext, ".exe" );
    }
    _makepath( file_name, drive, dir, fname, ext );
    exe->fd = fopen( file_name, "rb" );
    FileCheck( exe->fd, file_name );
    _splitpath( sym_name, drive, dir, fname, ext );
    if( ext[0] == '\0' ) {
        strcpy( ext, ".sym" );
    }
    _makepath( file_name, drive, dir, fname, ext );
    exe->sym.fd = fopen( file_name, "rb" );
    FileCheck( exe->sym.fd, file_name );
    SeekCheck( fseek( exe->sym.fd, -(long)sizeof( dbg_head ), SEEK_END ), file_name );
    debug_header = ftell( exe->sym.fd );
    ReadCheck( fread( &dbg_head, 1, sizeof( dbg_head ), exe->sym.fd ), sizeof( dbg_head ), file_name );
    if( dbg_head.signature != VALID_SIGNATURE           ||
        dbg_head.exe_major_ver != EXE_MAJOR_VERSION     ||
        dbg_head.exe_minor_ver > EXE_MINOR_VERSION      ||
        dbg_head.obj_major_ver != OBJ_MAJOR_VERSION     ||
        dbg_head.obj_minor_ver > OBJ_MINOR_VERSION      ) {
        fatal( MSG_DEBUG_INFO );
    }
    SeekCheck( fseek( exe->sym.fd, 0, SEEK_END ), file_name );
    exe->sym.start = ftell( exe->sym.fd ) - dbg_head.debug_size;
    /* get segment positions in executable */
    first_section = exe->sym.start + dbg_head.lang_size + dbg_head.segment_size;
    SeekCheck( fseek( exe->sym.fd, first_section, SEEK_SET ), file_name );
    ReadCheck( fread( &section_head, 1, sizeof( section_head ), exe->sym.fd ), sizeof( section_head ), file_name );
    if( section_head.mod_offset >= section_head.gbl_offset      ||
        section_head.gbl_offset >= section_head.addr_offset     ||
        section_head.addr_offset >= section_head.section_size ) {
        fatal( MSG_DEBUG_INFO );
    }
    exe->mods_by_offset = SymInit( cmp_mod_offset );
    exe->mods_by_name = SymInit( cmp_mod_name );
    mod_list = first_section + section_head.mod_offset;
    SeekCheck( fseek( exe->sym.fd, mod_list, SEEK_SET ), file_name );
    for( curr_offset = section_head.mod_offset;
         curr_offset != section_head.gbl_offset;
         curr_offset += sizeof( mod_name ) + (byte)mod_name.name[0]
        ) {
        ReadCheck( fread( &mod_name, 1, sizeof( mod_name ), exe->sym.fd ), sizeof( mod_name ), file_name );
        new_mod = bdiff_malloc( sizeof( exe_mod ) + (byte)mod_name.name[0] );
        NotNull( new_mod, "new module" );
        new_mod->blocks = NULL;
        new_mod->mod_offset = curr_offset - section_head.mod_offset;
        new_mod->name[0] = mod_name.name[0];
        ReadCheck( fread( &new_mod->name[1], 1, (byte)mod_name.name[0], exe->sym.fd ), (byte)mod_name.name[0], file_name );
        SymAdd( exe->mods_by_offset, new_mod );
        SymAdd( exe->mods_by_name, new_mod );
    }
    if( exe == &new ) {
        new.blks = SymInit( cmp_blk );
    }
    if( first_section + section_head.section_size != debug_header ) {
        fatal( MSG_OVERLAYS );
    }
    exe->form = ExeForm( exe->fd, NULL, exe );
    addr_list = first_section + section_head.addr_offset;
    SeekCheck( fseek( exe->sym.fd, addr_list, SEEK_SET ), file_name );
    for( curr_offset = section_head.addr_offset;
         curr_offset != section_head.section_size;
         curr_offset += sizeof( seg_info ) - sizeof( addr_info )
        ) {
        ReadCheck( fread( &seg_desc, 1, sizeof( seg_info ) - sizeof( addr_info ), exe->sym.fd ), sizeof( seg_info ) - sizeof( addr_info ), file_name );
        seg_addr = seg_desc.base;
        for( num_blks = seg_desc.num; num_blks != 0; --num_blks ) {
            ReadCheck( fread( &seg_chunk, 1, sizeof( seg_chunk ), exe->sym.fd ), sizeof( seg_chunk ), file_name );
            curr_offset += sizeof( seg_chunk );
            tmp_mod.mod_offset = seg_chunk.mod;
            found_mod = SymFind( exe->mods_by_offset, &tmp_mod );
            if( found_mod == NULL ) {
                fatal( MSG_DEBUG_INFO );
            }
            new_blk = bdiff_malloc( sizeof( *new_blk ) );
            NotNull( new_blk, "new block" );
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
        fclose( old.fd );
        fclose( old.sym.fd );
    } else {
        SymWalk( new.blks, &data, only_new_walker );
    }
    fclose( new.fd );
    fclose( new.sym.fd );
}
#endif


void VerifyCorrect( const char *name )
{

    /* Try the patch file and ensure it produces new from old */

    char        *real_new;
    foff        offset;

    memset( NewFile, 0x00, EndNew );
    Execute( NewFile );
    bdiff_free( OldFile );
    real_new = ReadIn( name, EndNew, EndNew );
    if( real_new != NULL ) {
        if( memcmp( real_new, NewFile, EndNew ) != 0 ) {
            offset = 0;
            for( ;; ) {
                if( *real_new != *NewFile ) {
                    PatchError( ERR_PATCH_BUNGLED, offset, *real_new, *NewFile );
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

    if( CurrPatch - PatchFile + size >= PatchSize ) {
        oldpatch = PatchFile;
        PatchSize += 10 * 1024;
        PatchFile = bdiff_realloc( PatchFile, PatchSize );
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
    memcpy( LevelBuff, PATCH_LEVEL, sizeof( PATCH_LEVEL ) );
    _splitpath( name, NULL, NULL, NULL, LevelBuff + PATCH_LEVEL_HEAD_SIZE );
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
        FileCheck( fd, CommentFile );
        SeekCheck( fseek( fd, 0, SEEK_END ), CommentFile );
        size = ftell( fd );
        SeekCheck( fseek( fd, 0, SEEK_SET ), CommentFile );
        comment = bdiff_malloc( size + 1 );
        NotNull( comment, "comment file" );
        if( fread( comment, 1, size, fd ) != size ) {
            FilePatchError( ERR_CANT_READ, CommentFile );
        }
        fclose( fd );
        comment[size] = '\0';
        OutStr( comment );
    }
}

void WritePatchFile( const char *name, const char *newName )
{
    foff        len;
    size_t      size;
    FILE        *fd;

    PatchSize = EndNew;
    PatchFile = bdiff_malloc( PatchSize );
    NotNull( PatchFile, "patch file" );
    CurrPatch = PatchFile;

    if( AppendPatchLevel )
        AddLevel( name );

    OutStr( PATCH_SIGNATURE );
    CopyComment();

    OutPatch( EOF_CHAR, byte );
    OutStr( newName );
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

    fd = fopen( name, "wb" );
    FileCheck( fd, name );
    size = CurrPatch - PatchFile;
    if( fwrite( PatchFile, 1, size, fd ) != size ) {
        FilePatchError( ERR_CANT_WRITE, name );
    }
    fclose( fd );
}


void MakeHoleArray( void )
{
    region      *new_hole;
    region      *curr;

    if( NumHoles == 0 )
        return;
    HoleArray = bdiff_malloc( sizeof( region ) * NumHoles );
    NotNull( HoleArray, "sorted holes" );
    new_hole = HoleArray;
    while( (curr = HoleRegions) != NULL ) {
        HoleRegions = curr->next;
        *new_hole++ = *curr;
        bdiff_free( curr );
    }
}

void SortHoleArray( void )
{
    qsort( HoleArray, NumHoles, sizeof( region ), HoleCompare );
}

void FreeHoleArray( void )
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

void ScanSyncString( const char *syncString )
{
    if( syncString != NULL ) {
        SyncOld = FindSyncString( OldFile, EndOld, syncString );
        SyncNew = FindSyncString( NewFile, EndNew, syncString );
        if( SyncOld == (foff)-1 || SyncNew == (foff)-1 ) {
            fatal( ERR_NO_SYNCSTRING );
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
}

void init_diff( void )
{
    NumSimilarities = 0;
}

void print_stats( long savings )
{
    foff        best_from_new;

    stats( "similar regions:    %8lu bytes (%lu chunks)\n", SimilarSize, NumSimilarities);
    stats( "different regions:  %8lu bytes (%lu chunks)\n", DiffSize, NumDiffs );
    stats( "hole->diff savings: %8lu bytes\n", savings );
    stats( "number of holes:    %8lu\n", NumHoles );
    stats( "(%lu headers + %lu single + %lu double + %lu triple)\n\n", HoleHeaders, HoleCount[0], HoleCount[1], HoleCount[2] );
    stats( "old file: %8lu bytes   new file: %8lu bytes\n", EndOld, EndNew );
    stats( "%lu%% of old executable referenced in patch file (largest amount is 100%%)\n", (SimilarSize*100) / EndOld );
    best_from_new = 0;
    if( EndNew > EndOld ) {
        best_from_new = EndNew - EndOld;
    }
    stats( "%lu%% of new executable output to patch file (least amount is %lu%%)\n", (DiffSize*100) / EndNew, (best_from_new*100) / EndNew );
    stats( "%lu total patch file size (%lu%%)\n", CurrPatch - PatchFile, ( ( CurrPatch - PatchFile ) * 100 ) / EndNew );
}

/*
 * For debugging purposes only (In debugger issue "CALL DUMP" command)
 */

void dump( void )
{
    region *reg;

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
