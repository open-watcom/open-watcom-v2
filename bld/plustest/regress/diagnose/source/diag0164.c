char *p;
unsigned char *q;
signed char *r;

enum E1 { CE1A, CE1B };
enum E2 { CE2A, CE2B };

E1 *pe1;
E2 *pe2;

struct X11 {
    E1 x1;
    E2 x2;
};

E1 X11::* mp1;
E2 X11::* mp2;

void vio( unsigned char * );

E1 x1( E2 );
E2 x2( E1 );
E2 (*fp1)( E1 );

void fio( E1 ( &p )( E2 ) );

void foo()
{
    vio( "asdf" );
    pe1 = pe2;
    pe2 = pe1;
    p = q;
    q = p;
    p = r;
    q = r;
    r = p;
    r = q;
    fp1 = x1;
    fio( x2 );
    fio( *fp1 );
    mp1 = &X11::x2;
    mp2 = &X11::x1;
}

struct A {
    int z(); 
}; 
 
int f(void) 
{ 
    A obj; 
    obj.z + 1;
    obj.z - 1;
    obj.z << 1;
    obj.z * 1;
    obj.z > 1;
    obj.z += 1;
    obj.z <<= 1;
    obj.z *= 1;
    obj.z =  0;
    obj.z ++;
    return 1; 
} 

void poo()          // should not cause errors
{
    p = ( __typeof(p) )q;
    q = ( __typeof(q) )p;
    p = ( __typeof(p) )r;
    q = ( __typeof(q) )r;
    r = ( __typeof(r) )p;
    r = ( __typeof(r) )q;
}
