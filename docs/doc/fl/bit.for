      integer i,j
      logical l
      i = 'F846'x
      j = '7F21'x

*  0000F846    1111 1000 0100 0110
*  00007F21    0111 1111 0010 0001

      print '(1x,z8)', .not.i
*  FFFF07B9    0000 0111 1011 1001
      print '(1x,z8)', .not.j
*  FFFF80DE    1000 0000 1101 1110
      print '(1x,z8)', i.and.j
*  00007800
      print '(1x,z8)', i.or.j
*  0000FF67
      print '(1x,z8)', i.eqv.j
*  FFFF7898    0111 1000 1001 1000
      print '(1x,z8)', i.neqv.j
*  00008767    1000 0111 0110 0111
      print '(1x,z8)', i.xor.j
*  00008767    1000 0111 0110 0111
      print *, 1.and.2+3
      if( (1.and.2+3) .eq. (1.and.(2+3)) ) print *,'+>.and.'
      if( (1.and.2+3) .eq. ((1.and.2)+3) ) print *,'.and.>+'
      print *, 3+2.and.1
      if( (3+2.and.1) .eq. ((3+2).and.1) ) print *,'+>.and.'
      if( (3+2.and.1) .eq. (3+(2.and.1)) ) print *,'.and.>+'
      print *, 2.or.1+2
      if( (2.or.1+2) .eq. (2.or.(1+2)) ) print *,'+>.or.'
      if( (2.or.1+2) .eq. ((2.or.1)+2) ) print *,'.or.>+'
      print *, 2+1.or.2
      if( (2+1.or.2) .eq. ((2+1).or.2) ) print *,'+>.or.'
      if( (2+1.or.2) .eq. (2+(1.or.2)) ) print *,'.or.>+'
      print *, 1.or.2.and.6
      if( (1.or.2.and.6) .eq. (1.or.(2.and.6)) ) print *,'.and.>.or.'
      if( (1.or.2.and.6) .eq. ((1.or.2).and.6) ) print *,'.or.>.and.'
      print *, 6.and.2.or.1
      if( (6.and.2.or.1) .eq. ((6.and.2).or.1) ) print *,'.and.>.or.'
      if( (6.and.2.or.1) .eq. (6.and.(2.or.1)) ) print *,'.or.>.and.'
      end
