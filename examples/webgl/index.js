"use strict";
/**
 * Credit is given to the following people for their work:
 * https://github.com/gfxfundamentals/webgl2-fundamentals/graphs/contributors
 * This is a modified version of the code from this repository:
 * https://webgl2fundamentals.org/webgl/lessons/webgl-shadertoy.html
 * Many thanks to the authors for their hard work.
 */

const test_stroke = [
  { "x":0, "y":0, "t": 0},
  { "x":1.0101, "y":0, "t": 1},
  { "x":2.0202, "y":0, "t": 1},
  { "x":3.0303, "y":0, "t": 1},
  { "x":4.0404, "y":0, "t": 1},
  { "x":5.05051, "y":0, "t": 1},
  { "x":6.06061, "y":0, "t": 1},
  { "x":7.07071, "y":0, "t": 1},
  { "x":8.08081, "y":0, "t": 1},
  { "x":9.09091, "y":0, "t": 1},
  { "x":0.10101, "y":1, "t": 0},
  { "x":1.11111, "y":1, "t": 1},
  { "x":2.12121, "y":1, "t": 1},
  { "x":3.13131, "y":1, "t": 1},
  { "x":4.14141, "y":1, "t": 1},
  { "x":5.15152, "y":1, "t": 1},
  { "x":6.16162, "y":1, "t": 1},
  { "x":7.17172, "y":1, "t": 1},
  { "x":8.18182, "y":1, "t": 1},
  { "x":9.19192, "y":1, "t": 1},
  { "x":0.20202, "y":2, "t": 0},
  { "x":1.21212, "y":2, "t": 1},
  { "x":2.22222, "y":2, "t": 1},
  { "x":3.23232, "y":2, "t": 1},
  { "x":4.24242, "y":2, "t": 1},
  { "x":5.25253, "y":2, "t": 1},
  { "x":6.26263, "y":2, "t": 1},
  { "x":7.27273, "y":2, "t": 1},
  { "x":8.28283, "y":2, "t": 1},
  { "x":9.29293, "y":2, "t": 1},
  { "x":0.30303, "y":3, "t": 0},
  { "x":1.31313, "y":3, "t": 1},
  { "x":2.32323, "y":3, "t": 1},
  { "x":3.33333, "y":3, "t": 1},
  { "x":4.34343, "y":3, "t": 1},
  { "x":5.35354, "y":3, "t": 1},
  { "x":6.36364, "y":3, "t": 1},
  { "x":7.37374, "y":3, "t": 1},
  { "x":8.38384, "y":3, "t": 1},
  { "x":9.39394, "y":3, "t": 1},
  { "x":0.40404, "y":4, "t": 0},
  { "x":1.41414, "y":4, "t": 1},
  { "x":2.42424, "y":4, "t": 1},
  { "x":3.43434, "y":4, "t": 1},
  { "x":4.44444, "y":4, "t": 1},
  { "x":5.45455, "y":4, "t": 1},
  { "x":6.46465, "y":4, "t": 1},
  { "x":7.47475, "y":4, "t": 1},
  { "x":8.48485, "y":4, "t": 1},
  { "x":9.49495, "y":4, "t": 1},
  { "x":0.505051, "y":5, "t": 0},
  { "x":1.51515, "y":5, "t": 1},
  { "x":2.52525, "y":5, "t": 1},
  { "x":3.53535, "y":5, "t": 1},
  { "x":4.54545, "y":5, "t": 1},
  { "x":5.55556, "y":5, "t": 1},
  { "x":6.56566, "y":5, "t": 1},
  { "x":7.57576, "y":5, "t": 1},
  { "x":8.58586, "y":5, "t": 1},
  { "x":9.59596, "y":5, "t": 1},
  { "x":0.606061, "y":6, "t": 0},
  { "x":1.61616, "y":6, "t": 1},
  { "x":2.62626, "y":6, "t": 1},
  { "x":3.63636, "y":6, "t": 1},
  { "x":4.64646, "y":6, "t": 1},
  { "x":5.65657, "y":6, "t": 1},
  { "x":6.66667, "y":6, "t": 1},
  { "x":7.67677, "y":6, "t": 1},
  { "x":8.68687, "y":6, "t": 1},
  { "x":9.69697, "y":6, "t": 1},
  { "x":0.707071, "y":7, "t": 0},
  { "x":1.71717, "y":7, "t": 1},
  { "x":2.72727, "y":7, "t": 1},
  { "x":3.73737, "y":7, "t": 1},
  { "x":4.74747, "y":7, "t": 1},
  { "x":5.75758, "y":7, "t": 1},
  { "x":6.76768, "y":7, "t": 1},
  { "x":7.77778, "y":7, "t": 1},
  { "x":8.78788, "y":7, "t": 1},
  { "x":9.79798, "y":7, "t": 1},
  { "x":0.808081, "y":8, "t": 0},
  { "x":1.81818, "y":8, "t": 1},
  { "x":2.82828, "y":8, "t": 1},
  { "x":3.83838, "y":8, "t": 1},
  { "x":4.84848, "y":8, "t": 1},
  { "x":5.85859, "y":8, "t": 1},
  { "x":6.86869, "y":8, "t": 1},
  { "x":7.87879, "y":8, "t": 1},
  { "x":8.88889, "y":8, "t": 1},
  { "x":9.89899, "y":8, "t": 1},
  { "x":0.909091, "y":9, "t": 0},
  { "x":1.91919, "y":9, "t": 1},
  { "x":2.92929, "y":9, "t": 1},
  { "x":3.93939, "y":9, "t": 1},
  { "x":4.94949, "y":9, "t": 1},
  { "x":5.9596, "y":9, "t": 1},
  { "x":6.9697, "y":9, "t": 1},
  { "x":7.9798, "y":9, "t": 1},
  { "x":8.9899, "y":9, "t": 1},
  { "x":10, "y":9, "t": 0},
  ];

const mesh_data = {
  "draw_mode": "triangle_strip",
  "vertex_count": 8,
  "attributes": {
    //             [ size, stride, offset ]
    "a_position":  [    2,      7,      0 ],
    "a_uv":        [    2,      7,      2 ],
    "a_arclength": [    1,      7,      4 ],
    "a_brush":     [    2,      7,      5 ]
  },
  "buffer_data": [
//   p.x    p.y   uv.x   uv.y    s    b.x  b.y
    -0.25, -0.25, -0.5,  -0.5,   0.,  0.,  0.,
    -0.25,  0.25, -0.5,   0.5,   0.,  0.,  0.,
      0   , -0.25,  0  , -0.5,   0.,  0.,  0., 
      0   ,  0.25,  0  ,  0.5,   0.,  0.,  0., 
      1   ,  0.  ,  1  , -0.5,   1.,  1.,  0.25,
      1   ,  0.5 ,  1  ,  0.5,   1.,  1.,  0.25,
      1.25,  0.  ,  1.5, -0.5,   1.,  1.,  0.25,
      1.25,  0.5 ,  1.5,  0.5,   1.,  1.,  0.25
  ],
  "stroke_range": [0, 1],
  "brush_color": [1, 0, 0, 1],
};

const greenland = {
  "draw_mode": "triangle_strip",
  "vertex_count": 210,
  "attributes": {
    "a_position": [ 2, 7, 0 ],
    "a_uv": [ 2, 7, 2 ],
    "a_brush": [ 2, 7, 4 ],
    "a_section": [ 1, 7, 6 ],
  },
  "buffer_data": [
    -54.825, 588.642, 0.5, -0.5, -47.7539, 588.642, 0, 
    -47.7539, 581.571, -0.5, -0.5, -47.7539, 588.642, 0, 
    -51.2894, 592.178, 0.5, 0, -47.7539, 588.642, 0, 
    -44.2184, 585.107, -0.5, 0, -47.7539, 588.642, 0, 
    47.9667, 691.434, 0.5, 14.0369, 51.5022, 687.898, 0, 
    55.0377, 684.363, -0.5, 14.0369, 51.5022, 687.898, 0, 
    51.5022, 694.969, 0.5, 14.5369, 51.5022, 687.898, 0, 
    58.5733, 687.898, -0.5, 14.5369, 51.5022, 687.898, 0, 
    -53.8476, 518.909, 0.5, 13.5369, -46.7765, 518.909, 1, 
    -46.7765, 511.838, -0.5, 13.5369, -46.7765, 518.909, 1, 
    -53.8476, 518.909, 0.5, 13.5369, -46.7765, 518.909, 2, 
    -46.7765, 511.838, -0.5, 13.5369, -46.7765, 518.909, 2, 
    -50.3121, 522.444, 0.5, 14.0369, -46.7765, 518.909, 2, 
    -43.241, 515.373, -0.5, 14.0369, -46.7765, 518.909, 2, 
    118.571, 691.328, 0.5, 37.9207, 122.107, 687.792, 2, 
    125.643, 684.257, -0.5, 37.9207, 122.107, 687.792, 2, 
    122.107, 694.863, 0.5, 38.4207, 122.107, 687.792, 2, 
    129.178, 687.792, -0.5, 38.4207, 122.107, 687.792, 2, 
    -15.4362, 486.61, 0.5, 37.4207, -8.36511, 486.61, 3, 
    -8.36511, 479.539, -0.5, 37.4207, -8.36511, 486.61, 3, 
    -15.4362, 486.61, 0.5, 37.4207, -8.36511, 486.61, 4, 
    -8.36511, 479.539, -0.5, 37.4207, -8.36511, 486.61, 4, 
    -11.9006, 490.145, 0.5, 37.9207, -8.36511, 486.61, 4, 
    -4.82957, 483.074, -0.5, 37.9207, -8.36511, 486.61, 4, 
    189.574, 691.62, 0.5, 66.4135, 193.11, 688.085, 4, 
    196.645, 684.549, -0.5, 66.4135, 193.11, 688.085, 4, 
    193.11, 695.156, 0.5, 66.9135, 193.11, 688.085, 4, 
    200.181, 688.085, -0.5, 66.9135, 193.11, 688.085, 4, 
    -32.9211, 398.414, 0.5, 65.9135, -25.85, 398.414, 5, 
    -25.85, 391.343, -0.5, 65.9135, -25.85, 398.414, 5, 
    -32.9211, 398.414, 0.5, 65.9135, -25.85, 398.414, 6, 
    -25.85, 391.343, -0.5, 65.9135, -25.85, 398.414, 6, 
    -29.3856, 401.95, 0.5, 66.4135, -25.85, 398.414, 6, 
    -22.3145, 394.879, -0.5, 66.4135, -25.85, 398.414, 6, 
    259.494, 690.829, 0.5, 107.267, 263.03, 687.294, 6, 
    266.565, 683.758, -0.5, 107.267, 263.03, 687.294, 6, 
    263.03, 694.365, 0.5, 107.767, 263.03, 687.294, 6, 
    270.101, 687.294, -0.5, 107.767, 263.03, 687.294, 6, 
    19.6196, 380.244, 0.5, 106.767, 26.6906, 380.244, 7, 
    26.6906, 373.173, -0.5, 106.767, 26.6906, 380.244, 7, 
    19.6196, 380.244, 0.5, 106.767, 26.6906, 380.244, 8, 
    26.6906, 373.173, -0.5, 106.767, 26.6906, 380.244, 8, 
    23.1551, 383.78, 0.5, 107.267, 26.6906, 380.244, 8, 
    30.2262, 376.708, -0.5, 107.267, 26.6906, 380.244, 8, 
    331.293, 691.918, 0.5, 150.845, 334.829, 688.382, 8, 
    338.364, 684.847, -0.5, 150.845, 334.829, 688.382, 8, 
    334.829, 695.453, 0.5, 151.345, 334.829, 688.382, 8, 
    341.9, 688.382, -0.5, 151.345, 334.829, 688.382, 8, 
    31.0782, 320.992, 0.5, 150.345, 38.1493, 320.992, 9, 
    38.1493, 313.921, -0.5, 150.345, 38.1493, 320.992, 9, 
    31.0782, 320.992, 0.5, 150.345, 38.1493, 320.992, 10, 
    38.1493, 313.921, -0.5, 150.345, 38.1493, 320.992, 10, 
    34.6138, 324.528, 0.5, 150.845, 38.1493, 320.992, 10, 
    41.6848, 317.456, -0.5, 150.845, 38.1493, 320.992, 10, 
    34.6138, 324.528, 0.5, 150.845, 38.1493, 320.992, 11, 
    41.6848, 317.456, -0.5, 150.845, 38.1493, 320.992, 11, 
    40.5395, 330.453, 0.5, 151.683, 44.075, 326.918, 11, 
    47.6106, 323.382, -0.5, 151.683, 44.075, 326.918, 11, 
    40.5395, 330.453, 0.5, 151.683, 44.075, 326.918, 12, 
    47.6106, 323.382, -0.5, 151.683, 44.075, 326.918, 12, 
    44.075, 333.989, 0.5, 152.183, 44.075, 326.918, 12, 
    51.1461, 326.918, -0.5, 152.183, 44.075, 326.918, 12, 
    223.665, 513.578, 0.5, 151.183, 230.736, 513.578, 13, 
    230.736, 506.507, -0.5, 151.183, 230.736, 513.578, 13, 
    227.2, 517.114, 0.5, 151.683, 230.736, 513.578, 13, 
    234.271, 510.043, -0.5, 151.683, 230.736, 513.578, 13, 
    402.009, 691.923, 0.5, 176.404, 405.545, 688.388, 13, 
    409.081, 684.852, -0.5, 176.404, 405.545, 688.388, 13, 
    405.545, 695.459, 0.5, 176.904, 405.545, 688.388, 13, 
    412.616, 688.388, -0.5, 176.904, 405.545, 688.388, 13, 
    299.223, 518.426, 0.5, 175.904, 306.294, 518.426, 14, 
    306.294, 511.355, -0.5, 175.904, 306.294, 518.426, 14, 
    299.223, 518.426, 0.5, 175.904, 306.294, 518.426, 15, 
    306.294, 511.355, -0.5, 175.904, 306.294, 518.426, 15, 
    302.759, 521.962, 0.5, 176.404, 306.294, 518.426, 15, 
    309.83, 514.891, -0.5, 176.404, 306.294, 518.426, 15, 
    471.642, 690.845, 0.5, 200.288, 475.178, 687.31, 15, 
    478.713, 683.774, -0.5, 200.288, 475.178, 687.31, 15, 
    475.178, 694.381, 0.5, 200.788, 475.178, 687.31, 15, 
    482.249, 687.31, -0.5, 200.788, 475.178, 687.31, 15, 
    369.254, 517.747, 0.5, 199.788, 376.325, 517.747, 16, 
    376.325, 510.675, -0.5, 199.788, 376.325, 517.747, 16, 
    369.254, 517.747, 0.5, 199.788, 376.325, 517.747, 17, 
    376.325, 510.675, -0.5, 199.788, 376.325, 517.747, 17, 
    372.79, 521.282, 0.5, 200.288, 376.325, 517.747, 17, 
    379.861, 514.211, -0.5, 200.288, 376.325, 517.747, 17, 
    543.155, 691.647, 0.5, 224.381, 546.69, 688.111, 17, 
    550.226, 684.576, -0.5, 224.381, 546.69, 688.111, 17, 
    546.69, 695.183, 0.5, 224.881, 546.69, 688.111, 17, 
    553.761, 688.111, -0.5, 224.881, 546.69, 688.111, 17, 
    429.313, 507.095, 0.5, 223.881, 436.384, 507.095, 18, 
    436.384, 500.024, -0.5, 223.881, 436.384, 507.095, 18, 
    429.313, 507.095, 0.5, 223.881, 436.384, 507.095, 19, 
    436.384, 500.024, -0.5, 223.881, 436.384, 507.095, 19, 
    432.849, 510.63, 0.5, 224.381, 436.384, 507.095, 19, 
    439.92, 503.559, -0.5, 224.381, 436.384, 507.095, 19, 
    613.584, 691.365, 0.5, 249.941, 617.119, 687.83, 19, 
    620.655, 684.294, -0.5, 249.941, 617.119, 687.83, 19, 
    617.119, 694.901, 0.5, 250.441, 617.119, 687.83, 19, 
    624.19, 687.83, -0.5, 250.441, 617.119, 687.83, 19, 
    435.305, 442.376, 0.5, 249.441, 442.376, 442.376, 20, 
    442.376, 435.305, -0.5, 249.441, 442.376, 442.376, 20, 
    435.305, 442.376, 0.5, 249.441, 442.376, 442.376, 21, 
    442.376, 435.305, -0.5, 249.441, 442.376, 442.376, 21, 
    438.841, 445.912, 0.5, 249.941, 442.376, 442.376, 21, 
    445.912, 438.841, -0.5, 249.941, 442.376, 442.376, 21, 
    480.321, 487.392, 0.5, 255.807, 483.857, 483.857, 21, 
    487.392, 480.321, -0.5, 255.807, 483.857, 483.857, 21, 
    483.857, 490.928, 0.5, 256.307, 483.857, 483.857, 21, 
    490.928, 483.857, -0.5, 256.307, 483.857, 483.857, 21, 
    497.526, 504.597, 0.5, 255.307, 504.597, 504.597, 22, 
    504.597, 497.526, -0.5, 255.307, 504.597, 504.597, 22, 
    501.061, 508.132, 0.5, 255.807, 504.597, 504.597, 22, 
    508.132, 501.061, -0.5, 255.807, 504.597, 504.597, 22, 
    512.913, 519.984, 0.5, 257.483, 516.448, 516.448, 22, 
    519.984, 512.913, -0.5, 257.483, 516.448, 516.448, 22, 
    516.448, 523.519, 0.5, 257.983, 516.448, 516.448, 22, 
    523.519, 516.448, -0.5, 257.983, 516.448, 516.448, 22, 
    512.34, 519.411, 0.5, 256.983, 519.411, 519.411, 23, 
    519.411, 512.34, -0.5, 256.983, 519.411, 519.411, 23, 
    512.34, 519.411, 0.5, 256.983, 519.411, 519.411, 24, 
    519.411, 512.34, -0.5, 256.983, 519.411, 519.411, 24, 
    515.875, 522.947, 0.5, 257.483, 519.411, 519.411, 24, 
    519.411, 512.34, -0.5, 256.983, 519.411, 519.411, 25, 
    515.875, 522.947, 0.5, 257.483, 519.411, 519.411, 25, 
    522.947, 515.875, -0.5, 257.483, 519.411, 519.411, 25, 
    652.167, 659.238, 0.5, 276.758, 655.703, 655.703, 25, 
    659.238, 652.167, -0.5, 276.758, 655.703, 655.703, 25, 
    655.703, 662.774, 0.5, 277.258, 655.703, 655.703, 25, 
    662.774, 655.703, -0.5, 277.258, 655.703, 655.703, 25, 
    462.038, 398.398, 0.5, 276.258, 469.109, 398.398, 26, 
    469.109, 391.327, -0.5, 276.258, 469.109, 398.398, 26, 
    462.038, 398.398, 0.5, 276.258, 469.109, 398.398, 27, 
    469.109, 391.327, -0.5, 276.258, 469.109, 398.398, 27, 
    465.573, 401.934, 0.5, 276.758, 469.109, 398.398, 27, 
    472.644, 394.862, -0.5, 276.758, 469.109, 398.398, 27, 
    465.573, 401.934, 0.5, 276.758, 469.109, 398.398, 28, 
    472.644, 394.862, -0.5, 276.758, 469.109, 398.398, 28, 
    504.09, 440.451, 0.5, 282.205, 507.626, 436.915, 28, 
    511.161, 433.38, -0.5, 282.205, 507.626, 436.915, 28, 
    504.09, 440.451, 0.5, 282.205, 507.626, 436.915, 29, 
    511.161, 433.38, -0.5, 282.205, 507.626, 436.915, 29, 
    507.626, 443.986, 0.5, 282.705, 507.626, 436.915, 29, 
    514.697, 436.915, -0.5, 282.705, 507.626, 436.915, 29, 
    544.998, 481.358, 0.5, 281.705, 552.069, 481.358, 30, 
    552.069, 474.287, -0.5, 281.705, 552.069, 481.358, 30, 
    548.533, 484.894, 0.5, 282.205, 552.069, 481.358, 30, 
    555.605, 477.823, -0.5, 282.205, 552.069, 481.358, 30, 
    557.422, 493.782, 0.5, 283.462, 560.958, 490.247, 30, 
    564.493, 486.711, -0.5, 283.462, 560.958, 490.247, 30, 
    560.958, 497.318, 0.5, 283.962, 560.958, 490.247, 30, 
    568.029, 490.247, -0.5, 283.962, 560.958, 490.247, 30, 
    576.108, 512.468, 0.5, 282.962, 583.179, 512.468, 31, 
    583.179, 505.397, -0.5, 282.962, 583.179, 512.468, 31, 
    579.644, 516.004, 0.5, 283.462, 583.179, 512.468, 31, 
    586.715, 508.933, -0.5, 283.462, 583.179, 512.468, 31, 
    652.234, 588.594, 0.5, 293.728, 655.769, 585.059, 31, 
    659.305, 581.523, -0.5, 293.728, 655.769, 585.059, 31, 
    655.769, 592.13, 0.5, 294.228, 655.769, 585.059, 31, 
    662.84, 585.059, -0.5, 294.228, 655.769, 585.059, 31, 
    504.922, 370.572, 0.5, 293.228, 511.993, 370.572, 32, 
    511.993, 363.501, -0.5, 293.228, 511.993, 370.572, 32, 
    504.922, 370.572, 0.5, 293.228, 511.993, 370.572, 33, 
    511.993, 363.501, -0.5, 293.228, 511.993, 370.572, 33, 
    508.458, 374.107, 0.5, 293.728, 511.993, 370.572, 33, 
    515.529, 367.036, -0.5, 293.728, 511.993, 370.572, 33, 
    545.494, 411.143, 0.5, 298.966, 549.029, 407.608, 33, 
    552.565, 404.072, -0.5, 298.966, 549.029, 407.608, 33, 
    549.029, 414.679, 0.5, 299.466, 549.029, 407.608, 33, 
    556.1, 407.608, -0.5, 299.466, 549.029, 407.608, 33, 
    618.993, 484.642, 0.5, 298.466, 626.064, 484.642, 34, 
    626.064, 477.571, -0.5, 298.466, 626.064, 484.642, 34, 
    622.528, 488.178, 0.5, 298.966, 626.064, 484.642, 34, 
    629.599, 481.107, -0.5, 298.966, 626.064, 484.642, 34, 
    656.601, 522.251, 0.5, 303.784, 660.137, 518.715, 34, 
    663.672, 515.18, -0.5, 303.784, 660.137, 518.715, 34, 
    660.137, 525.786, 0.5, 304.284, 660.137, 518.715, 34, 
    667.208, 518.715, -0.5, 304.284, 660.137, 518.715, 34, 
    559.26, 354.199, 0.5, 303.284, 566.331, 354.199, 35, 
    566.331, 347.128, -0.5, 303.284, 566.331, 354.199, 35, 
    559.26, 354.199, 0.5, 303.284, 566.331, 354.199, 36, 
    566.331, 347.128, -0.5, 303.284, 566.331, 354.199, 36, 
    562.795, 357.735, 0.5, 303.784, 566.331, 354.199, 36, 
    569.867, 350.663, -0.5, 303.784, 566.331, 354.199, 36, 
    706.495, 501.434, 0.5, 324.106, 710.03, 497.898, 36, 
    713.566, 494.363, -0.5, 324.106, 710.03, 497.898, 36, 
    710.03, 504.969, 0.5, 324.606, 710.03, 497.898, 36, 
    717.101, 497.898, -0.5, 324.606, 710.03, 497.898, 36, 
    607.274, 331.502, 0.5, 323.606, 614.345, 331.502, 37, 
    614.345, 324.431, -0.5, 323.606, 614.345, 331.502, 37, 
    607.274, 331.502, 0.5, 323.606, 614.345, 331.502, 38, 
    614.345, 324.431, -0.5, 323.606, 614.345, 331.502, 38, 
    610.809, 335.038, 0.5, 324.106, 614.345, 331.502, 38, 
    617.88, 327.967, -0.5, 324.106, 614.345, 331.502, 38, 
    711.547, 435.775, 0.5, 338.353, 715.082, 432.24, 38, 
    718.618, 428.704, -0.5, 338.353, 715.082, 432.24, 38, 
    715.082, 439.311, 0.5, 338.853, 715.082, 432.24, 38, 
    722.154, 432.24, -0.5, 338.853, 715.082, 432.24, 38, 
    665.26, 318.777, 0.5, 337.853, 672.331, 318.777, 39, 
    672.331, 311.706, -0.5, 337.853, 672.331, 318.777, 39, 
    665.26, 318.777, 0.5, 337.853, 672.331, 318.777, 40, 
    672.331, 311.706, -0.5, 337.853, 672.331, 318.777, 40, 
    668.795, 322.313, 0.5, 338.353, 672.331, 318.777, 40, 
    675.866, 315.242, -0.5, 338.353, 672.331, 318.777, 40, 
    668.795, 322.313, 0.5, 338.353, 672.331, 318.777, 41, 
    675.866, 315.242, -0.5, 338.353, 672.331, 318.777, 41, 
    682.128, 335.646, 0.5, 340.238, 685.664, 332.11, 41, 
    689.199, 328.575, -0.5, 340.238, 685.664, 332.11, 41, 
    685.664, 339.181, 0.5, 340.738, 685.664, 332.11, 41, 
    692.735, 332.11, -0.5, 340.738, 685.664, 332.11, 41, 
  ],
  "stroke_range": [0,1],
  "brush_color": [1,0,0,1],
};

function parse_draw_mode(gl, draw_mode)
{
  return gl[draw_mode.toUpperCase()];
}

function get_attribute(mesh_data, row, attr)
{
  let a = mesh_data[attr];

  return row.slice(a[2], a[2] + a[0]);
}

function Attribute(stroke, obj, no_enable)
{
  this.name = obj.name;
  this.size = obj.size;
  this.stride = obj.stride;
  this.offset = obj.offset;
  this.stroke = stroke;

  this.location = this.stroke.gl.getAttribLocation(this.stroke.mesh_program, this.name);

  if(!no_enable)
    this.enable();
}
Attribute.prototype.vertex_value = function(row) 
{
  return row.slice(this.offset, this.offset + this.size);
};
Attribute.prototype.enable = function()
{
  this.stroke.gl.vertexAttribPointer(this.location,
                                     this.size, 
                                     this.stroke.gl.FLOAT, 
                                     false, 
                                     this.stride * this.stroke.float_bytes, 
                                     this.offset * this.stroke.float_bytes);

  this.stroke.gl.enableVertexAttribArray(this.location);
};

function AttributeQuery(att, query_value)
{
  this.att = att;
  this.query_value = query_value;
}
AttributeQuery.prototype.test = function(row)
{
  let val = this.att.vertex_value(row);

  if(this.query_value.length < val.length)
    return -1;
  if(this.query_value.length > val.length)
    return 1;

  for(let i = 0; i < val.length; i++)
  {
    if(this.query_value[i] < val[i])
      return -1;

    if(this.query_value[i] > val[i])
      return 1;
  }

  return 0;
};
function Section(stroke, section_range)
{
  this._section_range = section_range;
  this._stroke = stroke;
}
Section.prototype.uv_range = function(a_uv)
{
  let min_uv = [0,0], max_uv = [0,0];

  this._stroke.for_each_vertex(this._section_range, (row, i) => {
    let uv = a_uv.vertex_value(row);
    if(uv[0] < min_uv[0])
      min_uv[0] = uv[0];
    if(uv[1] < min_uv[1])
      min_uv[1] = uv[1];
    if(uv[0] > max_uv[0])
      max_uv[0] = uv[0];
    if(uv[1] > max_uv[1])
      max_uv[1] = uv[1];
  });

  return [min_uv, max_uv];
};
Section.prototype.vertex_start = function()
{
  return this._section_range[0];
};
Section.prototype.vertex_count = function()
{
  return this._section_range[1] - this._section_range[0];
};


function Stroke(gl, mesh_data, mesh_program)
{
  this.gl = gl;
  this.mesh_data = mesh_data;
  this.mesh_program = mesh_program;
  this.float_bytes = 4; // 32 bit floats
  this.section_attribute_name = "a_section"; // TODO: allow this to be configurable
  this.uv_attribute_name = "a_uv";

  // TODO: maybe just dump the stride in the JSON
  this.stride = mesh_data.attributes.a_position[1];
  this.rows = ( this.mesh_data.buffer_data.length / this.stride ) >> 0;

  this.draw_mode = parse_draw_mode(this.gl, this.mesh_data.draw_mode);

  this.vao = gl.createVertexArray();
  gl.bindVertexArray(this.vao);
  this.data = this.read_vertex_data();

  this.attributes = this.collect_attributes();
  this.sections = this.collect_sections();

  this.view_location = gl.getUniformLocation(this.mesh_program, "view");
  this.stroke_range_location = gl.getUniformLocation(this.mesh_program, "stroke_range");
  this.brush_color_location = gl.getUniformLocation(this.mesh_program, "brush_color");
  this.uv_range_location = gl.getUniformLocation(this.mesh_program, "uv_range");



};
Stroke.prototype.for_each_vertex = function(range, func)
{
  if(typeof func == 'undefined')
  {
    func = range;
    range = [0, this.rows];
  }

  let stride = this.stride;

  for(let i = range[0] * stride; i < range[1] * stride; i += stride)
      func(this.mesh_data.buffer_data.slice(i, i + stride), i / stride);
}

Stroke.prototype.section_attribute = function() 
{
  return this.attributes[this.section_attribute_name];
};
Stroke.prototype.uv_attribute = function()
{
  return this.attributes[this.uv_attribute_name];
};
Stroke.prototype.collect_attributes = function() 
{
  let a = {};
  for(const [k,v] of Object.entries(this.mesh_data.attributes))
    a[k] = new Attribute(this, {
      name: k,
      size: v[0],
      stride: v[1],
      offset: v[2]
    });

  return a;
};
/* ASSUMES a_section is an increasing sequence of integers beginning at 0 */
Stroke.prototype.collect_sections = function()
{
  let sections = [];

  this.for_each_vertex((row, i) => {
    let sec = this.section_attribute().vertex_value(row)[0] >> 0;

    while(sections.length < sec + 1)
      sections.push([i,i]);

    /* section[sec] == [ index, count ] */
    sections[sec][1] = i + 1;
  });

  let s = [];
  for(let i = 0; i < sections.length; ++i) {
    s[i] = new Section(this, sections[i]);
  }
  return s;
};
Stroke.prototype.parse_query = function(query)
{
  let k = Object.getOwnPropertyNames(query)[0];
  let a = this.attributes[k];

  return new AttributeQuery(a, query[k]);
};
/*
query = { a_section: 0 }
 */
Stroke.prototype.vertex_range = function(query) 
{
  let q = this.parse_query(query);

  let min_index = this.rows, max_index = 0;

  this.for_each_vertex((row, i) => {
    if(q.compare(row) == 0) 
    {
      if(i < min_index)
        min_index = i;
      if(i > max_index)
        max_index = i;
    }
  });

  return [min_index, max_index + 1];
}
Stroke.prototype.read_vertex_data = function()
{
  let buf = this.gl.createBuffer();
  this.gl.bindBuffer(this.gl.ARRAY_BUFFER, buf);
  let buffer_array = new Float32Array(
    this.mesh_data.buffer_data
  );
  this.gl.bufferData(this.gl.ARRAY_BUFFER, buffer_array, this.gl.STATIC_DRAW);

  return {
    buffer: buf,
    buffer_array: buffer_array,
  };
};

// TODO: use UV coords and add a length parameter here
Stroke.prototype.draw = function(view, range)
{
  this.gl.useProgram(this.mesh_program);
  this.gl.uniformMatrix3fv(this.view_location, false, view);
  this.gl.uniform4f(this.brush_color_location, this.mesh_data.brush_color[0], 
                                              this.mesh_data.brush_color[1],
                                              this.mesh_data.brush_color[2],
                                              this.mesh_data.brush_color[3]);
  this.gl.uniform2f(this.resolution_location, this.gl.canvas.width, 
                                              this.gl.canvas.height);
  this.gl.bindVertexArray(this.vao);


  for(let j = 0; j < this.sections.length; ++j)
  {
    let i = this.sections.length - j - 1;
    let section = this.sections[i];

    let uv_range = section.uv_range(this.uv_attribute());

    /* uv.y is the stroke length in brush units */
    if(uv_range[0][1] > range[1] || uv_range[1][1] < range[0])
      continue; // outside the requested range


    // Tell it to use our program (pair of shaders)
    this.gl.uniform2f(this.stroke_range_location, range[0],
                                                  range[1]);
    this.gl.uniform4f(this.uv_range_location, -0.5, 
                                              range[0] -0.5, 
                                              0.5, 
                                              range[1]);

    

    this.gl.drawArrays(
      this.draw_mode,
      section.vertex_start(),              // start vertex
      section.vertex_count()  // count
    );
  }
};


async function fetch_uri(uri)
{
  const req = new Request(uri);
  const res = await fetch(req);

  if(!res.ok)
    throw new Error(`could not fetch ${uri}: [${res.status}] ${res.statusText}`);

  return await res.text();
}

async function collect_shaders(vertex_shader_path, fragment_shader_path)
{
  const vs = await fetch_uri(vertex_shader_path);
  const fs = await fetch_uri(fragment_shader_path);

  return [vs, fs];
}

async function main_mesh()
{
  const canvas = document.querySelector("#canvas");
  const gl = canvas.getContext("webgl2");
  if (!gl) {
    return;
  }

  let mouse = {x: 0, y: 0};
  window.onmousemove = function(e)
  {
    mouse.x = e.clientX;
    mouse.y = e.clientY;
  };

  let vs, fs;

  [vs, fs] = await collect_shaders('mesh-test.vert', 'mesh-test.frag');

  // setup GLSL program
  const program = webglUtils.createProgramFromSources(gl, [vs, fs]);

  // let m = new Stroke(gl, mesh_data, program);
  let m = new Stroke(gl, greenland, program);

  let view = new Float32Array([
    0.002, 0.0, -0.7,
    0.0, -0.002, 0.5,
    0.0, 0.0, 1.0
  ]);

  gl.clearColor(1., 1., 1., 0.);
  gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

  gl.depthFunc(gl.ALWAYS);

  // gl.enable(gl.DEPTH_TEST); 
  gl.disable(gl.DEPTH_TEST);
  gl.disable(gl.CULL_FACE);

  gl.enable(gl.BLEND);
  gl.blendFuncSeparate(gl.DST_ALPHA, gl.SRC_ALPHA, gl.ONE, gl.ONE);
  gl.blendEquationSeparate(gl.MAX, gl.MAX);

  
  function render(time) {
    time *= 0.001;  // convert to seconds

    webglUtils.resizeCanvasToDisplaySize(gl.canvas);
    

    // Tell WebGL how to convert from clip space to pixels
    gl.viewport(0, 0, gl.canvas.width, gl.canvas.height);



    m.draw(view, [0, 100 * (time % 10)]);
    // m.draw(view, [0, (1.0 - mouse.y / gl.canvas.height) * 12 ]);

    requestAnimationFrame(render);
  }
  requestAnimationFrame(render);

}

async function main(vertex_shader_path, fragment_shader_path) {
  // test_rendering();
  // return;

  // Get A WebGL context
  /** @type {HTMLCanvasElement} */
  const canvas = document.querySelector("#canvas");
  const gl = canvas.getContext("webgl2");
  if (!gl) {
    return;
  }

  let vs, fs;

  const sin = Math.sin, cos = Math.cos;

  [vs, fs] = await collect_shaders(vertex_shader_path, fragment_shader_path);

  // setup GLSL program
  const program = webglUtils.createProgramFromSources(gl, [vs, fs]);

  // look up where the vertex data needs to go.
  const positionAttributeLocation = gl.getAttribLocation(program, "a_position");

  // look up uniform locations
  const resolutionLocation = gl.getUniformLocation(program, "iResolution");
  const mouseLocation = gl.getUniformLocation(program, "iMouse");
  const timeLocation = gl.getUniformLocation(program, "iTime");
  const colorLocation = gl.getUniformLocation(program, "stroke_color");

  // Create a vertex array object (attribute state)
  const vao = gl.createVertexArray();

  // and make it the one we're currently working with
  gl.bindVertexArray(vao);

  // Create a buffer to put three 2d clip space points in
  const positionBuffer = gl.createBuffer();

  // Bind it to ARRAY_BUFFER (think of it as ARRAY_BUFFER = positionBuffer)
  gl.bindBuffer(gl.ARRAY_BUFFER, positionBuffer);

  // fill it with a 2 triangles that cover clip space
  gl.bufferData(gl.ARRAY_BUFFER, new Float32Array([
    -1, -1,  // first triangle
    1, -1,
    -1, 1,
    -1, 1,  // second triangle
    1, -1,
    1, 1,
  ]), gl.STATIC_DRAW);

  // Turn on the attribute
  gl.enableVertexAttribArray(positionAttributeLocation);

  // Tell the attribute how to get data out of positionBuffer (ARRAY_BUFFER)
  gl.vertexAttribPointer(
    positionAttributeLocation,
    2,          // 2 components per iteration
    gl.FLOAT,   // the data is 32bit floats
    false,      // don't normalize the data
    0,          // 0 = move forward size * sizeof(type) each iteration to get the next position
    0,          // start at the beginning of the buffer
  );

  let mouseX = 0;
  let mouseY = 0;

  function setMousePosition(e) {
    const rect = canvas.getBoundingClientRect();
    mouseX = e.clientX - rect.left;
    mouseY = rect.height - (e.clientY - rect.top) - 1;  // bottom is 0 in WebGL
  }

  canvas.addEventListener('mousemove', setMousePosition);
  canvas.addEventListener('touchstart', (e) => {
    e.preventDefault();
  }, { passive: false });
  canvas.addEventListener('touchmove', (e) => {
    e.preventDefault();
    setMousePosition(e.touches[0]);
  }, { passive: false });

  function render(time) {
    time *= 0.001;  // convert to seconds

    webglUtils.resizeCanvasToDisplaySize(gl.canvas);

    // Tell WebGL how to convert from clip space to pixels
    gl.viewport(0, 0, gl.canvas.width, gl.canvas.height);

    // Tell it to use our program (pair of shaders)
    gl.useProgram(program);

    // Bind the attribute/buffer set we want.
    gl.bindVertexArray(vao);

    gl.uniform2f(resolutionLocation, gl.canvas.width, gl.canvas.height);
    gl.uniform2f(mouseLocation, mouseX, mouseY);
    gl.uniform1f(timeLocation, time);
    // gl.uniform4f(colorLocation, sin(time), cos(time), -sin(2.3 * time), 1.);
    gl.uniform4f(colorLocation, 0.2, 0.2, 0.2, 1.);

    gl.drawArrays(
      gl.TRIANGLES,
      0,     // offset
      6,     // num vertices to process
    );

    requestAnimationFrame(render);
  }
  requestAnimationFrame(render);
}

// main('brush-strokes.vert', 'brush-strokes.frag');

main_mesh();
