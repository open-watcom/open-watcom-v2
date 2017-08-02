// this should test bad tokens in a macro

#define TRTYPE(a,b) TRTYPE_ ## a ## b = ' ## a ## '
int i = TRTYPE( C, H );


