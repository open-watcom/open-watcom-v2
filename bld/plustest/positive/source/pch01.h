#if defined(__AXP__)
inline int inc( int x, int y ) {
    _asm {
	ldl	$a0,x
	ldl	$a1,y
	addl	$a0,$a1,$a0
	stl	$a0,x
    }
    return x;
}
#elif defined(__386__) && __WATCOMC__ > 1060
inline int inc( int x, int y ) {
    _asm {
	mov	eax,x
	add	eax,y
	mov	x,eax
    }
    return x;
}
#elif defined(__I86__) && __WATCOMC__ > 1060
inline int inc( int x, int y ) {
    _asm {
	mov	ax,x
	add	ax,y
	mov	x,ax
    }
    return x;
}
#else
inline int inc( int x, int y ) {
    x += y;
    return x;
}
#endif
