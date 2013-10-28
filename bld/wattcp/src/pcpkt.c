/*
 *  Packet Driver interface for WatTCP/Watt-32
 *
 *  Heavily modified and extended for DOSX by
 *  G.Vanem <giva@bgnett.no>
 */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>
#include <dos.h>

#include "copyrigh.h"
#include "wattcp.h"
#include "wdpmi.h"
#include "asmpkt.h"
#include "strings.h"
#include "language.h"
#include "misc.h"
#include "profile.h"
#include "pcsed.h"
#include "pcstat.h"
#include "pcconfig.h"
#include "pcmulti.h"
#include "pcqueue.h"

#define DEFINE_IREGS
#include "pcpkt.h"
#include "pcpkt32.h"

/*@-usedef@*/

WORD _pktdevclass = PD_ETHER;     /* Ethernet, Token, FDDI etc.       */
WORD _pkt_ip_ofs  = 0;            /* ofs from link-layer head to ip   */
BYTE _pktserial   = FALSE;        /* using serial driver, SLIP/PPP    */
BYTE _pktdevlevel = 1;            /* basic unless otherwise specified */
BYTE _pkterror    = 0;            /* error code set in pcpkt.c API    */
BYTE _pktretries  = 2;            /* # of retries in pkt_send loop    */

struct pkt_info *_pkt_inf = NULL; /* module data that will be locked. */

static BYTE eth_addr[6]   = { 0,0,0,0,0,0 };
static char pkt_sign[8]   = "PKT DRVR";
static WORD pkt_interrupt = 0;

#if (DOSX & PHARLAP)
  #include <mw/exc.h>
  #ifndef __FLAT__
  #include <pldos32.h>
  #endif
  static REALPTR rm_base;

  /*
   * real-data is organised as follows: PKT_TMP at rm_base + 0x40
   *                                    TX_BUF  at PKT_TMP + 30
   *                                    RX_BUF  at TX_BUF + IP_SIZE
   */
  #define PKT_TMP()   (WORD)0x40
  #define TX_BUF()    (PKT_TMP() + 30)
  #define RX_BUF()    (TX_BUF() + IP_SIZE)
  #define RDATA_SIZE  (RX_BUF() + IP_SIZE)

#elif (DOSX & DJGPP)
  static _go32_dpmi_seginfo rm_cb;
  static __dpmi_regs  rm_reg;
  static DWORD        rm_base;

  /*
   * real-data is organised as follows: PKT_TMP at rm_base + 0
   *                                    TX_BUF  at PKT_TMP + 30
   *                                    RX_BUF  at TX_BUF  + IP_SIZE
   */
  #define PKT_TMP()     0
  #define TX_BUF()      (PKT_TMP() + 30)
  #define RX_BUF()      (TX_BUF() + IP_SIZE)
  #define RDATA_SIZE    (RX_BUF() + IP_SIZE)
  #define RP_SEG()      _pkt_inf->rm_mem.rm_segment

#elif (DOSX & DOS4GW)         /* All DOS4GW type extenders (not WDOSX) */

  static BYTE *rm_base;       /* Linear address (in DOS) for allocated area */
  static WORD  rm_base_seg;   /* DOS-segment for allocated area */
  static WORD  pkt_inf_sel;   /* selector returned from `_pkt_inf' allocation */
  static WORD  pkt_rcv_sel;   /* ditto, from `rm_base' allocation */

  /*
   * The DOS-area (0-1MB) is automagically mapped into application
   * data.  This eases communication with packet-driver, but clean
   * crashes can not be expected. i.e. bugs (dereferencing null-ptr)
   * in application will most likely hang the machine.
   *
   * Real-mode code/data is organised like this:
   *   pkt_receiver4_start copied to allocated area at rm_base
   *   PKT_TMP   at rm_base + (pkt_receiver4_end - pkt_receiver4_start)
   *   TX_BUF    at PKT_TMP + 30
   *   end area  at TX_BUF + IP_SIZE
   *   RX_BUF is in DOS-allocated `_pkt_inf' structure.
   */
  #define PKT_TMP()     ((DWORD)&pkt_receiver4_end -  \
                         (DWORD)&pkt_receiver4_start)
  #define TX_BUF()      (PKT_TMP() + 30)
  #define RCV_OFS()     ((DWORD)&pkt_receiver4_rm - \
                         (DWORD)&pkt_receiver4_start)
  #define RP_SEG()      rm_base_seg

#elif (DOSX & WDOSX)
  static struct DPMI_callback rm_cb;
  static BYTE                *rm_base;
  static WORD                 rm_base_seg;
  static WORD                 rm_base_sel;

  /*
   * real-data is organised as follows: PKT_TMP at rm_base + 0
   *                                    TX_BUF  at PKT_TMP + 30
   *                                    RX_BUF  at TX_BUF  + IP_SIZE
   */
  #define PKT_TMP()     0
  #define TX_BUF()      (PKT_TMP() + 30)
  #define RX_BUF()      (TX_BUF() + IP_SIZE)
  #define RDATA_SIZE    (RX_BUF() + IP_SIZE)
  #define RP_SEG()      rm_base_seg
  #define RCV_OFS()     rm_cb.cb_offset

#elif (DOSX & POWERPAK)    /* to-do !! */
  #define PKT_TMP()     0  /* for now */
  #define TX_BUF()      (PKT_TMP() + 30)
  #define RX_BUF()      (TX_BUF() + IP_SIZE)
  #define RDATA_SIZE    (RX_BUF() + IP_SIZE)
  #define RP_SEG()      0

#else  /* r-mode targets */

  void (_cdecl _far * pkt_enque_ptr) (BYTE _far *buf, WORD len, WORD handle);
  void (_cdecl _far *_pkt_enque_ptr) (BYTE _far *buf, WORD len, WORD handle);

  #ifdef _MSC_VER       /* 16-bit Microsoft C compilers (v6+) */
    #undef  FP_SEG
    #undef  FP_OFF
  //#define FP_SEG(p) (WORD)((DWORD)(p) >> 16)
  //#define FP_OFF(p) (WORD)((DWORD)(p) & 0xFFFF)
    #define FP_SEG(p) ((unsigned)(_segment)(void _far *)(p))
    #define FP_OFF(p) ((unsigned)(p))
  #endif
#endif

#if (DOSX)
  static int setup_rmode_callback (void);
  static int lock_code_and_data   (void);
  static int unlock_code_and_data (void);
#endif
        
static int  release_handles (BOOL quiet);
static WORD find_vector (int first, int num);
static int  setup_pkt_inf (void);
static int  pkt_api_entry (IREGS *reg, unsigned called_from_line);

#define PKT_API(reg)     pkt_api_entry (reg, __LINE__)
#define CARRY_BIT        1   /* carry bit in flags register */

#define PKT_ERR(str,dx)  do {                                              \
                           const char *s = pkt_errStr ((BYTE)((dx) >> 8)); \
                           outs (_LANG(str));                              \
                           outsnl (s);                                     \
                         } while (0)


/*
 * Return textual error representing error-code.
 */
static const char * pkt_errStr (BYTE code)
{
  static char  buf[50];
  static const char *err[] = {
              __LANG("Unknown driver error \0(0x--)"),
              __LANG("Invalid handle number"),
              __LANG("No interfaces of specified class found"),
              __LANG("No interfaces of specified type found"),
              __LANG("No interfaces of specified number found"),
              __LANG("Bad packet type specified"),
              __LANG("This interface does not support multicast"),
              __LANG("This packet driver cannot terminate"),
              __LANG("An invalid receiver mode was specified"),
              __LANG("Operation failed because of insufficient space"),
              __LANG("Type previously accessed, and not released"),
              __LANG("The command was out of range, or not implemented"),
              __LANG("Cannot send packet (hardware error)"),
              __LANG("Cannot change hardware address"),
              __LANG("Hardware address has bad length/format"),
              __LANG("Could not reset interface"),
              __LANG("Extended driver needed")
            };
  char *p;

  _pkterror = code;
  if (code && code < DIM(err))
     return (_LANG (err[code]));

  p = strchr (err[0], 0);
  *(p+4) = hex_CHARS [code >> 4];
  *(p+5) = hex_CHARS [code & 15];
  strcpy (buf, _LANG (err[0]));
  return strcat (buf, p+1);
}


#if (DOSX & DOS4GW) && 0     /* test asmpkt4.asm !! */
static void dump_asm4 (void)
{
  BYTE *p = rm_base;
  int   i;

  printf ("PKT_INF %08lX, HEAD %08lX, QUEUE %08lX, INDEX %04X, XYPOS %d\n",
          *(DWORD*)p, *(DWORD*)(p+4), *(DWORD*)(p+8), *(WORD*)(p+12),
          *(WORD*)(p+14));

  printf ("rm_base %08lX, ip_handle %04X, arp_handle %04X, is_serial %d\n",
          rm_base, _pkt_inf->ip_handle, _pkt_inf->arp_handle,
          _pkt_inf->is_serial);

  printf ("VAR_1-14: ");
  for (i = 16; i <= 42; i += 2)
     printf ("%04X ", *(WORD*)(p+i));

#if 0
  printf ("PKT_RECV: ");
  for (i = RCV_OFS(); i < RCV_OFS()+10; i++)
     printf ("%02X ", p[i]);

  printf ("\nPKT_TMP:  ");
  for (i = PKT_TMP(); i < PKT_TMP()+30; i++)
     printf ("%02X ", p[i]);
#endif
  fflush (stdout);
}
#endif


/*
 * Return array specifying what MAC framing to use for all supported
 * protocols (IP/ARP/RARP). 'len' is size of one array element (2, 4 or 8).
 */
static BYTE *get_access_types (int *len, int *num)
{
  static BYTE eth_types [5*2];
  static BYTE llc_types [3*8] = { TR_DSAP, TR_SSAP, TR_CTRL, 0,0,0,0,0,
                                  TR_DSAP, TR_SSAP, TR_CTRL, 0,0,0,0,0,
                                  TR_DSAP, TR_SSAP, TR_CTRL, 0,0,0,0,0
                                };
  if (_pktserial)  /* Serial driver don't need this */
  {
    *len = 0;
    *num = 0;
    return (NULL);
  }
  if (_pktdevclass == PD_TOKEN ||
      _pktdevclass == PD_FDDI)     /* correct? */
  {
    *(WORD*) (llc_types+ 6) = IP_TYPE;
    *(WORD*) (llc_types+12) = ARP_TYPE;
    *(WORD*) (llc_types+18) = RARP_TYPE;
    *len = 8;
    *num = 3;
    return (llc_types);
  }

  /* PD_ETHER
   */
  *(WORD*) (eth_types+0) = IP_TYPE;
  *(WORD*) (eth_types+2) = ARP_TYPE;
  *(WORD*) (eth_types+4) = RARP_TYPE;
  *(WORD*) (eth_types+6) = PPPOE_DISC_TYPE;
  *(WORD*) (eth_types+8) = PPPOE_SESS_TYPE;
  *num = 5;
  *len = 2;
  return (eth_types);
}

/**************************************************************************/

static int pkt_set_access (void)
{
  IREGS regs, regs2, regs3, regs4, regs5;

  BYTE *types;
  int   tlen = 0;
  int   num_types;

  ASSERT_PKT_INF (0);

  memset (&regs, 0, sizeof(regs));

  types = get_access_types (&tlen, &num_types);

  regs.r_ax  = PD_ACCESS | _pktdevclass;
  regs.r_bx  = 0xFFFF;                   /* any type          */
  regs.r_dx  = 0;                        /* generic interface */
  regs.r_cx  = tlen;

#if (DOSX & PHARLAP)
  if (types)
     WriteRealMem (rm_base + PKT_TMP(), types, num_types*tlen);

  /* if CX is 0, there should be no harm setting DS:DI
   */
  regs.r_ds = RP_SEG (rm_base);
  regs.r_si = PKT_TMP();
  regs.r_es = RP_SEG (rm_base);          
  regs.r_di = 0;                         /* RMCB aligned at para address */
  memcpy (&regs2, &regs, sizeof(regs2)); /* make copy for ARP type */
  regs2.r_si += tlen;
  memcpy (&regs3, &regs, sizeof(regs3)); /* make copy for RARP type */
  regs3.r_si += sizeof(WORD)*tlen;
  memcpy (&regs4, &regs, sizeof(regs4)); /* make copy for PPPoE type */
  regs4.r_si += sizeof(WORD)*tlen;
  memcpy (&regs5, &regs, sizeof(regs5)); /* make copy for PPPoE type */
  regs5.r_si += sizeof(WORD)*tlen;

#elif (DOSX & DJGPP)
  if (tlen)
     dosmemput (types, num_types*tlen, rm_base + PKT_TMP());

  regs.r_ds = RP_SEG();
  regs.r_si = PKT_TMP();
  regs.r_es = rm_cb.rm_segment;
  regs.r_di = rm_cb.rm_offset;
  memcpy (&regs2, &regs, sizeof(regs2));
  regs2.r_si += tlen;
  memcpy (&regs3, &regs, sizeof(regs3));
  regs3.r_si += sizeof(WORD)*tlen;
  memcpy (&regs4, &regs, sizeof(regs4));
  regs4.r_si += sizeof(WORD)*tlen;
  memcpy (&regs5, &regs, sizeof(regs5));
  regs5.r_si += sizeof(WORD)*tlen;

#elif (DOSX & DOS4GW)
  if (tlen)
     memcpy ((void*)(rm_base + PKT_TMP()), types, num_types*tlen);

  regs.r_ds = RP_SEG();
  regs.r_si = PKT_TMP();
  regs.r_es = RP_SEG();
  regs.r_di = RCV_OFS();
  memcpy (&regs2, &regs, sizeof(regs2));
  regs2.r_si += tlen;
  memcpy (&regs3, &regs, sizeof(regs3));
  regs3.r_si += sizeof(WORD)*tlen;
  memcpy (&regs4, &regs, sizeof(regs4));
  regs4.r_si += sizeof(WORD)*tlen;
  memcpy (&regs5, &regs, sizeof(regs5));
  regs5.r_si += sizeof(WORD)*tlen;

#elif (DOSX & WDOSX)
  if (tlen)
     memcpy ((void*)(rm_base + PKT_TMP()), types, num_types*tlen);

  regs.r_ds = RP_SEG();
  regs.r_si = PKT_TMP();
  regs.r_es = rm_cb.cb_segment;
  regs.r_di = rm_cb.cb_offset;
  memcpy (&regs2, &regs, sizeof(regs2));
  regs2.r_si += tlen;
  memcpy (&regs3, &regs, sizeof(regs3));
  regs3.r_si += sizeof(WORD)*tlen;
  memcpy (&regs4, &regs, sizeof(regs4));
  regs4.r_si += sizeof(WORD)*tlen;
  memcpy (&regs5, &regs, sizeof(regs5));
  regs5.r_si += sizeof(WORD)*tlen;

#elif (DOSX & POWERPAK)
  UNFINISHED();

#else /* real-mode targets */
  if (types)
  {
    regs.r_ds = FP_SEG (&types[0]);       /* = global DS */
    regs.r_si = FP_OFF (&types[0]);
  }
  regs.r_es = FP_SEG (pkt_receiver_rm);   /* = this CS */
  regs.r_di = FP_OFF (pkt_receiver_rm);

  memcpy (&regs2, &regs, sizeof(regs2));
  regs2.r_si += tlen;
  memcpy (&regs3, &regs, sizeof(regs3));
  regs3.r_si += sizeof(WORD)*tlen;
  memcpy (&regs4, &regs, sizeof(regs4));
  regs4.r_si += sizeof(WORD)*tlen;
  memcpy (&regs5, &regs, sizeof(regs5));
  regs5.r_si += sizeof(WORD)*tlen;
#endif

  if (!PKT_API(&regs))
  {
    PKT_ERR ("Error allocating IP handle: ", regs.r_dx);
    return (0);
  }
  _pkt_inf->ip_handle = regs.r_ax;

  if (_pktserial)
     return (1);

  if (!PKT_API(&regs2))
  {
    PKT_ERR ("Error allocating ARP handle: ", regs2.r_dx);
    return (0);
  }
  _pkt_inf->arp_handle = regs2.r_ax;

#if defined(USE_RARP)
  if (!PKT_API(&regs3))
  {
    PKT_ERR ("Error allocating RARP handle: ", regs3.r_dx);
    return (0);
  }
  _pkt_inf->rarp_handle = regs3.r_ax;
#endif

#if defined(USE_PPPOE)
  if (!PKT_API(&regs4))
  {
    PKT_ERR ("Error allocating PPPoE discovery handle: ", regs4.r_dx);
    return (0);
  }
  _pkt_inf->pppoe_disc_handle = regs4.r_ax;

  if (!PKT_API(&regs5))
  {
    PKT_ERR ("Error allocating PPPoE session handle: ", regs5.r_dx);
    return (0);
  }
  _pkt_inf->pppoe_sess_handle = regs5.r_ax;
#endif

  return (1);
}

/**************************************************************************/

static int pkt_drvr_info (void)
{
  IREGS regs;

  ASSERT_PKT_INF (0);

  /* Lets find out about the driver
   */
  regs.r_ax = PD_DRIVER_INFO;  

  /* Handle old versions, assume a class and just keep trying
   * This method will fail for Token-Ring
   */
  if (!PKT_API(&regs))
  {
    int class;
  
    for (class = 0; class < 2; class++)   /* test for SLIP/Ether */
    {
      static WORD ip_type = IP_TYPE;
      int    max_type = 128;
      int    type;

      _pktdevclass = (class ? PD_SLIP : PD_ETHER);

      for (type = 1; type < max_type; type++)
      {
        regs.r_ax = PD_ACCESS | _pktdevclass;  /* ETH, SLIP */
        regs.r_bx = type;                      /* pkt type  */
        regs.r_dx = 0;                         /* iface num */
        regs.r_cx = class ? sizeof(ip_type) : 0;
#if (DOSX & PHARLAP)
        PokeRealWord (rm_base + PKT_TMP(), ip_type);
        regs.r_ds = RP_SEG (rm_base);
        regs.r_si = PKT_TMP();
        regs.r_es = RP_SEG (rm_base);
        regs.r_di = 0;
#elif (DOSX & DJGPP)
        _farpokew (_dos_ds, rm_base + PKT_TMP(), ip_type);
        regs.r_ds = RP_SEG();
        regs.r_si = PKT_TMP();
        regs.r_es = rm_cb.rm_segment;
        regs.r_di = rm_cb.rm_offset;
#elif (DOSX & DOS4GW)
        *(WORD*) (rm_base + PKT_TMP()) = ip_type;
        regs.r_ds = RP_SEG();
        regs.r_si = PKT_TMP();
        regs.r_es = RP_SEG();
        regs.r_di = RCV_OFS();
#elif (DOSX & WDOSX)
        *(WORD*) (rm_base + PKT_TMP()) = ip_type;
        regs.r_ds = RP_SEG();
        regs.r_si = PKT_TMP();
        regs.r_es = rm_cb.cb_segment;
        regs.r_di = rm_cb.cb_offset;
#elif (DOSX & POWERPAK)
        UNFINISHED();
#else
        regs.r_ds = FP_SEG (&ip_type);
        regs.r_si = FP_OFF (&ip_type);
        regs.r_es = FP_SEG (pkt_receiver_rm);
        regs.r_di = FP_OFF (pkt_receiver_rm);
#endif
        if (PKT_API(&regs))
           break;
      }

      if (type == max_type)
      {
        outsnl (_LANG("Error initializing packet driver"));
        return (0);
      }
      /* we have found a working type, so kill it
       */
      regs.r_bx = regs.r_ax;      /* handle */
      regs.r_ax = PD_RELEASE;
      PKT_API (&regs);
    }
  }
  else          /* new Packet-Driver (1.09+) */
  {
    _pktdevlevel = (regs.r_ax & 0xFF);
    _pktdevclass = (BYTE)(regs.r_cx >> 8);

    switch (_pktdevclass)
    {
      case PD_TOKEN:
           _pkt_ip_ofs = sizeof(tok_Header);
           break;

      case PD_ETHER:
           _pkt_ip_ofs = sizeof(eth_Header);
           break;

      case PD_FDDI:
           _pkt_ip_ofs = sizeof(fddi_Header);
           break;

      case PD_SLIP:
      case PD_PPP:
           _pkt_ip_ofs = 0;
           break;

      default:
           outs (_LANG("ERROR: Unsupported driver class "));
           outhex ((char)_pktdevclass);
           outsnl ("");
           return (0);
    }
  }
  _pktserial = (_pktdevclass == PD_SLIP ||
                _pktdevclass == PD_PPP);
  _pkt_inf->is_serial = _pktserial;
  return (1);
}


/**************************************************************************/

#include <sys/packon.h>

static struct {
       BYTE  major_rev;      /* Revision of Packet Driver spec */
       BYTE  minor_rev;      /*  this driver conforms to. */
       BYTE  length;         /* Length of structure in bytes */
       BYTE  addr_len;       /* Length of a MAC-layer address */
       WORD  mtu;            /* MTU, including MAC headers */
       WORD  multicast_aval; /* Buffer size for multicast addr */
       WORD  rcv_bufs;       /* (# of back-to-back MTU rcvs) - 1 */
       WORD  xmt_bufs;       /* (# of successive xmits) - 1 */
       WORD  int_num;        /* interrupt for post-EOI processing */
     } pkt_params;

#include <sys/packoff.h>

static BOOL got_params = FALSE;

static int pkt_get_params (void)
{
  IREGS regs;

  regs.r_ax = PD_GET_PARAM;  /* get driver parameters */
  if (!PKT_API(&regs))
     return (0);

#if (DOSX & PHARLAP)
  {
    REALPTR rp;
    RP_SET (rp, (WORD)regs.r_di, regs.r_es);
    ReadRealMem (&pkt_params, rp, sizeof(pkt_params));
  }

#elif (DOSX & DJGPP)
  dosmemget (regs.r_di + (regs.r_es << 4), sizeof(pkt_params), &pkt_params);

#elif (DOSX & (DOS4GW|WDOSX))
  memcpy (&pkt_params, SEG_OFS_TO_LIN(regs.r_es,regs.r_di), sizeof(pkt_params));

#elif (DOSX & POWERPAK)
  UNFINISHED();

#else
  _fmemcpy (&pkt_params, MK_FP(regs.r_es,regs.r_di), sizeof(pkt_params));
#endif

  got_params = TRUE;
  return (1);
}

int pkt_get_mtu (void)
{
  if (got_params)
     return (pkt_params.mtu);
  return (-1);
}

int pkt_get_mac_len (void)
{
  if (got_params)
     return (pkt_params.addr_len);
  return (-1);
}

int pkt_get_drvr_ver (void)
{
  if (got_params)
     return ((pkt_params.major_rev << 8) + pkt_params.minor_rev);
  return (-1);
}

/**************************************************************************/


#if (DOSX == 0) && defined(USE_DEBUG)
/*
 *  Some paranoia checks for real-targets; if our IREGS structure
 *  doesn't match REGPACK of the C-lib, intr() will probably cause a
 *  crash. Better safe than sorry..
 */
static int check_reg_struct (void)
{
#define OffsetOf(x) (unsigned)&(x)

#if defined(_MSC_VER)  /* We made our own intr(), hence no need to do this */
  return (1);

#elif defined(__WATCOMC__)
  union  REGPACK *r1 = NULL;
  struct IREGS   *r2 = NULL;

  if ((OffsetOf(r1->w.ax)    != OffsetOf(r2->r_ax)) ||
      (OffsetOf(r1->w.bx)    != OffsetOf(r2->r_bx)) ||
      (OffsetOf(r1->w.cx)    != OffsetOf(r2->r_cx)) ||
      (OffsetOf(r1->w.dx)    != OffsetOf(r2->r_dx)) ||
      (OffsetOf(r1->w.bp)    != OffsetOf(r2->r_bp)) ||
      (OffsetOf(r1->w.si)    != OffsetOf(r2->r_si)) ||
      (OffsetOf(r1->w.di)    != OffsetOf(r2->r_di)) ||
      (OffsetOf(r1->w.ds)    != OffsetOf(r2->r_ds)) ||
      (OffsetOf(r1->w.es)    != OffsetOf(r2->r_es)) ||
      (OffsetOf(r1->x.flags) != OffsetOf(r2->r_flags)))
    return (0);

#else  /* Borland */
  struct REGPACK *r1 = NULL;
  struct IREGS   *r2 = NULL;

  if ((OffsetOf(r1->r_ax)    != OffsetOf(r2->r_ax)) ||
      (OffsetOf(r1->r_bx)    != OffsetOf(r2->r_bx)) ||
      (OffsetOf(r1->r_cx)    != OffsetOf(r2->r_cx)) ||
      (OffsetOf(r1->r_dx)    != OffsetOf(r2->r_dx)) ||
      (OffsetOf(r1->r_bp)    != OffsetOf(r2->r_bp)) ||
      (OffsetOf(r1->r_si)    != OffsetOf(r2->r_si)) ||
      (OffsetOf(r1->r_di)    != OffsetOf(r2->r_di)) ||
      (OffsetOf(r1->r_ds)    != OffsetOf(r2->r_ds)) ||
      (OffsetOf(r1->r_es)    != OffsetOf(r2->r_es)) ||
      (OffsetOf(r1->r_flags) != OffsetOf(r2->r_flags)))
    return (0);
#endif

  return (1);
}
#endif /* (DOSX == 0) && USE_DEBUG */

/*
 * pkt_init - Called from pkt_eth_init() to search for PKT-DRVR.
 *          - Allocates '_pkt_inf' structure.
 */
static int pkt_init (void)
{
#if (DOSX)
  int rc;
#endif

#if (DOSX == 0) && defined(USE_DEBUG)
  if (!check_reg_struct())
  {
    outsnl (__FILE__ ": IREGS/REGPACK size mismatch!");
    return (0);
  }
#endif

  /* If interrupt specified in environment ("WATTCP.VEC=0xNN" or
   * "TCP_PKTINT=0xNN") check a single vector. Else, search for the 1st
   * driver in range 0x60-0x80.
   */
  if (pkt_interrupt)
       pkt_interrupt = find_vector (pkt_interrupt, 1);
  else pkt_interrupt = find_vector (PKT_FIRST_VEC,
                                    PKT_LAST_VEC - PKT_FIRST_VEC + 1);

  if (pkt_interrupt == 0)
  {
    outsnl (_LANG("NO PACKET DRIVER FOUND."));
    return (0);
  }

  if (!setup_pkt_inf())
  {
    outsnl (_LANG("Failed to allocate PACKET DRIVER data."));
    return (0);
  }

#if (DOSX)
  if ((rc = setup_rmode_callback()) < 0)
  {
    if (rc == -1 || rc == -2)
       outsnl (rc == -1 ?
               _LANG("Failed to allocate callback for PACKET DRIVER.") :
               _LANG("Failed to allocate DOS-mem for PACKET DRIVER."));
    return (0);
  }
  if (lock_code_and_data() < 0)
  {
    outsnl (_LANG("Failed to lock code/data for PACKET DRIVER."));
    return (0);
  }
#endif

  if (!pkt_drvr_info())         /* get device class etc. of driver */
     return (0);

  if (!pkt_set_access())        /* get handles for IP, ARP (and RARP, PPPoE) */
  {
    release_handles (TRUE);
    return (0);
  }

  if (!pkt_get_addr(&eth_addr)) /* get our MAC address */
  {
    release_handles (TRUE);
    return (0);
  }

  if (_pktdevlevel >= 2 && _pktdevlevel < 255)
     pkt_get_params();

#if defined(USE_MULTICAST)
  /* set receive mode to limited multicast
   */
  if (_multicast_on)
     _pkt_set_rcv_mode (_pkt_inf->ip_handle, RM_MULTICAST1);
#endif

  return (1);
}

/*
 *  The following functions (ending at '_pkt_end()') are called
 *  at interrupt time (or asynchronously). 'pkt_release()' may be
 *  called from SIGSEGV handler. Therefore don't assume anything
 *  about the state of our stack (except hope it's large enough).
 *  And don't use large local variables here. Hope we will not be
 *  reentered since not all functions below are reentrant.
 */

/* Disable stack-checking here
 */
#if defined(__HIGHC__)
#pragma off(check_stack)
#pragma off(call_trace)
#pragma off(prolog_trace)
#pragma off(epilog_trace)
#endif

#if defined(__WATCOMC__)
#pragma off(check_stack)
#endif

#if defined(__WATCOM386__)
//#pragma option -zu                    /* assume SS != DS (doesn't work) */
//#pragma aux pkt_release __modify[ss]; /* !! fix-me (doesn't work) */
#endif

#if (defined(__TURBOC__) || defined(__BORLANDC__)) && !defined(OLD_TURBOC)
#pragma option -N-
#endif

/*
 * Release all allocated protocol handles
 */
static int release_handles (BOOL quiet)
{
  static IREGS regs;  /* `static' because the stack could be too small */

  ASSERT_PKT_INF (0);

  /* to-do!!: change to local stack just in case (use longjmp?)
   */

  if (!_pktserial)
  {
    regs.r_ax = PD_RELEASE;
    regs.r_bx = _pkt_inf->arp_handle;
    if (!PKT_API(&regs) && !quiet)
       PKT_ERR ("Error releasing ARP handle: ", regs.r_dx);

#if defined(USE_RARP)
    regs.r_ax = PD_RELEASE;
    regs.r_bx = _pkt_inf->rarp_handle;
    if (!PKT_API(&regs) && !quiet)
       PKT_ERR ("Error releasing RARP handle: ", regs.r_dx);
#endif

#if defined(USE_PPPOE)
    regs.r_ax = PD_RELEASE;
    regs.r_bx = _pkt_inf->pppoe_sess_handle;
    if (!PKT_API(&regs) && !quiet)
       PKT_ERR ("Error releasing PPPOE session handle: ", regs.r_dx);

    regs.r_ax = PD_RELEASE;
    regs.r_bx = _pkt_inf->pppoe_disc_handle;
    if (!PKT_API(&regs) && !quiet)
       PKT_ERR ("Error releasing PPPOE discovery handle: ", regs.r_dx);
#endif
  }

  regs.r_ax = PD_RELEASE;
  regs.r_bx = _pkt_inf->ip_handle;

  if (!PKT_API(&regs) && !quiet)
     PKT_ERR ("Error releasing IP handle: ", regs.r_dx);
  return (1);
}

/*
 *  Release the pkt-driver.
 *  Might be called from exception/signal handler.
 */
int pkt_release (void)
{
  release_handles (FALSE);

  pkt_interrupt = 0;
  DISABLE();         /* no upcalls now */

  /*
   * to-do !!: We might be called between 1st and 2nd packet-driver
   *           upcall. Need to wait for 2nd upcall to finish or else
   *           freeing the RMCB too early could cause a crash.
   */

#if (DOSX & PHARLAP)
  _dx_free_rmode_wrapper (rm_base);

#elif (DOSX & DJGPP)
  _go32_dpmi_free_real_mode_callback (&rm_cb);
  __dpmi_error = 0;
  _go32_dpmi_free_dos_memory (&_pkt_inf->rm_mem);
  if (__dpmi_error)                /* !!above free function clears eax */
    fprintf (stderr, "%s (%u): DPMI/DOS error %04Xh\n",
             __FILE__, __LINE__, __dpmi_error);

#elif (DOSX & DOS4GW)
  if (pkt_rcv_sel) dpmi_real_free (pkt_rcv_sel);
  if (pkt_inf_sel) dpmi_real_free (pkt_inf_sel);
  pkt_rcv_sel = 0;
  pkt_inf_sel = 0;
  _pkt_inf = NULL;

#elif (DOSX & WDOSX)
  if (rm_base_sel)
     dpmi_real_free (rm_base_sel);
  rm_base_sel = 0;

#elif (DOSX & POWERPAK)
  UNFINISHED();
#endif

#if (DOSX)
  unlock_code_and_data();

  if (_pkt_inf)
     free (_pkt_inf);
  _pkt_inf = NULL;  /* drop anything still in the queue */
#endif

  ENABLE();
  return (1);
}

#if !(DOSX & DOS4GW) /* Not used for DOS4GW targets;
                      * This is done in asmpkt4.asm
                      */
/*
 *  Enqueue a received packet into '_pkt_inf->ip_queue' or
 *  '_pkt_inf->arp_queue'.
 *
 *  This routine is called from pkt_receiver_rm/_pm().
 *  The packet has been copied to rx_buffer (in DOS memory) by the
 *  packet-driver. We now must copy it to correct queue.
 *  Interrupts are disabled on entry.
 *
 *  Note 1: For real-mode targets SS and SP have been setup to a small
 *          work stack in asmpkt.asm. The stack can only take 64 pushes,
 *          hence use few local variables here.
 *
 *  Note 2: The C-compiler must be told NOT to use register calling
 *          for this routine (MUST use _cdecl) because it's called from
 *          asmpkt.asm via `pkt_enque_ptr' function pointer.
 *
 *  Note 3: Watcom/DOS4GW targets doesn't use real->prot mode upcall (RMCB),
 *          but does the following in asmpkt4.asm instead.
 *
 *  Note 4: For DOSX targets, all code from pkt_enqueue() down to pkt_end()
 *          must be locked in memory.
 *
 *  HACK: For real-mode targets this routine is called via the
 *        `pkt_enque_ptr' function pointer. This was the only way
 *        I could avoid a fixup error for small-model programs.
 */                      

#if (DOSX)
static void pkt_enqueue (unsigned rxBuf, WORD rxLen, WORD handle)
#else
static void _cdecl _far pkt_enqueue (BYTE _far *rxBuf, WORD rxLen, WORD handle)
#endif
{
  struct pkt_ringbuf *q;
  int    index;

  if (handle == _pkt_inf->arp_handle && !_pkt_inf->is_serial)
       q = &_pkt_inf->arp_queue;    /* ARP only packets */
  else q = &_pkt_inf->ip_queue;     /* RARP, IP, PPPOE packets */

  /* don't use pktq_in_index() and pktq_in_buf() because they
   * are not in locked code area.
   */
  index = q->in_index + 1;
  if (index >= q->num_buf)
      index = 0;

  if (index != q->out_index)
  {
    char *head   = q->buf_start + (q->buf_size * q->in_index);
    int   padLen = q->buf_size - 4 - rxLen;

    if (rxLen > q->buf_size - 4)  /* don't overwrite marker */
    {
      rxLen  = q->buf_size - 4;
      padLen = 0;
    }

#if (DOSX & PHARLAP)
    ReadRealMem (head, rm_base + rxBuf, rxLen);

#elif (DOSX & DJGPP)
    dosmemget (rm_base + rxBuf, rxLen, head);

#elif (DOSX & WDOSX)
    memcpy (head, rm_base + rxBuf, rxLen);

#elif (DOSX & POWERPAK)
    UNFINISHED();

#else  /* real-mode targets */
    _fmemcpy (head, rxBuf, rxLen);
#endif

    /* To stay on the safe side we zero-fill remaining old
     * data in this buffer.
     */
    head += rxLen;
    while (padLen--)
       *head++ = 0;
    q->in_index = index;   /* update buffer tail-index */
  }
  else
    q->num_drop++;
}  


/*
 * We have allocated a real-mode callback (RMCB) to gain control
 * here when the packet-driver makes an upcall.
 *
 * Entry AL = 0; driver requests a buffer. We return ES:DI of real-mode buffer
 *       BX = handle (IP, ARP or RARP)
 *       CX = length of packet
 * Entry AL = 1; driver has put the data in buffer, we then enqueues the buffer
 *       BX = handle (IP, ARP or RARP)
 *       CX = length of packet
 *
 * Interrupts are disabled on entry.
 *
 * to-do!!: allocate a real-stub that calls the RMCB only on the 2nd upcall.
 */
#if (DOSX & PHARLAP)
  static void pkt_receiver_pm (SWI_REGS *r)
  { 
    if ((BYTE)r->eax == 0)         /* AL == 0; rx-buffer request */
    {
      if (!_pkt_inf || (WORD)r->ecx > ETH_MAX) /* !!should be for current driver */
      {
        r->es  = 0;
        r->edi = 0;
      }
      else
      {
        r->es  = RP_SEG (rm_base);
        r->edi = RX_BUF();
      }
    }
    else if ((WORD)r->esi && _pkt_inf)   /* AL != 0; rx-buffer filled */
            pkt_enqueue (RX_BUF(), (WORD)r->ecx, (WORD)r->ebx);
  }

#elif (DOSX & DJGPP)
  static void pkt_receiver_pm (void)
  {
    __dpmi_regs *r = &rm_reg;

    if (r->h.al == 0)
    {
      if (!_pkt_inf || r->x.cx > ETH_MAX)
      {
        r->x.es = 0;
        r->x.di = 0;
      }
      else
      {
        r->x.es = RP_SEG();
        r->x.di = RX_BUF();
      }
    }
    else if (r->x.si && _pkt_inf)
            pkt_enqueue (RX_BUF(), (WORD)r->x.cx, (WORD)r->x.bx);
  }

#elif (DOSX & WDOSX)      /* !!fix-me: assumes bcc32 */
  static void pkt_receiver_pm (void)
  {
    static struct DPMI_regs *r; /* static because of "pop ebp" below */

    r = &rm_cb.cb_reg;
    if ((BYTE)r->r_ax == 0)
    {
      if (!_pkt_inf || r->r_cx > ETH_MAX)
      {
        r->r_es = 0;
        r->r_di = 0;
      }
      else
      {
        r->r_es = RP_SEG();
        r->r_di = RX_BUF();
      }
    }
    else if (_pkt_inf && r->r_si)
    {
      static UINT stack [256];  /* !!fix-me: not locked */

      stackset (stack[256-1]);
      pkt_enqueue (RX_BUF(), r->r_cx, r->r_bx);
      stackrestore();
    }
    __asm {   /* make IRET from this near function */
      pop ebp
      iret
    }
    /* not reached */
  }

#elif (DOSX & POWERPAK)
  static void pkt_receiver_pm (IREGS *r)
  {
    UNFINISHED();

    if ((BYTE)r->r_ax == 0)
    {
      if (!_pkt_inf || r->r_cx > ETH_MAX)
      {
        r->r_es = 0;
        r->r_di = 0;
      }
      else
      {
        r->r_es = RP_SEG();
        r->r_di = RX_BUF();
      }
    }
    else if (r->r_si && _pkt_inf)
            pkt_enqueue (RX_BUF(), r->r_cx, r->r_bx);
  }
#endif

void _pkt_end (void) {}

#endif  /* !(DOSX & DOS4GW) */

/* Restore default stack checking and tracing
 */
#if defined(__HIGHC__)   
#pragma pop(check_stack)
#pragma pop(call_trace)
#pragma pop(prolog_trace)
#pragma pop(epilog_trace)
#endif

#if defined(__WATCOMC__)
#pragma pop(check_stack)
#endif

#if defined(_MSC_VER_)
#pragma pop(check_stack)
#endif

#if (defined(__TURBOC__) || defined(__BORLANDC__)) && !defined(OLD_TURBOC)
#pragma option -N.
#endif


/*
 * Send a link-layer frame. For PPP/SLIP 'tx' contains no MAC-header.
 * For EtherNet, Token-Ring and FDDI, 'tx' contains the complete frame.
 */
int pkt_send (const void *tx, int length)
{
  IREGS regs;
  WORD  seg, ofs;
  int   tx_cnt;

  START_PROFILE ("pkt_send");

  ASSERT_PKT_INF (0);

#if (DOSX & PHARLAP)
  seg = RP_SEG (rm_base);
  ofs = TX_BUF();
  WriteRealMem (rm_base + ofs, (void*)tx, length);

#elif (DOSX & DJGPP)
  seg = RP_SEG();
  ofs = TX_BUF();
  dosmemput (tx, length, rm_base + ofs);

#elif (DOSX & (DOS4GW|WDOSX))
  seg = RP_SEG();
  ofs = TX_BUF();
  memcpy (rm_base + ofs, tx, length);

#elif (DOSX & POWERPAK)
  UNFINISHED();

#else
  seg = FP_SEG (tx);
  ofs = FP_OFF (tx);
  /* it's no need to copy anything
   */
#endif

  tx_cnt = 1 + _pktretries;

  while (tx_cnt--)
  {
    regs.r_ax = PD_SEND;
    regs.r_ds = seg;
    regs.r_si = ofs;
    regs.r_cx = length;
    if (!PKT_API(&regs))
       continue;

    STOP_PROFILE();
    return (length);
  }
  STOP_PROFILE();
  STAT (macstats.num_tx_err++);
  return (0);
}

/**************************************************************************/

int pkt_get_addr (eth_address *eth)
{
  IREGS regs;

  ASSERT_PKT_INF (0);

  regs.r_ax = PD_GET_ADDRESS;
  regs.r_bx = _pkt_inf->ip_handle;
  regs.r_cx = sizeof (*eth);

#if (DOSX & PHARLAP)
  regs.r_es = RP_SEG (rm_base);
  regs.r_di = PKT_TMP();

#elif (DOSX & (DJGPP|DOS4GW|WDOSX))
  regs.r_es = RP_SEG();
  regs.r_di = PKT_TMP();

#elif (DOSX & POWERPAK)
  UNFINISHED();

#else
  regs.r_es = FP_SEG (eth);
  regs.r_di = FP_OFF (eth);
#endif

  if (!PKT_API(&regs))
  {
    outsnl (_LANG("ERROR: Cannot read ethernet address"));
    return (0);
  }

#if (DOSX & PHARLAP)
  ReadRealMem (eth, rm_base + PKT_TMP(), sizeof(*eth));

#elif (DOSX & DJGPP)
  dosmemget (rm_base + PKT_TMP(), sizeof(*eth), eth);

#elif (DOSX & (DOS4GW|WDOSX))
  memcpy (eth, rm_base + PKT_TMP(), sizeof(*eth));

#elif (DOSX & POWERPAK)
  UNFINISHED();
#endif
  return (1);
}

/**************************************************************************/

int pkt_set_addr (eth_address *eth)
{
  IREGS regs;

  ASSERT_PKT_INF (0);

  regs.r_ax = PD_SET_ADDR;
  regs.r_cx = sizeof (*eth);

#if (DOSX & PHARLAP)
  WriteRealMem (rm_base + PKT_TMP(), (void*)eth, sizeof(*eth));
  regs.r_es = RP_SEG (rm_base);
  regs.r_di = PKT_TMP();

#elif (DOSX & DJGPP)
  dosmemput ((void*)eth, sizeof(*eth), rm_base + PKT_TMP());
  regs.r_es = RP_SEG();
  regs.r_di = PKT_TMP();

#elif (DOSX & (DOS4GW|WDOSX))
  memcpy (rm_base + PKT_TMP(), eth, sizeof(*eth));
  regs.r_es = RP_SEG();
  regs.r_di = PKT_TMP();

#elif (DOSX & POWERPAK)
  UNFINISHED();

#else
  regs.r_es = FP_SEG (eth);
  regs.r_di = FP_OFF (eth);
#endif

  if (!PKT_API(&regs))
     return (0);

  memcpy (&eth_addr, eth, sizeof(*eth));
  return (1);
}

/*
 *  Clear both the IP and ARP queues.
 */
int pkt_buf_wipe (void)
{
  ASSERT_PKT_INF (0);
  pktq_clear (&_pkt_inf->ip_queue);
  pktq_clear (&_pkt_inf->arp_queue);
  return (1);
}

/*
 *  Release an IP or ARP/RARP packet.
 */
void pkt_free_pkt (const void *pkt, BOOL is_ip)
{
  struct pkt_ringbuf *q;

  if (!_pkt_inf || !pkt)
     return;

  q = is_ip ? &_pkt_inf->ip_queue : &_pkt_inf->arp_queue;

  if (pkt != (const void*) (pktq_out_buf(q) + _pkt_ip_ofs))
  {
#if defined(USE_DEBUG)
    (*_printf) ("%s: freeing illegal %s packet %ph.\r\n",
                __FILE__, is_ip ? "IP" : "ARP/RARP", pkt);
#endif
    pktq_clear (q);
  }
  else
    pktq_inc_out (q);
}


/*
 *  Return number of packets waiting in queues
 */
int pkt_waiting_ip (void)
{
  if (_pkt_inf)
     return pktq_queued (&_pkt_inf->ip_queue);
  return (-1);
}

int pkt_waiting_arp (void)
{
  if (_pkt_inf)
     return pktq_queued (&_pkt_inf->arp_queue);
  return (-1);
}

int pkt_waiting (void)
{
  if (_pkt_inf)
     return pktq_queued (&_pkt_inf->ip_queue) +
            pktq_queued (&_pkt_inf->arp_queue);
  return (-1);
}

/*
 *  Return number of packets dropped.
 *  Reset counter afterwards.
 */
int pkt_dropped_ip (void)
{
  int n = -1;

  if (_pkt_inf)
  {
    n = (int)_pkt_inf->ip_queue.num_drop;
    _pkt_inf->ip_queue.num_drop = 0;
  }
  return (n);
}

int pkt_dropped_arp (void)
{
  int n = -1;

  if (_pkt_inf)
  {
    n = (int)_pkt_inf->arp_queue.num_drop;
    _pkt_inf->arp_queue.num_drop = 0;
  }
  return (n);
}

int pkt_dropped (void)
{
  return pkt_dropped_ip() + pkt_dropped_arp();
}

/*
 *  Initialise Packet driver interface. First determine vector to use;
 *  if env. vars WATTCP.VEC or TCP_PKTINT is specified, use that vector,
 *  else search for PKTDRVR between PKT_FIRST_VEC and PKT_LAST_VEC
 *  (0x60 - 0x80).
 *
 *  Note: Env-var "TCP_PKTINT" is for compatibility with
 *        tcplib for djgpp.
 *
 *  If DOSX, probe and initialise protected-mode driver.
 *  Call pkt_init() to initialise API.
 */
BOOL pkt_eth_init (eth_address *eth)
{
  char *rm_vector = getenv ("WATTCP.VEC");
  int   drvr_ok   = 0;

  if (rm_vector)
  {
    pkt_interrupt = atox (rm_vector);
    if (pkt_interrupt < PKT_FIRST_VEC || pkt_interrupt > PKT_LAST_VEC)
        pkt_interrupt = 0;  /* discard illegal value */
  }
  else if ((rm_vector = getenv ("TCP_PKTINT")) != NULL)
  {
    pkt_interrupt = atox (rm_vector);
    if (pkt_interrupt < PKT_FIRST_VEC || pkt_interrupt > PKT_LAST_VEC)
        pkt_interrupt = 0;  /* discard illegal value */
  }
  else
    pkt_interrupt = 0;


  {
#if (DOSX)
    int pm_driver = pkt32_drvr_probe (pm_driver_list);

    drvr_ok = (pm_driver > 0 && pkt32_drvr_init(pm_driver) != 0);

#if defined(USE_DEBUG)
    if (pm_driver)
       (*_printf) ("Using Pmode `%s' driver at %08lX",
                   pkt32_drvr_name(pm_driver), (DWORD)_pkt32_drvr);
#endif

    if (!drvr_ok)  /* pmode driver failed, try rmode driver */
#endif
      drvr_ok = (pkt_init() != 0);

    if (!drvr_ok)
       return (FALSE);  /* no suitable driver found */
  }
  memcpy (eth, &eth_addr, sizeof(*eth));
  return (TRUE);
}

/*
 *  Find the interrupt for the PKTDRVR by searching interrupt handler
 *  entries (at vector+3) for signature string "PKT DRVR".
 */
#if (DOSX & PHARLAP)
  static WORD find_vector (int first, int num)
  {
    WORD vector;

    for (vector = first; vector < first+num; vector++)
    {
      char    temp[16];
      REALPTR rp;

      _dx_rmiv_get ((UINT)vector, &rp);
      if (rp)
      {
        ReadRealMem (&temp, rp, sizeof(temp));
        if (!memcmp(temp+3, &pkt_sign, sizeof(pkt_sign)))
           return (vector);
      }
    }
    return (0);
  }

#elif (DOSX & DJGPP)
  static WORD find_vector (int first, int num)
  {
    WORD vector;

    for (vector = first; vector < first+num; vector++)
    {
      char  temp[16];
      DWORD rp;
      __dpmi_raddr realAdr;

      __dpmi_get_real_mode_interrupt_vector (vector, &realAdr);
      rp = (realAdr.segment << 4) + realAdr.offset16;
      if (rp)
      {
        dosmemget (rp, sizeof(temp), &temp);
        if (!memcmp(temp+3, &pkt_sign, sizeof(pkt_sign)))
           return (vector);
      }
    }
    return (0);
  }

#elif (DOSX & (DOS4GW|WDOSX))
  static WORD find_vector (int first, int num)
  {
    WORD vector;

    for (vector = first; vector < first+num; vector++)
    {
      BYTE *addr = (BYTE*) dpmi_get_real_vector (vector);

      if (addr && !memcmp(addr+3, &pkt_sign, sizeof(pkt_sign)))
         return (vector);
    }
    return (0);
  }

#elif (DOSX & POWERPAK)
  static WORD find_vector (int first, int num)
  {
    UNFINISHED();
    return (0);
  }

#else       /* real-mode version */
  static WORD find_vector (int first, int num)
  {
    WORD vector;

     pkt_enque_ptr = pkt_enqueue;
    _pkt_enque_ptr = pkt_enqueue;

    for (vector = first; vector < first+num; vector++)
    {
      char _far *addr = (char _far *)getvect(vector);

      if (addr && !_fmemcmp (addr+3, &pkt_sign, sizeof(pkt_sign)))
         return (vector);
    }
    return (0);
  }
#endif


/*
 *  DOS-extender functions for allocation a real-mode callback that
 *  the real-mode PKTDRVR will call when a packet is received.
 *  Lock down all code and data that is touched in this callback.
 */
#if (DOSX & PHARLAP)
  static int setup_rmode_callback (void)
  {
    rm_base = _dx_alloc_rmode_wrapper_retf (pkt_receiver_pm, NULL,
                                            RDATA_SIZE, 1024);
    if (!rm_base)
       return (-1);
    return (0);
  }

  static int lock_code_and_data (void) /* Needed for 386|VMM only (?) */
  {
    UINT size = (UINT)&_pkt_end - (UINT)&pkt_enqueue;

    _dx_lock_pgsn ((void*)&pkt_enqueue, size);
    _dx_lock_pgsn ((void*)&ReadRealMem, 100);
    _dx_lock_pgsn ((void*)_pkt_inf, sizeof(*_pkt_inf));
    return (0);
  }

  static int unlock_code_and_data (void)
  {
    UINT size = (UINT)&_pkt_end - (UINT)&pkt_enqueue;

    _dx_ulock_pgsn ((void*)&pkt_enqueue, size);
    _dx_ulock_pgsn ((void*)&ReadRealMem, 100);
    _dx_ulock_pgsn ((void*)_pkt_inf, sizeof(*_pkt_inf));
    return (0);
  }

#elif (DOSX & DJGPP)
  static int setup_rmode_callback (void)
  {
    int i;

    rm_cb.pm_offset       = (DWORD) &pkt_receiver_pm;
    _pkt_inf->rm_mem.size = (RDATA_SIZE + 15) / 16;

    if (_go32_dpmi_allocate_dos_memory(&_pkt_inf->rm_mem))
       return (-2);

    if (_go32_dpmi_allocate_real_mode_callback_retf(&rm_cb,&rm_reg))
       return (-1);

    if (_pkt_inf->rm_mem.rm_offset != 0)
       return (-2);

    rm_base = (_pkt_inf->rm_mem.rm_segment << 4);
    for (i = 0; i < RDATA_SIZE/4; i++)
        _farpokel (_dos_ds, rm_base + 4*i, 0L);

  #if 0  /* test */
    (*_printf) ("rm_mem = %04X:%04X  rmode call-back %04X:%04X\r\n",
                _pkt_inf->rm_mem.rm_segment, _pkt_inf->rm_mem.rm_offset,
                rm_cb.rm_segment, rm_cb.rm_offset);
  #endif
    return (0);
  }

  static int lock_code_and_data (void)
  {
    DWORD size = (DWORD)&_pkt_end - (DWORD)&pkt_enqueue;

    if (_go32_dpmi_lock_code(&pkt_enqueue, size) ||
        _go32_dpmi_lock_code(&dosmemget, 100)    ||
        _go32_dpmi_lock_data(_pkt_inf, sizeof(*_pkt_inf)))
       return (-1);
    /* rm_reg is already locked */
    return (0);
  }

  static int unlock_code_and_data (void)
  {
    __dpmi_meminfo mem;
    DWORD base = 0;
 
    __dpmi_get_segment_base_address (_my_ds(), &base);

    mem.address = base + (DWORD)&pkt_enqueue;
    mem.size    = (DWORD)&_pkt_end - (DWORD)&pkt_enqueue;
    __dpmi_unlock_linear_region (&mem);

    mem.address = base + (DWORD)&dosmemget;
    mem.size    = 100;
    __dpmi_unlock_linear_region (&mem);

    mem.address = base + (DWORD)_pkt_inf;
    mem.size    = sizeof(*_pkt_inf);
    __dpmi_unlock_linear_region (&mem);
    return (0);
  }

#elif (DOSX & WDOSX)
  static int setup_rmode_callback (void)
  {
    WORD size  = RDATA_SIZE;
    BYTE entry = *(BYTE*) pkt_receiver_pm;

    if (entry != 0xC8 && entry != 0x55)  /* "enter x,y" or "push (e)bp" */
    {
      outsnl (_LANG("pkt_receiver_pm() has wrong entry. Use "
                    "\"normal\" stack-frame."));
      return (-3);
    }

    rm_base_sel = 0;
    rm_base_seg = dpmi_real_malloc (size, &rm_base_sel);

    if (!rm_base_seg || !rm_base_sel)
       return (-1);

    if (!dpmi_alloc_callback(pkt_receiver_pm, &rm_cb))
       return (-2);

    rm_base = (BYTE*) (rm_base_seg << 4);
    memset (rm_base, 0, size);

  #if 1  /* test */
    (*_printf) ("rm_mem = %04X:0000  rmode call-back %04X:%04X\r\n",
                rm_base_seg, rm_cb.cb_segment, rm_cb.cb_offset);
  #endif
    return (0);
  }

  static int lock_code_and_data (void)
  {
    if (!dpmi_lock_region (&rm_cb, sizeof(rm_cb)))
       return (-1);
    return (0);
  }

  static int unlock_code_and_data (void)
  {
    if (!dpmi_unlock_region (&rm_cb, sizeof(rm_cb)))
       return (-1);
    return (0);
  }

#elif (DOSX & DOS4GW) /* pkt_receiver4_rm() isn't a r->pmode callback,
                       * but what the heck...
                       */
  static int setup_rmode_callback (void) 
  {
    int length;

    /* test for asmpkt4.asm/pcpkt.h mismatch
     */
    if (asmpkt_size_chk != sizeof(*_pkt_inf)) 
    {
#ifdef USE_DEBUG
      fprintf (stderr,
               "sizeof(pkt_info) = %d pcpkt.h\n"
               "sizeof(pkt_info) = %d asmpkt4.asm, (diff %d)\r\n",
               sizeof(*_pkt_inf), asmpkt_size_chk,
               sizeof(*_pkt_inf) - asmpkt_size_chk);
#endif
      return (-3);
    }

    /* Allocate DOS-memory for pkt_receiver4_rm() and temp/Tx buffers.
     */
    length = TX_BUF() + IP_SIZE;
    rm_base_seg = dpmi_real_malloc (length, &pkt_rcv_sel);
    if (!rm_base_seg)
       return (-2);

    rm_base = (BYTE*) (((DWORD)rm_base_seg) << 4);

    /* Clear DOS area and copy code down into it.
     */
    memset (rm_base, 0, length);
    length = PKT_TMP() - 1;
    memcpy (rm_base, (void*)&pkt_receiver4_start, length);
    return (0);
  }

  static int lock_code_and_data (void)  
  {
    return (0);
  }

  static int unlock_code_and_data (void)
  {
    return (0);
  }

#elif (DOSX & POWERPAK)
  static int setup_rmode_callback (void) { UNFINISHED(); return (-1); }
  static int lock_code_and_data (void)   { UNFINISHED(); return (-1); }
  static int unlock_code_and_data (void) { UNFINISHED(); return (-1); }
#endif


/*
 *  For DOS4GW targets, allocate the '_pkt_inf' structure
 *  from DOS memory. All others allocate from heap.
 */
static int setup_pkt_inf (void)
{
#if (DOSX & DOS4GW)
  DWORD seg = dpmi_real_malloc (sizeof(*_pkt_inf), &pkt_inf_sel);

  asmpkt_inf = (struct pkt_info*) (seg << 16); /* run-time location */
  _pkt_inf   = (struct pkt_info*) (seg << 4);
#else
  _pkt_inf = malloc (sizeof(*_pkt_inf));
#endif

  if (!_pkt_inf)   /* Allocation failed */
     return (0);

  /* Clear area
   */
  memset (_pkt_inf, 0, sizeof(*_pkt_inf));

#if !defined(OLD_TURBOC)
  assert (ARP_SIZE >= (ARP_MAX - PKT_MARGIN));
#endif

  pktq_init (&_pkt_inf->ip_queue, sizeof(_pkt_inf->ip_buf[0]),
             DIM(_pkt_inf->ip_buf), (char*)&_pkt_inf->ip_buf[0][0]);

  pktq_init (&_pkt_inf->arp_queue, sizeof(_pkt_inf->arp_buf[0]),
             DIM(_pkt_inf->arp_buf), (char*)&_pkt_inf->arp_buf[0][0]);

#if (DOSX & DOS4GW)
  _pkt_inf->ip_queue.dos_ofs  = offsetof (struct pkt_info, ip_buf[0][0]);
  _pkt_inf->arp_queue.dos_ofs = offsetof (struct pkt_info, arp_buf[0][0]);
#endif
  return (1);
}


/*
 * Microsoft Quick-C doesn't have 'intr()' so we make our own.
 */
#if defined(_MSC_VER) && (DOSX == 0)
static void intr (int intno, IREGS *reg)
{
  union  REGS  r;
  struct SREGS s;

  r.x.ax = reg->r_ax;
  r.x.bx = reg->r_bx;
  r.x.cx = reg->r_cx;
  r.x.dx = reg->r_dx;
  r.x.si = reg->r_si;
  r.x.di = reg->r_di;
  s.ds   = reg->r_ds;
  s.es   = reg->r_es;
  int86x (intno, &r, &r, &s);
  reg->r_flags = r.x.cflag;
  reg->r_ax    = r.x.ax;
  reg->r_bx    = r.x.bx;
  reg->r_cx    = r.x.cx;
  reg->r_dx    = r.x.dx;
  reg->r_si    = r.x.si;
  reg->r_di    = r.x.di;
  reg->r_ds    = s.ds;
  reg->r_es    = s.es;
}
#endif

/*
 *  The API entry to the network link-driver. Either use protected mode
 *  interface via a (far) call (dynamic loaded module) or issue an
 *  interrupt for the real mode PKTDRVR.
 *
 *  Return TRUE if CARRRY is clear, else FALSE.
 */
static int pkt_api_entry (IREGS *reg, unsigned line)
{
#if (DOSX)
 /* Use 32-bit API; accessing card via pmode driver (to-do!!)
  */
  if (_pkt32_drvr) 
  {
    reg->r_flags = 0;
    if (!(*_pkt32_drvr)(reg))   /* call the pmode interface */
    {
      reg->r_flags |= CARRY_BIT;
      return (0);
    }
    return (1);
  }
#endif

  if (!pkt_interrupt)
  {
#if defined(USE_DEBUG)
    fprintf (stderr, "%s (%d): API called after deinit.\n", __FILE__, line);
#endif
    reg->r_flags |= CARRY_BIT;
    ARGSUSED (line);
    return (0);
  }

 /* Use the (slower) 16-bit real-mode PKTDRVR API.
  */
#if (DOSX & PHARLAP)
  _dx_real_int ((UINT)pkt_interrupt, reg);

#elif (DOSX & DJGPP)
  __dpmi_int ((int)pkt_interrupt, reg);

#elif (DOSX & DOS4GW) && defined(__WATCOMC__)
  dpmi_real_interrupt ((int)pkt_interrupt, reg);

#elif (DOSX & WDOSX)
  dpmi_real_interrupt2 ((int)pkt_interrupt, reg);

#elif (DOSX & POWERPAK)
  UNFINISHED();

#elif (DOSX == 0) && defined(_MSC_VER)
  intr ((int)pkt_interrupt, reg);

#elif (DOSX == 0) && defined(__WATCOMC__)
  intr ((int)pkt_interrupt, (union REGPACK*)reg);

#elif (DOSX == 0)
  intr ((int)pkt_interrupt, (struct REGPACK*)reg);

#else
  #error Unknown method in pkt_api_entry
#endif

  return ((reg->r_flags & CARRY_BIT) == 0);
}


#if defined(USE_MULTICAST)
/*
 * _pkt_set_rcv_mode - sets the receive mode of the interface
 *
 * int _pkt_set_rcv_mode (int handle, int mode)
 * Where:
 *    handle  is the handle returned by access_type
 *    mode    is one of the following modes:
 *            1       turn off receiver
 *            2       receive only packets sent to this interface
 *            3       mode 2 plus broadcast packets <default>
 *            4       mode 3 plus limited multicast packets
 *            5       mode 3 plus all multicast packets
 *            6       all packets (AKA promiscuous mode )
 * Returns:
 *    -1      upon error - _pkterror is set
 *     0      if the mode was set successfully
 */
int _pkt_set_rcv_mode (int handle, int mode)
{
  IREGS regs;

  if (!_multicast_on) /* normal unicast/broadcast don't need to set mode */
  {
    _pkterror = CANT_SET;
    return (-1);
  }

  /* This needs an Extended driver. SLIP/PPP is point-to-point.
   */
  if (_pktdevlevel < 2 || _pktserial)
  {
    _pkterror = CANT_SET;
    return (-1);
  }

  regs.r_ax = PD_SET_RCV;
  regs.r_bx = handle;
  regs.r_cx = mode;

  if (!PKT_API(&regs))
  {
    PKT_ERR ("Error setting receiver mode: ", regs.r_dx);
    return (-1);
  }
  return (0);
}


/*
 * _pkt_get_rcv_mode - gets the receive mode of the interface
 *
 * int _pkt_get_rcv_mode (int handle)
 * Where:
 *    handle  is the handle returned by access_type
 *
 * Returns:
 *    -1      upon error - _pkterror is set
 *    mode    is one of the following modes: (upon return)
 *            1       turn off receiver
 *            2       receive only packets sent to this interface
 *            3       mode 2 plus broadcast packets <default>
 *            4       mode 3 plus limited multicast packets
 *            5       mode 3 plus all multicast packets
 *            6       all packets (AKA promiscuous mode )
 */
int _pkt_get_rcv_mode (int handle)
{
  IREGS regs;

  /* This needs an Extended driver (not SLIP/PPP)
   */
  if (_pktdevlevel < 2 || _pktserial)
  {
    _pkterror = BAD_COMMAND;
    return (-1);
  }

  regs.r_ax = PD_GET_RCV;
  regs.r_bx = handle;

  if (!PKT_API(&regs))
  {
    PKT_ERR ("Error getting receiver mode: ", regs.r_dx);
    return (-1);
  }
  return (regs.r_ax);
}

/*
 * _pkt_get_multicast_list - gets the current list of multicast addresses
 *                           from the packet driver
 *
 * int _pkt_get_multicast_list (int len, eth_address *listbuf)
 * Where:
 *    len     is the length of listbuf
 *    listbuf is the buffer into which the list is placed
 * Returns:
 *    -1      upon error - _pkterror is set
 *    len     if retrieval was successful
 *
 */
int _pkt_get_multicast_list (int len, eth_address *listbuf)
{
  IREGS regs;

  /* Basic drivers don't support multicast
   */
  if (_pktdevlevel < 2 || _pktserial)
  {
    _pkterror = NO_MULTICAST;
    return (-1);
  }
  regs.r_ax = PD_GET_MULTI;

  if (!PKT_API(&regs))
  {
    PKT_ERR ("Error getting multicast list: ", regs.r_dx);
    return (-1);
  }

  /* move it into the caller's buffer and return happily
   */
  len = min (len, (WORD)regs.r_cx);
#if (DOSX & PHARLAP)
  {
    REALPTR rp;
    RP_SET (rp, (WORD)regs.r_di, regs.r_es);
    ReadRealMem ((void*)listbuf, rp, len);
  }

#elif (DOSX & DJGPP)
  dosmemget (regs.r_di + (regs.r_es << 4), len, (void*)listbuf);

#elif (DOSX & (DOS4GW|WDOSX))
  memcpy (listbuf, SEG_OFS_TO_LIN(regs.r_es,regs.r_di), len);

#elif (DOSX & POWERPAK)
  UNFINISHED();

#else
  _fmemcpy (listbuf, MK_FP(regs.r_es,regs.r_di), len);
#endif

  return (len);
}

/*
 * _pkt_set_multicast_list - sets the list of multicast addresses for which
 *                         the PD is responsible.
 *
 * int _pkt_set_multicast_list (int len, eth_address *listbuf)
 * Where:
 *    len     is the length of listbuf
 *    listbuf is the buffer containing the list
 * Returns:
 *    -1      upon error - _pkterror is set
 *     0      if set was successful
 */
int _pkt_set_multicast_list (int len, eth_address *listbuf)
{
  IREGS regs;
  WORD  seg, ofs;

  ASSERT_PKT_INF (0);

  /* Basic drivers don't support multicast
   */
  if (_pktdevlevel < 2 || _pktserial)
  {
    _pkterror = NO_MULTICAST;
    return (-1);
  }

#if (DOSX & PHARLAP)
  seg = RP_SEG (rm_base);
  ofs = PKT_TMP();
  WriteRealMem (rm_base + PKT_TMP(), (void*)listbuf, len);

#elif (DOSX & DJGPP)
  seg = RP_SEG();
  ofs = PKT_TMP();
  dosmemput ((void*)listbuf, len, rm_base + PKT_TMP());

#elif (DOSX & (DOS4GW|WDOSX))
  seg = RP_SEG();
  ofs = PKT_TMP();
  memcpy (rm_base + PKT_TMP(), (void*)listbuf, len);

#elif (DOSX & POWERPAK)
  UNFINISHED();

#else
  seg = FP_SEG (listbuf);
  ofs = FP_OFF (listbuf);
#endif

  regs.r_ax = PD_SET_MULTI;
  regs.r_cx = len;
  regs.r_es = seg;
  regs.r_di = ofs;

  if (!PKT_API(&regs))
  {
    PKT_ERR ("Error setting multicast list: ", regs.r_dx);
    return (-1);
  }
  return (0);
}

/*
 * _pkt_get_ip_rcv_mode - gets the receive mode of the interface for
 *                      the IP handle
 *
 * int _pkt_get_ip_rcv_mode(void)
 * Returns:
 *    -1      if the request failed - _pkterror is set
 *    mode    is one of the following modes: (upon return)
 *            1       turn off receiver
 *            2       receive only packets sent to this interface
 *            3       mode 2 plus broadcast packets <default>
 *            4       mode 3 plus limited multicast packets
 *            5       mode 3 plus all multicast packets
 *            6       all packets (AKA promiscuous mode )
 */
int _pkt_get_ip_rcv_mode (void)
{
  return _pkt_get_rcv_mode (_pkt_inf->ip_handle);
}

/*
 * _pkt_set_ip_rcv_mode - sets the receive mode of the interface for
 *                      the IP handle
 *
 * int _pkt_set_ip_rcv_mode(int mode)
 * Where:
 *    mode    is one of the following modes: (upon return)
 *            1       turn off receiver
 *            2       receive only packets sent to this interface
 *            3       mode 2 plus broadcast packets <default>
 *            4       mode 3 plus limited multicast packets
 *            5       mode 3 plus all multicast packets
 *            6       all packets (AKA promiscuous mode )
 * Returns:
 *    -1      if the request failed - _pkterror is set
 *     0      mode set was successful
 */
int _pkt_set_ip_rcv_mode (int mode)
{
  return _pkt_set_rcv_mode (_pkt_inf->ip_handle, mode);
}

#endif /* USE_MULTICAST */

