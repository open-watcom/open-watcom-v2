:: Key word used by header source files
::
:: BITS16       header file supports 16-bit target (now DOS and QNX)
::
:: INTEL_ONLY   header file supports only Intel architecture (now RDOS and QNX)
::
:: UNICODE      header file supports UNICODE (now DOS)
::
:::::::::::::::::::::
:segment DOS
::
:keep BITS16
:keep UNICODE
:::::::::::::::::::::
:elsesegment LINUX
::
:::::::::::::::::::::
:elsesegment QNX
::
:keep INTEL_ONLY
:keep BITS16
::
:::::::::::::::::::::
:elsesegment RDOS
::
:keep INTEL_ONLY
::
:::::::::::::::::::::
:endsegment
