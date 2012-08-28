c$include pmord.fap

        logical function BYTE_ORDER( oc )
        include 'pmocodes.fi'
        integer oc
        BYTE_ORDER = oc .eq. OCODE_GNOP1 .or. oc .eq. OCODE_GESD
        end

        logical function SHORT_ORDER( oc )
        include 'pmocodes.fi'
        integer oc
        SHORT_ORDER = ( ( oc .xor. OCODE2_1 ) .and. OCODE2_2 ) .eq.
     &                OCODE2_2
        end

        logical function LONG_ORDER( oc )
        include 'pmocodes.fi'
        integer oc
        logical BYTE_ORDER, SHORT_ORDER
        LONG_ORDER = .not. ( ( oc .eq. OCODE_VLONG ) .or.
     &                       BYTE_ORDER( oc ) .or. SHORT_ORDER( oc ) )
        end

        logical function VLONG_ORDER( oc )
        include 'pmocodes.fi'
        integer oc
        VLONG_ORDER = oc .eq. OCODE_VLONG
        end
