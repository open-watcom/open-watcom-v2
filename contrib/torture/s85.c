s85(pd0)          /*  8.5 Structure and union declarations   */
#include "defs"
struct defs *pd0;
{
   static char s85er[] = "s85,er%d\n";
   static char qs85[8] = "s85    ";
   int rc;
   char *ps, *pt;
   
   struct tnode {
     char tword[20];
     int count;
     struct tnode *left;
     struct tnode *right;
   };

   struct tnode s1, s2, *sp;

   struct{
     char cdummy;
     char c;
   } sc;

   struct{
     char cdummy;
     short s;
   } ss;

   struct{
     char cdummy;
     int i;
   } si;

   struct{
     char cdummy;
     long l;
   } sl;

   struct{
     char cdummy;
     unsigned u;
   } su;

   struct{
     char cdummy;
     float f;
   } sf;

   struct{
     char cdummy;
     double d;
   } sd;

   int diff[7], j;

   static char *type[] = {
     "char",
     "short",
     "int",
     "long",
     "unsigned",
     "float",
     "double"
   };

   static char aln[] = " alignment: ";

   struct{
     int twobit:2;
     int       :1;
     int threebit:3;
     int onebit:1;
   } s3;

   union{
     char u1[30];
     short u2[30];
     int u3[30];
     long u4[30];
     unsigned u5[30];
     float u6[30];
     double u7[30];
   } u0;

   ps = qs85;
   pt = pd0->rfs;
   rc = 0;
   while (*pt++ = *ps++);

        /* Within a structure, the objects declared have
        addresses which increase as their declarations are
        read left to right.
                                                                */

   if( (char *)&s1.count - &s1.tword[0] <= 0
     ||(char *)&s1.left - (char *)&s1.count <= 0
     ||(char *)&s1.right - (char *)&s1.left <= 0){
     if(pd0->flgd != 0) printf(s85er,1);
     rc = rc+1;
   }

        /* Each non-field member of a structure begins on an
        addressing boundary appropriate to its type.
                                                                */

   diff[0] = &sc.c - &sc.cdummy;
   diff[1] = (char *)&ss.s - &ss.cdummy;
   diff[2] = (char *)&si.i - &si.cdummy;
   diff[3] = (char *)&sl.l - &sl.cdummy;
   diff[4] = (char *)&su.u - &su.cdummy;
   diff[5] = (char *)&sf.f - &sf.cdummy;
   diff[6] = (char *)&sd.d - &sd.cdummy;

   if(pd0->flgm != 0)
    for(j=0; j<7; j++)
     printf("%s%s%d\n",type[j],aln,diff[j]);

        /* Field specifications are highly implementation de-
        pendent. About the only thing we can do here is to
        check is that the compiler accepts the field constructs,
        and that they seem to work, after a fashion, at
        run time...
                                                                */

   s3.threebit = 7;
   s3.twobit = s3.threebit;
   s3.threebit = s3.twobit;

   if(s3.threebit != 3){
     if(s3.threebit == -1){
       if(pd0->flgm != 0) printf("Sign extension in fields\n");
     }
     else{
       if(pd0->flgd != 0) printf(s85er,2);
       rc = rc+2;
     }
   }

   s3.onebit = 1;
   if(s3.onebit != 1){
     if(pd0->flgm != 0)
      printf("Be especially careful with 1-bit fields!\n");
   }

        /* A union may be thought of as a structure all of whose
        members begin at offset 0 and whose size is sufficient
        to contain any of its members.
                                                                */

   if( (char *)u0.u1 - (char *)&u0 != 0
     ||(char *)u0.u2 - (char *)&u0 != 0
     ||(char *)u0.u3 - (char *)&u0 != 0
     ||(char *)u0.u4 - (char *)&u0 != 0
     ||(char *)u0.u5 - (char *)&u0 != 0
     ||(char *)u0.u6 - (char *)&u0 != 0
     ||(char *)u0.u7 - (char *)&u0 != 0){

     if(pd0->flgd != 0) printf(s85er,4);
     rc = rc+4;
   }

   if( sizeof u0 < sizeof u0.u1
     ||sizeof u0 < sizeof u0.u2
     ||sizeof u0 < sizeof u0.u3
     ||sizeof u0 < sizeof u0.u4
     ||sizeof u0 < sizeof u0.u5
     ||sizeof u0 < sizeof u0.u6
     ||sizeof u0 < sizeof u0.u7){

     if(pd0->flgd != 0) printf(s85er,8);
     rc = rc+8;
   }

        /* Finally, we check that the pointers work.            */

   s1.right = &s2;
   s2.tword[0] = 2;
   s1.right->tword[0] += 1;
   if(s2.tword[0] != 3){
     if(pd0->flgd != 0) printf(s85er,16);
     rc = rc+16;
   }
   return rc;
}
