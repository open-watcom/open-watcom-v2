/**
 * Test for a misplaced ')' in a unary-expression, specifically when the
 * expression contains an out-of-place token with a higher priority; and
 * that the sizeof operator works in usual and unusual cases.
 */
extern void x(unsigned long);

int main(void)
{
	char *b;
	unsigned long a;

	a += (unsigned long)sizeof a;
	a += (unsigned long)sizeof *b;
	a += (unsigned long)sizeof(unsigned int);
	a += (unsigned long)sizeof char;
	x(sizeof a);
	x(sizeof *b);
	x(sizeof(unsigned int));
	x(sizeof sizeof sizeof sizeof ++*b);
	x(sizeof char);
}

