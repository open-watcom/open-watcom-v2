:TITLEP.
..if '&target' = 'PC' ..th ..do begin
    :TITLE.&company. C
    :TITLE.Language Reference
:CMT.    :TITLE.(Draft)
..do end
..el ..if '&target' = 'PC 370' ..th ..do begin
    :TITLE.C Language Reference
    :TITLE.for
    :TITLE.&company. C
    :TITLE.and
    :TITLE.Waterloo C
..do end
:CMT. :TITLE.
:CMT. :TITLE.including Programmer's Guide
:INCLUDE file='DOCTITLE'.
:eTITLEP.
