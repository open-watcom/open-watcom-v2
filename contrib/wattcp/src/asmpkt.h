#ifndef __ASMPKT_H
#define __ASMPKT_H

#if (DOSX == 0)                                  /* for real-mode targets   */
  extern void cdecl far pkt_receiver_rm(void);   /* in asmpkt.asm/asmpkt2.s */

#elif defined(__WATCOMC__) && (DOSX & DOS4GW) || /* in asmpkt4.asm */ \
      defined(__BORLAND386__) && (DOSX == WDOSX)

  extern WORD             asmpkt_size_chk;
  extern struct pkt_info *asmpkt_inf;

  extern void pkt_receiver4_start();
  extern void pkt_receiver4_rm();
  extern void pkt_receiver4_end();
#endif

#endif

