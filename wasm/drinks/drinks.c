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

const Blend anis_blend = {
    .color = { 0., 0., 0., 0. },
    .equation_alpha = GL_FUNC_ADD,
    .equation_rgb = GL_FUNC_ADD,
    .destination_alpha = GL_ONE,
    .destination_rgb = GL_ONE,
    .source_alpha = GL_ONE,
    .source_rgb = GL_ONE,
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
    }
};

const float anis_vertex_data[] = {
    262.416, 476.271,
    262.683, 474.106,
    263.115, 471.963,
    263.115, 471.963,
    263.699, 469.852,
    265.281, 465.755,
    265.281, 465.755,
    267.339, 461.882,
    373.654, 461.882,
    373.654, 461.882,
    375.711, 465.755,
    377.293, 469.852,
    377.293, 469.852,
    377.876, 471.963,
    378.308, 474.106,
    378.308, 474.106,
    378.575, 476.271,
    378.666, 478.45,
    378.666, 478.45,
    378.372, 482.004,
    377.51, 485.455,
    377.51, 485.455,
    376.108, 488.786,
    374.194, 491.98,
    374.194, 491.98,
    371.798, 495.018,
    368.948, 497.884,
    368.948, 497.884,
    365.672, 500.56,
    361.999, 503.028,
    361.999, 503.028,
    357.958, 505.271,
    353.577, 507.272,
    353.577, 507.272,
    348.885, 509.012,
    343.911, 510.476,
    343.911, 510.476,
    338.682, 511.645,
    333.228, 512.501,
    333.228, 512.501,
    327.578, 513.028,
    321.759, 513.207,
    321.759, 513.207,
    319.233, 513.207,
    313.415, 513.028,
    313.415, 513.028,
    307.764, 512.501,
    302.31, 511.645,
    302.31, 511.645,
    297.082, 510.476,
    292.107, 509.012,
    292.107, 509.012,
    287.415, 507.272,
    283.034, 505.271,
    283.034, 505.271,
    278.993, 503.028,
    275.32, 500.56,
    275.32, 500.56,
    272.044, 497.884,
    269.194, 495.018,
    269.194, 495.018,
    266.797, 491.98,
    264.884, 488.786,
    264.884, 488.786,
    263.481, 485.455,
    262.619, 482.004,
    262.416, 476.271,
    263.115, 471.963,
    265.281, 465.755,
    265.281, 465.755,
    373.654, 461.882,
    377.293, 469.852,
    377.293, 469.852,
    378.308, 474.106,
    378.666, 478.45,
    378.666, 478.45,
    377.51, 485.455,
    374.194, 491.98,
    374.194, 491.98,
    368.948, 497.884,
    361.999, 503.028,
    361.999, 503.028,
    353.577, 507.272,
    343.911, 510.476,
    343.911, 510.476,
    333.228, 512.501,
    321.759, 513.207,
    321.759, 513.207,
    313.415, 513.028,
    302.31, 511.645,
    302.31, 511.645,
    292.107, 509.012,
    283.034, 505.271,
    283.034, 505.271,
    275.32, 500.56,
    269.194, 495.018,
    269.194, 495.018,
    264.884, 488.786,
    262.619, 482.004,
    262.416, 476.271,
    265.281, 465.755,
    377.293, 469.852,
    377.293, 469.852,
    378.666, 478.45,
    374.194, 491.98,
    374.194, 491.98,
    361.999, 503.028,
    343.911, 510.476,
    343.911, 510.476,
    321.759, 513.207,
    302.31, 511.645,
    302.31, 511.645,
    283.034, 505.271,
    269.194, 495.018,
    269.194, 495.018,
    262.619, 482.004,
    262.325, 478.45,
    262.416, 476.271,
    377.293, 469.852,
    374.194, 491.98,
    374.194, 491.98,
    343.911, 510.476,
    302.31, 511.645,
    302.31, 511.645,
    269.194, 495.018,
    262.325, 478.45,
    262.416, 476.271,
    374.194, 491.98,
    302.31, 511.645,
    262.416, 476.271,
    302.31, 511.645,
    262.325, 478.45,      
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
                    // glass
                    .shader_name = "solid_gradient_3",
                    .vertex_array_name = "anis_drink_background_vertex",
                    .element_array_name = NULL,
                    .draw_mode = GL_TRIANGLES,
                    .range = { 0, 132 },
                    .attributes = {
                        {
                            .name = "a_position",
                            .normalized = GL_FALSE,
                            .stride = 2 * 4,
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
                }
            },
            .blend = &anis_blend,
            .clear_color = { 0., 0., 0., 1.},
            .clear_bits = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT,
        },
    }
};


