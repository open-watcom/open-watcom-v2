extern "C" int printf( char *, ... );
struct D {
    int d1, d2;
    D()
    {
	printf("D: %p\n",this);
	d1=0xff01;
	d2=0xff01;
    };
};
struct F {
    int f1, f2;
    virtual void foo( void );
    F()
    {
	printf("F: %p\n",this);
	f1=0xff02;
	f2=0xff02;
    };
};
void F::foo( void )
{
    f1=1;
}
struct V : D, F {
    int v1, v2;
    V()
    {
	printf("V: %p\n",this);
	v1=0xff03;
	v2=0xff03;
    };
};
struct X : virtual V {
    int x1, x2;
    virtual void foo( void );
    X()
    {
	printf("X: %p\n",this);
	x1=0xff04;
	x2=0xff04;
    };
};
void X::foo( void )
{
    x1=2;
}
struct E {
    int e1, e2;
    E()
    {
	printf("E: %p\n",this);
	e1=0xff05;
	e2=0xff05;
    };
};
struct H : E, X {
    int h1, h2;
    H()
    {
	printf("H: %p\n",this);
	h1=0xff06;
	h2=0xff06;
    };
};
struct O {
    int o1, o2;
    O()
    {
	printf("O: %p\n",this);
	o1=0xff07;
	o2=0xff07;
    };
};
struct I : O, virtual H {
    int i1, i2;
    virtual void bar(void);
    I()
    {
	printf("I: %p\n",this);
	i1=0xff08;
	i2=0xff08;
    };
};
void I::bar( void )
{
    i1=3;
}
struct P {
    int p1, p2;
    virtual void sam( void );
    P()
    {
	printf("P: %p\n",this);
	p1=0xff09;
	p2=0xff09;
    };
};
void P::sam( void )
{
    p1=4;
}
struct R : P, I {
    int r1, r2;
    R()
    {
	printf("R: %p\n",this);
	r1=0xff0a;
	r2=0xff0a;
	F *p = this;
	p->foo();
    };
};
/*
    Inheritance hierarchy:

			  D     F(foo)
			   \   /
			    \ /
	      O  E     X === V
	      |   \   /(foo)
	      |    \ /
	P     I === H
	 \   /
	  \ /
	   R

    Object layout:

	0	p1	P
	2	p2	P
	4	vfptr	P
	6	o1	O
	8	o2	O
	a	vbptr	I
	c	i1	I
	e	i2	I
	10	vfptr	I
	12	r1	R
	14	r2	R
	16	ctor-disp
	18	e1	E
	1a	e2	E
	1c	vbptr	X
	1e	x1	X
	20	x2	X
	22	vfptr	X
	24	h1	H
	26	h2	H
	28	ctor-disp
	2a	d1	D
	2c	d2	D
	2e	f1	F
	30	f2	F
	32	vfptr	F
	34	v1	V
	36	v2	V
*/

R x;
H *ph;
V *pv;

void main( void )
{
    ph = &x;
    pv = ph;
    x.foo();
    x.bar();
    x.sam();
    ph->foo();
    pv->foo();
}
