% Program for WATFILE/Plus V5.2a
def start=5n
def stop=5n
def name=30
calc start=index(text,' ')+1
calc stop=index(text,' ')+index(text[index(text,' ')+1:],' ')
calc name=text[start:stop-start]
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
rem name,start,stop
