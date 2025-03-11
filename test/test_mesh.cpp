
#include "mesh.hpp"
#include "stroke_mesh.hpp"
#include "output/stl.hpp"
#include "output/obj.hpp"
#include "output/json_mesh.hpp"
#include "parser.hpp"
#include "plotter.hpp"
#include "fills.hpp"
#include "attribute_mesh.hpp"

#include <iostream>
#include <format>
#include <cmath>

#include "testing.hpp"

using std::sqrt;

int main(int ac, char * av[])
{
    // vector<Event> stroke;
    // stroke.insert(stroke.end(), {
    //     Event(Point(0,0), Vector(1,0), 0),
    //     Event(Point(1,0), Vector(1,0), 1)
    // });

    StrokeMesh mesher(50);

    // auto mesh = mesher(stroke);
    // std::cout << JSONMeshOutput(mesh.first);

    SVGPath greenland(R"(M-11.608,540.851H21.74l76.801,10.737l21.221,18.54l10.105,19.526l31.027,20.497l8.469,13.093l17.239,10.76   c0,0,8.229,20.121,0,20.584s-194.169,2.924-194.169,2.924L-11.608,540.851z)");
    Plotter plotter;

    // Lines lines(greenland.bounding_box(), 50, Vector(1./sqrt(2), 1./sqrt(2)));

    // auto stroke = plotter.fill(greenland, lines);
    auto stroke = plotter.plot(greenland);
    
    auto mesh = mesher.create_mesh(stroke);

    std::cout << JSONAttributeMeshOutput(mesh);

    // std::cout << OBJOutput(mesh.first);
    // for(auto const & m : mesh)
    //     std::cout << STLOutput(m);

    return 0;
}

