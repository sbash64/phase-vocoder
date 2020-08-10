add_library(
  sbash64-phase-vocoder OverlapAdd.cpp OverlapAddFilter.cpp OverlapExtract.cpp
                        InterpolateFrames.cpp SignalConverter.cpp)
target_include_directories(sbash64-phase-vocoder PUBLIC include)
target_include_directories(sbash64-phase-vocoder
                           PRIVATE include/sbash64/phase-vocoder)
target_compile_options(sbash64-phase-vocoder
                       PRIVATE ${SBASH64_PHASE_VOCODER_WARNINGS})
target_compile_features(sbash64-phase-vocoder PUBLIC cxx_trailing_return_types)
set_target_properties(sbash64-phase-vocoder PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(sbash64-phase-vocoder GSL)
