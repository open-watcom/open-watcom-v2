/******************************************************************************
*
* Layer III decoding routines.
*
******************************************************************************/

/* Include files */
#include <string.h>
#include "mp3dec.h"
#include "internal.h"

/* Local functions and variables (defined here, used here) */

#define POW34_TABLE
/*  #define POW34_ITERATE   */
#if !defined(POW34_TABLE) && !defined(POW34_ITERATE)
#error "Must define one of POW34_TABLE and POW34_ITERATE!"
#endif

static void MPG_L3_Requantize( uint32_t gr, uint32_t ch );
static void MPG_Requantize_Process_Long( uint32_t gr, uint32_t ch,
                     uint32_t is_pos, uint32_t sfb );
static void MPG_Requantize_Process_Short( uint32_t gr, uint32_t ch, uint32_t is_pos,
                      uint32_t sfb, uint32_t win );
static void MPG_L3_Reorder( uint32_t gr, uint32_t ch );
static void MPG_L3_Stereo( uint32_t gr );
static void MPG_Stereo_Process_Intensity_Long( uint32_t gr, uint32_t sfb );
static void MPG_Stereo_Process_Intensity_Short (uint32_t gr, uint32_t sfb);
static void MPG_L3_Antialias( uint32_t gr, uint32_t ch );
static void MPG_L3_Hybrid_Synthesis( uint32_t gr, uint32_t ch );
static void MPG_IMDCT_Win( FLOAT32 in[18], FLOAT32 out[36], uint32_t block_type );
static void MPG_IMDCT_Short( FLOAT32 invec[6], FLOAT32 outvec[12] );
static void MPG_IMDCT_Long( FLOAT32 invec[18], FLOAT32 outvec[36] );
static void MPG_L3_Frequency_Inversion( uint32_t gr, uint32_t ch );
static void MPG_L3_Subband_Synthesis( uint32_t gr, uint32_t ch,
                      uint32_t outdata[576] );

static uint32_t hsynth_init = 1;
static uint32_t synth_init = 1;

FLOAT32 twiddle_tab[37][18];
FLOAT32 powtab34[8207];



static void MPG_DCT_2pt( FLOAT32 in[2], FLOAT32 out[2] )
{
    /*int i, j;

    for( i = 0; i < 2; i++ ) {
        out[i] = 0.0;
        for( j = 0; j < 2; j++ ) {
            out[i] += in[j] * cos( (2*j+1) * i * (C_PI/4.0) );
        }
    }*/

    out[0] = in[0] + in[1];
    out[1] = (in[0] - in[1]) * 0.707106781;
}


static void MPG_IMDCT_3pt( FLOAT32 in[3], FLOAT32 out[3] )
{
    FLOAT32 t0, t1;


    t0 = in[2]/2.0 + in[0];         /* 2 flop */
    t1 = in[1] * 0.866025403; //(sqrt(3) / 2.0); /* 1 flop */

    out[0] = t0 + t1;               /* 1 flop */
    out[1] = in[0] - in[2];         /* 1 flop */
    out[2] = t0 - t1;               /* 1 flop */

    /* Total of 6 flop */
}


static void MPG_IMDCT_4pt( FLOAT32 in[4], FLOAT32 out[4] )
{
    FLOAT32 t0, t1;


    t0 = in[3]/2.0 + in[0];         /* 2 flop */
    t1 = in[1] - in[2];             /* 1 flop */

    out[0] = t0 + in[1] * 0.93969262 + in[2] * 0.766044443; /* 4 flop */
    out[1] = t1/2.0 + in[0] - in[3]; /* 3 flop */
    out[2] = t0 - in[1] * 0.173648177 - in[2] * 0.93969262; /* 4 flop */
    out[3] = t0 - in[1] * 0.766044443 + in[2] * 0.173648177; /* 4 flop */

    /* Total of 18 flop */
}


static void MPG_IMDCT_5pt( FLOAT32 in[5], FLOAT32 out[5] )
{
    FLOAT32 t0, t1, t2;

    t0 = in[3]/2.0 + in[0];         /* 2 flop */
    t1 = in[0] - in[3];             /* 1 flop */
    t2 = in[1] - in[2] - in[4];     /* 2 flop */

    out[0] = t0 + in[1] * 0.93969262 + in[2] * 0.766044443 +
            in[4] * 0.173648177;    /* 6 flop */

    out[1] = t2/2.0 + t1;           /* 2 flop */

    out[2] = t0 - in[1] * 0.173648177 - in[2] * 0.93969262 +
            in[4] * 0.766044443;    /* 6 flop */

    out[3] = t0 - in[1] * 0.766044443 + in[2] * 0.173648177 -
            in[4] * 0.93969262;     /* 6 flop */

    out[4] = t1 - t2;               /* 1 flop */

    /* Total of 26 flop */
}


static void MPG_IMDCT_9pt( FLOAT32 invec[9], FLOAT32 outvec[9] )
{
    int     i;
    FLOAT32 H[9], h[9], even[5], odd[4], even_idct[5], odd_idct[4];

    for( i = 0; i < 9; i++ ) {
        H[i] = invec[i];
    }

    for( i = 0; i < 5; i++ ) {
        even[i] = H[2*i];
    }

    MPG_IMDCT_5pt (even, even_idct);   /* 26 flop */

    odd[0] = H[1];
    for( i = 1; i < 4; i++ ) {
        odd[i] = H[2*i-1] + H[2*i+1];   /* 3 flop */
    }

    MPG_IMDCT_4pt( odd, odd_idct );     /* 18 flop */

    /* Adjust for non power of 2 IDCT */
    odd_idct[0] +=  invec[7] * 0.173648177; //sin ((2*0+1)*(C_PI/18.0));
    odd_idct[1] += -invec[7] * 0.5; //sin ((2*1+1)*(C_PI/18.0));
    odd_idct[2] +=  invec[7] * 0.766044443; //sin ((2*2+1)*(C_PI/18.0));
    odd_idct[3] += -invec[7] * 0.93969262; //sin ((2*3+1)*(C_PI/18.0)); /* Total 4 flop */

    /* Post-Twiddle */
    for( i = 0; i < 4; i++ ) {
        odd_idct[i] *= twiddle_tab[9][i]; /* Total 4 flop */
    }

    for( i = 0; i < 4; i++ ) {
        h[i] = even_idct[i] + odd_idct[i]; /* Total 4 flop */
    }

    h[4] = even_idct[4];

    for( i = 5; i < 9; i++ ) {
        h[i] = even_idct[8-i] - odd_idct[8-i]; /* Total 4 flop */
    }

    for( i = 0; i < 9; i++ ) {
        outvec[i] = h[i];
    }
    /* Total: 63 flop */
}


static void MPG_DCT( FLOAT32 in[], FLOAT32 out[], int N )
{
    int i;
    //FLOAT32 even_in[N/2], even_out[N/2], odd_in[N/2], odd_out[N/2];
    FLOAT32 even_in[16], even_out[16], odd_in[16], odd_out[16];


    /* We use recursion here to make the function easier to understand.
     * It should be unrolled manually in order to make it more effective for
     * a real implementation.
     */
    if( N == 2 ) {
        MPG_DCT_2pt( in, out );
        return;
    }

    for( i = 0; i < N/2; i++ ) {
        even_in[i] = in[i] + in[N-1-i]; /* N/2 flop */
    }

    MPG_DCT( even_in, even_out, N/2 );  /* DCT(N/2) flop */

    for (i = 0; i < N / 2; i++) {
        odd_in[i] = (in[i] - in[N-1-i]) * twiddle_tab[N][i]; /* N flop */
    }

    MPG_DCT( odd_in, odd_out, N/2 );    /* DCT(N/2) flop */

    for( i = 0; i < N/2; i++ ) {
        out[2*i] = even_out[i];
    }

    for( i = 0; i < N/2 - 1; i++ ) {
        out[2*i+1] = odd_out[i] + odd_out[i+1]; /* N/2 - 1 flop */
    }
    out[N-1] = odd_out[N/2-1];

    /* Total of 2*N - 1 + 2*DCT(N/2) flop */
}


static void MPG_Polyphase_Matrixing( FLOAT32 invec[32], FLOAT32 outvec[64] )
{
    int i;
    FLOAT32 tmp[32];


    MPG_DCT (invec, tmp, 32);

    for( i = 0; i < 16; i++ ) {
        outvec[i] = tmp[i+16];
    }

    outvec[16] = 0.0;

    for( i = 17; i < 32; i++ ) {
        outvec[i] = -tmp[48-i];
    }

    for( i = 32; i < 48; i++ ) {
        outvec[i] = -tmp[48-i];
    }

    for( i = 48; i < 64; i++ ) {
        outvec[i] = -tmp[i-48];
    }
}


/******************************************************************************
*
* Description: This function is used to reinit the decoder before playing a new
*              song, or when seeking inside the current song.
*
******************************************************************************/

/* This macro is used by both the short and long IMDCTs */
#define POST_TWIDDLE(i,N) (1.0 / (2.0 * cos ((2*i+1) * (C_PI / (2*N)))))

void MPG_Decode_L3_Init_Song( void )
{
    int         i, j;
    static int  init = 1;

    hsynth_init = 1;
    synth_init = 1;
    g_main_data_top = 0;        /* Clear bit reservoir */

// TODO: this needs to go into a library init function
    if( init ) {
        for( i = 0; i <= 36; i++ )
            for( j = 0; j < i/2; j++ )
                twiddle_tab[i][j] = POST_TWIDDLE( j, i );

        for( i = 0; i < 8207; i++ ) {
            powtab34[i] = pow( (FLOAT32) i, 4.0 / 3.0 );
        }
        init = 0;
    }
}


/******************************************************************************
*
* Description: This function decodes a Layer III frame into PCM samples.
* Parameters: Outdata vector.
* Return value: OK or ERROR if the frame contains errors.
*
******************************************************************************/
int MPG_Decode_L3( uint32_t out[2 * 576] )
{
    uint32_t    gr, ch, nch;


    /* Number of channels (1 for mono and 2 for stereo) */
    nch = g_frame_header.mode == mpeg1_mode_single_channel ? 1 : 2;

    for( gr = 0; gr < 2; gr++ ) {

        for( ch = 0; ch < nch; ch++ ) {
            /* Requantize samples */
            MPG_L3_Requantize( gr, ch );
            /* Reorder short blocks */
            MPG_L3_Reorder( gr, ch );
        } /* end for (ch... */

        /* Stereo processing */
        MPG_L3_Stereo( gr );

        for( ch = 0; ch < nch; ch++ ) {
            /* Antialias */
            MPG_L3_Antialias( gr, ch );
            /* Hybrid synthesis (IMDCT, windowing, overlapp add) */
            MPG_L3_Hybrid_Synthesis( gr, ch );
            /* Frequency inversion */
            MPG_L3_Frequency_Inversion( gr, ch );
            /* Polyphase subband synthesis */
            MPG_L3_Subband_Synthesis( gr, ch, out + 576 * gr );
        } /* end for (ch... */
    } /* end for (gr... */

    /* Done */
    return( OK );
}


static void MPG_L3_Requantize( uint32_t gr, uint32_t ch )
{
    uint32_t    sfb /* scalefac band index */;
    uint32_t    next_sfb /* frequency of next sfb */;
    uint32_t    sfreq, i, j, win, win_len;


    /* Setup sampling frequency index */
    sfreq = g_frame_header.sampling_frequency;

    /* Determine type of block to process */
    if( (g_side_info.win_switch_flag[gr][ch] == 1) &&
            (g_side_info.block_type[gr][ch] == 2) ) {   /* Short blocks */

        /* Check if the first two subbands
         * (=2*18 samples = 8 long or 3 short sfb's) uses long blocks */
        if( g_side_info.mixed_block_flag[gr][ch] != 0 ) {   /* 2 longbl. sb  first */

            /*
             * First process the 2 long block subbands at the start
             */
            sfb = 0;
            next_sfb = g_sf_band_indices[sfreq].l[sfb+1];
            for( i = 0; i < 36; i++ ) {
                if( i == next_sfb ) {
                    sfb++;
                    next_sfb = g_sf_band_indices[sfreq].l[sfb+1];
                } /* end if */
                MPG_Requantize_Process_Long( gr, ch, i, sfb );
            }

            /*
             * And next the remaining, non-zero, bands which uses short blocks
             */
            sfb = 3;
            next_sfb = g_sf_band_indices[sfreq].s[sfb+1] * 3;
            win_len = g_sf_band_indices[sfreq].s[sfb+1] -
            g_sf_band_indices[sfreq].s[sfb];

            for( i = 36; i < g_side_info.count1[gr][ch]; /* i++ done below! */ ) {
                /* Check if we're into the next scalefac band */
                if( i == next_sfb ) {    /* Yes */
                    sfb++;
                    next_sfb = g_sf_band_indices[sfreq].s[sfb+1] * 3;
                    win_len = g_sf_band_indices[sfreq].s[sfb+1] -
                    g_sf_band_indices[sfreq].s[sfb];
                } /* end if (next_sfb) */

                for( win = 0; win < 3; win++ ) {
                    for( j = 0; j < win_len; j++ ) {
                        MPG_Requantize_Process_Short( gr, ch, i, sfb, win );
                        i++;
                    }
                }
            }
        } else {            /* Only short blocks */
            sfb = 0;
            next_sfb = g_sf_band_indices[sfreq].s[sfb+1] * 3;
            win_len = g_sf_band_indices[sfreq].s[sfb+1] -
            g_sf_band_indices[sfreq].s[sfb];

            for( i = 0; i < g_side_info.count1[gr][ch]; /* i++ done below! */ ) {
                /* Check if we're into the next scalefac band */
                if( i == next_sfb ) {   /* Yes */
                    sfb++;
                    next_sfb = g_sf_band_indices[sfreq].s[sfb+1] * 3;
                    win_len = g_sf_band_indices[sfreq].s[sfb+1] -
                        g_sf_band_indices[sfreq].s[sfb];
                } /* end if (next_sfb) */

                for( win = 0; win < 3; win++ ) {
                    for( j = 0; j < win_len; j++ ) {
                        MPG_Requantize_Process_Short( gr, ch, i, sfb, win );
                        i++;
                    }
                }
            }
        } /* end else (only short blocks) */
    } else {          /* Only long blocks */
        sfb = 0;
        next_sfb = g_sf_band_indices[sfreq].l[sfb+1];
        for( i = 0; i < g_side_info.count1[gr][ch]; i++ ) {
            if( i == next_sfb ) {
                sfb++;
                next_sfb = g_sf_band_indices[sfreq].l[sfb+1];
            } /* end if */
            MPG_Requantize_Process_Long( gr, ch, i, sfb );
        }
    } /* end else (only long blocks) */

    //for (; i < 576; i++) g_main_data.is[gr][ch][i] = 0.0;
    if( i < 576 )
        memset( &g_main_data.is[gr][ch][i], 0, (576-i)*4 );
}


/******************************************************************************
*
* Description: This function is used to calculate y=x^(4/3) when requantizing
*              samples.
*
******************************************************************************/
static FLOAT32 MPG_Requantize_Pow_43( uint32_t is_pos )
{

#ifdef POW34_TABLE
    static uint32_t init = 0;
    uint32_t    i;


    /* First time initialization */
    if( init == 0 ) {
        for( i = 0; i < 8207; i++ ) {
            powtab34[i] = pow( (FLOAT32) i, 4.0 / 3.0 );
        }
        init = 1;
    }

    //  assert( is_pos <= 8206 );

    /* Done */
    return( powtab34[is_pos] );

#endif /* POW34_TABLE */


#ifdef POW34_ITERATE
    FLOAT32             a4, a2;
    FLOAT32             x, x2, x3, x_next, is_f1, is_f2, is_f3;
    uint32_t            i;
    static uint32_t     init = 0;
    static FLOAT32      coeff[3] = {
        -1.030797119e+02,
        6.319399834e+00,
        2.395095071e-03,
    };


// TODO: this needs to go into a library init function

    /* First time initialization */
    if( init == 0 ) {
        for( i = 0; i < 32; i++ ) {
            powtab34[i] = pow( (FLOAT32) i, 4.0 / 3.0 );
        }
        init = 1;
    }

    /* We use a table for 0<is_pos<32 since they are so common */
    if( is_pos < 32 )
        return( powtab34[is_pos] );

    a2 = is_pos * is_pos;
    a4 = a2 * a2;

    is_f1 = (float)is_pos;
    is_f2 = is_f1 * is_f1;
    is_f3 = is_f1 * is_f2;

    /*  x = coeff[0] + coeff[1]*is_f1 + coeff[2]*is_f2 + coeff[3]*is_f3; */
    x = coeff[0] + coeff[1]*is_f1 + coeff[2]*is_f2;

    for( i = 0; i < 3; i++ ) {
        x2 = x*x;
        x3 = x*x2;

        x_next = (2*x3 + a4) / (3*x2);

        x = x_next;
    }

    return (x);
#endif /* POW34_ITERATE */
}


/******************************************************************************
*
* Description: This function is used to requantize a sample in a subband
*              that uses long blocks.
*
******************************************************************************/
static void MPG_Requantize_Process_Long( uint32_t gr, uint32_t ch,
                                         uint32_t is_pos, uint32_t sfb)
{
    FLOAT32             res, sf_mult, pf_x_pt;
    static FLOAT32      tmp1, tmp2, tmp12, tmp3;
    static uint32_t     lastsfb;
    static FLOAT32      pretab[21] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                       1, 1, 1, 1, 2, 2, 3, 3, 3, 2 };


    /*tmp1 =
    pow (2.0, -(sf_mult * (g_main_data.scalefac_l[gr][ch][sfb] + pf_x_pt)));

    tmp2 = pow (2.0, 0.25 * ((int32_t) g_side_info.global_gain[gr][ch] - 210));*/

    if( is_pos == 0 )
        tmp2 = exp( 0.25 * ((int32_t)g_side_info.global_gain[gr][ch] - 210) * 0.69314718 );

    if( (is_pos == 0) || (sfb != lastsfb) ) {
        sf_mult = g_side_info.scalefac_scale[gr][ch] ? 1.0 : 0.5;
        pf_x_pt = g_side_info.preflag[gr][ch] * pretab[sfb];

        tmp1 = exp (-(sf_mult * (g_main_data.scalefac_l[gr][ch][sfb] + pf_x_pt)) * 0.69314718);
        tmp12 = tmp1 * tmp2;
    }

    if( g_main_data.isi[gr][ch][is_pos] < 0 ) {
        tmp3 = -powtab34[-g_main_data.isi[gr][ch][is_pos]];
    } else {
        tmp3 = powtab34[g_main_data.isi[gr][ch][is_pos]];
    }

    res = g_main_data.is[gr][ch][is_pos] = tmp12 * tmp3;
    lastsfb = sfb;
}


/******************************************************************************
*
* Description: This function is used to requantize a sample in a subband
*              that uses short blocks.
*
******************************************************************************/
static void MPG_Requantize_Process_Short( uint32_t gr, uint32_t ch,
                        uint32_t is_pos, uint32_t sfb, uint32_t win)
{
    FLOAT32     res, tmp1, tmp2, tmp3, sf_mult;

    sf_mult = g_side_info.scalefac_scale[gr][ch] ? 1.0 : 0.5;

    tmp1 = pow( 2.0, -(sf_mult * g_main_data.scalefac_s[gr][ch][sfb][win]) );
    tmp2 = pow( 2.0, 0.25 * ((FLOAT32) g_side_info.global_gain[gr][ch] - 210.0 -
              8.0 * (FLOAT32) g_side_info.subblock_gain[gr][ch][win]) );

    if( g_main_data.isi[gr][ch][is_pos] < 0 ) {
        tmp3 = -MPG_Requantize_Pow_43( -g_main_data.isi[gr][ch][is_pos] );
    } else {
        tmp3 = MPG_Requantize_Pow_43( g_main_data.isi[gr][ch][is_pos] );
    }

    res = g_main_data.is[gr][ch][is_pos] = tmp1 * tmp2 * tmp3;
}


static void MPG_L3_Reorder( uint32_t gr, uint32_t ch )
{
    uint32_t    sfreq, i, j, next_sfb, sfb, win_len, win;
    FLOAT32     re[576];


    /* Setup sampling frequency index */
    sfreq = g_frame_header.sampling_frequency;

    /* Only reorder short blocks */
    if( (g_side_info.win_switch_flag[gr][ch] == 1) &&
        (g_side_info.block_type[gr][ch] == 2) ) { /* Short blocks */

        /* Check if the first two subbands
         * (=2*18 samples = 8 long or 3 short sfb's) uses long blocks */
        if( g_side_info.mixed_block_flag[gr][ch] != 0 ) { /* 2 longbl. sb  first */

            /* Don't touch the first 36 samples */

            /*
             * Reorder the remaining, non-zero, bands which uses short blocks
             */
            sfb = 3;
            next_sfb = g_sf_band_indices[sfreq].s[sfb+1] * 3;
            win_len = g_sf_band_indices[sfreq].s[sfb+1] -
                      g_sf_band_indices[sfreq].s[sfb];
        } else { /* Only short blocks */
            sfb = 0;
            next_sfb = g_sf_band_indices[sfreq].s[sfb+1] * 3;
            win_len = g_sf_band_indices[sfreq].s[sfb+1] -
                      g_sf_band_indices[sfreq].s[sfb];
        } /* end else (only short blocks) */

        for( i = ((sfb == 0) ? 0 : 36); i < 576; /* i++ done below! */ ) {
            /* Check if we're into the next scalefac band */
            if( i == next_sfb ) {  /* Yes */

                /* Copy reordered data back to the original vector */
                for( j = 0; j < 3*win_len; j++ ) {
                    g_main_data.is[gr][ch][3*g_sf_band_indices[sfreq].s[sfb] + j] = re[j];
                }

                /* Check if this band is above the rzero region, if so we're done */
                if( i >= g_side_info.count1[gr][ch] ) {
                    /* Done */
                    return;
                }

                sfb++;
                next_sfb = g_sf_band_indices[sfreq].s[sfb+1] * 3;
                win_len = g_sf_band_indices[sfreq].s[sfb+1] -
                          g_sf_band_indices[sfreq].s[sfb];

            } /* end if (next_sfb) */

            /* Do the actual reordering */
            for( win = 0; win < 3; win++ ) {
                for( j = 0; j < win_len; j++ ) {
                    re[j*3 + win] = g_main_data.is[gr][ch][i];
                    i++;
                }
            }
        }   /* end for (i... */

        /* Copy reordered data of the last band back to the original vector */
        for( j = 0; j < 3*win_len; j++ ) {
            g_main_data.is[gr][ch][3 * g_sf_band_indices[sfreq].s[12] + j] = re[j];
        }
    } else {          /* Only long blocks */
        /* No reorder necessary, do nothing! */
        return;
    }
}


static void MPG_L3_Stereo( uint32_t gr )
{
    uint32_t    max_pos, i;
    FLOAT32     left, right;
    uint32_t    sfb /* scalefac band index */;
    uint32_t    sfreq;

    /* Do nothing if joint stereo is not enabled */
    if( (g_frame_header.mode != 1) || (g_frame_header.mode_extension == 0) ) {
        return;
    }

    /* Do Middle/Side ("normal") stereo processing */
    if( g_frame_header.mode_extension & 0x2 ) {
        /* Determine how many frequency lines to transform */
        if( g_side_info.count1[gr][0] > g_side_info.count1[gr][1] ) {
            max_pos = g_side_info.count1[gr][0];
        } else {
            max_pos = g_side_info.count1[gr][1];
        }

        /* Do the actual processing */
        for( i = 0; i < max_pos; i++ ) {
            left = (g_main_data.is[gr][0][i] + g_main_data.is[gr][1][i])
                * (C_INV_SQRT_2);
            right = (g_main_data.is[gr][0][i] - g_main_data.is[gr][1][i])
                * (C_INV_SQRT_2);
            g_main_data.is[gr][0][i] = left;
            g_main_data.is[gr][1][i] = right;
        }
    } /* end if (ms_stereo... */

    /* Do intensity stereo processing */
    if (g_frame_header.mode_extension & 0x1) {

    /* Setup sampling frequency index */
    sfreq = g_frame_header.sampling_frequency;

    /* The first band that is intensity stereo encoded is the first band
     * scale factor band on or above the count1 frequency line.
     * N.B.: Intensity stereo coding is only done for the higher subbands,
     * but the logic is still included to process lower subbands.
     */

    /* Determine type of block to process */
    if( (g_side_info.win_switch_flag[gr][0] == 1) &&
        (g_side_info.block_type[gr][0] == 2) ) { /* Short blocks */

        /* Check if the first two subbands
         * (=2*18 samples = 8 long or 3 short sfb's) uses long blocks */
        if( g_side_info.mixed_block_flag[gr][0] != 0 ) { /* 2 longbl. sb  first */
            /* First process the 8 sfb's at the start */
            for( sfb = 0; sfb < 8; sfb++ ) {
                /* Is this scale factor band above count1 for the right channel? */
                if( g_sf_band_indices[sfreq].l[sfb] >= g_side_info.count1[gr][1] ) {
                    MPG_Stereo_Process_Intensity_Long( gr, sfb );
                }

            }

            /* And next the remaining bands which uses short blocks */
            for( sfb = 3; sfb < 12; sfb++ ) {
                    /* Is this scale factor band above count1 for the right channel? */
                    if( g_sf_band_indices[sfreq].s[sfb]*3 >= g_side_info.count1[gr][1] ) {
                        /* Perform the intensity stereo processing */
                        MPG_Stereo_Process_Intensity_Short( gr, sfb );
                    }
                }
            } else {          /* Only short blocks */
                for( sfb = 0; sfb < 12; sfb++ ) {
                    /* Is this scale factor band above count1 for the right channel? */
                    if( g_sf_band_indices[sfreq].s[sfb]*3 >= g_side_info.count1[gr][1] ) {
                        /* Perform the intensity stereo processing */
                        MPG_Stereo_Process_Intensity_Short( gr, sfb );
                    }
                }

            } /* end else (only short blocks) */
        } else {            /* Only long blocks */
            for( sfb = 0; sfb < 21; sfb++ ) {
                /* Is this scale factor band above count1 for the right channel? */
                if( g_sf_band_indices[sfreq].l[sfb] >= g_side_info.count1[gr][1] ) {
                    /* Perform the intensity stereo processing */
                    MPG_Stereo_Process_Intensity_Long( gr, sfb );
                }
            }
        }
    } /* end if (intensity_stereo processing) */
}


/******************************************************************************
*
* Description: This function is used to perform intensity stereo processing
*              for an entire subband that uses long blocks.
*
******************************************************************************/
static void MPG_Stereo_Process_Intensity_Long( uint32_t gr, uint32_t sfb )
{
    static  int init = 0;
    static  FLOAT32 is_ratios[6];
    uint32_t    i;
    uint32_t    sfreq;
    uint32_t    sfb_start, sfb_stop;
    uint32_t    is_pos;
    FLOAT32     is_ratio_l, is_ratio_r;
    FLOAT32     left, right;

    /* First-time init */
    if( init == 0 ) {
        init = 1;
        for( i = 0; i < 6; i++ ) {
            is_ratios[i] = tan( (i * C_PI) / 12.0 );
        }
    }

    /* Check that ((is_pos[sfb]=scalefac) != 7) => no intensity stereo */
    if( (is_pos = g_main_data.scalefac_l[gr][0][sfb]) != 7 ) {
        /* Setup sampling frequency index */
        sfreq = g_frame_header.sampling_frequency;

        sfb_start = g_sf_band_indices[sfreq].l[sfb];
        sfb_stop = g_sf_band_indices[sfreq].l[sfb+1];

        /* tan((6*PI)/12 = PI/2) needs special treatment! */
        if( is_pos == 6 ) {
            is_ratio_l = 1.0;
            is_ratio_r = 0.0;
        } else {
            is_ratio_l = is_ratios[is_pos] / (1.0 + is_ratios[is_pos]);
            is_ratio_r = 1.0 / (1.0 + is_ratios[is_pos]);
        }

        /* Now decode all samples in this scale factor band */
        for( i = sfb_start; i < sfb_stop; i++ ) {
            left = is_ratio_l * g_main_data.is[gr][0][i];
            right = is_ratio_r * g_main_data.is[gr][0][i];
            g_main_data.is[gr][0][i] = left;
            g_main_data.is[gr][1][i] = right;
        }
    }
} /* end MPG_Stereo_Process_Intensity_Long() */


/******************************************************************************
*
* Description: This function is used to perform intensity stereo processing
*              for an entire subband that uses short blocks.
*
******************************************************************************/
static void MPG_Stereo_Process_Intensity_Short( uint32_t gr, uint32_t sfb )
{
    FLOAT32   left, right;
    uint32_t  i;
    uint32_t  sfb_start, sfb_stop;
    uint32_t  is_pos, is_ratio_l, is_ratio_r, is_ratios[6];
    uint32_t  sfreq, win, win_len;


    /* Setup sampling frequency index */
    sfreq = g_frame_header.sampling_frequency;

    /* The window length */
    win_len = g_sf_band_indices[sfreq].s[sfb+1] - g_sf_band_indices[sfreq].s[sfb];

    /* The three windows within the band have different scalefactors */
    for( win = 0; win < 3; win++ ) {
        /* Check that ((is_pos[sfb]=scalefac) != 7) => no intensity stereo */
        if( (is_pos = g_main_data.scalefac_s[gr][0][sfb][win]) != 7 ) {
            sfb_start = g_sf_band_indices[sfreq].s[sfb]*3 + win_len*win;
            sfb_stop = sfb_start + win_len;

            /* tan((6*PI)/12 = PI/2) needs special treatment! */
            if( is_pos == 6 ) {
                is_ratio_l = 1.0;
                is_ratio_r = 0.0;
            } else {
                is_ratio_l = is_ratios[is_pos] / (1.0 + is_ratios[is_pos]);
                is_ratio_r = 1.0 / (1.0 + is_ratios[is_pos]);
            }

            /* Now decode all samples in this scale factor band */
            for (i = sfb_start; i < sfb_stop; i++) {
                left = is_ratio_l = g_main_data.is[gr][0][i];
                right = is_ratio_r = g_main_data.is[gr][0][i];
                g_main_data.is[gr][0][i] = left;
                g_main_data.is[gr][1][i] = right;
            }
        }
    }
} /* end MPG_Stereo_Process_Intensity_Short() */


static void MPG_L3_Antialias( uint32_t gr, uint32_t ch )
{
    static FLOAT32 cs[8], ca[8];
    static FLOAT32 ci[8] = { -0.6,   -0.535, -0.33,   -0.185,
               -0.095, -0.041, -0.0142, -0.0037 };
    static uint32_t init = 1;

    uint32_t    sb /* subband of 18 samples */, i, sblim, ui, li;
    FLOAT32     ub, lb;

    if( init ) {
        for( i = 0; i < 8; i++ ) {
            cs[i] = 1.0 / sqrt( 1.0 + ci[i]*ci[i] );
            ca[i] = ci[i] / sqrt( 1.0 + ci[i]*ci[i] );
        }
        init = 0;
    }

    /* No antialiasing is done for short blocks */
    if ((g_side_info.win_switch_flag[gr][ch] == 1) &&
        (g_side_info.block_type[gr][ch] == 2) &&
        (g_side_info.mixed_block_flag[gr][ch]) == 0 ) {

        return;
    }

    /* Setup the limit for how many subbands to transform */
    if( (g_side_info.win_switch_flag[gr][ch] == 1) &&
        (g_side_info.block_type[gr][ch] == 2) &&
        (g_side_info.mixed_block_flag[gr][ch]) == 1 ) {

        sblim = 2;
    } else {
        sblim = 32;
    }

    /* Do the actual antialiasing */
    for( sb = 1; sb < sblim; sb++ ) {
        for( i = 0; i < 8; i++ ) {
            li = 18*sb-1-i;
            ui = 18*sb+i;
            lb = g_main_data.is[gr][ch][li]*cs[i] - g_main_data.is[gr][ch][ui]*ca[i];
            ub = g_main_data.is[gr][ch][ui]*cs[i] + g_main_data.is[gr][ch][li]*ca[i];
            g_main_data.is[gr][ch][li] = lb;
            g_main_data.is[gr][ch][ui] = ub;
        }
    }
}


static void MPG_L3_Hybrid_Synthesis( uint32_t gr, uint32_t ch )
{
    uint32_t sb, i, j, bt;
    FLOAT32 rawout[36];
    static FLOAT32 store[2][32][18];


    if( hsynth_init ) {
        /* Clear stored samples vector */
        for( j = 0; j < 2; j++ ) {
            for( sb = 0; sb < 32; sb++ ) {
                for( i = 0; i < 18; i++ ) {
                    store[j][sb][i] = 0.0;
                }
            }
        }
        hsynth_init = 0;
    } /* end if (init) */

    /* Loop through all 32 subbands */
    for( sb = 0; sb < 32; sb++ ) {
        /* Determine blocktype for this subband */
        if( (g_side_info.win_switch_flag[gr][ch] == 1) &&
            (g_side_info.mixed_block_flag[gr][ch] == 1) && (sb < 2) ) {

            bt = 0;           /* Long blocks in first 2 subbands */
        } else {
            bt = g_side_info.block_type[gr][ch];
        }

        /* Do the inverse modified DCT and windowing */
        MPG_IMDCT_Win( &(g_main_data.is[gr][ch][sb*18]), rawout, bt );

        /* Overlapp add with stored vector into main_data vector */
        for( i = 0; i < 18; i++ ) {
            g_main_data.is[gr][ch][sb*18 + i] = rawout[i] + store[ch][sb][i];
            store[ch][sb][i] = rawout[i + 18];
        }
    } /* end for (sb... */
}


/******************************************************************************
*
* Description: Does inverse modified DCT and windowing.
*
******************************************************************************/
static void MPG_IMDCT_Win( FLOAT32 in[18], FLOAT32 out[36], uint32_t block_type )
{
    uint32_t    i, m, p;
    FLOAT32     tmp[12];
    FLOAT32     tin[18];
    static FLOAT32  g_imdct_win[4][36];
    static uint32_t init = 1;


// TODO: this needs to go into a library init function
    /* Setup the four (one for each block type) window vectors */
    if( init ) {
        /* Blocktype 0 */
        for( i = 0; i < 36; i++ )
            g_imdct_win[0][i] = sin( C_PI/36 * (i + 0.5) );

        /* Blocktype 1 */
        for( i = 0; i < 18; i++ )
            g_imdct_win[1][i] = sin( C_PI/36 * (i + 0.5) );
        for( i = 18; i < 24; i++ )
            g_imdct_win[1][i] = 1.0;
        for( i = 24; i < 30; i++ )
            g_imdct_win[1][i] = sin( C_PI/12 * (i + 0.5 - 18.0) );
        for( i = 30; i < 36; i++ )
            g_imdct_win[1][i] = 0.0;

        /* Blocktype 2 */
        for( i = 0; i < 12; i++ )
            g_imdct_win[2][i] = sin( C_PI/12 * (i + 0.5) );
        for( i = 12; i < 36; i++ )
            g_imdct_win[2][i] = 0.0;

        /* Blocktype 3 */
        for (i = 0; i < 6; i++)
          g_imdct_win[3][i] = 0.0;
        for( i = 6; i < 12; i++ )
            g_imdct_win[3][i] = sin( C_PI/12 * (i + 0.5 - 6.0) );
        for( i = 12; i < 18; i++ )
            g_imdct_win[3][i] = 1.0;
        for( i = 18; i < 36; i++ )
            g_imdct_win[3][i] = sin( C_PI/36 * (i + 0.5) );

        init = 0;

    } /* end of init */

    if( block_type == 2 ) {    /* 3 short blocks */
        for( i = 0; i < 36; i++ ) {
            out[i] = 0.0;
        }

        /* The short blocks input vector has to be re-arranged */
        for( i = 0; i < 3; i++ ) {
            for( m = 0; m < 6; m++ ) {
                tin[i*6+m] = in[i+3*m];
            }
        }

        for( i = 0; i < 3; i++ ) {
            MPG_IMDCT_Short( &tin[6*i], tmp );

            /* The three short blocks must be windowed and overlapped added
             * with each other */
            for( p = 0; p < 12; p++ ) {
                out[6*i+p+6] += tmp[p] * g_imdct_win[block_type][p];
            }

        }
    } else { /* block_type != 2 */
        MPG_IMDCT_Long( in, out );

        /* Perform the windowing. This could be inlined in the MPG_IMDCT_Long()
         * function. */
        for( i = 0; i < 36; i++ ) {
            out[i] *= g_imdct_win[block_type][i];
        }
    }
}


static void MPG_IMDCT_Short( FLOAT32 invec[6], FLOAT32 outvec[12] )
{
    int     i;
    FLOAT32 H[6], h[6], even[3], odd[3], even_idct[3], odd_idct[3];


    /* Preprocess the input to the two 3-point IDCT's */
    H[0] = invec[0];
    for( i = 1; i < 6; i++ ) {
        H[i] = invec[i-1] + invec[i]; /* 5 flop */
    }

    even[0] = H[0];
    even[1] = H[2];
    even[2] = H[4];

    MPG_IMDCT_3pt( even, even_idct );   /* 6 flop */

    odd[0] = H[1];
    odd[1] = H[1] + H[3];
    odd[2] = H[3] + H[5];         /* Total 2 flop */

    MPG_IMDCT_3pt( odd, odd_idct );  /* 6 flop */

    /* Post-Twiddle */
    odd_idct[0] *= twiddle_tab[6][0];
    odd_idct[1] *= twiddle_tab[6][1];
    odd_idct[2] *= twiddle_tab[6][2];

    h[0] = even_idct[0] + odd_idct[0];
    h[1] = even_idct[1] + odd_idct[1];
    h[2] = even_idct[2] + odd_idct[2];

    h[3] = even_idct[2] - odd_idct[2];
    h[4] = even_idct[1] - odd_idct[1];
    h[5] = even_idct[0] - odd_idct[0]; /* Total: 9 flop */

    for( i = 0; i < 6; i++ ) {
        h[i] *= twiddle_tab[12][i]; /* 6 flop */
    }

    /* Total: 5+6+2+6+9+6=34 flop */

    /* Rearrange the 6 values from the IDCT to the output vector */
    outvec[0]  =  h[3];
    outvec[1]  =  h[4];
    outvec[2]  =  h[5];
    outvec[3]  = -h[5];
    outvec[4]  = -h[4];
    outvec[5]  = -h[3];
    outvec[6]  = -h[2];
    outvec[7]  = -h[1];
    outvec[8]  = -h[0];
    outvec[9]  = -h[0];
    outvec[10] = -h[1];
    outvec[11] = -h[2];
}


void MPG_IMDCT_Long( FLOAT32 invec[18], FLOAT32 outvec[36] )
{
    int     i;
    FLOAT32 H[18], h[18], even[9], odd[9], even_idct[9], odd_idct[9];

    H[0] = invec[0];
    for( i = 1; i < 18; i++ ) {
        H[i] = invec[i-1] + invec[i]; /* 17 flop */
    }

    for( i = 0; i < 9; i++ ) {
        even[i] = H[i*2];
    }

    MPG_IMDCT_9pt( even, even_idct );   /* 63 flop */

    odd[0] = H[1];
    for( i = 1; i < 9; i++ ) {
        odd[i] = H[i*2-1] + H[i*2+1];   /* Total 8 flop */
    }

    MPG_IMDCT_9pt( odd, odd_idct );  /* 63 flop */

    /* Post-Twiddle */
    for( i = 0; i < 9; i++ ) {
        odd_idct[i] *= twiddle_tab[18][i]; /* Total 9 flop */
    }

    for( i = 0; i < 9; i++ ) {
        h[i] = even_idct[i] + odd_idct[i]; /* Total 9 flop */
    }

    for( i = 9; i < 18; i++ ) {
        h[i] = even_idct[17-i] - odd_idct[17-i]; /* Total 9 flop */
    }

    for( i = 0; i < 18; i++ ) {
        h[i] *= twiddle_tab[36][i]; /* 18 flop */
    }

    /* Total: 17+63+8+63+9+9+9+18 = 196 flop */

    /* Rearrange the 18 values from the IDCT to the output vector */
    outvec[0]  =  h[9];
    outvec[1]  =  h[10];
    outvec[2]  =  h[11];
    outvec[3]  =  h[12];
    outvec[4]  =  h[13];
    outvec[5]  =  h[14];
    outvec[6]  =  h[15];
    outvec[7]  =  h[16];
    outvec[8]  =  h[17];

    outvec[9]  = -h[17];
    outvec[10] = -h[16];
    outvec[11] = -h[15];
    outvec[12] = -h[14];
    outvec[13] = -h[13];
    outvec[14] = -h[12];
    outvec[15] = -h[11];
    outvec[16] = -h[10];
    outvec[17] = -h[9];

    outvec[18] = -h[8];
    outvec[19] = -h[7];
    outvec[20] = -h[6];
    outvec[21] = -h[5];
    outvec[22] = -h[4];
    outvec[23] = -h[3];
    outvec[24] = -h[2];
    outvec[25] = -h[1];
    outvec[26] = -h[0];

    outvec[27] = -h[0];
    outvec[28] = -h[1];
    outvec[29] = -h[2];
    outvec[30] = -h[3];
    outvec[31] = -h[4];
    outvec[32] = -h[5];
    outvec[33] = -h[6];
    outvec[34] = -h[7];
    outvec[35] = -h[8];
}


static void MPG_L3_Frequency_Inversion( uint32_t gr, uint32_t ch )
{
    uint32_t sb, i;

    for( sb = 1; sb < 32; sb += 2 ) {
        for( i = 1; i < 18; i += 2 ) {
            g_main_data.is[gr][ch][sb*18 + i] = -g_main_data.is[gr][ch][sb*18 + i];
        }
    }
}


static void MPG_L3_Subband_Synthesis( uint32_t gr, uint32_t ch, uint32_t outdata[576] )
{
    FLOAT32     s_vec[32], sum;
    int32_t     samp;
    uint32_t    ss;
    uint32_t    i, j;
    uint32_t    nch;
    static uint32_t init = 1;

    static FLOAT32 u_vec[512];
    static FLOAT32 v_vec[2 /* ch */][1024];

    /* Number of channels (1 for mono and 2 for stereo) */
    nch = (g_frame_header.mode == mpeg1_mode_single_channel ? 1 : 2);


// TODO: this needs to go into a library init function
    /* Setup the n_win windowing vector and the v_vec intermediate vector */
    if( init ) {
        synth_init = 1;
        init = 0;
    }

    if( synth_init ) {
        /* Setup the v_vec intermediate vector */
        for( i = 0; i < 2; i++ ) {
            for( j = 0; j < 1024; j++ ) {
               v_vec[i][j] = 0.0;
            }
        }

        synth_init = 0;
    }

    /* Loop through the 18 samples in each of the 32 subbands */
    for( ss = 0; ss < 18; ss++ ) {
        /* Shift up the V vector */
        /*for (i = 1023; i > 63; i--) {
            v_vec[ch][i] = v_vec[ch][i-64];
        }*/
        for( i = 1024; i > 0; i -= 64 )
            memcpy(&v_vec[ch][i-64], &v_vec[ch][i-128], 64 * sizeof(v_vec[ch][0]));
            samp = ch;

            /* Copy the next 32 time samples to a temp vector */
            for( i = 0; i < 32; i++ ) {
                s_vec[i] = ((FLOAT32) g_main_data.is[gr][ch][i*18 + ss]);
            }

            /* Perform the matrixing operation on the input vector */
            MPG_Polyphase_Matrixing( s_vec, v_vec[ch] );

            /* Build the U vector */
            for( i = 0; i < 8; i++ ) {
                for( j = 0; j < 32; j++ ) {
                    u_vec[i*64 + j]      = v_vec[ch][i*128 + j];
                    u_vec[i*64 + j + 32] = v_vec[ch][i*128 + j + 96];
                }
            }
            for( i = 0; i < 8; i++ ) {
                memcpy(&u_vec[i*64], &v_vec[ch][i*128], 32*4);
                memcpy(&u_vec[i*64 + 32], &v_vec[ch][i*128 + 96], 32*4);
            }

            /* Window by u_vec[i] with g_synth_dtbl[i] */
            for( i = 0; i < 512; i++ ) {
                u_vec[i] = u_vec[i] * g_synth_dtbl[i];
            }

            /* Calculate 32 samples and store them in the outdata vector */
            for( i = 0; i < 32; i++ ) {
                sum = 0.0;
            for( j = 0; j < 16; j++ ) {
                sum += u_vec[j*32 + i];
            }

            /* sum now contains time sample 32*ss+i. Convert to 16-bit signed int */
            samp = (int32_t)(sum * 32767.0);
            if( samp > 32767 ) {
                samp = 32767;
            } else if( samp < -32767 ) {
                samp = -32767;
            }

            samp &= 0xffff;

            /* This function must be called for channel 0 first */
            if( ch == 0 ) {
                /* We always run the audio system in stereo mode, and duplicate
                 * the channels here for mono */
                if( nch == 1 ) {
                    outdata[32*ss + i] = (samp << 16) | (samp);
                } else {
                    outdata[32*ss + i] = samp;
                }
            } else {
                outdata[32*ss + i] |= samp << 16;
            }
        } /* end for (i... */
    } /* end for (ss... */
}
