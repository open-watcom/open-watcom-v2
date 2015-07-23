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
* Description:  .RTF file parsing.
*
****************************************************************************/


#include "parsing.h"
#include "hcerrors.h"
#include <string.h>
#include <ctype.h>


//  Generate the list of supported RTF commands.
enum com_nums {
    #define _COMMAND( n, s ) n
    #include "commands.h"
    #undef _COMMAND
};

static char const *com_strs[] = {
    #define _COMMAND( n, s ) s
    #include "commands.h"
    #undef _COMMAND
};


#define TOPIC_LIMIT 512 // Arbitrary limit to topic size.

char const RtfExt[] = ".RTF";


//  FindCommand     --Map command strings to their assigned numbers.
//            Returns (-1) for failure.

static int FindCommand( char const string[] )
{
    int left=0, right=RC_DUMMY-1, _current, result;

    // I use binary search; the list is reasonably small.
    while( right >= left ) {
        _current = (left+right)/2;
        result = strcmp( string, com_strs[_current] );
        if( result < 0 ) {
            right = _current - 1;
        } else if( result > 0 ) {
            left = _current + 1;
        } else if( result == 0 ) {
            return _current;
        }
    }
    return -1;
}


#define MAX_STATES  20
#define BLOCK_SIZE  80


//  RTFparser::RTFparser    --Mostly, this function just records
//                where other important objects are.

RTFparser::RTFparser( Pointers *p, InFile *src )
    : _storage( BLOCK_SIZE )
{
    _topFile = p->_topFile;
    _fontFile = p->_fontFile;
    _hashFile = p->_hashFile;
    _keyFile = p->_keyFile;
    _titleFile = p->_titleFile;
    _bitFiles = p->_bitFiles;
    _sysFile = p->_sysFile;

    _input = new Scanner( src );
    _fname = new char[strlen( src->name() ) + 1];
    strcpy( _fname, src->name() );

    _nestLevel = 0;

    _storSize = 0;
    _maxStor = BLOCK_SIZE;

    _tabType = TAB_LEFT;
}


//  RTFparser::~RTFparser

RTFparser::~RTFparser()
{
    delete _input;
    if( _fname ) {
        delete[] _fname;
    }
}


//  RTFparser::skipSection  --Pass over RTF text until the "_nestLevel"
//                decreases.

void RTFparser::skipSection()
{
    int target = _nestLevel-1;
    while( _nestLevel > target ) {
        _current = _input->next();
        switch( _current->_type ) {
        case TOK_PUSH_STATE:
            ++_nestLevel;
            break;
        case TOK_POP_STATE:
            --_nestLevel;
            break;
        case TOK_END:
            HCWarning( RTF_BADEOF, _fname );
            _wereWarnings = true;
            return;
        }
    }
    return;
}


//  RTFparser::closeBraces  --Attempt to combine successive font changes.
//                Returns the index of the final font.

uint_16 RTFparser::closeBraces()
{
    uint_16 result = _fontFile->currentFont();
    TokenTypes  t_type;

    for( ;; ) {
        t_type = _input->look( 1 )->_type;
        if( t_type == TOK_PUSH_STATE ) {
            _fontFile->push();
            ++_nestLevel;
        } else if( t_type == TOK_POP_STATE && _nestLevel > 0 ) {
            result = _fontFile->pop();
            --_nestLevel;
        } else if( t_type == TOK_COMMAND ) {
            if( !isFontCommand( _input->look( 1 ), &result ) ) {
                break;
            }
        } else {
            break;
        }
        _input->next();
    }

    return result;
}


//  RTFparser::isParCommand --Identify commands which affect paragraph
//                attributes (tab stops, indents, ...)

bool RTFparser::isParCommand()
{
    bool result = false;
    if( _input->look( 1 )->_type == TOK_COMMAND ) {
        switch( FindCommand( _input->look( 1 )->_text ) ) {
        case RC_BOX:
        case RC_FI:
        case RC_LI:
        case RC_KEEP:
        case RC_KEEPN:
        case RC_PARD:
        case RC_RI:
        case RC_QC:
        case RC_QJ:
        case RC_QL:
        case RC_QR:
        case RC_SA:
        case RC_SB:
        case RC_SL:
        case RC_TX:
            result = true;
        }
    }
    return result;
}


//  RTFparser::isFontCommand    --Identify a "font" command, AND implement
//                the corresponding font change.
//                The new font is stored in "newfont".

bool RTFparser::isFontCommand( Token * tok, uint_16 *newfont )
{
    bool result;
    int num = FindCommand( tok->_text );
    uint_8  style = 0;

    // First check for a command which 'toggles' a font attribute.
    switch( num ) {
    case RC_B:
    case RC_I:
    case RC_SCAPS:
    case RC_STRIKE:
    case RC_UL:
    case RC_ULDB:
        switch( num ) {
        case RC_B: style = FNT_BOLD; break;
        case RC_I: style = FNT_ITALICS; break;
        case RC_SCAPS: style = FNT_SMALL_CAPS; break;
        case RC_STRIKE: style = FNT_STRIKEOUT; break;
        case RC_UL: style = FNT_UNDERLINE; break;
        case RC_ULDB: style = FNT_DBL_UNDER; break;
        }
        if( !tok->_hasValue || tok->_value != 0 ) {
            *newfont = _fontFile->setAttribs( style );
        } else {
            *newfont = _fontFile->clearAttribs( style );
        }
        result = true;
        break;

    default:
        // Now check for commands which change the base font.
        switch( num ) {
        case RC_F:
            if( tok->_hasValue ) {
                *newfont = _fontFile->selectFont( (short)tok->_value, tok->_lineNum, _input->file()->name() );
                result = true;
            } else {
                HCWarning( FONT_NONUM, tok->_lineNum, _fname );
                _wereWarnings = true;
                tok->_type = TOK_NONE;
                result = false;
            }
            break;

        case RC_FS:
            if( tok->_hasValue ) {
                *newfont = _fontFile->newSize( (uint_8)(tok->_value) );
                result = true;
            } else {
                HCWarning( RTF_NOARG, (const char *)tok->_text, tok->_lineNum, _fname );
                _wereWarnings = true;
                tok->_type = TOK_NONE;
                result = false;
            }
            break;

        uint_8  pos;
        case RC_UP:
            if( tok->_hasValue ) {
                pos = (uint_8)(tok->_value);
            } else {
                pos = 6;
            }
            *newfont = _fontFile->newSupPos( pos );
            result = true;
            break;

        case RC_DN:
            if( tok->_hasValue ) {
                pos = (uint_8)(tok->_value);
            } else {
                pos = 6;
            }
            *newfont = _fontFile->newSubPos( pos );
            result = true;
            break;

        case RC_PLAIN:
            *newfont = _fontFile->clearAttribs( 0xFF );
            result = true;
            break;

        default:
            result = false;
            break;
        }
        break;
    }
    return result;
}


//  RTFparser::handleCommand    --Implement non-font-related commands.

void RTFparser::handleCommand()
{
    int com_num = FindCommand( _current->_text );
    uint_8  attribs;

    // two variables we may need to deal with a \par command.
    uint_16 temp_font;
    bool    is_new_topic;

    // Certain commands may necessitate a new node in the |TOPIC file.
    if( _writeState == HEADER ) {
        switch( com_num ) {
        case RC_LINE:
        case RC_PAR:
        case RC_SECT:
        case RC_TAB:
        case RC_V:
            _writeState = SCROLL;
            _topFile->newNode(0);
            _topFile->addAttr( TOP_FONT_CHANGE, _curFont );
            _topFile->startScroll();
        }
    }

    switch( com_num ) {
    // Sections to skip 'cos they're unused in .HLP files.
    case RC_COLORTBL:   // actually, I should support this one.
    case RC_INFO:
    case RC_STYLESHEET:
        skipSection();
        break;

    // Commands to ignore 'cos they're meaningless in .HLP files.
    case RC_ENDNHERE:
    case RC_FTNBJ:
    case RC_LINEX:
    case RC_SECTD:
    case RC_ULW:
        // do nothing
        break;

    case RC_BOX:    // The "Boxed paragraph" command
        _topFile->setPar( TOP_BORDER, 0x1 );
        break;

    case RC_DEFF:   // The "Set Default Font" command
        if( !_current->_hasValue ) {
            HCWarning( RTF_NOARG, (const char *)_current->_text, _current->_lineNum, _fname );
            _wereWarnings = true;
        } else {
            _defFont = (uint_16)_current->_value;
        }
        break;

    case RC_FI: // The "First Line Indent" command
        if( !_current->_hasValue ) {
            HCWarning( RTF_NOARG, (const char *)_current->_text, _current->_lineNum, _fname );
            _wereWarnings = true;
        } else {
            if( !_topFile->setPar( TOP_FIRST_INDENT, _current->_value ) ) {
                HCWarning( TOP_BADARG, _current->_value, _current->_lineNum, _fname );
                _wereWarnings = true;
            }
        }
        break;

    case RC_FONTTBL: // The "Font Table" command
        if( !_current->_hasValue || _current->_value != 0 ) {
            handleFonts();
        }
        break;

    case RC_KEEP:   // The "No LineWrap" command
        _topFile->setPar( TOP_NO_LINE_WRAP );
        break;

    case RC_KEEPN:  // The "Start Non-scroll Area" command.
        if( _writeState == HEADER ) {
            _topFile->newNode(0);
            _topFile->addAttr( TOP_FONT_CHANGE, _curFont );
            _topFile->startNonScroll();
            _writeState = NON_SCROLL;
        } else if( _writeState == SCROLL ) {
            HCWarning( RTF_LATEKEEPN, _current->_lineNum, _fname );
            _wereWarnings = true;
        }
        break;

    case RC_LI: // The "Left Indent" command.
        if( !_current->_hasValue ) {
            HCWarning( RTF_NOARG, (const char *)_current->_text, _current->_lineNum, _fname );
            _wereWarnings = true;
        } else {
            if( !_topFile->setPar( TOP_LEFT_INDENT, _current->_value ) ) {
                HCWarning( TOP_BADARG, _current->_value, _current->_lineNum, _fname );
                _wereWarnings = true;
            }
        }
        break;

    case RC_LINE:   // The "Explicit New-Line".
        _topFile->addAttr( TOP_NEW_LINE );
        break;

    case RC_PAGE:   // The "Hard Page".
        // \page ALWAYS signals a new topic.
        HCTick();
        _topFile->newNode( 1 );
        _topFile->clearPar();
        _curFont = _fontFile->clearAttribs( 0xFF );
        _writeState = HEADER;
        break;

    case RC_PAR:        // "Paragraph return".
    case RC_SECT:       // deliberate fall-through
        _topFile->addAttr( TOP_NEW_PAR );
        _lastFont = _fontFile->clearAttribs( 0xFF );
        temp_font = closeBraces();

        // These commands signal a new topic if the next token is
        // a paragraph command, or if the current topic has grown
        // too large for comfort.

        is_new_topic = ( _topFile->presentSize() >= TOPIC_LIMIT || isParCommand() );
        if( is_new_topic || _curFont != temp_font ) {
            if( is_new_topic ) {
                _topFile->newNode(0);
            }
            _curFont = temp_font;
            int attr = _topFile->addAttr( TOP_FONT_CHANGE, _curFont );
            attribs = _fontFile->getAttribs( _curFont );
            if( attribs & ( FNT_UNDERLINE | FNT_STRIKEOUT | FNT_DBL_UNDER ) ) {
                _hotlinkStart = attr;
            }
        }
        break;

    case RC_PARD:   // "Set Paragraph Properties to Default"
        _topFile->clearPar();
        if( _writeState == NON_SCROLL ) {
            _topFile->startScroll();
            _writeState = SCROLL;
        }
        _tabType = TAB_LEFT;
        break;

    case RC_QC: // Centre Justification
        _topFile->unsetPar( TOP_RIGHT_JUST );
        _topFile->setPar( TOP_CENTRE_JUST );
        break;

    case RC_QJ: // Normal justification
    case RC_QL: // deliberate fall-through
        _topFile->unsetPar( TOP_RIGHT_JUST );
        _topFile->unsetPar( TOP_CENTRE_JUST );
        break;

    case RC_QR: // Right Justification
        _topFile->unsetPar( TOP_CENTRE_JUST );
        _topFile->setPar( TOP_RIGHT_JUST );
        break;

    case RC_RI: // The "Right Indent" command.
        if( !_current->_hasValue ) {
            HCWarning( RTF_NOARG, (const char *)_current->_text, _current->_lineNum, _fname );
            _wereWarnings = true;
        } else {
            if( !_topFile->setPar( TOP_RIGHT_INDENT, _current->_value ) ) {
                HCWarning( TOP_BADARG, _current->_value, _current->_lineNum, _fname );
                _wereWarnings = true;
            }
        }
        break;

    case RC_SA: // The "Space After Paragraph" command.
        if( !_current->_hasValue ) {
            HCWarning( RTF_NOARG, (const char *)_current->_text, _current->_lineNum, _fname );
            _wereWarnings = true;
        } else {
            if( !_topFile->setPar( TOP_SPACE_AFTER, _current->_value ) ) {
                HCWarning( TOP_BADARG, _current->_value, _current->_lineNum, _fname );
                _wereWarnings = true;
            }
        }
        break;

    case RC_SB: // The "Space Before Paragraph" command.
        if( !_current->_hasValue ) {
            HCWarning( RTF_NOARG, (const char *)_current->_text, _current->_lineNum, _fname );
            _wereWarnings = true;
        } else {
            if( !_topFile->setPar( TOP_SPACE_BEFORE, _current->_value ) ) {
                HCWarning( TOP_BADARG, _current->_value, _current->_lineNum, _fname );
                _wereWarnings = true;
            }
        }
        break;

    case RC_SL: // "Line Spacing".
        if( !_current->_hasValue ) {
            HCWarning( RTF_NOARG, (const char *)_current->_text, _current->_lineNum, _fname );
            _wereWarnings = true;
        } else {
            if( !_topFile->setPar( TOP_LINE_SPACE, _current->_value ) ) {
                HCWarning( TOP_BADARG, _current->_value, _current->_lineNum, _fname );
                _wereWarnings = true;
            }
        }
        break;

    case RC_TAB:    // The "Tab" command. (Well, DUH! :-)
        _topFile->addAttr( TOP_HTAB );
        break;

    case RC_TQC:    // "Center tab"
        _tabType = TAB_CENTER;
        break;

    case RC_TQR:
        _tabType = TAB_RIGHT;
        break;

    case RC_TX: // "Set Tab Stop"
        if( !_current->_hasValue ) {
            HCWarning( RTF_NOARG, (const char *)_current->_text, _current->_lineNum, _fname );
            _wereWarnings = true;
        } else {
            if( !_topFile->setTab( _current->_value, _tabType ) ) {
                HCWarning( TOP_BADTAB, _current->_value, _current->_lineNum, _fname );
                _wereWarnings = true;
            } else {
                _tabType = TAB_LEFT;
            }
        }
        break;

    case RC_V:  // "Start/Stop Hidden Text".
        if( !_current->_hasValue || _current->_value == 0 ) {
            attribs = _fontFile->getAttribs( _curFont );
            if( attribs & FNT_UNDERLINE ) {
                _linkType = POPUP;
                handleHidden( true );
            } else if( attribs & (FNT_STRIKEOUT | FNT_DBL_UNDER ) ) {
                _linkType = JUMP;
                handleHidden( true );
            } else {
                handleHidden( false );
            }
        }
        break;

    default:
        HCWarning( RTF_UNKNOWN, (const char *)_current->_text, _current->_lineNum, _fname );
        _wereWarnings = true;
    }
}


//  RTFparser::Go   --The parser "main loop".

#define NOT_A_BITMAP ((FontFlags) 0)

#define HARD_SPACE  '\xA0'

void RTFparser::Go()
{
    HCStartFile( _fname );

    // Make sure the first three tokens are an RTF header.
    _input->next();
    _wereWarnings = true;
    if( _input->look( 0 )->_type != TOK_PUSH_STATE ) {
        HCWarning( RTF_HEADER, _fname );
    } else if( _input->look( 1 )->_type != TOK_COMMAND
      || FindCommand( _input->look( 1 )->_text ) != RC_RTF ) {
        HCWarning( RTF_RTF, _fname );
    } else if( _input->look( 2 )->_type != TOK_COMMAND ) {
        HCWarning( RTF_CHARSET, _fname );
    } else {
        int command = FindCommand( _input->look( 2 )->_text );
        if( command != RC_ANSI &&
            command != RC_PC &&
            command != RC_WINDOWS ) {
            HCWarning( RTF_CHARSET, _fname );
        } else {
            _wereWarnings = false;
        }
    }
    if( _wereWarnings ) {
        HCError( BAD_RTF, _fname );
    }
    _nestLevel = 1;


    // Dump the first three tokens; we don't need them anymore.
    _input->next();
    _input->next();


    uint_16 temp_font;  // for font changes
    uint_8  attribs;    // for font changes
    char    smallstr[2];    // for creating bits of text.
    smallstr[1] = '\0';

    _writeState = HEADER;

    // Loop until we 'pop' out of the main block.
    while( _nestLevel > 0 ) {
        _current = _input->next();
        if( _current->_type == TOK_END )
            break;
        if( _current->_type == TOK_NONE )
            continue;
    
        switch( _current->_type ) {
    
        case TOK_TEXT:
            // At this stage of the program, text is just
            // dumped to output.
            if( _writeState == HEADER ) {
                _writeState = SCROLL;
                _topFile->newNode(0);
                _topFile->addAttr( TOP_FONT_CHANGE, _curFont );
                _topFile->startScroll();
            }
            _topFile->addText( _current->_text, true );
            break;
    
    
        case TOK_PUSH_STATE:
            // Store the _current font once for every consecutive '{'
            for( ;; ) {
                _fontFile->push();
                ++_nestLevel;
                if( _input->look( 1 )->_type != TOK_PUSH_STATE )
                    break;
                _input->next();
            }
            break;
    
    
        case TOK_POP_STATE:
            // Restore the correct font.
            --_nestLevel;
            _fontFile->pop();
            temp_font = closeBraces();
    
            // If the next command is "\v" we may have just terminated
            // a hotlink.
            if( _input->look( 1 )->_type == TOK_COMMAND && FindCommand( _input->look( 1 )->_text ) == RC_V && ( !_input->look( 1 )->_hasValue || _input->look( 1 )->_value != 0 ) ) {
                attribs = _fontFile->getAttribs( _curFont );
                if( attribs & FNT_UNDERLINE ) {
                    _linkType = POPUP;
                    handleHidden( true );
                } else if( attribs & (FNT_STRIKEOUT | FNT_DBL_UNDER ) ) {
                    _linkType = JUMP;
                    handleHidden( true );
                } else {
                    handleHidden( false );
                }
            } else if( temp_font != _curFont ) {
                // We may also be just starting a hotlink.
                _lastFont = _curFont;
                _curFont = temp_font;
                int attr = _topFile->addAttr( TOP_FONT_CHANGE, _curFont );
                attribs = _fontFile->getAttribs( _curFont );
                if( attribs & ( FNT_UNDERLINE | FNT_STRIKEOUT | FNT_DBL_UNDER ) ) {
                    _hotlinkStart = attr;
                }
            }
            break;
    
    
        case TOK_SPEC_CHAR:
            // This character may start a footnote.
            if( _input->isFootnoteChar( (char) _current->_value ) ) {
                if( _input->look( 1 )->_type == TOK_PUSH_STATE
                  && _input->look( 2 )->_type == TOK_COMMAND
                  && FindCommand( _input->look( 2 )->_text ) == RC_FOOTNOTE ) {
                    char fchar = (char) _current->_value;
                    _input->next();
                    _input->next();
                    handleFootnote( fchar );
                } else {
                    smallstr[0] = (char) _current->_value;
                    _topFile->addText( smallstr );
                }
            } else if( _current->_value == '{'
              && _input->look( 1 )->_type == TOK_TEXT
              && _input->look( 2 )->_type == TOK_SPEC_CHAR
              && _input->look( 2 )->_value == '}' ) {
                // We must perform a special check for the
                // "\{bm* image_file\}" construct.
        
                char const  *string = _input->look( 1 )->_text;
                uint_16     bmnum;
        
                FontFlags   bmtype = NOT_A_BITMAP;
        
                if( strncmp( string, "bmc ", 4 ) == 0 ) {
                    bmtype = TOP_CENT_BITMAP;
                } else if( strncmp( string, "bml ", 4 ) == 0 ) {
                    bmtype = TOP_LEFT_BITMAP;
                } else if( strncmp( string, "bmr ", 4 ) == 0 ) {
                    bmtype = TOP_RIGHT_BITMAP;
                }
        
                if( bmtype != NOT_A_BITMAP ) {
                    string += 4;
                    while( isspace( *string ) ) {
                        string++;
                    }
                    try {
                        bmnum = _bitFiles->use( string );
                        _topFile->addAttr( bmtype, bmnum );
                    } catch( HFBitmaps::ImageNotFound ) {
                        HCWarning( RTF_NOSUCHIMAGE, string, _current->_lineNum, _fname );
                        _wereWarnings = true;
                    } catch( HFBitmaps::ImageNotSupported ) {
                        HCWarning( UNKNOWN_IMAGE, string, _current->_lineNum, _fname );
                        _wereWarnings = true;
                    } catch( HFBitmaps::ImageNotValid ) {
                        HCWarning( RTF_USEDBADIMAGE, string, _current->_lineNum, _fname );
                        _wereWarnings = true;
                    }
        
                    _input->next();
                    _input->next();
                } else {
                    smallstr[0] = '{';
                    _topFile->addText( smallstr );
                }
            } else {
                if( _current->_value == '~' ) {
                    smallstr[0] = HARD_SPACE;
                } else {
                    smallstr[0] = (char)_current->_value;
                }
                _topFile->addText( smallstr );
            }
            break;
    
    
        case TOK_COMMAND:
            // For font commands, we go through the same junk as for
            // TOK_POP_STATE.  Other commands go to handleCommand().
            temp_font = _curFont;
            if( isFontCommand( _current, &temp_font ) ) {
                if( _writeState == HEADER ) {
                    _topFile->newNode(0);
                    _topFile->addAttr( TOP_FONT_CHANGE, _curFont );
                    _topFile->startScroll();
                    _writeState = SCROLL;
                }
                temp_font = closeBraces();
                if( _input->look( 1 )->_type == TOK_COMMAND
                  && FindCommand( _input->look( 1 )->_text ) == RC_V
                  && ( !_input->look( 1 )->_hasValue || _input->look( 1 )->_value != 0 ) ) {
                    attribs = _fontFile->getAttribs( _curFont );
                    if( attribs & FNT_UNDERLINE ) {
                        _linkType = POPUP;
                        handleHidden( true );
                    } else if( attribs & (FNT_STRIKEOUT | FNT_DBL_UNDER ) ) {
                        _linkType = JUMP;
                        handleHidden( true );
                    } else {
                        handleHidden( false );
                    }
                } else if( temp_font != _curFont ) {
                    _lastFont = _curFont;
                    _curFont = temp_font;
                    int attr = _topFile->addAttr( TOP_FONT_CHANGE, _curFont );
                    attribs = _fontFile->getAttribs( _curFont );
                    if( attribs & ( FNT_UNDERLINE | FNT_STRIKEOUT | FNT_DBL_UNDER ) ) {
                        _hotlinkStart = attr;
                    }
                }
            } else {
                handleCommand();
            }
            break;
        }
    }

    // Now we clean up...check the nesting level, close the topic text...
    if( _nestLevel != 0 ) {
        HCWarning( RTF_BADEOF, _fname );
        _wereWarnings = true;
    } else {
        do {
            _current = _input->next();
        } while( _current->_type == TOK_NONE );
        if( _current->_type != TOK_END ) {
            HCWarning( RTF_EXTRATEXT, _fname );
            _wereWarnings = true;
        }
    }
    _fontFile->clearFonts();
    _topFile->newNode(1);
    HCDoneTick();

    if( _wereWarnings ) {
        HCWarning( PROBLEM_RTF, _fname );
        _wereWarnings = true;
    }
}


//  RTFparser::skipSpaces   --utility function to skip whitespace.

char *RTFparser::skipSpaces( char * start )
{
    while( isspace( *start ) ) {
        start++;
    }
    return start;
}


//  RTFparser::handleFootnote   --Footnotes signal all sorts of things
//                in a HLP file.

void RTFparser::handleFootnote( char Fchar )
{
    // store the text of the footnote in a buffer
    _storSize = 0;
    for( ;; ) {
        _current = _input->next();
        if( _current->_type == TOK_POP_STATE || _current->_type == TOK_END )
            break;
    
        if( _current->_type == TOK_TEXT ) {
            if( _storSize + _current->_value + 1 >= _maxStor ) {
                _maxStor = (_storSize+_current->_value+1) / BLOCK_SIZE + 1;
                _maxStor *= BLOCK_SIZE;
                _storage.resize( _maxStor );
            }
            memcpy( _storage+_storSize, _current->_text, _current->_value );
            _storSize += _current->_value;
        } else if( _current->_type == TOK_SPEC_CHAR ) {
            if( _storSize + 2 >= _maxStor ) {
                _maxStor += BLOCK_SIZE;
                _storage.resize( _maxStor );
            }
            _storage[_storSize++] = (char)_current->_value;
        }
    }
    if( _current->_type == TOK_END ) {
        HCWarning( RTF_BADEOF, _fname );
        _wereWarnings = true;
        return;
    }

    _storage[_storSize] = '\0';

    // parse the footnote string according to the footnote character.
    char *start = _storage;
    if( *start == Fchar )
        ++start;
    char *end;
    bool finished = false;
    switch( Fchar ) {
    case '#':   // Context string
        start = skipSpaces( start );
        if( *start == '\0' ) {
            HCWarning( CON_MISSING, _current->_lineNum, _fname );
            _wereWarnings = true;
            break;
        }
        end = start;
        while( *end != '\0' && !isspace( *end ) ) {
            if( !isalnum( *end ) && *end != '.' && *end != '_' ) {
                HCWarning( CON_BADCHAR, _current->_lineNum, _fname );
                _wereWarnings = true;
                break;
            }
            ++end;
        }
        if( *end == '\0' || isspace( *end ) ) {
            *end = '\0';
            _hashFile->addOffset( Hash( start ), _topFile->charOffset() );
        }
        break;


    case '$':   // Title string.
        start = skipSpaces( start );
        if( *start != '\0' ) {
            end = _storage+_storSize;
            --end;
            while( end > start && isspace( *end ) ) {
                --end;
            }
            *++end = '\0';
        }
        _titleFile->addTitle( _topFile->charOffset(), start );
        if( _writeState == HEADER ) {
            _topFile->addText( start );
        } else {
            HCWarning( TTL_TOOLATE, _current->_lineNum, _fname );
            _wereWarnings = true;
        }
        break;


    case 'K':   // Keyword strings.
        while( !finished ) {
            start = skipSpaces( start );
            if( *start == '\0' )
                break;
            end = start;
            while( *end != '\0' && *end != ';' ) {
                ++end;
            }
            if( *end == '\0' )
                finished = true;
            do {
                --end;
            } while( end >= start && isspace( *end ) );
            *++end = '\0';
            if( start != end ) {
                _keyFile->addKW( start, _topFile->charOffset() );
            }
            start = end + 1;
        }
        break;


    case '+':   // Browse sequence identifiers.
        start = skipSpaces( start );
        if( *start != '\0' ) {
            end = _storage+_storSize;
            --end;
            while( end > start && isspace( *end ) ) {
            --end;
            }
            *++end = '\0';
            _topFile->addBrowse( start );
        }
        break;


    case '!':   // Macros.
        if( _writeState != HEADER ) {
            HCWarning( MAC_TOOLATE, _current->_lineNum, _fname );
            _wereWarnings = true;
            break;
        }
        start = skipSpaces( start );
        if( *start != '\0' ) {
            char    terminator = '\0';
            end = _storage+_storSize;
            --end;
            while( end > start && isspace( *end ) ) {
                --end;
            }
            *++end = '\0';
            _topFile->addText( &terminator );
            _topFile->addText( start );
        }
    }

    return;
}


//  RTFparser::handleHidden --Parse hidden text (which may be a
//                hotlink).

void RTFparser::handleHidden( bool IsHotLink )
{
    uint_16 result = _fontFile->currentFont();
    uint_16 new_font;
    uint_32 hash_value;
    uint_16 length;
    uint_8  attribs;
    char    *pstorage, *pwindow, *pfile;
    int     target = _nestLevel-1;
    bool    done = false;

    enum { NORMAL, INVISIBLE, UNDERLINED } hotlink_type;
    enum { TO_WINDOW = 0x01, TO_FILE = 0x04, TO_BOTH = 0x06 };

    _storSize = 0;
    while( _nestLevel > target && !done ) {
        // Read text into a buffer until the hidden state is turned off.
        _current = _input->next();
        if( _current->_type == TOK_END )
            break;
        if( _current->_type == TOK_NONE )
            continue;
    
        switch( _current->_type ) {
        case TOK_PUSH_STATE:
            _fontFile->push();
            ++_nestLevel;
            break;
    
        case TOK_POP_STATE:
            --_nestLevel;
            result = _fontFile->pop();
            result = closeBraces();
            break;
    
        case TOK_COMMAND:
            // We do have to keep track of font changes.
            if( isFontCommand( _current, &result ) ) {
                result = closeBraces();
            } else if( FindCommand( _current->_text ) == RC_V && _current->_hasValue && _current->_value == 0 ) {
                done = true;
            }
            break;
    
        case TOK_TEXT:
            if( _storSize+_current->_value+1 >= _maxStor ) {
                _maxStor = (_storSize+_current->_value+1) / BLOCK_SIZE + 1;
                _maxStor *= BLOCK_SIZE;
                _storage.resize( _maxStor );
            }
            memcpy( _storage+_storSize, _current->_text, _current->_value );
            _storSize += _current->_value;
            break;
    
        case TOK_SPEC_CHAR:
            if( _storSize+2 >= _maxStor ) {
                _maxStor += BLOCK_SIZE;
                _storage.resize( _maxStor );
            }
            _storage[_storSize++] = (char)_current->_value;
            break;
        }
    }

    _storage[_storSize] = '\0';
    if( _current->_type == TOK_END ) {
        HCWarning( RTF_BADEOF, _fname );
        _wereWarnings = true;
    } else if( IsHotLink ) {
        // Parse the destination context string.
        hotlink_type = NORMAL;
        pfile = pwindow = NULL;
    
        pstorage = skipSpaces( _storage );
        if( *pstorage == '%' || *pstorage == '*' ) {
            if( *pstorage == '%' ) {
                hotlink_type = INVISIBLE;
            } else {
                hotlink_type = UNDERLINED;
            }
            pstorage++;
            pstorage = skipSpaces( pstorage );
        }
        if( *pstorage == '!' ) {
            _linkType = MACRO;
            pstorage++;
            pstorage = skipSpaces( pstorage );
        }
    
        if( _linkType != MACRO ) {
            pfile = pstorage;
            while( *pfile != '\0' && *pfile != '>' && *pfile != '@' ) {
                pfile++;
            }
            if( *pfile == '>' ) {
                *pfile = '\0';
                pwindow = ++pfile;
                while( *pfile != '\0' && *pfile != '@' ) {
                    pfile++;
                }
            }
            if( *pfile != '\0' ) {
                *pfile = '\0';
                pfile++;
            }
    
            hash_value = Hash( pstorage );
    
            length = (uint_16) (strlen( pfile )+2);
            if( pwindow != NULL ) {
                length = (uint_16) (length + strlen( pwindow ) + 1 );
            }
    
            if( pwindow != NULL && *pwindow != '\0' ) {
                pwindow--;
                if( *pfile != '\0' ) {
                    *pwindow = TO_BOTH;
                } else {
                    *pwindow = TO_WINDOW;
                    int win_num = _sysFile->winNumberOf( pwindow+1 );
                    if( win_num != HFSystem::NoSuchWin ) {
                        *(pwindow+1) = (uint_8) win_num;
                    } else {
                        HCWarning( TOP_NOWIN, pwindow+1, _current->_lineNum, _fname );
                        *(pwindow+1) = (uint_8) 0;
                    }
                    length = 2;
                }
                pfile = pwindow;
            } else if( *pfile != '\0' ) {
                pfile--;
                *pfile = TO_FILE;
            }
        } else {
            hash_value = 0;
            length = (uint_16) (strlen( pstorage )+1);
        }
    
        FontFlags   link_t;
        if( pwindow == NULL && (pfile == NULL || *pfile == '\0') ) {
            if( hotlink_type == NORMAL ) {
                if( _linkType == MACRO ) {
                    link_t = TOP_MACRO_LINK;
                } else if( _linkType == POPUP ) {
                    link_t = TOP_POPUP_LINK;
                } else {
                    link_t = TOP_JUMP_LINK;
                }
            } else {
                if( _linkType == MACRO ) {
                    link_t = TOP_MACRO_INVIS;
                } else if( _linkType == POPUP ) {
                    link_t = TOP_POPUP_INVIS;
                } else {
                    link_t = TOP_JUMP_INVIS;
                }
            }
        } else {
            if( hotlink_type == NORMAL ) {
                if( _linkType == POPUP ) {
                    link_t = TOP_POPUP_FILE;
                } else {
                    link_t = TOP_JUMP_FILE;
                }
            } else {
                if( _linkType == POPUP ) {
                    link_t = TOP_POPUP_FILE_INVIS;
                } else {
                    link_t = TOP_JUMP_FILE_INVIS;
                }
            }
        }
    
        if( hotlink_type == UNDERLINED ) {
            _fontFile->setFont( (uint_16) _topFile->attrData( _hotlinkStart ) );
            new_font = _fontFile->setAttribs( FNT_UNDERLINE );
            _topFile->chgAttr( _hotlinkStart, TOP_FONT_CHANGE, new_font );
            _fontFile->setFont( result );
        }
    
        if( result != _lastFont || hotlink_type != NORMAL ) {
            if( link_t < TOP_POPUP_LINK ) {
                _topFile->appendAttr( _hotlinkStart, link_t, pstorage, length );
            } else if( link_t < TOP_POPUP_FILE ) {
                _topFile->appendAttr( _hotlinkStart, link_t, hash_value );
            } else {
                _topFile->appendAttr( _hotlinkStart, link_t, pfile, length, hash_value );
            }
        } else {
            if( link_t < TOP_POPUP_LINK ) {
                _topFile->chgAttr( _hotlinkStart, link_t, pstorage, length );
            } else if( link_t < TOP_POPUP_FILE ) {
                _topFile->chgAttr( _hotlinkStart, link_t, hash_value );
            } else {
                _topFile->chgAttr( _hotlinkStart, link_t, pfile, length, hash_value );
            }
        }
        _topFile->addAttr( TOP_END_LINK );
    
        if( _linkType != MACRO && ( link_t<TOP_POPUP_FILE || *pfile==TO_WINDOW ) ) {
            *pfile = '\0';
            _hashFile->recordContext( pstorage );
        }
    
        if( result != _curFont ) {
            _lastFont = _curFont;
            _curFont = result;
            int attr = _topFile->addAttr( TOP_FONT_CHANGE, _curFont );
            attribs = _fontFile->getAttribs( _curFont );
            if( attribs & (FNT_UNDERLINE | FNT_STRIKEOUT | FNT_DBL_UNDER ) ) {
                _hotlinkStart = attr;
            }
        }
    }

    return;
}


#define FONT_NAME_SIZE 25


//  RTFparser::handleFonts  --Parse the font table with a simple FSM.

void RTFparser::handleFonts()
{
    int target = _nestLevel-1;
    bool done = false;
    int com_num;
    bool ok2read = true;
    short   cur_num = 0;
    uint_8  cur_family = 0;
    char    cur_name[FONT_NAME_SIZE];
    int     name_size = 0;
    enum { NUMBER, FAMILY, NAME } state = NUMBER;

    // some handy loop variables we may need.
    char const  *charp;
    int     i;

    while( _nestLevel > target && !done ) {
        if( ok2read ) {
            _current = _input->next();
        }
        if( _current->_type == TOK_END )
            break;
        if( _current->_type == TOK_NONE )
            continue;
    
        ok2read = true;    // the default behaviour
        switch( _current->_type ) {
        case TOK_POP_STATE:
            // Note we have to keep track of font changes caused
            // by pops.
            _curFont = _fontFile->pop();
            --_nestLevel;
            if( state != NUMBER ) {  // Incomplete font definition.
                HCWarning( FONT_CUTOFF, _current->_lineNum, _fname );
                _wereWarnings = true;
            }
            break;
    
        case TOK_PUSH_STATE:
            _fontFile->push();
            ++_nestLevel;
            break;
    
        case TOK_COMMAND:
            com_num = FindCommand( _current->_text );
            if( com_num == RC_FONTTBL && _current->_hasValue && _current->_value == 0 ) {
                done = true;
                if( state != NUMBER ) {  // Incomplete font definition.
                    HCWarning( FONT_CUTOFF, _current->_lineNum, _fname );
                    _wereWarnings = true;
                }
                continue;
            }
            // commands mean different things in different states.
            switch( state ) {
            case NUMBER:    // We're looking for the font number.
                if( com_num == RC_F ) {
                    if( !_current->_hasValue ) {
                        HCWarning( FONT_NONUM, _current->_lineNum, _fname );
                        _wereWarnings = false;
                        continue;
                    } else {
                        cur_num = (short) _current->_value;
                        name_size = 0;
                        state = FAMILY;
                    }
                }
                break;
    
            case FAMILY:    // We're looking for the font family.
                state = NAME;
                switch( com_num ) {
                case RC_FDECOR:
                    cur_family = FNT_DECORATIVE;
                    break;
                case RC_FMODERN:
                    cur_family = FNT_MODERN;
                    break;
                case RC_FNIL:
                case RC_FSWISS:
                case RC_FTECH:
                    cur_family = FNT_SWISS;
                    break;
                case RC_FROMAN:
                    cur_family = FNT_ROMAN;
                    break;
                case RC_FSCRIPT:
                    cur_family = FNT_SCRIPT;
                    break;
                default:
                    state = FAMILY;
                }
                break;
    
            case NAME:  // We're looking for the font name.
                if( com_num == RC_F ) {
                    HCWarning( FONT_CUTOFF, _current->_lineNum, _fname );
                    _wereWarnings = true;
                    state = NUMBER;
                    ok2read = false;
                    if( name_size > 0 ) {
                        cur_name[name_size] = '\0';
                        _fontFile->addFont( cur_name, cur_family, cur_num );
                    }
                }
                break;
            }
            break;
    
        case TOK_TEXT:  // We'd better be in the NAME state.
            if( state == NAME ) {
                i = 0;
                charp = _current->_text;
                while( name_size+i+1 < FONT_NAME_SIZE ) {
                    if( *charp == ';' || *charp == '\0' )
                        break;
                    ++i;
                    ++charp;
                }
                memcpy( cur_name+name_size, _current->_text, i );
                name_size += i;
                if( *charp == ';' ) {
                    cur_name[name_size] = '\0';
                    _fontFile->addFont( cur_name, cur_family, cur_num );
                    state = NUMBER;
                }
            }
            break;
        }
    }
}
