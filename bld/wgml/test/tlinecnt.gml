.* test for linecounter and other variables
.im tsimplei.lay
:layout
:default
 spacing = 2
:elayout
.*
.dm lc begin
.ty &*   SYSLC = &SYSLC     SYSLINE = &SYSLINE     SYSLST = &SYSLST
.dm lc end
.*
.*tm 8
.lc 0
:gdoc sec='top secret'
.lc 1
:frontm
:titlep
.lc t1
:title stitle='simple doc'.Simple Document
.lc t2
:title stitle='simple doc2'.Simple Document Title 2
:docnum.SIMPLE 7
:date.Date was 11.11.2009
:author.First Author
:author.My Second Personality
.lc ae
:address.
:aline.address 1 1
:aline.address 1 2
:cmt.aline.address 2 1
:cmt.aline.address 2 2
:eaddress.
.lc ade
:etitlep
.lc te
:body
.ty in body sysenv &$env
.lc 2
:p
This :HP2.test:eHP2. fragment.
.lc 2a
.br
.*im ts.gml
.lc 3
Here we have some text to fill the line. Let it overflow, just to see
how wgml 4.0 and our new wgml handle line overflow to see the differences
in the output.
.co on
.lc 4
:p
Hopefully no differences! :12345 test for undefined user tag. More text
to make a true line overflow for a 2 line paragraph.
.lc 5
.*im tgeov
.ty &amp.sysju=&sysju   &amp.sysco=&sysco  c2d(&amp.sysrb)=&'c2d(&sysrb.)
:P.:HP1.hi1::EHP1.
with colon
:HP1.hp1:HP2.hp1+2:hp3. 33 :hp1. 11 :ehp1. 3 :ehp3. :EHP2. hi1:EHP1.
and :HP3.hi3:EHP3. without colon
 A :HP1.p:EHP1.aragraph with text. And some more for line overflow.
.br
.co off
test paragraph one

test paragraph two

.lc 6
============================ 11:43:59 H:\ow\bld =============================
.br .co on
Languages Build: rel2
.br
WATCOM points to: H:\ow\rel2
.br
**** REL2 rule
.br
01====================== 11:43:59 H:\ow\bld\builder =========================
02====================== 11:43:59 H:\ow\bld\builder =========================
03=================== 11:43:59 H:\ow\bld\builder\os2386 =====================
.sk 2
04====================== 11:43:59 H:\ow\bld\builder =========================
.lc 10
.sk
05====================== 11:43:59 H:\ow\bld\builder =========================
06** REL2 rule
07======================= 11:43:59 H:\ow\bld\pmake ==========================
08=================== 11:43:59 H:\ow\bld\pmake\prebuild =====================
09======================= 11:43:59 H:\ow\bld\pmake ==========================
10** REL2 rule
11======================= 11:43:59 H:\ow\bld\cc\wcl =========================
12================== 11:43:59 H:\ow\bld\cc\wcl\prebuild =====================
13======================= 11:43:59 H:\ow\bld\cc\wcl =========================
14** REL2 rule
15====================== 11:43:59 H:\ow\bld\w32loadr ========================
16====================== 11:43:59 H:\ow\bld\w32loadr ========================
17** REL2 rule
18======================== 11:43:59 H:\ow\bld\yacc ==========================
19=================== 11:43:59 H:\ow\bld\yacc\prebuild ======================
20======================== 11:43:59 H:\ow\bld\yacc ==========================
21** REL2 rule
22m tgeov
23======================== 11:44:00 H:\ow\bld\re2c ==========================
24=================== 11:44:00 H:\ow\bld\re2c\prebuild ======================
25======================== 11:44:00 H:\ow\bld\re2c ==========================
26** REL2 rule
27======================= 11:44:00 H:\ow\bld\whpcvt =========================
28================== 11:44:00 H:\ow\bld\whpcvt\prebuild =====================
29======================= 11:44:00 H:\ow\bld\whpcvt =========================
30** REL2 rule
31====================== 11:44:00 H:\ow\bld\helpcomp ========================
32================= 11:44:00 H:\ow\bld\helpcomp\prebuild ====================
33====================== 11:44:00 H:\ow\bld\helpcomp ========================
34** REL2 rule
35======================= 11:44:00 H:\ow\bld\hcdos ==========================
36=================== 11:44:00 H:\ow\bld\hcdos\prebuild =====================
37======================= 11:44:00 H:\ow\bld\hcdos ==========================
38** REL2 rule
39m tgeov
40======================= 11:44:01 H:\ow\bld\wipfc ==========================
41=================== 11:44:01 H:\ow\bld\wipfc\prebuild =====================
42======================= 11:44:02 H:\ow\bld\wipfc ==========================
43** REL2 rule
44====================== 11:44:02 H:\ow\bld\bmp2eps =========================
45================== 11:44:02 H:\ow\bld\bmp2eps\prebuild ====================
46====================== 11:44:03 H:\ow\bld\bmp2eps =========================
47** REL2 rule
48======================== 11:44:03 H:\ow\bld\ssl ===========================
49==================== 11:44:03 H:\ow\bld\ssl\prebuild ======================
50======================== 11:44:03 H:\ow\bld\ssl ===========================
51** REL2 rule
52======================= 11:44:03 H:\ow\bld\wstub ==========================
53======================= 11:44:03 H:\ow\bld\wstub ==========================
54======================= 11:44:03 H:\ow\bld\wstub ==========================
55======================= 11:44:03 H:\ow\bld\wstub ==========================
56** REL2 rule
57======================= 11:44:03 H:\ow\bld\wpack ==========================
58=================== 11:44:03 H:\ow\bld\wpack\prebuild =====================
59======================= 11:44:03 H:\ow\bld\wpack ==========================
60** REL2 rule
61======================= 11:44:03 H:\ow\bld\nwlib ==========================
62=================== 11:44:03 H:\ow\bld\nwlib\prebuild =====================
63======================= 11:44:04 H:\ow\bld\nwlib ==========================
64======================= 11:44:01 H:\ow\bld\wipfc ==========================
65=================== 11:44:01 H:\ow\bld\wipfc\prebuild =====================
66======================= 11:44:02 H:\ow\bld\wipfc ==========================
67** REL2 rule
68====================== 11:44:02 H:\ow\bld\bmp2eps =========================
69================== 11:44:02 H:\ow\bld\bmp2eps\prebuild ====================
70====================== 11:44:03 H:\ow\bld\bmp2eps =========================
71** REL2 rule
72======================== 11:44:03 H:\ow\bld\ssl ===========================
73==================== 11:44:03 H:\ow\bld\ssl\prebuild ======================
74======================== 11:44:03 H:\ow\bld\ssl ===========================
75** REL2 rule
76======================= 11:44:03 H:\ow\bld\wstub ==========================
77======================= 11:44:03 H:\ow\bld\wstub ==========================
78======================= 11:44:03 H:\ow\bld\wstub ==========================
79======================= 11:44:03 H:\ow\bld\wstub ==========================
80** REL2 rule
81======================= 11:44:03 H:\ow\bld\wpack ==========================
82=================== 11:44:03 H:\ow\bld\wpack\prebuild =====================
83======================= 11:44:03 H:\ow\bld\wpack ==========================
84** REL2 rule
85======================= 11:44:03 H:\ow\bld\nwlib ==========================
86=================== 11:44:03 H:\ow\bld\nwlib\prebuild =====================
87======================= 11:44:04 H:\ow\bld\nwlib ==========================
88** REL2 rule last text record
.se $apage=100
.ty sysapage  &SYSAPAGE
.im ts.inc
:p
:appendix
.ty in appendix sysenv &$env
:backm
.ty in backm sysenv &$env
.ty before egdoc sysenv &$env
:egdoc
