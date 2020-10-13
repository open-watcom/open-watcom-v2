/* Braces around scalar initializer */
int a = {{{{{{{{{{{{ 0, }}}}}}}}}}}};

/* Initializer index out of bounds */
int x[2] = { [3] = 1 };

/* Overridden Initializers */
union uu {
	char c;
	int i;
} uux = { .i = 0xcaca, .c = 0xfe };

struct fred
{
	char s [6];
	int n;
};

struct fred f1[] = { { { "abc" }, 1 }, [0].s[0] = 'q'        };
struct fred f2[] = { { { "abc" }, 1 }, [0] = { .s[0] = 'q' } };

/* Can't initialize fam without extensions enabled */
struct fam {
	int len;
	char fam[];
} f = { 6, { 'a', 'b', 'c', 'd', 'e', 'f', '\0' }};

/* Can't use anon. struct/unions without C11 or extensions */
struct xx {
	struct {
		int x;
	};
} zz;

struct yy {
	union {
		char c;
		int i;
		long l;
	};
} qq;

