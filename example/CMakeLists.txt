option(PHASE_VOCODER_USE_ALSA_REALTIME_SCHEDULING
       "Use ALSA realtime scheduling" OFF)
if(${PHASE_VOCODER_USE_ALSA_REALTIME_SCHEDULING})
  set(PHASE_VOCODER_EXAMPLE_MAIN main-alsa.cpp)
else()
  set(PHASE_VOCODER_EXAMPLE_MAIN main-default.cpp)
endif()

add_executable(phase-vocoder-example mono-live-vocoding.cpp
                                     ${PHASE_VOCODER_EXAMPLE_MAIN})
target_include_directories(phase-vocoder-example PRIVATE ${fftw_SOURCE_DIR}/api)
target_link_libraries(phase-vocoder-example fftw3f portaudio phase-vocoder)
target_compile_options(phase-vocoder-example PRIVATE ${PHASE_VOCODER_WARNINGS})
set_target_properties(phase-vocoder-example PROPERTIES CXX_EXTENSIONS OFF)
