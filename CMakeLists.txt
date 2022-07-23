cmake_minimum_required(VERSION 3.10)

project(HContainers)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
	add_link_options(/DEBUG:FULL)
	add_compile_options(/Oi /GR- /D_HAS_EXCEPTIONS=0)
endif()

file(GLOB_RECURSE HSOURCES HContainers/*.hpp HContainers/*.cpp)

add_executable(HContainers ${HSOURCES})
target_include_directories(HContainers PUBLIC HContainers)

target_compile_definitions(HContainers PUBLIC NOMINMAX)