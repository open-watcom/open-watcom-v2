#ifndef __STRINGS_H
#define __STRINGS_H

extern void (*_outch)(char c);                /* point to current output handler */
extern int  (*_printf)(const char *fmt, ...); /* ditto, only inside O_DEBUG      */

extern void  outs    (const char *s);         /* print a ASCIIZ string to stdio  */
extern void  outsnl  (const char *s);         /* print a ASCIIZ string w/newline */
extern void  outsn   (const char *s, int n);  /* print a string with len max n   */
extern void  outhexes(const char *s, int n);  /* print a hex-string with len n   */
extern void  outhex  (char ch);               /* print a hex-char to stdio       */
extern char *rip     (char *s);               /* strip off '\r' and '\n' from s  */

#endif

