s71(pd0)          /*         7.1  Primary expressions   */
#include "defs"
struct defs *pd0;
{
   static char s71er[] = "s71,er%d\n";
   static char qs71[8] = "s71    ";
   int rc;
   char *ps, *pt;
   static char q = 'q';
   int x[10], McCarthy(), clobber(), a, b, *p;
   ps = qs71;
   pt = pd0->rfs;
   rc = 0;
   while (*pt++ = *ps++);

/*   Testing of expressions and operators is quite complicated,
     because (a) problems are apt to surface in queer combinations
     of operators and operands, rather than in isolation,
     and (b) the number of expressions needed to provoke a case
     of improper behaviour may be quite large. Hence, we take the
     following approach: for this section, and for subsequent
     sections through 7.15, we will check the primitive operations
     in isolation, thus verifying that the primitives work,
     after a fashion. The job of testing combinations, we will
     leave to a separate, machine-generated program, to be included
     in the C test package at some later date.
                                                                */

/*   A string is a primary expression. The identifier points to
     the first character of a string.
                                                                  */

   if(*"queep" != q){
     rc = rc+1;
     if(pd0->flgd  != 0) printf(s71er,1);
   }
/*   A parenthesized expression is a primary expression whose
     type and value are the same as those of the unadorned
     expression.
                                                                */
   if((2+3) != 2+3) {
     rc = rc+2;
     if(pd0->flgd != 0) printf(s71er,2);
   }

/*   A primary expression followed by an expression in square 
     brackets is a primary expression. The intuitive meaning is
     that of a subscript. The expression E1[E2] is identical
     (by definition) to *((E1)+(E2)).
                                                                */

   x[5] = 1942;
   if(x[5] != 1942 || x[5] != *((x)+(5))){
     rc = rc+4;
     if(pd0->flgd != 0) printf(s71er,4);
   }

/*   If the various flavors of function calls didn't work, we
     would never have gotten this far; however, we do need to 
     show that functions can be recursive...
                                                               */

   if ( McCarthy(-5) != 91){
     rc = rc+8;
     if(pd0->flgd != 0) printf(s71er,8);
   }

/*   and that argument passing is strictly by value.           */

   a = 2;
   b = 3;
   p = &b;

   clobber(a,p);

   if(a != 2 || b != 2){
     rc = rc+16;
     if(pd0->flgd != 0) printf(s71er,16);
   }

/*   Finally, structures and unions are addressed thusly:      */

   if(pd0->dprec != (*pd0).dprec){
     rc = rc+32;
     if(pd0->flgd != 0) printf(s71er,32);
   }

   return rc;
}
McCarthy(x)
int x;
{
   if(x>100) return x-10;
   else return McCarthy( McCarthy(x+11));
}
clobber(x,y)
int x, *y;
{
   x = 3;
   *y = 2;
}
