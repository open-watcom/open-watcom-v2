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


#include <errno.h>
#include <wclist.h>
#include <wclistit.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <ctype.h>

#include "assure.h"
#include "chbffile.h"
#include "cmdparse.h"
#include "util.h"

enum TokenVal {
    TOK_EOF         = '\0',
    TOK_Id,
    TOK_OpenBrace   = '{',
    TOK_CloseBrace  = '}',
    TOK_Comma       = ',',
    TOK_At          = '@',
    TOK_NotATok
};

typedef struct fullName {
    char        path[ _MAX_PATH + 1 ];
    char        drive[ _MAX_DRIVE + 1 ];
    char        dir[ _MAX_DIR + 1 ];
    char        fname[ _MAX_FNAME + 1 ];
    char        ext[ _MAX_EXT + 1 ];
} FullName;

CommandParser::CommandParser( char * cmdLine, bool optAllowed )
                : _cmdLine( cmdLine )
                , _command( NULL )
                , _database( "" )
                , _options( "" )
                , _files( NULL )
                , _optAllowed( optAllowed )
                , _quiet( FALSE )
//-------------------------------------------------------------
{
    try {
        _files = new WCValSList<String>;

        parse();
    } catch( FileExcept oops ) {
        errMessage( "%s: %s", oops._fileName, oops._message );
    }
}

CommandParser::~CommandParser()
//-----------------------------
{
    delete _files;
    delete _command;
}

void CommandParser::setCommand( const char * name )
//-------------------------------------------------
{
    String nameStr( name );

    if( _command ) {
        errMessage( "Warning:  ignoring nested include @%s.", name );
    } else {
        setExtension( nameStr, ".cbr" );
        _command = new CheckedBufferedFile( nameStr );
        _command->open( CheckedFile::ReadText, CheckedFile::UserReadWrite );

        if( *_cmdLine ) {
            _cmdLine--;
        }
        getNextChar();
    }
}

void CommandParser::setDatabase( const char * dbs )
//-------------------------------------------------
{
    _database = dbs;
    setExtension( _database, ".dbr" );
}

void CommandParser::setSearchPath( const char * path )
//---------------------------------------------------
{
    _searchPath = path;
}

void CommandParser::setOptions( const char * opt )
//------------------------------------------------
{
    _options = opt;
    setExtension( _options, ".obr" );
}


const char * CommandParser::database()
//------------------------------------
{
    if( _database.length() > 0 ) {
        return _database;
    } else {
        return NULL;
    }
}


const char * CommandParser::searchPath()
//--------------------------------------
{
    if( _searchPath.length() > 0 ) {
        return _searchPath;
    } else {
        return NULL;
    }
}

const char * CommandParser::options()
//-----------------------------------
{
    if( _options.length() > 0 ) {
        return _options;
    } else {
        return NULL;
    }
}

bool CommandParser::quiet()
//-------------------------
{
    return _quiet;
}

void CommandParser::parse()
//-------------------------
{
    int token;

    getNextChar();
    token = scan();
    while( token != TOK_EOF ) {
        if( token != TOK_Id  && token != '@' ) {
            syntaxError();
            break;
        }

        if( token == '@' ) {
            token = scan();
            if( token != TOK_Id ) {
                errMessage( "Missing command file name." );
                break;
            } else {
                setCommand( _token );
                token = scan();
                continue;       // <----------- unusual flow
            }
        }

        if( !strncmp( "database", _token, _token.length() ) ) {
            token = scan();
            if( token != TOK_Id ) {
                errMessage( "Missing database file name." );
                break;
            } else {
                setDatabase( _token );
                token = scan();
                continue;       // <----------- unusual flow
            }
        }

        if( !strncmp( "quiet", _token, _token.length() ) ) {
            _quiet = TRUE;
            token = scan();
            continue;           // <----------- unusual flow
        }

        if( !strncmp( "path", _token, _token.length() ) ) {
            token = scan();
            if( token != TOK_Id ) {
                errMessage( "Missing file search path." );
                break;
            } else {
                setSearchPath( _token );
                token = scan();
                continue;       // <----------- unusual flow
            }
        }

        if( !strncmp( "options", _token, _token.length() ) ) {
            token = scan();
            if( token != TOK_Id ) {
                errMessage( "Missing options file name." );
                break;
            } else {
                setOptions( _token );
                token = scan();
                continue;       // <----------- unusual flow
            }
        }

        if( !strncmp( "file", _token, _token.length() ) ) {
            bool error = FALSE;

            token = scan();
            switch( token ) {
            case '{':
                token = scan();
                while( token == TOK_Id ) {
                    addFile( _token );
                    token = scan();
                }
                if( token != '}' ) {
                    syntaxError();
                    error = TRUE;
                } else {
                    token = scan();
                }
                break;
            case TOK_Id:
                addFile( _token );
                token = scan();
                while( token == ',' ) {
                    token = scan();
                    if( token != TOK_Id ) {
                        syntaxError();
                        error = TRUE;
                        break;
                    } else {
                        addFile( _token );
                        token = scan();
                    }
                }
                break;
            default:
                syntaxError();
                error = TRUE;
            }

            if( error ) {
                break;
            } else {
                continue;
            }
        }

        // the directive is not database, option, or file
        syntaxError();
        break;
    }
}

void CommandParser::syntaxError()
//-------------------------------
{
    if( _token.length() ) {
        errMessage( "Syntax error near \"%s\".", (const char *) _token );
    } else {
        errMessage( "Syntax error near end of input." );
    }
}

int CommandParser::scan()
//-----------------------
{
    int     state = TOK_NotATok;
    char    buf[ 2 ] = { 0 };

    _token = "";

    while( 1 ) {
        switch( _lookAhead ) {
        case '{':
        case '}':
        case ',':
        case '@':
        case '\0':
            if( state == TOK_NotATok ) {
                state = _lookAhead;
                buf[ 0 ] = _lookAhead;
                _token = buf;
                getNextChar();
            }
            return state;
        default:
            if( isspace( _lookAhead ) ) {
                getNextChar();
                if( state != TOK_NotATok ) {
                    return state;
                }
            } else {
                if( _lookAhead != '\0' ) {
                    state = TOK_Id;
                    _token += _lookAhead;
                    getNextChar();
                } else {
                    return state;
                }
            }
        }
    }
}

bool ListContains( WCValSList<String> * list, const String & str )
//----------------------------------------------------------------
{
    WCValSListIter<String> iter( *list );

    while( ++iter ) {
        if( iter.current() == str ) {
            return TRUE;
        }
    }

    return FALSE;
}

void CommandParser::addFile( const char * fname )
//-----------------------------------------------
// add all the files specified by the (possibly wild-carded) option
// return TRUE if successful, FALSE on error
{
    DIR *           dirp;
    struct dirent * direntp;
    String          file( fname );
    FullName        dirName;

    setExtension( file, ".mbr" );

    // FIXME -- these error messages have to do something smarter!
    if( strchr( file, '?' ) != NULL | strchr( file, '*' ) != NULL ) {

        _splitpath( file, dirName.drive, dirName.dir,
                    dirName.fname, dirName.ext );

        dirp = opendir( file );
        if( dirp == NULL ) {
            errMessage( "No files found matching \"%s\".", fname );
            return;
        }

        for(;;) {
            const char AvoidAttribs = _A_HIDDEN | _A_SYSTEM | _A_VOLID |
                                      _A_SUBDIR;

            direntp = readdir( dirp );
            if( direntp == NULL ) break;

            if( (dirp->d_attr & AvoidAttribs) == 0 ) {
                String addFile;
                addFile = dirName.drive;
                addFile += dirName.dir;
                addFile += dirp->d_name;

                // do NOT do a setExtension here as we only get here
                // if user did *.* or *. something.
                // setExtension( addFile, ".mbr" );

                if( !ListContains( _files, addFile ) ) {
                    _files->append( addFile );
                }
            }
        }

        if( closedir( dirp ) ){
            FileExcept oops( FileExcept::Close, errno, file );
            throw oops;
        }
    } else {
        if( !ListContains( _files, file ) ) {
            _files->append( file );
        }
    }

    return;
}

void CommandParser::setExtension( String & str, const char * ext )
//----------------------------------------------------------------
// split a path apart, set its extension to ext if it is blank,
// stick it back together and return the full path name.
{
    FullName    name;
    char        newDir[ _MAX_PATH ];
    char *      res;
    String      retStr;

    _splitpath( str, name.drive, name.dir, name.fname, name.ext );

    if( *name.ext == '\0' ) {
        strcpy( name.ext, ext );
    }

    _makepath( name.path, name.drive, name.dir, name.fname, name.ext );

    res = _fullpath( newDir, name.path, _MAX_PATH );

    if( res == NULL ) {
        FileExcept oops( FileExcept::Read, errno, str );
        throw( oops );
    }

    str = newDir;
}

char CommandParser::getNextChar()
//-------------------------------
{
    int     nRead;

    if( _command == NULL ) {
        if( *_cmdLine == '\0' ) {
            _lookAhead = '\0';
        } else {
            _lookAhead = *_cmdLine++;
        }
    } else {
        nRead = _command->read( &_lookAhead, sizeof( char ) );
        if( nRead == 0 ) {
            _command->close();
            delete _command;
            _command = NULL;
            getNextChar();
        }
    }

    return _lookAhead;
}

#ifdef DEBUG_CMDPARSE

#include <stdio.h>
#include <process.h>

void main()
{
    char                    buf[ 512 ];
    CommandParser           prs( getcmd( buf ), TRUE );
    int                     i;
    WCValSListIter<String>  iter( *prs.files() );

    printf( "    database = \"%s\"\n", prs.database() ? prs.database() : "<NULL>" );
    printf( "    options  = \"%s\"\n", prs.options()  ? prs.options()  : "<NULL>" );
    printf( "    files  = \n" );

    while( ++ iter ) {
        printf( "        %s\n", (const char *) iter.current() );
    }

    printf( "--------------" );
}

#endif
