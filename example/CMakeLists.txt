add_executable(example main.cpp)
target_include_directories(example PRIVATE ${fftw_SOURCE_DIR}/api)
target_link_libraries(example fftw3f portaudio phase-vocoder)
