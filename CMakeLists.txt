cmake_minimum_required(VERSION 3.14)
project(hashfile
        VERSION 0.1
        LANGUAGES C
)

option(ASAN "" off)

if(MSVC AND (NOT DEFINED ENV{VCINSTALLDIR}) OR (NOT DEFINED ENV{VCToolsVersion}))
    message(FATAL_ERROR "Please run CMake in a Visual Studio command prompt to build.")
endif()

# for debugging use
if(ASAN AND (CMAKE_C_COMPILER_ID MATCHES "Clang"))
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address,undefined")
else()
    message(WARNING "ASan is only supported on Clang")
endif()

if(MINGW)
    message(WARNING "Using MinGW will result in wildcard support being disabled.")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -municode")
endif()

add_executable(hashfile
    src/hash.c
    src/progress.c
    src/main.c
    src/util.c
)
target_link_libraries(hashfile bcrypt)
target_link_libraries(hashfile ntdll)
if(MSVC)
    target_link_libraries(hashfile "$ENV{VCINSTALLDIR}\\Tools\\MSVC\\$ENV{VCToolsVersion}\\lib\\x64\\wsetargv.obj")
endif()