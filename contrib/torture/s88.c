int *metricp;
s88(pd0)          /*  8.8 Typedef  */
#include "defs"
struct defs *pd0;
{
   static char s88er[] = "s88,er%d\n";
   static char qs88[8] = "s88    ";
   int rc;
   char *ps, *pt;

        /* Declarations whose "storage class" is typdef do not
        define storage, but instead define identifiers which
        can later be used as if they were type keywords naming
        fundamental or derived types.
                                                                */

   typedef int MILES, *KLICKSP;
   typedef struct {double re, im;} complex;

   MILES distance;
   extern KLICKSP metricp;
   complex z, *zp;

   ps = qs88;
   pt = pd0->rfs;
   rc = 0;
   while(*pt++ = *ps++);

        /* Hopefully, all of this stuff will compile. After that,
        we can only make some superficial tests.

        The type of distance is int,
                                                                */

   if(sizeof distance != sizeof(int)){
     if(pd0->flgd != 0) printf(s88er,1);
     rc = rc+1;
   }

        /* that of metricp is "pointer to int",                 */

   metricp = &distance;
   distance = 2;
   *metricp = 3;

   if(distance != 3){
     if(pd0->flgd != 0) printf(s88er,2);
     rc = rc+2;
   }

        /* and that of z is the specified structure. zp is a
        pointer to such a structure.
                                                                */

   z.re = 0.;
   z.im = 0.;
   zp = &z;
   zp->re = 1.;
   zp->im = 1.;
   if(z.re+z.im != 2.){
     if(pd0->flgd != 0) printf(s88er,4);
     rc = rc+4;
   }

   return rc;
}
