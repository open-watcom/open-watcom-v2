.* test for .ti controlword
.* co off for one line in = one line out
.co off
.se xx='xxvar'
word1  stillword1 word2
.ti ? &
.ti = |
.ti set \
.ty \?xx.
text\=text\=text\=tex-                 -ext\ text\ text\ text\ text\ text\ text\ text\ text\ text text text text text text  text
.ti set
text\=text\=text\=tex- -ext=text=text=text=text=text text=text=text=text=text
.ti set ›
.ti < [ > ] ( { ) } . "
›.=INDex›<-Queues›>=›(›<Time=(›<mm›>›<,ss›>)›)›>
text›=text›=text›=tex-
