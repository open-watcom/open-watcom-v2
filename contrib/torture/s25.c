s25(pd0)
#include "defs"
struct defs *pd0;
{
   char *s, *s2;
   int rc, lrc, j;
   static char s25er[] = "s25,er%d\n";
   static char qs25[8] = "s25    ";
   char *ps, *pt;

   ps = qs25;
   pt = pd0->rfs;
   while(*pt++ = *ps++);
   rc = 0;

   /* A string is a sequence of characters surrounded by double
      quotes, as in "...".                         */

   s = "...";

   /* A string has type "array of characters" and storage class
      static and is initialized with the given characters.  */

   if ( s[0] != s[1] || s[1] != s[2]
     || s[2] != '.' ) {

    rc = rc+1;
     if(pd0->flgd != 0) printf(s25er,1);
   }

#if 0
   /* All strings, even when written identically, are distinct. */

   "..."[1] = '?';
   if( s[1] != '.' ){
     rc = rc+2;
     if(pd0->flgd != 0) printf(s25er,2);
   }
#endif

   /* The compiler places a null byte \0 at the end of each string
      so the program which scans the string can find its end.   */

   if( s[3] != '\0' ){
     rc = rc+4;
     if(pd0->flgd != 0) printf(s25er,4);
   }

   /* In a string, the double quote character " must be preceded
      by a \.                                               */

    if( ".\"."[1] != '"' ){
    rc = rc+8;
     if(pd0->flgd != 0) printf(s25er,8);
   }

   /* In addition, the same escapes described for character constants
      may be used.                                            */

   s = "\n\t\b\r\f\\\'";

   if( s[0] != '\n'
    || s[1] != '\t'
    || s[2] != '\b'
    || s[3] != '\r'
    || s[4] != '\f'
    || s[5] != '\\'
    || s[6] != '\'' ){

     rc = rc+16;
     if( pd0->flgd != 0) printf(s25er,16);
   }

   /* Finally, a \ and an immediately following newline are ignored */

   s2 = "queep!";
   s = "que\
ep!";

   lrc = 0;
   for (j=0; j<sizeof "queep!"; j++) if(s[j] != s2[j]) lrc = 1;
   if (lrc != 0){
     rc = rc+32;
     if(pd0->flgd != 0) printf(s25er,32);
   }
   return rc;
}
