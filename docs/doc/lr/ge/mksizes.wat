% Program for WATFILE/Plus V5.0
def dummy=6
def lines=2n
def name=16
input x.x
select name<>'--Total'
def output=40
calc lines=lines+1
calc output=".if '&*1' eq '";%name;"' .cp ";lines
keep output
erase gr_sizes.gml
output gr_sizes.gml
