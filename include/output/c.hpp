#include "output.hpp"

#include "mesh.hpp"

#include <string>
#include <iomanip>


class COutputer : 
    public Outputer
{
private:
    std::vector<Triangle> const * _triangles;
    std::string _name;

public:
    void add_solid(Cover const &, Style const &);
    void add_stroke(Path const &, Style const &);

public:
    COutputer(std::vector<Triangle> const & tris, std::string name = "stencil") :
        _triangles(&tris), _name(name)
    { }

    virtual void print(std::ostream & os) const override
    {
        os << "#include \"stencil.h\"\n\n";

        os  << "Stencil stencils[] = {{\n"
            << "\t.name = \"" << _name << "\",\n"
            << "\t.mode = TRIANGLES,\n" 
            << "\t.vertex_range = { 0, " << _triangles->size() * 3 << ",\n"
            << "}};\n\n";

        os  << "uint stencil_count = 1;\n\n";

        os  << "Attribute stencil_attributes[] = {{\n"
            << "\t.name = \"a_position\",\n"
            << "\t.size = 2,\n"
            << "\t.offset = 0,\n"
            << "}};\n\n";

        os  << "float const stencil_buffer[] = {\n";
        for(Triangle const & tri : *_triangles)
        {
            os  << "\t"
                << std::setw(8) << std::setprecision(2) << std::fixed << tri.p0.x << ","
                << std::setw(8) << std::setprecision(2) << std::fixed << tri.p0.y << ",\n";
            os  << "\t"
                << std::setw(8) << std::setprecision(2) << std::fixed << tri.p1.x << ","
                << std::setw(8) << std::setprecision(2) << std::fixed << tri.p1.y << ",\n";
            os  << "\t"
                << std::setw(8) << std::setprecision(2) << std::fixed << tri.p2.x << ","
                << std::setw(8) << std::setprecision(2) << std::fixed << tri.p2.y << ",\n";
        }

        os  << "};\n\n";

        os  << "uint stencil_buffer_size = " << _triangles->size() * 3 << ";\n"
            << "uint stencil_stride = 2;\n\n";
            
        os.flush();
    }
};