s84(pd0)          /*  8.4 Meaning of declarators   */
#include "defs"
struct defs *pd0;
{
   int *ip, i, *fip(), (*pfi)(), j, k, array(), glork();
   static int x3d[3][5][7];
   float fa[17], *afp[17], sum;
   static char s84er[] = "s84,er%d\n";
   static char qs84[8] = "s84    ";
   int rc;
   char *ps, *pt;
   ps = qs84;
   pt = pd0->rfs;
   rc = 0;
   while (*pt++ = *ps++);

        /* The more common varieties of declarators have al-
        ready been touched upon, some more than others. It
        is useful to compare *fip() and (*pfi)().
                                                                */

   ip = fip(3);
   if(*ip != 3){
     if(pd0->flgd != 0) printf(s84er,1);
     rc = rc+1;
   }

   pfi = glork;
   if((*pfi)(4) != 4){
     if(pd0->flgd != 0) printf(s84er,2);
     rc = rc+2;
   }

        /* Float fa[17] declares an array of floating point
        numbers, and *afp[17] declares an array of pointers
        to floats.
                                                                */

   for(j=0; j<17; j++){
     fa[j] = j;
     afp[j] = &fa[j];
   }

   sum = 0.;
   for(j=0; j<17; j++) sum += *afp[j];
   if(sum != 136){
     if(pd0->flgd != 0) printf(s84er,4);
     rc = rc+4;
   }

        /*  static int x3d[3][5][7] declares a static three
        dimensional array of integers, with rank 3x5x7.
        In complete detail, x3d is an array of three items;
        each item is an array of five arrays, and each of 
        the latter arrays is an array of seven integers.
        Any of the expressions x3d, x3d[i], x3d[i][j],
        and x3d[i][j][k] may reasonably appear in an express-
        ion. The first three have type "array"; the last has
        type int.
                                                                */

   for (i=0; i<3; i++)
     for (j=0; j<5; j++)
       for (k=0; k<7; k++)
         x3d[i][j][k] = i*35+j*7+k;

   i = 1; j = 2; k = 3;

   if( array(x3d,105,0)
      +array(x3d[i],35,35)
      +array(x3d[i][j],7,49)
      +      x3d[i][j][k]-52){
 
      if(pd0->flgd != 0) printf(s84er,8);
      rc = rc+8;
   }

   return rc;
}
array(a,size,start)
int a[], size, start;
{
   int i;
   for(i=0; i<size; i++)
     if(a[i] != i+start) return 1;

   return 0;
}
int *fip(x)
int x;
{
   static int y;
   y = x;
   return &y;
}
glork(x)
int x;
{return x;}
