set(
    SOURCES 
        "src/OverlapExtract.cpp"
		"include/phase-vocoder/OverlapExtract.h"
)

add_library(phase-vocoder ${SOURCES})
target_include_directories(phase-vocoder PUBLIC include)
target_compile_options(phase-vocoder PRIVATE /W4 /WX)
#target_compile_definitions(phase-vocoder PRIVATE PHASEVOCODER_EXPORTS)
install(
    TARGETS phase-vocoder 
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
add_subdirectory(tests)