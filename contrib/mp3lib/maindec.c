/******************************************************************************
*
* Top level functions for reading MPEG 1 and 2 Layer III audio data.
*
******************************************************************************/

#include "mp3dec.h"
#include "internal.h"

/*
 * Bitrate tables for all three layers, MPEG 1 and 2.
 *
 * Index 0 means free bitrate which is 0 > bitrate >= TBD bit/s
 */
uint32_t g_mpeg_bitrates[2][3][15 /* header bitrate_index */] = {
  {
    {   /* MPEG2 Layer I */
         0,  32000,  48000,  56000,  64000,  80000,  96000, 112000,
    128000, 144000, 160000, 176000, 192000, 224000, 256000
    },

    {   /* MPEG2 Layer II */
         0,   8000,  16000,  24000,  32000,  40000,  48000,  56000,
     64000,  80000,  96000, 112000, 128000, 144000, 160000
    },

    {   /* MPEG2 Layer III (same as Layer II) */
         0,   8000,  16000,  24000,  32000,  40000,  48000,  56000,
     64000,  80000,  96000, 112000, 128000, 144000, 160000
    }
  },
  {
    {   /* MPEG1 Layer I */
         0,  32000,  64000,  96000, 128000, 160000, 192000, 224000,
    256000, 288000, 320000, 352000, 384000, 416000, 448000
    },

    {  /* MPEG1 Layer II */
         0,  32000,  48000,  56000,  64000,  80000,  96000, 112000,
    128000, 160000, 192000, 224000, 256000, 320000, 384000
    },

    {   /* MPEG1 Layer III */
         0,  32000,  40000,  48000,  56000,  64000,  80000,  96000,
    112000, 128000, 160000, 192000, 224000, 256000, 320000
    }
  }
};

/* Sampling frequencies in Hertz (MPEG 1 and 2, all layers) */
uint32_t g_sampling_frequency[2][3] = {
  {
    22050,
    24000,
    16000
  },
  {
    44100,
    48000,
    32000
  }
};

/*
 * Scale factor band indices
 *
 * One table per sample rate. Each table contains the frequency indices
 * for the 12 short and 21 long scalefactor bands. The short indices
 * must be multiplied by 3 to get the actual index.
 */
t_sf_band_indices g_sf_band_indices[3 /* Sampling freq. */] = {
    {
      { 0, 4, 8, 12, 16, 20, 24, 30, 36, 44, 52, 62, 74, 90, 110, 134, 162,
    196, 238, 288, 342, 418, 576 },
      { 0, 4, 8, 12, 16, 22, 30, 40, 52, 66, 84, 106, 136, 192 }
    },
    {
      { 0, 4, 8, 12, 16, 20, 24, 30, 36, 42, 50, 60, 72, 88, 106, 128, 156,
    190, 230, 276, 330, 384, 576 },
      { 0, 4, 8, 12, 16, 22, 28, 38, 50, 64, 80, 100, 126, 192 }
    },
    {
      { 0, 4, 8, 12, 16, 20, 24, 30, 36, 44, 54, 66, 82, 102, 126, 156, 194,
    240, 296, 364, 448, 550, 576 },
      { 0, 4, 8, 12, 16, 22, 30, 42, 58, 78, 104, 138, 180, 192 }
    }
};

t_mpeg1_header    g_frame_header;
t_mpeg1_side_info g_side_info;  /* < 100 words */
t_mpeg1_main_data g_main_data;  /* Large static data (~2500 words) */

/********** Local functions and variables (defined here, used here) **********/
static int MPG_Get_Main_Data( uint32_t main_data_size, uint32_t main_data_begin );
static void MPG_Get_Sideinfo( uint32_t sideinfo_size );

/* Bit reservoir for main data */
uint32_t    g_main_data_vec[2*1024];/* Large static data */
uint32_t    *g_main_data_ptr;       /* Pointer into the reservoir */
uint32_t    g_main_data_idx;        /* Index into the current byte (0-7) */
uint32_t    g_main_data_top = 0;    /* Number of bytes in reservoir (0-1024) */

/* Bit reservoir for side info */
static uint32_t     side_info_vec[32+4];
static uint32_t     *side_info_ptr;     /* Pointer into the reservoir */
static uint32_t     side_info_idx;      /* Index into the current byte (0-7) */

static uint32_t mpeg1_scalefac_sizes[16][2 /* slen1, slen2 */] = {
    { 0, 0 }, { 0, 1 }, { 0, 2 }, { 0, 3 },
    { 3, 0 }, { 1, 1 }, { 1, 2 }, { 1, 3 },
    { 2, 1 }, { 2, 2 }, { 2, 3 }, { 3, 1 },
    { 3, 2 }, { 3, 3 }, { 4, 2 }, { 4, 3 }
};

/******************************************************************************
*
* Description: Searches for next frame and read it into the buffer.
*              Main data in this frame is saved for two frames since it
*              might be needed for them also.
* Parameters: None
* Return value: OK if a frame is successfully read, ERROR otherwise.
*
******************************************************************************/
int MPG_Read_Frame( void )
{
    uint32_t    first = 0;


    if( MPG_Get_Filepos() == 0 ) {
        first = 1;
        g_main_data_top = 0;
    }

    /* Try to find the next frame in the bitstream and decode it */
    if( MPG_Read_Header() != OK ) {
        return (ERROR);
    }

    if( first ) {
        MPG_Decode_L3_Init_Song();
    }

    /* Get CRC word if present */
    if( g_frame_header.protection_bit == 0 ) { /* CRC present */
        if( MPG_Read_CRC() != OK ) {
            return( ERROR );
        }
    }

    /* Get audio data */
    if( g_frame_header.layer == 3 ) {
        /* Get side info */
        MPG_Read_Audio_L3();

        /* If there's not enough main data in the bit reservoir,
         * signal to calling function so that decoding isn't done!
         */

        /* Get main data (scalefactors and Huffman coded frequency data) */
        if( MPG_Read_Main_L3() != OK ) {
            return( ERROR );
        }
    } else {
        ERR( "Only layer 3 (!= %d) is supported!\n",
             g_frame_header.layer );
        return( ERROR );
    }
    return (OK);
}


/******************************************************************************
*
* Description: Scans the bitstream for a syncword until we find it or EOF.
*              The syncword must be byte-aligned. It then reads and parses
*              the audio header.
* Parameters: None
* Return value: OK or ERROR if the syncword can't be found, or the header
*               contains impossible values.
*
******************************************************************************/
int MPG_Read_Header( void )
{
    uint32_t    b1, b2, b3, b4, header;


    /* Get the next four bytes from the bitstream */
    b1 = MPG_Get_Byte();
    b2 = MPG_Get_Byte();
    b3 = MPG_Get_Byte();
    b4 = MPG_Get_Byte();

    /* If we got an End Of File condition we're done */
    if( (b1 == C_MPG_EOF) || (b2 == C_MPG_EOF) || (b3 == C_MPG_EOF) ||
        (b4 == C_MPG_EOF) ) {

        return( ERROR );
    }

    header = (b1 << 24) | (b2 << 16) | (b3 << 8) | (b4 << 0);

    /* Are the high 12 bits the syncword (0xfff)? */
    if( (header & 0xfff00000) != C_MPG_SYNC ) {
        /* No, so scan the bitstream one byte at a time until we find it or EOF */
        while( 1 ) {
            /* Shift the values one byte to the left */
            b1 = b2;
            b2 = b3;
            b3 = b4;

            /* Get one new byte from the bitstream */
            b4 = MPG_Get_Byte();

            /* If we got an End Of File condition we're done */
            if( b4 == C_MPG_EOF ) {
                return( ERROR );
            }

            /* Make up the new header */
            header = (b1 << 24) | (b2 << 16) | (b3 << 8) | (b4 << 0);

            /* If it's the syncword (0xfff00000) we're done */
            if( (header & 0xfff00000) == C_MPG_SYNC ) {
                break;          /* Exit while(1) loop */
            }
        } /* while (1) */
    }

    /* If we get here we've found the sync word, and can decode the header
    * which is in the low 20 bits of the 32-bit sync+header word.
    */

    /* Decode the header */
    g_frame_header.id                 = (header & 0x00080000) >> 19;
    g_frame_header.layer              = (header & 0x00060000) >> 17;
    g_frame_header.protection_bit     = (header & 0x00010000) >> 16;

    g_frame_header.bitrate_index      = (header & 0x0000f000) >> 12;

    g_frame_header.sampling_frequency = (header & 0x00000c00) >> 10;
    g_frame_header.padding_bit        = (header & 0x00000200) >> 9;
    g_frame_header.private_bit        = (header & 0x00000100) >> 8;

    g_frame_header.mode               = (header & 0x000000c0) >> 6;
    g_frame_header.mode_extension     = (header & 0x00000030) >> 4;

    g_frame_header.copyright          = (header & 0x00000008) >> 3;
    g_frame_header.original_or_copy   = (header & 0x00000004) >> 2;
    g_frame_header.emphasis           = (header & 0x00000003) >> 0;

    /* Check for invalid values and impossible combinations */
    if( g_frame_header.id != 1 ) {
        ERR( "MPEG-2 NIY!\n" );
        ERR( "Header word is 0x%08x at file pos %d\n", header, MPG_Get_Filepos() );
        return( ERROR );
    }

    if( g_frame_header.bitrate_index == 0 ) {
        ERR( "Free bitrate format NIY!\n" );
        ERR( "Header word is 0x%08x at file pos %d\n", header, MPG_Get_Filepos() );
        return( ERROR );
    }

    if( g_frame_header.bitrate_index == 15 ) {
        ERR( "bitrate_index = 15 is invalid!\n");
        ERR( "Header word is 0x%08x at file pos %d\n", header, MPG_Get_Filepos() );
        return( ERROR );
    }

    if( g_frame_header.sampling_frequency == 3 ) {
        ERR( "sampling_frequency = 3 is invalid!\n" );
        ERR( "Header word is 0x%08x at file pos %d\n", header, MPG_Get_Filepos() );
        return( ERROR );
    }

    if( g_frame_header.layer == 0 ) {
        ERR( "layer = 0 is invalid!\n" );
        ERR( "Header word is 0x%08x at file pos %d\n", header, MPG_Get_Filepos() );
        return( ERROR );
    }
    g_frame_header.layer = 4 - g_frame_header.layer;
    return( OK );
}


/******************************************************************************
*
* Description: Reads the 16 CRC bits
* Parameters: None
* Return value: OK or ERROR if CRC could not be read.
*
******************************************************************************/
/* Read CRC bits */
int MPG_Read_CRC( void )
{
    uint32_t    b1, b2;

    /* Get the next two bytes from the bitstream */
    b1 = MPG_Get_Byte();
    b2 = MPG_Get_Byte();

    /* If we got an End Of File condition we're done */
    if( (b1 == C_MPG_EOF) || (b2 == C_MPG_EOF) ) {
        return( FALSE );
    }
    return( OK );
}


/******************************************************************************
*
* Description: Reads the audio and main data from the bitstream into a private
*              buffer. The main data is taken from this frame and up to two
*              previous frames.
* Parameters: None
* Return value: OK or ERROR if data could not be read, or contains errors.
*
******************************************************************************/
int MPG_Read_Audio_L3( void )
{
    uint32_t    framesize, sideinfo_size, main_data_size;
    uint32_t    nch, ch, gr, scfsi_band, region, window;


    /* Number of channels (1 for mono and 2 for stereo) */
    nch = g_frame_header.mode == mpeg1_mode_single_channel ? 1 : 2;

    /* Calculate header audio data size */
    framesize = (144 *
        g_mpeg_bitrates[g_frame_header.id][g_frame_header.layer-1][g_frame_header.bitrate_index]) /
        g_sampling_frequency[g_frame_header.id][g_frame_header.sampling_frequency] +
        g_frame_header.padding_bit;

    if( framesize > 2000 ) {
        ERR( "framesize = %d\n", framesize );
        return( ERROR );
    }

    /* Sideinfo is 17 bytes for one channel and 32 bytes for two */
    sideinfo_size = nch == 1 ? 17 : 32;

    /* Main data size is the rest of the frame, including ancillary data */
    main_data_size = framesize - sideinfo_size - 4 /* sync+header */;

    /* CRC is 2 bytes */
    if( g_frame_header.protection_bit == 0 )
        main_data_size -= 2;

    /* Read the sideinfo from the bitstream into a local buffer used by the
     * MPG_Get_Side_Bits function.
     */
    MPG_Get_Sideinfo( sideinfo_size );
    if( MPG_Get_Filepos() == C_MPG_EOF )
        return( ERROR );

    /* Parse audio data */

    /* Pointer to where we should start reading main data */
    g_side_info.main_data_begin = MPG_Get_Side_Bits( 9 );

    /* Get private bits. Not used for anything. */
    if( g_frame_header.mode == mpeg1_mode_single_channel )
        g_side_info.private_bits = MPG_Get_Side_Bits( 5 );
    else
        g_side_info.private_bits = MPG_Get_Side_Bits( 3 );

    /* Get scale factor selection information */
    for( ch = 0; ch < nch; ch++ ) {
        for( scfsi_band = 0; scfsi_band < 4; scfsi_band++ ) {
            g_side_info.scfsi[ch][scfsi_band] = MPG_Get_Side_Bits( 1 );
        }
    }

    /* Get the rest of the side information */
    for( gr = 0; gr < 2; gr++ ) {
        for( ch = 0; ch < nch; ch++ ) {
            g_side_info.part2_3_length[gr][ch]    = MPG_Get_Side_Bits( 12 );
            g_side_info.big_values[gr][ch]        = MPG_Get_Side_Bits( 9 );
            g_side_info.global_gain[gr][ch]       = MPG_Get_Side_Bits( 8 );
            g_side_info.scalefac_compress[gr][ch] = MPG_Get_Side_Bits( 4 );

            g_side_info.win_switch_flag[gr][ch]   = MPG_Get_Side_Bits( 1 );

            if( g_side_info.win_switch_flag[gr][ch] == 1 ) {
                g_side_info.block_type[gr][ch]       = MPG_Get_Side_Bits( 2 );
                g_side_info.mixed_block_flag[gr][ch] = MPG_Get_Side_Bits( 1 );
                for( region = 0; region < 2; region++ ) {
                    g_side_info.table_select[gr][ch][region] = MPG_Get_Side_Bits( 5 );
                }
                for (window = 0; window < 3; window++) {
                   g_side_info.subblock_gain[gr][ch][window] = MPG_Get_Side_Bits( 3 );
                }
                if( (g_side_info.block_type[gr][ch] == 2) &&
                    (g_side_info.mixed_block_flag[gr][ch] == 0) ) {

                    g_side_info.region0_count[gr][ch] = 8; /* Implicit */
                } else {
                    g_side_info.region0_count[gr][ch] = 7; /* Implicit */
                }
                /* The standard is wrong on this!!! */
                g_side_info.region1_count[gr][ch] =
                    20 - g_side_info.region0_count[gr][ch];   /* Implicit */
            } else {
                for( region = 0; region < 3; region++ ) {
                    g_side_info.table_select[gr][ch][region] = MPG_Get_Side_Bits( 5 );
                }
                g_side_info.region0_count[gr][ch] = MPG_Get_Side_Bits( 4 );
                g_side_info.region1_count[gr][ch] = MPG_Get_Side_Bits( 3 );
                g_side_info.block_type[gr][ch] = 0; /* Implicit */
            } /* end if ... */

            g_side_info.preflag[gr][ch]            = MPG_Get_Side_Bits( 1 );
            g_side_info.scalefac_scale[gr][ch]     = MPG_Get_Side_Bits( 1 );
            g_side_info.count1table_select[gr][ch] = MPG_Get_Side_Bits( 1 );
        } /* end for (channel... */
    } /* end for (granule... */
    return( OK );
}


/******************************************************************************
*
* Description: This function reads the main data for layer 3 from the
*              main_data bit reservoir.
* Parameters: None
* Return value: OK or ERROR if the data contains errors.
*
******************************************************************************/
int MPG_Read_Main_L3( void )
{
    uint32_t    framesize, sideinfo_size, main_data_size;
    uint32_t    gr, ch, nch, sfb, win, slen1, slen2, nbits, part_2_start;


    /* Number of channels (1 for mono and 2 for stereo) */
    nch = g_frame_header.mode == mpeg1_mode_single_channel ? 1 : 2;

    /* Calculate header audio data size */
    framesize = (144 *
        g_mpeg_bitrates[g_frame_header.id][g_frame_header.layer-1][g_frame_header.bitrate_index]) /
        g_sampling_frequency[g_frame_header.id][g_frame_header.sampling_frequency] +
        g_frame_header.padding_bit;

    if( framesize > 2000 ) {
        ERR( "framesize = %d\n", framesize );
        return( ERROR );
    }

    /* Sideinfo is 17 bytes for one channel and 32 bytes for two */
    sideinfo_size = nch == 1 ? 17 : 32;

    /* Main data size is the rest of the frame, including ancillary data */
    main_data_size = framesize - sideinfo_size - 4 /* sync+header */;

    /* CRC is 2 bytes */
    if( g_frame_header.protection_bit == 0 )
        main_data_size -= 2;

    /* Assemble main data buffer with data from this frame and the previous
    * two frames. main_data_begin indicates how many bytes from previous
    * frames that should be used. This buffer is later accessed by the
    * MPG_Get_Main_Bits function in the same way as the side info is.
    */
    if( MPG_Get_Main_Data( main_data_size, g_side_info.main_data_begin) != OK ) {
        return( ERROR );    /* This could be due to not enough data in reservoir */
    }

    for( gr = 0; gr < 2; gr++ ) {
        for( ch = 0; ch < nch; ch++ ) {
            part_2_start = MPG_Get_Main_Pos();

            /* Number of bits in the bitstream for the bands */
            slen1 = mpeg1_scalefac_sizes[g_side_info.scalefac_compress[gr][ch]][0];
            slen2 = mpeg1_scalefac_sizes[g_side_info.scalefac_compress[gr][ch]][1];

            if( (g_side_info.win_switch_flag[gr][ch] != 0) &&
                (g_side_info.block_type[gr][ch] == 2) ) {

                if( g_side_info.mixed_block_flag[gr][ch] != 0 ) {
                    for( sfb = 0; sfb < 8; sfb++ ) {
                        g_main_data.scalefac_l[gr][ch][sfb] = MPG_Get_Main_Bits( slen1 );
                    }
                    for( sfb = 3; sfb < 12; sfb++ ) {
                        if( sfb < 6 ) {  /* slen1 is for bands 3-5, slen2 for 6-11 */
                            nbits = slen1;
                        } else {
                            nbits = slen2;
                        }

                        for( win = 0; win < 3; win++ ) {
                            g_main_data.scalefac_s[gr][ch][sfb][win] =
                                MPG_Get_Main_Bits( nbits );
                        }
                    }
                } else {
                    for( sfb = 0; sfb < 12; sfb++ ) {
                        if( sfb < 6 ) {     /* slen1 is for bands 3-5, slen2 for 6-11 */
                            nbits = slen1;
                        } else {
                            nbits = slen2;
                        }

                        for( win = 0; win < 3; win++ ) {
                            g_main_data.scalefac_s[gr][ch][sfb][win] =
                            MPG_Get_Main_Bits( nbits );
                        }
                    }
                }
            } else { /* block_type == 0 if winswitch == 0 */
                /* Scale factor bands 0-5 */
                if( (g_side_info.scfsi[ch][0] == 0) || (gr == 0) ) {
                    for( sfb = 0; sfb < 6; sfb++ ) {
                        g_main_data.scalefac_l[gr][ch][sfb] = MPG_Get_Main_Bits( slen1 );
                    }
                } else if( (g_side_info.scfsi[ch][0] == 1) && (gr == 1) ) {
                    /* Copy scalefactors from granule 0 to granule 1 */
                    for( sfb = 0; sfb < 6; sfb++ ) {
                        g_main_data.scalefac_l[1][ch][sfb] =
                            g_main_data.scalefac_l[0][ch][sfb];
                    }
                }

                /* Scale factor bands 6-10 */
                if ((g_side_info.scfsi[ch][1] == 0) || (gr == 0)) {
                    for( sfb = 6; sfb < 11; sfb++ ) {
                        g_main_data.scalefac_l[gr][ch][sfb] = MPG_Get_Main_Bits( slen1 );
                    }
                } else if( (g_side_info.scfsi[ch][1] == 1) && (gr == 1) ) {
                    /* Copy scalefactors from granule 0 to granule 1 */
                    for( sfb = 6; sfb < 11; sfb++ ) {
                        g_main_data.scalefac_l[1][ch][sfb] =
                            g_main_data.scalefac_l[0][ch][sfb];
                    }
                }

                /* Scale factor bands 11-15 */
                if( (g_side_info.scfsi[ch][2] == 0) || (gr == 0) ) {
                    for( sfb = 11; sfb < 16; sfb++ ) {
                        g_main_data.scalefac_l[gr][ch][sfb] = MPG_Get_Main_Bits( slen2 );
                    }
                } else if( (g_side_info.scfsi[ch][2] == 1) && (gr == 1) ) {
                    /* Copy scalefactors from granule 0 to granule 1 */
                    for( sfb = 11; sfb < 16; sfb++ ) {
                        g_main_data.scalefac_l[1][ch][sfb] =
                            g_main_data.scalefac_l[0][ch][sfb];
                    }
                }

                /* Scale factor bands 16-20 */
                if( (g_side_info.scfsi[ch][3] == 0) || (gr == 0) ) {
                    for( sfb = 16; sfb < 21; sfb++ ) {
                        g_main_data.scalefac_l[gr][ch][sfb] = MPG_Get_Main_Bits( slen2 );
                    }
                } else if( (g_side_info.scfsi[ch][3] == 1) && (gr == 1) ) {
                    /* Copy scalefactors from granule 0 to granule 1 */
                    for( sfb = 16; sfb < 21; sfb++ ) {
                        g_main_data.scalefac_l[1][ch][sfb] =
                            g_main_data.scalefac_l[0][ch][sfb];
                    }
                }
            }

            /* Read Huffman coded data. Skip stuffing bits. */
            MPG_Read_Huffman( part_2_start, gr, ch );
        } /* end for (gr... */
    } /* end for (ch... */

    /* The ancillary data is stored here, but we ignore it. */
    return( OK );
}


/******************************************************************************
*
* Description: This function reads 'number_of_bits' bits from the local buffer
*              which contains the side_info.
* Parameters: number_of_bits to read (max 16)
* Return value: The bits are returned in the LSB of the return value.
*
******************************************************************************/
uint32_t MPG_Get_Side_Bits( uint32_t number_of_bits )
{
    uint32_t tmp;

    /* Form a word of the next four bytes */
    tmp = (side_info_ptr[0] << 24) | (side_info_ptr[1] << 16) |
          (side_info_ptr[2] <<  8) | (side_info_ptr[3] <<  0);

    /* Remove bits already used */
    tmp = tmp << side_info_idx;

    /* Remove bits after the desired bits */
    tmp = tmp >> (32 - number_of_bits);

    /* Update pointers */
    side_info_ptr += (side_info_idx + number_of_bits) >> 3;
    side_info_idx  = (side_info_idx + number_of_bits) & 0x07;

    return( tmp );
}


/******************************************************************************
*
* Description: This function reads 'number_of_bits' bits from the local buffer
*              which contains the main_data.
* Parameters: number_of_bits to read (max 24)
* Return value: The bits are returned in the LSB of the return value.
*
******************************************************************************/
uint32_t MPG_Get_Main_Bits( uint32_t number_of_bits )
{
    uint32_t tmp;

    if( number_of_bits == 0 )
        return( 0 );

    /* Form a word of the next four bytes */
    tmp = (g_main_data_ptr[0] << 24) | (g_main_data_ptr[1] << 16) |
          (g_main_data_ptr[2] <<  8) | (g_main_data_ptr[3] <<  0);

    /* Remove bits already used */
    tmp = tmp << g_main_data_idx;

    /* Remove bits after the desired bits */
    tmp = tmp >> (32 - number_of_bits);

    /* Update pointers */
    g_main_data_ptr += (g_main_data_idx + number_of_bits) >> 3;
    g_main_data_idx  = (g_main_data_idx + number_of_bits) & 0x07;

    return( tmp );
}


/******************************************************************************
*
* Description: This function one bit from the local buffer
*              which contains the main_data.
* Parameters: None
* Return value: The bit is returned in the LSB of the return value.
*
******************************************************************************/
uint32_t MPG_Get_Main_Bit( void )
{
    uint32_t    tmp;

    tmp = g_main_data_ptr[0] >> (7 - g_main_data_idx);

    /*g_main_data_ptr += (g_main_data_idx + 1) >> 3;
    g_main_data_idx = (g_main_data_idx + 1) & 0x07;*/
    g_main_data_idx++;
    if( g_main_data_idx >= 8 ) {
        g_main_data_ptr++;
        g_main_data_idx = 0;
    }
    return( tmp & 1 );
}


/******************************************************************************
*
* Description: This function sets the position of the next bit to be read from
*              the main data bitstream.
* Parameters: Bit position. 0 = start, 8 = start of byte 1, etc.
* Return value: OK or ERROR if bit_pos is beyond the end of the main data for this
*               frame.
*
******************************************************************************/
int MPG_Set_Main_Pos( uint32_t bit_pos )
{
    g_main_data_ptr = &(g_main_data_vec[bit_pos >> 3]);
    g_main_data_idx = bit_pos & 0x7;
    return( OK );
}


/******************************************************************************
*
* Description: This function returns the position of the next bit to be read
*              from the main data bitstream.
* Parameters: None
* Return value: Bit position.
*
******************************************************************************/
uint32_t MPG_Get_Main_Pos( void )
{
    uint32_t pos;


    pos = ((uint32_t)g_main_data_ptr) - ((uint32_t) &(g_main_data_vec[0]));

    pos /= 4;               /* Divide by four to get number of bytes */
    pos = pos << 3;         /* Multiply by 8 to get number of bits */
    pos = pos + g_main_data_idx;    /* Add current bit index */

    return( pos );
}


/******************************************************************************
*
* Description: Reads the sideinfo from the bitstream into a local buffer
*              used by the MPG_Get_Side_Bits function.
*
******************************************************************************/
static void MPG_Get_Sideinfo( uint32_t sideinfo_size )
{
    if( MPG_Get_Bytes (sideinfo_size, side_info_vec) != OK ) {
        ERR( "\nCouldn't read sideinfo %d bytes at pos %d\n",
            sideinfo_size, MPG_Get_Filepos() );
        return;
    }

    side_info_ptr = &(side_info_vec[0]);
    side_info_idx = 0;
}


/*****************************************************************************
*
* Description: This function assembles the main data buffer with data from
*              this frame and the previous two frames into a local buffer
*              used by the MPG_Get_Main_Bits function.
* Parameters: main_data_begin indicates how many bytes from previous
*             frames that should be used. main_data_size indicates the number
*             of data bytes in this frame.
* Return value: None
*
******************************************************************************/
static int MPG_Get_Main_Data( uint32_t main_data_size, uint32_t main_data_begin )
{
    int i, start_pos;

    if( main_data_size > 1500 )
        ERR( "main_data_size = %d\n", main_data_size );

    /* Check that there's data available from previous frames if needed */
    if( main_data_begin > g_main_data_top ) {
        /* No, there is not, so we skip decoding this frame, but we have to
         * read the main_data bits from the bitstream in case they are needed
         * for decoding the next frame.
         */
        MPG_Get_Bytes( main_data_size, &(g_main_data_vec[g_main_data_top]) );

        /* Set up pointers */
        g_main_data_ptr = &(g_main_data_vec[0]);
        g_main_data_idx = 0;
        g_main_data_top += main_data_size;

        return( ERROR );     /* This frame cannot be decoded! */
    }

    /* Copy data from previous frames */
    for( i = 0; i < main_data_begin; i++ ) {
        g_main_data_vec[i] = g_main_data_vec[g_main_data_top - main_data_begin + i];
    }

    start_pos = MPG_Get_Filepos();

    /* Read the main_data from file */
    MPG_Get_Bytes( main_data_size, &(g_main_data_vec[main_data_begin]) );

    /* Set up pointers */
    g_main_data_ptr = &(g_main_data_vec[0]);
    g_main_data_idx = 0;
    g_main_data_top = main_data_begin + main_data_size;

    return( OK );
}
