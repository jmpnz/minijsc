cmake_minimum_required(VERSION 3.2)

project(minijsc LANGUAGES CXX)

set(CMAKE_CPP_COMPILER clang++)
set(CMAKE_CXX_STANDARD 20)
# Debug symbols and lots of warnings and errors.
add_compile_options(-g -Wall -Wextra -pedantic)
# warn about unused things
add_compile_options(-Wno-unused-parameter -Wno-unused-function -Wno-sign-conversion)
# headers and third party libraries.
include_directories("${PROJECT_SOURCE_DIR}/include")

include_directories("${PROJECT_SOURCE_DIR}/third_party/fmt/include")
add_definitions("-DFMT_HEADER_ONLY=1")
# source code directories.
add_subdirectory("${PROJECT_SOURCE_DIR}/src/lib")
add_subdirectory("${PROJECT_SOURCE_DIR}/src/bin")

# build the test executable
find_package(doctest REQUIRED)

# build the binary
add_executable(minijsc src/bin/main.cpp)
target_include_directories(minijsc PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(minijsc libminijsc)

# build test executable
add_executable(tests tests/main.cpp)
target_compile_features(tests PRIVATE cxx_std_20)
target_link_libraries(tests PRIVATE libminijsc doctest::doctest)# build the main minijsc executable

target_link_libraries(libminijsc ${extra_libs})

# setup clang-tidy
# clang-tidy
if (NOT DEFINED CLANG_TIDY_BIN)
    # attempt to find the binary if user did not specify
    find_program(CLANG_TIDY_BIN
            NAMES clang-tidy clang-tidy-12)
endif()
if ("${CLANG_TIDY_BIN}" STREQUAL "CLANG_TIDY_BIN-NOTFOUND")
    message(WARNING "minijsc/main couldn't find clang-tidy.")
else()
    # Output compile_commands.json
    set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
    message(STATUS "minijsc/main found clang-tidy at ${CLANG_TIDY_BIN}")
endif()
