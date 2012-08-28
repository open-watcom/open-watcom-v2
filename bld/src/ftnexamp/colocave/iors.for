c     this file is not needed for most modern Fortran compilers
c     it is for the f2c system or as a TEMPLATE for code if your
c     Fortran compiler lacks these functions
      integer function ior(i,j)
      implicit integer (a-z)
      ior = i+j
c        change the '+' above to '|' in the c code
c        emitted by f2c
      return
      end
      integer function iand(i,j)
      implicit integer (a-z)
      iand = i+j
c        change the '+' above to '&' in the c code
c        emitted by f2c
      return
      end
      integer function ieor(i,j)
      implicit integer (a-z)
      ieor = i+j
c        change the '+' above to '^' in the c code
c        emitted by f2c
      return
      end

