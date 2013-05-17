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
* Description:  BIOS emulation routines for QNX.
*
****************************************************************************/


#include "vi.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/seginfo.h>
#include <sys/console.h>
#include <sys/con_msg.h>
#include <sys/dev.h>
#include <sys/osinfo.h>
#include <sys/sidinfo.h>
#include <sys/psinfo.h>
#include <sys/kernel.h>
#include <sys/sendmx.h>
#include <sys/proxy.h>
#include <sys/vc.h>
#include "dosx.h"
#include "win.h"
#include "vibios.h"

extern pid_t            _my_pid;
extern int              PageCnt;

struct                  _console_ctrl *QNXCon;
int                     QNXConHandle;
unsigned int            QNXConsole;
struct termios          SaveTermSet;
int                     SaveTermProtocol;
pid_t                   Proxy;


struct map {
    unsigned char   qnx;
    vi_key          event;
};

static struct map events[] = {
    { 0x80,     VI_KEY( SHIFT_TAB )     },
    { 0x81,     VI_KEY( F1 )            },
    { 0x82,     VI_KEY( F2 )            },
    { 0x83,     VI_KEY( F3 )            },
    { 0x84,     VI_KEY( F4 )            },
    { 0x85,     VI_KEY( F5 )            },
    { 0x86,     VI_KEY( F6 )            },
    { 0x87,     VI_KEY( F7 )            },
    { 0x88,     VI_KEY( F8 )            },
    { 0x89,     VI_KEY( F9 )            },
    { 0x8a,     VI_KEY( F10 )           },
    { 0x8b,     VI_KEY( SHIFT_F1 )      },
    { 0x8c,     VI_KEY( SHIFT_F2 )      },
    { 0x8d,     VI_KEY( SHIFT_F3 )      },
    { 0x8e,     VI_KEY( SHIFT_F4 )      },
    { 0x8f,     VI_KEY( SHIFT_F5 )      },
    { 0x90,     VI_KEY( SHIFT_F6 )      },
    { 0x91,     VI_KEY( SHIFT_F7 )      },
    { 0x92,     VI_KEY( SHIFT_F8 )      },
    { 0x93,     VI_KEY( SHIFT_F9 )      },
    { 0x94,     VI_KEY( SHIFT_F10 )     },
    { 0x95,     VI_KEY( CTRL_F1 )       },
    { 0x96,     VI_KEY( CTRL_F2 )       },
    { 0x97,     VI_KEY( CTRL_F3 )       },
    { 0x98,     VI_KEY( CTRL_F4 )       },
    { 0x99,     VI_KEY( CTRL_F5 )       },
    { 0x9a,     VI_KEY( CTRL_F6 )       },
    { 0x9b,     VI_KEY( CTRL_F7 )       },
    { 0x9c,     VI_KEY( CTRL_F8 )       },
    { 0x9d,     VI_KEY( CTRL_F9 )       },
    { 0x9e,     VI_KEY( CTRL_F10 )      },
    { 0x9f,     VI_KEY( CTRL_TAB )      },
    { 0xa0,     VI_KEY( HOME )          },
    { 0xa1,     VI_KEY( UP )            },
    { 0xa2,     VI_KEY( PAGEUP )        },
    { 0xa3,     '-'                     },
    { 0xa4,     VI_KEY( LEFT )          },
    { 0xa5,     '5'                     },
    { 0xa6,     VI_KEY( RIGHT )         },
    { 0xa7,     '+'                     },
    { 0xa8,     VI_KEY( END )           },
    { 0xa9,     VI_KEY( DOWN )          },
    { 0xaa,     VI_KEY( PAGEDOWN )      },
    { 0xab,     VI_KEY( INS )           },
    { 0xac,     VI_KEY( DEL )           },
//  { 0xad,     VI_KEY( prtscr )        },
    { 0xae,     VI_KEY( F11 )           },
    { 0xaf,     VI_KEY( F12 )           },
    { 0xb0,     VI_KEY( CTRL_HOME )     },
    { 0xb1,     VI_KEY( CTRL_UP )       },
    { 0xb2,     VI_KEY( CTRL_PAGEUP )   },
    { 0xb3,     '-'                     },
    { 0xb4,     VI_KEY( CTRL_LEFT )     },
    { 0xb5,     '5'                     },
    { 0xb6,     VI_KEY( CTRL_RIGHT )    },
    { 0xb7,     '+'                     },
    { 0xb8,     VI_KEY( CTRL_END )      },
    { 0xb9,     VI_KEY( CTRL_DOWN )     },
    { 0xba,     VI_KEY( CTRL_PAGEDOWN ) },
    { 0xbb,     VI_KEY( CTRL_INS )      },
    { 0xbc,     VI_KEY( CTRL_DEL )      },
//  { 0xbd,     VI_KEY( ctrl-prtscr )   },
    { 0xbe,     VI_KEY( CTRL_F11 )      },
    { 0xbf,     VI_KEY( CTRL_F12 )      },
    { 0xc0,     VI_KEY( ALT_HOME )      },
    { 0xc1,     VI_KEY( ALT_UP )        },
    { 0xc2,     VI_KEY( ALT_PAGEUP )    },
    { 0xc3,     '-'                     },
    { 0xc4,     VI_KEY( ALT_LEFT )      },
    { 0xc5,     '5'                     },
    { 0xc6,     VI_KEY( ALT_RIGHT )     },
    { 0xc7,     '+'                     },
    { 0xc8,     VI_KEY( ALT_END )       },
    { 0xc9,     VI_KEY( ALT_DOWN )      },
    { 0xca,     VI_KEY( ALT_PAGEDOWN )  },
    { 0xcb,     VI_KEY( ALT_INS )       },
    { 0xcc,     VI_KEY( ALT_DEL )       },
//    { 0xcd,   VI_KEY(   alt-prtscr )  },
    { 0xce,     VI_KEY( ALT_F11 )       },
    { 0xcf,     VI_KEY( ALT_F12 )       },
    { 0xd0,     0x0a                    },
    { 0xd1,     VI_KEY( ALT_F1 )        },
    { 0xd2,     VI_KEY( ALT_F2 )        },
    { 0xd3,     VI_KEY( ALT_F3 )        },
    { 0xd4,     VI_KEY( ALT_F4 )        },
    { 0xd5,     VI_KEY( ALT_F5 )        },
    { 0xd6,     VI_KEY( ALT_F6 )        },
    { 0xd7,     VI_KEY( ALT_F7 )        },
    { 0xd8,     VI_KEY( ALT_F8 )        },
    { 0xd9,     VI_KEY( ALT_F9 )        },
    { 0xda,     VI_KEY( ALT_F10 )       },
    { 0xdb,     VI_KEY( SHIFT_F11 )     },
    { 0xdc,     VI_KEY( SHIFT_F12 )     },
//  { 0xdd,     VI_KEY( sysrq )         },
    { 0xe1,     VI_KEY( ALT_A )         },
    { 0xe2,     VI_KEY( ALT_B )         },
    { 0xe3,     VI_KEY( ALT_C )         },
    { 0xe4,     VI_KEY( ALT_D )         },
    { 0xe5,     VI_KEY( ALT_E )         },
    { 0xe6,     VI_KEY( ALT_F )         },
    { 0xe7,     VI_KEY( ALT_G )         },
    { 0xe8,     VI_KEY( ALT_H )         },
    { 0xe9,     VI_KEY( ALT_I )         },
    { 0xea,     VI_KEY( ALT_J )         },
    { 0xeb,     VI_KEY( ALT_K )         },
    { 0xec,     VI_KEY( ALT_L )         },
    { 0xed,     VI_KEY( ALT_M )         },
    { 0xee,     VI_KEY( ALT_N )         },
    { 0xef,     VI_KEY( ALT_O )         },
    { 0xf0,     VI_KEY( ALT_P )         },
    { 0xf1,     VI_KEY( ALT_Q )         },
    { 0xf2,     VI_KEY( ALT_R )         },
    { 0xf3,     VI_KEY( ALT_S )         },
    { 0xf4,     VI_KEY( ALT_T )         },
    { 0xf5,     VI_KEY( ALT_U )         },
    { 0xf6,     VI_KEY( ALT_V )         },
    { 0xf7,     VI_KEY( ALT_W )         },
    { 0xf8,     VI_KEY( ALT_X )         },
    { 0xf9,     VI_KEY( ALT_Y )         },
    { 0xfa,     VI_KEY( ALT_Z )         },
};

#pragma off (unreferenced);
void BIOSGetColorPalette( void far *a ) {}
long BIOSGetColorRegister( short a ) { return( 0 ); }
void BIOSSetNoBlinkAttr() {}
void BIOSSetBlinkAttr() {}
void BIOSSetColorRegister( short reg, char r, char g, char b ) {}
#pragma on (unreferenced);

static unsigned short _crow, _ccol, _ctype;

void BIOSSetCursor( char page, char row, char col )
{
    struct _mxfer_entry sx[2];
    struct _mxfer_entry rx;

    union _console_msg {
            struct _console_write       write;
            struct _console_write_reply write_reply;
    } msg;

    page = page;
    _crow = row;
    _ccol = col;

    if( QNXCon == NULL ) return;
    msg.write.type = _CONSOLE_WRITE;
    msg.write.handle = QNXCon->handle;
    msg.write.console = QNXConsole;
    msg.write.curs_row = row;
    msg.write.curs_col = col;
    msg.write.curs_type = _ctype;
    msg.write.offset = 0;
    msg.write.nbytes = 0;

    _setmx( &sx[1], 0, 0 );
    _setmx( &sx[0], &msg.write, sizeof( msg.write ) - sizeof( msg.write.data ) );

    _setmx( &rx, &msg.write_reply, sizeof( msg.write_reply ) );

    Sendmx( QNXCon->driver, 2, 1, &sx, &rx );

} /* BIOSSetCursor */

short BIOSGetCursor( char page )
{
    page = page;
    return( (short)((unsigned short)_crow * 256 + _crow ) );

} /* BIOSGetCursor */

void BIOSNewCursor( char top, char bottom )
{
    if( bottom-top > 5 ) {
        _ctype = CURSOR_BLOCK;
    } else {
        _ctype = CURSOR_UNDERLINE;
    }

} /* BIOSNewCursor */

static int CompareEvents( const void *d1, const void *d2 )
{
    const struct map *p1 = d1;
    const struct map *p2 = d2;

    return( p1->qnx - p2->qnx );
}

/*
 * BIOSGetKeyboard - get a keyboard char
 */
vi_key BIOSGetKeyboard( int *scan )
{
    unsigned char   ch;
    struct map      what;
    struct map      *ev;
    vi_key          key;

    while( dev_read( QNXConHandle, &ch, 1, 0, 0, 0, Proxy, 0 ) <= 0 ) {
        WaitForProxy();
    }
    if( ch == 0xff ) {
        key = VI_KEY( DUMMY );
        if( dev_read( QNXConHandle, &ch, 1, 0, 0, 0, Proxy, 0 ) > 0 ) {
            what.qnx = ch;
            ev = bsearch( &what, events, sizeof( events )/sizeof( struct map ),
                        sizeof( what ), CompareEvents );
            if( ev != NULL ) {
                key = ev->event;
            }
        }
    } else {
        if( ch == 127 ) {
            ch = 8; /* KLUDGE - bs comes through as del */
        }
        key = ch;
    }
    if( scan != NULL ) {
        *scan = 0;
    }
    if( key == 0xe0 ) {
        return( 0 );
    }
    return( key );

} /* BIOSGetKeyboard */

/*
 * BIOSKeyboardHit - test for keyboard hit
 */
bool BIOSKeyboardHit( void )
{
    return( dev_ischars( QNXConHandle ) != 0 );

} /* BIOSKeyboardHit */


void WaitForProxy( void )
{
    dev_arm( QNXConHandle, Proxy, _DEV_EVENT_INPUT );
    Receive( Proxy, 0, 0 );

} /* WaitForProxy */

static void addConsoleNumber( char * ptr, unsigned number )
{
    ptr[0] = (number / 10) + '0';
    ptr[1] = (number % 10) + '0';
    ptr[2] = 0;

} /* addConsoleNumber */

/*
 * BIOSKeyboardInit
 */
int BIOSKeyboardInit( void )
{
    struct _dev_info_entry      dev;
    char                        *ptr;
    char                        *digits;
    struct _psinfo              psinfo;
    struct _sidinfo             sidinfo;
    pid_t                       proc;
    unsigned                    console;

    Proxy = qnx_proxy_attach( 0, 0, 0, -1 );
    if( Proxy == -1 ) {
        FatalError( ERR_WIND_NO_MORE_WINDOWS );
    }
    if( strcmp( getenv( "TERM" ), "qnxw" ) == 0 ) {
//      EditFlags.IsWindowedConsole = TRUE;
    }

    if( qnx_psinfo( PROC_PID, _my_pid, &psinfo, 0, 0 ) != _my_pid ) {
        return( ERR_WIND_NO_MORE_WINDOWS );
    }
    proc = qnx_vc_attach( psinfo.sid_nid, PROC_PID, sizeof( sidinfo ), 0 );
    if( proc == -1 ) {
        return( ERR_WIND_NO_MORE_WINDOWS );
    }
    if( qnx_sid_query( proc, psinfo.sid, &sidinfo )!= psinfo.sid ) {
        return( ERR_WIND_NO_MORE_WINDOWS );
    }
    qnx_vc_detach( proc );

    ptr = &sidinfo.tty_name[strlen( sidinfo.tty_name )];
    for( ;; ) {
        if( ptr[-1] < '0' || ptr[-1] > '9' ) {
            break;
        }
        ptr--;
    }
    digits = ptr;
    console = 0;
    while( *ptr != '\0' ) {
        console = (console * 10) + (*ptr - '0');
        ++ptr;
    }
    if( QNXConsole == 0 ) {             // open the current console
        QNXConsole = console;
    } else if( QNXConsole == -1 ) {     // open any available console
        digits[0] = '\0';
    } else {                            // open a specific console
        addConsoleNumber( digits, QNXConsole );
    }

    QNXConHandle = open( sidinfo.tty_name, O_RDWR );
    if( QNXConHandle == -1 ) {
        if( QNXConsole != -1 ) {
            FatalError( ERR_WIND_NO_MORE_WINDOWS );
        }
        addConsoleNumber( digits, console );
        QNXConHandle = open( sidinfo.tty_name, O_RDWR );
        if( QNXConHandle == -1 ) {
            FatalError( ERR_WIND_NO_MORE_WINDOWS );
        }
    }

    if( QNXConsole == -1 ) {
        if( dev_info( QNXConHandle, &dev ) == -1 ) {
            return( ERR_WIND_NO_MORE_WINDOWS );
        }
        QNXConsole = dev.unit;
    }

    fcntl( QNXConHandle, F_SETFD, (int) FD_CLOEXEC );
    if( tcsetpgrp( QNXConHandle, psinfo.pid_group ) != 0 ) {
        return( ERR_WIND_NO_MORE_WINDOWS );
    }
    tcgetattr( QNXConHandle, &SaveTermSet );
    SaveTermProtocol = console_protocol( QNXCon, QNXConsole, -1 );
    RestoreKeyboard();
//    StopKeyboard();
    return( ERR_NO_ERR );

} /* BIOSKeyboardInit */

void RestoreKeyboard( void )
{
    struct termios  new;

    new = SaveTermSet;
    new.c_iflag &= ~(IXOFF | IXON | ICRNL | INLCR);
    new.c_oflag &= ~OPOST;
    new.c_lflag &= ~(ECHO | ICANON | NOFLSH | ECHOE);
    new.c_lflag |= ISIG;
    new.c_cc[VINTR] = '\0';
    new.c_cc[VMIN] = 1;
    new.c_cc[VTIME] = 0;
    tcsetattr( QNXConHandle, TCSADRAIN, &new );
    console_protocol( QNXCon, QNXConsole, _CON_PROT_QNX4 );

} /* RestoreKeyboard */

void KeyboardFini( void )
{
    tcsetattr( QNXConHandle, TCSADRAIN, &SaveTermSet );
    console_protocol( QNXCon, QNXConsole, SaveTermProtocol );

} /* KeyboardFini */


void StopKeyboard( void )
{
    dev_read( QNXConHandle, NULL, 0, 0, 0, 0, 0, 0 );
    while( Creceive( Proxy, 0, 0 ) > 0 );

} /* StopKeyboard */

/*
 * BIOSUpdateScreen - update the screen
 */
void BIOSUpdateScreen( unsigned offset, unsigned nchars )
{
    struct _mxfer_entry sx[2];
    struct _mxfer_entry rx;
    union _console_msg {
            struct _console_write       write;
            struct _console_write_reply write_reply;
    }                   msg;

    if( PageCnt > 0 ) {
        return;
    }

    msg.write.type = _CONSOLE_WRITE;
    msg.write.handle = QNXCon->handle;
    msg.write.console = QNXConsole;
    msg.write.curs_row = _crow;
    msg.write.curs_col = _ccol;
    msg.write.curs_type = _ctype;
    msg.write.offset = offset * sizeof( char_info );
    msg.write.nbytes = nchars * sizeof( char_info );

    _setmx( &sx[1], Scrn + offset, nchars * sizeof( char_info ) );
    _setmx( &sx[0], &msg.write, sizeof( msg.write ) - sizeof( msg.write.data ) );

    _setmx( &rx, &msg.write_reply, sizeof( msg.write_reply ) );

    Sendmx(QNXCon->driver, 2, 1, &sx, &rx );

} /* BIOSUpdateScreen */

