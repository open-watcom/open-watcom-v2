extern int           tcp_cbrk (int mode);
extern unsigned long set_ttimeout (unsigned ticks);
extern void          sock_sturdy (sock_type *s, int level);
#if defined(__DJGPP__)
extern time_t        dostounix (struct date *d, struct time *t);
extern void          unixtodos (time_t time, struct date *d, struct time *t);
#endif
