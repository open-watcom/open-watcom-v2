int     x;
#pragma aux pinit0=".486p"
#pragma aux p001 = "or al,0fH"
r001() { p001(); }
#pragma aux p002 = "or al,7fH"
r002() { p002(); }
#pragma aux p004 = "or al,0ffH"
r004() { p004(); }
#pragma aux p006 = "or ax,000fH"
r006() { p006(); }
#pragma aux p008 = "or ax,007fH"
r008() { p008(); }
#pragma aux p010 = "or ax,00ffH"
r010() { p010(); }
#pragma aux p012 = "or ax,7fffH"
r012() { p012(); }
#pragma aux p014 = "or ax,0ffffH"
r014() { p014(); }
#pragma aux p016 = "or eax,0000000fH"
r016() { p016(); }
#pragma aux p018 = "or eax,0000007fH"
r018() { p018(); }
#pragma aux p020 = "or eax,000000ffH"
r020() { p020(); }
#pragma aux p022 = "or eax,00007fffH"
r022() { p022(); }
#pragma aux p024 = "or eax,0000ffffH"
r024() { p024(); }
#pragma aux p026 = "or eax,7fffffffH"
r026() { p026(); }
#pragma aux p028 = "or eax,0ffffffffH"
r028() { p028(); }
#pragma aux p030 = "or dl,0fH"
r030() { p030(); }
#pragma aux p032 = "or dl,7fH"
r032() { p032(); }
#pragma aux p034 = "or dl,0ffH"
r034() { p034(); }
#pragma aux p036 = "or dx,000fH"
r036() { p036(); }
#pragma aux p038 = "or dx,007fH"
r038() { p038(); }
#pragma aux p040 = "or dx,00ffH"
r040() { p040(); }
#pragma aux p042 = "or dx,7fffH"
r042() { p042(); }
#pragma aux p044 = "or dx,0ffffH"
r044() { p044(); }
#pragma aux p046 = "or edx,0000000fH"
r046() { p046(); }
#pragma aux p048 = "or edx,0000007fH"
r048() { p048(); }
#pragma aux p050 = "or edx,000000ffH"
r050() { p050(); }
#pragma aux p052 = "or edx,00007fffH"
r052() { p052(); }
#pragma aux p054 = "or edx,0000ffffH"
r054() { p054(); }
#pragma aux p056 = "or edx,7fffffffH"
r056() { p056(); }
#pragma aux p058 = "or edx,0ffffffffH"
r058() { p058(); }
#pragma aux p060 = "or byte ptr x,0fH"
r060() { p060(); }
#pragma aux p062 = "or byte ptr x,7fH"
r062() { p062(); }
#pragma aux p064 = "or byte ptr x,0ffH"
r064() { p064(); }
#pragma aux p066 = "or word ptr x,000fH"
r066() { p066(); }
#pragma aux p068 = "or word ptr x,007fH"
r068() { p068(); }
#pragma aux p070 = "or word ptr x,00ffH"
r070() { p070(); }
#pragma aux p072 = "or word ptr x,7fffH"
r072() { p072(); }
#pragma aux p074 = "or word ptr x,0ffffH"
r074() { p074(); }
#pragma aux p076 = "or dword ptr x,0000000fH"
r076() { p076(); }
#pragma aux p078 = "or dword ptr x,0000007fH"
r078() { p078(); }
#pragma aux p080 = "or dword ptr x,000000ffH"
r080() { p080(); }
#pragma aux p082 = "or dword ptr x,00007fffH"
r082() { p082(); }
#pragma aux p084 = "or dword ptr x,0000ffffH"
r084() { p084(); }
#pragma aux p086 = "or dword ptr x,7fffffffH"
r086() { p086(); }
#pragma aux p088 = "or dword ptr x,0ffffffffH"
r088() { p088(); }
#pragma aux p090 = "or byte ptr x,dl"
r090() { p090(); }
#pragma aux p092 = "or word ptr x,dx"
r092() { p092(); }
#pragma aux p094 = "or dword ptr x,edx"
r094() { p094(); }
#pragma aux p096 = "or dl,byte ptr x"
r096() { p096(); }
#pragma aux p098 = "or dx,word ptr x"
r098() { p098(); }
#pragma aux p100 = "or edx,dword ptr x"
r100() { p100(); }
#pragma aux p102 = "or dl,bl"
r102() { p102(); }
#pragma aux p104 = "or dx,bx"
r104() { p104(); }
#pragma aux p106 = "or edx,ebx"
r106() { p106(); }
#pragma aux p108 = "or al,bl"
r108() { p108(); }
#pragma aux p110 = "or ax,bx"
r110() { p110(); }
#pragma aux p112 = "or eax,ebx"
r112() { p112(); }
#pragma aux p114 = "or dl,al"
r114() { p114(); }
#pragma aux p116 = "or dx,ax"
r116() { p116(); }
#pragma aux p118 = "or edx,eax"
r118() { p118(); }
#pragma aux p120 = "or edx,ebx"
r120() { p120(); }
#pragma aux p122 = "or edx,ecx"
r122() { p122(); }
#pragma aux p124 = "or edx,edx"
r124() { p124(); }
#pragma aux p126 = "or edx,esi"
r126() { p126(); }
#pragma aux p128 = "or edx,edi"
r128() { p128(); }
#pragma aux p130 = "or edx,esp"
r130() { p130(); }
#pragma aux p132 = "or edx,ebp"
r132() { p132(); }
#pragma aux p134 = "or edx,edi"
r134() { p134(); }
#pragma aux p136 = "or dx,ax"
r136() { p136(); }
#pragma aux p138 = "or dx,bx"
r138() { p138(); }
#pragma aux p140 = "or dx,cx"
r140() { p140(); }
#pragma aux p142 = "or dx,dx"
r142() { p142(); }
#pragma aux p144 = "or dx,si"
r144() { p144(); }
#pragma aux p146 = "or dx,di"
r146() { p146(); }
#pragma aux p148 = "or dx,sp"
r148() { p148(); }
#pragma aux p150 = "or dx,bp"
r150() { p150(); }
#pragma aux p152 = "or dx,di"
r152() { p152(); }
#pragma aux p154 = "or dl,al"
r154() { p154(); }
#pragma aux p156 = "or dl,bl"
r156() { p156(); }
#pragma aux p158 = "or dl,cl"
r158() { p158(); }
#pragma aux p160 = "or dl,dl"
r160() { p160(); }
#pragma aux p162 = "or dl,ah"
r162() { p162(); }
#pragma aux p164 = "or dl,bh"
r164() { p164(); }
#pragma aux p166 = "or dl,ch"
r166() { p166(); }
#pragma aux p168 = "or dl,dh"
r168() { p168(); }
#pragma aux p170 = "or eax,dword ptr [eax]"
r170() { p170(); }
#pragma aux p172 = "or eax,dword ptr [edx]"
r172() { p172(); }
#pragma aux p174 = "or eax,dword ptr [ebx]"
r174() { p174(); }
#pragma aux p176 = "or eax,dword ptr [ecx]"
r176() { p176(); }
#pragma aux p178 = "or eax,dword ptr [esi]"
r178() { p178(); }
#pragma aux p180 = "or eax,dword ptr [edi]"
r180() { p180(); }
#pragma aux p182 = "or eax,dword ptr [esp]"
r182() { p182(); }
#pragma aux p184 = "or eax,dword ptr +0H[ebp]"
r184() { p184(); }
#pragma aux p186 = "or eax,dword ptr +0fH[edx]"
r186() { p186(); }
#pragma aux p188 = "or eax,dword ptr +7fH[edx]"
r188() { p188(); }
#pragma aux p190 = "or eax,dword ptr +7fffH[edx]"
r190() { p190(); }
#pragma aux p192 = "or eax,dword ptr +0ffffH[edx]"
r192() { p192(); }
#pragma aux p194 = "or eax,dword ptr +7fffffffH[edx]"
r194() { p194(); }
#pragma aux p196 = "or eax,dword ptr -1H[edx]"
r196() { p196(); }
#pragma aux p198 = "or edx,dword ptr [eax]"
r198() { p198(); }
#pragma aux p200 = "or edx,dword ptr [edx]"
r200() { p200(); }
#pragma aux p202 = "or edx,dword ptr [ebx]"
r202() { p202(); }
#pragma aux p204 = "or edx,dword ptr [ecx]"
r204() { p204(); }
#pragma aux p206 = "or edx,dword ptr [esi]"
r206() { p206(); }
#pragma aux p208 = "or edx,dword ptr [edi]"
r208() { p208(); }
#pragma aux p210 = "or edx,dword ptr [esp]"
r210() { p210(); }
#pragma aux p212 = "or edx,dword ptr +0H[ebp]"
r212() { p212(); }
#pragma aux p214 = "or edx,dword ptr +0fH[edx]"
r214() { p214(); }
#pragma aux p216 = "or edx,dword ptr +7fH[edx]"
r216() { p216(); }
#pragma aux p218 = "or edx,dword ptr +7fffH[edx]"
r218() { p218(); }
#pragma aux p220 = "or edx,dword ptr +0ffffH[edx]"
r220() { p220(); }
#pragma aux p222 = "or edx,dword ptr +7fffffffH[edx]"
r222() { p222(); }
#pragma aux p224 = "or edx,dword ptr -1H[edx]"
r224() { p224(); }
#pragma aux p226 = "or edx,dword ptr [eax+esi]"
r226() { p226(); }
#pragma aux p228 = "or edx,dword ptr [edx+esi]"
r228() { p228(); }
#pragma aux p230 = "or edx,dword ptr [ebx+esi]"
r230() { p230(); }
#pragma aux p232 = "or edx,dword ptr [ecx+esi]"
r232() { p232(); }
#pragma aux p234 = "or edx,dword ptr [esi+esi]"
r234() { p234(); }
#pragma aux p236 = "or edx,dword ptr [edi+esi]"
r236() { p236(); }
#pragma aux p238 = "or edx,dword ptr [esp+esi]"
r238() { p238(); }
#pragma aux p240 = "or edx,dword ptr +0H[ebp+esi]"
r240() { p240(); }
#pragma aux p242 = "or edx,dword ptr +0fH[edx+esi]"
r242() { p242(); }
#pragma aux p244 = "or edx,dword ptr +7fH[edx+esi]"
r244() { p244(); }
#pragma aux p246 = "or edx,dword ptr +7fffH[edx+esi]"
r246() { p246(); }
#pragma aux p248 = "or edx,dword ptr +0ffffH[edx+esi]"
r248() { p248(); }
#pragma aux p250 = "or edx,dword ptr +7fffffffH[edx+esi]"
r250() { p250(); }
#pragma aux p252 = "or edx,dword ptr -1H[edx+esi]"
r252() { p252(); }
#pragma aux p254 = "or edx,dword ptr -0fH[edx+esi*2]"
r254() { p254(); }
#pragma aux p256 = "or edx,dword ptr -7fffH[edx+esi*4]"
r256() { p256(); }
#pragma aux p258 = "or edx,dword ptr [edx+esi*8]"
r258() { p258(); }
