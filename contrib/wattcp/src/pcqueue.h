#ifndef __PC_QUEUE_H
#define __PC_QUEUE_H

/*
 * asmpkt4.asm depends on '_pkt_inf' beeing packed
 */
#if (DOSX & DOS4GW)
#include <sys/packon.h>
#endif

struct pkt_ringbuf {
       volatile int   in_index;   /* queue index head */
       int            out_index;  /* queue index tail */
       int            buf_size;   /* size of each buffer */
       int            num_buf;    /* number of buffers */
       volatile DWORD num_drop;   /* number of dropped pkts */
       char          *buf_start;  /* start of buffer pool (linear addr)*/
#if (DOSX & DOS4GW)
       WORD           dos_ofs;    /* offset of pool, used by rmode stub */
#endif                            /* total size = 26 for DOS4GW/WDOSX */
     };            

#if (DOSX & DOS4GW)
#include <sys/packoff.h>
#endif

extern int   pktq_init     (struct pkt_ringbuf *q, int size, int num, char *buf);
extern int   pktq_check    (struct pkt_ringbuf *q);
extern int   pktq_inc_out  (struct pkt_ringbuf *q);
extern int   pktq_in_index (struct pkt_ringbuf *q);
extern char *pktq_in_buf   (struct pkt_ringbuf *q);
extern char *pktq_out_buf  (struct pkt_ringbuf *q);
extern void  pktq_clear    (struct pkt_ringbuf *q);
extern int   pktq_queued   (struct pkt_ringbuf *q);

#endif
