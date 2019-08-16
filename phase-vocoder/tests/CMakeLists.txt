add_executable(
	tests 
	OverlapExtractTests.cpp
	InterpolateFramesTests.cpp
	FirFilterTests.cpp
	OverlapAddFilterTests.cpp
	OverlapAddTests.cpp
	ExpandTests.cpp
)
target_compile_features(tests PRIVATE cxx_std_17)
set_target_properties(tests PROPERTIES CXX_EXTENSIONS OFF)
if (MSVC)
	target_compile_options(tests PRIVATE /W4 /WX)
else()
	target_compile_options(tests PRIVATE -Wall -Wextra -pedantic -Werror)
endif()
target_link_libraries(tests phase-vocoder fftw3 gtest_main)
add_test(NAME tests COMMAND tests)