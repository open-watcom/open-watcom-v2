#ifndef __ASMPKT_H
#define __ASMPKT_H

#if (DOSX == 0)

/* for real-mode targets   */
/* in asmpkt16.asm */
extern void __far pkt_receiver_rm( void );
#pragma aux pkt_receiver_rm "_*"

#elif (DOSX & DOS4GW) && defined(__386__)

/* for prot-mode targets   */
/* in asmpkt32.asm */
extern WORD             asmpkt_size_chk;
#pragma aux asmpkt_size_chk "*"
extern struct pkt_info *asmpkt_inf;
#pragma aux asmpkt_inf "*"

extern char pkt_receiver32_start;
#pragma aux pkt_receiver32_start "*"
extern void pkt_receiver32_rm( void );
#pragma aux pkt_receiver32_rm "*"
extern char pkt_receiver32_end;
#pragma aux pkt_receiver32_end "*"
#endif

#endif

