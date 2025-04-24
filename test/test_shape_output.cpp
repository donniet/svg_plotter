#include "mesh.hpp"
#include "stroke_mesh.hpp"
#include "output/stl.hpp"
#include "output/obj.hpp"
#include "output/json_mesh.hpp"
#include "parser.hpp"
#include "plotter.hpp"
#include "fills.hpp"
#include "attribute_mesh.hpp"
#include "plot_cover.hpp"

#include <iostream>
#include <format>
#include <cmath>

// #include "testing.hpp"

using std::pair;

int main(int ac, char * av[])
{
    RGBA brush_color = RGBA::from_hex("#135e56");
    pair<double,double> drawing_size{640,816};

    Plotter plotter(5000, {0,1}, 0.5);
    MeshPlot mesher(drawing_size);

    SVGPath p("M 50 408 A 290 290 0 1 1 590 408");

    auto s = plotter.plot(p);
    s = simplify_plot(s, false, 0.01);

    mesher.stroke("stroke", s, BrushStyle::BrushRound, 45., brush_color, {0., 10.});

    // mesher.to_stl(std::cout);
    mesher.to_c(std::cout);

    return 0;
}
