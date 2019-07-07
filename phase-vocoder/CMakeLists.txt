add_library(phase-vocoder INTERFACE)
target_include_directories(phase-vocoder INTERFACE include)
target_link_libraries(phase-vocoder INTERFACE GSL)
add_subdirectory(tests)