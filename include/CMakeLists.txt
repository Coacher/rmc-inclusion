project(headers)

file(GLOB rmc_HEADERS rmc/*.h)

install(
    FILES ${rmc_HEADERS}
    DESTINATION ${INCLUDE_INSTALL_DIR}/${APPLICATION_NAME}
    COMPONENT headers
)
