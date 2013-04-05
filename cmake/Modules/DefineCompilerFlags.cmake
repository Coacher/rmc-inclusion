# define system dependent compiler flags

if (UNIX AND NOT WIN32)
    #
    # Define GNUCC compiler flags
    #
    if (${CMAKE_C_COMPILER_ID} MATCHES "(GNU|Clang)")

        # add -Wconversion ?
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -std=gnu99 -pedantic")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wshadow -Wmissing-prototypes -Wdeclaration-after-statement")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wunused -Wfloat-equal -Wpointer-arith -Wwrite-strings -Wformat-security")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wmissing-format-attribute")

    endif (${CMAKE_C_COMPILER_ID} MATCHES "(GNU|Clang)")

endif (UNIX AND NOT WIN32)
