s626(pd0)          /* 6.2 Float and double                  */
                   /* 6.3 Floating and integral                 */
                   /* 6.4 Pointers and integers                 */
                   /* 6.5 Unsigned                              */
                   /* 6.6 Arithmetic conversions                */
#include "defs"
struct defs *pd0;
{
   static char s626er[] = "s626,er%d\n";
   static char qs626[8] = "s626   ";
   int rc;
   char *ps, *pt;
   float eps, f1, f2, f3, f4, f;
   long lint1, lint2, l, ls;
   char c, t[28], t0;
   short s;
   int is, i, j;
   unsigned u, us;
   double d, ds;
   ps = qs626;
   pt = pd0->rfs;
   rc = 0;
   while (*pt++ = *ps++);

        /* All floating arithmetic in C is carried out to double
        precision. (Provided, of course, that the host machine's
        hardware is so equipped.)                               */

   if (sizeof(float) != sizeof(double)){

     eps = (pd0->fprec)/2.;
     f1 = 1.;
     f2 = f1+pd0->fprec;
     f3 = f1+eps;
     f4 = f1+eps+eps+eps;

     if( f2<=f1 || f3!=f1 || f4<=f1 ){

       rc = rc+1;
       if(pd0->flgd != 0) printf(s626er,1);
     }
   }

        /* Conversions of integral values to floating type are
        well-behaved.                                           */

   f1 = 1.;
   lint1 = 1.;
   lint2 = 1.;

   for(j=0;j<pd0->lbits-2;j++){
     f1 = f1*2;
     lint2 = (lint2<<1)|lint1;
   }
   f2 = lint2;
   f1 = (f1-f2)/f1;
   if(f1>2.*pd0->fprec){

     rc = rc+2;
     if(pd0->flgd != 0) printf(s626er,2);
   }

        /* Pointer-integer combinations are discussed in s74,
        "Additive operators". The unsigned-int combination
        appears below.                                          */

   c = 125;
   s = 125;
   i = 125;     is = 15625;
   u = 125;     us = 15625;
   l = 125;     ls = 15625;
   f = 125.;
   d = 125.;    ds = 15625.;

   for(j=0;j<28;j++) t[j] = 0;

   if(c*c != is) t[ 0] = 1;
   if(s*c != is) t[ 1] = 1;
   if(s*s != is) t[ 2] = 1;
   if(i*c != is) t[ 3] = 1;
   if(i*s != is) t[ 4] = 1;
   if(i*i != is) t[ 5] = 1;
   if(u*c != us) t[ 6] = 1;
   if(u*s != us) t[ 7] = 1;
   if(u*i != us) t[ 8] = 1;
   if(u*u != us) t[ 9] = 1;
   if(l*c != ls) t[10] = 1;
   if(l*s != ls) t[11] = 1;
   if(l*i != ls) t[12] = 1;
   if(l*u != us) t[13] = 1;
   if(l*l != ls) t[14] = 1;
   if(f*c != ds) t[15] = 1;
   if(f*s != ds) t[16] = 1;
   if(f*i != ds) t[17] = 1;
   if(f*u != ds) t[18] = 1;
   if(f*l != ds) t[19] = 1;
   if(f*f != ds) t[20] = 1;
   if(d*c != ds) t[21] = 1;
   if(d*s != ds) t[22] = 1;
   if(d*i != ds) t[23] = 1;
   if(d*u != ds) t[24] = 1;
   if(d*l != ds) t[25] = 1;
   if(d*f != ds) t[26] = 1;
   if(d*d != ds) t[27] = 1;

   t0 = 0;
   for(j=0; j<28; j++) t0 = t0+t[j];

   if(t0 != 0){

     rc = rc+4;
     if(pd0->flgd != 0){

       printf(s626er,4);
       printf("   key=");
       for(j=0;j<28;j++) printf("%d",t[j]);
       printf("\n");
     }
   }

        /* When an unsigned integer is converted to long,
           the value of the result is the same numerically
           as that of the unsigned integer.               */

   l = (unsigned)0100000;
   if((long)l > (unsigned)0100000){

      rc = rc+8;
      if(pd0->flgd != 0) printf(s626er,8);
   }

   return rc;
}
