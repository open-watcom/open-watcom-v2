% Program for WATFILE/Plus V5.2a
red text=40
def name=20
calc name=text
mark name[1:1]='_'
calc name=text[2:39]
sort name
select prev(name)~=name
rem mrk
rem name
