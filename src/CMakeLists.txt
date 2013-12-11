set(RMC_LINK_LIBRARIES rmc gmp)


project(rmc-ig)

set(rmc-ig_SOURCES
    info.c
    graph.c
    rmc-ig.c
)

add_executable(rmc-ig ${rmc-ig_SOURCES})
target_link_libraries(rmc-ig ${RMC_LINK_LIBRARIES})

install(TARGETS rmc-ig
    RUNTIME DESTINATION ${BIN_INSTALL_DIR}
    COMPONENT binary
)


project(rmc-short)

set(rmc-short_SOURCES
    graph.c
    rmc-short.c
)

add_executable(rmc-short ${rmc-short_SOURCES})
target_link_libraries(rmc-short ${RMC_LINK_LIBRARIES})

install(TARGETS rmc-short
    RUNTIME DESTINATION ${BIN_INSTALL_DIR}
    COMPONENT binary
)

project(rmc-diff)

set(rmc-diff_SOURCES
    color.c
    rmc-diff.c
)

add_executable(rmc-diff ${rmc-diff_SOURCES})
target_link_libraries(rmc-diff ${RMC_LINK_LIBRARIES})

install(TARGETS rmc-diff
    RUNTIME DESTINATION ${BIN_INSTALL_DIR}
    COMPONENT binary
)


project(utilities)

set(UTILITIES
    M_RadM_diff
    M_RadM_print
    Rad_print
    RadM_collision
)

if (WITH_GMP)
    set(UTILITIES ${UTILITIES} u_calculator)
endif (WITH_GMP)

foreach (UTILITY ${UTILITIES})
    add_executable(${UTILITY} utils/${UTILITY}.c)
    target_link_libraries(${UTILITY} ${RMC_LINK_LIBRARIES})

    install(TARGETS ${UTILITY}
        RUNTIME DESTINATION ${BIN_INSTALL_DIR}
        COMPONENT binary
    )
endforeach (UTILITY ${UTILITIES})
