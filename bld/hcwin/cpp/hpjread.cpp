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
* Description:  Parsing of .hpj files.
*
****************************************************************************/


#include <stdlib.h>
#include <ctype.h>
#ifdef __UNIX__
#include <dirent.h>
#include <unistd.h>
#else
#include <direct.h>
#endif
#include "hpjread.h"
#include "hcerrors.h"
#include "parsing.h"
#include "topic.h"
#include "phrase.h"
#include "bmx.h"
#include "clibext.h"

#define NB_FILES    10
#define LINE_BLOCK  128


char const  HpjExt[] = ".HPJ";

// Other string resources.
static char const   SBaggage[]  = "BAGGAGE";
static char const   SOptions[]  = "OPTIONS";
static char const   SConfig[]   = "CONFIG";
static char const   SFiles[]    = "FILES";
static char const   SMap[]      = "MAP";
static char const   SBitmaps[]  = "BITMAPS";
static char const   SWindows[]  = "WINDOWS";
static char const   STitle[]    = "TITLE";
static char const   SCopyright[]    = "COPYRIGHT";
static char const   SCompress[] = "COMPRESS";
static char const   STrue[]     = "TRUE";
static char const   SHigh[]     = "HIGH";
static char const   SMedium[]   = "MEDIUM";
static char const   SYes[]      = "YES";
static char const   SFalse[]    = "FALSE";
static char const   SLow[]      = "LOW";
static char const   SNo[]       = "NO";
static char const   SOldKeyPhrase[] = "OLDKEYPHRASE";
static char const   SContents[] = "CONTENTS";
static char const   SIndex[]    = "INDEX";
static char const   SBmRoot[]   = "BMROOT";
static char const   SRoot[]     = "ROOT";
static char const   Sinclude[]  = "#include";
static char const   Sdefine[]   = "#define";
static char const   Smain[]     = "main";
static char const   Ssecondary[]    = "secondary";
static char const   SstartComment[] = "/*";
static char const   SendComment[]   = "*/";



//  C-tor and D-tor for class HPJScanner

HPJScanner::HPJScanner( InFile *src )
    : _input( src )
{
    _lineNum = 0;
    if( !_input->bad() ) {
        _curLine = new char[LINE_BLOCK];
        _lineSize = LINE_BLOCK;
    } else {
        _curLine = NULL;
    }
}

HPJScanner::~HPJScanner()
{
    if( _curLine ) {
        delete[] _curLine;
    }
}



//  HPJScanner::open    --Initialize the parser on a given filename.

int HPJScanner::open( char const filename[] )
{
    int result = _input->open( filename, File::READ|File::TEXT );
    if( !result ) {
        if( _curLine == NULL ) {
            _curLine = new char[120];   // Overflow possibility
        }
    }
    return result;
}


//  HPJScanner::getLine --Read a single line of text into _curLine.
//                         Returns the line length (0 in case of failure).

int HPJScanner::getLine()
{
    int     current;
    int     has_text;
    int     cur_len=0;

    // Loop until we've identified a single line.
    while( cur_len == 0 ) {
        ++_lineNum;
        current = 0;
        cur_len = 0;
        has_text = 0;
        for( ;; ) {
            current = _input->nextch();
            if( current == ';' && cur_len == 0 )
                break;
            if( current == EOF || current == '\n' )
                break;
            if( !isspace( current ) ) {
                has_text = 1;
            }
    
            if( cur_len == _lineSize ) {
                _lineSize += LINE_BLOCK;
                renew( _curLine, _lineSize );
            }
    
            _curLine[cur_len++] = (char)current;
        }
        if( current == ';' && cur_len == 0 ) {
            do {
                current = _input->nextch();
            } while( current != EOF && current != '\n' );
            if( cur_len == 0 ) {
                continue;
            }
        }
        if( current != EOF || cur_len > 0 ) {
            if( cur_len == _lineSize ) {
                _lineSize += LINE_BLOCK;
                renew( _curLine, _lineSize );
            }
            _curLine[cur_len++] = '\0';
        } else {
            break;
        }
        if( !has_text ) {
            cur_len = 0;
        }
    }

    // Set up the buffer so this line can be tokenized.
    if( cur_len > 0 ) {
        _bufPos = 0;
        _bufChar = _curLine[0];
    }
    return cur_len;
}


//  HPJScanner::getArg  --Read a "= <string>" argument from the .HPJ file.

char *HPJScanner::getArg( int start_pos )
{
    int     i;

    // Eat whitespace.
    for( i = start_pos; isspace( _curLine[i] ) ; i++ ) {
        if( _curLine[i] == '\0' ) {
            break;
        }
    }

    // The next character had better be an '='.
    if( _curLine[i] != '=' ) {
        HCWarning( HPJ_NOARG, _lineNum, name() );
        return NULL;
    }
    i++;

    // Eat whitespace again.
    while( isspace( _curLine[i] ) && _curLine[i] != '\0' ) {
        i++;
    }

    return _curLine+i;
}


//  HPJScanner::tokLine --Tokenize a line, like the strtok() function.

char *HPJScanner::tokLine()
{
    int i,j;
    _curLine[_bufPos] = _bufChar;

    // Find the beginning of the token.
    for( i=_bufPos; _curLine[i] != '\0' && isspace( _curLine[i] ); ++i) {
    }   // empty

    if( _curLine[i] == '\0' )
        return NULL;

    // Find the end of the token.
    for( j=i; _curLine[j] != '\0' && !isspace( _curLine[j] ); ++j) {
    }   // empty

    _bufPos = j;
    _bufChar = _curLine[j];
    _curLine[j] = '\0';
    return _curLine+i;
}


//  HPJScanner::endTok  --Stop tokenizing and get the rest of the line.

char *HPJScanner::endTok()
{
    _curLine[_bufPos] = _bufChar;
    return _curLine+_bufPos;
}


HPJReader::StrNode  *HPJReader::_topFile    = NULL;
HPJReader::StrNode  *HPJReader::_curFile    = NULL;
HPJReader::StrNode  *HPJReader::_firstDir   = NULL;
char const          *HPJReader::_startDir   = NULL;


//  HPJReader::HPJReader

HPJReader::HPJReader( HFSDirectory * d_file, Pointers *other_files,
              InFile *input )
    : _scanner( input )
{
    _dir = d_file;
    _theFiles = other_files;
    _sysFile = other_files->_sysFile;
    _numBagFiles = 0;
    _rtfFiles = _root = NULL;
    _homeDir = new char[_MAX_PATH];
    getcwd( _homeDir, _MAX_PATH );
    if( input->bad() ) {
        _bagFiles = NULL;
    } else {
        _bagFiles = new Baggage*[NB_FILES];
    }
    _oldPhrases = 0;
}


//  HPJReader::~HPJReader

HPJReader::~HPJReader()
{
    delete[] _homeDir;
    StrNode *current, *temp;
    current = _root;
    while( current != NULL ) {
        temp = current;
        current = current->_next;
        delete[] temp->_name;
        delete temp;
    }
    current = _rtfFiles;
    while( current != NULL ) {
        temp = current;
        current = current->_next;
        delete[] temp->_name;
        delete temp;
    }

    if( _bagFiles ) {
        for( int i=0; i<_numBagFiles; i++ ) {
            delete _bagFiles[i];
        }
        delete[] _bagFiles;
    }
}


//  HPJReader::firstFile    --Callback function for the phrase handler.

InFile *HPJReader::firstFile()
{
    _curFile = _topFile;
    return nextFile();
}


//  HPJReader::nextFile --Callback function for the phrase handler.

InFile *HPJReader::nextFile()
{
    static InFile   result;
    StrNode     *curdir;

    do {
        if( _curFile == NULL ) {
            return NULL;
        }
    
        curdir = _firstDir;
        if( curdir == NULL ) {
            result.open( _curFile->_name );
        } else {
            while( curdir != NULL ) {
                chdir( curdir->_name );
                result.open( _curFile->_name );
                chdir( _startDir );
                if( !result.bad() )
                    break;
                curdir = curdir->_next;
            }
        }
        _curFile = _curFile->_next;
    } while( result.bad() );

    return &result;
}


//  HPJReader::parseFile --Parse an .HPJ file, and call .RTF parsers
//                          as appropriate.

void HPJReader::parseFile()
{
    HCStartFile( _scanner.name() );

    int     length = _scanner.getLine();    // Get the first line.
    char    section[15];
    int     i;
    while( length != 0 ) {
        // The first line had better be the beginning of a section.
        if( _scanner[0] != '[' ) {
            HCWarning( HPJ_NOTSECTION, _scanner.lineNum(), _scanner.name() );
            length = skipSection();
            continue;
        }
    
        // Read in the name of the section.
        for( i=1; i < length ; i++ ) {
            if( _scanner[i] == ']' ) break;
            section[i - 1] = (char)toupper( _scanner[i] );
        }
    
        // If the section name wasn't terminated properly, skip the section.
        if( i == length ) {
            HCWarning( HPJ_BADSECTION, _scanner.lineNum(), _scanner.name() );
            length = skipSection();
            continue;
        }
        section[i - 1] = '\0';
    
        // Pass control to the appropriate "section handler".
        if( strcmp( section, SBaggage ) == 0 ) {
            length = handleBaggage();
        } else if( strcmp( section, SOptions ) == 0 ) {
            length = handleOptions();
        } else if( strcmp( section, SConfig ) == 0 ) {
            length = handleConfig();
        } else if( strcmp( section, SFiles ) == 0 ) {
            length = handleFiles();
        } else if( strcmp( section, SMap ) == 0 ) {
            length = handleMap();
        } else if( strcmp( section, SBitmaps ) == 0 ) {
            length = handleBitmaps();
        } else if( strcmp( section, SWindows ) == 0 ) {
            length = handleWindows();
        } else {
            HCWarning( HPJ_BADSECTION, _scanner.lineNum(), _scanner.name() );
            length = skipSection();
        }
    }

    if( _rtfFiles == NULL ) {
        HCError( HPJ_NOFILES );
    }

    // Now parse individual RTF files.
    StrNode *curfile = _rtfFiles;
    StrNode *curdir;
    InFile  source;

    // First, implement phrase replacement if desired.
    if( _theFiles->_sysFile->isCompressed() ) {
        _topFile = _rtfFiles;
        _firstDir = _root;
        _startDir = _homeDir;
    
        _theFiles->_phrFile = new HFPhrases( _dir, &firstFile, &nextFile );
    
        char    full_path[_MAX_PATH];
        char    drive[_MAX_DRIVE];
        char    dir[_MAX_DIR];
        char    fname[_MAX_FNAME];
        char    ext[_MAX_EXT];
    
        _fullpath( full_path, _scanner.name(), _MAX_PATH );
        _splitpath( full_path, drive, dir, fname, ext );
        _makepath( full_path, drive, dir, fname, PhExt );
    
        if( !_oldPhrases || !_theFiles->_phrFile->oldTable(full_path) ) {
            _theFiles->_phrFile->readPhrases();
            _theFiles->_phrFile->createQueue( full_path );
        }
    }

    _theFiles->_topFile = new HFTopic( _dir, _theFiles->_phrFile );

    // For each file, search the ROOT path, and create a RTFparser
    // to deal with it.
    curfile = _rtfFiles;
    while( curfile != NULL ) {
        curdir = _root;
        if( curdir == NULL ) {
            source.open( curfile->_name );
        } else while( curdir != NULL ) {
            chdir( curdir->_name );
            source.open( curfile->_name );
            chdir( _homeDir );
            if( !source.bad() ) break;
            curdir = curdir->_next;
        }
        if( source.bad() ) {
            HCWarning( FILE_ERR, curfile->_name );
        } else {
            RTFparser rtfhandler( _theFiles, &source );
            rtfhandler.Go();
            source.close();
        }
        curfile = curfile->_next;
    }
}


//  HPJReader::skipSection --Jump to the next section header.

int HPJReader::skipSection()
{
    int result;
    do {
        result = _scanner.getLine();
    } while( result != 0 && _scanner[0] != '[' );
    return result;
}


//  HPJReader::handleBaggage --Create baggage files.

int HPJReader::handleBaggage()
{
    int result = 0;

    while( _numBagFiles < NB_FILES ) {
        result = _scanner.getLine();
        if( !result )
            break;
        if( _scanner[0] == '[' )
            break;
        _bagFiles[_numBagFiles++] = new Baggage( _dir, _scanner );
    }
    return result;
}


//  HPJReader::handleOptions --Parse the [OPTIONS] section.

#define MAX_OPTION_LEN  12

int HPJReader::handleOptions()
{
    int result;
    char    option[MAX_OPTION_LEN + 1];
    char    *arg;
    int     i;
    for( ;; ) {
        result = _scanner.getLine();
        if( !result || _scanner[0] == '[' )
            break;
    
        // Read in the name of the option.
        for( i=0; i<MAX_OPTION_LEN; i++ ) {
            if( isspace( _scanner[i] ) || _scanner[i] == '=' )
                break;
            option[i] = (char)toupper( _scanner[i] );
        }
        option[i] = '\0';
    
        // At present, I only support a few options.
        // Most of these involve passing information to
        // the HFSystem object "_sysFile".
        if( strcmp( option, STitle ) == 0 ) {
            arg = _scanner.getArg( i );
            if( arg != NULL ) {
                _sysFile->addRecord( new SystemText( HFSystem::SYS_TITLE, arg ) );
            }
        } else if( strcmp( option, SCopyright ) == 0 ) {
            arg = _scanner.getArg( i );
            if( arg != NULL ) {
                _sysFile->addRecord( new SystemText( HFSystem::SYS_COPYRIGHT, arg ) );
            }
        } else if( strcmp( option, SCompress ) == 0 ) {
            arg = _scanner.getArg( i );
            if( arg != NULL ) {
                if( stricmp( arg, STrue ) == 0 ||
                    stricmp( arg, SHigh ) == 0 ||
                    stricmp( arg, SMedium ) == 0 ||
                    stricmp( arg, SYes  ) == 0 ) {
                    _sysFile->setCompress( 1 );
                } else if( stricmp( arg, SFalse ) == 0 ||
                           stricmp( arg, SLow   ) == 0 ||
                       stricmp( arg, SNo    ) == 0 ) {
                    _sysFile->setCompress( 0 );
                }
            }
        } else if( strcmp( option, SOldKeyPhrase ) == 0 ) {
            arg = _scanner.getArg( i );
            if( arg != NULL ) {
                if( stricmp( arg, STrue ) == 0 ||
                    stricmp( arg, SYes  ) == 0 ) {
                    _oldPhrases = 1;
                } else {
                    _oldPhrases = 0;
                }
            }
        } else if( strcmp( option, SContents ) == 0 || strcmp( option, SIndex    ) == 0 ) {
            arg = _scanner.getArg( i );
            if( arg != NULL ) {
                _sysFile->setContents( Hash( arg ) );
            }
        } else if( strcmp( option, SBmRoot ) == 0 ) {
            arg = _scanner.getArg( i );
            _theFiles->_bitFiles->addToPath( arg );
        } else if( strcmp( option, SRoot ) == 0 ) {
    
            // Update the search paths.
            arg = _scanner.getArg( i );
            StrNode *current;
            current = _root;
            if( current != NULL ) {
                while( current->_next != NULL ) {
                    current = current->_next;
                }
            }
    
            // There may be many directories specified on each line.
            if( arg != NULL ) {
                int j;
                StrNode *temp;
                while( arg[0] != '\0' ) {
                    j = 0;
                    while( arg[j] != '\0' && arg[j] != ',' && arg[j] != ';' ) {
                        ++j;
                    }
                    temp = new StrNode;
                    temp->_name = new char[j + 1];
                    strncpy( temp->_name, arg, j );
                    temp->_name[j] = '\0';
                    temp->_next = NULL;
                    if( chdir( temp->_name ) == 0 ) {
                        chdir( _homeDir );
                        if( current == NULL ) {
                            _root = temp;
                            current = _root;
                        } else {
                            current->_next = temp;
                            current = current->_next;
                        }
                    } else {
                        HCWarning( HPJ_BADDIR, temp->_name );
                        delete[] temp->_name;
                        delete temp;
                    }
                    arg += j;
                    if( arg[0] != '\0' ) {
                        ++arg;
                    }
                }
            }
        }
    }
    return result;
}


//  HPJReader::handleConfig --Parse the [CONFIG] section (where macros
//                             are kept).

int HPJReader::handleConfig()
{
    int result;
    for( ;; ) {
        result = _scanner.getLine();
        if( !result || _scanner[0] == '[' )
            break;
        _sysFile->addRecord( new SystemText( HFSystem::SYS_MACRO, _scanner ) );
    }
    return result;
}


//  HPJReader::handleFiles --Parse the [FILES] section.

int HPJReader::handleFiles()
{
    int result;
    int i;
    StrNode *current = _rtfFiles;
    StrNode *temp;
    if( current != NULL ) {
        while( current->_next != NULL ) {
            current = current->_next;
        }
    }
    for( ;; ) {
        result = _scanner.getLine();
        if( !result || _scanner[0] == '[' )
            break;
        for( i=0; _scanner[i] != '\0'; ++i ) {
            if( isspace( _scanner[i] ) ) {
                break;
            }
        }
        _scanner[i] = '\0';
        temp = new StrNode;
        temp->_name = new char[i + 1];
        strncpy( temp->_name, _scanner, i+1 );
        temp->_next = NULL;
        if( current == NULL ) {
            current = _rtfFiles = temp;
        } else {
            current = current->_next = temp;
        }
    }
    return result;
}


//  HPJReader::handleBitmaps    --Parse the [BITMAPS] section.

int HPJReader::handleBitmaps()
{
    int result;
    int i;
    for( ;; ) {
        result = _scanner.getLine();
        if( !result || _scanner[0] == '[' )
            break;
        for( i=0; _scanner[i] != '\0'; i++ ) {
            if( !isspace( _scanner[i] ) ) {
                break;
            }
        }
        if( _scanner[i] == '\0' )
            continue;
        try{
            _theFiles->_bitFiles->note( &_scanner[i] );
        } catch( HFBitmaps::ImageNotSupported ) {
            HCWarning( UNKNOWN_IMAGE, &_scanner[0], _scanner.lineNum(), _scanner.name() );
        }
    }
    return result;
}


//  HPJReader::handleWindows    --Parse the [WINDOWS] section.
#define VALID_TYPE  0x0001
#define VALID_NAME  0x0002
#define VALID_CAPTION   0x0004
#define VALID_X     0x0008
#define VALID_Y     0x0010
#define VALID_WIDTH 0x0020
#define VALID_HEIGHT    0x0040
#define VALID_MAX   0x0080
#define VALID_RGB1  0x0100
#define VALID_RGB2  0x0200
#define VALID_ONTOP 0x0400

#define PARAM_MAX   1023

int HPJReader::handleWindows()
{
    int     result;
    int     i, limit;
    char    *arg;
    int     bad_param;
    int     red, green, blue;
    uint_16 wflags;
    char    name[HLP_SYS_NAME];
    char    caption[HLP_SYS_CAP];
    uint_16 x = 0;
    uint_16 y = 0;
    uint_16 width = 0;
    uint_16 height = 0;
    uint_16 use_max_flag = 0;
    uint_32 rgb_main, rgb_nonscroll;

    for( ; (result = _scanner.getLine()) != 0; ) {
        if( _scanner[0] == '[' )
            break;

        limit = HLP_SYS_NAME-1;
        if( limit > result-1 ) {
            limit = result-1;
        }
        for( i=0; i<limit && !isspace(_scanner[i]) && _scanner[i] != '=' ; i++ ) {
            name[i] = _scanner[i];
        }
        if( i == result - 1 ) {
            HCWarning( HPJ_INCOMPLETEWIN, _scanner.lineNum(), _scanner.name() );
            continue;
        } else if( i == HLP_SYS_NAME ) {
            HCWarning( HPJ_LONGWINNAME, _scanner.lineNum(), _scanner.name() );
        }
        name[i] = '\0';
        while( i<result-1 && !isspace(_scanner[i]) ) {
            i++;
        }

        arg = _scanner.getArg(i);
        if( arg == NULL || *arg == '\0' ) {
            HCWarning( HPJ_INCOMPLETEWIN, _scanner.lineNum(), _scanner.name() );
            continue;
        }

        wflags = VALID_TYPE | VALID_NAME;
        _winParamBuf = arg;

        arg = nextWinParam();
        if( *arg != '\0' ) {
            i = 0;
            if( *arg == '"' ) {
                arg++;
            }
            while( i<HLP_SYS_CAP-1 && *arg != '\0' && *arg != '"' ) {
                caption[i++] = *arg++;
            }
            caption[i] = '\0';
            wflags |= VALID_CAPTION;
        }

        bad_param = 0;
        arg = nextWinParam();
        if( *arg != '\0' ) {
            x = (uint_16) strtol( arg, NULL, 0 );
            if( x > PARAM_MAX ) {
                bad_param = 1;
            } else {
                wflags |= VALID_X;
            }
        }
        arg = nextWinParam();
        if( *arg != '\0' ) {
            y = (uint_16) strtol( arg, NULL, 0 );
            if( y > PARAM_MAX ) {
                bad_param = 1;
            } else {
                wflags |= VALID_Y;
            }
        }
        arg = nextWinParam();
        if( *arg != '\0' ) {
            width = (uint_16) strtol( arg, NULL, 0 );
            if( width > PARAM_MAX ) {
                bad_param = 1;
            } else {
                wflags |= VALID_WIDTH;
            }
        }
        arg = nextWinParam();
        if( *arg != '\0' ) {
            height = (uint_16) strtol( arg, NULL, 0 );
            if( height > PARAM_MAX ) {
                bad_param = 1;
            } else {
                wflags |= VALID_HEIGHT;
            }
        }
        if( bad_param ) {
            HCWarning( HPJ_WINBADPARAM, _scanner.lineNum(), _scanner.name() );
            continue;
        }

        arg = nextWinParam();
        if( *arg != '\0' ) {
            use_max_flag = (uint_16) strtol( arg, NULL, 0 );
            wflags |= VALID_MAX;
        }

        red = green = blue = 0;
        bad_param = 0;

        arg = nextWinParam();
        if( *arg != '\0' ) {
            red = strtol( arg, NULL, 0 );
            if( red < 0 || red > 255 ) {
                bad_param = 1;
            }
            wflags |= VALID_RGB1;
        }
        arg = nextWinParam();
        if( *arg != '\0' ) {
            green = strtol( arg, NULL, 0 );
            if( green < 0 || green > 255 ) {
                bad_param = 1;
            }
            wflags |= VALID_RGB1;
        }
        arg = nextWinParam();
        if( *arg != '\0' ) {
            blue = strtol( arg, NULL, 0 );
            if( blue < 0 || blue > 255 ) {
                bad_param = 1;
            }
            wflags |= VALID_RGB1;
        }

        if( bad_param ) {
            HCWarning( HPJ_WINBADCOLOR, _scanner.lineNum(), _scanner.name() );
            continue;
        } else {
            rgb_main = (uint_32) (red + (green<<8) + (blue<<16));
        }

        red = green = blue = 0;
        bad_param = 0;

        arg = nextWinParam();
        if( *arg != '\0' ) {
            red = strtol( arg, NULL, 0 );
            if( red < 0 || red > 255 ) {
                bad_param = 1;
            }
            wflags |= VALID_RGB2;
        }
        arg = nextWinParam();
        if( *arg != '\0' ) {
            green = strtol( arg, NULL, 0 );
            if( green < 0 || green > 255 ) {
                bad_param = 1;
            }
            wflags |= VALID_RGB2;
        }
        arg = nextWinParam();
        if( *arg != '\0' ) {
            blue = strtol( arg, NULL, 0 );
            if( blue < 0 || blue > 255 ) {
                bad_param = 1;
            }
            wflags |= VALID_RGB2;
        }

        if( bad_param ) {
            HCWarning( HPJ_WINBADCOLOR, _scanner.lineNum(), _scanner.name() );
            continue;
        } else {
            rgb_nonscroll = (uint_32) (red + (green<<8) + (blue<<16));
        }

        arg = nextWinParam();
        if( *arg != 0 || strtol( arg, NULL, 0 ) != 0 ) {
            wflags |= VALID_ONTOP;
        }

        if( strcmp( name, Smain ) == 0 ) {
            _sysFile->addRecord( new SystemWin( wflags, Smain, name, caption,
                            x, y, width, height, use_max_flag,
                        rgb_main, rgb_nonscroll ) );
        } else {
            _sysFile->addRecord( new SystemWin( wflags, Ssecondary, name, caption,
                            x, y, width, height, use_max_flag,
                        rgb_main, rgb_nonscroll ) );
        }
    }
    return result;
}


//  HPJReader::nextWinParam --Helper function for handleWindows.

char *HPJReader::nextWinParam()
{
    char * result = _winParamBuf;
    char * newbuf;

    if( *result != '\0' ) {
        while( isspace( *result ) ) {
            result++;
        }
        newbuf = result;
        if( *newbuf == '"' ) {
            while( *newbuf != ',' && *newbuf != '\0' ) {
                newbuf++;
            }
        } else {
            while( *newbuf != ',' && *newbuf != '\0' ) {
                if( *newbuf == '(' || *newbuf == ')' ) {
                    *newbuf = ' ';
                }
                newbuf++;
            }
            while( isspace( *result ) ) {
                result++;
            }
        }
        if( *newbuf == ',' ) {
            *newbuf = '\0';
            newbuf++;
        }
        _winParamBuf = newbuf;
    }
    return result;
}


//  HPJReader::handleMap --Parse the [MAP] section.

int HPJReader::handleMap()
{
    int     result;
    char    *token;
    uint_32 hash_value;
    int     con_num;
    int     is_good_string, i;
    for( ;; ) {
        result = _scanner.getLine();
        if( !result || _scanner[0] == '[' )
            break;
        token = _scanner.tokLine();
        if( token == NULL )
            continue;
    
        // "#include" means go to another file.
        if( stricmp( token, Sinclude ) == 0 ) {
            includeMapFile( _scanner.endTok() );
            continue;
        }
    
        // "#define" is an optional header, ignore it.
        if( stricmp( token, Sdefine ) == 0 ) {
            token = _scanner.tokLine();
        }
        if( token == NULL )
            continue;
    
        // verify that the current token at this point is a context string.
        is_good_string = 1;
        for( i=0; token[i] != '\0'; ++i ) {
            if( !isalnum( token[i] ) && token[i] != '.' && token[i] != '_' ) {
                is_good_string = 0;
            }
        }
        if( !is_good_string ) {
            HCWarning( CON_BAD, token, _scanner.lineNum(), _scanner.name() );
        } else {
            // Associate the context string with a context number.
            hash_value = Hash(token);
            token = _scanner.tokLine();
            if( token == NULL ) {
                HCWarning( CON_NONUM, token, _scanner.lineNum(), _scanner.name() );
            } else {
                con_num = atol( token );
                _theFiles->_mapFile->addMapRec( con_num, hash_value );
            }
        }
    }
    return result;
}


//  HPJReader::includeMapFile   --Parse an #include-d MAP file.

void HPJReader::includeMapFile( char i_str[] )
{
    int i = 0;
    char seek_char;

    // Get the filename.
    while( i_str[i] != '\0' && isspace( i_str[i] ) ) {
        ++i;
    }
    switch( i_str[i] ) {
    case '"':
        seek_char = '"';
        break;
    case '<':
        seek_char = '>';
        break;
    default:
        HCWarning( HPJ_BADINCLUDE, _scanner.lineNum(), _scanner.name() );
        return;
    }

    ++i;
    int j = i;
    while( i_str[j] != '\0' && i_str[j] != seek_char ) {
        ++j;
    }
    if( j == '\0' ) {
        HCWarning( HPJ_BADINCLUDE, _scanner.lineNum(), _scanner.name() );
        return;
    }

    // Now try to find it in the ROOT path and/or current directory.
    i_str[j] = '\0';
    StrNode *current = _root;
    InFile  source;
    if( current == NULL ) {
        source.open( i_str+i );
    } else while( current != NULL ) {
        chdir( current->_name );
        source.open( i_str+i );
        chdir( _homeDir );
        if( !source.bad() )
            break;
        current = current->_next;
    }

    if( source.bad() ) {
        HCWarning(INCLUDE_ERR, (const char *)(i_str+i), _scanner.lineNum(), _scanner.name() );
        return;
    }

    HCStartFile( i_str+i );

    // Now parse the secondary file.
    HPJScanner  input( &source );
    int     not_done;
    char    *token;
    int     is_good_str, con_num;
    uint_32 hash_value;
    for( ;; ) {
        not_done = input.getLine();
        if( !not_done )
            break;
    
        token = input.tokLine();
    
        if( stricmp( token, Sinclude ) == 0 ) {
    
            // "#include" directives may be nested.
            includeMapFile( input.endTok() );
            continue;
        } else if( stricmp( token, Sdefine ) == 0 ) {
    
            // "#define" directives specify a context string.
            token = input.tokLine();
            if( token == NULL )
                continue;
            is_good_str = 1;
            for( i=0; token[i] != '\0'; ++i ) {
                if( !isalnum( token[i] ) && token[i] != '.' && token[i] != '_' ) {
                    is_good_str = 0;
                }
            }
            if( !is_good_str ) {
                HCWarning( CON_BAD, token, input.lineNum(), input.name() );
            } else {
                hash_value = Hash(token);
                token = input.tokLine();
                if( token == NULL ) {
                    HCWarning( CON_NONUM, token, input.lineNum(),
                               input.name() );
                } else {
                    con_num = atol( token );
                    _theFiles->_mapFile->addMapRec( con_num, hash_value );
                }
            }
        } else if( strncmp( token, SstartComment, 2 ) == 0 ) {
    
            // #include-d files may contain comments.
            int startcomment = input.lineNum();
            while( token != NULL && strstr( token, SendComment ) == NULL ) {
                do {
                    token = input.tokLine();
                    if( token != NULL )
                        break;
                    not_done = input.getLine();
                } while( not_done );
            }
    
            if( token == NULL ) {
                HCWarning( HPJ_RUNONCOMMENT, startcomment, input.name() );
                break;
            }
        } else {
            HCWarning( HPJ_INC_JUNK, input.lineNum(), input.name() );
            continue;
        }
    }
    HCDoneTick();
    return;
}
