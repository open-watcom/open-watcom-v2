s86(pd0)          /*  8.6 Initialization  */
#include "defs"
struct defs *pd0;
{
   static char s86er[] = "s86,er%d\n";
   static char qs86[8] = "s86    ";
   int lrc, rc;
   char *ps, *pt;
   int one(), i, j, k;
   static int x[] = {1,3,5};
   static int *pint = x+2;
   static int zero[10];
   int *apint = pint-1;
   register int *rpint = apint+one();
   static float y0[] = {1,3,5,2,4,6,3,5,7,0,0,0};
   static float y1[4][3] = {
     {1,3,5},
     {2,4,6},
     {3,5,7},
   };
   static float y2[4][3] = {1,3,5,2,4,6,3,5,7};
   static float y3[4][3] = {
     {1},{2},{3},{4}
   };
   ps = qs86;
   pt = pd0->rfs;
   rc = 0;
   while (*pt++ = *ps++);

        /* The expression in an initializer for a static or
        external variable must be a constant expression or
        an expression that reduces to the address of a pre-
        viously declared variable, possibly offset by a
        constant expression.
                                                                */

   if(*pint != 5){
     if(pd0->flgd != 0) printf(s86er,1);
     rc = rc+1;
   }

        /* Automatic and register variables may be initialized
        by arbitrary expressions involving constants and previously
        declared variables and functions.
                                                                */

   if(*apint != 3){
     if(pd0->flgd != 0) printf(s86er,2);
     rc = rc+2;
   }

   if(*rpint != 5){
     if(pd0->flgd != 0) printf(s86er,4);
     rc = rc+4;
   }

        /* Static variables that are not initialized are guar-
        anteed to start off as zero.
                                                        */

   lrc = 0;
   for(j=0; j<10; j++)
     if(zero[j] != 0) lrc = 1;
   if(lrc != 0){
     if(pd0->flgd != 0) printf(s86er,8);
     rc = rc+8;
   }

        /* y0, y1, and y2, as declared, should define and 
        initialize identical arrays.
                                                                */
   lrc = 0;
   for(i=0; i<4; i++)
     for(j=0; j<3; j++){
       k = 3*i+j;
       if( y1[i][j] != y2[i][j]
         ||y1[i][j] != y0[k]) lrc = 1;
     }

   if(lrc != 0){
     if(pd0->flgd != 0) printf(s86er,16);
     rc = rc+16;
   }

        /* y3 initializes the first column of the array and
        leaves the rest zero.
                                                                */

   lrc = 0;
   for(j=0; j<4; j++) if(y3[j][0] != j+1) lrc = 1;

   if(lrc != 0){
     if(pd0->flgd != 0) printf(s86er,32);
     rc = rc+32;
   }
   return rc;
}
one(){
   return 1;
}
