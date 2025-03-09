cmake_minimum_required(VERSION 3.14)

# Название проекта
project(LAB2 LANGUAGES CXX)

# Устанавливаем стандарт C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Добавляем исходный файл
add_executable(LAB2 lab2.cpp)

# Добавляем Google Test
enable_testing()
set(GTEST_ROOT "C:/Users/user/Documents/GTest/googletest/install")
find_package(GTest REQUIRED HINTS ${GTEST_ROOT} NO_DEFAULT_PATH)
include_directories(${GTEST_ROOT}/include)

# Создаем исполняемый файл для тестов
add_executable(runTests test.cpp)

# Линкуем тесты с Google Test
target_link_libraries(runTests ${GTEST_ROOT}/lib/gtest.lib ${GTEST_ROOT}/lib/gtest_main.lib)

# Указываем путь к исполняемому файлу тестов
add_test(NAME Lab2Tests COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/runTests)
