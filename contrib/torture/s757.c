s757(pd0)          /* 7.5 Shift operators          */
                   /* 7.6 Relational operators     */
                   /* 7.7 Equality operator        */
#include "defs"
struct defs *pd0;
{
   static char s757er[] = "s757,er%d\n";
   static char qs757[8] = "s757   ";
   int rc;
   char *ps, *pt;
   int t,lrc,k,j,a,b,c,d,x[16],*p;
   unsigned rs, ls, rt, lt;
   ps = qs757;
   pt = pd0->rfs;
   rc = 0;
   while (*pt++ = *ps++);

        /* The shift operators << and >> group left-to-right.
                                                                */

   t = 40;
   if(t<<3<<2 != 1280 || t>>3>>2 != 1){
     rc = rc+1;
     if(pd0->flgd != 0) printf(s757er,1);
   }

        /* In the following test, an n-bit unsigned consisting
        of all 1s is shifted right (resp. left) k bits, 0<=k<n.
        We expect to find k 0s followed by n-k 1s (resp. n-k 1s
        followed by k 0s). If not, we complain.
                                                                */

   lrc = 0;
   for(k=0; k<pd0->ubits; k++){
     rs = 1;
     ls = rs<<(pd0->ubits-1);

     rt = 0;
     lt = ~rt>>k;
     rt = ~rt<<k;

     for(j=0; j<pd0->ubits;j++){
       if((j<k) != ((rs&rt) == 0) || (j<k) != ((ls&lt) == 0)) lrc = 1;
       rs = rs<<1;
       ls = ls>>1;
     }
   }

   if(lrc != 0){
     rc = rc+2;
     if(pd0->flgd != 0) printf(s757er,2);
   }

        /* The relational operators group left-to-right, but this
        fact is not very useful; a<b<c does not mean what it 
        seems to...
                                                                */

   a = 3;
   b = 2;
   c = 1;

   if((a<b<c) != 1){
     rc = rc+4;
     if(pd0->flgd != 0) printf(s757er,4);
   }

        /* In general, we take note of the fact that if we got this
        far the relational operators have to be working. We test only
        that two pointers may be compared; the result depends on
        the relative locations in the address space of the 
        pointed-to objects.
                                                                */
   if( &x[1] == &x[0] ){
     rc = rc+8;
     if(pd0->flgd != 0) printf(s757er,8);
   }

   if( &x[1] < &x[0] ) if(pd0->flgm != 0)
     printf("Increasing array elements assigned to decreasing locations\n");

        /* a<b == c<d whenever a<b and c<d have the same 
        truth value.                                            */

   lrc = 0;

   for(j=0;j<16;j++) x[j] = 1;
   x[1] = 0;
   x[4] = 0;
   x[6] = 0;
   x[7] = 0;
   x[9] = 0;
   x[13] = 0;

   for(a=0;a<2;a++)
     for(b=0;b<2;b++)
       for(c=0;c<2;c++)
         for(d=0;d<2;d++)
           if((a<b==c<d) != x[8*a+4*b+2*c+d] ) lrc = 1;

   if(lrc != 0){
     rc = rc+16;
     if(pd0->flgd != 0) printf(s757er,16);
   }

        /* A pointer to which zero has been assigned will
        appear to be equal to zero.
                                                                */

   p = 0;

   if(p != 0){
     rc = rc+32;
     if(pd0->flgd != 0) printf(s757er,32);
   }

   return rc;
}
