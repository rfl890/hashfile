cmake_minimum_required(VERSION 3.14)
project(hashfile
        VERSION 0.1
        LANGUAGES C
)

option(ASAN "" on)

if (MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /STACK:4194304")
elseif (MINGW AND ("${CMAKE_C_COMPILER_ID}" MATCHES "Clang"))
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--stack,4194304 -municode")
else()
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-z,stack-size=4194304 -municode")
endif()

add_executable(hashfile
    src/hash.c
    src/progress.c
    src/main.c
)
target_link_libraries(hashfile bcrypt)