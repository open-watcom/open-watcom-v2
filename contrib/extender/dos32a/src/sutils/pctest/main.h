/*
 * Copyright (C) 1996-2002 Supernar Systems, Ltd. All rights reserved.
 *
 * Redistribution  and  use  in source and  binary  forms, with or without
 * modification,  are permitted provided that the following conditions are
 * met:
 *
 * 1.  Redistributions  of  source code  must  retain  the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * 2.  Redistributions  in binary form  must reproduce the above copyright
 * notice,  this  list of conditions and  the  following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * 3. The end-user documentation included with the redistribution, if any,
 * must include the following acknowledgment:
 *
 * "This product uses DOS/32 Advanced DOS Extender technology."
 *
 * Alternately,  this acknowledgment may appear in the software itself, if
 * and wherever such third-party acknowledgments normally appear.
 *
 * 4.  Products derived from this software  may not be called "DOS/32A" or
 * "DOS/32 Advanced".
 *
 * THIS  SOFTWARE AND DOCUMENTATION IS PROVIDED  "AS IS" AND ANY EXPRESSED
 * OR  IMPLIED  WARRANTIES,  INCLUDING, BUT  NOT  LIMITED  TO, THE IMPLIED
 * WARRANTIES  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN  NO  EVENT SHALL THE  AUTHORS  OR  COPYRIGHT HOLDERS BE
 * LIABLE  FOR  ANY DIRECT, INDIRECT,  INCIDENTAL,  SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL  DAMAGES  (INCLUDING, BUT NOT  LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE  GOODS  OR  SERVICES;  LOSS OF  USE,  DATA,  OR  PROFITS; OR
 * BUSINESS  INTERRUPTION) HOWEVER CAUSED AND  ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE)  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <dos.h>
#include <math.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <typedefs.h>
//#include <debug.h>

void show_copyright(void);
void check_mem_avail(void);
void measure_cpu_speed(void);
void measure_fpu_speed(void);
void measure_risc_speed(void);

void measure_int_speed(void);
void measure_irq_speed(void);

void measure_lmem_speed(void);
void measure_hmem_speed(void);
void measure_vmem_speed(void);
void measure_system_speed(void);

extern	byte get_cpl(void);
extern	byte get_iopl(void);
extern	char *get_cpu_id(void);
extern	byte get_cpu_type(void);
extern	byte get_fpu_type(void);
extern	byte get_fpu_info(void);
extern	byte get_sys_type(void);
extern	byte get_extender_type(void);
extern	byte get_dpmi_flags(void);
extern	dword get_lomem_size(void);
extern	dword get_himem_size(void);
extern	dword get_total_size(void);
extern	dword get_pmodew_ver(void);

extern	dword test_cpu(void);
extern	dword test_fpu(void);
extern	dword test_int(void);
extern	dword test_irq(void);
extern	dword test_risc(void);
extern	dword test_priv(void);
extern	dword test_low_movsw(void);
extern	dword test_low_movsd(void);
extern	dword test_low_stosw(void);
extern	dword test_low_stosd(void);
extern	dword test_high_movsw(void);
extern	dword test_high_movsd(void);
extern	dword test_high_stosw(void);
extern	dword test_high_stosd(void);
extern	dword test_vid_movsw(void);
extern	dword test_vid_movsd(void);
extern	dword test_vid_stosw(void);
extern	dword test_vid_stosd(void);

extern	void setvideomode(byte);
extern	void gotoxy(byte, byte);
