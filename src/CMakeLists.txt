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


project(rmc-beautifier)

set(rmc-beautifier_SOURCES
    graph.c
    rmc-beautifier.c
)

add_executable(rmc-beautifier ${rmc-beautifier_SOURCES} $<TARGET_OBJECTS:constants>)
target_link_libraries(rmc-beautifier ${RMC_LINK_LIBRARIES})

install(TARGETS rmc-beautifier
    RUNTIME DESTINATION ${BIN_INSTALL_DIR}
    COMPONENT binary
)


project(utilities)

set(UTILITIES
    M_RM_diff
    M_RM_print
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
