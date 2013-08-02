set macro source v {accel {&Module}}
set macro assembly v {accel {&Module}}
set macro all v {dis source;accel {&Module}}
set macro all ? {display watch; accel {&New}}
set macro all e {accelerate main {&Data} {&Memory at}}
set macro all i {accelerate main {&Run} {&Trace into}}
set macro all x {accelerate main {&Run} {&Next sequential}}
set macro all { } {accelerate main {&Run} {&Step over}}
set macro all g {accelerate main {&Run} {E&xecute to}}
set macro all b {accelerate main {&Break} {&New}}
set macro all t {accelerate main {&Break} {&Toggle}}
