set(CONFIG_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/libnthread)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/libnthreadConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/libnthreadConfig.cmake"
    INSTALL_DESTINATION ${CONFIG_INSTALL_DIR}
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/libnthreadConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY ${VERSION_CHECK_METHOD}
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/libnthreadConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/libnthreadConfigVersion.cmake"
    DESTINATION ${CONFIG_INSTALL_DIR}
)