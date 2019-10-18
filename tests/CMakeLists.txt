add_executable(
	tests 
	OverlapExtractTests.cpp
	InterpolateFramesTests.cpp
	FirFilterTests.cpp
	OverlapAddFilterTests.cpp
	OverlapAddTests.cpp
	ExpandTests.cpp
	DecimateTests.cpp
)
target_compile_features(tests PRIVATE cxx_std_17)
set_target_properties(tests PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(tests phase-vocoder fftw3 gtest_main GSL)
add_test(NAME tests COMMAND tests)