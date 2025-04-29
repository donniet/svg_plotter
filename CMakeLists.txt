cmake_minimum_required(VERSION 3.10)
project(svg_plotter VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)


# find_package(TBB REQUIRED)

include_directories(include)
include_directories(/home/donniet/src/stl_reader)
# link_libraries(TBB::tbb)

# hide compiler warnings for now
if ( CMAKE_COMPILER_IS_GNUCC )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas")
endif()

include(CTest)
enable_testing()

add_subdirectory(src)
add_subdirectory(test)

# install(TARGETS svg_parser DESTINATION bin)

