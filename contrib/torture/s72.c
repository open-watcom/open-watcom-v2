s72(pd0)          /*  7.2  Unary operators  */
#include "defs"
struct defs *pd0;
{
   static char s72er[] = "s72,er%d\n";
   static char qs72[8] = "s72    ";
   int rc;
   char *ps, *pt;
   int k, j, i, lrc;
   char c;
   short s;
   long l;
   unsigned u;
   double d;
   float f;
   ps = qs72;
   pt = pd0->rfs;
   rc = 0;
   while (*pt++ = *ps++);

        /* The *, denoting indirection, and the &, denoting a
        pointer, are duals of each other, and ought to behave as 
        such...                                                 */

   k = 2;
   if(*&*&k != 2){
     rc = rc+1;
     printf(s72er,1);
   }

        /* The unary minus has the conventional meaning.        */

   if(k+(-k) != 0){
     rc = rc+2;
     printf(s72er,2);
   }

        /*  The negation operator (!) has been thoroughly checked out,
        perhaps more thoroughly than any of the others. The ~ oper-
        ator gets us a ones complement.                         */

   k = 0;
   for(j=0;j<pd0->ibits;j++) k = (k<<1)|1;
   if(~k != 0){
     rc = rc+4;
     printf(s72er,4);
   }

        /*  Now we look at the ++ and -- operators, which can be
        used in either prefix or suffix form. With side
        effects they're loaded.                                 */

   k = 5;

   if( ++k != 6 || --k != 5
    || k++ != 5 || k-- != 6
    ||   k != 5 ){
     rc = rc+8;
     printf(s72er,8);
   }

        /*  An expression preceded by the parenthesised name of a
        data type causes conversion of the value of the expression
        to the named type. This construction is called a cast.
        Here, we check to see that all of the possible casts and
        their simple combinations are accepted by the compiler,
        and that they all produce a correct result for this sample
        of size one.                                            */

   c = 26;  l = 26;  d = 26.;
   s = 26;  u = 26; 
   i = 26;  f = 26.;

   lrc = 0;

   if( (char)s != 26 || (char)i != 26
    || (char)l != 26 || (char)u != 26
    || (char)f != 26 || (char)d != 26 ) lrc = lrc+1;

   if( (short)c != 26 || (short)i != 26
    || (short)l != 26 || (short)u != 26
    || (short)f != 26 || (short)d != 26) lrc = lrc+2;

   if( (int)c != 26 || (int)s != 26
    || (int)l != 26 || (int)u != 26
    || (int)f != 26 || (int)d != 26 ) lrc = lrc+4;

   if( (long)c != 26 || (long)s != 26
    || (long)i != 26 || (long)u != 26
    || (long)f != 26 || (long)d != 26 ) lrc = lrc+8;

   if( (unsigned)c != 26 || (unsigned)s != 26
    || (unsigned)i != 26 || (unsigned)l != 26
    || (unsigned)f != 26 || (unsigned)d != 26 ) lrc = lrc+16;

   if( (float)c != 26. || (float)s != 26.
    || (float)i != 26. || (float)l != 26.
    || (float)u != 26. || (float)d != 26. ) lrc = lrc+32;

   if( (double)c != 26. || (double)s != 26.
    || (double)i != 26. || (double)l != 26.
    || (double)u != 26. || (double)f != 26. ) lrc = lrc+64;

   if(lrc != 0){
     rc = rc+16;
     printf(s72er,16);
   }

        /*  The sizeof operator has been tested previously.     */

   return rc;
}
