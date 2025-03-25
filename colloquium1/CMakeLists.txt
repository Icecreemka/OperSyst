cmake_minimum_required(VERSION 3.14)
project(project LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(project
    main.cpp
    fibonacci.cpp
    palindrome.cpp
    list.cpp
)

enable_testing()
set(GTEST_ROOT "C:/Users/user/Documents/GTest/googletest/install")
find_package(GTest REQUIRED HINTS ${GTEST_ROOT} NO_DEFAULT_PATH)
include_directories(${GTEST_ROOT}/include)

add_executable(runTests
    test.cpp
    fibonacci.cpp
    palindrome.cpp
    list.cpp
)

target_link_libraries(runTests ${GTEST_ROOT}/lib/gtest.lib ${GTEST_ROOT}/lib/gtest_main.lib)

add_test(NAME ProjectTests COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/runTests)
