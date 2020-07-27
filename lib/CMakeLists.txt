add_library(
  phase-vocoder OverlapAdd.cpp OverlapAddFilter.cpp OverlapExtract.cpp
                InterpolateFrames.cpp SignalConverter.cpp)
target_include_directories(phase-vocoder PUBLIC include)
target_include_directories(phase-vocoder PRIVATE include/phase-vocoder)
target_compile_options(phase-vocoder PRIVATE ${PHASE_VOCODER_WARNINGS})
set_target_properties(phase-vocoder PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(phase-vocoder GSL)
