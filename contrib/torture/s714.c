s714(pd0)          /*  7.14  Assignment operators       */
#include "defs"
struct defs *pd0;
{
   static char f[] = "Local error %d.\n";
   static char s714er[] = "s714,er%d\n";
   static char qs714[8] = "s714   ";
   register int prlc, lrc;
   int rc;
   char cl, cr;
   short sl, sr;
   int il, ir;
   long ll, lr;
   unsigned ul, ur;
   float fl, fr;
   double dl, dr;
   char *ps, *pt;
   ps = qs714;
   pt = pd0->rfs;
   rc = 0;
   lrc = 0;
   prlc = pd0->flgl;
   while (*pt++ = *ps++);

        /* This section tests the assignment operators.

        It is an exhaustive test of all assignment statements
        of the form:

                vl op vr

        where vl and vr are variables from the set
        {char,short,int,long,unsigned,float,double} and op is
        one of the assignment operators. There are 395 such
        statements.

        The initial values for the variables have been chosen
        so that both the initial values and the results will
        "fit" in just about any implementation, and that the re-
        sults will be such that they test for the proper form-
        ation of composite operators, rather than checking for
        the valid operation of those operators' components.
        For example, in checking >>=, we want to verify that
        a right shift and a move take place, rather than
        whether or not there may be some peculiarities about
        the right shift. Such tests have been made previously,
        and to repeat them here would be to throw out a red
        herring.

        The table below lists the operators, assignment targets,
        initial values for left and right operands, and the
        expected values of the results.


          =  +=  -=  *=  /=  %=  >>=  <<=  &=  ^=  |=	
char      2   7   3  10   2   1   1    20   8   6  14
short     2   7   3  10   2   1   1    20   8   6  14
int       2   7   3  10   2   1   1    20   8   6  14
long      2   7   3  10   2   1   1    20   8   6  14
unsigned  2   7   3  10   2   1   1    20   8   6  14
float     2   7   3  10 2.5 |             |
double    2   7   3  10 2.5 |             |
                            |             |
initial         (5,2)       |    (5,2)    |  (12,10)

        The following machine-generated program reflects the
        tests described in the table.
                                                                */

   cl = 5; cr = 2;
   cl = cr;
   if(cl != 2){
     lrc = 1;
     if(prlc) printf(f,lrc);
   }
   cl = 5; sr = 2;
   cl = sr;
   if(cl != 2){
     lrc = 2;
     if(prlc) printf(f,lrc);
   }
   cl = 5; ir = 2;
   cl = ir;
   if(cl != 2){
     lrc = 3;
     if(prlc) printf(f,lrc);
   }
   cl = 5; lr = 2;
   cl = lr;
   if(cl != 2){
     lrc = 4;
     if(prlc) printf(f,lrc);
   }
   cl = 5; ur = 2;
   cl = ur;
   if(cl != 2){
     lrc = 5;
     if(prlc) printf(f,lrc);
   }
   cl = 5; fr = 2;
   cl = fr;
   if(cl != 2){
     lrc = 6;
     if(prlc) printf(f,lrc);
   }
   cl = 5; dr = 2;
   cl = dr;
   if(cl != 2){
     lrc = 7;
     if(prlc) printf(f,lrc);
   }
   sl = 5; cr = 2;
   sl = cr;
   if(sl != 2){
     lrc = 8;
     if(prlc) printf(f,lrc);
   }
   sl = 5; sr = 2;
   sl = sr;
   if(sl != 2){
     lrc = 9;
     if(prlc) printf(f,lrc);
   }
   sl = 5; ir = 2;
   sl = ir;
   if(sl != 2){
     lrc = 10;
     if(prlc) printf(f,lrc);
   }
   sl = 5; lr = 2;
   sl = lr;
   if(sl != 2){
     lrc = 11;
     if(prlc) printf(f,lrc);
   }
   sl = 5; ur = 2;
   sl = ur;
   if(sl != 2){
     lrc = 12;
     if(prlc) printf(f,lrc);
   }
   sl = 5; fr = 2;
   sl = fr;
   if(sl != 2){
     lrc = 13;
     if(prlc) printf(f,lrc);
   }
   sl = 5; dr = 2;
   sl = dr;
   if(sl != 2){
     lrc = 14;
     if(prlc) printf(f,lrc);
   }
   il = 5; cr = 2;
   il = cr;
   if(il != 2){
     lrc = 15;
     if(prlc) printf(f,lrc);
   }
   il = 5; sr = 2;
   il = sr;
   if(il != 2){
     lrc = 16;
     if(prlc) printf(f,lrc);
   }
   il = 5; ir = 2;
   il = ir;
   if(il != 2){
     lrc = 17;
     if(prlc) printf(f,lrc);
   }
   il = 5; lr = 2;
   il = lr;
   if(il != 2){
     lrc = 18;
     if(prlc) printf(f,lrc);
   }
   il = 5; ur = 2;
   il = ur;
   if(il != 2){
     lrc = 19;
     if(prlc) printf(f,lrc);
   }
   il = 5; fr = 2;
   il = fr;
   if(il != 2){
     lrc = 20;
     if(prlc) printf(f,lrc);
   }
   il = 5; dr = 2;
   il = dr;
   if(il != 2){
     lrc = 21;
     if(prlc) printf(f,lrc);
   }
   ll = 5; cr = 2;
   ll = cr;
   if(ll != 2){
     lrc = 22;
     if(prlc) printf(f,lrc);
   }
   ll = 5; sr = 2;
   ll = sr;
   if(ll != 2){
     lrc = 23;
     if(prlc) printf(f,lrc);
   }
   ll = 5; ir = 2;
   ll = ir;
   if(ll != 2){
     lrc = 24;
     if(prlc) printf(f,lrc);
   }
   ll = 5; lr = 2;
   ll = lr;
   if(ll != 2){
     lrc = 25;
     if(prlc) printf(f,lrc);
   }
   ll = 5; ur = 2;
   ll = ur;
   if(ll != 2){
     lrc = 26;
     if(prlc) printf(f,lrc);
   }
   ll = 5; fr = 2;
   ll = fr;
   if(ll != 2){
     lrc = 27;
     if(prlc) printf(f,lrc);
   }
   ll = 5; dr = 2;
   ll = dr;
   if(ll != 2){
     lrc = 28;
     if(prlc) printf(f,lrc);
   }
   ul = 5; cr = 2;
   ul = cr;
   if(ul != 2){
     lrc = 29;
     if(prlc) printf(f,lrc);
   }
   ul = 5; sr = 2;
   ul = sr;
   if(ul != 2){
     lrc = 30;
     if(prlc) printf(f,lrc);
   }
   ul = 5; ir = 2;
   ul = ir;
   if(ul != 2){
     lrc = 31;
     if(prlc) printf(f,lrc);
   }
   ul = 5; lr = 2;
   ul = lr;
   if(ul != 2){
     lrc = 32;
     if(prlc) printf(f,lrc);
   }
   ul = 5; ur = 2;
   ul = ur;
   if(ul != 2){
     lrc = 33;
     if(prlc) printf(f,lrc);
   }
   ul = 5; fr = 2;
   ul = fr;
   if(ul != 2){
     lrc = 34;
     if(prlc) printf(f,lrc);
   }
   ul = 5; dr = 2;
   ul = dr;
   if(ul != 2){
     lrc = 35;
     if(prlc) printf(f,lrc);
   }
   fl = 5; cr = 2;
   fl = cr;
   if(fl != 2){
     lrc = 36;
     if(prlc) printf(f,lrc);
   }
   fl = 5; sr = 2;
   fl = sr;
   if(fl != 2){
     lrc = 37;
     if(prlc) printf(f,lrc);
   }
   fl = 5; ir = 2;
   fl = ir;
   if(fl != 2){
     lrc = 38;
     if(prlc) printf(f,lrc);
   }
   fl = 5; lr = 2;
   fl = lr;
   if(fl != 2){
     lrc = 39;
     if(prlc) printf(f,lrc);
   }
   fl = 5; ur = 2;
   fl = ur;
   if(fl != 2){
     lrc = 40;
     if(prlc) printf(f,lrc);
   }
   fl = 5; fr = 2;
   fl = fr;
   if(fl != 2){
     lrc = 41;
     if(prlc) printf(f,lrc);
   }
   fl = 5; dr = 2;
   fl = dr;
   if(fl != 2){
     lrc = 42;
     if(prlc) printf(f,lrc);
   }
   dl = 5; cr = 2;
   dl = cr;
   if(dl != 2){
     lrc = 43;
     if(prlc) printf(f,lrc);
   }
   dl = 5; sr = 2;
   dl = sr;
   if(dl != 2){
     lrc = 44;
     if(prlc) printf(f,lrc);
   }
   dl = 5; ir = 2;
   dl = ir;
   if(dl != 2){
     lrc = 45;
     if(prlc) printf(f,lrc);
   }
   dl = 5; lr = 2;
   dl = lr;
   if(dl != 2){
     lrc = 46;
     if(prlc) printf(f,lrc);
   }
   dl = 5; ur = 2;
   dl = ur;
   if(dl != 2){
     lrc = 47;
     if(prlc) printf(f,lrc);
   }
   dl = 5; fr = 2;
   dl = fr;
   if(dl != 2){
     lrc = 48;
     if(prlc) printf(f,lrc);
   }
   dl = 5; dr = 2;
   dl = dr;
   if(dl != 2){
     lrc = 49;
     if(prlc) printf(f,lrc);
   }
   cl = 5; cr = 2;
   cl += cr;
   if(cl != 7){
     lrc = 50;
     if(prlc) printf(f,lrc);
   }
   cl = 5; sr = 2;
   cl += sr;
   if(cl != 7){
     lrc = 51;
     if(prlc) printf(f,lrc);
   }
   cl = 5; ir = 2;
   cl += ir;
   if(cl != 7){
     lrc = 52;
     if(prlc) printf(f,lrc);
   }
   cl = 5; lr = 2;
   cl += lr;
   if(cl != 7){
     lrc = 53;
     if(prlc) printf(f,lrc);
   }
   cl = 5; ur = 2;
   cl += ur;
   if(cl != 7){
     lrc = 54;
     if(prlc) printf(f,lrc);
   }
   cl = 5; fr = 2;
   cl += fr;
   if(cl != 7){
     lrc = 55;
     if(prlc) printf(f,lrc);
   }
   cl = 5; dr = 2;
   cl += dr;
   if(cl != 7){
     lrc = 56;
     if(prlc) printf(f,lrc);
   }
   sl = 5; cr = 2;
   sl += cr;
   if(sl != 7){
     lrc = 57;
     if(prlc) printf(f,lrc);
   }
   sl = 5; sr = 2;
   sl += sr;
   if(sl != 7){
     lrc = 58;
     if(prlc) printf(f,lrc);
   }
   sl = 5; ir = 2;
   sl += ir;
   if(sl != 7){
     lrc = 59;
     if(prlc) printf(f,lrc);
   }
   sl = 5; lr = 2;
   sl += lr;
   if(sl != 7){
     lrc = 60;
     if(prlc) printf(f,lrc);
   }
   sl = 5; ur = 2;
   sl += ur;
   if(sl != 7){
     lrc = 61;
     if(prlc) printf(f,lrc);
   }
   sl = 5; fr = 2;
   sl += fr;
   if(sl != 7){
     lrc = 62;
     if(prlc) printf(f,lrc);
   }
   sl = 5; dr = 2;
   sl += dr;
   if(sl != 7){
     lrc = 63;
     if(prlc) printf(f,lrc);
   }
   il = 5; cr = 2;
   il += cr;
   if(il != 7){
     lrc = 64;
     if(prlc) printf(f,lrc);
   }
   il = 5; sr = 2;
   il += sr;
   if(il != 7){
     lrc = 65;
     if(prlc) printf(f,lrc);
   }
   il = 5; ir = 2;
   il += ir;
   if(il != 7){
     lrc = 66;
     if(prlc) printf(f,lrc);
   }
   il = 5; lr = 2;
   il += lr;
   if(il != 7){
     lrc = 67;
     if(prlc) printf(f,lrc);
   }
   il = 5; ur = 2;
   il += ur;
   if(il != 7){
     lrc = 68;
     if(prlc) printf(f,lrc);
   }
   il = 5; fr = 2;
   il += fr;
   if(il != 7){
     lrc = 69;
     if(prlc) printf(f,lrc);
   }
   il = 5; dr = 2;
   il += dr;
   if(il != 7){
     lrc = 70;
     if(prlc) printf(f,lrc);
   }
   ll = 5; cr = 2;
   ll += cr;
   if(ll != 7){
     lrc = 71;
     if(prlc) printf(f,lrc);
   }
   ll = 5; sr = 2;
   ll += sr;
   if(ll != 7){
     lrc = 72;
     if(prlc) printf(f,lrc);
   }
   ll = 5; ir = 2;
   ll += ir;
   if(ll != 7){
     lrc = 73;
     if(prlc) printf(f,lrc);
   }
   ll = 5; lr = 2;
   ll += lr;
   if(ll != 7){
     lrc = 74;
     if(prlc) printf(f,lrc);
   }
   ll = 5; ur = 2;
   ll += ur;
   if(ll != 7){
     lrc = 75;
     if(prlc) printf(f,lrc);
   }
   ll = 5; fr = 2;
   ll += fr;
   if(ll != 7){
     lrc = 76;
     if(prlc) printf(f,lrc);
   }
   ll = 5; dr = 2;
   ll += dr;
   if(ll != 7){
     lrc = 77;
     if(prlc) printf(f,lrc);
   }
   ul = 5; cr = 2;
   ul += cr;
   if(ul != 7){
     lrc = 78;
     if(prlc) printf(f,lrc);
   }
   ul = 5; sr = 2;
   ul += sr;
   if(ul != 7){
     lrc = 79;
     if(prlc) printf(f,lrc);
   }
   ul = 5; ir = 2;
   ul += ir;
   if(ul != 7){
     lrc = 80;
     if(prlc) printf(f,lrc);
   }
   ul = 5; lr = 2;
   ul += lr;
   if(ul != 7){
     lrc = 81;
     if(prlc) printf(f,lrc);
   }
   ul = 5; ur = 2;
   ul += ur;
   if(ul != 7){
     lrc = 82;
     if(prlc) printf(f,lrc);
   }
   ul = 5; fr = 2;
   ul += fr;
   if(ul != 7){
     lrc = 83;
     if(prlc) printf(f,lrc);
   }
   ul = 5; dr = 2;
   ul += dr;
   if(ul != 7){
     lrc = 84;
     if(prlc) printf(f,lrc);
   }
   fl = 5; cr = 2;
   fl += cr;
   if(fl != 7){
     lrc = 85;
     if(prlc) printf(f,lrc);
   }
   fl = 5; sr = 2;
   fl += sr;
   if(fl != 7){
     lrc = 86;
     if(prlc) printf(f,lrc);
   }
   fl = 5; ir = 2;
   fl += ir;
   if(fl != 7){
     lrc = 87;
     if(prlc) printf(f,lrc);
   }
   fl = 5; lr = 2;
   fl += lr;
   if(fl != 7){
     lrc = 88;
     if(prlc) printf(f,lrc);
   }
   fl = 5; ur = 2;
   fl += ur;
   if(fl != 7){
     lrc = 89;
     if(prlc) printf(f,lrc);
   }
   fl = 5; fr = 2;
   fl += fr;
   if(fl != 7){
     lrc = 90;
     if(prlc) printf(f,lrc);
   }
   fl = 5; dr = 2;
   fl += dr;
   if(fl != 7){
     lrc = 91;
     if(prlc) printf(f,lrc);
   }
   dl = 5; cr = 2;
   dl += cr;
   if(dl != 7){
     lrc = 92;
     if(prlc) printf(f,lrc);
   }
   dl = 5; sr = 2;
   dl += sr;
   if(dl != 7){
     lrc = 93;
     if(prlc) printf(f,lrc);
   }
   dl = 5; ir = 2;
   dl += ir;
   if(dl != 7){
     lrc = 94;
     if(prlc) printf(f,lrc);
   }
   dl = 5; lr = 2;
   dl += lr;
   if(dl != 7){
     lrc = 95;
     if(prlc) printf(f,lrc);
   }
   dl = 5; ur = 2;
   dl += ur;
   if(dl != 7){
     lrc = 96;
     if(prlc) printf(f,lrc);
   }
   dl = 5; fr = 2;
   dl += fr;
   if(dl != 7){
     lrc = 97;
     if(prlc) printf(f,lrc);
   }
   dl = 5; dr = 2;
   dl += dr;
   if(dl != 7){
     lrc = 98;
     if(prlc) printf(f,lrc);
   }
   cl = 5; cr = 2;
   cl -= cr;
   if(cl != 3){
     lrc = 99;
     if(prlc) printf(f,lrc);
   }
   cl = 5; sr = 2;
   cl -= sr;
   if(cl != 3){
     lrc = 100;
     if(prlc) printf(f,lrc);
   }
   cl = 5; ir = 2;
   cl -= ir;
   if(cl != 3){
     lrc = 101;
     if(prlc) printf(f,lrc);
   }
   cl = 5; lr = 2;
   cl -= lr;
   if(cl != 3){
     lrc = 102;
     if(prlc) printf(f,lrc);
   }
   cl = 5; ur = 2;
   cl -= ur;
   if(cl != 3){
     lrc = 103;
     if(prlc) printf(f,lrc);
   }
   cl = 5; fr = 2;
   cl -= fr;
   if(cl != 3){
     lrc = 104;
     if(prlc) printf(f,lrc);
   }
   cl = 5; dr = 2;
   cl -= dr;
   if(cl != 3){
     lrc = 105;
     if(prlc) printf(f,lrc);
   }
   sl = 5; cr = 2;
   sl -= cr;
   if(sl != 3){
     lrc = 106;
     if(prlc) printf(f,lrc);
   }
   sl = 5; sr = 2;
   sl -= sr;
   if(sl != 3){
     lrc = 107;
     if(prlc) printf(f,lrc);
   }
   sl = 5; ir = 2;
   sl -= ir;
   if(sl != 3){
     lrc = 108;
     if(prlc) printf(f,lrc);
   }
   sl = 5; lr = 2;
   sl -= lr;
   if(sl != 3){
     lrc = 109;
     if(prlc) printf(f,lrc);
   }
   sl = 5; ur = 2;
   sl -= ur;
   if(sl != 3){
     lrc = 110;
     if(prlc) printf(f,lrc);
   }
   sl = 5; fr = 2;
   sl -= fr;
   if(sl != 3){
     lrc = 111;
     if(prlc) printf(f,lrc);
   }
   sl = 5; dr = 2;
   sl -= dr;
   if(sl != 3){
     lrc = 112;
     if(prlc) printf(f,lrc);
   }
   il = 5; cr = 2;
   il -= cr;
   if(il != 3){
     lrc = 113;
     if(prlc) printf(f,lrc);
   }
   il = 5; sr = 2;
   il -= sr;
   if(il != 3){
     lrc = 114;
     if(prlc) printf(f,lrc);
   }
   il = 5; ir = 2;
   il -= ir;
   if(il != 3){
     lrc = 115;
     if(prlc) printf(f,lrc);
   }
   il = 5; lr = 2;
   il -= lr;
   if(il != 3){
     lrc = 116;
     if(prlc) printf(f,lrc);
   }
   il = 5; ur = 2;
   il -= ur;
   if(il != 3){
     lrc = 117;
     if(prlc) printf(f,lrc);
   }
   il = 5; fr = 2;
   il -= fr;
   if(il != 3){
     lrc = 118;
     if(prlc) printf(f,lrc);
   }
   il = 5; dr = 2;
   il -= dr;
   if(il != 3){
     lrc = 119;
     if(prlc) printf(f,lrc);
   }
   ll = 5; cr = 2;
   ll -= cr;
   if(ll != 3){
     lrc = 120;
     if(prlc) printf(f,lrc);
   }
   ll = 5; sr = 2;
   ll -= sr;
   if(ll != 3){
     lrc = 121;
     if(prlc) printf(f,lrc);
   }
   ll = 5; ir = 2;
   ll -= ir;
   if(ll != 3){
     lrc = 122;
     if(prlc) printf(f,lrc);
   }
   ll = 5; lr = 2;
   ll -= lr;
   if(ll != 3){
     lrc = 123;
     if(prlc) printf(f,lrc);
   }
   ll = 5; ur = 2;
   ll -= ur;
   if(ll != 3){
     lrc = 124;
     if(prlc) printf(f,lrc);
   }
   ll = 5; fr = 2;
   ll -= fr;
   if(ll != 3){
     lrc = 125;
     if(prlc) printf(f,lrc);
   }
   ll = 5; dr = 2;
   ll -= dr;
   if(ll != 3){
     lrc = 126;
     if(prlc) printf(f,lrc);
   }
   ul = 5; cr = 2;
   ul -= cr;
   if(ul != 3){
     lrc = 127;
     if(prlc) printf(f,lrc);
   }
   ul = 5; sr = 2;
   ul -= sr;
   if(ul != 3){
     lrc = 128;
     if(prlc) printf(f,lrc);
   }
   ul = 5; ir = 2;
   ul -= ir;
   if(ul != 3){
     lrc = 129;
     if(prlc) printf(f,lrc);
   }
   ul = 5; lr = 2;
   ul -= lr;
   if(ul != 3){
     lrc = 130;
     if(prlc) printf(f,lrc);
   }
   ul = 5; ur = 2;
   ul -= ur;
   if(ul != 3){
     lrc = 131;
     if(prlc) printf(f,lrc);
   }
   ul = 5; fr = 2;
   ul -= fr;
   if(ul != 3){
     lrc = 132;
     if(prlc) printf(f,lrc);
   }
   ul = 5; dr = 2;
   ul -= dr;
   if(ul != 3){
     lrc = 133;
     if(prlc) printf(f,lrc);
   }
   fl = 5; cr = 2;
   fl -= cr;
   if(fl != 3){
     lrc = 134;
     if(prlc) printf(f,lrc);
   }
   fl = 5; sr = 2;
   fl -= sr;
   if(fl != 3){
     lrc = 135;
     if(prlc) printf(f,lrc);
   }
   fl = 5; ir = 2;
   fl -= ir;
   if(fl != 3){
     lrc = 136;
     if(prlc) printf(f,lrc);
   }
   fl = 5; lr = 2;
   fl -= lr;
   if(fl != 3){
     lrc = 137;
     if(prlc) printf(f,lrc);
   }
   fl = 5; ur = 2;
   fl -= ur;
   if(fl != 3){
     lrc = 138;
     if(prlc) printf(f,lrc);
   }
   fl = 5; fr = 2;
   fl -= fr;
   if(fl != 3){
     lrc = 139;
     if(prlc) printf(f,lrc);
   }
   fl = 5; dr = 2;
   fl -= dr;
   if(fl != 3){
     lrc = 140;
     if(prlc) printf(f,lrc);
   }
   dl = 5; cr = 2;
   dl -= cr;
   if(dl != 3){
     lrc = 141;
     if(prlc) printf(f,lrc);
   }
   dl = 5; sr = 2;
   dl -= sr;
   if(dl != 3){
     lrc = 142;
     if(prlc) printf(f,lrc);
   }
   dl = 5; ir = 2;
   dl -= ir;
   if(dl != 3){
     lrc = 143;
     if(prlc) printf(f,lrc);
   }
   dl = 5; lr = 2;
   dl -= lr;
   if(dl != 3){
     lrc = 144;
     if(prlc) printf(f,lrc);
   }
   dl = 5; ur = 2;
   dl -= ur;
   if(dl != 3){
     lrc = 145;
     if(prlc) printf(f,lrc);
   }
   dl = 5; fr = 2;
   dl -= fr;
   if(dl != 3){
     lrc = 146;
     if(prlc) printf(f,lrc);
   }
   dl = 5; dr = 2;
   dl -= dr;
   if(dl != 3){
     lrc = 147;
     if(prlc) printf(f,lrc);
   }
   cl = 5; cr = 2;
   cl *= cr;
   if(cl != 10){
     lrc = 148;
     if(prlc) printf(f,lrc);
   }
   cl = 5; sr = 2;
   cl *= sr;
   if(cl != 10){
     lrc = 149;
     if(prlc) printf(f,lrc);
   }
   cl = 5; ir = 2;
   cl *= ir;
   if(cl != 10){
     lrc = 150;
     if(prlc) printf(f,lrc);
   }
   cl = 5; lr = 2;
   cl *= lr;
   if(cl != 10){
     lrc = 151;
     if(prlc) printf(f,lrc);
   }
   cl = 5; ur = 2;
   cl *= ur;
   if(cl != 10){
     lrc = 152;
     if(prlc) printf(f,lrc);
   }
   cl = 5; fr = 2;
   cl *= fr;
   if(cl != 10){
     lrc = 153;
     if(prlc) printf(f,lrc);
   }
   cl = 5; dr = 2;
   cl *= dr;
   if(cl != 10){
     lrc = 154;
     if(prlc) printf(f,lrc);
   }
   sl = 5; cr = 2;
   sl *= cr;
   if(sl != 10){
     lrc = 155;
     if(prlc) printf(f,lrc);
   }
   sl = 5; sr = 2;
   sl *= sr;
   if(sl != 10){
     lrc = 156;
     if(prlc) printf(f,lrc);
   }
   sl = 5; ir = 2;
   sl *= ir;
   if(sl != 10){
     lrc = 157;
     if(prlc) printf(f,lrc);
   }
   sl = 5; lr = 2;
   sl *= lr;
   if(sl != 10){
     lrc = 158;
     if(prlc) printf(f,lrc);
   }
   sl = 5; ur = 2;
   sl *= ur;
   if(sl != 10){
     lrc = 159;
     if(prlc) printf(f,lrc);
   }
   sl = 5; fr = 2;
   sl *= fr;
   if(sl != 10){
     lrc = 160;
     if(prlc) printf(f,lrc);
   }
   sl = 5; dr = 2;
   sl *= dr;
   if(sl != 10){
     lrc = 161;
     if(prlc) printf(f,lrc);
   }
   il = 5; cr = 2;
   il *= cr;
   if(il != 10){
     lrc = 162;
     if(prlc) printf(f,lrc);
   }
   il = 5; sr = 2;
   il *= sr;
   if(il != 10){
     lrc = 163;
     if(prlc) printf(f,lrc);
   }
   il = 5; ir = 2;
   il *= ir;
   if(il != 10){
     lrc = 164;
     if(prlc) printf(f,lrc);
   }
   il = 5; lr = 2;
   il *= lr;
   if(il != 10){
     lrc = 165;
     if(prlc) printf(f,lrc);
   }
   il = 5; ur = 2;
   il *= ur;
   if(il != 10){
     lrc = 166;
     if(prlc) printf(f,lrc);
   }
   il = 5; fr = 2;
   il *= fr;
   if(il != 10){
     lrc = 167;
     if(prlc) printf(f,lrc);
   }
   il = 5; dr = 2;
   il *= dr;
   if(il != 10){
     lrc = 168;
     if(prlc) printf(f,lrc);
   }
   ll = 5; cr = 2;
   ll *= cr;
   if(ll != 10){
     lrc = 169;
     if(prlc) printf(f,lrc);
   }
   ll = 5; sr = 2;
   ll *= sr;
   if(ll != 10){
     lrc = 170;
     if(prlc) printf(f,lrc);
   }
   ll = 5; ir = 2;
   ll *= ir;
   if(ll != 10){
     lrc = 171;
     if(prlc) printf(f,lrc);
   }
   ll = 5; lr = 2;
   ll *= lr;
   if(ll != 10){
     lrc = 172;
     if(prlc) printf(f,lrc);
   }
   ll = 5; ur = 2;
   ll *= ur;
   if(ll != 10){
     lrc = 173;
     if(prlc) printf(f,lrc);
   }
   ll = 5; fr = 2;
   ll *= fr;
   if(ll != 10){
     lrc = 174;
     if(prlc) printf(f,lrc);
   }
   ll = 5; dr = 2;
   ll *= dr;
   if(ll != 10){
     lrc = 175;
     if(prlc) printf(f,lrc);
   }
   ul = 5; cr = 2;
   ul *= cr;
   if(ul != 10){
     lrc = 176;
     if(prlc) printf(f,lrc);
   }
   ul = 5; sr = 2;
   ul *= sr;
   if(ul != 10){
     lrc = 177;
     if(prlc) printf(f,lrc);
   }
   ul = 5; ir = 2;
   ul *= ir;
   if(ul != 10){
     lrc = 178;
     if(prlc) printf(f,lrc);
   }
   ul = 5; lr = 2;
   ul *= lr;
   if(ul != 10){
     lrc = 179;
     if(prlc) printf(f,lrc);
   }
   ul = 5; ur = 2;
   ul *= ur;
   if(ul != 10){
     lrc = 180;
     if(prlc) printf(f,lrc);
   }
   ul = 5; fr = 2;
   ul *= fr;
   if(ul != 10){
     lrc = 181;
     if(prlc) printf(f,lrc);
   }
   ul = 5; dr = 2;
   ul *= dr;
   if(ul != 10){
     lrc = 182;
     if(prlc) printf(f,lrc);
   }
   fl = 5; cr = 2;
   fl *= cr;
   if(fl != 10){
     lrc = 183;
     if(prlc) printf(f,lrc);
   }
   fl = 5; sr = 2;
   fl *= sr;
   if(fl != 10){
     lrc = 184;
     if(prlc) printf(f,lrc);
   }
   fl = 5; ir = 2;
   fl *= ir;
   if(fl != 10){
     lrc = 185;
     if(prlc) printf(f,lrc);
   }
   fl = 5; lr = 2;
   fl *= lr;
   if(fl != 10){
     lrc = 186;
     if(prlc) printf(f,lrc);
   }
   fl = 5; ur = 2;
   fl *= ur;
   if(fl != 10){
     lrc = 187;
     if(prlc) printf(f,lrc);
   }
   fl = 5; fr = 2;
   fl *= fr;
   if(fl != 10){
     lrc = 188;
     if(prlc) printf(f,lrc);
   }
   fl = 5; dr = 2;
   fl *= dr;
   if(fl != 10){
     lrc = 189;
     if(prlc) printf(f,lrc);
   }
   dl = 5; cr = 2;
   dl *= cr;
   if(dl != 10){
     lrc = 190;
     if(prlc) printf(f,lrc);
   }
   dl = 5; sr = 2;
   dl *= sr;
   if(dl != 10){
     lrc = 191;
     if(prlc) printf(f,lrc);
   }
   dl = 5; ir = 2;
   dl *= ir;
   if(dl != 10){
     lrc = 192;
     if(prlc) printf(f,lrc);
   }
   dl = 5; lr = 2;
   dl *= lr;
   if(dl != 10){
     lrc = 193;
     if(prlc) printf(f,lrc);
   }
   dl = 5; ur = 2;
   dl *= ur;
   if(dl != 10){
     lrc = 194;
     if(prlc) printf(f,lrc);
   }
   dl = 5; fr = 2;
   dl *= fr;
   if(dl != 10){
     lrc = 195;
     if(prlc) printf(f,lrc);
   }
   dl = 5; dr = 2;
   dl *= dr;
   if(dl != 10){
     lrc = 196;
     if(prlc) printf(f,lrc);
   }
   cl = 5; cr = 2;
   cl /= cr;
   if(cl != 2){
     lrc = 197;
     if(prlc) printf(f,lrc);
   }
   cl = 5; sr = 2;
   cl /= sr;
   if(cl != 2){
     lrc = 198;
     if(prlc) printf(f,lrc);
   }
   cl = 5; ir = 2;
   cl /= ir;
   if(cl != 2){
     lrc = 199;
     if(prlc) printf(f,lrc);
   }
   cl = 5; lr = 2;
   cl /= lr;
   if(cl != 2){
     lrc = 200;
     if(prlc) printf(f,lrc);
   }
   cl = 5; ur = 2;
   cl /= ur;
   if(cl != 2){
     lrc = 201;
     if(prlc) printf(f,lrc);
   }
   cl = 5; fr = 2;
   cl /= fr;
   if(cl != 2){
     lrc = 202;
     if(prlc) printf(f,lrc);
   }
   cl = 5; dr = 2;
   cl /= dr;
   if(cl != 2){
     lrc = 203;
     if(prlc) printf(f,lrc);
   }
   sl = 5; cr = 2;
   sl /= cr;
   if(sl != 2){
     lrc = 204;
     if(prlc) printf(f,lrc);
   }
   sl = 5; sr = 2;
   sl /= sr;
   if(sl != 2){
     lrc = 205;
     if(prlc) printf(f,lrc);
   }
   sl = 5; ir = 2;
   sl /= ir;
   if(sl != 2){
     lrc = 206;
     if(prlc) printf(f,lrc);
   }
   sl = 5; lr = 2;
   sl /= lr;
   if(sl != 2){
     lrc = 207;
     if(prlc) printf(f,lrc);
   }
   sl = 5; ur = 2;
   sl /= ur;
   if(sl != 2){
     lrc = 208;
     if(prlc) printf(f,lrc);
   }
   sl = 5; fr = 2;
   sl /= fr;
   if(sl != 2){
     lrc = 209;
     if(prlc) printf(f,lrc);
   }
   sl = 5; dr = 2;
   sl /= dr;
   if(sl != 2){
     lrc = 210;
     if(prlc) printf(f,lrc);
   }
   il = 5; cr = 2;
   il /= cr;
   if(il != 2){
     lrc = 211;
     if(prlc) printf(f,lrc);
   }
   il = 5; sr = 2;
   il /= sr;
   if(il != 2){
     lrc = 212;
     if(prlc) printf(f,lrc);
   }
   il = 5; ir = 2;
   il /= ir;
   if(il != 2){
     lrc = 213;
     if(prlc) printf(f,lrc);
   }
   il = 5; lr = 2;
   il /= lr;
   if(il != 2){
     lrc = 214;
     if(prlc) printf(f,lrc);
   }
   il = 5; ur = 2;
   il /= ur;
   if(il != 2){
     lrc = 215;
     if(prlc) printf(f,lrc);
   }
   il = 5; fr = 2;
   il /= fr;
   if(il != 2){
     lrc = 216;
     if(prlc) printf(f,lrc);
   }
   il = 5; dr = 2;
   il /= dr;
   if(il != 2){
     lrc = 217;
     if(prlc) printf(f,lrc);
   }
   ll = 5; cr = 2;
   ll /= cr;
   if(ll != 2){
     lrc = 218;
     if(prlc) printf(f,lrc);
   }
   ll = 5; sr = 2;
   ll /= sr;
   if(ll != 2){
     lrc = 219;
     if(prlc) printf(f,lrc);
   }
   ll = 5; ir = 2;
   ll /= ir;
   if(ll != 2){
     lrc = 220;
     if(prlc) printf(f,lrc);
   }
   ll = 5; lr = 2;
   ll /= lr;
   if(ll != 2){
     lrc = 221;
     if(prlc) printf(f,lrc);
   }
   ll = 5; ur = 2;
   ll /= ur;
   if(ll != 2){
     lrc = 222;
     if(prlc) printf(f,lrc);
   }
   ll = 5; fr = 2;
   ll /= fr;
   if(ll != 2){
     lrc = 223;
     if(prlc) printf(f,lrc);
   }
   ll = 5; dr = 2;
   ll /= dr;
   if(ll != 2){
     lrc = 224;
     if(prlc) printf(f,lrc);
   }
   ul = 5; cr = 2;
   ul /= cr;
   if(ul != 2){
     lrc = 225;
     if(prlc) printf(f,lrc);
   }
   ul = 5; sr = 2;
   ul /= sr;
   if(ul != 2){
     lrc = 226;
     if(prlc) printf(f,lrc);
   }
   ul = 5; ir = 2;
   ul /= ir;
   if(ul != 2){
     lrc = 227;
     if(prlc) printf(f,lrc);
   }
   ul = 5; lr = 2;
   ul /= lr;
   if(ul != 2){
     lrc = 228;
     if(prlc) printf(f,lrc);
   }
   ul = 5; ur = 2;
   ul /= ur;
   if(ul != 2){
     lrc = 229;
     if(prlc) printf(f,lrc);
   }
   ul = 5; fr = 2;
   ul /= fr;
   if(ul != 2){
     lrc = 230;
     if(prlc) printf(f,lrc);
   }
   ul = 5; dr = 2;
   ul /= dr;
   if(ul != 2){
     lrc = 231;
     if(prlc) printf(f,lrc);
   }
   fl = 5; cr = 2;
   fl /= cr;
   if(fl != 2.5){
     lrc = 232;
     if(prlc) printf(f,lrc);
   }
   fl = 5; sr = 2;
   fl /= sr;
   if(fl != 2.5){
     lrc = 233;
     if(prlc) printf(f,lrc);
   }
   fl = 5; ir = 2;
   fl /= ir;
   if(fl != 2.5){
     lrc = 234;
     if(prlc) printf(f,lrc);
   }
   fl = 5; lr = 2;
   fl /= lr;
   if(fl != 2.5){
     lrc = 235;
     if(prlc) printf(f,lrc);
   }
   fl = 5; ur = 2;
   fl /= ur;
   if(fl != 2.5){
     lrc = 236;
     if(prlc) printf(f,lrc);
   }
   fl = 5; fr = 2;
   fl /= fr;
   if(fl != 2.5){
     lrc = 237;
     if(prlc) printf(f,lrc);
   }
   fl = 5; dr = 2;
   fl /= dr;
   if(fl != 2.5){
     lrc = 238;
     if(prlc) printf(f,lrc);
   }
   dl = 5; cr = 2;
   dl /= cr;
   if(dl != 2.5){
     lrc = 239;
     if(prlc) printf(f,lrc);
   }
   dl = 5; sr = 2;
   dl /= sr;
   if(dl != 2.5){
     lrc = 240;
     if(prlc) printf(f,lrc);
   }
   dl = 5; ir = 2;
   dl /= ir;
   if(dl != 2.5){
     lrc = 241;
     if(prlc) printf(f,lrc);
   }
   dl = 5; lr = 2;
   dl /= lr;
   if(dl != 2.5){
     lrc = 242;
     if(prlc) printf(f,lrc);
   }
   dl = 5; ur = 2;
   dl /= ur;
   if(dl != 2.5){
     lrc = 243;
     if(prlc) printf(f,lrc);
   }
   dl = 5; fr = 2;
   dl /= fr;
   if(dl != 2.5){
     lrc = 244;
     if(prlc) printf(f,lrc);
   }
   dl = 5; dr = 2;
   dl /= dr;
   if(dl != 2.5){
     lrc = 245;
     if(prlc) printf(f,lrc);
   }
   cl = 5; cr = 2;
   cl %= cr;
   if(cl != 1){
     lrc = 246;
     if(prlc) printf(f,lrc);
   }
   cl = 5; sr = 2;
   cl %= sr;
   if(cl != 1){
     lrc = 247;
     if(prlc) printf(f,lrc);
   }
   cl = 5; ir = 2;
   cl %= ir;
   if(cl != 1){
     lrc = 248;
     if(prlc) printf(f,lrc);
   }
   cl = 5; lr = 2;
   cl %= lr;
   if(cl != 1){
     lrc = 249;
     if(prlc) printf(f,lrc);
   }
   cl = 5; ur = 2;
   cl %= ur;
   if(cl != 1){
     lrc = 250;
     if(prlc) printf(f,lrc);
   }
   sl = 5; cr = 2;
   sl %= cr;
   if(sl != 1){
     lrc = 251;
     if(prlc) printf(f,lrc);
   }
   sl = 5; sr = 2;
   sl %= sr;
   if(sl != 1){
     lrc = 252;
     if(prlc) printf(f,lrc);
   }
   sl = 5; ir = 2;
   sl %= ir;
   if(sl != 1){
     lrc = 253;
     if(prlc) printf(f,lrc);
   }
   sl = 5; lr = 2;
   sl %= lr;
   if(sl != 1){
     lrc = 254;
     if(prlc) printf(f,lrc);
   }
   sl = 5; ur = 2;
   sl %= ur;
   if(sl != 1){
     lrc = 255;
     if(prlc) printf(f,lrc);
   }
   il = 5; cr = 2;
   il %= cr;
   if(il != 1){
     lrc = 256;
     if(prlc) printf(f,lrc);
   }
   il = 5; sr = 2;
   il %= sr;
   if(il != 1){
     lrc = 257;
     if(prlc) printf(f,lrc);
   }
   il = 5; ir = 2;
   il %= ir;
   if(il != 1){
     lrc = 258;
     if(prlc) printf(f,lrc);
   }
   il = 5; lr = 2;
   il %= lr;
   if(il != 1){
     lrc = 259;
     if(prlc) printf(f,lrc);
   }
   il = 5; ur = 2;
   il %= ur;
   if(il != 1){
     lrc = 260;
     if(prlc) printf(f,lrc);
   }
   ll = 5; cr = 2;
   ll %= cr;
   if(ll != 1){
     lrc = 261;
     if(prlc) printf(f,lrc);
   }
   ll = 5; sr = 2;
   ll %= sr;
   if(ll != 1){
     lrc = 262;
     if(prlc) printf(f,lrc);
   }
   ll = 5; ir = 2;
   ll %= ir;
   if(ll != 1){
     lrc = 263;
     if(prlc) printf(f,lrc);
   }
   ll = 5; lr = 2;
   ll %= lr;
   if(ll != 1){
     lrc = 264;
     if(prlc) printf(f,lrc);
   }
   ll = 5; ur = 2;
   ll %= ur;
   if(ll != 1){
     lrc = 265;
     if(prlc) printf(f,lrc);
   }
   ul = 5; cr = 2;
   ul %= cr;
   if(ul != 1){
     lrc = 266;
     if(prlc) printf(f,lrc);
   }
   ul = 5; sr = 2;
   ul %= sr;
   if(ul != 1){
     lrc = 267;
     if(prlc) printf(f,lrc);
   }
   ul = 5; ir = 2;
   ul %= ir;
   if(ul != 1){
     lrc = 268;
     if(prlc) printf(f,lrc);
   }
   ul = 5; lr = 2;
   ul %= lr;
   if(ul != 1){
     lrc = 269;
     if(prlc) printf(f,lrc);
   }
   ul = 5; ur = 2;
   ul %= ur;
   if(ul != 1){
     lrc = 270;
     if(prlc) printf(f,lrc);
   }
   cl = 5; cr = 2;
   cl >>= cr;
   if(cl != 1){
     lrc = 271;
     if(prlc) printf(f,lrc);
   }
   cl = 5; sr = 2;
   cl >>= sr;
   if(cl != 1){
     lrc = 272;
     if(prlc) printf(f,lrc);
   }
   cl = 5; ir = 2;
   cl >>= ir;
   if(cl != 1){
     lrc = 273;
     if(prlc) printf(f,lrc);
   }
   cl = 5; lr = 2;
   cl >>= lr;
   if(cl != 1){
     lrc = 274;
     if(prlc) printf(f,lrc);
   }
   cl = 5; ur = 2;
   cl >>= ur;
   if(cl != 1){
     lrc = 275;
     if(prlc) printf(f,lrc);
   }
   sl = 5; cr = 2;
   sl >>= cr;
   if(sl != 1){
     lrc = 276;
     if(prlc) printf(f,lrc);
   }
   sl = 5; sr = 2;
   sl >>= sr;
   if(sl != 1){
     lrc = 277;
     if(prlc) printf(f,lrc);
   }
   sl = 5; ir = 2;
   sl >>= ir;
   if(sl != 1){
     lrc = 278;
     if(prlc) printf(f,lrc);
   }
   sl = 5; lr = 2;
   sl >>= lr;
   if(sl != 1){
     lrc = 279;
     if(prlc) printf(f,lrc);
   }
   sl = 5; ur = 2;
   sl >>= ur;
   if(sl != 1){
     lrc = 280;
     if(prlc) printf(f,lrc);
   }
   il = 5; cr = 2;
   il >>= cr;
   if(il != 1){
     lrc = 281;
     if(prlc) printf(f,lrc);
   }
   il = 5; sr = 2;
   il >>= sr;
   if(il != 1){
     lrc = 282;
     if(prlc) printf(f,lrc);
   }
   il = 5; ir = 2;
   il >>= ir;
   if(il != 1){
     lrc = 283;
     if(prlc) printf(f,lrc);
   }
   il = 5; lr = 2;
   il >>= lr;
   if(il != 1){
     lrc = 284;
     if(prlc) printf(f,lrc);
   }
   il = 5; ur = 2;
   il >>= ur;
   if(il != 1){
     lrc = 285;
     if(prlc) printf(f,lrc);
   }
   ll = 5; cr = 2;
   ll >>= cr;
   if(ll != 1){
     lrc = 286;
     if(prlc) printf(f,lrc);
   }
   ll = 5; sr = 2;
   ll >>= sr;
   if(ll != 1){
     lrc = 287;
     if(prlc) printf(f,lrc);
   }
   ll = 5; ir = 2;
   ll >>= ir;
   if(ll != 1){
     lrc = 288;
     if(prlc) printf(f,lrc);
   }
   ll = 5; lr = 2;
   ll >>= lr;
   if(ll != 1){
     lrc = 289;
     if(prlc) printf(f,lrc);
   }
   ll = 5; ur = 2;
   ll >>= ur;
   if(ll != 1){
     lrc = 290;
     if(prlc) printf(f,lrc);
   }
   ul = 5; cr = 2;
   ul >>= cr;
   if(ul != 1){
     lrc = 291;
     if(prlc) printf(f,lrc);
   }
   ul = 5; sr = 2;
   ul >>= sr;
   if(ul != 1){
     lrc = 292;
     if(prlc) printf(f,lrc);
   }
   ul = 5; ir = 2;
   ul >>= ir;
   if(ul != 1){
     lrc = 293;
     if(prlc) printf(f,lrc);
   }
   ul = 5; lr = 2;
   ul >>= lr;
   if(ul != 1){
     lrc = 294;
     if(prlc) printf(f,lrc);
   }
   ul = 5; ur = 2;
   ul >>= ur;
   if(ul != 1){
     lrc = 295;
     if(prlc) printf(f,lrc);
   }
   cl = 5; cr = 2;
   cl <<= cr;
   if(cl != 20){
     lrc = 296;
     if(prlc) printf(f,lrc);
   }
   cl = 5; sr = 2;
   cl <<= sr;
   if(cl != 20){
     lrc = 297;
     if(prlc) printf(f,lrc);
   }
   cl = 5; ir = 2;
   cl <<= ir;
   if(cl != 20){
     lrc = 298;
     if(prlc) printf(f,lrc);
   }
   cl = 5; lr = 2;
   cl <<= lr;
   if(cl != 20){
     lrc = 299;
     if(prlc) printf(f,lrc);
   }
   cl = 5; ur = 2;
   cl <<= ur;
   if(cl != 20){
     lrc = 300;
     if(prlc) printf(f,lrc);
   }
   sl = 5; cr = 2;
   sl <<= cr;
   if(sl != 20){
     lrc = 301;
     if(prlc) printf(f,lrc);
   }
   sl = 5; sr = 2;
   sl <<= sr;
   if(sl != 20){
     lrc = 302;
     if(prlc) printf(f,lrc);
   }
   sl = 5; ir = 2;
   sl <<= ir;
   if(sl != 20){
     lrc = 303;
     if(prlc) printf(f,lrc);
   }
   sl = 5; lr = 2;
   sl <<= lr;
   if(sl != 20){
     lrc = 304;
     if(prlc) printf(f,lrc);
   }
   sl = 5; ur = 2;
   sl <<= ur;
   if(sl != 20){
     lrc = 305;
     if(prlc) printf(f,lrc);
   }
   il = 5; cr = 2;
   il <<= cr;
   if(il != 20){
     lrc = 306;
     if(prlc) printf(f,lrc);
   }
   il = 5; sr = 2;
   il <<= sr;
   if(il != 20){
     lrc = 307;
     if(prlc) printf(f,lrc);
   }
   il = 5; ir = 2;
   il <<= ir;
   if(il != 20){
     lrc = 308;
     if(prlc) printf(f,lrc);
   }
   il = 5; lr = 2;
   il <<= lr;
   if(il != 20){
     lrc = 309;
     if(prlc) printf(f,lrc);
   }
   il = 5; ur = 2;
   il <<= ur;
   if(il != 20){
     lrc = 310;
     if(prlc) printf(f,lrc);
   }
   ll = 5; cr = 2;
   ll <<= cr;
   if(ll != 20){
     lrc = 311;
     if(prlc) printf(f,lrc);
   }
   ll = 5; sr = 2;
   ll <<= sr;
   if(ll != 20){
     lrc = 312;
     if(prlc) printf(f,lrc);
   }
   ll = 5; ir = 2;
   ll <<= ir;
   if(ll != 20){
     lrc = 313;
     if(prlc) printf(f,lrc);
   }
   ll = 5; lr = 2;
   ll <<= lr;
   if(ll != 20){
     lrc = 314;
     if(prlc) printf(f,lrc);
   }
   ll = 5; ur = 2;
   ll <<= ur;
   if(ll != 20){
     lrc = 315;
     if(prlc) printf(f,lrc);
   }
   ul = 5; cr = 2;
   ul <<= cr;
   if(ul != 20){
     lrc = 316;
     if(prlc) printf(f,lrc);
   }
   ul = 5; sr = 2;
   ul <<= sr;
   if(ul != 20){
     lrc = 317;
     if(prlc) printf(f,lrc);
   }
   ul = 5; ir = 2;
   ul <<= ir;
   if(ul != 20){
     lrc = 318;
     if(prlc) printf(f,lrc);
   }
   ul = 5; lr = 2;
   ul <<= lr;
   if(ul != 20){
     lrc = 319;
     if(prlc) printf(f,lrc);
   }
   ul = 5; ur = 2;
   ul <<= ur;
   if(ul != 20){
     lrc = 320;
     if(prlc) printf(f,lrc);
   }
   cl = 12; cr = 10;
   cl &= cr;
   if(cl != 8){
     lrc = 321;
     if(prlc) printf(f,lrc);
   }
   cl = 12; sr = 10;
   cl &= sr;
   if(cl != 8){
     lrc = 322;
     if(prlc) printf(f,lrc);
   }
   cl = 12; ir = 10;
   cl &= ir;
   if(cl != 8){
     lrc = 323;
     if(prlc) printf(f,lrc);
   }
   cl = 12; lr = 10;
   cl &= lr;
   if(cl != 8){
     lrc = 324;
     if(prlc) printf(f,lrc);
   }
   cl = 12; ur = 10;
   cl &= ur;
   if(cl != 8){
     lrc = 325;
     if(prlc) printf(f,lrc);
   }
   sl = 12; cr = 10;
   sl &= cr;
   if(sl != 8){
     lrc = 326;
     if(prlc) printf(f,lrc);
   }
   sl = 12; sr = 10;
   sl &= sr;
   if(sl != 8){
     lrc = 327;
     if(prlc) printf(f,lrc);
   }
   sl = 12; ir = 10;
   sl &= ir;
   if(sl != 8){
     lrc = 328;
     if(prlc) printf(f,lrc);
   }
   sl = 12; lr = 10;
   sl &= lr;
   if(sl != 8){
     lrc = 329;
     if(prlc) printf(f,lrc);
   }
   sl = 12; ur = 10;
   sl &= ur;
   if(sl != 8){
     lrc = 330;
     if(prlc) printf(f,lrc);
   }
   il = 12; cr = 10;
   il &= cr;
   if(il != 8){
     lrc = 331;
     if(prlc) printf(f,lrc);
   }
   il = 12; sr = 10;
   il &= sr;
   if(il != 8){
     lrc = 332;
     if(prlc) printf(f,lrc);
   }
   il = 12; ir = 10;
   il &= ir;
   if(il != 8){
     lrc = 333;
     if(prlc) printf(f,lrc);
   }
   il = 12; lr = 10;
   il &= lr;
   if(il != 8){
     lrc = 334;
     if(prlc) printf(f,lrc);
   }
   il = 12; ur = 10;
   il &= ur;
   if(il != 8){
     lrc = 335;
     if(prlc) printf(f,lrc);
   }
   ll = 12; cr = 10;
   ll &= cr;
   if(ll != 8){
     lrc = 336;
     if(prlc) printf(f,lrc);
   }
   ll = 12; sr = 10;
   ll &= sr;
   if(ll != 8){
     lrc = 337;
     if(prlc) printf(f,lrc);
   }
   ll = 12; ir = 10;
   ll &= ir;
   if(ll != 8){
     lrc = 338;
     if(prlc) printf(f,lrc);
   }
   ll = 12; lr = 10;
   ll &= lr;
   if(ll != 8){
     lrc = 339;
     if(prlc) printf(f,lrc);
   }
   ll = 12; ur = 10;
   ll &= ur;
   if(ll != 8){
     lrc = 340;
     if(prlc) printf(f,lrc);
   }
   ul = 12; cr = 10;
   ul &= cr;
   if(ul != 8){
     lrc = 341;
     if(prlc) printf(f,lrc);
   }
   ul = 12; sr = 10;
   ul &= sr;
   if(ul != 8){
     lrc = 342;
     if(prlc) printf(f,lrc);
   }
   ul = 12; ir = 10;
   ul &= ir;
   if(ul != 8){
     lrc = 343;
     if(prlc) printf(f,lrc);
   }
   ul = 12; lr = 10;
   ul &= lr;
   if(ul != 8){
     lrc = 344;
     if(prlc) printf(f,lrc);
   }
   ul = 12; ur = 10;
   ul &= ur;
   if(ul != 8){
     lrc = 345;
     if(prlc) printf(f,lrc);
   }
   cl = 12; cr = 10;
   cl ^= cr;
   if(cl != 6){
     lrc = 346;
     if(prlc) printf(f,lrc);
   }
   cl = 12; sr = 10;
   cl ^= sr;
   if(cl != 6){
     lrc = 347;
     if(prlc) printf(f,lrc);
   }
   cl = 12; ir = 10;
   cl ^= ir;
   if(cl != 6){
     lrc = 348;
     if(prlc) printf(f,lrc);
   }
   cl = 12; lr = 10;
   cl ^= lr;
   if(cl != 6){
     lrc = 349;
     if(prlc) printf(f,lrc);
   }
   cl = 12; ur = 10;
   cl ^= ur;
   if(cl != 6){
     lrc = 350;
     if(prlc) printf(f,lrc);
   }
   sl = 12; cr = 10;
   sl ^= cr;
   if(sl != 6){
     lrc = 351;
     if(prlc) printf(f,lrc);
   }
   sl = 12; sr = 10;
   sl ^= sr;
   if(sl != 6){
     lrc = 352;
     if(prlc) printf(f,lrc);
   }
   sl = 12; ir = 10;
   sl ^= ir;
   if(sl != 6){
     lrc = 353;
     if(prlc) printf(f,lrc);
   }
   sl = 12; lr = 10;
   sl ^= lr;
   if(sl != 6){
     lrc = 354;
     if(prlc) printf(f,lrc);
   }
   sl = 12; ur = 10;
   sl ^= ur;
   if(sl != 6){
     lrc = 355;
     if(prlc) printf(f,lrc);
   }
   il = 12; cr = 10;
   il ^= cr;
   if(il != 6){
     lrc = 356;
     if(prlc) printf(f,lrc);
   }
   il = 12; sr = 10;
   il ^= sr;
   if(il != 6){
     lrc = 357;
     if(prlc) printf(f,lrc);
   }
   il = 12; ir = 10;
   il ^= ir;
   if(il != 6){
     lrc = 358;
     if(prlc) printf(f,lrc);
   }
   il = 12; lr = 10;
   il ^= lr;
   if(il != 6){
     lrc = 359;
     if(prlc) printf(f,lrc);
   }
   il = 12; ur = 10;
   il ^= ur;
   if(il != 6){
     lrc = 360;
     if(prlc) printf(f,lrc);
   }
   ll = 12; cr = 10;
   ll ^= cr;
   if(ll != 6){
     lrc = 361;
     if(prlc) printf(f,lrc);
   }
   ll = 12; sr = 10;
   ll ^= sr;
   if(ll != 6){
     lrc = 362;
     if(prlc) printf(f,lrc);
   }
   ll = 12; ir = 10;
   ll ^= ir;
   if(ll != 6){
     lrc = 363;
     if(prlc) printf(f,lrc);
   }
   ll = 12; lr = 10;
   ll ^= lr;
   if(ll != 6){
     lrc = 364;
     if(prlc) printf(f,lrc);
   }
   ll = 12; ur = 10;
   ll ^= ur;
   if(ll != 6){
     lrc = 365;
     if(prlc) printf(f,lrc);
   }
   ul = 12; cr = 10;
   ul ^= cr;
   if(ul != 6){
     lrc = 366;
     if(prlc) printf(f,lrc);
   }
   ul = 12; sr = 10;
   ul ^= sr;
   if(ul != 6){
     lrc = 367;
     if(prlc) printf(f,lrc);
   }
   ul = 12; ir = 10;
   ul ^= ir;
   if(ul != 6){
     lrc = 368;
     if(prlc) printf(f,lrc);
   }
   ul = 12; lr = 10;
   ul ^= lr;
   if(ul != 6){
     lrc = 369;
     if(prlc) printf(f,lrc);
   }
   ul = 12; ur = 10;
   ul ^= ur;
   if(ul != 6){
     lrc = 370;
     if(prlc) printf(f,lrc);
   }
   cl = 12; cr = 10;
   cl |= cr;
   if(cl != 14){
     lrc = 371;
     if(prlc) printf(f,lrc);
   }
   cl = 12; sr = 10;
   cl |= sr;
   if(cl != 14){
     lrc = 372;
     if(prlc) printf(f,lrc);
   }
   cl = 12; ir = 10;
   cl |= ir;
   if(cl != 14){
     lrc = 373;
     if(prlc) printf(f,lrc);
   }
   cl = 12; lr = 10;
   cl |= lr;
   if(cl != 14){
     lrc = 374;
     if(prlc) printf(f,lrc);
   }
   cl = 12; ur = 10;
   cl |= ur;
   if(cl != 14){
     lrc = 375;
     if(prlc) printf(f,lrc);
   }
   sl = 12; cr = 10;
   sl |= cr;
   if(sl != 14){
     lrc = 376;
     if(prlc) printf(f,lrc);
   }
   sl = 12; sr = 10;
   sl |= sr;
   if(sl != 14){
     lrc = 377;
     if(prlc) printf(f,lrc);
   }
   sl = 12; ir = 10;
   sl |= ir;
   if(sl != 14){
     lrc = 378;
     if(prlc) printf(f,lrc);
   }
   sl = 12; lr = 10;
   sl |= lr;
   if(sl != 14){
     lrc = 379;
     if(prlc) printf(f,lrc);
   }
   sl = 12; ur = 10;
   sl |= ur;
   if(sl != 14){
     lrc = 380;
     if(prlc) printf(f,lrc);
   }
   il = 12; cr = 10;
   il |= cr;
   if(il != 14){
     lrc = 381;
     if(prlc) printf(f,lrc);
   }
   il = 12; sr = 10;
   il |= sr;
   if(il != 14){
     lrc = 382;
     if(prlc) printf(f,lrc);
   }
   il = 12; ir = 10;
   il |= ir;
   if(il != 14){
     lrc = 383;
     if(prlc) printf(f,lrc);
   }
   il = 12; lr = 10;
   il |= lr;
   if(il != 14){
     lrc = 384;
     if(prlc) printf(f,lrc);
   }
   il = 12; ur = 10;
   il |= ur;
   if(il != 14){
     lrc = 385;
     if(prlc) printf(f,lrc);
   }
   ll = 12; cr = 10;
   ll |= cr;
   if(ll != 14){
     lrc = 386;
     if(prlc) printf(f,lrc);
   }
   ll = 12; sr = 10;
   ll |= sr;
   if(ll != 14){
     lrc = 387;
     if(prlc) printf(f,lrc);
   }
   ll = 12; ir = 10;
   ll |= ir;
   if(ll != 14){
     lrc = 388;
     if(prlc) printf(f,lrc);
   }
   ll = 12; lr = 10;
   ll |= lr;
   if(ll != 14){
     lrc = 389;
     if(prlc) printf(f,lrc);
   }
   ll = 12; ur = 10;
   ll |= ur;
   if(ll != 14){
     lrc = 390;
     if(prlc) printf(f,lrc);
   }
   ul = 12; cr = 10;
   ul |= cr;
   if(ul != 14){
     lrc = 391;
     if(prlc) printf(f,lrc);
   }
   ul = 12; sr = 10;
   ul |= sr;
   if(ul != 14){
     lrc = 392;
     if(prlc) printf(f,lrc);
   }
   ul = 12; ir = 10;
   ul |= ir;
   if(ul != 14){
     lrc = 393;
     if(prlc) printf(f,lrc);
   }
   ul = 12; lr = 10;
   ul |= lr;
   if(ul != 14){
     lrc = 394;
     if(prlc) printf(f,lrc);
   }
   ul = 12; ur = 10;
   ul |= ur;
   if(ul != 14){
     lrc = 395;
     if(prlc) printf(f,lrc);
   }
   if(lrc != 0) {
     rc = 1;
     if(pd0->flgd != 0) printf(s714er,1);
   }
   return rc;
}
