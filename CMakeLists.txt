cmake_minimum_required(VERSION 3.2)

project(minjsc LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
# Useful debug options for the default build
add_compile_options(-g -fno-exceptions)
# lots of warnings and errors.
add_compile_options(-Wall -Wextra -Werror -pedantic)

include_directories("${PROJECT_SOURCE_DIR}/include")
add_subdirectory("${PROJECT_SOURCE_DIR}/src/lib")
add_subdirectory("${PROJECT_SOURCE_DIR}/src/bin")

# build the test executable
find_package(doctest REQUIRED)

# Make test executable
add_executable(tests tests/main.cpp)
target_compile_features(tests PRIVATE cxx_std_20)
target_link_libraries(tests PRIVATE doctest::doctest)# build the main minjsc executable

# build the binary
add_executable(minjsc src/bin/main.cpp)
target_include_directories(minjsc PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(minjsc libminijsc)

target_link_libraries(libminijsc ${extra_libs})

# setup clang-tidy
# clang-tidy
if (NOT DEFINED CLANG_TIDY_BIN)
    # attempt to find the binary if user did not specify
    find_program(CLANG_TIDY_BIN
            NAMES clang-tidy clang-tidy-12)
endif()
if ("${CLANG_TIDY_BIN}" STREQUAL "CLANG_TIDY_BIN-NOTFOUND")
    message(WARNING "minjsc/main couldn't find clang-tidy.")
else()
    # Output compile_commands.json
    set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
    message(STATUS "minjsc/main found clang-tidy at ${CLANG_TIDY_BIN}")
endif()
