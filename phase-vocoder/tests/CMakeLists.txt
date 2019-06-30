set(
    TESTS
        "OverlapExtractTests.cpp"
)
add_executable(tests ${TESTS})
target_link_libraries(tests gtest_main)
target_compile_options(tests PRIVATE /W4 /WX)