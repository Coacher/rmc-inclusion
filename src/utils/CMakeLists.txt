project(utilities)

set(RMC_LINK_LIBRARIES rmc gmp)

set(UTILITIES
    M_RM_diff
    M_RM_print
    RM_collision
    u_calculator
)

foreach(UTILITY ${UTILITIES})
    add_executable(${UTILITY} ${UTILITY}.c)
    target_link_libraries(${UTILITY} ${RMC_LINK_LIBRARIES})

    install(TARGETS ${UTILITY}
        RUNTIME DESTINATION ${BIN_INSTALL_DIR}
        COMPONENT binary
    )
endforeach(UTILITY ${UTILITIES})
