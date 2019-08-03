set(
    TESTS
        "OverlapExtractTests.cpp"
		"InterpolateFramesTests.cpp"
		"FirFilterTests.cpp"
)
add_executable(tests ${TESTS})
if (MSVC)
	target_compile_options(tests PRIVATE /W4 /WX)
else()
	target_compile_options(tests PRIVATE -Wall -Wextra -pedantic -Werror)
endif()
target_link_libraries(tests phase-vocoder gtest_main)
add_test(NAME tests COMMAND tests)