#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>

#include <cstdint>

#ifdef __cplusplus
using std::size_t;

namespace lib 
{
#endif 

    struct Section
    {
        GLint begin;
        GLint end;
        double arclength;
    };

    struct Stroke
    {
        double arclength;
        
        GLenum draw_mode;
        size_t section_count;
        Section * sections;

        GLsizeiptr buffer_size;
        void * buffer;
    };

    #ifdef __cplusplus
    extern "C" 
    {
    #endif

    size_t painterStrokeCount();
    int painterGetStroke(size_t id, Stroke const **);

    #ifdef __cplusplus
    }
    #endif

#ifdef __cplusplus
} // namespace lib
#endif