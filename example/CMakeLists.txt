file(GLOB EXAMPLE_SOURCES *.c *.h)

add_executable(example ${EXAMPLE_SOURCES})

target_link_libraries(example vkmedia)
