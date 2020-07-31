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
  set(ENABLE_FLOAT
      ON
      CACHE BOOL "" FORCE)
  FetchContent_Declare(
    fftw
    URL http://fftw.org/fftw-3.3.8.tar.gz
    URL_MD5 8aac833c943d8e90d51b697b27d4384d)
  FetchContent_MakeAvailable(fftw)

  enable_language(C)
  FetchContent_Declare(
    portaudio
    GIT_REPOSITORY https://git.assembla.com/portaudio.git
    GIT_TAG pa_stable_v190600_20161030)
  FetchContent_MakeAvailable(portaudio)

  add_subdirectory(example)
endif()
