      structure /address/
	  character*20 street
	  character*15 city
	  union
	      map
		  character*20 province
	      end map
	      map
		  character*20 state
	      end map
	  end union
	  character*20 country
	  union
	      map
		  character*7 postal_code
	      end map
	      map
		  character*10 zip_code
	      end map
	  end union
      end structure

      structure /people/
	  character*20 name
	  record /address/ addr
	  integer*2 age
      end structure

      external CreatePerson
      record /people/ CreatePerson, person

      person = CreatePerson()
      print *, 'Name:', person%name
      print *, 'Address:', person%addr%street
      print *, '        ', person%addr%city
      if( person%addr%country .eq. 'Canada' )then
	  print *, '        ', person%addr%province
	  print *, '        ', person%addr%country
	  print *, '        ', person%addr%postal_code
      else
	  print *, '        ', person%addr%state
	  print *, '        ', person%addr%country
	  print *, '        ', person%addr%zip_code
      endif
      print *, 'Age:', person%age

      end

      record /people/ function CreatePerson()

      structure /address/
	  character*20 street
	  character*15 city
	  union
	      map
		  character*20 province
	      end map
	      map
		  character*20 state
	      end map
	  end union
	  character*20 country
	  union
	      map
		  character*7 postal_code
	      end map
	      map
		  character*10 zip_code
	      end map
	  end union
      end structure

      structure /people/
	  character*20 name
	  record /address/ addr
	  integer*2 age
      end structure

      CreatePerson%name = 'Elmar Pugsley'
      CreatePerson%age = 27
      CreatePerson%addr%street = '123 Main Street'
      CreatePerson%addr%city = 'Waterloo'
      CreatePerson%addr%province = 'Ontario'
      CreatePerson%addr%country = 'Canada'
      CreatePerson%addr%postal_code = 'N2L 2X2'

      end
