#include "fail.h"

/* Incorrect handling of expressions with both int64 values and far pointers */

/* This test needs segment/offset relocations which cannot be used with
 * PE, ELF and other executable formats. 32-bit OS/2 and DOS (using LX/LE 
 * format) is fine.
 */

#if defined(__NT__) || defined(__UNIX__)

ALWAYS_PASS

#else

/* Must be not compiled in flat model, use small (-ms) */
#if defined( __386__ ) && !defined( __SMALL__ )
#error  Must use small model, not flat
#endif

signed   long long far __based(__segname("FARAWAY")) s_store1;
signed   long long far __based(__segname("FARAWAY")) s_store2;
unsigned long long far __based(__segname("FARAWAY")) u_store1;
unsigned long long far __based(__segname("FARAWAY")) u_store2;

/*
 * Ugly hack used to prevent access via DS. Do not know how to place data
 * in separate PHYSICAL segment in flat model
 */

void clear_ds(void);
void restore_ds(void);

#pragma aux clear_ds    = "push ds" "push 0" "pop ds";
#pragma aux restore_ds  = "pop  ds";

#define testdata(type, d1, d2, d3, d4)                                           \
signed   type far __based(__segname("FARAWAY")) s_##type = d1;                   \
unsigned type far __based(__segname("FARAWAY")) u_##type = d2;                   \
signed   type far __based(__segname("FARAWAY")) sneg_##type = d3;                \
unsigned type far __based(__segname("FARAWAY")) ubig_##type = d4;                \
signed   long long  s_expand_##type(signed   type far *p)  { return *p; }        \
unsigned long long  u_expand_##type(unsigned type far *p)  { return *p; }        \
void store_s_s_##type(signed   long long far *p, signed   type n) { *p = n; }    \
void store_s_u_##type(signed   long long far *p, unsigned type n) { *p = n; }    \
void store_u_s_##type(unsigned long long far *p, signed   type n) { *p = n; }    \
void store_u_u_##type(unsigned long long far *p, unsigned type n) { *p = n; }    \
int  verify_expand_##type(void)                                                  \
{                                                                                \
        int errors;                                                              \
        clear_ds();                                                              \
        errors  = s_expand_##type(&s_##type) != d1;                              \
        errors += u_expand_##type(&u_##type) != d2;                              \
        errors += s_expand_##type(&sneg_##type) != d3;                           \
        errors += u_expand_##type(&ubig_##type) != d4;                           \
        restore_ds();                                                            \
        return errors;                                                           \
}                                                                                \
int  verify_store_##type(void)                                                   \
{                                                                                \
        int errors = 0;                                                          \
                                                                                 \
        s_store1 = 0x8877665544332211;                                           \
        s_store2 = 0x9988776655443322;                                           \
        u_store1 = 0xFFEEDDCCAABB9988;                                           \
        u_store2 = 0xEEDDCCAABB998877;                                           \
                                                                                 \
        clear_ds();                                                              \
        store_s_s_##type(&s_store1, d1);                                         \
        store_s_u_##type(&s_store2, d2);                                         \
        store_u_s_##type(&u_store1, d1);                                         \
        store_u_u_##type(&u_store2, d2);                                         \
        restore_ds();                                                            \
                                                                                 \
        if (s_store1 != d1)  errors++;                                          \
        if (s_store2 != d2)  errors++;                                          \
        if (u_store1 != d1)  errors++;                                          \
        if (u_store2 != d2)  errors++;                                          \
                                                                                 \
        return errors;                                                           \
}

#pragma off(check_stack);   /* I zeroed DS, do not call __STK  */

testdata(char,   'C', 'D', -123, 0xFE)
testdata(short,  'S', 'T', -12345, 0xFEED)
#ifdef __386__
testdata(int,    'I', 'J', -1234567, 0xDEADBEEF)
#else
testdata(int,    'I', 'J', -23456, 0xFEEE)
#endif
testdata(long,   'L', 'M', -12345678, 0x0C0FFEE0)
testdata(__int64, 'I', 'J', -1, 0xDEADC0FFEEFEED)

#define expand(type)  if (verify_expand_##type()) _fail
#define store(type)   if (verify_store_##type())  _fail

int main(void)
{
	expand(char);
	expand(short);
	expand(int);
	expand(long);
	expand(__int64);

	store(char);
	store(short);
	store(int);
	store(long);
	store(__int64);

	_PASS;

	return 0;
}

#endif
