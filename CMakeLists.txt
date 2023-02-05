# ---------------------------------------------------------------------------- #

cmake_minimum_required(VERSION 3.16)
include(FetchContent)

project(padll VERSION 1.0.0 DESCRIPTION "padll: ...")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Setup the options that CMake can take in
option(PADLL_INSTALL "Install PADLL's header and library" ON)
option(PADLL_BUILD_TESTS "Build PADLL's unit tests" OFF)
option(PADLL_BUILD_BENCHMARKS "Build benchmarks" ON)
option(FETCH_FROM_GIT "Fetch PAIO repo from github" OFF)

# Path to (local) PAIO lib
set(PAIO_LOCAL_PATH "/path/to/paio/build")

# Setup the basic C++ Compiler flags
if (APPLE)
message(STATUS "Detecting Local Environment (apple-clang)")
    option(IS_LOCAL "Install PADLL in local setup" ON)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -fPIC")
endif (APPLE)

if (UNIX AND NOT APPLE)
    message(STATUS "Detecting UNIX")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -pthread -fPIC -Wl,--no-as-needed -ldl")
endif ()

# Install PADLL to be used in production. This option will modify the 
# compilation options, regarding compiler-based optimizations.
option(PRODUCTION "Compiling with release mode" ON)

if (PRODUCTION) 
    message(STATUS "Compiling with compiler-based optimizations (-O3)")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
else() 
    message(STATUS "Compiling with default compiling options")
endif (PRODUCTION)

# Test whether -Wthread-safety is available. See
# https://clang.llvm.org/docs/ThreadSafetyAnalysis.html
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-Wthread-safety HAVE_CLANG_THREAD_SAFETY)

# ---------------------------------------------------------------------------- #
# padll

add_library(padll SHARED "")

set_target_properties(
        padll
        PROPERTIES
        # CXX_VISIBILITY_PRESET hidden
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
)

target_include_directories(padll PUBLIC include)

if (HAVE_CLANG_THREAD_SAFETY)
    target_compile_options(padll PUBLIC -Wthread-safety)
endif (HAVE_CLANG_THREAD_SAFETY)

find_package(Threads REQUIRED)
target_link_libraries(padll Threads::Threads)

target_sources(
    padll
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include/padll/configurations/libc_calls.hpp
    ${PROJECT_SOURCE_DIR}/include/padll/interface/ldpreloaded/ld_preloaded_posix.hpp
    ${PROJECT_SOURCE_DIR}/include/padll/interface/ldpreloaded/dlsym_hook_libc.hpp
    ${PROJECT_SOURCE_DIR}/include/padll/interface/native/posix_file_system.hpp
    ${PROJECT_SOURCE_DIR}/include/padll/interface/passthrough/posix_passthrough.hpp
    ${PROJECT_SOURCE_DIR}/include/padll/library_headers/libc_enums.hpp
    ${PROJECT_SOURCE_DIR}/include/padll/library_headers/libc_headers.hpp
    ${PROJECT_SOURCE_DIR}/include/padll/options/options.hpp
    ${PROJECT_SOURCE_DIR}/include/padll/stage/data_plane_stage.hpp
    ${PROJECT_SOURCE_DIR}/include/padll/stage/mount_point_entry.hpp
    ${PROJECT_SOURCE_DIR}/include/padll/stage/mount_point_table.hpp
    ${PROJECT_SOURCE_DIR}/include/padll/statistics/statistic_entry.hpp
    ${PROJECT_SOURCE_DIR}/include/padll/statistics/statistics.hpp
    ${PROJECT_SOURCE_DIR}/include/padll/utils/log.hpp
    ${PROJECT_SOURCE_DIR}/include/padll/third_party/enum.h
    ${PROJECT_SOURCE_DIR}/include/padll/third_party/tabulate.hpp
    ${PROJECT_SOURCE_DIR}/include/padll/third_party/xoshiro.hpp
)

target_sources(
        padll
        PRIVATE
        src/interface/ldpreloaded/ld_preloaded_posix.cpp
        src/interface/native/posix_file_system.cpp
        src/interface/passthrough/posix_passthrough.cpp
        src/stage/data_plane_stage.cpp
        src/stage/mount_point_entry.cpp
        src/stage/mount_point_table.cpp
        src/statistics/statistic_entry.cpp
        src/statistics/statistics.cpp
        src/utils/log.cpp
)

# ---------------------------------------------------------------------------- #
# paio -- Paio library

message(STATUS "Installing PAIO data plane library")

if (FETCH_FROM_GIT)
    message(STATUS "Fetching libpaio from git")
    FetchContent_Declare(paio
        GIT_REPOSITORY  https://github.com/dsrhaslab/paio.git
        GIT_TAG         origin/main
    )

    FetchContent_MakeAvailable(paio)
    target_link_libraries(padll paio)
else()
    message(STATUS "Finding libpaio in local environment")
    find_library(PAIO_LIBRARY paio HINTS ${PAIO_LOCAL_PATH})
    target_link_libraries(padll ${PAIO_LIBRARY})
endif()

# ---------------------------------------------------------------------------- #
# spdlog -- logging library

if(${CMAKE_VERSION} VERSION_LESS "3.24.0") 
    FetchContent_Declare(spdlog
        URL             https://github.com/gabime/spdlog/archive/v1.8.1.tar.gz
        UPDATE_COMMAND  ""
        INSTALL_COMMAND ""
    )
else ()
    FetchContent_Declare(spdlog
        URL             https://github.com/gabime/spdlog/archive/v1.8.1.tar.gz
        UPDATE_COMMAND  ""
        INSTALL_COMMAND ""
        DOWNLOAD_EXTRACT_TIMESTAMP NEW
    )
endif ()

FetchContent_MakeAvailable(spdlog)

set_target_properties(spdlog PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_link_libraries(padll spdlog)

# ---------------------------------------------------------------------------- #
# tests

if (PADLL_BUILD_TESTS)
    enable_testing()

    function(padll_test test_file executable_name)
        get_filename_component(test_target_name "${executable_name}" NAME_WE)

        add_executable("${test_target_name}" "")
        target_sources("${test_target_name}"
                PRIVATE
                "${test_file}"
                )

        target_link_libraries("${test_target_name}" padll)

        add_test(NAME "${test_target_name}" COMMAND "${test_target_name}")
    endfunction(padll_test)

    padll_test("tests/padll_mount_point_differentiation_test.cpp" "mountpoint_test")
    padll_test("tests/padll_paio_integration_test.cpp" "stage_integration_test")
    padll_test("tests/padll_simulate_macro_test.cpp" "macro_test")
    padll_test("tests/padll_simulate_micro_test.cpp" "micro_test")
    padll_test("tests/padll_statistics_test.cpp" "statistics_test")
    padll_test("tests/padll_xoshiro_test.cpp" "xoshiro_bench")

    padll_test("tests/posix/simple_test.cpp" "simple_test")
    padll_test("tests/posix/hybrid_calls_test.cpp" "hybrid_test")
    padll_test("tests/posix/directory_calls_test.cpp" "dir_test")
    padll_test("tests/posix/extended_attributes_calls_test.cpp" "xattr_test")


endif (PADLL_BUILD_TESTS)

if (PADLL_BUILD_BENCHMARKS)
    enable_testing()

    function(padll_benchmarks test_file executable_name)
        get_filename_component(test_target_name "${executable_name}" NAME_WE)

        add_executable("${test_target_name}" "")
        target_sources("${test_target_name}"
                PRIVATE
                "${test_file}"
                )

        target_link_libraries("${test_target_name}")

        add_test(NAME "${test_target_name}" COMMAND "${test_target_name}")
    endfunction(padll_benchmarks)

    padll_benchmarks("benchmarking/padll_scalability_benchmark.cpp" "padll_scalability_bench")

endif (PADLL_BUILD_BENCHMARKS)

# ---------------------------------------------------------------------------- #
# install

if (PADLL_INSTALL)
    include(GNUInstallDirs)
    install(
            TARGETS padll
            EXPORT padllTargets
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
    install(DIRECTORY include/padll TYPE INCLUDE)
endif (PADLL_INSTALL)

# ---------------------------------------------------------------------------- #
