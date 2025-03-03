
#include "mesh.hpp"
#include "stroke_mesh.hpp"
#include "output/stl.hpp"
#include "output/obj.hpp"

#include <iostream>
#include <format>

#include "testing.hpp"

int main(int ac, char * av[])
{
    vector<Event> stroke;
    stroke.insert(stroke.end(), {
        Event(Point(0,0), Vector(1,0), 0),
        Event(Point(1,0), Vector(1,0), 1)
    });

    StrokeMesh stroker(0.5);

    auto mesh = stroker(stroke);

    std::cout << OBJOutput(mesh.first);

    return 0;
}

