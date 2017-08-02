struct S {} foo();

__segment s;
void __based(s) *p;
void __based(p) *q;

void __based(void) *balloc( unsigned );
void __based(__self) *balloc_self( unsigned );

void foo( int __based(s) & x );

template <class T,class T,int *p,int *p,int s,int s>
    struct Stack {
	Stack()
	{
	}
    };

extern void __based(__segname("AAA")) ack19( void );
extern void __based(__segname("BBB")) ack19( void );

extern int __based(__segname("AAA")) ack22;
extern int __based(__segname("BBB")) ack22;
