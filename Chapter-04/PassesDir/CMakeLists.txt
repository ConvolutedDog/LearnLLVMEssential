add_library(FnNamePrintPass
  MODULE
  FnNamePrint.cpp
  )

set_target_properties(FnNamePrintPass PROPERTIES COMPILER_FLAGS "-fno-rtti")
