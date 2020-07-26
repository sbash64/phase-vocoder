add_executable(
  tests
  OverlapExtractTests.cpp
  InterpolateFramesTests.cpp
  OverlapAddFilterTests.cpp
  OverlapAddTests.cpp
  SignalConverterTests.cpp
  SampleRateConverterTests.cpp
  HannWindowTests.cpp)
target_compile_features(tests PRIVATE cxx_std_17)
target_compile_options(tests PRIVATE ${PHASE_VOCODER_WARNINGS})
set_target_properties(tests PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(tests phase-vocoder gtest_main GSL)
add_test(NAME tests COMMAND tests)
