#include "fail.h"

#if __WATCOM_REVISION__ >= 8 && defined(__386__)
// bug in codegen with long code bursts that have relocs
void foo()
{
   long SrcPitch             = 0; 

#if __WATCOM_CPLUSPLUS__ > 1220
_asm .MMX
#endif

_asm                 
   {
   pushad
   
   movq        mm7, mm1                         
   movq        mm7, mm1 
   movq        mm7, mm1   
   movq        mm7, mm1                         
   movq        mm7, mm1                         
   movq        mm7, mm1                         
   movq        mm7, mm1                         
   movq        mm7, mm1                         
   movq        mm7, mm1                         
   movq        mm7, mm1                         
   movq        mm7, mm1                         
   movq        mm7, mm1                         
   movq        mm7, mm1
   movq        mm7, mm1                         
   movq        mm7, mm1                         
   movq        mm7, mm1                         
   movq        mm7, mm1                         
   movq        mm7, mm1                         
   movq        mm7, mm1                         
   movq        mm7, mm1                         
   movq        mm7, mm1                         
   movq        mm7, mm1                         
   movq        mm7, mm1                         
   movq        mm7, mm1                         
   movq        mm7, mm1                         
   movq        mm7, mm1                         
   movq        mm7, mm1                         
   movq        mm7, mm1                         
   movq        mm7, mm1                         
   movq        mm7, mm1                         
   movq        mm7, mm1                         
   movq        mm7, mm1                         
   movq        mm7, mm1                         
   movq        mm7, mm1                         
   movq        mm7, mm1                         
   movq        mm7, mm1                         
   movq        mm7, mm1                         
   movq        mm7, mm1                         
   movq        mm7, mm1                         
   movq        mm7, mm1                         
   movq        mm7, mm1                         
   movq        mm7, mm1                         
   movq        mm7, mm1                         
   movq        mm7, mm1                         
   movq        mm7, mm1                         
   movq        mm7, mm1		//removing this line compiles OK

   sub         eax, SrcPitch	//removing this line compiles OK

   EMMS

   popad
   }
}   
#endif

int main() {
    _PASS;
}
