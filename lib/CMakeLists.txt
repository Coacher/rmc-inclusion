project(librmc)

set(LIBRARY_VERSION "1.4.1")
set(LIBRARY_SOVERSION "1")

set(LIBRMC_LINK_LIBRARIES gmp)

file(GLOB librmc_SOURCES rmc/*.c)

# enable debug support
if (WITH_DEBUG)
    add_definitions(-DENABLE_DEBUG)
endif (WITH_DEBUG)

if (WITH_STATIC_LIB)

    add_library(rmc STATIC ${librmc_SOURCES})

else (WITH_STATIC_LIB)

    add_library(rmc SHARED ${librmc_SOURCES})

    set_target_properties(rmc PROPERTIES
        VERSION ${LIBRARY_VERSION}
        SOVERSION ${LIBRARY_SOVERSION}
    )

endif (WITH_STATIC_LIB)

target_link_libraries(rmc ${LIBRMC_LINK_LIBRARIES})

install(TARGETS rmc
    RUNTIME DESTINATION ${BIN_INSTALL_DIR}
    LIBRARY DESTINATION ${LIB_INSTALL_DIR}
    ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
    COMPONENT libraries
)
