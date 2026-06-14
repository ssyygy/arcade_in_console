cmake_minimum_required(VERSION 3.14...4.0)
project(arcade_in_console VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_VERSION VERSION_GREATER_EQUAL "4.0")
    set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
endif()

add_executable(arcade_in_console
    main.cpp
    enemies.cpp
    bullets.cpp
    players.cpp
    game_utils.cpp
)

enable_testing()

include(FetchContent)
FetchContent_Declare(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG v2.4.12
)
FetchContent_MakeAvailable(doctest)

add_executable(game_tests
    test.cpp
    enemies.cpp
    bullets.cpp
    players.cpp
    game_utils.cpp
)

target_link_libraries(game_tests doctest::doctest)

add_test(NAME all_tests COMMAND game_tests)
