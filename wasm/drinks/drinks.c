#include "drawing.h"

const float anis_gradient_colors_hsl[] = {
	195. / 360., 77. / 100., 95. / 100., 100. / 100.,
	174. / 360., 22. / 100., 64. / 100., 6.   / 100.,
	195. / 360., 77. / 100., 95. / 100., 51.  / 100.
};

const float anis_gradient_colors_rgb[] = {
    232. / 256, 247. / 256, 252. / 256, 100. / 100,
    144. / 256, 184. / 256, 180. / 256,   6. / 100,
    232. / 256, 247. / 256, 252. / 256,  51. / 100,
};

const float anis_gradient_offsets[] = {
	0., 0.46, 1.
};

const float anis_gradient_linear[] = {
	0, 462.6123, // 341, 
	0, 514.28601, // 400, 
};

const float anis_top_gradient_colors_hsl[] = {
	  0. / 360,   0. / 100,  82. / 100,  10. / 100,
	  0. / 360,   0. / 100,  82. / 100, 100. / 100,
};

const float anis_top_gradient_offsets[] = {
	0.2788, 1.,
};

const float anis_top_gradient_linear[] = {
	0, 461.88181, 
	0, 420.68188, 
};


const float anis_highlight_gradient_colors_hsl[] = {
    0. / 360,   0. / 100,  100. / 100,  79. / 100,
    0. / 360,   0. / 100,  100. / 100,  40. / 100,
};

const float anis_highlight_gradient_offsets[] = {
  0., 1.,
};

const float anis_highlight_gradient_linear[] = {
  0, 430.96091, 
  0, 497.62689, 
};


const float anis_shadow_colors_hsl[] = {
    0. / 360,   0. / 100,  100. / 100,  40. / 100,
    0. / 360,   0. / 100,  100. / 100,   0. / 100,
};

const float anis_shadow_offsets[] = {
  0.3212, 0.7697,
};

const float anis_shadow_linear[] = {
  0, 409.78131, 
  0, 591.54462, 
};

const float anis_edge_fill_color[] = {
    1., 1., 1., 1.,
};

const Blend anis_blend = {
    .color = { 0., 0., 0., 0. },
    .equation_rgb = GL_FUNC_ADD,
    .equation_alpha = GL_FUNC_ADD,
    .source_rgb = GL_SRC_ALPHA,
    .source_alpha = GL_ONE_MINUS_SRC_ALPHA,
    .destination_rgb = GL_ONE,
    .destination_alpha = GL_ONE_MINUS_SRC_ALPHA,
};

const Shader shaders[MAX_SHADERS] = {
    {
        .name = "solid_gradient_3", 
        .shader_files = {
            {
                .path = "solid_gradient.vertex",
                .shader_type = GL_VERTEX_SHADER,
            },
            {
                .path = "solid_gradient.fragment",
                .shader_type = GL_FRAGMENT_SHADER,
            }
        },
        .defines = {
            {
                .name = "STOPS",
                .value = "3",
            }
        },
        .attributes = {
            {
                .name = "a_position",
                .size = 2,
                .type = GL_FLOAT,
            }
        },
    },
    {
        .name = "solid_gradient_2", 
        .shader_files = {
            {
                .path = "solid_gradient.vertex",
                .shader_type = GL_VERTEX_SHADER,
            },
            {
                .path = "solid_gradient.fragment",
                .shader_type = GL_FRAGMENT_SHADER,
            }
        },
        .defines = {
            {
                .name = "STOPS",
                .value = "2",
            }
        },
        .attributes = {
            {
                .name = "a_position",
                .size = 2,
                .type = GL_FLOAT,
            }
        },
    },
    {
        .name = "solid",
        .shader_files = {
            {
                .path = "solid.vertex",
                .shader_type = GL_VERTEX_SHADER,
            },
            {
                .path = "solid.fragment",
                .shader_type = GL_FRAGMENT_SHADER,
            },
        },
        .attributes = {
            {
                .name = "a_position",
                .size = 2,
                .type = GL_FLOAT,
            },
        }
    }
};

const float anis_vertex_data[] = {
#include "anis_vertex_data.data"
};

const Array arrays[MAX_ARRAYS] = {
    {
        .name = "anis_drink_background_vertex",
        .type = GL_FLOAT,
        .data = anis_vertex_data,
        .size = sizeof(anis_vertex_data),
        .target = GL_ARRAY_BUFFER,
    },
};

const Drink drinks[] = {
    {
        .name = "anis",
        .drawing = {
            .name = "anis_drawing",
            .dimensions = { 640, 816 },
            .layers = {
                {
                    .shader_name = "solid_gradient_2",
                    .vertex_array_name = "anis_drink_background_vertex",
                    .element_array_name = NULL,
                    .draw_mode = GL_TRIANGLES,
                    .range = {0, 741}, // 741
                    .attributes = {
                        {
                            .name = "a_position",
                            .normalized = GL_FALSE,
                            .stride = 2 * sizeof(float),
                            .offset = 0,
                        }
                    },
                    .uniforms = {
                        {
                            .name = "u_stop_colors",
                            .type = GL_FLOAT_VEC4,
                            .data = anis_shadow_colors_hsl,
                            .data_size = sizeof(anis_shadow_colors_hsl) / sizeof(float),
                        },
                        {
                            .name = "u_stop_offsets",
                            .type = GL_FLOAT,
                            .data = anis_shadow_offsets,
                            .data_size = sizeof(anis_shadow_offsets) / sizeof(float),
                        },
                        {
                            .name = "u_linear_gradient",
                            .type = GL_FLOAT_VEC2,
                            .data = anis_shadow_linear,
                            .data_size = sizeof(anis_shadow_linear) / sizeof(float),
                        },
                    },
                },
                {
                    .shader_name = "solid_gradient_3",
                    .vertex_array_name = "anis_drink_background_vertex",
                    .element_array_name = NULL,
                    .draw_mode = GL_TRIANGLES,
                    .range = { 741, 873 },  // 132
                    .attributes = {
                        {
                            .name = "a_position",
                            .normalized = GL_FALSE,
                            .stride = 2 * sizeof(float),
                            .offset = 0,
                        }
                    },
                    .uniforms = {
                        {
                            .name = "u_stop_colors",
                            .type = GL_FLOAT_VEC4,
                            .data = anis_gradient_colors_hsl,
                            .data_size = sizeof(anis_gradient_colors_hsl) / sizeof(float),
                        },
                        {
                            .name = "u_stop_offsets",
                            .type = GL_FLOAT,
                            .data = anis_gradient_offsets,
                            .data_size = sizeof(anis_gradient_offsets) / sizeof(float),
                        },
                        {
                            .name = "u_linear_gradient",
                            .type = GL_FLOAT_VEC2,
                            .data = anis_gradient_linear,
                            .data_size = sizeof(anis_gradient_linear) / sizeof(float),
                        },
                    },
                },
                {
                    .shader_name = "solid_gradient_2",
                    .vertex_array_name = "anis_drink_background_vertex",
                    .element_array_name = NULL,
                    .draw_mode = GL_TRIANGLES,
                    .range = { 873, 906 }, // 33
                    .attributes = {
                        {
                            .name = "a_position",
                            .normalized = GL_FALSE,
                            .stride = 2 * sizeof(float),
                            .offset = 0,
                        }
                    },
                    .uniforms = {
                        {
                            .name = "u_stop_colors",
                            .type = GL_FLOAT_VEC4,
                            .data = anis_top_gradient_colors_hsl,
                            .data_size = sizeof(anis_top_gradient_colors_hsl) / sizeof(float),
                        },
                        {
                            .name = "u_stop_offsets",
                            .type = GL_FLOAT,
                            .data = anis_top_gradient_offsets,
                            .data_size = sizeof(anis_top_gradient_offsets) / sizeof(float),
                        },
                        {
                            .name = "u_linear_gradient",
                            .type = GL_FLOAT_VEC2,
                            .data = anis_top_gradient_linear,
                            .data_size = sizeof(anis_top_gradient_linear) / sizeof(float),
                        },
                    }
                },
                {
                    .shader_name = "solid_gradient_2",
                    .vertex_array_name = "anis_drink_background_vertex",
                    .element_array_name = NULL,
                    .draw_mode = GL_TRIANGLES,
                    .range = { 906, 1155 }, // 249
                    .attributes = {
                        {
                            .name = "a_position",
                            .normalized = GL_FALSE,
                            .stride = 2 * sizeof(float),
                            .offset = 0,
                        }
                    },
                    .uniforms = {
                        {
                            .name = "u_stop_colors",
                            .type = GL_FLOAT_VEC4,
                            .data = anis_highlight_gradient_colors_hsl,
                            .data_size = sizeof(anis_highlight_gradient_colors_hsl) / sizeof(float),
                        },
                        {
                            .name = "u_stop_offsets",
                            .type = GL_FLOAT,
                            .data = anis_highlight_gradient_offsets,
                            .data_size = sizeof(anis_highlight_gradient_offsets) / sizeof(float),
                        },
                        {
                            .name = "u_linear_gradient",
                            .type = GL_FLOAT_VEC2,
                            .data = anis_highlight_gradient_linear,
                            .data_size = sizeof(anis_highlight_gradient_linear) / sizeof(float),
                        },
                    }
                },
                {
                    .shader_name = "solid",
                    .vertex_array_name = "anis_drink_background_vertex",
                    .element_array_name = NULL,
                    .draw_mode = GL_TRIANGLES,
                    .range = { 2184, 3168 }, // 984
                    .attributes = {
                        {
                            .name = "a_position",
                            .normalized = GL_FALSE,
                            .stride = 2 * sizeof(float),
                            .offset = 0,
                        },
                    },
                    .uniforms = {
                        {
                            .name = "u_fill_color",
                            .type = GL_FLOAT_VEC4,
                            .data = anis_edge_fill_color,
                            .data_size = sizeof(anis_edge_fill_color) / sizeof(float),
                        },
                    },
                },
                {
                    .shader_name = "solid",
                    .vertex_array_name = "anis_drink_background_vertex",
                    .element_array_name = NULL,
                    .draw_mode = GL_TRIANGLES,
                    .range = { 1155, 2184 }, // 629
                    .attributes = {
                        {
                            .name = "a_position",
                            .normalized = GL_FALSE,
                            .stride = 2 * sizeof(float),
                            .offset = 0,
                        }
                    },
                    .uniforms = {
                        {
                            .name = "u_fill_color",
                            .type = GL_FLOAT_VEC4,
                            .data = anis_edge_fill_color,
                            .data_size = sizeof(anis_edge_fill_color) / sizeof(float),
                        }
                    }
                }
            },
            .blend = &anis_blend,
            .clear_color = { 0., 0., 0., 1. },
            .clear_bits = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT,
        },
    }
};


