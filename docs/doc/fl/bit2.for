      integer i
      logical l
      i = 'F846'x
      print '(1x,z8)', i
      l =.true.
      print *, 'Integer with Logical TRUE'
      print '(1x,z8)', .not.l
      print '(1x,z8)', i.and.l
      print '(1x,z8)', i.or.l
      print '(1x,z8)', i.eqv.l
      print '(1x,z8)', i.neqv.l
      print '(1x,z8)', i.xor.l
      l =.false.
      print *, 'Integer with Logical FALSE'
      print '(1x,z8)', .not.l
      print '(1x,z8)', i.and.l
      print '(1x,z8)', i.or.l
      print '(1x,z8)', i.eqv.l
      print '(1x,z8)', i.neqv.l
      print '(1x,z8)', i.xor.l
      end
