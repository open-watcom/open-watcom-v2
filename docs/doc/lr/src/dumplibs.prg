clear
get DOS16.LST
sort text
erase $save_filename
output $save_filename
clear
get DOS32.LST
sort text
erase $save_filename
output $save_filename
clear
get DOSPM.LST
sort text
erase $save_filename
output $save_filename
clear
get WIN16.LST
sort text
erase $save_filename
output $save_filename
clear
get WIN386.LST
sort text
erase $save_filename
output $save_filename
clear
get WIN32.LST
sort text
erase $save_filename
output $save_filename
clear
get NET32.LST
sort text
select text<>prev(text)
erase $save_filename
output $save_filename
clear
get OS216.LST
sort text
erase $save_filename
output $save_filename
clear
get OS216DL.LST
sort text
erase $save_filename
output $save_filename
clear
get OS216MT.LST
sort text
erase $save_filename
output $save_filename
clear
get OS232.LST
sort text
erase $save_filename
output $save_filename
clear
get QNX16.LST
sort text
erase $save_filename
output $save_filename
clear
get QNX32.LST
sort text
erase $save_filename
output $save_filename
clear
get MATH.LST
sort text
erase $save_filename
output $save_filename
clear
get KANJI.LST
sort text
erase $save_filename
output $save_filename
clear
get GRAPH.LST
sort text
erase $save_filename
output $save_filename
clear
get MACRO.LST
sort text
erase $save_filename
output $save_filename
clear
get dos16.lst
redef text=30
def dos16=6
calc dos16='DOS16'
combine dos32.lst, text
def dos32=6
mark src='BOTH' | src='FILE'
calc dos32='DOS32'
rem mrk
rem src
combine dospm.lst, text
def dospm=6
mark src='BOTH' | src='FILE'
calc dospm='DOSPM'
rem mrk
rem src
combine win16.lst, text
def win16=6
mark src='BOTH' | src='FILE'
calc win16='WIN16'
rem mrk
rem src
combine win386.lst, text
def win386=7
mark src='BOTH' | src='FILE'
calc win386='WIN386'
rem mrk
rem src
combine win32.lst, text
def win32=6
mark src='BOTH' | src='FILE'
calc win32='WIN32'
rem mrk
rem src
combine os216.lst, text
def os216=6
mark src='BOTH' | src='FILE'
calc os216='OS216'
rem mrk
rem src
combine os216dl.lst, text
def os216dl=8
mark src='BOTH' | src='FILE'
calc os216dl='OS216DL'
rem mrk
rem src
combine os216mt.lst, text
def os216mt=8
mark src='BOTH' | src='FILE'
calc os216mt='OS216MT'
rem mrk
rem src
combine os232.lst, text
def os232=6
mark src='BOTH' | src='FILE'
calc os232='OS232'
rem mrk
rem src
combine qnx16.lst, text
def qnx16=6
mark src='BOTH' | src='FILE'
calc qnx16='QNX16'
rem mrk
rem src
combine qnx32.lst, text
def qnx32=6
mark src='BOTH' | src='FILE'
calc qnx32='QNX32'
rem mrk
rem src
combine net32.lst, text
def net32=6
mark src='BOTH' | src='FILE'
calc net32='NET32'
rem mrk
rem src
combine math.lst, text
def math=5
mark src='BOTH' | src='FILE'
calc math='MATH'
rem mrk
rem src
combine macro.lst, text
def macro=6
mark src='BOTH' | src='FILE'
calc macro='MACRO'
rem mrk
rem src
combine kanji.lst, text
def kanji=6
mark src='BOTH' | src='FILE'
calc kanji='KANJI'
rem mrk
rem src
combine graph.lst, text
def graph=6
mark src='BOTH' | src='FILE'
calc graph='GRAPH'
rem mrk
rem src
keep t,DOS1,DOS3,WIN1,WIN38,WIN32,QNX1,QNX3,OS216,OS216M,OS216D,OS23,DOSP,N,...
mark graph='GRAPH'
calc dos16='DOS16'
calc dos32='DOS32'
calc qnx16='QNX16'
calc qnx32='QNX32'
rem mrk_
rem graph
rem kanji
def start=5n
def name=30
calc name=text
mark name@'_'
calc start=index(name,'_')
calc name=name[1:start-1];name[start+1:]
mark name@'_'
calc start=index(name,'_')
calc name=name[1:start-1];name[start+1:]
mark name@'_'
calc start=index(name,'_')
calc name=name[1:start-1];name[start+1:]
mark name@'_'
calc start=index(name,'_')
calc name=name[1:start-1];name[start+1:]
mark name@'_'
calc start=index(name,'_')
calc name=name[1:start-1];name[start+1:]
rem mrk_
sort name
rem name,start
config listing file "dumplibs.lst" pagewidth 150 pageheight 2000
report
