      logical l
      l =.true.
      print '(1x,z8)', l
      l =.false.
      print '(1x,z8)', l
      print *, .TRUE. .NEQV. .TRUE., .TRUE. .XOR. .TRUE.
      print *, .TRUE. .NEQV. .FALSE., .TRUE. .XOR. .FALSE.
      print *, .FALSE. .NEQV. .FALSE., .FALSE. .XOR. .FALSE.
      end
