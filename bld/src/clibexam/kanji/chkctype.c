#include <stdio.h>
#include <jstring.h>

static void checkKanji( JMOJI c )
  {
    if( chkctype( c >> 8, CT_ANK ) == CT_KJ1 ) {
      if( chkctype( c & 0xFF, CT_KJ1 ) == CT_KJ2 ) {
        printf( "Char is double-byte Kanji\n" );
      } else {
        printf( "Char is illegal\n" );
      }
    } else {
        printf( "Char is not double-byte Kanji\n" );
    }
  }

void main()
  {
    checkKanji( 'a' );
    checkKanji( (0x81<<8) + 0x55 );
    checkKanji( (0x81<<8) + 0x10 );
  }
