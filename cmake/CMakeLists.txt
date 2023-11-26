add_library(SimpleCollections
        ../src/SCCircularBuffer.cpp
        ../src/SCThreadingSupport.cpp
        ../src/SimpleCollections.cpp
)

target_compile_definitions(SimpleCollections
        PUBLIC BUILD_FOR_PICO_CMAKE=1 BUILD_PICO_FORCE_UART=1 IO_LOGGING_DEBUG=1
)
target_include_directories(SimpleCollections PUBLIC
        ${PROJECT_SOURCE_DIR}/SimpleCollections/src
)

target_link_libraries(SimpleCollections PUBLIC pico_stdlib pico_sync)
