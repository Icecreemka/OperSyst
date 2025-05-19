cmake_minimum_required(VERSION 3.14)

# Название проекта
project(MarkerThreadsProject LANGUAGES CXX)

# Устанавливаем стандарт C++98
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Пути к исходникам
set(SOURCES
    main.cpp
    marker.cpp
    thread.cpp
)

# Создаем основной исполняемый файл
add_executable(main_exec ${SOURCES})

# --------------------
# Google Test настройки
# --------------------

enable_testing()

# Укажи путь к установленному GTest
set(GTEST_ROOT "C:/Users/user/Documents/GTest/googletest/install")
find_package(GTest REQUIRED HINTS ${GTEST_ROOT} NO_DEFAULT_PATH)

include_directories(${GTEST_ROOT}/include)

# Тестовый исполняемый файл
add_executable(runTests
    tests.cpp
    marker.cpp
    thread.cpp
)

set_target_properties(runTests PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

# Линковка с Google Test
target_link_libraries(runTests
    ${GTEST_ROOT}/lib/gtest.lib
    ${GTEST_ROOT}/lib/gtest_main.lib
    kernel32
)

# Тест запуска
add_test(NAME MarkerThreadsTests COMMAND runTests)
