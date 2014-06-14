//
// ios.hpp
//

#ifndef _IOS_HPP_INCLUDED
#define _IOS_HPP_INCLUDED

namespace stream {
    typedef long long streamoff;  // Signed type for representing file sizes.

    // TODO: The type 'int' should be replaced with a platform specific type related to size_t.
    typedef int streamsize; // Signed type for representing buffer sizes in memory.

    // TODO: Declare ios_base fully.
    class ios_base;

    // TODO: Declare basic_ios fully.
    // template< typename CharT, typename traits = char_traits<CharT> >
    // class basic_ios;

    // 27.5.6, manipulators:
    ios_base &boolalpha  ( ios_base &str );
    ios_base &noboolalpha( ios_base &str );
    ios_base &showbase   ( ios_base &str );
    ios_base &noshowbase ( ios_base &str );
    ios_base &showpoint  ( ios_base &str );
    ios_base &noshowpoint( ios_base &str );
    ios_base &showpos    ( ios_base &str );
    ios_base &noshowpos  ( ios_base &str );
    ios_base &skipws     ( ios_base &str );
    ios_base &noskipws   ( ios_base &str );
    ios_base &uppercase  ( ios_base &str );
    ios_base &nouppercase( ios_base &str );
    ios_base &unitbuf    ( ios_base &str );
    ios_base &nounitbuf  ( ios_base &str );

    // 27.5.6.2, adjustfield:
    ios_base &internal   ( ios_base &str );
    ios_base &left       ( ios_base &str );
    ios_base &right      ( ios_base &str );

    // 27.5.6.3, basefield:
    ios_base &dec        ( ios_base &str );
    ios_base &hex        ( ios_base &str );
    ios_base &oct        ( ios_base &str );

    // 27.5.6.4, floatfield:
    ios_base &fixed      ( ios_base &str );
    ios_base &scientific ( ios_base &str );
    ios_base &hexfloat   ( ios_base &str );
    ios_base &defaultfloat( ios_base &str );

}

#endif
