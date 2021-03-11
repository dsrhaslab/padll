# ---------------------------------------------------------------------------- #

cmake_minimum_required(VERSION 3.11)
include(FetchContent)

project(ldpaio VERSION 1.0.0 DESCRIPTION "ldpaio: ...")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Setup the options that CMake can take in
option(LDPAIO_INSTALL "Install ldpaio's header and library" ON)
option(LDPAIO_BUILD_LDPAIO_TESTS "Build ldpaio's unit tests" ON)
option(LDPAIO_BUILD_LIBC_TESTS "Build libc tests" ON)

# Setup the basic C++ Compiler flags
if (Apple)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fPIC")
endif (Apple)

if (Linux)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fPIC -Wl,--no-as-needed -ldl")
endif (Linux)


# Test whether -Wthread-safety is available. See
# https://clang.llvm.org/docs/ThreadSafetyAnalysis.html
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-Wthread-safety HAVE_CLANG_THREAD_SAFETY)

# ---------------------------------------------------------------------------- #
# ldpaio

add_library(ldpaio SHARED "")

set_target_properties(
        ldpaio
        PROPERTIES
        # CXX_VISIBILITY_PRESET hidden
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
)

target_include_directories(ldpaio PUBLIC include)

if (HAVE_CLANG_THREAD_SAFETY)
    target_compile_options(ldpaio PUBLIC -Wthread-safety)
endif (HAVE_CLANG_THREAD_SAFETY)

# needed by (LevelDB's?) port_stdcxx.h
find_package(Threads REQUIRED)
target_link_libraries(ldpaio Threads::Threads)

target_sources(
    ldpaio
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include/ldpaio/interface/posix_file_system.hpp
    ${PROJECT_SOURCE_DIR}/include/ldpaio/interface/posix_passthrough.hpp
    ${PROJECT_SOURCE_DIR}/include/ldpaio/statistics/statistic_entry.hpp
    ${PROJECT_SOURCE_DIR}/include/ldpaio/statistics/statistics.hpp
    ${PROJECT_SOURCE_DIR}/include/ldpaio/utils/logging.hpp
    ${PROJECT_SOURCE_DIR}/include/ldpaio/utils/operation_utils.hpp
    ${PROJECT_SOURCE_DIR}/include/ldpaio/utils/options.hpp
    ${PROJECT_SOURCE_DIR}/include/ldpaio/thrid_party/enum.h
)

target_sources(
        ldpaio
        PRIVATE
        src/interface/posix_file_system.cpp
        src/interface/posix_passthrough.cpp
        src/statistics/statistic_entry.cpp
        src/statistics/statistics.cpp
        src/utils/logging.cpp
)

# ---------------------------------------------------------------------------- #
# spdlog -- logging library

FetchContent_Declare(spdlog
        URL https://github.com/gabime/spdlog/archive/v1.8.1.tar.gz
        UPDATE_COMMAND ""
        INSTALL_COMMAND ""
        )

FetchContent_MakeAvailable(spdlog)

set_target_properties(spdlog PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_link_libraries(ldpaio spdlog)

# ---------------------------------------------------------------------------- #
# tests
if (LDPAIO_BUILD_LIBC_TESTS)
    enable_testing()

    function(ldpaio_test test_file executable_name)
        get_filename_component(test_target_name "${executable_name}" NAME_WE)

        add_executable("${test_target_name}" "")
        target_sources("${test_target_name}"
                PRIVATE
                "${test_file}"
                )

        target_link_libraries("${test_target_name}")

        add_test(NAME "${test_target_name}" COMMAND "${test_target_name}")
    endfunction(ldpaio_test)

    ldpaio_test("tests/ldpaio_test.cpp" "ldpaio_test")
    ldpaio_test("tests/ldpaio_directory_calls_test.cpp" "dir_test")
    ldpaio_test("tests/ldpaio_extended_attributes_calls_test.cpp" "xattr_test")

endif (LDPAIO_BUILD_LIBC_TESTS)

if (LDPAIO_BUILD_LDPAIO_TESTS)
    enable_testing()

    function(ldpaio_test test_file)
        get_filename_component(test_target_name "${test_file}" NAME_WE)

        add_executable("${test_target_name}" "")
        target_sources("${test_target_name}"
                PRIVATE
                "${test_file}"
                )

        target_link_libraries("${test_target_name}" ldpaio)

        add_test(NAME "${test_target_name}" COMMAND "${test_target_name}")
    endfunction(ldpaio_test)

    ldpaio_test("tests/ldpaio_statistics_test.cpp")

endif (LDPAIO_BUILD_LDPAIO_TESTS)

# ---------------------------------------------------------------------------- #
# install

if (LDPAIO_INSTALL)
    include(GNUInstallDirs)
    install(
            TARGETS ldpaio
            EXPORT ldpaioTargets
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
    install(DIRECTORY include/ldpaio TYPE INCLUDE)
endif (LDPAIO_INSTALL)

# ---------------------------------------------------------------------------- #
