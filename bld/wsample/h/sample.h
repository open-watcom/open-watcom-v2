/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


/*
            0 ->+--------+
                |        | (there is text followed by a ctrl-Z in case
                |        |  a user tries to edit or type a sample file)
                |        |
sample_start  ->+--------+ *
                | PREFIX |  *
                +========+   * repeated
                |  DATA  |  *
                +--------+ *
                    .
                    .
                    .
 header_start ->+--------+
                | HEADER |
          EOF ->+--------+

notes:
        - there are multiple CODE_LOAD records
        - after all the ADDR_MAPS have been processed (after a CODE_LOAD),
          use DoAddrMap to invoke WVIDEO's symbolic code
        - REMAP_SECTION records will occur after an OVL_LOAD. The data
          in them should be fed into the RemapSection routine
        - CALLGRAPH records must immediately follow the corresponding
          SAMPLES record
*/

#include <stdio.h>              /* for fpos_t */
#include "digtypes.h"           /* for system config */

#define SAMP_SIGNATURE          0xDEED
#define SAMP_MAJOR_VER          2
#define SAMP_MINOR_VER          2
#define SAMP_LARGEST_BLOCK      (0xfe00)

#pragma pack(push, 1);
typedef struct samp_address {
    uint_32             offset;
    uint_16             segment;
} samp_address;
#define SIZE_SAMP_ADDR  (sizeof( samp_address ))

/* sample header: lseek( fh, -((fpos) sizeof( samp_header )), SEEK_END ) */
typedef struct samp_header {
    uint_16             signature;      /* must == SAMP_SIGNATURE */
    uint_8              major_ver;      /* must == SAMP_MAJOR_VER */
    uint_8              minor_ver;      /* must <= SAMP_MINOR_VER */
    fpos_t              sample_start;   /* first sample block offset in file */
} samp_header;
#define SIZE_HEADER     (sizeof( samp_header ))

typedef enum samp_block_kinds {
    SAMP_INFO       =  0, /* statistics for sample file */
    SAMP_SAMPLES    =  1, /* actual CS:EIP samples */
    SAMP_MARK       =  2, /* a character string occurring at a certain time */
    SAMP_OVL_LOAD   =  3, /* an overlay section was loaded */
    SAMP_CODE_LOAD  =  4, /* executable file loaded */
    SAMP_ADDR_MAP   =  5, /* translation from map addr's to loaded code addr' */
    SAMP_MAIN_LOAD  =  6, /* main executable file loaded */
    SAMP_LAST       =  7, /* always the last block */
    SAMP_REMAP_SECTION=8, /* overlay section(s) have moved in memory */
    SAMP_CALLGRAPH  =  9, /* callgraph information for samples */
    NUM_BLOCK_KINDS = 10
} samp_block_kinds;


typedef struct samp_block_prefix {
    uint_32             tick;   /* tick number for block */
    uint_16             length; /* total size of block, including prefix */
    uint_16             kind;   /* what type of information in the block */
} samp_block_prefix;
#define SIZE_PREFIX     (sizeof( samp_block_prefix ))

typedef struct count_info {
    uint_32             number; /* quantity of a block type in sample file */
    uint_32             size;   /* total size of all blocks of a type */
} count_info;

/* there is only one samp_info block in a sample file and it is always the
    first block (pointed at by sample_start field in header)

    the tick field in the prefix is the total number of ticks that have
    occurred during program execution
*/
typedef struct samp_info_2_1 {
    uint_32             timer_rate;  /* microseconds between ticks*/
    count_info          count[NUM_BLOCK_KINDS];
} samp_info_2_1;

typedef struct samp_info {
    uint_32             timer_rate;  /* microseconds between ticks*/
    system_config       config;
    count_info          count[NUM_BLOCK_KINDS];
} samp_info;
#define SIZE_INFO       (sizeof( samp_info ))

typedef struct samp_mark {
    samp_address        addr;           /* CS:EIP at time of mark */
    uint_16             thread_id;      /* thread doing mark */
    char                mark_string[1]; /* variable len char string */
} samp_mark;
#define SIZE_MARK       (sizeof( samp_mark ) - sizeof( char ))


/* the tick field in the prefix is set to the tick number at the time the
    first sample in the block was recorded */
typedef struct samp_samples {
    uint_16             thread_id;      /* thread for samples */
    samp_address        sample[1];      /* variable number of samples */
} samp_samples;
#define SIZE_SAMPLE     (sizeof( samp_samples ) - sizeof( samp_address ))

#define OVL_RETURN              (0x8000)
#define OVL_BITMAP_SIZE( ns )   (((ns) + 7 ) / 8 )

/*
  NOTE: actual section number is ( req_section & ~OVL_RETURN )
        ( ( req_section & OVL_RETURN ) != 0 ) indicates RET instr (vs CALL)
*/
typedef struct samp_ovl_load {
    uint_16             req_section;    /* section num requested to load */
    samp_address        addr;           /* address of requesting instr */
    uint_8              ovl_map[1];     /* bit map of loaded sections */
} samp_ovl_load;
#define SIZE_OVL_LOAD   (sizeof( samp_ovl_load ) - sizeof( uint_8 ))

typedef struct samp_code_load {
    unsigned long       time_stamp;     /* time-stamp of .EXE file */
    samp_address        ovl_tab;        /* pointer to overlay table in root */
    char                name[1];        /* full name of file (includes '\0') */
} samp_code_load;
#define SIZE_CODE_LOAD  (sizeof( samp_code_load ) - sizeof( char ))


/* the samp_addr_map block gives the transformation from map file addresses
    to loaded code addresses for the most recent samp_code_load executable file.
    to convert from a given map address A, perform the following steps.
        1) find the map.segment field that matches the segment value for A
        2) replace the segment of A with the segment contained in the
            corresponding actual.segment field
        3) add the actual.offset field to the offset of A
        4) if in step 1 you could not find a segment number that matched the
            segment of A, then add data[0].actual.segment to the segment of A
*/
typedef struct mapping {
    samp_address                map;
    samp_address                actual;
} mapping;

typedef struct samp_addr_map {
    mapping                     data[1]; /* variable number */
} samp_addr_map;
#define SIZE_ADDR_MAP   (sizeof( samp_addr_map ))


/* A remapping takes place when the dynamic overlay manager decides to
   load or move a section to a different address than what the linker
   assigned it. These records will only occur after a SAMP_OVL_LOAD record,
   but there might be multiple records for a single load (depending on
   what's easier for the sampler to generate). The section field in the
   'remapping' struct identifies the section number that has moved, and
    the 'segment' field gives the new base address in memory for the
    section.
*/

typedef struct remapping {
    uint_16     section;
    uint_16     segment;
} remapping;


typedef struct samp_remap_sect {
    remapping                   data[1]; /* variable number */
} samp_remap_sect;
#define SIZE_REMAP_SECT (sizeof( samp_remap_sect ))


/* A callgraph record must immediately follow the sample record, and
   the sample information must be aligned.  The record is stored as
   a pop/push number giving the delta from the previous callgraph information
   stored, followed by a number of samples (corresponding to the push number
   above).  Note that the last sample pushed gives the address in the last
   entered routine from which __PRO was called, whereas earlier callgraph
   samples store the address from which the next routine was called.
*/

typedef struct cgraph_sample {
    uint_16             pop_n;          /* number of differing entries */
    uint_16             push_n;         /* number entries to replace */
    samp_address        addr[1];        /* variable # of entries to replace */
} cgraph_sample;
#define SIZE_CGRAPH_SAMPLE (sizeof( cgraph_sample ) - sizeof( samp_address ))


typedef struct samp_callgraph {
    uint_16             thread_id;      /* thread for callgraph information */
    uint_16             number;         /* number of samples: not strictly */
                                        /* necessary, but useful for smpdump */
    cgraph_sample       sample[1];      /* variable number of samples */
} samp_callgraph;
#define SIZE_CALLGRAPH  (sizeof( samp_callgraph ) - sizeof( cgraph_sample ))


typedef union samp_data {
    samp_info_2_1               old_info;
    samp_info                   info;
    samp_mark                   mark;
    samp_samples                sample;
    samp_ovl_load               ovl;
    samp_code_load              code;
    samp_addr_map               map;
    samp_remap_sect             remap;
    samp_callgraph              cgraph;
} samp_data;
#define SIZE_DATA       (sizeof( samp_data ))


typedef struct samp_block {
    samp_block_prefix           pref;
    samp_data                   d;
} samp_block;
#define SIZE_BLOCK      (sizeof( samp_block ))

#pragma pack(pop);
