set(
    TESTS
        "OverlapExtractTests.cpp"
		"InterpolateFramesTests.cpp"
		"FirFilterTests.cpp"
)
add_executable(tests ${TESTS})
if (MSVC)
	target_compile_options(tests PRIVATE /W4 /WX)
endif()
target_link_libraries(tests phase-vocoder gtest_main)