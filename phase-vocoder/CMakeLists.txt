set(
    SOURCES 
        "src/OverlapExtract.cpp"
		"include/phase-vocoder/OverlapExtract.h"
)
add_library(phase-vocoder ${SOURCES})
target_include_directories(phase-vocoder PUBLIC include)
target_link_libraries(phase-vocoder GSL)
if (MSVC)
	target_compile_options(phase-vocoder PRIVATE /W4 /WX)
endif()
add_subdirectory(tests)