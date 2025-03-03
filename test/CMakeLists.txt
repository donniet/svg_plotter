
add_executable(test_parser test_parser.cpp)
add_test(parser test_parser)

add_executable(test_filler test_filler.cpp)
add_test(filler test_filler)

add_executable(test_ray_intersect test_ray_intersect.cpp)
add_test(ray_intersect, test_ray_intersect)

add_executable(test_plotter test_plotter.cpp)
add_test(plotter test_plotter)

add_executable(test_adapters test_adapters.cpp)
add_test(adapters test_adapters)

add_executable(test_mesh test_mesh.cpp)
add_test(mesh test_mesh)