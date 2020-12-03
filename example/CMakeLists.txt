option(SBASH64_PHASE_VOCODER_USE_ALSA_REALTIME_SCHEDULING
       "Use ALSA realtime scheduling" OFF)
if(${SBASH64_PHASE_VOCODER_USE_ALSA_REALTIME_SCHEDULING})
  set(SBASH64_PHASE_VOCODER_EXAMPLE_MAIN main-alsa.cpp)
else()
  set(SBASH64_PHASE_VOCODER_EXAMPLE_MAIN main-default.cpp)
endif()

add_executable(sbash64-phase-vocoder-example
               mono-live-vocoding.cpp ${SBASH64_PHASE_VOCODER_EXAMPLE_MAIN})
target_include_directories(sbash64-phase-vocoder-example
                           PRIVATE ${fftw_SOURCE_DIR}/api)
target_link_libraries(sbash64-phase-vocoder-example fftw3f portaudio
                      sbash64-phase-vocoder)
target_compile_options(sbash64-phase-vocoder-example
                       PRIVATE ${SBASH64_PHASE_VOCODER_WARNINGS})
set_target_properties(sbash64-phase-vocoder-example PROPERTIES CXX_EXTENSIONS
                                                               OFF)

# See Craig Scott's CppCon 2019 talk, "Deep CMake for Library Authors".
# https://github.com/CppCon/CppCon2019/blob/master/Presentations/deep_cmake_for_library_authors/deep_cmake_for_library_authors__craig_scott__cppcon_2019.pdf

# "Installing Libraries With CMake"
include(GNUInstallDirs)
install(
  TARGETS sbash64-phase-vocoder-example
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT PhaseVocoder_RunTime
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
          COMPONENT PhaseVocoder_RunTime
          NAMELINK_COMPONENT PhaseVocoder_Development
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
          COMPONENT PhaseVocoder_Development)
