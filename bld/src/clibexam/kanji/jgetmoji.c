#include <stdio.h>
#include <jstring.h>

const JCHAR set[] = {
    "ab\x81\x41\x81\x42\cd\x81"
};

void main()
  {
    JMOJI c;
    const JCHAR *str;

    str = set;
    for( ; *str != '\0'; ) {
        str = jgetmoji( str, &c );
        printf( "Character code 0x%2.2x\n", c );
    }
  }
