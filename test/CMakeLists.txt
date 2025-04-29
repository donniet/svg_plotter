
add_executable(test_parser test_parser.cpp)
target_link_libraries(test_parser parser)
add_test(parser test_parser)

add_executable(test_filler test_filler.cpp)
target_link_libraries(test_filler fills plotter)
add_test(filler test_filler)

add_executable(test_ray_intersect test_ray_intersect.cpp)
target_link_libraries(test_ray_intersect utility)
add_test(ray_intersect test_ray_intersect)

add_executable(test_plotter test_plotter.cpp)
target_link_libraries(test_plotter plotter parser adapters)
add_test(plotter test_plotter)

add_executable(test_adapters test_adapters.cpp)
target_link_libraries(test_adapters adapters)
add_test(adapters test_adapters)

add_executable(test_mesh test_mesh.cpp)
target_link_libraries(test_mesh mesh plotter parser fills plot_cover stroke_mesh)
add_test(mesh test_mesh)

add_executable(test_mesh_cover test_mesh_cover.cpp)
target_link_libraries(test_mesh_cover cover output parser plotter)
add_test(mesh_cover test_mesh_cover)

add_executable(test_line test_line.cpp)
target_link_libraries(test_line point)
add_test(line test_line)

add_executable(test_point test_point.cpp)
target_link_libraries(test_point point)
add_test(point test_point)

add_executable(test_shape_output test_shape_output.cpp)
target_link_libraries(test_shape_output mesh plotter parser fills plot_cover stroke_mesh)
add_test(shape_output test_shape_output)

add_executable(stl_converter stl_converter.cpp)
target_link_libraries(stl_converter mesh)

