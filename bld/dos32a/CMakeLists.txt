cmake_minimum_required(VERSION 3.15)
project(dos32a VERSION 9.1.2 LANGUAGES C)

enable_testing()

get_filename_component(OW_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)
set(D32_ROOT "${OW_ROOT}/contrib/extender/dos32a")
set(D32_BIN_OUT "${CMAKE_CURRENT_BINARY_DIR}/binw")

if(WIN32)
    set(D32_BUILD_CMD "${CMAKE_CURRENT_SOURCE_DIR}/build.cmd" "${CMAKE_CURRENT_BINARY_DIR}")
else()
    set(D32_BUILD_CMD /bin/sh "${CMAKE_CURRENT_SOURCE_DIR}/build.sh" "${CMAKE_CURRENT_BINARY_DIR}")
endif()

set(D32_ARTIFACTS
    "${D32_BIN_OUT}/dos32a.exe"
    "${D32_BIN_OUT}/dos32a.stb"
    "${D32_BIN_OUT}/dos32a_stock.exe"
    "${D32_BIN_OUT}/stub32a.exe"
    "${D32_BIN_OUT}/stub32c.exe"
    "${D32_BIN_OUT}/sb.exe"
    "${D32_BIN_OUT}/ss.exe"
    "${D32_BIN_OUT}/sc.exe"
    "${D32_BIN_OUT}/sd.exe"
    "${D32_BIN_OUT}/sver.exe"
)

file(GLOB_RECURSE D32_SOURCES
    "${D32_ROOT}/src/*.asm"
    "${D32_ROOT}/src/*.inc"
    "${D32_ROOT}/src/*.inf"
    "${D32_ROOT}/src/*.c"
    "${D32_ROOT}/src/*.h"
    "${D32_ROOT}/h32/*.h"
)

add_custom_command(
    OUTPUT ${D32_ARTIFACTS}
    COMMAND ${D32_BUILD_CMD}
    DEPENDS ${D32_SOURCES} "${CMAKE_CURRENT_SOURCE_DIR}/build.sh" "${CMAKE_CURRENT_SOURCE_DIR}/build.cmd"
    COMMENT "Building MS-DOS DOS/32A v9.1.2 extender, stubs, and tools from assembly/C source"
    VERBATIM
)

add_custom_target(dos32a_all ALL DEPENDS ${D32_ARTIFACTS})

find_program(RUBY_EXECUTABLE ruby)
if(RUBY_EXECUTABLE)
    add_test(
        NAME dos32a_headers_and_reproducibility
        COMMAND "${RUBY_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/test/run_dos32a_tests.rb"
                --mode headers
                --build-dir "${CMAKE_CURRENT_BINARY_DIR}"
    )
    add_test(
        NAME dos32a_msdos_equivalence_and_ablation
        COMMAND "${RUBY_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/test/run_dos32a_tests.rb"
                --mode qemu
                --build-dir "${CMAKE_CURRENT_BINARY_DIR}"
                --benchmark-out "${CMAKE_CURRENT_BINARY_DIR}/benchmark_results.json"
    )
endif()
