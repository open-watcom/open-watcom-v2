
        function Random( high )
        integer         high
c
c
c
        include         'life.fi'

        external        urand
        real            urand
        integer         seed /75347/
        Random = urand( seed ) * high

        end
