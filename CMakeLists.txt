cmake_minimum_required(VERSION 3.14)
project(
  PhaseVocoder
  VERSION 0.0.1
  LANGUAGES CXX)
option(ENABLE_TESTS "Enable tests" OFF)
if(${ENABLE_TESTS})
  enable_testing()
endif()

include(FetchContent)

FetchContent_Declare(
  GSL
  GIT_REPOSITORY https://github.com/microsoft/GSL
  GIT_TAG v3.1.0)
FetchContent_MakeAvailable(GSL)

add_library(
  phase-vocoder
  src/OverlapAdd.cpp src/OverlapAddFilter.cpp src/OverlapExtract.cpp
  src/InterpolateFrames.cpp src/SignalConverter.cpp)
target_include_directories(phase-vocoder PUBLIC include)
target_include_directories(phase-vocoder PRIVATE include/phase-vocoder)
target_compile_options(phase-vocoder PRIVATE ${PHASE_VOCODER_WARNINGS})
target_link_libraries(phase-vocoder GSL)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest
  GIT_TAG release-1.10.0)
FetchContent_MakeAvailable(googletest)

add_subdirectory(tests)
