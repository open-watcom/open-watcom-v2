s61(pd0)          /* Characters and integers */
#include "defs"
struct defs *pd0;
{
   static char s61er[] = "s61,er%d\n";
   static char qs61[8] = "s61    ";
   short from, shortint;
   long int to, longint;
   int rc, lrc;
   int j;
   char fromc, charint;
   char *wd, *pc[6];
   
   static char upper_alpha[]             = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
   static char lower_alpha[]             = "abcdefghijklmnopqrstuvwxyz";
   static char numbers[]               = "0123456789";
   static char special_characters[]    = "~!\"#%&()_=-^|{}[]+;*:<>,.?/";
   static char extra_special_characters[] = "\n\t\b\r\f\\\'";
   static char blank_and_NUL[]            = " \0";

   char *ps, *pt;
   ps = qs61;
   pt = pd0->rfs;
   rc = 0;
   while (*pt++ = *ps++);

/*      A character or a short integer may be used wherever
an integer may be used. In all cases, the value is converted
to integer. This principle is extensively used throughout this
program, and will not be explicitly tested here.        */

/*      Conversion of a shorter integer to a longer always
involves sign extension.                                */

   from = -19;
   to = from;

   if(to != -19){
     rc = rc+1;
     if(pd0->flgd != 0) printf(s61er,1);
   }

/*      Whether or not sign extension occurs for characters
is machine dependent.                                   */

   fromc = '\377';
   to = fromc;
   if(to >= 0) wd = "no ";
     else wd = "";
   if(pd0->flgm != 0) printf("%ssign extension in char assignments\n",wd);

/*      It is guaranteed that a member of the standard char-
acter set is nonnegative.                               */

   pc[0] = upper_alpha;
   pc[1] = lower_alpha;
   pc[2] = numbers;
   pc[3] = special_characters;
   pc[4] = extra_special_characters;
   pc[5] = blank_and_NUL;

   lrc = 0;
   for (j=0; j<6; j++)
     while(*pc[j]) if(*pc[j]++ < 0) lrc =1;

   if(lrc != 0){
     rc=rc+2;
     if(pd0->flgd != 0) printf(s61er,2);
   }

        /* A character constant specified with an octal escape
        suffers sign extension and may appear negative. The
        example given in the manual is that '\377' == -1.
        This may not be true on some machines or on some com-
        pilers; however, we do expect that an implementation
        will be consistent in its behaviour, in that character
        variable assignments will be treated in the same way
        as character constant assignments. In the following test,
        remember that wd has been previously set to "" or "no",
        depending on whether or not sign propagation occurs
        when assigning a character variable to an int.
                                                                */

   if('\377' != -1){
     if(pd0->flgm != 0) printf("'\\377' is not equal to -1.\n");
     if(*wd != 'n'){
       rc = rc+4;
       if(pd0->flgd != 0) printf(s61er,4);
     }
   } else{

     if(*wd == 'n'){
       rc = rc+4;
       if(pd0->flgd != 0) printf(s61er,4);
     }
   }

/*      When a longer integer is converted to a shorter or
to  a char, it is truncated on the left; excess bits are 
simply discarded.                                       */

   longint = 1048579;           /* =2**20+3 */
   shortint = longint;
   charint = longint;

   if((shortint != longint && shortint != 3) ||
      (charint  != longint && charint  != 3)) {
     rc = rc+8;
     if(pd0->flgd != 0) printf(s61er,8);
   }

   return rc;
}
