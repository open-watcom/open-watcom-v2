#if !defined(__PCPKT32_H) && (DOSX)
#define __PCPKT32_H

#define PM_DRVR_3C501   1  /* 3COM EtherLink I, a real museum piece! */
#define PM_DRVR_3C503   2
#define PM_DRVR_3C505   3  /* 3COM EtherLink II */
#define PM_DRVR_3C507   4
#define PM_DRVR_3C5x9   5  /* 3COM EtherLink III, ISA */
#define PM_DRVR_3C59x   6  /* 3COM EtherLink III, PCI */
#define PM_DRVR_NE2000  7  /* NE2000 compatible cards */
#define PM_DRVR_EEXPR   8  /* Intel EtherExpress */

typedef struct PM_driver {
        int         type;
        const char *name;
      } PM_driver;

extern int (*_pkt32_drvr)(IREGS*);

extern struct PM_driver pm_driver_list[];

extern int         pkt32_drvr_probe (const PM_driver *drivers);
extern int         pkt32_drvr_init  (int driver);
extern const char *pkt32_drvr_name  (int driver);

#endif /* !__PCPKT32_H && DOSX */

