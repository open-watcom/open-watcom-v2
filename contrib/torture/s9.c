s9(pd0)          /*  9  Statements  */
#include "defs"
struct defs *pd0;
{
   static char s9er[] = "s9,er%d\n";
   static char qs9[8] = "s9     ";
   int rc;
   char *ps, *pt;
   int lrc, i;

   ps = qs9;
   pt = pd0->rfs;
   rc = 0;
   while (*pt++ = *ps++);

        /* One would think that the section on statements would
        provide the most variety in the entire sequence of tests.
        As it turns out, most of the material in this section has 
        already been checked in the process of checking out
        everything else, and the section at this point is somewhat
        anticlimactic. For this reason, we restrict ourselves
        to testing two features not already covered.

        Compound statements are delimited by braces. They have the
        nice property that identifiers of the auto and register
        variety are pushed and popped. It is currently legal to
        transfer into a block, but we wont...
                                                                */

   lrc = 0;
   for(i=0; i<2; i++){
     int j;
     register int k;
     j = k = 2;
       {
       int j;
       register int k;
       j = k = 3;
       if((j != 3) || (k != 3)) lrc = 1;
       }
     if((j != 2) || (k != 2)) lrc = 1;
   }

   if(lrc != 0){
     if(pd0->flgd != 0) printf(s9er,1);
     rc = rc+1;
   }

        /* Goto statements go to labeled statements, we hope.   */

   goto nobarf;
     if(pd0->flgd != 0) printf(s9er,2);
     rc = rc+2;
   nobarf:;

   return rc;
}
