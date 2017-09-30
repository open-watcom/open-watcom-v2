
typedef char *va_list[1];

#if 0
#define va_arg(ap,type)     ((ap)[0]+=\
    ((sizeof(type)+sizeof(int)-1)&~(sizeof(int)-1)),\
    (*(type *)((ap)[0]-((sizeof(type)+sizeof(int)-1)&~(sizeof(int)-1)))))
#else
#define va_arg(ap,type)     ((ap)[0]+=\
    (7&~(3)),\
    (*(int *)((ap)[0]-(7&~(3)))))
#endif
extern    va_list al;

void foo() {
    va_arg(al,int);
}

#error This should be the only error
