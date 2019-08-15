cmake_minimum_required(VERSION 3.5)
project(PhaseVocoder)
option(ENABLE_TESTS "Enable tests" OFF)
if (${ENABLE_TESTS})
    enable_testing()
endif()
find_library(FFTW_LIB fftw3)
if(NOT FFTW_LIB)
    # https://cmake.org/pipermail/cmake/2017-May/065490.html
    include(ExternalProject)

    set(FFTW_PREFIX_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/fftw)
    set(FFTW_INSTALL_DIRECTORY ${FFTW_PREFIX_DIRECTORY}/install)

    ExternalProject_Add(project_fftw
        URL http://www.fftw.org/fftw-3.3.8.tar.gz
        PREFIX ${FFTW_PREFIX_DIRECTORY}
        CONFIGURE_COMMAND ${FFTW_PREFIX_DIRECTORY}/src/project_fftw/configure --prefix=${FFTW_INSTALL_DIRECTORY}
        INSTALL_DIR ${FFTW_INSTALL_DIRECTORY}
    )

    add_library(fftw3 STATIC IMPORTED)
    set(FFTW_LIB_NAME ${CMAKE_STATIC_LIBRARY_PREFIX}fftw3${CMAKE_STATIC_LIBRARY_SUFFIX})
    set_target_properties(fftw3 PROPERTIES IMPORTED_LOCATION ${FFTW_INSTALL_DIRECTORY}/lib/${FFTW_LIB_NAME})

    target_include_directories(fftw3 PUBLIC ${FFTW_INSTALL_DIRECTORY}/include)
endif()
add_subdirectory(googletest)
add_subdirectory(GSL)
add_subdirectory(phase-vocoder)