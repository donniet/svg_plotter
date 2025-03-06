add_library(point point.cpp)

add_library(cover cover.cpp)
target_link_libraries(cover point)

add_library(drawable drawable.cpp)
target_link_libraries(drawable cover)

add_library(adapters adapters.cpp)
target_link_libraries(adapters drawable)

add_library(mesh mesh.cpp)
target_link_libraries(mesh point)

add_library(path path.cpp)
target_link_libraries(path drawable)

add_library(fills fills.cpp)
target_link_libraries(fills path)

add_library(parser parser.cpp)
target_link_libraries(parser path bezier cover)

add_library(stroke stroke.cpp)
target_link_libraries(stroke point)

add_library(plotter plotter.cpp)
target_link_libraries(plotter stroke cover)

add_library(bezier bezier.cpp)
target_link_libraries(bezier point)

add_library(utility utility.cpp)
target_link_libraries(utility point)

