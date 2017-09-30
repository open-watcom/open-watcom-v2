/*
 * Definitions etc. for regexp(3) routines.
 *
 * Caveat:  this is V8 regexp(3) [actually, a reimplementation thereof],
 * not the System V one.
 */
#define NSUBEXP  10
typedef struct regexp {
        char *startp[NSUBEXP];
        char *endp[NSUBEXP];
        char regstart;          /* Internal use only. */
        char reganch;           /* Internal use only. */
        char *regmust;          /* Internal use only. */
        int regmlen;            /* Internal use only. */
        char program[1];        /* Unwarranted chumminess with compiler. */
} regexp;

/*
 * c++ headers added by Jim Morris.
 */
#ifdef  __cplusplus
extern "C"{
regexp *regcomp(const char *);
int regexec(regexp *, const char *);
}
#else
extern regexp *regcomp();
extern int regexec();
extern void regsub();
extern void regerror();
#endif
