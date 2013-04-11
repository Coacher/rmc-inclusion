project(rmc-ig)

set(RMC_LINK_LIBRARIES rmc gmp)

set(rmc-ig_SOURCES
    constants.c
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

add_subdirectory(utils)
