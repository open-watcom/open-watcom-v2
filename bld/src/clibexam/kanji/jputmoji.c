#include <stdio.h>
#include <jstring.h>

void main()
  {
    JMOJI c;
    JCHAR *str1;
    JCHAR *str2;
    JCHAR buf[30];

    str1 = "abCDef";
    str2 = buf;

    for( ; *str1 != '\0'; ) {
        str1 = jgetmoji( str1, &c );
        str2 = jputmoji( str2, '<' );
        str2 = jputmoji( str2, c );
        str2 = jputmoji( str2, '>' );
    }
    *str2 = '\0';
    printf( "%s\n", buf );
  }
