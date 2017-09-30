// check diagnostic for incorrect array redeclaration

extern char x1[];
extern char x2[5];
extern char x3[];
extern char x4[5];

extern unsigned char ux1[];
extern unsigned char ux2[5];
extern unsigned char ux3[];
extern unsigned char ux4[5];

unsigned int x1[];
unsigned int x2[];
unsigned int x3[5];
unsigned int x4[5];

int ux1[];
int ux2[];
int ux3[5];
int ux4[5];

unsigned int x5[];
unsigned int x6[];
unsigned int x7[5];
unsigned int x8[5];

int ux5[];
int ux6[];
int ux7[5];
int ux8[5];

extern char x5[];
extern char x6[5];
extern char x7[];
extern char x8[5];

extern unsigned char ux5[];
extern unsigned char ux6[5];
extern unsigned char ux7[];
extern unsigned char ux8[5];

extern char c1[];
extern char c1[5];
extern char c1[];
extern char c1[6];

extern int a1[5];
extern int a2[6];

int a1[6];
int a2[5];

int a3[6];
int a4[5];

extern int a3[5];
extern int a4[6];

void main( void )
{
}
