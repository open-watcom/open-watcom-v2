int     x;
#pragma aux pinit0=".486p"
#pragma aux p001 = "xor al,0fH"
r001() { p001(); }
#pragma aux p002 = "xor al,7fH"
r002() { p002(); }
#pragma aux p004 = "xor al,0ffH"
r004() { p004(); }
#pragma aux p006 = "xor ax,000fH"
r006() { p006(); }
#pragma aux p008 = "xor ax,007fH"
r008() { p008(); }
#pragma aux p010 = "xor ax,00ffH"
r010() { p010(); }
#pragma aux p012 = "xor ax,7fffH"
r012() { p012(); }
#pragma aux p014 = "xor ax,0ffffH"
r014() { p014(); }
#pragma aux p016 = "xor eax,0000000fH"
r016() { p016(); }
#pragma aux p018 = "xor eax,0000007fH"
r018() { p018(); }
#pragma aux p020 = "xor eax,000000ffH"
r020() { p020(); }
#pragma aux p022 = "xor eax,00007fffH"
r022() { p022(); }
#pragma aux p024 = "xor eax,0000ffffH"
r024() { p024(); }
#pragma aux p026 = "xor eax,7fffffffH"
r026() { p026(); }
#pragma aux p028 = "xor eax,0ffffffffH"
r028() { p028(); }
#pragma aux p030 = "xor dl,0fH"
r030() { p030(); }
#pragma aux p032 = "xor dl,7fH"
r032() { p032(); }
#pragma aux p034 = "xor dl,0ffH"
r034() { p034(); }
#pragma aux p036 = "xor dx,000fH"
r036() { p036(); }
#pragma aux p038 = "xor dx,007fH"
r038() { p038(); }
#pragma aux p040 = "xor dx,00ffH"
r040() { p040(); }
#pragma aux p042 = "xor dx,7fffH"
r042() { p042(); }
#pragma aux p044 = "xor dx,0ffffH"
r044() { p044(); }
#pragma aux p046 = "xor edx,0000000fH"
r046() { p046(); }
#pragma aux p048 = "xor edx,0000007fH"
r048() { p048(); }
#pragma aux p050 = "xor edx,000000ffH"
r050() { p050(); }
#pragma aux p052 = "xor edx,00007fffH"
r052() { p052(); }
#pragma aux p054 = "xor edx,0000ffffH"
r054() { p054(); }
#pragma aux p056 = "xor edx,7fffffffH"
r056() { p056(); }
#pragma aux p058 = "xor edx,0ffffffffH"
r058() { p058(); }
#pragma aux p060 = "xor byte ptr x,0fH"
r060() { p060(); }
#pragma aux p062 = "xor byte ptr x,7fH"
r062() { p062(); }
#pragma aux p064 = "xor byte ptr x,0ffH"
r064() { p064(); }
#pragma aux p066 = "xor word ptr x,000fH"
r066() { p066(); }
#pragma aux p068 = "xor word ptr x,007fH"
r068() { p068(); }
#pragma aux p070 = "xor word ptr x,00ffH"
r070() { p070(); }
#pragma aux p072 = "xor word ptr x,7fffH"
r072() { p072(); }
#pragma aux p074 = "xor word ptr x,0ffffH"
r074() { p074(); }
#pragma aux p076 = "xor dword ptr x,0000000fH"
r076() { p076(); }
#pragma aux p078 = "xor dword ptr x,0000007fH"
r078() { p078(); }
#pragma aux p080 = "xor dword ptr x,000000ffH"
r080() { p080(); }
#pragma aux p082 = "xor dword ptr x,00007fffH"
r082() { p082(); }
#pragma aux p084 = "xor dword ptr x,0000ffffH"
r084() { p084(); }
#pragma aux p086 = "xor dword ptr x,7fffffffH"
r086() { p086(); }
#pragma aux p088 = "xor dword ptr x,0ffffffffH"
r088() { p088(); }
#pragma aux p090 = "xor byte ptr x,dl"
r090() { p090(); }
#pragma aux p092 = "xor word ptr x,dx"
r092() { p092(); }
#pragma aux p094 = "xor dword ptr x,edx"
r094() { p094(); }
#pragma aux p096 = "xor dl,byte ptr x"
r096() { p096(); }
#pragma aux p098 = "xor dx,word ptr x"
r098() { p098(); }
#pragma aux p100 = "xor edx,dword ptr x"
r100() { p100(); }
#pragma aux p102 = "xor dl,bl"
r102() { p102(); }
#pragma aux p104 = "xor dx,bx"
r104() { p104(); }
#pragma aux p106 = "xor edx,ebx"
r106() { p106(); }
#pragma aux p108 = "xor al,bl"
r108() { p108(); }
#pragma aux p110 = "xor ax,bx"
r110() { p110(); }
#pragma aux p112 = "xor eax,ebx"
r112() { p112(); }
#pragma aux p114 = "xor dl,al"
r114() { p114(); }
#pragma aux p116 = "xor dx,ax"
r116() { p116(); }
#pragma aux p118 = "xor edx,eax"
r118() { p118(); }
#pragma aux p120 = "xor edx,ebx"
r120() { p120(); }
#pragma aux p122 = "xor edx,ecx"
r122() { p122(); }
#pragma aux p124 = "xor edx,edx"
r124() { p124(); }
#pragma aux p126 = "xor edx,esi"
r126() { p126(); }
#pragma aux p128 = "xor edx,edi"
r128() { p128(); }
#pragma aux p130 = "xor edx,esp"
r130() { p130(); }
#pragma aux p132 = "xor edx,ebp"
r132() { p132(); }
#pragma aux p134 = "xor edx,edi"
r134() { p134(); }
#pragma aux p136 = "xor dx,ax"
r136() { p136(); }
#pragma aux p138 = "xor dx,bx"
r138() { p138(); }
#pragma aux p140 = "xor dx,cx"
r140() { p140(); }
#pragma aux p142 = "xor dx,dx"
r142() { p142(); }
#pragma aux p144 = "xor dx,si"
r144() { p144(); }
#pragma aux p146 = "xor dx,di"
r146() { p146(); }
#pragma aux p148 = "xor dx,sp"
r148() { p148(); }
#pragma aux p150 = "xor dx,bp"
r150() { p150(); }
#pragma aux p152 = "xor dx,di"
r152() { p152(); }
#pragma aux p154 = "xor dl,al"
r154() { p154(); }
#pragma aux p156 = "xor dl,bl"
r156() { p156(); }
#pragma aux p158 = "xor dl,cl"
r158() { p158(); }
#pragma aux p160 = "xor dl,dl"
r160() { p160(); }
#pragma aux p162 = "xor dl,ah"
r162() { p162(); }
#pragma aux p164 = "xor dl,bh"
r164() { p164(); }
#pragma aux p166 = "xor dl,ch"
r166() { p166(); }
#pragma aux p168 = "xor dl,dh"
r168() { p168(); }
#pragma aux p170 = "xor eax,dword ptr [eax]"
r170() { p170(); }
#pragma aux p172 = "xor eax,dword ptr [edx]"
r172() { p172(); }
#pragma aux p174 = "xor eax,dword ptr [ebx]"
r174() { p174(); }
#pragma aux p176 = "xor eax,dword ptr [ecx]"
r176() { p176(); }
#pragma aux p178 = "xor eax,dword ptr [esi]"
r178() { p178(); }
#pragma aux p180 = "xor eax,dword ptr [edi]"
r180() { p180(); }
#pragma aux p182 = "xor eax,dword ptr [esp]"
r182() { p182(); }
#pragma aux p184 = "xor eax,dword ptr +0H[ebp]"
r184() { p184(); }
#pragma aux p186 = "xor eax,dword ptr +0fH[edx]"
r186() { p186(); }
#pragma aux p188 = "xor eax,dword ptr +7fH[edx]"
r188() { p188(); }
#pragma aux p190 = "xor eax,dword ptr +7fffH[edx]"
r190() { p190(); }
#pragma aux p192 = "xor eax,dword ptr +0ffffH[edx]"
r192() { p192(); }
#pragma aux p194 = "xor eax,dword ptr +7fffffffH[edx]"
r194() { p194(); }
#pragma aux p196 = "xor eax,dword ptr -1H[edx]"
r196() { p196(); }
#pragma aux p198 = "xor edx,dword ptr [eax]"
r198() { p198(); }
#pragma aux p200 = "xor edx,dword ptr [edx]"
r200() { p200(); }
#pragma aux p202 = "xor edx,dword ptr [ebx]"
r202() { p202(); }
#pragma aux p204 = "xor edx,dword ptr [ecx]"
r204() { p204(); }
#pragma aux p206 = "xor edx,dword ptr [esi]"
r206() { p206(); }
#pragma aux p208 = "xor edx,dword ptr [edi]"
r208() { p208(); }
#pragma aux p210 = "xor edx,dword ptr [esp]"
r210() { p210(); }
#pragma aux p212 = "xor edx,dword ptr +0H[ebp]"
r212() { p212(); }
#pragma aux p214 = "xor edx,dword ptr +0fH[edx]"
r214() { p214(); }
#pragma aux p216 = "xor edx,dword ptr +7fH[edx]"
r216() { p216(); }
#pragma aux p218 = "xor edx,dword ptr +7fffH[edx]"
r218() { p218(); }
#pragma aux p220 = "xor edx,dword ptr +0ffffH[edx]"
r220() { p220(); }
#pragma aux p222 = "xor edx,dword ptr +7fffffffH[edx]"
r222() { p222(); }
#pragma aux p224 = "xor edx,dword ptr -1H[edx]"
r224() { p224(); }
#pragma aux p226 = "xor edx,dword ptr [eax+esi]"
r226() { p226(); }
#pragma aux p228 = "xor edx,dword ptr [edx+esi]"
r228() { p228(); }
#pragma aux p230 = "xor edx,dword ptr [ebx+esi]"
r230() { p230(); }
#pragma aux p232 = "xor edx,dword ptr [ecx+esi]"
r232() { p232(); }
#pragma aux p234 = "xor edx,dword ptr [esi+esi]"
r234() { p234(); }
#pragma aux p236 = "xor edx,dword ptr [edi+esi]"
r236() { p236(); }
#pragma aux p238 = "xor edx,dword ptr [esp+esi]"
r238() { p238(); }
#pragma aux p240 = "xor edx,dword ptr +0H[ebp+esi]"
r240() { p240(); }
#pragma aux p242 = "xor edx,dword ptr +0fH[edx+esi]"
r242() { p242(); }
#pragma aux p244 = "xor edx,dword ptr +7fH[edx+esi]"
r244() { p244(); }
#pragma aux p246 = "xor edx,dword ptr +7fffH[edx+esi]"
r246() { p246(); }
#pragma aux p248 = "xor edx,dword ptr +0ffffH[edx+esi]"
r248() { p248(); }
#pragma aux p250 = "xor edx,dword ptr +7fffffffH[edx+esi]"
r250() { p250(); }
#pragma aux p252 = "xor edx,dword ptr -1H[edx+esi]"
r252() { p252(); }
#pragma aux p254 = "xor edx,dword ptr -0fH[edx+esi*2]"
r254() { p254(); }
#pragma aux p256 = "xor edx,dword ptr -7fffH[edx+esi*4]"
r256() { p256(); }
#pragma aux p258 = "xor edx,dword ptr [edx+esi*8]"
r258() { p258(); }
