s7813(pd0)          /* 7.8 Bitwise AND operator
                       7.9 Bitwise OR operator
                       7.10 Bitwise exclusive OR operator
                       7.11 Logical AND operator
                       7.12 Logical OR operator
                       7.13 Conditional operator            */
#include "defs"
struct defs *pd0;
{
   register int prlc, lrc;
   int i, j, r, zero, one;
   static char fl[] = "Local error %d.\n";
   static char s7813er[] = "s7813,er%d\n";
   static char qs7813[8] = "s7813  ";
   int rc;
   char *ps, *pt;
   ps = qs7813;
   pt = pd0->rfs;
   lrc = 0;
   rc = 0;
   prlc = pd0->flgl;
   while (*pt++ = *ps++);

        /* If bitwise AND, OR, and exclusive OR are to cause
        trouble, they will probably do so when they are used in
        an unusual context. The number of contexts in which
        they can be used is infinite, so to save time we select
        a finite subset: the set of all expressions of the form:

                item1 op item2

        where item1 and item2 are chosen from the set
        {char,short,long,unsigned,int} and op is one of {&,|,^}.
        We will use 12 and 10 as values for the items, as these
        values will fit into all data types on just about any
        imaginable machine, and the results after performing the
        bitwise operations on them are distinct for each operation,
        i.e.,

                12 | 10  -> 1100 | 1010  -> 1110 -> 14
                12 ^ 10  -> 1100 ^ 1010  -> 0110 ->  6
                12 & 10  -> 1100 & 1010  -> 1000 ->  8

        There are 75 such combinations:
                                                                */

   if(((char)12 & (char)10) !=  8) {lrc = 1;
      if(prlc) printf(fl,lrc);}
   if(((char)12 | (char)10) != 14) {lrc = 2;
      if(prlc) printf(fl,lrc);}
   if(((char)12 ^ (char)10) !=  6) {lrc = 3;
      if(prlc) printf(fl,lrc);}
   if(((char)12 & (short)10) !=  8) {lrc = 4;
      if(prlc) printf(fl,lrc);}
   if(((char)12 | (short)10) != 14) {lrc = 5;
      if(prlc) printf(fl,lrc);}
   if(((char)12 ^ (short)10) !=  6) {lrc = 6;
      if(prlc) printf(fl,lrc);}
   if(((char)12 & (long)10) !=  8) {lrc = 7;
      if(prlc) printf(fl,lrc);}
   if(((char)12 | (long)10) != 14) {lrc = 8;
      if(prlc) printf(fl,lrc);}
   if(((char)12 ^ (long)10) !=  6) {lrc = 9;
      if(prlc) printf(fl,lrc);}
   if(((char)12 & (unsigned)10) !=  8) {lrc = 10;
      if(prlc) printf(fl,lrc);}
   if(((char)12 | (unsigned)10) != 14) {lrc = 11;
      if(prlc) printf(fl,lrc);}
   if(((char)12 ^ (unsigned)10) !=  6) {lrc = 12;
      if(prlc) printf(fl,lrc);}
   if(((char)12 & (int)10) !=  8) {lrc = 13;
      if(prlc) printf(fl,lrc);}
   if(((char)12 | (int)10) != 14) {lrc = 14;
      if(prlc) printf(fl,lrc);}
   if(((char)12 ^ (int)10) !=  6) {lrc = 15;
      if(prlc) printf(fl,lrc);}
   if(((short)12 & (char)10) !=  8) {lrc = 16;
      if(prlc) printf(fl,lrc);}
   if(((short)12 | (char)10) != 14) {lrc = 17;
      if(prlc) printf(fl,lrc);}
   if(((short)12 ^ (char)10) !=  6) {lrc = 18;
      if(prlc) printf(fl,lrc);}
   if(((short)12 & (short)10) !=  8) {lrc = 16;
      if(prlc) printf(fl,lrc);}
   if(((short)12 | (short)10) != 14) {lrc = 20;
      if(prlc) printf(fl,lrc);}
   if(((short)12 ^ (short)10) !=  6) {lrc = 21;
      if(prlc) printf(fl,lrc);}
   if(((short)12 & (long)10) !=  8) {lrc = 22;
      if(prlc) printf(fl,lrc);}
   if(((short)12 | (long)10) != 14) {lrc = 23;
      if(prlc) printf(fl,lrc);}
   if(((short)12 ^ (long)10) !=  6) {lrc = 24;
      if(prlc) printf(fl,lrc);}
   if(((short)12 & (unsigned)10) !=  8) {lrc = 25;
      if(prlc) printf(fl,lrc);}
   if(((short)12 | (unsigned)10) != 14) {lrc = 26;
      if(prlc) printf(fl,lrc);}
   if(((short)12 ^ (unsigned)10) !=  6) {lrc = 27;
      if(prlc) printf(fl,lrc);}
   if(((short)12 & (int)10) !=  8) {lrc = 28;
      if(prlc) printf(fl,lrc);}
   if(((short)12 | (int)10) != 14) {lrc = 26;
      if(prlc) printf(fl,lrc);}
   if(((short)12 ^ (int)10) !=  6) {lrc = 30;
      if(prlc) printf(fl,lrc);}
   if(((long)12 & (char)10) !=  8) {lrc = 31;
      if(prlc) printf(fl,lrc);}
   if(((long)12 | (char)10) != 14) {lrc = 32;
      if(prlc) printf(fl,lrc);}
   if(((long)12 ^ (char)10) !=  6) {lrc = 33;
      if(prlc) printf(fl,lrc);}
   if(((long)12 & (short)10) !=  8) {lrc = 34;
      if(prlc) printf(fl,lrc);}
   if(((long)12 | (short)10) != 14) {lrc = 35;
      if(prlc) printf(fl,lrc);}
   if(((long)12 ^ (short)10) !=  6) {lrc = 36;
      if(prlc) printf(fl,lrc);}
   if(((long)12 & (long)10) !=  8) {lrc = 37;
      if(prlc) printf(fl,lrc);}
   if(((long)12 | (long)10) != 14) {lrc = 38;
      if(prlc) printf(fl,lrc);}
   if(((long)12 ^ (long)10) !=  6) {lrc = 39;
      if(prlc) printf(fl,lrc);}
   if(((long)12 & (unsigned)10) !=  8) {lrc = 40;
      if(prlc) printf(fl,lrc);}
   if(((long)12 | (unsigned)10) != 14) {lrc = 41;
      if(prlc) printf(fl,lrc);}
   if(((long)12 ^ (unsigned)10) !=  6) {lrc = 42;
      if(prlc) printf(fl,lrc);}
   if(((long)12 & (int)10) !=  8) {lrc = 43;
      if(prlc) printf(fl,lrc);}
   if(((long)12 | (int)10) != 14) {lrc = 44;
      if(prlc) printf(fl,lrc);}
   if(((long)12 ^ (int)10) !=  6) {lrc = 45;
      if(prlc) printf(fl,lrc);}
   if(((unsigned)12 & (char)10) !=  8) {lrc = 46;
      if(prlc) printf(fl,lrc);}
   if(((unsigned)12 | (char)10) != 14) {lrc = 47;
      if(prlc) printf(fl,lrc);}
   if(((unsigned)12 ^ (char)10) !=  6) {lrc = 48;
      if(prlc) printf(fl,lrc);}
   if(((unsigned)12 & (short)10) !=  8) {lrc = 49;
      if(prlc) printf(fl,lrc);}
   if(((unsigned)12 | (short)10) != 14) {lrc = 50;
      if(prlc) printf(fl,lrc);}
   if(((unsigned)12 ^ (short)10) !=  6) {lrc = 51;
      if(prlc) printf(fl,lrc);}
   if(((unsigned)12 & (long)10) !=  8) {lrc = 52;
      if(prlc) printf(fl,lrc);}
   if(((unsigned)12 | (long)10) != 14) {lrc = 53;
      if(prlc) printf(fl,lrc);}
   if(((unsigned)12 ^ (long)10) !=  6) {lrc = 54;
      if(prlc) printf(fl,lrc);}
   if(((unsigned)12 & (unsigned)10) !=  8) {lrc = 55;
      if(prlc) printf(fl,lrc);}
   if(((unsigned)12 | (unsigned)10) != 14) {lrc = 56;
      if(prlc) printf(fl,lrc);}
   if(((unsigned)12 ^ (unsigned)10) !=  6) {lrc = 57;
      if(prlc) printf(fl,lrc);}
   if(((unsigned)12 & (int)10) !=  8) {lrc = 58;
      if(prlc) printf(fl,lrc);}
   if(((unsigned)12 | (int)10) != 14) {lrc = 56;
      if(prlc) printf(fl,lrc);}
   if(((unsigned)12 ^ (int)10) !=  6) {lrc = 60;
      if(prlc) printf(fl,lrc);}
   if(((int)12 & (char)10) !=  8) {lrc = 61;
      if(prlc) printf(fl,lrc);}
   if(((int)12 | (char)10) != 14) {lrc = 62;
      if(prlc) printf(fl,lrc);}
   if(((int)12 ^ (char)10) !=  6) {lrc = 63;
      if(prlc) printf(fl,lrc);}
   if(((int)12 & (short)10) !=  8) {lrc = 64;
      if(prlc) printf(fl,lrc);}
   if(((int)12 | (short)10) != 14) {lrc = 65;
      if(prlc) printf(fl,lrc);}
   if(((int)12 ^ (short)10) !=  6) {lrc = 66;
      if(prlc) printf(fl,lrc);}
   if(((int)12 & (long)10) !=  8) {lrc = 67;
      if(prlc) printf(fl,lrc);}
   if(((int)12 | (long)10) != 14) {lrc = 68;
      if(prlc) printf(fl,lrc);}
   if(((int)12 ^ (long)10) !=  6) {lrc = 69;
      if(prlc) printf(fl,lrc);}
   if(((int)12 & (unsigned)10) !=  8) {lrc = 70;
      if(prlc) printf(fl,lrc);}
   if(((int)12 | (unsigned)10) != 14) {lrc = 71;
      if(prlc) printf(fl,lrc);}
   if(((int)12 ^ (unsigned)10) !=  6) {lrc = 72;
      if(prlc) printf(fl,lrc);}
   if(((int)12 & (int)10) !=  8) {lrc = 73; if(prlc) printf(fl,lrc);}
   if(((int)12 | (int)10) != 14) {lrc = 74; if(prlc) printf(fl,lrc);}
   if(((int)12 ^ (int)10) !=  6) {lrc = 75; if(prlc) printf(fl,lrc);}

   if(lrc != 0){
     if(pd0->flgd != 0) printf(s7813er,1);
     rc = rc+1;
   }

        /* The && operator groups left to right. It returns 1
        if both of the operands are nonzero; 0 otherwise.
        It guarantees left to right evaluation; moreover, the
        second operand is not evaluated if the value of the 
        first operand is 0.
                                                                */

   lrc = 0;
   i = j = 0;

   r = i++ && j++;
    if(i!=1) {lrc = 1; if(prlc) printf(fl,lrc);}
    if(j!=0) {lrc = 2; if(prlc) printf(fl,lrc);}
    if(r!=0) {lrc = 3; if(prlc) printf(fl,lrc);}
   r = i && j++;
    if(i!=1) {lrc = 4; if(prlc) printf(fl,lrc);}
    if(j!=1) {lrc = 5; if(prlc) printf(fl,lrc);}
    if(r!=0) {lrc = 6; if(prlc) printf(fl,lrc);}
   r = i-- && j;
    if(i!=0) {lrc = 7; if(prlc) printf(fl,lrc);}
    if(j!=1) {lrc = 8; if(prlc) printf(fl,lrc);}
    if(r!=1) {lrc = 9; if(prlc) printf(fl,lrc);}
   r = i && j--;
    if(i!=0) {lrc = 10; if(prlc) printf(fl,lrc);}
    if(j!=1) {lrc = 11; if(prlc) printf(fl,lrc);}
    if(r!=0) {lrc = 12; if(prlc) printf(fl,lrc);}

   if(lrc!=0){
     if(pd0->flgd != 0) printf(s7813er,2);
     rc = rc+2;
   }

        /* The || operator groups left to right. It returns 1
        if either of its operands is nonzero; 0 otherwise. It
        guarantees left to right evaluation; moreover, the second
        operand is not evaluated if the value of the first 
        operand is nonzero.
                                                                */

   lrc = 0;
   i = j = 0;
   r = i++ || j;
    if(i!=1) {lrc = 1; if(prlc) printf(fl,lrc);}
    if(j!=0) {lrc = 2; if(prlc) printf(fl,lrc);}
    if(r!=0) {lrc = 3; if(prlc) printf(fl,lrc);}
   r = j++ || i;
    if(i!=1) {lrc = 4; if(prlc) printf(fl,lrc);}
    if(j!=1) {lrc = 5; if(prlc) printf(fl,lrc);}
    if(r!=1) {lrc = 6; if(prlc) printf(fl,lrc);}
   r = i-- || j--;
    if(i!=0) {lrc = 7; if(prlc) printf(fl,lrc);}
    if(j!=1) {lrc = 8; if(prlc) printf(fl,lrc);}
    if(r!=1) {lrc = 9; if(prlc) printf(fl,lrc);}
   r = i || j--;
    if(i!=0) {lrc = 10; if(prlc) printf(fl,lrc);}
    if(j!=0) {lrc = 11; if(prlc) printf(fl,lrc);}
    if(r!=1) {lrc = 12; if(prlc) printf(fl,lrc);}

   if(lrc!=0){
     if(pd0->flgd != 0) printf(s7813er,4);
     rc = rc+4;
   }

        /* Conditional expressions group right to left.  */

   i = j = 0;
   zero = 0;
   one = 1;
   r = one?zero:one?i++:j++;
   if(r!=0 || i!=0 || j!=0){
     if(pd0->flgd != 0) printf(s7813er,8);
     rc = rc+8;
   }

        /* The first expression is evaluated and if it is non-
        zero, the result is the value of the second expression;
        otherwise, that of the third expression.
                                                                */

   if((one?zero:1) != 0 || (zero?1:zero) != 0){
     if(pd0->flgd != 0) printf(s7813er,16);
     rc = rc+16;
   }
   return rc;
}
