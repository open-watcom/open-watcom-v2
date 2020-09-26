/**
 * Check that C95 predef. macros are defined
 */
#if !defined(__LINE__)
#error __LINE__ not defined with -std:c95
#endif

#if !defined(__FILE__)
#error __FILE__ not defined  -std:c95
#endif

#if !defined(__DATE__)
#error __DATE__ not defined with -std:c95
#endif

#if !defined(__TIME__)
#error __TIME__ not defined with -std:c95
#endif

#if !defined(__STDC__)
#error __STDC__ not defined with -std:c95
#endif

#if !defined(__STDC_VERSION__)
#error __STDC_VERSION__ not defined with -std:c95
#elif __STDC_VERSION__ != 199409L
#error __STDC_VERSION__ incorrectly defined with -std:c95
#endif

/**
 * Check for predef. macros that should NOT be defined in C95 mode
 */
#if defined(__STDC_HOSTED__)
#error __STDC_HOSTED__ defined with -std:c95
#endif

/**
 * __func__ and __FUNCTION__ would only be defined in function scope
 */
int f(void)
{
	const char *f = __func__;
	const char *g = __FUNCTION__;
	return f == g;
}

