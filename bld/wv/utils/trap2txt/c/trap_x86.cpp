#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <trpcore.h>
#include <madregs.h>
#include <trpfile.h>

void DumpRegistersMAD_X86( mad_registers * pregs )
{
    struct x86_mad_registers * px = &pregs->x86;
    //          XXX:
    printf( "    EAX:%.08x EBX:%.08x ECX:%.08x EDX:%.08x\n", px->cpu.eax, px->cpu.ebx, px->cpu.ecx, px->cpu.edx );
    printf( "    ESI:%.08x EDI:%.08x EBP:%.08x ESP:%.08x\n", px->cpu.esi, px->cpu.edi, px->cpu.ebp, px->cpu.esp );
    printf( "    EIP:%.08x EFL:%.08x\n", px->cpu.eip, px->cpu.efl );
    printf( "    CR0:%.08x CR2:%.08x CR3:%.08x\n", px->cpu.cr0, px->cpu.cr2, px->cpu.cr3 );
    printf( "    DS: %.04x     ES: %.04x     SS: %.04x     CS: %.04x\n", px->cpu.ds, px->cpu.es, px->cpu.ss, px->cpu.cs );
    printf( "    FS: %.04x     GS: %.04x\n", px->cpu.fs, px->cpu.gs );

    printf( "    *** to be done - FPU, MMX, XMM *** \n" );
}
