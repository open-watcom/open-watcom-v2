
/**
 * IndexOp() was not consuming T_RIGHT_BRACKET when given a tree containing
 * an error node, thus causing the compiler to spin.
 */
int *invalid = &undefined[1][0][1];

/**
 * Statement() didn't check for T_ELSE, thus it spins looking for
 * T_SEMI_COLON, never advancing beyond T_ELSE.
 */
void bare_else(void)
{
	else ;
}

/**
 * If a union only contained a bit-field member, the size was never
 * being set on the type, and thus it was being seen as incomplete.
 */
union uzero {
	unsigned int f:12;
} not_zero = { 1 };

