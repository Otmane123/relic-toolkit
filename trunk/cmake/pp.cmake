message(STATUS "Bilinear pairings arithmetic configuration (PP module):\n")

message("   ** Options for the bilinear pairing module (default = on):")
message("      PP_PARAL=[off|on] Parallel implementation.\n")

option(PP_PARAL "Parallel implementation." off)

message("   ** Available bilinear pairing methods (default = BASIC;O-ATE):")

message("      PP_METHD=BASIC    Basic quadratic extension field arithmetic.")    
message("      PP_METHD=INTEG    Quadratic extension field arithmetic with embedded modular reduction.\n")

message("      PP_METHD=BASIC    Basic extension field arithmetic.")    
message("      PP_METHD=LAZYR    Lazy reduced extension field arithmetic.\n")

message("      PP_METHD=R_ATE    R-ate pairing.")
message("      PP_METHD=O_ATE    O-ate pairing.")
message("      PP_METHD=X_ATE    X-ate pairing.\n")

# Choose the arithmetic methods.
if (NOT PP_METHD)
	set(PP_METHD "BASIC;BASIC;O_ATE")
endif(NOT PP_METHD)
list(LENGTH PP_METHD PP_LEN)
if (PP_LEN LESS 3)
	message(FATAL_ERROR "Incomplete PP_METHD specification: ${PP_METHD}")
endif(PP_LEN LESS 3)

list(GET PP_METHD 0 PP_QUD)
list(GET PP_METHD 1 PP_EXT)
list(GET PP_METHD 2 PP_MAP)
set(PP_METHD ${PP_METHD} CACHE STRING "Bilinear pairings arithmetic method.")