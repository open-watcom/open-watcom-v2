/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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


#include <stddef.h>
#include <stdio.h>
#include "ide.rh"
#ifdef __WINDOWS__
    #include "common.h"
    #include "link.h"
#endif
#include "batcher.h"
#include "vpemain.hpp"
#include "vmsglog.hpp"
#include "mproject.hpp"
#include "wstrlist.hpp"
#include "wlistbox.hpp"
#include "wstring.hpp"
#include "wstrlist.hpp"
#include "wtimer.hpp"
#include "wmsgdlg.hpp"
#include "wfiledlg.hpp"
#include "wfile.hpp"
#include "wfilenam.hpp"
#include "wmenu.hpp"
#include "wmenuitm.hpp"
#include "wmetrics.hpp"
#include "wprocess.hpp"
#include "wsystem.hpp"
#include "wpshbttn.hpp"
#include "wsyshelp.hpp"

#define LOG_HELP_KEY    GUI_KEY_F1
#define LOG_ESCAPE_KEY  GUI_KEY_ESCAPE

#define CONNECTION_TRIES        10
#define CONNECTION_INTERVAL     300

extern char _viperError[];

static WString lastCD;

static char sFilter[] = {
    "Listing Files(*.txt)\0*.txt\0"
    "All files(*.*)\0*.*\0"
    "\0"
};

static int _connectionTries = 0;

static char fortranGrpCodes[] = {
     'A','R'
    ,'B','D'
    ,'C','C'
    ,'C','M'
    ,'C','N'
    ,'C','O'
    ,'C','P'
    ,'C','V'
    ,'D','A'
    ,'D','M'
    ,'D','O'
    ,'E','C'
    ,'E','N'
    ,'E','Q'
    ,'E','V'
    ,'E','X'
    ,'E','Y'
    ,'F','M'
    ,'G','O'
    ,'H','O'
    ,'I','F'
    ,'I','L'
    ,'I','M'
    ,'I','O'
    ,'K','O'
    ,'L','I'
    ,'M','D'
    ,'M','O'
    ,'P','C'
    ,'P','R'
    ,'R','E'
    ,'S','A'
    ,'S','F'
    ,'S','M'
    ,'S','P'
    ,'S','R'
    ,'S','S'
    ,'S','T'
    ,'S','V'
    ,'S','X'
    ,'T','Y'
    ,'V','A'
    ,'\0','\0'
};

Define( VMsgLog )

WEXPORT VMsgLog::VMsgLog( VpeMain* parent )
    : WMdiChild( parent, "IDE Log" )
    , _parent( parent )
    , _batcher( NULL )
    , _connecting( false )
    , _runQueued( false )
    , _connectTimer( NULL )
    , _serverConnected( false )
    , _maxLength( 0 )
    , _running( false )
    , _batserv( true )
    , _localBatserv( false )
    , _vxdPresent( false )
{
    if( _config->hostType() == HOST_WINDOWS ||
        _config->hostType() == HOST_NEC_WIN ||
        _config->hostType() == HOST_J_WIN ) {
        _batserv = false;
    }
    setIcon( I_MsgLog );

    WRect sc;
    _parent->getClientRect( sc );
    sc.y( sc.h() * 2/3 );
    sc.h( sc.h() - sc.y() );
    move( sc );

    int yoff = 0;

    _batcher = new VListBox( this, WRect(0,yoff,-1,-1) );
    _batcher->onDblClick( this, (cbw)&VMsgLog::selected );
    _batcher->show();

    show();

    loadHelpList();

    clearData();

    _batcher->setPopup( _parent->logPopup() );

    addAccelKey( LOG_ESCAPE_KEY, this, (bcbk)&VMsgLog::kAccelKey );
    addAccelKey( LOG_HELP_KEY, this, (bcbk)&VMsgLog::kAccelKey );
}

void VMsgLog::startConnect()
{
    _connectTimer = new WTimer( this, (cbt)&VMsgLog::connectTimer, 32768 );
    if( !_batserv ) {
#ifdef __WINDOWS__
        _vxdPresent = (bool)VxDPresent();
        if( _vxdPresent ) {
            const char* res = VxDLink( LINK_NAME );
            if( !res ) {
                WSystemService::sysExecBackground( _config->batserv() );
                _connectionTries = CONNECTION_TRIES;
                _connecting = true;
                _connectTimer->start( CONNECTION_INTERVAL );
                addLine( "Connecting..." );
            } else {
                WMessageDialog::info( this, "VxD: %s", res );
            }
        } else {
            WMessageDialog::info( this, "VxD: WDEBUG.386 not present" );
        }
#endif
    } else {
#ifdef __WINDOWS__
        const char* err = BatchLink( NULL );
        if( err ) {
            _localBatserv = true;
            WSystemService::sysExecBackground( _config->batserv() );
            _connectionTries = CONNECTION_TRIES;
            _connecting = true;
            _connectTimer->start( CONNECTION_INTERVAL );
            addLine( "Connecting..." );
        } else {
            _serverConnected = true;
        }
#else
        const char* err = BatchLink( NULL );
        if( err ) {
            _localBatserv = true;
            WSystemService::sysExecBackground( _config->batserv() );
            addLine( "Connecting..." );
            _connectionTries = CONNECTION_TRIES;
            while( err && _connectionTries > 0 ) {
                WSystemService::sysSleep( CONNECTION_INTERVAL );
                err = BatchLink( NULL );
                _connectionTries -= 1;
            }
        }
        if( err ) {
            WMessageDialog::info( this, err );
            _parent->deleteMsglog();
            //zombie code at this point!!!!!!!!!!!!!!
        } else {
            _serverConnected = true;
        }
#endif
    }
}

WEXPORT VMsgLog::~VMsgLog()
{
    if( !_batserv ) {
#ifdef __WINDOWS__
        if( _vxdPresent ) {
            if( _serverConnected ) {
                VxDPut( TERMINATE_CLIENT_STR, sizeof( TERMINATE_CLIENT_STR )+1 );
            }
            for( int i = 0; i < 100; i++ ) {
                if( VxDUnLink() == 0 ) {
                    break;
                }
            }
        }
#endif
    } else {
        if( _serverConnected ) {
            BatchUnlink( _localBatserv );
        }
    }
    delete _connectTimer;
    _data.deleteContents();
    _helpList.deleteContents();
}

#ifndef NOPERSIST
VMsgLog* WEXPORT VMsgLog::createSelf( WObjectFile& )
{
    return( NULL );
}

void WEXPORT VMsgLog::readSelf( WObjectFile& )
{
}

void WEXPORT VMsgLog::writeSelf( WObjectFile& )
{
}
#endif

void VMsgLog::getState( bool& editOk, bool& helpOk )
{
    int index = _batcher->selected();
    if( index >= 0 ) {
        char file[101]; int line, offset; char help[51];
        if( matchLine( index, file, line, offset, help ) ) {
            if( strlen( file ) > 0 ) {
                editOk = true;
            }
            int hcount = _helpList.count();
            if( hcount > 0 && strlen( help ) > 0 ) {
                helpOk = true;
            }
        }
    }
}

bool VMsgLog::saveLogAs()
{
    bool ok = false;
    WFileName fn( "log" );
    MProject* project = _parent->project();
    if( project ) {
        project->filename().noPath( fn );
    }
    fn.setExt( "txt" );
    WFileDialog fd( this, sFilter );
    fn = fd.getSaveFileName( fn, "Save Log as", WFSaveDefault );
    if( fn.legal() ) {
//        fn.toLower();
        WFile f;
        if( !f.open( fn, OStyleWrite ) ) {
            WMessageDialog::messagef( this, MsgError, MsgOk, _viperError, "Unable to save log file '%s'", (const char*)fn );
        } else {
            int icount  = _data.count();
            for( int i = 0; i < icount; i++ ) {
                f.puts( *(WString*)_data[i] );
                f.puts( "\n" );
            }
            f.close();
            ok = true;
        }
    }
    return( ok );
}

bool VMsgLog::kAccelKey( gui_key _key )
{
    int key = _key;

    switch( key ) {
        case LOG_ESCAPE_KEY: {
            stopRequest( NULL );
            break;
        }
        case LOG_HELP_KEY: {
            helpRequest( NULL );
            break;
        }
    }
    return( true );
}

void VMsgLog::stopRequest( WMenuItem* )
{
    if( _running ) {
        MsgRetType ret = WMessageDialog::messagef( this, MsgInfo, MsgOkCancel, NULL,
                "OK to interrupt the executing process?" );
        if( ret == MsgRetOk ) {
            if( !_batserv ) {
#ifdef __WINDOWS__
                VxDRaiseInterrupt( 0x1B );      //send ctrlBreak
                addLine( "Stop invoked..." );
#endif
            } else {
                if( BatchCancel() ) {
                    addLine( "Stop failed" );
                } else {
                    addLine( "Stop invoked..." );
                }
            }
        }
    }
}

void VMsgLog::killBatcher()
{
    if( _batserv ) {
        BatchAbort();
    }
}

void VMsgLog::connectTimer( WTimer* timer, DWORD )
{
    const char* err = "Unable to connect to batch server.";
    if( !_batserv ) {
#ifdef __WINDOWS__
        _serverConnected = ( VxDConnect() != 0 );
#endif
    } else {
        err = BatchLink( NULL );
        if( !err ) {
            _serverConnected = true;
        }
    }
    if( _serverConnected ) {
        timer->stop();
        _connecting = false;
        if( _runQueued ) {
            _runQueued = false;
            doRun();
        }
    } else if( _connectionTries == 0 ) {
        timer->stop();
        _connecting = false;
        WMessageDialog::info( this, err );
        _parent->deleteMsglog();
        //zombie code at this point!!!!!!!!!!!!!!
    } else {
        _connectionTries -= 1;
    }
}

void WEXPORT VMsgLog::runCommand( const char* cmd )
{
    _command = cmd;
    if( _serverConnected ) {
        clearData();
        doRun();
    } else {
        _runQueued = true;
    }
}

void VMsgLog::doRun()
{
    _running = true;
    if( isIconic() ) {
        show( WWinStateShowNormal );
    } else {
        show();
    }
    setFocus();
    _batcher->setFocus();
    if( _cwd.size() > 0 ) {
        WString c;
        if( _cwd.needQuotes() ) {
            c.printf( "cd \"%s\"", (const char*)_cwd );
        } else {
            c.printf( "cd %s", (const char*)_cwd );
        }
        lastCD = "";
        if( !_batserv ) {
            runCmd( c );
        } else {
            addLine( c );
            BatchChdir( _cwd );
        }
    }

    size_t icount = _command.size();
    for( size_t i = 0; i < icount; ) {
        WString cbuff;
        for( ; i < icount; ) {
            char ch = _command[i++];
            if( ch == '\n' )
                break;
            cbuff.concat( ch );
        }
        if( cbuff.size() > 0 ) {
            runCmd( cbuff );
        }
    }
    addLine( "Execution complete" );
    _running = false;
    _parent->quickRefresh();
}

#define MAX_BUFF                1000
static int blength = 0;
static char buffer[MAX_BUFF+1];

void VMsgLog::scanLine( const char* buff, int len )
{
    for( int i = 0; i < len; i++ ) {
        if( buff[i] == 10 ) {
        } else if( buff[i] == 13 ) {
            addLine( buffer );
            blength = 0;
            buffer[blength] = '\0';
        } else {
            buffer[blength++] = buff[i];
            buffer[blength] = '\0';
            if( blength >= MAX_BUFF ) {
                addLine( buffer );
                blength = 0;
                buffer[blength] = '\0';
            }
        }
    }
}

void VMsgLog::runCmd( WString &cmd )
{
static char buff[MAX_BUFF+1];
    addLine( cmd );
    blength = 0;
    buffer[blength] = '\0';
    if( !_batserv ) {
#ifdef __WINDOWS__
        VxDPut( cmd.gets(), cmd.size() + 1 );
        for( ;; ) {
            int len = VxDGet( buff, MAX_BUFF );
            buff[len] = '\0';
            if( streq( buff, TERMINATE_COMMAND_STR ) ) {
                break;
            } else if( len > 0 ) {
                scanLine( buff, len );
            }
        }
#endif
    } else {
        unsigned        maxlen;

        maxlen = BatchMaxCmdLine();
        cmd.truncate( maxlen );
        BatchSpawn( cmd );
        for( ;; ) {
            WSystemService::sysYield(); //allow other tasks to run
            unsigned long stat;
            int len = BatchCollect( buff, MAX_BUFF, &stat );
            if( len < 0 ) {
                break;
            } else if( len > 0 ) {
                scanLine( buff, len );
            }
        }
    }
    if( strlen( buffer ) > 0 ) {
        addLine( buffer );
    }
}

#define GROUP_INCREMENT         100
static bool parseFortranId( WString &str, unsigned j, char *help ) {

    char        groupid[2];
    char        num[50];
    char        *table;
    unsigned    id;
    unsigned    i;
    bool        ret;

    ret = true;
    if( isalpha( str[j] ) && isalpha( str[j+1] ) && str[j+2] == '-' ) {
        groupid[0] = str[j];
        groupid[1] = str[j+1];
        j += 3;
    } else {
        ret = false;
    }
    if( !isdigit( str[j] ) )
        ret = false;
    if( ret ) {
        i=0;
        while( isdigit( str[j] ) ) {
            num[i] = str[j];
            j++;
            i++;
        }
        num[i] = '\0';
        id = GROUP_INCREMENT;
        table = fortranGrpCodes;
        for( ;; ) {
            if( table[0] == '\0' && table[1] == '\0' ) {
                ret = false;
                break;
            }
            if( groupid[0] == table[0] && groupid[1] == table[1] ) {
                id += atoi( num );
                ltoa( id, help, 10 );
                break;
            }
            id += GROUP_INCREMENT;
            table += 2;
        }
    }
    return( ret );
}

/*
 * matchPattern- literal text and match the following identifiers:
 *              %f - filename
 *              %i - fortran style error identifier
 *              %l - line number
 *              %o - offset (column)
 *              %h - error idetifier (ex E100)
 *              %* - any text
 */
bool VMsgLog::matchPattern( const char* p, int index, char* file, int& line, int& offset, char* help )
{
    WString str( *(WString*)_data[index] );
    int i = 0, j = 0; int k, kk;
    while( p[i] != '<' ) {
        if( p[i] == '\0' )
            return( false );
        i++;
    }
    i++;
    for( ;; ) {
        if( p[i] == '\0' )
            return( false );
        if( p[i] == '>' )
            break;
        if( strncmp( &p[i], "%f", 2 ) == 0 ) {
            i += 2;
            k = 0;
            if( isalpha( str[j] ) && str[j+1]==':' ) {
                file[k++] = str[j++];
                file[k++] = str[j++];
            }
            for( kk = -1; kk != k ; ) {
                kk = k;
                if( str[j] == '\\' || str[j] == '/' )
                    file[k++] = str[j++];
                while( isalnum( str[j] ) || str[j] == '.'
                        || str[j] == '_' || str[j] == '-' ) {
                    file[k++] = str[j++];
                }
            }
            file[k] = '\0';
        } else if( strncmp( &p[i], "%l", 2 ) == 0 ) {
            i += 2;
            line = 0;
            for( ;; ) {
                if( !isdigit( str[j] ) )
                    break;
                line = line*10 + (str[j++]-'0');
            }
        } else if( strncmp( &p[i], "%o", 2 ) == 0 ) {
            i += 2;
            offset = 0;
            for( ;; ) {
                if( !isdigit( str[j] ) )
                    break;
                offset = offset*10 + (str[j++]-'0');
            }
        } else if( strncmp( &p[i], "%h", 2 ) == 0 ) {
            i += 2;
            if( str[j] == 'E' || str[j] == 'W' || str[j] == 'N' ) {
//              help[k++] = str[j++];   drw 12/13/94 k could be uninitialized
                j++;
            }
            for( k=0; isdigit( str[j] ); ) {
                help[k++] = str[j++];
            }
            help[k] = '\0';
        // this is a kludge to get fortran help working
        } else if( strncmp( &p[i], "%i", 2 ) == 0 ) {
            i += 2;
            if( !parseFortranId( str, j, help ) ) {
                break;
            }
        } else if( strncmp( &p[i], "%*", 2 ) == 0 ) {
            i += 2;
            for( ;; ) {
                if( p[i] != '>' && str[j] == p[i] )
                    break;
                if( str[j] == '\0' )
                    break;
                j++;
            }
        } else if( p[i] == str[j] ) {
            i++; j++;
        } else {
            break;
        }
    }
    if( p[i]=='>' && str[j]=='\0' ) {
        while( p[i] != '<' ) {
            if( p[i] == '\0' )
                return( true );
            i++;
        }
        while( index > 0 ) {
            index -= 1;
            int l, o;
            if( matchPattern( &p[i], index, file, l, o, help ) ) {
                return( true );
            }
        }
    }
    return( false );
}

bool VMsgLog::matchLine( int index, char* file, int& line, int& offset, char* help )
{
    file[0] = '\0';     line = 0; offset = 0; help[0] = '\0';
    for( int i=0; i<_config->logScanPatterns().count(); i++ ) {
        WString& p = *(WString*)_config->logScanPatterns()[i];
        if( matchPattern( p, index, file, line, offset, help ) ) {
            WFileName f( file );
            while( index > 0 ) {
                index --;
                WString* data = (WString*)_data[index];
                if( data->match( "cd *" ) ) {
                    WString dir( &(*data)[3] );
                    size_t dirLen = dir.size() - 1;
                    if( dir[dirLen] != '\\' ) {
                        dir.concat( '\\' );
                    }
                    f.absoluteTo( dir );
                    strcpy( file, f );
                    break;
                }
            }
            return( true );
        }
    }
    return( false );
}

void VMsgLog::loadHelpList()
{
    int hcount;

#ifdef __NT__
    hcount = _config->logHtmlHelpFiles().count();
    for( int i=0; i<hcount; i+=LOG_HELP_WIDTH ) {
        const char* hx = *(WString*)_config->logHtmlHelpFiles()[i];
        const char* hf = *(WString*)_config->logHtmlHelpFiles()[i + 1];
        int offset = atoi( *(WString*)_config->logHtmlHelpFiles()[i + 2] );
        _helpList.add( new WSystemHelp( this, hx, NULL, hf, offset ) );
    }
#endif
    hcount = _config->logHelpFiles().count();
    for( int i=0; i<hcount; i+=LOG_HELP_WIDTH ) {
        const char* hx = *(WString*)_config->logHelpFiles()[i];
        const char* hf = *(WString*)_config->logHelpFiles()[i + 1];
        int offset = atoi( *(WString*)_config->logHelpFiles()[i + 2] );
        _helpList.add( new WSystemHelp( this, hx, hf, NULL, offset ) );
    }
}

int VMsgLog::findHelpFile( const char *file, WSystemHelp **hobj, int from )
{
    if( strlen( file ) > 0 ) {
        int hcount = _helpList.count();
        if( hcount > 0 ) {
            WFileName f( file );
            while( from < hcount ) {
                WSystemHelp *shf = (WSystemHelp *)_helpList[from++];
                if( strieq( shf->getHelpTitle(), f.ext() ) ) {
                    if( hobj )
                        *hobj = shf;
                    return( from );
                }
            }
        }
    }
    return( 0 );
}

void VMsgLog::helpRequest( WMenuItem* )
{
    int index = _batcher->selected();
    if( index >= 0 ) {
        char file[101];
        int line, offset;
        char help[51];
        if( matchLine( index, file, line, offset, help ) ) {
            WSystemHelp* hobj;
            int next = 0;
            while( (next = findHelpFile( file, &hobj, next )) != 0 ) {
                if( hobj->sysHelpId( atoi( help ) + hobj->getHelpOffset() ) ) {
                    return;
                }
            }
            WMessageDialog::info( this, "No help available for error %s.", help );
        }
    }
}

void VMsgLog::selected( WWindow* )
{
    editRequest( NULL );
}

void VMsgLog::editRequest( WMenuItem* )
{
    int index = _batcher->selected();
    if( index >= 0 ) {
        const char* text = *(WString*)_data[index];
        char file[101]; int line, offset; char help[51];
        if( matchLine( index, file, line, offset, help ) ) {
            WSystemHelp *sh;
            const char *hf;
            if( findHelpFile( file, &sh, 0 ) == 0 ) {
                hf = "";
            } else {
                hf = sh->getHelpFile();
            }
            WString msg;
            int resId = atoi( help ) + 1;
            WFileName filename( file );
            if( filename.needQuotes() ) {
                filename.addQuotes();
            }
            msg.printf( "EditFileAtPos -f%s %d %d 0 %d \"%s\" %s",
                            (const char*)filename, line, offset, resId, text, hf );
            _parent->executeCommand( msg, EXECUTE_EDITOR, "LogEdit" );
        } else {
            WMessageDialog::info( this, "Can't find a filename in '%s'", text );
        }
    }
}

void VMsgLog::clearData()
{
    _data.deleteContents();
    _batcher->resetContents();
    _maxLength = 0;
}

void VMsgLog::addLine( const WString& str, bool newline )
{
    if( str.match( "cd *" ) ) {
        if( str == lastCD ) {
            return;
        }
        lastCD = str;
    }
//drw    size_t length = str.size();
//drw    if( length > _maxLength ) {
//drw   _maxLength = length;
//drw   _batcher->setExtent( _batcher->getTextExtentX( str ) + WSystemMetrics::vScrollBarWidth() );
//drw    }
    if( _data.count() > 999 ) {
        delete _data.removeAt( 0 );
        _batcher->deleteString( 0 );
    }
    WString* s = new WString( str );
    if( !newline ) {
        int dcount = _data.count();
        if( dcount > 0 ) {
            delete _data.removeAt( dcount - 1 );
        }
        int bcount = _batcher->count();
        if( bcount > 0 ) {
            _batcher->deleteString( bcount - 1 );
        }
    }
    _data.add( s );
    _batcher->insertString( *s );
    _batcher->select( _batcher->count() -1 );
//    _batcher->update();
}

bool VMsgLog::keyDown( WKeyCode kc, WKeyState ks )
{
    if( !WMdiChild::keyDown( kc, ks ) ) {
        return( _batcher->keyDown( kc, ks ) );
    } else {
        return( true );
    }
}

// forward notification to the child control
bool VMsgLog::scrollNotify( WScrollNotification sn, int diff )
{
    return( _batcher->scrollNotify( sn, diff ) );
}

