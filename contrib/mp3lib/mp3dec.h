/******************************************************************************
*
* Public interface to the mp3 decoder library
*
******************************************************************************/

#ifndef MP3_LIB_INCLUDED
#define MP3_LIB_INCLUDED

#define OK         0
#define ERROR     -1
#define TRUE       1
#define FALSE      0

typedef float         FLOAT32;

/* Include files */
#include <stdio.h>
#include <math.h>
#include <inttypes.h>

#define ERR(str, ...) { fprintf (stderr, str, ## __VA_ARGS__) ; fprintf (stderr, "\n"); }

extern int      MPG_Stream_Open( const char *filename );
extern int      MPG_Read_Frame( void );
extern int      MPG_Read_Header( void );
extern int      MPG_Read_CRC( void );
extern int      MPG_Read_Audio_L3( void );
extern int      MPG_Read_Main_L3( void );
extern void     MPG_Read_Ancillary( void );
extern uint32_t MPG_Get_Side_Bits( uint32_t number_of_bits );
extern uint32_t MPG_Get_Main_Bits( uint32_t number_of_bits );
extern uint32_t MPG_Get_Main_Bit( void );
extern int      MPG_Set_Main_Pos( uint32_t bit_pos );
extern uint32_t MPG_Get_Main_Pos( void );
extern int      MPG_Decode_L3( uint32_t out[2 * 576] );
extern void     MPG_Decode_L3_Init_Song( void );

/* Local functions and variables (defined here, used here) */

/* Global definitions */

#define C_MPG_SYNC              0xfff00000
#define C_MPG_EOF               0xffffffff

#define C_PI                    3.14159265358979323846
#define C_INV_SQRT_2            0.70710678118654752440


/* Types used in the frame header */

/* Layer number */
typedef enum {
  mpeg1_layer_reserved = 0,
  mpeg1_layer_3        = 1,
  mpeg1_layer_2        = 2,
  mpeg1_layer_1        = 3
} t_mpeg1_layer;

/* Modes */
typedef enum {
    mpeg1_mode_stereo = 0,
    mpeg1_mode_joint_stereo,
    mpeg1_mode_dual_channel,
    mpeg1_mode_single_channel
} t_mpeg1_mode;

/* Bitrate table for all three layers.  */
extern uint32_t g_mpeg_bitrates[2][3 /* layer 1-3 */][15 /* header bitrate_index */];

/* Sampling frequencies in hertz (valid for all layers) */
extern uint32_t g_sampling_frequency[2][3];

/* MPEG1 Layer 1-3 frame header */
typedef struct {
    uint32_t id;                        /* 1 bit */
    t_mpeg1_layer layer;                /* 2 bits */
    uint32_t protection_bit;            /* 1 bit */
    uint32_t bitrate_index;             /* 4 bits */
    uint32_t sampling_frequency;        /* 2 bits */
    uint32_t padding_bit;               /* 1 bit */
    uint32_t private_bit;               /* 1 bit */
    t_mpeg1_mode mode;                  /* 2 bits */
    uint32_t mode_extension;            /* 2 bits */
    uint32_t copyright;                 /* 1 bit */
    uint32_t original_or_copy;          /* 1 bit */
    uint32_t emphasis;                  /* 2 bits */
} t_mpeg1_header;

/* MPEG1 Layer 3 Side Information */
/* [2][2] means [gr][ch] */
typedef struct {
    uint32_t main_data_begin;           /* 9 bits */
    uint32_t private_bits;              /* 3 bits in mono, 5 in stereo */
    uint32_t scfsi[2][4];               /* 1 bit */
    uint32_t part2_3_length[2][2];      /* 12 bits */
    uint32_t big_values[2][2];          /* 9 bits */
    uint32_t global_gain[2][2];         /* 8 bits */
    uint32_t scalefac_compress[2][2];   /* 4 bits */
    uint32_t win_switch_flag[2][2];     /* 1 bit */
    /* if (win_switch_flag[][]) */
    uint32_t block_type[2][2];          /* 2 bits */
    uint32_t mixed_block_flag[2][2];    /* 1 bit */
    uint32_t table_select[2][2][3];     /* 5 bits */
    uint32_t subblock_gain[2][2][3];    /* 3 bits */
    /* else */
    /* table_select[][][] */
    uint32_t region0_count[2][2];       /* 4 bits */
    uint32_t region1_count[2][2];       /* 3 bits */
    /* end */
    uint32_t preflag[2][2];             /* 1 bit */
    uint32_t scalefac_scale[2][2];      /* 1 bit */
    uint32_t count1table_select[2][2];  /* 1 bit */
    uint32_t count1[2][2];              /* Not in file, calc. by huff.dec.! */
} t_mpeg1_side_info;

/* MPEG1 Layer 3 Main Data */
typedef struct {
    uint32_t    scalefac_l[2][2][21];   /* 0-4 bits */
    uint32_t    scalefac_s[2][2][12][3];/* 0-4 bits */
    int32_t     isi[2][2][576];         /* Huffman coded freq. lines */
    FLOAT32     is[2][2][576];          /* Huffman coded freq. lines */
} t_mpeg1_main_data;

/* Scale factor band indices, for long and short windows */
typedef struct  {
    uint32_t    l[23];
    uint32_t    s[14];
} t_sf_band_indices;

/* Global variables */
extern t_mpeg1_header     g_frame_header;
extern t_mpeg1_side_info  g_side_info;
extern t_mpeg1_main_data  g_main_data;
extern t_sf_band_indices  g_sf_band_indices[3];
extern FLOAT32            g_synth_dtbl[512];

extern uint32_t g_main_data_vec[];
extern uint32_t *g_main_data_ptr;   /* Pointer into the reservoir */
extern uint32_t g_main_data_idx;    /* Index into the current byte (0-7) */
extern uint32_t g_main_data_top;    /* Number of bytes in reservoir (0-1024) */

#endif /* MP3_LIB_INCLUDED */
