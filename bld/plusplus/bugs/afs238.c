// Compiler crash with switch -d2
extern  unsigned I[];
struct S0 { unsigned a,b,c,d; };
struct S1 { struct S0 *S; };
struct S2 { unsigned a,b; };
struct S3 { struct S2 S; char a; };
struct S4 { unsigned a,b,c,d,e; char f; };
struct S5 { struct S3 Sa[2]; struct S4 Sb; };
extern __segment Seg;
#define P ((struct S5 far *)(Seg:>(void __based(void) *)0))
extern  struct  S1 A[];
void    f(unsigned,unsigned,unsigned,unsigned);
void    F(unsigned i) {
  switch (i) {
  case 0:
    f(A[0].S[0].a,A[0].S[0].b,A[0].S[0].c,A[0].S[0].d);
    P->Sb.c=I[0];
    P->Sb.a=I[1];
    P->Sb.b=I[2];
    P->Sb.d=I[3];
    P->Sb.e=
        (long)(P->Sb.c-P->Sb.a)*
          (P->Sa[P->Sb.f].S.b-P->Sa[P->Sb.f].S.a)/
          (P->Sb.b-P->Sb.a)+
      P->Sa[P->Sb.f].S.a; }}

/*
Program: E:\WATCOM\BINW\WCC.EXE
CmdLine:  wat003 -d2
**** Access violation ****
OS=OS/2 BaseAddr=00080000 CS:EIP=005B:000E0D36 SS:ESP=0053:00037F48
EAX=00000000 EBX=00000000 ECX=00080000 EDX=00000000
ESI=00000000 EDI=0012C480 EBP=00000000 FLG=00092293
DS=0053 ES=0053 FS=150B GS=0000
00000000 00000000 00000000 00000000 00000000 00000000 0012DAAC 00080000
00000000 00000000 00000000 00000000 00000000 0012C4A4 00000000 00038040
0012C480 0012C480 00129AC4 00000000 C0FFDC00 000E06E4 00129AC4 00129AC4
000DD9A2 C0FFDC00 00000001 00129AC4 000DDC64 0012C400 00038040 00129AC4
CS:EIP -> 8B 76 04 31 C0 8A 46 22 83 F8 4B 0F 84 D3 01 00
Environment Variables:
WP_OBJHANDLE=190184
USER_INI=C:\OS2\OS2.INI
SYSTEM_INI=C:\OS2\OS2SYS.INI
OS2_SHELL=C:\OS2\CMD.EXE
AUTOSTART=PROGRAMS,TASKLIST,FOLDERS,LAUNCHPAD
RUNWORKPLACE=C:\OS2\PMSHELL.EXE
COMSPEC=C:\OS2\CMD.EXE
*/
