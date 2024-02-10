cmake_minimum_required (VERSION 2.8)
project (fst_m1_rdv_snowman)

include(CheckCXXCompilerFlag)
include(FetchContent)

function(enable_cxx_compiler_flag_if_supported flag)
    string(FIND "${CMAKE_CXX_FLAGS}" "${flag}" flag_already_set)
    if(flag_already_set EQUAL -1)
        check_cxx_compiler_flag("${flag}" flag_supported)
        if(flag_supported)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}" PARENT_SCOPE)
        endif()
        unset(flag_supported CACHE)
    endif()
endfunction()

enable_cxx_compiler_flag_if_supported("-Wall")
enable_cxx_compiler_flag_if_supported("-Wextra")
enable_cxx_compiler_flag_if_supported("-pedantic")
enable_cxx_compiler_flag_if_supported("-std=c++17")
enable_cxx_compiler_flag_if_supported("-O3")
enable_cxx_compiler_flag_if_supported("-fopenmp")

file(GLOB SOURCES *.h *.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

# stb
FetchContent_Declare(
        stb
        GIT_REPOSITORY https://github.com/nothings/stb
)
FetchContent_GetProperties(stb)
if(NOT stb_POPULATED)
    FetchContent_Populate(stb)
endif()
target_include_directories(${PROJECT_NAME} PRIVATE ${stb_SOURCE_DIR})
