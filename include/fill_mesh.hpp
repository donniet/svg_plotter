#ifndef __FILL_MESH_HPP__
#define __FILL_MESH_HPP__

#include "point.hpp"
#include "attribute_mesh.hpp"

struct FillStyle 
{
    RGBA color;
};

struct FillData
{
    std::string name;
    FillStyle style;
    std::pair<double,double> time_range;


    std::pair<double, double> time_range;
    DrawMode draw_mode;
    std::pair<size_t, size_t> vertex_range;
    std::pair<size_t, size_t> section_range;
};


class MeshFill
{
public:
    typedef AttributeMesh<Vector> mesh_type;

    static inline constexpr const char * attribute_names[] = {
        "a_position", "a_nearest_edge"
    };


    MeshFill() = default;
    MeshFill(MeshFill const &) = default;
    MeshFill(MeshFill &&) = default;

    
private:

    mesh_type _mesh;
    std::vector<FillData> _fills;

    void fill(std::string const & name, 
              std::vector<std::vector<Point>> const & plot,
              FillStyle style,
              std::pair<double,double> time_range = {0., 1.});

};


#endif