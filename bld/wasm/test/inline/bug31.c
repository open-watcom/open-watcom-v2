#ifdef __cplusplus
extern "C" {
#endif


extern void jmp1( void );
#pragma aux jmp1 =         \
"        xor  ax, ax    "  \
"l1:                    "  \
"        db 126 dup(90h)"  \
"        jnz  l1        "  \
"a1:                    "  \
"        jnz  a1 - 2    "  \
"        jnz  l2        "  \
"        db 127 dup(90h)"  \
"l2:                    "  \
"        xor  ax, ax    "


extern void jmp2( void );
#pragma aux jmp2 =         \
"        xor  ax, ax    "  \
"l1:                    "  \
"        db 126 dup(90h)"  \
"a1:                    "  \
"        jnz  l1        "  \
"a2:                    "  \
"        jnz  l3        "  \
"l2:                    "  \
"        db 127 dup(90h)"  \
"l3:                    "  \
"        xor  ax, ax    "


void test (void)
{
    jmp1();
    jmp2();
}


#ifdef __cplusplus
}
#endif
