cmake_minimum_required(VERSION 3.14)
project(PhaseVocoder VERSION 0.0.1 LANGUAGES CXX)
option(ENABLE_TESTS "Enable tests" OFF)
if (${ENABLE_TESTS})
    enable_testing()
endif()

include(FetchContent)

FetchContent_Declare(
    GSL
    GIT_REPOSITORY https://github.com/microsoft/GSL
    GIT_TAG v2.0.0
)
FetchContent_MakeAvailable(GSL)

add_library(phase-vocoder INTERFACE)
target_include_directories(phase-vocoder INTERFACE include)
target_link_libraries(phase-vocoder INTERFACE GSL)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest
    GIT_TAG release-1.10.0
)
FetchContent_MakeAvailable(googletest)

add_subdirectory(tests)