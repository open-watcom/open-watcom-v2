#include "boxv.h"
#include "boxv_io.h"

/*-------------- VGA Specific ----------------*/

/* VGA I/O port addresses. */
#define VGA_CRTC            0x3D4   /* Color only! */
#define VGA_ATTR_W          0x3C0
#define VGA_ATTR_R          0x3C1
#define VGA_MISC_OUT_W      0x3C2
#define VGA_SEQUENCER       0x3C4
#define VGA_SEQUENCER_DATA  0x3C5
#define VGA_PIXEL_MASK      0x3C6
#define VGA_DAC_W_INDEX     0x3C8
#define VGA_DAC_DATA        0x3C9
#define VGA_MISC_OUT_R      0x3CC
#define VGA_GRAPH_CNTL      0x3CE
#define VGA_GRAPH_CNTL_DATA 0x3CF
#define VGA_STAT_ADDR       0x3DA   /* Color only! */

/* VGA Attribute Controller register indexes. */
#define VGA_AR_MODE         0x10
#define VGA_AR_OVERSCAN     0x11
#define VGA_AR_PLANE_EN     0x12
#define VGA_AR_PIX_PAN      0x13
#define VGA_AR_COLOR_SEL    0x14

/* VGA Graphics Controller register indexes. */
#define VGA_GR_SET_RESET    0x00
#define VGA_GR_DATA_ROTATE  0x03
#define VGA_GR_READ_MAP_SEL 0x04
#define VGA_GR_MODE         0x05
#define VGA_GR_MISC         0x06
#define VGA_GR_BIT_MASK     0x08

/* VGA Sequencer register indexes. */
#define VGA_SR_RESET        0x00
#define VGA_SR_CLK_MODE     0x01
#define VGA_SR_PLANE_MASK   0x02
#define VGA_SR_MEM_MODE     0x04

/* Sequencer constants. */
#define VGA_SR0_NORESET     0x03
#define VGA_SR0_RESET       0x00
#define VGA_SR1_BLANK       0x20

/* VGA CRTC register indexes. */
#define VGA_CR_HORZ_TOTAL   0x00
#define VGA_CR_CUR_START    0x0A
#define VGA_CR_CUR_END      0x0B
#define VGA_CR_START_HI     0x0C
#define VGA_CR_START_LO     0x0D
#define VGA_CR_CUR_POS_HI   0x0E
#define VGA_CR_CUR_POS_LO   0x0F
#define VGA_CR_VSYNC_START  0x10
#define VGA_CR_VSYNC_END    0x11

/* VGA Input Status Register 1 constants. */
#define VGA_STAT_VSYNC      0x08

/*------------ End VGA Specific --------------*/

/*------------- bochs Specific ---------------*/

#define VBE_DISPI_BANK_ADDRESS          0xA0000
#define VBE_DISPI_BANK_SIZE_KB          64

#define VBE_DISPI_MAX_XRES              1024
#define VBE_DISPI_MAX_YRES              768

#define VBE_DISPI_IOPORT_INDEX          0x01CE
#define VBE_DISPI_IOPORT_DATA           0x01CF

#define VBE_DISPI_INDEX_ID              0x0
#define VBE_DISPI_INDEX_XRES            0x1
#define VBE_DISPI_INDEX_YRES            0x2
#define VBE_DISPI_INDEX_BPP             0x3
#define VBE_DISPI_INDEX_ENABLE          0x4
#define VBE_DISPI_INDEX_BANK            0x5
#define VBE_DISPI_INDEX_VIRT_WIDTH      0x6
#define VBE_DISPI_INDEX_VIRT_HEIGHT     0x7
#define VBE_DISPI_INDEX_X_OFFSET        0x8
#define VBE_DISPI_INDEX_Y_OFFSET        0x9

#define VBE_DISPI_ID0                   0xB0C0
#define VBE_DISPI_ID1                   0xB0C1
#define VBE_DISPI_ID2                   0xB0C2
#define VBE_DISPI_ID3                   0xB0C3
#define VBE_DISPI_ID4                   0xB0C4

#define VBE_DISPI_DISABLED              0x00
#define VBE_DISPI_ENABLED               0x01
#define VBE_DISPI_GETCAPS               0x02
#define VBE_DISPI_8BIT_DAC              0x20
#define VBE_DISPI_LFB_ENABLED           0x40
#define VBE_DISPI_NOCLEARMEM            0x80

#define VBE_DISPI_LFB_PHYSICAL_ADDRESS  0xE0000000

/*------------ End bochs Specific -------------*/


typedef unsigned char   v_byte;
typedef unsigned short  v_word;

/* A structure describing the contents of VGA registers for a mode set. */
typedef struct {
    v_byte          misc;           /* Miscellaneous register. */
    v_byte          seq[5];         /* Sequencer registers. */
    v_byte          crtc[25];       /* CRTC registers. */
    v_byte          gctl[9];        /* Graphics controller registers. */
    v_byte          atr[21];        /* Attribute registers. */        
} v_vgaregs;

/* A structure fully describing a graphics or text mode. */
typedef struct {
    int             mode_no;        /* Internal mode number. */
    int             xres;           /* Horizontal (X) resolution. */
    int             yres;           /* Vertical (Y) resolution. */
    int             bpp;            /* Bits per pixel. */
    int             ext;            /* Non-zero for extended modes. */
    v_vgaregs      *vgaregs;       /* Contents of VGA registers. */ 
} v_mode;

v_vgaregs       vga_regs_ext = {
    0xe3, { 0x01, 0x01, 0x0f, 0x00, 0x0a }, {
    0x5f, 0x4f, 0x50, 0x82, 0x54, 0x80, 0x0b, 0x3e,
    0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xea, 0x0c, 0xdf, 0x28, 0x4f, 0xe7, 0x04, 0xe3, 0xff }, {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0f, 0xff }, {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x41, 0x00, 0x0f, 0x00, 0x00 }
};

v_mode          mode_640_480_8 = {
    0x101, 640, 480, 8, 1, &vga_regs_ext
};

v_mode          mode_800_600_8 = {
    0x103, 800, 600, 8, 1, &vga_regs_ext
};

v_mode          mode_1024_768_8 = {
    0x105, 1024, 768, 8, 1, &vga_regs_ext
};

v_mode          mode_640_480_16 = {
    0x111, 640, 480, 16, 1, &vga_regs_ext
};

v_mode          mode_800_600_16 = {
    0x114, 800, 600, 16, 1, &vga_regs_ext
};

v_mode          mode_1024_768_16 = {
    0x117, 1024, 768, 16, 1, &vga_regs_ext
};

v_mode          mode_640_480_32 = {
    0x129, 640, 480, 32, 1, &vga_regs_ext
};

v_mode          mode_800_600_32 = {
    0x12E, 800, 600, 32, 1, &vga_regs_ext
};

v_mode          mode_1024_768_32 = {
    0x138, 1024, 768, 32, 1, &vga_regs_ext
};

v_mode          *mode_list[] = {
    &mode_640_480_8,
    &mode_800_600_8,
    &mode_1024_768_8,
    &mode_640_480_16,
    &mode_800_600_16,
    &mode_1024_768_16,
    &mode_640_480_32,
    &mode_800_600_32,
    &mode_1024_768_32,
    NULL
};

/* Write a single value to an indexed register at a specified 
 * index. Suitable for the CRTC or graphics controller.
 */
static void vid_wridx( void *cx, int idx_reg, int idx, v_byte data )
{
    vid_outw( cx, idx_reg, idx | (data << 8) );
}

/* Program a sequence of bytes into an indexed register, starting
 * at index 0. Suitable for loading the CRTC or graphics controller.
 */
static void vid_wridx_s( void *cx, int idx_reg, int count, v_byte *data )
{
    int     idx;

    for( idx = 0; idx < count; ++idx )
        vid_wridx( cx, idx_reg, idx, data[idx] );   /* Write index/data. */
}

/* Program a sequence of bytes into the attribute controller, starting 
 * at index 0. Note: This function may not be interrupted by code which
 * also accesses the attribute controller.
 */
static void vid_wratc_s( void *cx, int count, v_byte *data )
{
    int     idx;

    vid_inb( cx, VGA_STAT_ADDR );               /* Reset flip-flop. */
    for( idx = 0; idx < count; ++idx ) {
        vid_outb( cx, VGA_ATTR_W, idx );        /* Write index. */
        vid_outb( cx, VGA_ATTR_W, data[idx] );  /* Write data. */
    }
}

v_mode *find_mode( int mode_no )
{
    v_mode      **p_mode;
    v_mode      *mode;

    mode = NULL;
    for( p_mode = mode_list; *p_mode; ++p_mode ) {
        if( (*p_mode)->mode_no == mode_no ) {
            mode = *p_mode;
            break;
        }
    }
    return( mode );
}

/* Enumerate all available modes. Runs a callback for each mode; if the
 * callback returns zero, the enumeration is terminated.
 */
void BOXV_mode_enumerate( void *cx, int (cb)( void *cx, BOXV_mode_t *mode ) )
{
    BOXV_mode_t     mode_info;
    v_mode          **p_mode;
    v_mode          *mode;

    for( p_mode = mode_list; *p_mode; ++p_mode ) {
        mode = *p_mode;
        mode_info.mode_no = mode->mode_no;
        mode_info.xres    = mode->xres;
        mode_info.yres    = mode->yres;
        mode_info.bpp     = mode->bpp;
        if( !cb( cx, &mode_info ) )
            break;
    }
}

/* Set an extended non-VGA mode with given parameters. 8bpp and higher only.
 * Returns non-zero value on failure.
 */
int BOXV_ext_mode_set( void *cx, int xres, int yres, int bpp, int v_xres, int v_yres )
{
    /* Do basic parameter validation. */
    if( v_xres < xres || v_yres < yres )
        return( -1 );

    /* Put the hardware into a state where the mode can be safely set. */
    vid_inb( cx, VGA_STAT_ADDR );                   /* Reset flip-flop. */
    vid_outb( cx, VGA_ATTR_W, 0 );                  /* Disable palette. */
    vid_wridx( cx, VGA_SEQUENCER, VGA_SR_RESET, VGA_SR_RESET );

    /* Disable the extended display registers. */
    vid_outw( cx, VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_ENABLE );
    vid_outw( cx, VBE_DISPI_IOPORT_DATA, VBE_DISPI_DISABLED );

    /* Program the extended non-VGA registers. */

    /* Set X resoultion. */
    vid_outw( cx, VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_XRES );
    vid_outw( cx, VBE_DISPI_IOPORT_DATA, xres );
    /* Set Y resoultion. */
    vid_outw( cx, VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_YRES );
    vid_outw( cx, VBE_DISPI_IOPORT_DATA, yres );
    /* Set bits per pixel. */
    vid_outw( cx, VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_BPP );
    vid_outw( cx, VBE_DISPI_IOPORT_DATA, bpp );
    /* Set the virtual resolution. */
    vid_outw( cx, VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_VIRT_WIDTH );
    vid_outw( cx, VBE_DISPI_IOPORT_DATA, v_xres );
    vid_outw( cx, VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_VIRT_HEIGHT );
    vid_outw( cx, VBE_DISPI_IOPORT_DATA, v_yres );
    /* Reset the current bank. */
    vid_outw( cx, VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_BANK );
    vid_outw( cx, VBE_DISPI_IOPORT_DATA, 0 );
    /* Set the X and Y display offset to 0. */
    vid_outw( cx, VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_X_OFFSET );
    vid_outw( cx, VBE_DISPI_IOPORT_DATA, 0 );
    vid_outw( cx, VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_Y_OFFSET );
    vid_outw( cx, VBE_DISPI_IOPORT_DATA, 0 );
    /* Enable the extended display registers. */
    vid_outw( cx, VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_ENABLE );
    vid_outw( cx, VBE_DISPI_IOPORT_DATA, VBE_DISPI_ENABLED | VBE_DISPI_8BIT_DAC );

    /* Re-enable the sequencer. */
    vid_wridx( cx, VGA_SEQUENCER, VGA_SR_RESET, VGA_SR0_NORESET );

    /* Re-enable palette. */
    vid_outb( cx, VGA_ATTR_W, 0x20 );               

    return( 0 );
}

/* Set the requested mode (text or graphics). 
 * Returns non-zero value on failure.
 */
int BOXV_mode_set( void *cx, int mode_no )
{
    v_mode          *mode;
    v_vgaregs       *vgarg;

    mode = find_mode( mode_no );
    if( !mode )
        return( -1 );

    /* Put the hardware into a state where the mode can be safely set. */
    vid_inb( cx, VGA_STAT_ADDR );                   /* Reset flip-flop. */
    vid_outb( cx, VGA_ATTR_W, 0 );                  /* Disable palette. */
    vid_wridx( cx, VGA_CRTC, VGA_CR_VSYNC_END, 0 ); /* Unlock CR0-CR7. */
    vid_wridx( cx, VGA_SEQUENCER, VGA_SR_RESET, VGA_SR_RESET );

    /* Disable the extended display registers. */
    vid_outw( cx, VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_ENABLE );
    vid_outw( cx, VBE_DISPI_IOPORT_DATA, VBE_DISPI_DISABLED );

    /* Optionally program the extended non-VGA registers. */
    if( mode->ext ) {
        /* Set X resoultion. */
        vid_outw( cx, VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_XRES );
        vid_outw( cx, VBE_DISPI_IOPORT_DATA, mode->xres );
        /* Set Y resoultion. */
        vid_outw( cx, VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_YRES );
        vid_outw( cx, VBE_DISPI_IOPORT_DATA, mode->yres );
        /* Set bits per pixel. */
        vid_outw( cx, VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_BPP );
        vid_outw( cx, VBE_DISPI_IOPORT_DATA, mode->bpp );
        /* Set the virtual resolution. */
        vid_outw( cx, VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_VIRT_WIDTH );
        vid_outw( cx, VBE_DISPI_IOPORT_DATA, mode->xres );
        vid_outw( cx, VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_VIRT_HEIGHT );
        vid_outw( cx, VBE_DISPI_IOPORT_DATA, mode->yres );
        /* Reset the current bank. */
        vid_outw( cx, VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_BANK );
        vid_outw( cx, VBE_DISPI_IOPORT_DATA, 0 );
        /* Set the X and Y display offset to 0. */
        vid_outw( cx, VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_X_OFFSET );
        vid_outw( cx, VBE_DISPI_IOPORT_DATA, 0 );
        vid_outw( cx, VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_Y_OFFSET );
        vid_outw( cx, VBE_DISPI_IOPORT_DATA, 0 );
        /* Enable the extended display registers. */
        vid_outw( cx, VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_ENABLE );
        vid_outw( cx, VBE_DISPI_IOPORT_DATA, VBE_DISPI_ENABLED | VBE_DISPI_8BIT_DAC );
    }

    vgarg = mode->vgaregs;

    /* Program misc. output register. */
    vid_outb( cx, VGA_MISC_OUT_W, vgarg->misc );

    /* Program the sequencer. */
    vid_wridx_s( cx, VGA_SEQUENCER, sizeof( vgarg->seq ), vgarg->seq );
    vid_wridx( cx, VGA_SEQUENCER, VGA_SR_RESET, VGA_SR0_NORESET );

    /* Program the CRTC and graphics controller. */
    vid_wridx_s( cx, VGA_CRTC, sizeof( vgarg->crtc ), vgarg->crtc );
    vid_wridx_s( cx, VGA_GRAPH_CNTL, sizeof( vgarg->gctl ), vgarg->gctl );

    /* Finally program the attribute controller. */
    vid_wratc_s( cx, sizeof( vgarg->atr ), vgarg->atr );
    vid_outb( cx, VGA_ATTR_W, 0x20 );               /* Re-enable palette. */

    return( 0 );
}

/* Program the DAC. Each of the 'count' entries is 4 bytes in size,
 * red/green/blue/unused.
 * Returns non-zero on failure.
 */
int BOXV_dac_set( void *cx, unsigned start, unsigned count, void *pal )
{
    v_byte      *prgbu = pal;

    /* Basic argument validation. */
    if( start + count > 256 )
        return( -1 );

    /* Write the starting index. */
    vid_outb( cx, VGA_DAC_W_INDEX, start );
    /* Load the RGB data. */
    while( count-- ) {
        vid_outb( cx, VGA_DAC_DATA, *prgbu++ );
        vid_outb( cx, VGA_DAC_DATA, *prgbu++ );
        vid_outb( cx, VGA_DAC_DATA, *prgbu++ );
        ++prgbu;
    }
    return( 0 );
}

/* Detect the presence of a supported adapter and amount of installed
 * video memory. Returns zero if not found.
 */
int BOXV_detect( void *cx, unsigned long *vram_size )
{
    v_word      boxv_id;

    vid_outw( cx, VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_ID );
    boxv_id = vid_inw( cx, VBE_DISPI_IOPORT_DATA );
    if( vram_size ) {
        *vram_size = vid_ind( cx, VBE_DISPI_IOPORT_DATA );
    }
    if( boxv_id >= VBE_DISPI_ID0 && boxv_id <= VBE_DISPI_ID4 )
        return( boxv_id );
    else
        return( 0 );
}

/* Disable extended mode and place the hardware into a VGA compatible state.
 * Returns non-zero on failure.
 */
int BOXV_ext_disable( void *cx )
{
    /* Disable the extended display registers. */
    vid_outw( cx, VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_ENABLE );
    vid_outw( cx, VBE_DISPI_IOPORT_DATA, VBE_DISPI_DISABLED );
    return( 0 );
}
