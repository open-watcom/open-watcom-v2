s715(pd0)          /*  7.15 Comma operator     */
#include "defs"
struct defs *pd0;
{
   static char s715er[] = "s715,er%d\n";
   static char qs715[8] = "s715   ";
   int rc;
   char *ps, *pt;
   int a, t, c, i;
   a = c = 0;
   ps = qs715;
   pt = pd0->rfs;
   rc = 0;
   while (*pt++ = *ps++);

        /* A pair of expressions separated by a comma is
        evaluated left to right and the value of the left
        expression is discarded.
                                                                */
   i = 1;
   if( i++,i++,i++,i++,++i != 6 ){
     if(pd0->flgd != 0) printf(s715er,1);
     rc = rc+1;
   }

        /* In contexts where the comma is given a special mean-
        ing, for example in a list of actual arguments to 
        functions (sic) and lists of initializers, the comma
        operator as described in this section can only appear
        in parentheses; for example

                f( a, (t=3, t+2), c)

        has three arguments, the second of which has the
        value 5.
                                                                */

   if(s715f(a, (t=3, t+2), c) != 5){
     if(pd0->flgd != 0) printf(s715er,2);
     rc = rc+2;
   }
   return rc;
}
s715f(x,y,z)
int x, y, z;
{
   return y;
}
