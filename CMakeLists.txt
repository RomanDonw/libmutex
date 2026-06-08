cmake_minimum_required(VERSION 3.23)

# ========== SETTINGS ==========
project(mutex VERSION 0.2.0 LANGUAGES C)

set(VERSION_CHECK_METHOD SameMajorVersion)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
# ========== ======== ==========

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

# add target for build

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.h" "src/*.c")
add_library(${PROJECT_NAME} ${SOURCES})
unset(SOURCES)

target_compile_definitions(${PROJECT_NAME}
    PRIVATE $<$<BOOL:${DEBUG}>:LIBMUTEX_DEBUG>
    PRIVATE $<$<BOOL:${BUILD_SHARED_LIBS}>:LIBMUTEX_EXPORT>
    PUBLIC $<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:LIBMUTEX_STATIC>
)

# setup includes & required libs

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

if(UNIX)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _GNU_SOURCE _XOPEN_SOURCE=500)
    target_link_libraries(${PROJECT_NAME} INTERFACE pthread)
endif()

# setup correct installation process

install(TARGETS ${PROJECT_NAME}
    EXPORT libmutexTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(DIRECTORY "include/" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/configure.cmake")

install(EXPORT libmutexTargets
    FILE libmutexTargets.cmake
    NAMESPACE libmutex::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/libmutex
)
