cmake_minimum_required(VERSION 3.5)
project(PhaseVocoder)
option(ENABLE_TESTS "Enable tests" OFF)
if (${ENABLE_TESTS})
    enable_testing()
endif()
find_library(FFTW_LIB fftw3)
if(NOT FFTW_LIB)
    include(ExternalProject)

    MESSAGE(STATUS "Trying to install fftw...")

    ExternalProject_Add(project_fftw
        GIT_REPOSITORY  https://github.com/FFTW/fftw3
        URL http://www.fftw.org/fftw-3.3.8.tar.gz
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/fftw
        CONFIGURE_COMMAND ${CMAKE_CURRENT_BINARY_DIR}/fftw/src/project_fftw/configure --prefix=${CMAKE_CURRENT_BINARY_DIR}/fftw/install
        INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/fftw/install
    )

    add_library(fftw3 STATIC IMPORTED)
    set(lib_fftw_name ${CMAKE_STATIC_LIBRARY_PREFIX}fftw3${CMAKE_STATIC_LIBRARY_SUFFIX})
    set_target_properties(fftw3 PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/fftw/install/lib/${lib_fftw_name})

    target_include_directories(fftw3 PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/fftw/install/include)
endif()
add_subdirectory(googletest)
add_subdirectory(GSL)
add_subdirectory(phase-vocoder)