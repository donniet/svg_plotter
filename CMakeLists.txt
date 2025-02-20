cmake_minimum_required(VERSION 3.10)
project(svg_plotter VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(TBB REQUIRED)

include_directories(include)
link_libraries(TBB::tbb)


include(CTest)
enable_testing()

add_subdirectory(test)

# install(TARGETS svg_parser DESTINATION bin)

