struct S {
    int o00,o02,o04,o06;
    char o08;
    union {
	int i09;
	double d09;
	union {
	    char c09;
	    float f09;
	    char a09[23];
	};
    };
};

S *p;

void foo( void )
{
    p->o00 = 1;
    p->o02 = 2;
    p->o04 = 3;
    p->o06 = 4;
    p->o08 = 5;
    p->i09 = 6;
    p->d09 = 7;
    p->c09 = 8;
    p->f09 = 9;
    p->a09[0] = sizeof( *p );
}
