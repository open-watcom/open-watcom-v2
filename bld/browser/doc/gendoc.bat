@echo off
set t1=%gml%
set t2=%gmllib%
set t3=%gmlpag%
if [%gml%]==[] set gml=r:\bench\tools\wgml
if [%gmllib%]==[] set gmllib=r:\bench\tools\syslib
if [%gmlpag%]==[] set gmlpag=%tmpdir%
rem set gmlinc=path1;path2
set gmlopt=nb7x9ps
set gmlswt=%2 %3 %4 %5 %6
%gml% %1 (file %gmlopt% index pass 2 cpi 10 stat verb output (t:520) %gmlswt%
if [%t1%]==[] set gml=
if [%t2%]==[] set gmllib=
if [%t3%]==[] set gmlpag=
set gmlinc=
set gmlopt=
set gmlswt=
set t1=
set t2=
set t3=
