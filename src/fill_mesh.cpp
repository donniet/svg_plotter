#include "fill_mesh.hpp"
#include "earcut.hpp"
#include "utility.hpp"

#include <array>

using std::string;
using std::vector;
using std::pair;
using std::array;
using std::size_t;

using mapbox::earcut;


void MeshFill::fill(string const &name, 
                    vector<vector<Point>>const & plot,
                    FillStyle style,
                    std::pair<double,double> time_range = {0., 1.})
{
    FillData d{
        .name = name,
        // .style = style,
        .time_range = time_range,
        .draw_mode = DrawMode::triangles,
        .vertex_range = {0UL, 0UL}
    };

    vector<size_t> indices = earcut<size_t>(plot);


    for(size_t i = 0; i < indices.size(); i += 3)
    {
        array<pair<size_t, size_t>, 3> corner_indices;


        for(size_t l = 0; l < 3; l++)
            corner_indices[l] = parse_poly_index(plot, indices[i + l]);

        Vector n{0, 0};

        // if two of these will be from the same path in the plot
        // create a normal vector to them as our second attribute

        if(corner_indices[0].first == corner_indices[1].first == corner_indices[2].first)
        {
            size_t N = plot[corner_indices[0].first].size();

            // all three points come from the same path
            if(corner_indices[1].second == (corner_indices[0].second + 1) % N)
            {

            }
            else if(corner_indices[2].second == (corner_indices[1].second + 1) % N)
            {
                
            }
            else if(corner_indices[0].second == (corner_indices[2].second + 1) % N)
            {
                
            }
            else    
            {
                // all three points are non-sequential
            }
        }
        else if(corner_indices[0].first == corner_indices[1].first)
        {

        }
        else if(corner_indices[1].first == corner_indices[2].first)
        {

        }
        else if(corner_indices[2].first == corner_indices[0].first)
        {

        }
        else 
        {
            // all three points come from different paths
        }

        // calculate the distance to the nearest edge
        // Point nearby = closest_plot_point_to(plot, p);

        _mesh.append(p, nearby - p);

    }

}
