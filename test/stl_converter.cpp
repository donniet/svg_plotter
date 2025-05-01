#include "point.hpp"

#include <iostream>
#include <vector>
#include <filesystem>
#include <string>
#include <utility>
#include <sstream>

#include <stl_reader.h>

#include "stdlib.h"

using std::cout, std::cerr, std::cin, std::endl;
using std::size_t;
using std::vector;
using std::string;
using std::pair;
using std::istringstream;
using std::noskipws;

#define DEFAULT_TIME_START (0.)
#define DEFAULT_TIME_END (1.)
#define POSITION_ATTRIBUTE_NAME "a_position"

template<typename T>
bool parse(string const & s, T & value)
{
    std::istringstream iss(s);
    iss >> noskipws >> value; // noskipws considers leading whitespace invalid
    // Check the entire string was consumed and if either failbit or badbit is set
    return iss.eof() && !iss.fail(); 
}

std::string stem_of(const std::string& path) 
{
    namespace fs = std::filesystem;

    fs::path p(path);
    return p.stem().string();
}

std::string extension_of(const std::string& path) 
{
    namespace fs = std::filesystem;

    fs::path p(path);
    return p.extension().string();
}

void help_and_exit()
{
    cerr << "usage: ./stl_converter filename.stl [solid_name] [shader_name] [time_start] [time_end]" << endl;

    exit(-1);
}

struct Solid
{
    string name;
    string shader;
    size_t index;
    pair<size_t, size_t> vertex_range;
    pair<double,double> time_range;
};


// ./stl_converter ../../examples/mesh/itsfive.stl
int main(int ac, char * av[])
{
    if(ac < 2)
        help_and_exit();

    double time_start, time_end;


    vector<Point> vertices;
    vector<Solid> solids;
    size_t total_vertices = 0;

    for(int i = 1; i < ac; i++)
    {
        int n = 1;

        stl_reader::StlMesh<double, size_t> mesh(av[i]);

        std::string name = stem_of(av[i]);
        if(i + n < ac && extension_of(av[i+n]) != ".stl")
        {
            name = av[i+n];
            ++n;
        }

        std::string shader = name;

        if(i + n < ac && extension_of(av[i+n]) != ".stl")
        {
            shader = av[i+n];
            ++n;
        }

        time_start = DEFAULT_TIME_START;
        time_end = DEFAULT_TIME_END;

        if(i + n < ac && parse(av[i+n], time_start))
            n++;

        if(i + n < ac && parse(av[i+n], time_end))
            n++;

        i += n;

        for(size_t isolid = 0; isolid < mesh.num_solids(); ++isolid)
        {
            size_t vertex_start = total_vertices;

            for(size_t itri = mesh.solid_tris_begin(isolid); itri < mesh.solid_tris_end(isolid); ++itri)
            {
                for(size_t icorner = 0; icorner < 3; ++icorner, ++total_vertices)
                {
                    const double * c = mesh.tri_corner_coords(itri, icorner);

                    vertices.emplace_back(c[0], c[1] /* , c[2] */);
                }
            }

            solids.push_back({
                .name = name,
                .shader = shader,
                .index = isolid,
                .vertex_range = { vertex_start, total_vertices },
                .time_range = { time_start, time_end },
            });
        }
    }

    cout << "#include \"solid.h\"\n\n";

    cout << "Solid solids[] = {\n";
    for(auto const & solid : solids)
    {
        cout << "\t{\n"
             << "\t\t.name = \"" << solid.name << "\",\n"
             << "\t\t.index = " << solid.index << ",\n"
             << "\t\t.mode = TRIANGLES,\n"
             << "\t\t.vertex_range = {" << solid.vertex_range.first << ", " << solid.vertex_range.second << "},\n"
             << "\t\t.time_range = {" << solid.time_range.first << ", " << solid.time_range.second << "},\n"
             << "\t\t.shader = \"" << solid.shader << "\",\n"
             << "\t},\n";
    }
    cout << "};\n";
    cout << "const uint solid_count = " << solids.size() << ";\n\n";

    cout << "Attribute const _attributes[] = {\n"
         << "\t{\n"
         << "\t\t.name = \"" << POSITION_ATTRIBUTE_NAME << "\",\n"
         << "\t\t.size = 2,\n"
         << "\t\t.offset = 0,\n"
         << "\t},\n"
         << "};\n";
    
    cout << "uint _attribute_count = 1;\n\n";

    cout << "const float solid_buffer[] = {\n";
    for(auto const & p : vertices)
    {
        cout << "\t" << p.x << ", " << p.y << ",\n";
    }
    cout << "};\n\n";

    cout << "const uint solid_buffer_size = " << total_vertices * 2 << ";\n";
    cout << "const uint solid_stride = 8;\n\n";

    return 0;
}