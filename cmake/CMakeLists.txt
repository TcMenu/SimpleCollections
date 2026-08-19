cmake_minimum_required(VERSION 3.13)

add_library(SimpleCollections
        ../src/SCCircularBuffer.cpp
        ../src/SCThreadingSupport.cpp
        ../src/SimpleCollections.cpp
)

if (BUILD_FOR_STM32CUBE)
    target_compile_definitions(SimpleCollections
            PUBLIC BUILD_FOR_STM32CUBE_CMAKE=1 IO_LOGGING_DEBUG=1
    )
else ()
    target_compile_definitions(SimpleCollections
            PUBLIC BUILD_FOR_PICO_CMAKE=1 BUILD_PICO_FORCE_UART=1 IO_LOGGING_DEBUG=1
    )
endif ()

target_include_directories(SimpleCollections PUBLIC
        ../src
)

# only bring in pico libs when compiling for PicoSDK
if(NOT BUILD_FOR_STM32CUBE)
    message("Build is pico, adding simple libs")
    target_link_libraries(SimpleCollections PUBLIC pico_stdlib pico_sync)
endif()
