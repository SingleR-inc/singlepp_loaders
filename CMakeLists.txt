cmake_minimum_required(VERSION 3.14)

project(singlepp_loaders
    VERSION 0.1.1
    DESCRIPTION "Load pre-prepared reference datasets for SingleR"
    LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Library
add_library(singlepp_loaders INTERFACE)
add_library(singler::singlepp_loaders ALIAS singlepp_loaders)

target_include_directories(singlepp_loaders INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/singlepp_loaders>)
target_compile_features(singlepp_loaders INTERFACE cxx_std_17)

# Dependencies
option(SINGLEPP_LOADERS_FETCH_EXTERN "Automatically fetch singlepp_loaders's external dependencies." ON)
if(SINGLEPP_LOADERS_FETCH_EXTERN)
    add_subdirectory(extern)
else()
    find_package(singler_singlepp 2.0.0 CONFIG REQUIRED)
    find_package(tatami_tatami 3.0.0 CONFIG REQUIRED)
    find_package(ltla_byteme 1.2.0 CONFIG REQUIRED)
endif()

target_link_libraries(singlepp_loaders INTERFACE singler::singlepp tatami::tatami ltla::byteme)

option(SINGLEPP_LOADERS_FIND_ZLIB "Automatically look for ZLib installation" ON)
if (SINGLEPP_LOADERS_FIND_ZLIB)
    find_package(ZLIB)
    if (ZLIB_FOUND)
        target_link_libraries(singlepp_loaders INTERFACE ZLIB::ZLIB)
    endif()
endif()

# Tests
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    option(SINGLEPP_LOADERS_TESTS "Build singlepp_loaders's test suite." ON)
else()
    option(SINGLEPP_LOADERS_TESTS "Build singlepp_loaders's test suite." OFF)
endif()

if(SINGLEPP_LOADERS_TESTS)
    include(CTest)
    if(BUILD_TESTING)
        add_subdirectory(tests)
    endif() 
endif()

# Install
install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/singlepp_loaders)

install(TARGETS singlepp_loaders
    EXPORT singlepp_loadersTargets)

install(EXPORT singlepp_loadersTargets
    FILE singler_singlepp_loadersTargets.cmake
    NAMESPACE singler::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/singler_singlepp_loaders)

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/singler_singlepp_loadersConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/singler_singlepp_loaders)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/singler_singlepp_loadersConfigVersion.cmake"
    COMPATIBILITY SameMajorVersion)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/singler_singlepp_loadersConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/singler_singlepp_loadersConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/singler_singlepp_loaders)
