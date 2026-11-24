cmake_minimum_required(VERSION 3.22)

project(WordSprint LANGUAGES CXX)

include(CTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_PREFIX_PATH)
    if(EXISTS "C:/Qt/6.11.3/msvc2022_64")
        list(APPEND CMAKE_PREFIX_PATH "C:/Qt/6.11.3/msvc2022_64")
    endif()
    if(EXISTS "C:/Qt/6.11.2/mingw_64")
        list(APPEND CMAKE_PREFIX_PATH "C:/Qt/6.11.2/mingw_64")
    endif()
endif()

add_subdirectory(backend)
