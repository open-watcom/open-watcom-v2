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
* Description:  Invoke a debugger script.
*
****************************************************************************/


#include "dbgdefn.h"
#include "dbglit.h"
#include "dbgtoken.h"
#include "dbginvk.h"
#include "dbgerr.h"
#include "dbgmem.h"
#include "dbgtoggl.h"
#include "dbgio.h"
#include "dbginp.h"
#include <string.h>



extern unsigned         InvCount;
extern char             *TxtBuff;


extern bool             ScanItem( bool, char **, size_t * );
extern bool             ScanEOC( void );
extern bool             SwitchOnOff( void );
extern void             ShowSwitch( bool );
extern handle           LocalFullPathOpen( const char *name, unsigned name_len, const char *ext, char *result, unsigned max_result );
extern void             PushInpStack( void *, bool (*rtn)( void *, inp_rtn_action ), bool );
extern void             TypeInpStack( input_type );
extern char             *ReScan( char * );
extern unsigned int     ScanCmd( char * );
extern void             FreeRing( char_ring * );
extern char             *CnvULongDec( unsigned long, char * );
extern void             LogStart( void );
extern void             LogEnd( void );


/*
 * ImplicitSet - set implicit/explicit invoke file processing
 */

extern void ImplicitSet( void )
{
    _SwitchSet( SW_IMPLICIT, SwitchOnOff() );
}

extern void ImplicitConf( void )
{
    ShowSwitch( _IsOn( SW_IMPLICIT ) );
}


#define KEY_EOF         0

#define PARM_START      '<'
#define PARM_END        '>'



/*
 * InvRead -- read from an invoke file
 */

static int InvRead( invokes *inv, unsigned *save_point )
{
    int         ch;
    unsigned    left;
    int         size;

    if( inv->flags & AT_EOF ) return( KEY_EOF );
    for( ;; ) {
        if( inv->in_off < inv->in_size ) {
            ch = inv->in_buff[ inv->in_off++ ];
            if( ch == '\r' ) continue;
            return( ch );
        }
        left = inv->in_size - *save_point;
        memmove( &inv->in_buff[0], &inv->in_buff[*save_point], left );
        size = ReadText( inv->inv_input, &inv->in_buff[left], IN_BUFF_SIZE - left );
        if( size <= 0 ) break;
        inv->in_size = size + left;
        inv->in_off = left;
        *save_point = 0;
    }
    inv->flags |= AT_EOF;
    return( KEY_EOF );
}



/*
 * InvGetKey -- get input character from keyboard or invoke file
 */

static int InvGetKey( invokes *inv )
{
    int         count;
    char_ring   *next;
    int         ch;
    unsigned    parm;
    unsigned    save;
    static      char inv_num[5];

    for( ;; ) {
        if( inv->redirect != NULL ) {
            ch = *(inv->redirect);
            if( ch == NULLCHAR ) {
                inv->redirect = NULL;
            } else {
                inv->redirect++;
                return( ch );
            }
        }
        save = inv->in_off;
        ch = InvRead( inv, &save );
        if( ch != PARM_START ) return( ch );
        parm = InvRead( inv, &save );
        if( parm < '0' || parm > '9' ) break;
        ch = InvRead( inv, &save );
        if( ch != PARM_END ) break;
        count = parm - '0';
        if( count == 0 ) {
            CnvULongDec( inv->number, inv_num );
            inv->redirect = inv_num;
        } else {
            next = inv->prmlst;
            while( next != NULL && --count > 0 ) {
                next = next->next;
            }
            if( next != NULL ) inv->redirect = next->name;
        }
    }
    inv->in_off = save + 1;
    return( PARM_START );
}

static bool GetInvkCmd( invokes *inv )
{
    int         ch;
    int         unmatched;
    char        *cmd;
    char        *buff;
    bool        eatwhite;

    if( inv == NULL ) return( FALSE );
    cmd = inv->buff;
    buff = inv->buff;
    ch = '\0';
    unmatched = 0;
    eatwhite = FALSE;
    for( ;; ) {
        ch = InvGetKey( inv );
        if( inv->flags & AT_EOF ) return( FALSE );
        if( ( ch == ' ' || ch == '\t' ) && eatwhite ) continue;
        eatwhite = FALSE;
        if( ch == '}' ) --unmatched;
        if( ch == '{' ) ++unmatched;
        if( ch == '\n' ) {
            inv->line++;
            if( unmatched <= 0 ) break;
            eatwhite = TRUE;
            ch = '\r';
        }
        if( cmd >= &buff[ inv->buff_size ] ) {
            _Alloc( buff, inv->buff_size + CMD_LEN + 1 );
            if( buff != NULL ) {
                memcpy( buff, inv->buff, inv->buff_size );
                cmd = buff + inv->buff_size;
                _Free( inv->buff );
                inv->buff = buff;
                inv->buff_size += CMD_LEN;
            } else {
                buff = inv->buff;
                continue;
            }
        }
        *cmd = ( char ) ch;
        cmd++;
    }
    *cmd = NULLCHAR;
    return( TRUE );
}


static void Conclude( invokes *inv )
{
    if( inv->inv_input != NIL_HANDLE ) {
        FileClose( inv->inv_input );
    }
    FreeRing( inv->prmlst );
    _Free( inv->buff );
    _Free( inv );
}


OVL_EXTERN bool DoneInvLine( void *inv, inp_rtn_action action )
{
    switch( action ) {
    case INP_RTN_INIT:
    case INP_RTN_EOL:
        if( !GetInvkCmd( inv ) ) return( FALSE );
        ReScan( ((invokes *)inv)->buff );
        return( TRUE );
    case INP_RTN_FINI:
        Conclude( inv );
        return( TRUE );
    default:
        return( FALSE ); // silence compiler
    }
}


/*
 * Invoke -- invoke a file
 */

static void DoInvoke( handle hndl, char *name, char_ring *parmlist )
{
    invokes     *inv;

    _Alloc( inv, sizeof( invokes ) + strlen( name ) );
    if( inv != NULL ) {
        inv->buff_size = CMD_LEN;
        _Alloc( inv->buff, inv->buff_size+1 ); /* extra for NULLCHAR */
    }
    if( inv == NULL || inv->buff == NULL ) {
        if( inv != NULL ) _Free( inv );
        FileClose( hndl );
        FreeRing( parmlist );
        Error( ERR_NONE, LIT( ERR_NO_MEMORY ) );
    }
    strcpy( inv->name, name );
    inv->in_size = 0;
    inv->in_off = 0;
    inv->flags = 0;
    inv->redirect = NULL;
    inv->inv_input = hndl;
    inv->prmlst = parmlist;
    inv->number = InvCount++;
    inv->line = 0;
    PushInpStack( inv, DoneInvLine, TRUE );
    TypeInpStack( INP_CMD_FILE );
}

void Invoke( const char *invfile, int len, char_ring *parmlist )
{
    handle      hndl;

    hndl = LocalFullPathOpen( invfile, len, "dbg", TxtBuff, TXT_LEN );
    if( hndl == NIL_HANDLE ) {
        MakeFileName( TxtBuff, invfile, "dbg", 0 );
        FreeRing( parmlist );
        Error( ERR_NONE, LIT( ERR_FILE_NOT_OPEN ), TxtBuff );
    }
    DoInvoke( hndl, TxtBuff, parmlist );
}


/*
 * ProfileInvoke -- process invoking debugger profile file
 */

void ProfileInvoke( char *name )
{
    unsigned    len;
#if defined(__UNIX__)
    handle      hndl;
#endif

    len = strlen( name );
    if( len != 0 ) {
        Invoke( name, len, NULL );
        return;
    }
    /* default name search */
    len = strlen( EXENAME );
    _AllocA( name, len + 4 );
#if defined(__UNIX__)
        /* under QNX and Linux, look for .wdrc first */
        name[0] = '.';
        strcpy( &name[1], EXENAME );
        strcat( name, "rc" );
        strlwr( name );
        hndl = LocalFullPathOpen( name, strlen( name ), LIT( Empty ), TxtBuff, TXT_LEN );
        if( hndl != NIL_HANDLE ) {
            DoInvoke( hndl, TxtBuff, NULL );
            return;
        }
#endif
    strcpy( name, EXENAME );
    strlwr( name );
    Invoke( name, len, NULL );
}

/*
 * ProcInvoke -- process invoke command
 */

void ProcInvoke( void )
{
    char      *fstart;
    size_t    flen;
    char      *start;
    char_ring *parmlist;
    char_ring **owner;
    char_ring *path;
    size_t    len;

    if( !ScanItem( TRUE, &fstart, &flen ) ) Error( ERR_LOC, LIT( ERR_WANT_FILENAME ) );
    parmlist = NULL;
    owner = &parmlist;
    while( !ScanEOC() ) {
        ScanItem( TRUE, &start, &len );
        _Alloc( path, sizeof( char_ring ) + len );
        if( path == NULL ) {
            FreeRing( parmlist );
            Error( ERR_NONE, LIT( ERR_NO_MEMORY ) );
        }
        memcpy( path->name, start, len );
        path->name[ len ] = NULLCHAR;
        path->next = NULL;
        *owner = path;
        owner = &path->next;
    }
    Invoke( fstart, (int)flen, parmlist );
}


void CreateInvokeFile( char *name, void (*rtn)(void) )
{
    char        *p;

    p = ReScan( name );
    LogStart();
    rtn();
    LogEnd();
    ReScan( p );
}


void InvokeAFile( char *name )
{
    ReScan( name );
    ProcInvoke();
}
