# Where noted code is copied from Craig Scott's CppCon 2019 talk, "Deep CMake
# for Library Authors".
# https://github.com/CppCon/CppCon2019/blob/master/Presentations/deep_cmake_for_library_authors/deep_cmake_for_library_authors__craig_scott__cppcon_2019.pdf

option(PHASE_VOCODER_USE_ALSA_REALTIME_SCHEDULING
       "Use ALSA realtime scheduling" OFF)
if(${PHASE_VOCODER_USE_ALSA_REALTIME_SCHEDULING})
  set(PHASE_VOCODER_EXAMPLE_MAIN main-alsa.cpp)
else()
  set(PHASE_VOCODER_EXAMPLE_MAIN main-default.cpp)
endif()

# "Setting Install RPATH Details"
if(NOT APPLE)
  set(CMAKE_INSTALL_RPATH $ORIGIN/../${CMAKE_INSTALL_LIBDIR})
endif()

add_executable(phase-vocoder-example mono-live-vocoding.cpp
                                     ${PHASE_VOCODER_EXAMPLE_MAIN})
target_include_directories(phase-vocoder-example PRIVATE ${fftw_SOURCE_DIR}/api)
target_link_libraries(phase-vocoder-example fftw3f portaudio phase-vocoder)
target_compile_options(phase-vocoder-example PRIVATE ${PHASE_VOCODER_WARNINGS})
set_target_properties(phase-vocoder-example PROPERTIES CXX_EXTENSIONS OFF)

# "Installing Libraries With CMake"
include(GNUInstallDirs)
install(
  TARGETS phase-vocoder-example phase-vocoder
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT PhaseVocoder_RunTime
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
          COMPONENT PhaseVocoder_RunTime
          NAMELINK_COMPONENT PhaseVocoder_Development
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
          COMPONENT PhaseVocoder_Development)
