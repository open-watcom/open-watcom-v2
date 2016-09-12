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


#ifndef _SAMPINFO_H
#include <limits.h>
#include "sample.h"


/*
 * dig_fhandle can be pointer to file structure or handle number
 * therefore 0/NULL is reserved for errors
 * if handle number is used then handle must be 1 based
 */
#define PH2DFH(sh)  (dig_fhandle)(pointer_int)((sh) + 1)
#define DFH2PH(dfh) ((int)(pointer_int)(dfh) - 1)

typedef uint_16                 thread_id;
typedef uint_16                 section_id;
typedef unsigned long           clicks_t;
typedef unsigned long           sample_index_t;

enum {
    SORT_COUNT = 0,
    SORT_NAME,
    SORT_DISABLE
};

enum {
    LEVEL_SAMPLE,
    LEVEL_IMAGE,
    LEVEL_MODULE,
    LEVEL_FILE,
    LEVEL_ROUTINE,
    LEVEL_SOURCE,
    LEVEL_ASSEMBLY,
};

#define ROOT_SECTION            (0)


typedef struct ovl_entry {
    char *                      fname;
    uint_16                     start_para;
    addr_seg                    base_para;
    off_t                       disk_addr;
    unsigned                    separate_overlay : 1;
} ovl_entry;

typedef struct overlay_data {
    struct overlay_data *       next;
    clicks_t                    tick;
    section_id                  section;
    address                     req_addr;
    unsigned                    overlay_return : 1;
} overlay_data;

typedef struct map_to_actual {
    address                     map;
    address                     actual;
    addr_off                    length;
} map_to_actual;

typedef struct remap_data {
    struct remap_data *         next;
    clicks_t                    tick;
    section_id                  section;
    addr_seg                    segment;
} remap_data;

typedef struct mark_data {
    struct mark_data *          next;
    clicks_t                    tick;
    thread_id                   thread;
    address                     addr;
    char                        name[1];
} mark_data;

typedef struct asmsrc_state {
    unsigned                    bar_max             : 1;
    unsigned                    abs_bar             : 1;
    unsigned                    rel_bar             : 1;
} asmsrc_state;

typedef struct rtn_info {
    sym_handle *                sh;
    clicks_t                    tick_count;
    clicks_t                    first_tick_index;
    clicks_t                    last_tick_index;
    unsigned                    unknown_routine     : 1;
    unsigned                    gather_routine      : 1;
    unsigned                    ignore_unknown_rtn  : 1;
    unsigned                    ignore_gather       : 1;
    unsigned                    bar_max             : 1;
    unsigned                    abs_bar             : 1;
    unsigned                    rel_bar             : 1;
    char                        name[1];
} rtn_info;

typedef struct file_info {
    cue_fileid                  fid;
    clicks_t                    agg_count;
    clicks_t                    max_time;
    rtn_info * *                routine;
    int                         rtn_count;
    int                         number_gathered;
    int                         sort_type;
    unsigned                    unknown_file        : 1;
    unsigned                    gather_file         : 1;
    unsigned                    ignore_unknown_file : 1;
    unsigned                    ignore_unknown_rtn  : 1;
    unsigned                    ignore_gather       : 1;
    unsigned                    gather_active       : 1;
    unsigned                    bar_max             : 1;
    unsigned                    abs_bar             : 1;
    unsigned                    rel_bar             : 1;
    unsigned                    sort_needed         : 1;
    char                        name[1];
} file_info;

typedef struct mod_info {
    mod_handle                  mh;
    clicks_t                    agg_count;
    clicks_t                    max_time;
    clicks_t                    first_tick_index;
    file_info * *               mod_file;
    int                         file_count;
    int                         number_gathered;
    int                         sort_type;
    unsigned                    unknown_module      : 1;
    unsigned                    gather_module       : 1;
    unsigned                    ignore_unknown_mod  : 1;
    unsigned                    ignore_unknown_file : 1;
    unsigned                    ignore_gather       : 1;
    unsigned                    gather_active       : 1;
    unsigned                    bar_max             : 1;
    unsigned                    abs_bar             : 1;
    unsigned                    rel_bar             : 1;
    unsigned                    sort_needed         : 1;
    char                        name[1];
} mod_info;

typedef struct image_info {
    address                     overlay_table;
    unsigned long               time_stamp;
    mod_handle                  dip_handle;
    clicks_t                    agg_count;
    clicks_t                    max_time;
    int                         object_file;
    char *                      sym_name;
    char *                      name;
    mod_info * *                module;
    map_to_actual *             map_data;
    ovl_entry *                 ovl_data;
    section_id                  ovl_count;
    int                         mod_count;
    int                         map_count;
    int                         number_gathered;
    int                         sort_type;
    unsigned                    main_load           : 1;
    unsigned                    sym_deleted         : 1;
    unsigned                    unknown_image       : 1;
    unsigned                    ignore_unknown_image: 1;
    unsigned                    ignore_unknown_mod  : 1;
    unsigned                    gather_image        : 1;
    unsigned                    gather_active       : 1;
    unsigned                    ignore_gather       : 1;
    unsigned                    exe_not_found       : 1;
    unsigned                    exe_changed         : 1;
    unsigned                    bar_max             : 1;
    unsigned                    abs_bar             : 1;
    unsigned                    rel_bar             : 1;
    unsigned                    sort_needed         : 1;
} image_info;


typedef struct thread_data {
    struct thread_data          *next;
    thread_id                   thread;
    clicks_t                    start_time;
    clicks_t                    end_time;
    address                     **raw_bucket;
} thread_data;

/*
   A pointer is smaller than an address, so it's more space efficent to
   keep a pointer to the raw sample storage than make a copy.
*/
typedef struct massgd_sample_addr {
    address                     *raw;
    clicks_t                    hits;
} massgd_sample_addr;


typedef struct sio_data {
    struct sio_data *           next;
    char *                      samp_file_name;
    process_info *              dip_process;
    file_handle                 fh;
    clicks_t                    timer_rate;  /* microseconds between ticks*/
    clicks_t                    gather_cutoff;
    clicks_t                    abs_count;
    clicks_t                    rel_count;
    clicks_t                    max_time;
    sample_index_t              total_samples;
    mark_data *                 marks;
    overlay_data *              ovl_loads;
    remap_data *                remaps;
    image_info * *              images;
    thread_data                 *samples;
    massgd_sample_addr          **massaged_sample;
    unsigned long               number_massaged;
    unsigned                    image_count;
    int                         number_gathered;
    int                         level_open;
    int                         curr_proc_row;
    int                         curr_display_row;
    int                         sort_type;
    void *                      sample_window;
    image_info *                curr_image;
    mod_info *                  curr_mod;
    file_info *                 curr_file;
    rtn_info *                  curr_rtn;
    void *                      src_file;
    void *                      asm_file;
    asmsrc_state                asm_src_info;
    samp_header                 header;
    system_config               config;
    unsigned                    massaged_mapped     : 1;
    unsigned                    ignore_unknown_image: 1;
    unsigned                    gather_active       : 1;
    unsigned                    bar_max             : 1;
    unsigned                    abs_bar             : 1;
    unsigned                    rel_bar             : 1;
    unsigned                    abs_on_screen       : 1;
    unsigned                    rel_on_screen       : 1;
    unsigned                    sort_needed         : 1;
} sio_data;

#define MAX_RAW_BUCKET_INDEX    (SHRT_MAX/sizeof(address))
#define MAX_RAW_BUCKET_SIZE     (MAX_RAW_BUCKET_INDEX*sizeof(address))
#define MAX_MASSGD_BUCKET_INDEX (SHRT_MAX/sizeof(massgd_sample_addr))
#define MAX_MASSGD_BUCKET_SIZE  (MAX_RAW_BUCKET_INDEX*sizeof(massgd_sample_addr))

#define RAW_BUCKET_IDX( idx )   ((idx) / MAX_RAW_BUCKET_INDEX)
#define MSG_BUCKET_IDX( idx )   ((idx) / MAX_MASSGD_BUCKET_INDEX)

#define _SAMPINFO_H
#endif
