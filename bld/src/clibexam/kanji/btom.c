#include <stdio.h>
#include <jstring.h>

const JCHAR date_str[] = {
    "YYYY \x94\x4e MM \x8c\x8e DD \x93\xfa"
    " \x81\x69WW\x81\x6a HH:MM:SS"
};

void main()
  {
    printf( "%d characters found\n",
            btom( date_str, 35 ) );
  }
