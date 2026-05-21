set(CONFIG_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/libmutex)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/libmutexConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/libmutexConfig.cmake"
    INSTALL_DESTINATION ${CONFIG_INSTALL_DIR}
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/libmutexConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY ${VERSION_CHECK_METHOD}
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/libmutexConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/libmutexConfigVersion.cmake"
    DESTINATION ${CONFIG_INSTALL_DIR}
)