/*
 * Public interface to the boxv library.
 */

/* PCI vendor and device IDs. */
#define BOXV_PCI_VEN    0x80EE
#define BOXV_PCI_DEV    0xBEEF

/* A structure describing mode information. Note: The mode numbers
 * usually match VBE, but this should not be relied upon.
 */
typedef struct {
    int     mode_no;        /* Mode number */
    int     xres;           /* Horizontal resolution */
    int     yres;           /* Vertical resolution */
    int     bpp;            /* Color depth */
} BOXV_mode_t;

extern void BOXV_mode_enumerate( void *cx, int (cb)( void *cx, BOXV_mode_t *mode ) );
extern int  BOXV_detect( void *cx, unsigned long *vram_size );
extern int  BOXV_ext_mode_set( void *cx, int xres, int yres, int bpp, int v_xres, int v_yres );
extern int  BOXV_mode_set( void *cx, int mode_no );
extern int  BOXV_dac_set( void *cx, unsigned start, unsigned count, void *pal );
extern int  BOXV_ext_disable( void *cx );
