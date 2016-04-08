/*
 * If compiler/linker doesn't see our defines for htonl() etc.
 */
#undef htonl
#undef ntohl
#undef htons
#undef ntohs
#undef inchksum
#undef inchksum_fast

#undef intel
#undef intel16

extern WORD  _getshort  (const BYTE *x);
extern DWORD _getlong   (const BYTE *x);
extern void  __putshort (WORD var, BYTE *ptr);
extern void  __putlong  (DWORD var, BYTE *ptr);

extern DWORD htonl    (DWORD val);
extern DWORD ntohl    (DWORD val);
extern WORD  htons    (WORD  val);
extern WORD  ntohs    (WORD  val);
extern WORD  inchksum (const void *ptr, int len);
extern DWORD intel    (DWORD val);
extern WORD  intel16  (WORD  val);
