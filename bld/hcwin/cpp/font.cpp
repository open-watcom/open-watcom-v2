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
* Description:  Font handling.
*
****************************************************************************/


#include "font.h"
#include "hcerrors.h"
#include <string.h>


#define FONT_NAME_LEN   20
#define FONT_DESC_LEN   11

//  FontName::FontName

FontName::FontName( char const fontname[], short num1, uint_16 num2, uint_8 fam )
    : _nextName( NULL ),
      _prevName( NULL ),
      _userNum( num1 ),
      _sysNum( num2 ),
      _family( fam )
{
    _name = new char[FONT_NAME_LEN];
    strncpy( _name, fontname, FONT_NAME_LEN );
}


//  FontName::~FontName

FontName::~FontName()
{
    if( _name ) {
        delete[] _name;
    }
}


//  FontName::dump  --Dump the structure to a .HLP file.

int FontName::dump( OutFile * dest )
{
    dest->write( _name, 1, FONT_NAME_LEN );
    return 1;
}


//  FontDesc::FontDesc  --Simple constructor.

FontDesc::FontDesc( uint_8 flgs, uint_8 psize,
                    uint_8 fmily, uint_16 ind,
            uint_32 colour )
    : _flags( flgs ),
      _halfPoints( psize ),
      _family( fmily ),
      _index( ind ),
      _rgb( colour ),
      _supPos( 6 ),
      _subPos( 6 ),
      _nextDesc( NULL ),
      _prevDesc( NULL )
{
    // empty
}


//  FontDesc::FontDesc  --Copy constructor.

FontDesc::FontDesc( FontDesc & other )
{
    _flags = other._flags;
    _halfPoints = other._halfPoints;
    _family = other._family;
    _index = other._index;
    _rgb = other._rgb;
    _supPos = other._supPos;
    _subPos = other._subPos;
    _nextDesc = NULL;
    _prevDesc = NULL;
}


//  FontDesc::dump  --Dump the structure to a .HLP file.

int FontDesc::dump( OutFile * dest )
{
    dest->write( _flags );
    dest->write( _halfPoints );
    dest->write( _family );
    dest->write( _index );

    // Convert the _rgb "colour quad" into a triple of bytes.
    uint_8  colours[3];
    for( int i=0; i<3; i++ ) {
        colours[i] = (uint_8) ((_rgb >> i*8) & 0xFF);
    }

    dest->write( colours, 1, 3 );
    dest->write( colours, 1, 3 );
    return 1;
}


//  HFFont::HFFont

HFFont::HFFont( HFSDirectory *d_file )
    : _defDesc( 8 ), // A garbage value, but WinHelp likes it.
      _head( NULL )
{
    FontDesc    *current;

    // Set the default font name and descriptor.
    _firstName = new FontName( "Helv", -1, 0, FNT_SWISS );
    _lastName = _firstName;
    _firstDesc = new FontDesc( 0, 20, FNT_SWISS, 0, 0x00000101 );

    // Add descriptors for the default font in bold and underlined.
    current = _firstDesc;
    current->_nextDesc = new FontDesc( *_firstDesc );
    current->_nextDesc->_prevDesc = _firstDesc;
    current->_nextDesc->_flags |= FNT_BOLD;

    current = current->_nextDesc;
    current->_nextDesc = new FontDesc( *_firstDesc );
    current->_nextDesc->_prevDesc = current;
    current->_nextDesc->_flags |= FNT_UNDERLINE;

    _lastDesc = current->_nextDesc;

    _curDesc = new FontDesc( *_firstDesc );
    _numFonts = 1;
    _numDescs = 3;

    d_file->addFile( this, "|FONT" );
}


//  HFFont::~HFFont

HFFont::~HFFont()
{
    FontState *stemp, *scurrent = _head;
    while( scurrent != NULL ) {
        stemp = scurrent;
        scurrent = scurrent->_next;
        delete stemp;
    }

    FontName *ntemp, *ncurrent = _firstName;
    while( ncurrent != NULL ) {
        ntemp = ncurrent;
        ncurrent = ncurrent->_nextName;
        delete ntemp;
    }

    FontDesc *dtemp, *dcurrent = _firstDesc;
    while( dcurrent != NULL ) {
        dtemp = dcurrent;
        dcurrent = dcurrent->_nextDesc;
        delete dtemp;
    }

    if( _curDesc ) delete _curDesc;
}


//  HFFont::addFont --Add a new font to the table.

void HFFont::addFont( char const fontname[], uint_8 family, short num )
{
    // Check if any font with that number is already defined.
    FontName    *current = _firstName;
    while( current != NULL ) {
        if( current->_userNum == num ) {
            // Warn the user and "overwrite" the previous font.
            HCWarning( FONT_SAMENUM, (int) num );
            current->_userNum = -1;
            break;
        }
        current = current->_nextName;
    }

    // Check if a font with that name is already defined.
    current = _firstName;
    while( current != NULL ) {
        if( strcmp( current->_name, fontname ) == 0 )
            break;
        current = current->_nextName;
    }

    if( current != NULL ) {
        if( current->_userNum >= 0 ) {
            // If the font is already in use...
            HCWarning( FONT_SAMENAME, (const char *) fontname );
        } else {
            // If the font was defined before with a different family...
            current->_userNum = num;
            if( current->_family != family ) {
                HCWarning( FONT_NEWFAM, (const char *) fontname );
            }
        }
    } else {
        _lastName->_nextName = new FontName( fontname, num, (uint_16)(_lastName->_sysNum+1), family );
        _lastName->_nextName->_prevName = _lastName;
        _lastName = _lastName->_nextName;
        ++_numFonts;
    }

    return;
}


//  HFFont::clearFonts  --Clear the font numbers (but not the names).

void HFFont::clearFonts()
{
    FontName *current = _firstName;
    while( current != NULL ) {
        current->_userNum = -1;
        current = current->_nextName;
    }
    return;
}


//  HFFont::setFont --Go to a specific font, by number.

void HFFont::setFont( uint_16 font )
{
    FontDesc    *newdesc = _firstDesc;
    for( uint_16 i=0; newdesc!=NULL && i<font; newdesc=newdesc->_nextDesc, ++i ) {
        // empty
    }

    if( newdesc == NULL ) {
        HCError( SETFONT_ERR );
    }

    delete _curDesc;
    _curDesc = new FontDesc( *newdesc );
    _curNum = font;

    return;
}


//  HFFont::selectFont  --Get the system number for a certain font.

uint_16 HFFont::selectFont( short index, int lnum, char const fname[] )
{
    uint_16 result = _curNum;
    int     i;

    // First, find the index of the font.
    FontName    *current = _firstName;
    while( current != NULL ) {
        if( current->_userNum == index )
            break;
        current = current->_nextName;
    }
    if( current == NULL ) {
        HCWarning( FONT_UNKNOWN, (int) index, lnum, fname );
    } else {
        // Now, find the first font descriptor with that index.
        _curDesc->_index = current->_sysNum;
        FontDesc    *newdesc = _firstDesc;
        for( i=0; newdesc != NULL; newdesc = newdesc->_nextDesc, ++i ) {
            if( newdesc->_index != _curDesc->_index ) continue;
            if( newdesc->_flags != _curDesc->_flags ) continue;
            if( newdesc->_halfPoints != _curDesc->_halfPoints ) continue;
            if( newdesc->_rgb != _curDesc->_rgb ) continue;
            if( newdesc->_supPos != _curDesc->_supPos ) continue;
            if( newdesc->_subPos != _curDesc->_subPos ) continue;
            break;
        }
        if( newdesc == NULL ) {
            _curDesc->_family = current->_family;
            _lastDesc->_nextDesc = new FontDesc( *_curDesc );
            _lastDesc->_nextDesc->_prevDesc = _lastDesc;
            _lastDesc = _lastDesc->_nextDesc;
            result = _numDescs++;
        } else {
            _curDesc->_family = newdesc->_family;
            result = (uint_16) i;
        }
    }

    _curNum = result;
    return result;
}


//  HFFont::getAttribs  --Get the attributes of the current font.

uint_8 HFFont::getAttribs( uint_16 font )
{
    uint_8 result;
    FontDesc    *current = _firstDesc;
    int     i = 0;
    while( i < font && current != NULL ) {
        current = current->_nextDesc;
        ++i;
    }
    if( current == NULL ) {
        result = 0;
    } else {
        result = current->_flags;
    }
    return result;
}


//  HFFont::findDesc    --See if a font descriptor is already in the table.

#define FIND_DESC_ERROR 0xFFFF

uint_16 HFFont::findDesc( FontDesc * wanted )
{
    uint_16 result = (uint_16) 0;
    FontDesc    *current = _firstDesc;
    for( ; current != NULL; current=current->_nextDesc, ++result ) {
        if( current->_index != wanted->_index ) continue;
        if( current->_flags != wanted->_flags ) continue;
        if( current->_halfPoints != wanted->_halfPoints ) continue;
        if( current->_family != wanted->_family ) continue;
        if( current->_rgb != wanted->_rgb ) continue;
        if( current->_supPos != wanted->_supPos ) continue;
        if( current->_subPos != wanted->_subPos ) continue;
        break;
    }
    if( current == NULL ) {
        result = FIND_DESC_ERROR;
    }
    return result;
}


//  HFFont::setAttribs  --Set the attributes of the current font.

uint_16 HFFont::setAttribs( uint_8 attrs )
{
    uint_16 result;

    _curDesc->_flags |= attrs;
    result = findDesc( _curDesc );
    if( result == FIND_DESC_ERROR ) {
        result = _numDescs++;
        _lastDesc->_nextDesc = new FontDesc( *_curDesc );
        _lastDesc->_nextDesc->_prevDesc = _lastDesc;
        _lastDesc = _lastDesc->_nextDesc;
    }
    return _curNum=result;
}


//  HFFont::clearAttribs --Clear the attributes of the current font.

uint_16 HFFont::clearAttribs( uint_8 attrs )
{
    uint_16 result;

    _curDesc->_flags &= (uint_8) ~attrs;
    result = findDesc( _curDesc );
    if( result == FIND_DESC_ERROR ) {
        result = _numDescs++;
        _lastDesc->_nextDesc = new FontDesc( *_curDesc );
        _lastDesc->_nextDesc->_prevDesc = _lastDesc;
        _lastDesc = _lastDesc->_nextDesc;
    }
    return _curNum = result;
}


//  HFFont::newSize --Set a new fontsize.

uint_16 HFFont::newSize( uint_8 hpsize )
{
    uint_16 result;

    _curDesc->_halfPoints = hpsize;
    result = findDesc( _curDesc );
    if( result == FIND_DESC_ERROR ) {
        result = _numDescs++;
        _lastDesc->_nextDesc = new FontDesc( *_curDesc );
        _lastDesc->_nextDesc->_prevDesc = _lastDesc;
        _lastDesc = _lastDesc->_nextDesc;
    }
    return _curNum = result;
}


//  HFFont::newSupPos --Set a new superscript position.

uint_16 HFFont::newSupPos( uint_8 pos )
{
    uint_16 result;

    _curDesc->_supPos = pos;
    result = findDesc( _curDesc );
    if( result == FIND_DESC_ERROR ) {
        result = _numDescs++;
        _lastDesc->_nextDesc = new FontDesc( *_curDesc );
        _lastDesc->_nextDesc->_prevDesc = _lastDesc;
        _lastDesc = _lastDesc->_nextDesc;
    }
    return _curNum = result;
}

//  HFFont::newSupPos --Set a new subscript position.

uint_16 HFFont::newSubPos( uint_8 pos )
{
    uint_16 result;

    _curDesc->_supPos = pos;
    result = findDesc( _curDesc );
    if( result == FIND_DESC_ERROR ) {
        result = _numDescs++;
        _lastDesc->_nextDesc = new FontDesc( *_curDesc );
        _lastDesc->_nextDesc->_prevDesc = _lastDesc;
        _lastDesc = _lastDesc->_nextDesc;
    }
    return _curNum = result;
}

//  HFFont::push    --Store the current font state for later retrieval.

void HFFont::push()
{
    uint_16 index = findDesc( _curDesc );
    FontState   *newstate = new FontState( index );
    newstate->_next = _head;
    _head = newstate;
    return;
}


//  HFFont::pop     --Retrieve a previous font state.

uint_16 HFFont::pop()
{
    uint_16 result = 0;
    if( _head != NULL ) {
        FontState *old_state = _head;
        _head = _head->_next;
        result = old_state->_index;
        delete old_state;
    }
    FontDesc *current = _firstDesc;
    for( uint_16 i=0; i<result && current!=NULL;
         ++i, current=current->_nextDesc ) {
        // empty
    }
    delete _curDesc;
    if( current==NULL ) {
        _curDesc = new FontDesc( *_firstDesc );
    } else {
        _curDesc = new FontDesc( *current );
    }

    return _curNum = result;
}


//  HFFont::size    --Overrides Dumpable::size

uint_32 HFFont::size()
{
    return _numFonts*FONT_NAME_LEN + _numDescs*FONT_DESC_LEN + 4*sizeof( uint_16 );
}


//  HFFont::dump    --Overrides Dumpable::dump

int HFFont::dump( OutFile * dest )
{
    dest->write( _numFonts );
    dest->write( _numDescs );
    dest->write( _defDesc );
    uint_16 desc_offset = (uint_16) (_numFonts*FONT_NAME_LEN + 4*sizeof( uint_16 ));
    dest->write( desc_offset );

    FontName *cur_name = _firstName;
    while( cur_name != NULL ) {
        cur_name->dump( dest );
        cur_name = cur_name->_nextName;
    }

    FontDesc *desc_current = _firstDesc;
    while( desc_current != NULL ) {
        desc_current->dump( dest );
        desc_current = desc_current->_nextDesc;
    }
    return 1;
}
