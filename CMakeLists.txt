cmake_minimum_required(VERSION 3.14)
project(
  PhaseVocoder
  VERSION 1.0.0
  LANGUAGES CXX)

include(FetchContent)

FetchContent_Declare(
  GSL
  GIT_REPOSITORY https://github.com/microsoft/GSL
  GIT_TAG v3.1.0)
FetchContent_MakeAvailable(GSL)

add_subdirectory(lib)

option(ENABLE_TESTS "Enable tests" OFF)
if(${ENABLE_TESTS})
  enable_testing()

  set(gtest_force_shared_crt
      ON
      CACHE BOOL "" FORCE)
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest
    GIT_TAG release-1.10.0)
  FetchContent_MakeAvailable(googletest)

  add_subdirectory(tests)
endif()

option(PHASE_VOCODER_ENABLE_EXAMPLE "Enable example" OFF)
if(${PHASE_VOCODER_ENABLE_EXAMPLE})
  add_subdirectory(example)
endif()
