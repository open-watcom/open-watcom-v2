int extvar;
s4(pd0)                    /* 4. What's in a name?             */
#include "defs"
struct defs *pd0;
{
   static char s4er[] = "s4,er%d\n";
   static char qs4[8] = "s4     ";
   char *ps, *pt;
   int j, rc;

   short sint;             /* short integer, for size test      */
   int pint;               /* plain                             */
   long lint;              /* long                              */
   unsigned target;
   unsigned int mask;

   rc = 0;
   ps = qs4;
   pt = pd0->rfs;

   while(*pt++ = *ps++);

/*   There are four declarable storage classes: automatic,
static, external, and register. Automatic variables have
been dealt with extensively thus far, and will not be specif-
ically treated in this section. Register variables are treated
in section s81.

     Static variables are local to a block, but retain their
values upon reentry to a block, even after control has left
the block.                                                     */

   for (j=0; j<3; j++)
     if(svtest(j) != zero()){
       rc = 1;
       if(pd0->flgd != 0) printf(s4er,1);
     }
   ;

/*   External variables exist and retain their values throughout
the execution of the entire program, and may be used for comm-
unication between functions, even separately compiled functions.
                                                                */

   setev();
   if(testev() != 0){
     rc=rc+2;
     if(pd0->flgd != 0) printf(s4er,2);
   }
/*   
     Characters have been tested elsewhere (in s243).

     Up to three sizes of integer, declared short int, int, and
long int, are available. Longer integers provide no less storage
than shorter ones, but implementation may make either short
integers, or long integers, or both, equivalent to plain
integers.
                                                                */

   if(sizeof lint < sizeof pint || sizeof pint < sizeof sint){

     rc = rc+4;
     if(pd0->flgd != 0) printf(s4er,4);
   }

/*   Unsigned integers, declared unsigned, obey the laws of
arithmetic modulo 2**n, where n is the number of bits in the
implementation                                                  */

   target = 0;
   target = target-1;
   mask = 1;
 
   for(j=0; j<(sizeof target)*pd0->cbits; j++){
   
     mask = mask&target;
     target = target>>1;
   }

   if(mask != 1 || target != 0){

     rc = rc+8;
     if(pd0->flgd != 0) printf(s4er,8);
   }

   return rc;
}
svtest(n)
int n;
{
   static k;
   int rc;
   switch (n) {

     case 0: k = 1978;
             rc = 0;
             break;

     case 1: if(k != 1978) rc = 1;
             else{
              k = 1929;
              rc = 0;
             }
             break;

     case 2: if(k != 1929) rc = 1;
             else rc = 0;
             break;
   }
   return rc;
}
zero(){                 /* Returns a value of zero, possibly */
   static k;            /* with side effects, as it's called */
   int rc;              /* alternately with svtest, above,   */
   k = 2;               /* and has the same internal storage */
   rc = 0;              /* requirements.                     */
   return rc;
}
testev(){
   if(extvar != 1066) return 1;
   else return 0;
}
