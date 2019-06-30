set(
    TESTS
        "OverlapExtractTests.cpp"
)
add_executable(tests ${TESTS})
target_compile_options(tests PRIVATE /W4 /WX)
target_link_libraries(tests phase-vocoder gtest_main)