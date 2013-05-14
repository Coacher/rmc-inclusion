set(RMC_LINK_LIBRARIES rmc gmp)
add_library(constants OBJECT constants.c)


project(rmc-ig)

set(rmc-ig_SOURCES
    info.c
    graph.c
    rmc-ig.c
)

add_executable(rmc-ig ${rmc-ig_SOURCES} $<TARGET_OBJECTS:constants>)
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

add_executable(rmc-short ${rmc-short_SOURCES} $<TARGET_OBJECTS:constants>)
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

add_executable(rmc-diff ${rmc-diff_SOURCES} $<TARGET_OBJECTS:constants>)
target_link_libraries(rmc-diff ${RMC_LINK_LIBRARIES})

install(TARGETS rmc-diff
    RUNTIME DESTINATION ${BIN_INSTALL_DIR}
    COMPONENT binary
)


project(utilities)

set(UTILITIES
    M_RM_diff
    M_RM_print
    Rads_print
    RM_collision
    u_calculator
)

foreach (UTILITY ${UTILITIES})
    add_executable(${UTILITY} utils/${UTILITY}.c $<TARGET_OBJECTS:constants>)
    target_link_libraries(${UTILITY} ${RMC_LINK_LIBRARIES})

    install(TARGETS ${UTILITY}
        RUNTIME DESTINATION ${BIN_INSTALL_DIR}
        COMPONENT binary
    )
endforeach (UTILITY ${UTILITIES})
