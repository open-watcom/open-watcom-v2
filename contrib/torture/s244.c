s244(pd0)
#include "defs"
struct defs *pd0;
{
   double a[8];
   int rc, lrc, j;
   static char s244er[] = "s244,er%d\n";
   static char qs244[8] = "s244   ";
   char *ps, *pt;

   ps = qs244;
   pt = pd0->rfs;
   while(*pt++ = *ps++);
   rc = 0;
   lrc = 0;

   /* Unfortunately, there's not a lot we can do with floating constants.
      We can check to see that the various representations can be com-
      piled, that the conversion is such that they yield the same hard-
      ware representations in all cases, and that all representations
      thus checked are double precision.              */

   a[0] = .1250E+04;
   a[1] = 1.250E3;
   a[2] = 12.50E02;
   a[3] = 125.0e+1;
   a[4] = 1250e00;
   a[5] = 12500.e-01;
   a[6] = 125000e-2;
   a[7] = 1250.;

   lrc = 0;
   for (j=0; j<7; j++) if(a[j] != a[j+1]) lrc = 1;

   if(lrc != 0) {
     if(pd0->flgd != 0) printf(s244er,1);
     rc = rc+1;
   }

   if ( (sizeof .1250E+04 ) != sizeof(double)
     || (sizeof 1.250E3   ) != sizeof(double)
     || (sizeof 12.50E02  ) != sizeof(double)
     || (sizeof 1.250e+1  ) != sizeof(double)
     || (sizeof 1250e00   ) != sizeof(double)
     || (sizeof 12500.e-01) != sizeof(double)
     || (sizeof 125000e-2 ) != sizeof(double)
     || (sizeof 1250.     ) != sizeof(double)){
     
     if(pd0->flgd != 0) printf(s244er,2);
     rc = rc+2;
   }

   return rc;
}
