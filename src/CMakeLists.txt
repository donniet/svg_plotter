add_library(output output/output.cpp)

add_library(point point.cpp)
target_link_libraries(point utility)

add_library(utility utility.cpp)
target_link_libraries(utility point)

add_library(cover cover.cpp)
target_link_libraries(cover point)

add_library(drawable drawable.cpp)
target_link_libraries(drawable cover)

add_library(adapters adapters.cpp)
target_link_libraries(adapters drawable)

add_library(mesh mesh.cpp)
target_link_libraries(mesh point cover)

# add_library(path path.cpp)
# target_link_libraries(path drawable)

add_library(fills fills.cpp)
target_link_libraries(fills point bezier)

add_library(parser parser.cpp)
target_link_libraries(parser drawable point bezier cover)

add_library(stroke stroke.cpp)
target_link_libraries(stroke point)

add_library(plotter plotter.cpp)
target_link_libraries(plotter stroke cover)

add_library(bezier bezier.cpp)
target_link_libraries(bezier point)

add_library(plot_cover plot_cover.cpp)
target_link_libraries(plot_cover cover utility plotter)

add_library(stroke_mesh stroke_mesh.cpp)
target_link_libraries(stroke_mesh stroke mesh output)

add_library(fill_mesh fill_mesh.cpp)
target_link_libraries(fill_mesh stroke mesh output utility)

add_library(svg_path svg_path.cpp)
target_link_libraries(svg_path )

set(OpenGL_GL_PREFERENCE LEGACY)
find_package(OpenGL REQUIRED)

add_library(cpath path_lib.cpp)
target_link_libraries(cpath OpenGL::GL)

