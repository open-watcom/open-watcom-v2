      integer function __fdll_initialize_()
      integer __fdll_terminate_, dll_entry

      integer WORKING_SIZE
      parameter ( WORKING_SIZE = 16*1024 )
      integer ierr, WorkingStorage
      dimension WorkingStorage(:)

      allocate( WorkingStorage(WORKING_SIZE), stat=ierr )
      if( ierr .eq. 0 )then
          __fdll_initialize_ = 1
      else
          __fdll_initialize_ = 0
      endif
      return

      entry __fdll_terminate_()
      deallocate( WorkingStorage )
      __fdll_terminate_ = 1
      return

      entry dll_entry()
      ! use WorkingStorage
      return

      end
