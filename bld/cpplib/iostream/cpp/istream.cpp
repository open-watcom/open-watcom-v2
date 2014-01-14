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

#include "variety.h"
#include <ctype.h>
#include <limits.h>
#include <errno.h>
#include <stdlib.h>
#include <iostream>
#include <streambu>
#include <ioutil.h>
#include <lock.h>

#define ERR_CHAR    '\0'

// **************************** ISTREAM **************************************

// Used by getnumber. Multiplication by 8 is done using a left-shift of
// three bits. Multiplication by 16 is done using a left-shift of four
// bits. Multiplication by 10 is done using a left-shift of three bits
// plus a left-shift of one bit. This table is used to determine if a
// shift will overflow. The number of bits to shift is used to index
// into the table. The table entry is anded with the unsigned long
// number and if any bits are on, then the shift will overflow.

static unsigned long const overFlowMasks[] = {
    0x00000000,
    0x80000000,
    0xC0000000,
    0xE0000000,
    0xF0000000
};

namespace std {

  istream::istream() {
    /******************/
    // Protected constructor, making an istream without a streambuf attached.

    __last_read_length = 0;
    setf( ios::skipws );
  }

  istream::istream( streambuf *sb ) : ios( sb ) {
    /*********************************************/
    // Public constructor, making an istream with a streambuf attached.

    setf( ios::skipws );
  }

  istream::istream( istream const &istrm ) {
    /****************************************/
    // Public copy constructor, make an istream with the istrm streambuf attached.

    streambuf *sb;

    __lock_it( istrm.__i_lock );
    sb = istrm.rdbuf();
    ios::init( sb );
    setf( ios::skipws );
  }


  istream::~istream() {
    /*******************/
    // Destructor.
  }

  istream &istream::operator = ( streambuf *sb ) {
    /**********************************************/
    // *this is an istream that has been initialized, and may or may not
    // have a streambuf associated with it. Associate the streambuf "sb"
    // with the stream.

    __lock_it( __i_lock );
    ios::init( sb );
    return( *this );
  }

  istream &istream::operator = ( istream const &istrm ) {
    /*****************************************************/
    // *this is an istream that has been initialized, and may or may not
    // have a streambuf associated with it. Associate the streambuf
    // found in "istrm" with the stream.

    streambuf *sb;

    __lock_it( __lock_first( __i_lock, istrm.__i_lock ) );
    __lock_it( __lock_second( __i_lock, istrm.__i_lock ) );
    sb = istrm.rdbuf();
    ios::init( sb );
    return( *this );
  }

  int istream::ipfx( int noskipws ) {
    /*********************************/
    // Input prefix.

    __lock_it( __i_lock );
    if( !good() ) {
        __WATCOM_ios::setstate( this, ios::failbit );
        return( 0 );
    }
    __last_read_length = 0;

    // Flush any tied streams, if noskipws == 0
    if( tie() != NULL ) {
        tie()->flush();
    }

    // Skip whitespace if ios::skipws is set and noskipws == 0.
    if( !noskipws && ((flags() & ios::skipws) != 0) ) {
        ws( *this );
        if( eof() ) {
            __WATCOM_ios::setstate( this, ios::failbit );
        }
    }

    // Ensure the error state is still 0:
    return( good() );
  }

  istream &istream::operator >> ( char *buf ) {
    /*******************************************/
    // Input a string of non-whitespace characters into the buffer. If
    // the width is set, read a maximum of that many characters, less
    // one for the NULLCHAR on the end. Otherwise, keep reading until
    // EOF or a whitespace character.

    int c;
    int offset;
    int last_offset;

    offset = 0;
    __lock_it( __i_lock );
    if( ipfx() ) {
        last_offset = width() - 2;
        __lock_it( rdbuf()->__b_lock );
        for( ;; ) {
            if( (width() != 0) && (offset > last_offset) ) {
                break;
            }
            c = rdbuf()->sgetchar();
            if( c == EOF ) {
                if( offset == 0 ) {
                    __WATCOM_ios::setstate( this, ios::eofbit );
                }
                break;
            }
            if( isspace( c ) ) {
                rdbuf()->sputbackc( c );
                break;
            }
            buf[offset++] = (unsigned char)c;
        }
        isfx();
    }
#if 1
    // Borland's implementation does not set failbit if no
    // characters are read.
    if( offset == 0 ) {
        __WATCOM_ios::setstate( this, ios::failbit );
    }
#endif
    width( 0 );
    buf[offset] = '\0';
    return( *this );
  }

  istream &istream::operator >> ( char &c ) {
    /*****************************************/
    // Input a character into "c".

    int ch;

    __lock_it( __i_lock );
    if( ipfx() ) {
        ch = rdbuf()->sgetchar();
        if( ch == EOF ) {
            __WATCOM_ios::setstate( this, ios::eofbit|ios::failbit );
            c = ERR_CHAR;
        } else {
            c = (char)ch;
        }
        isfx();
    }
    return( *this );
  }

  istream &istream::operator >> ( signed short &s ) {
    /*************************************************/

    signed long number;

    __lock_it( __i_lock );
    if( good() ) {
        *this >> number;
        if( number >= 0 ) {
            if( number <= (long)SHRT_MAX ) {
                s = (signed short)number;
            } else {
                __WATCOM_ios::setstate( this, ios::failbit );
            }
        } else {
            if( number >= (long)SHRT_MIN ) {
                s = (signed short)number;
            } else {
                __WATCOM_ios::setstate( this, ios::failbit );
            }
        }
    }
    return( *this );
  }

  istream &istream::operator >> ( unsigned short &s ) {
    /***************************************************/

    unsigned long number;

    __lock_it( __i_lock );
    *this >> number;
    if( good() ) {
        if( number <= USHRT_MAX ) {
            s = (unsigned short)number;
        } else {
            __WATCOM_ios::setstate( this, ios::failbit );
        }
    }
    return( *this );
  }
  
  istream &istream::operator >> ( signed int &i ) {
    /***********************************************/
#if UINT_MAX == USHRT_MAX
    return( *this >> (signed short &) i );
#else
    return( *this >> (signed long &) i );
#endif
  }

  istream &istream::operator >> ( unsigned int &i ) {
    /*************************************************/
#if UINT_MAX == USHRT_MAX
    return( *this >> (unsigned short &) i );
#else
    return( *this >> (unsigned long &) i );
#endif
  }

}

static ios::iostate getsign( streambuf *sb, char &sign, int &base ) {
/*******************************************************************/
// Examine the input for a + or - sign character.

    int ch;

    __lock_it( sb->__b_lock );
    ch = sb->speekc();
    if( ch == EOF ) {
        return( ios::eofbit );
    }
    sign = (char)ch;
    if( (sign == '-') || (sign == '+') ) {
        if( base ) {
            if( base != 10 ) {
                return( ios::failbit );
            }
        } else {
            base = 10;
        }
    } else {
        sign = 0;
    }
    return( ios::goodbit );
}

static ios::iostate getbase( streambuf *sb, int &base, int &offset ) {
/********************************************************************/

    int ch;

    __lock_it( sb->__b_lock );
    ch = sb->speekc();
    if( ch == EOF ) {
        if( offset == 0 ) {
            return( ios::eofbit );
        }
        return( ios::failbit );
    }
    if( ch != '0' ) {
        base = 10;
    } else {
        offset++;
        ch = sb->snextc();
        if( tolower( ch ) == 'x' ) {
            base = 16;
            sb->sbumpc();
            offset++;
        } else {
            base = 8;
            sb->sputbackc( '0' );
            offset--;
        }
    }
    return( ios::goodbit );
}

static ios::iostate getnumber( streambuf *sb, unsigned long &number,
/******************************************************************/
    int base, int &offset ) {
// Extract digits from the stream.
// Stop when a non-digit is found, leaving the non-digit in the stream.
// As digits are read, convert to an "unsigned long".

    unsigned long  result;
    unsigned long  overflow;
    ios::iostate   state;
    int            ch;
    int            is_digit;
    int            digit_value;
    int            shift1;
    int            shift2;

    if( base == 8 ) {
        shift1 = 3;     // *8
        shift2 = 0;
    } else if( base == 16 ) {
        shift1 = 4;     // *16
        shift2 = 0;
    } else {
        shift1 = 3;     // *8
        shift2 = 1;     // *2
    }
    state    = ios::goodbit;
    result   = 0;
    overflow = 0;
    is_digit = true;
    __lock_it( sb->__b_lock );
    while( is_digit ) {
        ch = sb->speekc();
        if( ch == EOF ) {
            if( offset == 0 ) {
                state |= ios::eofbit;
            }
        }
        digit_value = ch - '0';
        if( base == 8 ) {
            is_digit = (ch >= '0'  &&  ch <= '7');
        } else {
            is_digit = isdigit( ch );
            if( base == 16 && !is_digit ) {
                char low_char;
                low_char = (char)tolower( ch );
                if( low_char >= 'a'  &&  low_char <= 'f' ) {
                    digit_value = low_char - 'a' + 10;
                    is_digit    = true;
                }
            }
        }
        if( is_digit ) {
            unsigned long tmp_result = result;
            overflow  |= (result & overFlowMasks[shift1]);
            result   <<= shift1;
            if( shift2 > 0 ) {
                tmp_result <<= shift2;
                if( result > (ULONG_MAX - tmp_result) ) overflow = 1;
                result += tmp_result;
            }
            if( result > ULONG_MAX - digit_value ) overflow = 1;
            result += digit_value;
            sb->sbumpc();
            offset++;
        }
    }
    if( overflow != 0 ) {
        state |= ios::failbit;
    }
    number = result;
    return( state );
}

namespace std {

  istream &istream::operator >> ( signed long &l ) {
    /************************************************/

    char          sign;
    int           base;
    int           offset;
    ios::iostate  state;
    ios::fmtflags format;
    unsigned long number;

    state = ios::goodbit;
    offset = 0;
    __lock_it( __i_lock );
    if( ipfx() ) {
        __lock_it( rdbuf()->__b_lock );
        format = flags() & ios::basefield;
        if( format ) {
            base = __FlagsToBase( format );
        } else {
            base = 0;
        }
        state = getsign( rdbuf(), sign, base );
        if( !state ) {
            if( sign ) {
                rdbuf()->sbumpc();
                offset++;
            }
            if( !base ) {
                state = getbase( rdbuf(), base, offset );
            }
        }
        if( !state ) {
            state = getnumber( rdbuf(), number, base, offset );
        }
        if( !state ) {
            if( sign == '-' ) {
                if( number <= -LONG_MIN ) {
                    l = -number;
                } else {
                    state |= ios::failbit;
                }
            } else {
                if( number <= LONG_MAX ) {
                    l = number;
                } else {
                    state |= ios::failbit;
                }
            }
        }
        isfx();
    }
    if( offset == 0 ) {
        state |= ios::failbit;
    }
    __WATCOM_ios::setstate( this, state );
    return( *this );
  }

  istream &istream::operator >> ( unsigned long &l ) {
    /**************************************************/

    unsigned long  number;
    ios::iostate   state;
    ios::fmtflags  format;
    char           sign;
    int            base;
    int            offset;

    state = ios::goodbit;
    offset = 0;
    __lock_it( __i_lock );
    if( ipfx() ) {
        __lock_it( rdbuf()->__b_lock );
        format = flags() & ios::basefield;
        if( format ) {
            base = __FlagsToBase( format );
        } else {
            base = 0;
        }
        state = getsign( rdbuf(), sign, base );
        if( sign == '-' ) {
            state |= ios::failbit;
        }
        if( !state ) {
            if( sign ) {
                rdbuf()->sbumpc();
                offset++;
            }
            if( !base ) {
                state = getbase( rdbuf(), base, offset );
            }
        }
        if( !state ) {
            state = getnumber( rdbuf(), number, base, offset );
        }
        if( !state ) {
            l = number;
        }
        isfx();
    }
    if( offset == 0 ) {
        state |= ios::failbit;
    }
    __WATCOM_ios::setstate( this, state );
    return( *this );
  }

  istream &istream::operator >> ( streambuf *tgt_sb ) {
    /***************************************************/
    // Read all characters from the istream and write them to the streambuf.

    char buf[DEFAULT_MAINBUF_SIZE];
    int  len;

    __lock_it( __i_lock );
    if( ipfx( 1 ) ) {
        streambuf *src_sb;
        src_sb = rdbuf();
        __lock_it( __lock_first( src_sb->__b_lock, tgt_sb->__b_lock ) );
        __lock_it( __lock_second( src_sb->__b_lock, tgt_sb->__b_lock ) );
        do {
            read( buf, sizeof( buf ) );
            len = gcount();
        } while( (len > 0) && (tgt_sb->sputn( buf, len ) == len) );
        isfx();
    }
    return( *this );
  }

  istream &istream::operator >> ( ios &(*f)( ios & ) ) {
    /****************************************************/
    // Handles things like
    //     cin >> ws
    // where "ws" is a function taking an ios reference and returning the
    // same. Essentially, this just does an indirect call to the function.

    f( *this );
    return( *this );
  }

  istream &istream::operator >> ( istream &(*f)( istream & ) ) {
    /************************************************************/
    // Handles things like
    //     cin >> ws
    // where "ws" is a function taking an ios reference and returning the
    // same. Essentially, this just does an indirect call to the function.

    return( f( *this ) );
  }

  int istream::get() {
    /******************/
    // Extract a single character from the input stream.
    // Don't set ios::failbit.

    int          c = EOF;

    __lock_it( __i_lock );
    if( ipfx( 1 ) ) {
        c = rdbuf()->sgetchar();
        if( c == EOF ) {
            __last_read_length = 0;
            __WATCOM_ios::setstate( this, ios::eofbit );
            return( EOF );
        }
        __last_read_length = 1;
        isfx();
    } else {
        __last_read_length = 0;
    }
    return( c );
  }

  istream &istream::get( char &ch ) {
    /*********************************/
    // Extract a single character and store it in "ch".

    int c;

    __lock_it( __i_lock );
    if( ipfx( 1 ) ) {
        c = rdbuf()->sgetchar();
        if( c == EOF ) {
            __last_read_length = 0;
            __WATCOM_ios::setstate( this, ios::eofbit|ios::failbit );
            ch = ERR_CHAR;
            return( *this );
        }
        ch = (char)c;
        __last_read_length = 1;
        isfx();
    } else {
        __last_read_length = 0;
    }
    return( *this );
  }

}

static ios::iostate getaline( istream &istrm, char *buf, int len,
/***************************************************************/
    char delim, int is_get, int &chars_read ) {
// Read characters into buffer "buf".
// At most "len - 1" characters are read, and a 0 is added at the end.
// If "delim" is encountered, it is left in the stream and the read is
// terminated (and the NULLCHAR is added).
// Used by:
//    get( char *buf, int len, char delim )
//    getline( char *buf, int len, char delim )
//
// NOTE: Borland sets eofbit only. A full buffer just stops reading.
//       If something has been read, set eofbit anyway.
//
//       The proposed standard says to set failbit if the buffer is filled
//       without finding the delimiter (if doing a "getline"), or if a read
//       fails and no characters are extracted. It says nothing about eofbit.
//
//       Currently we set eofbit only if eof occurs on first character read.
//       failbit is set if no characters were read.

    int           c;
    int           offset;
    ios::iostate  state = 0;
    streambuf    *sb;

    offset = 0;
    __lock_it( istrm.__i_lock );
    if( len > 1 && istrm.ipfx( 1 ) ) {
        sb = istrm.rdbuf();
        __lock_it( sb->__b_lock );
        len--;  // leave a space for the NULLCHAR
        while( offset < len ) {
            c = sb->sgetchar();
            if( c == EOF ) {
                if( offset == 0 ) {
                    state |= ios::eofbit;
                }
                break;
            }
            if( c == delim ) {
                if( is_get ) {
                    sb->sputbackc( c );
                }
                break;
            }
            buf[offset++] = (char)c;
        }
        istrm.isfx();
    }
    buf[offset]  = '\0';
    chars_read = offset;

    // the draft standard says that no characters is an
    // error if using get() or getline().
    // the IOStreams Handbook suggests that no characters is not
    // an error if the delim was seen, this seems to be what our
    // competitors do.
    if( (offset == 0) && (c != delim) ) {
        state |= ios::failbit;
    }
    // the draft standard says that len-1 characters is an
    // error if using getline()
    if( offset == len && !is_get ) {
        state |= ios::failbit;
    }
    return( state );
}

namespace std {

  istream &istream::get( char *buf, int len, char delim ) {
  /*****************************************************/

    ios::iostate state;

    __lock_it( __i_lock );
    state = getaline( *this, buf, len, delim, true, __last_read_length );
    __WATCOM_ios::setstate( this, state );
    return( *this );
  }

  istream &istream::getline( char *buf, int len, char delim ) {
    /*********************************************************/

    ios::iostate state;

    __lock_it( __i_lock );
    state = getaline( *this, buf, len, delim, false, __last_read_length );
    __WATCOM_ios::setstate( this, state );
    return( *this );
  }

  istream &istream::read( char *buf, int len ) {
    /********************************************/
    // Read up to "len" characters from the stream and store them in buffer "buf".

    int offset;

    __lock_it( __i_lock );

    offset = 0;
    if( ipfx( 1 ) ) {
        offset = rdbuf()->sgetn( buf, len );
        if( offset < len ) {
            if( offset == 0 ) {
                __WATCOM_ios::setstate( this, ios::eofbit|ios::failbit );
            } else {
                __WATCOM_ios::setstate( this, ios::failbit );
            }
        }
        isfx();
    }
    __last_read_length = offset;
    return( *this );
  }

  istream &istream::get( streambuf &tgt_sb, char delim ) {
    /******************************************************/
    // Extract characters from our streambuf and store them into the
    // specified streambuf.

    streambuf *src_sb;
    int        c;

    __lock_it( __i_lock );
    if( !ipfx( 1 ) ) {
        __last_read_length = 0;
        return( *this );
    }
    src_sb = rdbuf();
    __lock_it( __lock_first( src_sb->__b_lock, tgt_sb.__b_lock ) );
    __lock_it( __lock_second( src_sb->__b_lock, tgt_sb.__b_lock ) );
    for( ;; ) {
        c = src_sb->sgetchar();
        if( c == EOF ) {
            if( __last_read_length == 0 ) {
                __WATCOM_ios::setstate( this, ios::eofbit );
            }
            break;
        }
        if( c == delim ) {
            src_sb->sputbackc( c );
            break;
        }
        ++__last_read_length;
        if( tgt_sb.sputc( c ) == EOF ) {
            __WATCOM_ios::setstate( this, ios::failbit );
            break;
        }
    }
    isfx();
    return( *this );
  }

  istream &istream::ignore( int n, int delim ) {
    /********************************************/
    // Ignore "n" characters, or until the specified delimiter is found,
    // whichever comes first. If "delim" is EOF, don't look for a
    // delimiter. As an extension, specifying a negative "n" value will
    // not count ignored characters and will continue ignoring until the
    // delimiter is found.

    int c;

    __lock_it( __i_lock );
    if( !ipfx( 1 ) ) {
        __last_read_length = 0;
        return( *this );
    }
    __lock_it( rdbuf()->__b_lock );
    for( ;; ) {
        if( n >= 0 ) {
            if( --n < 0 ) break;
        }
        c = rdbuf()->sgetchar();
        if( c == EOF ) break;
        if( (delim != EOF) && (c == delim) ) break;
    }
    isfx();
    return( *this );
  }

  int istream::peek() {
    /*******************/
    // Return the next character without extracting it from the stream.

    int c;

    __lock_it( __i_lock );
    if( ipfx( 1 ) ) {
        c = rdbuf()->speekc();
        if( c == EOF ) {
            __WATCOM_ios::setstate( this, ios::eofbit );
        }
        isfx();
    } else {
        c = EOF;
    }
    return( c );
  }

  istream &istream::putback( char c ) {
    /***********************************/
    // Put character in "c" back into the stream.

    __lock_it( __i_lock );
    if( fail() ) {
        return( *this );
    }
    if( rdbuf()->sputbackc( c ) == EOF ) {
        setf( ios::failbit );
    }
    return( *this );
  }

  int istream::sync() {
    /*******************/
    // Not inline because it is virtual
    return( rdbuf()->sync() );
  }

  istream &istream::seekg( streampos pos ) {
    /****************************************/

    __lock_it( __i_lock );
    if( ipfx( 1 ) ) {
        if( rdbuf()->seekoff( pos, ios::beg, ios::in ) == EOF ) {
            __WATCOM_ios::setstate( this, ios::failbit );
        }
        isfx();
    }
    return( *this );
  }

  istream &istream::seekg( streamoff offset, ios::seekdir dir ) {
    /*************************************************************/

    __lock_it( __i_lock );
    if( ipfx( 1 ) ) {
        if( rdbuf()->seekoff( offset, dir, ios::in ) == EOF ) {
            __WATCOM_ios::setstate( this, ios::failbit );
        }
        isfx();
    }
    return( *this );
  }

  streampos istream::tellg() {
    /**************************/

    __lock_it( __i_lock );
    if( fail() ) {
        return( EOF );
    }
    return( rdbuf()->seekoff( 0, ios::cur, ios::in ) );
  }

  void istream::eatwhite() {
    /************************/
    // Skip any leading whitespace characters.

    __lock_it( __i_lock );
    if( ipfx( 1 ) ) {
        ws( *this );
        isfx();
    }
  }

}
