/**
 * In SymLeaf() if we have a symbol that happens to be a typedef,
 * SymLeaf() fails to grab the next token. Thus, upon return to
 * Statement(), the compiler gets stuck in an infinite loop hoping for
 * T_RIGHT_PAREN to magically transform into T_SEMI_COLON.
 */
extern void f(void *s);
extern void g(int i, void *s);
extern void x(void *s, int i);

typedef void * s;

int main(void)
{
	f(s);
	g(0, s);
	x(s, 0);
}

