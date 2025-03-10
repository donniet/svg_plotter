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
  "vertex_count": 204,
  "attributes": {
    "a_position": [ 2, 7, 0 ],
    "a_uv": [2, 7, 2],
    "a_arclength": [1, 7, 4],
    "a_brush": [2, 7, 5]
  },
  "buffer_data": [
    -36.608, 515.851, -0.5, -0.5, 0, -11.608, 540.851,
    -36.608, 565.851, -0.5, 0.5, 0, -11.608, 540.851,
    -11.608, 515.851, 0, -0.5, 0, -11.608, 540.851,
    -11.608, 565.851, 0, 0.5, 0, -11.608, 540.851,
    -5.86968, 515.851, 0.114766, -0.5, 5.73832, -5.86968, 540.851,
    -5.86968, 565.851, 0.114766, 0.5, 5.73832, -5.86968, 540.851,
    -0.131357, 515.851, 0.229533, -0.5, 11.4766, -0.131357, 540.851,
    -0.131357, 565.851, 0.229533, 0.5, 11.4766, -0.131357, 540.851,
    5.60696, 515.851, 0.344299, -0.5, 17.215, 5.60696, 540.851,
    5.60696, 565.851, 0.344299, 0.5, 17.215, 5.60696, 540.851,
    11.3453, 515.851, 0.459066, -0.5, 22.9533, 11.3453, 540.851,
    11.3453, 565.851, 0.459066, 0.5, 22.9533, 11.3453, 540.851,
    17.0836, 515.851, 0.573832, -0.5, 28.6916, 17.0836, 540.851,
    17.0836, 565.851, 0.573832, 0.5, 28.6916, 17.0836, 540.851,
    26.2729, 516.242, 0.688429, -0.5, 34.4215, 22.8115, 541.001,
    19.3501, 565.76, 0.688429, 0.5, 34.4215, 22.8115, 541.001,
    31.956, 517.036, 0.803196, -0.5, 40.1598, 28.4946, 541.795,
    25.0332, 566.555, 0.803196, 0.5, 40.1598, 28.4946, 541.795,
    37.639, 517.831, 0.917962, -0.5, 45.8981, 34.1776, 542.59,
    30.7162, 567.349, 0.917962, 0.5, 45.8981, 34.1776, 542.59,
    43.3221, 518.625, 1.03273, -0.5, 51.6364, 39.8607, 543.384,
    36.3993, 568.144, 1.03273, 0.5, 51.6364, 39.8607, 543.384,
    49.0051, 519.42, 1.1475, -0.5, 57.3748, 45.5437, 544.179,
    42.0823, 568.938, 1.1475, 0.5, 57.3748, 45.5437, 544.179,
    54.6882, 520.214, 1.26226, -0.5, 63.1131, 51.2268, 544.973,
    47.7654, 569.733, 1.26226, 0.5, 63.1131, 51.2268, 544.973,
    60.3712, 521.009, 1.37703, -0.5, 68.8514, 56.9098, 545.768,
    53.4484, 570.527, 1.37703, 0.5, 68.8514, 56.9098, 545.768,
    66.0543, 521.803, 1.49179, -0.5, 74.5897, 62.5929, 546.562,
    59.1315, 571.322, 1.49179, 0.5, 74.5897, 62.5929, 546.562,
    71.7373, 522.598, 1.60656, -0.5, 80.328, 68.2759, 547.357,
    64.8145, 572.116, 1.60656, 0.5, 80.328, 68.2759, 547.357,
    77.4204, 523.392, 1.72133, -0.5, 86.0664, 73.959, 548.151,
    70.4976, 572.911, 1.72133, 0.5, 86.0664, 73.959, 548.151,
    83.1034, 524.187, 1.83609, -0.5, 91.8047, 79.642, 548.946,
    76.1806, 573.705, 1.83609, 0.5, 91.8047, 79.642, 548.946,
    88.7865, 524.981, 1.95086, -0.5, 97.543, 85.3251, 549.74,
    81.8637, 574.5, 1.95086, 0.5, 97.543, 85.3251, 549.74,
    94.4696, 525.776, 2.06563, -0.5, 103.281, 91.0081, 550.535,
    87.5467, 575.294, 2.06563, 0.5, 103.281, 91.0081, 550.535,
    100.153, 526.57, 2.18039, -0.5, 109.02, 96.6912, 551.329,
    93.2298, 576.089, 2.18039, 0.5, 109.02, 96.6912, 551.329,
    117.904, 535.308, 2.29097, -0.5, 114.549, 101.456, 554.135,
    85.0074, 572.961, 2.29097, 0.5, 114.549, 101.456, 554.135,
    122.226, 539.083, 2.40574, -0.5, 120.287, 105.777, 557.91,
    89.3288, 576.737, 2.40574, 0.5, 120.287, 105.777, 557.91,
    126.547, 542.859, 2.52051, -0.5, 126.025, 110.099, 561.685,
    93.6502, 580.512, 2.52051, 0.5, 126.025, 110.099, 561.685,
    130.868, 546.634, 2.63527, -0.5, 131.764, 114.42, 565.461,
    97.9716, 584.288, 2.63527, 0.5, 131.764, 114.42, 565.461,
    135.19, 550.409, 2.75004, -0.5, 137.502, 118.741, 569.236,
    102.293, 588.063, 2.75004, 0.5, 137.502, 118.741, 569.236,
    143.979, 562.53, 2.86336, -0.5, 143.168, 121.777, 574.021,
    99.5735, 585.511, 2.86336, 0.5, 143.168, 121.777, 574.021,
    146.617, 567.627, 2.97812, -0.5, 148.906, 124.414, 579.117,
    102.211, 590.607, 2.97812, 0.5, 148.906, 124.414, 579.117,
    149.254, 572.723, 3.09289, -0.5, 154.644, 127.051, 584.213,
    104.848, 595.704, 3.09289, 0.5, 154.644, 127.051, 584.213,
    151.892, 577.819, 3.20766, -0.5, 160.383, 129.689, 589.31,
    107.486, 600.8, 3.20766, 0.5, 160.383, 129.689, 589.31,
    148.111, 571.744, 3.3215, -0.5, 166.075, 134.331, 592.603,
    120.551, 613.463, 3.3215, 0.5, 166.075, 134.331, 592.603,
    152.899, 574.907, 3.43627, -0.5, 171.813, 139.119, 595.766,
    125.339, 616.625, 3.43627, 0.5, 171.813, 139.119, 595.766,
    157.687, 578.07, 3.55103, -0.5, 177.552, 143.907, 598.929,
    130.127, 619.788, 3.55103, 0.5, 177.552, 143.907, 598.929,
    162.475, 581.233, 3.6658, -0.5, 183.29, 148.695, 602.092,
    134.915, 622.951, 3.6658, 0.5, 183.29, 148.695, 602.092,
    167.263, 584.396, 3.78057, -0.5, 189.028, 153.483, 605.255,
    139.703, 626.114, 3.78057, 0.5, 189.028, 153.483, 605.255,
    172.051, 587.559, 3.89533, -0.5, 194.767, 158.271, 608.418,
    144.491, 629.277, 3.89533, 0.5, 194.767, 158.271, 608.418,
    183.294, 598.751, 4.00769, -0.5, 200.384, 162.303, 612.329,
    141.312, 625.907, 4.00769, 0.5, 200.384, 162.303, 612.329,
    186.411, 603.57, 4.12245, -0.5, 206.123, 165.42, 617.148,
    144.428, 630.726, 4.12245, 0.5, 206.123, 165.42, 617.148,
    189.528, 608.388, 4.23722, -0.5, 211.861, 168.536, 621.966,
    147.545, 635.544, 4.23722, 0.5, 211.861, 168.536, 621.966,
    186.177, 604.268, 4.34985, -0.5, 217.492, 172.94, 625.476,
    159.702, 646.684, 4.34985, 0.5, 217.492, 172.94, 625.476,
    191.045, 607.307, 4.46461, -0.5, 223.231, 177.807, 628.515,
    164.57, 649.723, 4.46461, 0.5, 223.231, 177.807, 628.515,
    195.913, 610.345, 4.57938, -0.5, 228.969, 182.675, 631.553,
    169.438, 652.761, 4.57938, 0.5, 228.969, 182.675, 631.553,
    210.413, 626.263, 4.69078, -0.5, 234.539, 187.009, 635.053,
    163.605, 643.842, 4.69078, 0.5, 234.539, 187.009, 635.053,
    212.866, 633.725, 4.8058, -0.5, 240.29, 188.806, 640.516,
    164.746, 647.307, 4.8058, 0.5, 240.29, 188.806, 640.516,
    214.789, 642.632, 4.9205, -0.5, 246.025, 190.033, 646.118,
    165.277, 649.603, 4.9205, 0.5, 246.025, 190.033, 646.118,
    213.895, 658.891, 5.03431, -0.5, 251.715, 189.92, 651.807,
    165.944, 644.723, 5.03431, 0.5, 251.715, 189.92, 651.807,
    186.293, 679.619, 5.13927, -0.5, 256.964, 185.496, 654.632,
    184.699, 629.644, 5.13927, 0.5, 256.964, 185.496, 654.632,
    180.214, 679.766, 5.2553, -0.5, 262.765, 179.697, 654.771,
    179.18, 629.777, 5.2553, 0.5, 262.765, 179.697, 654.771,
    174.385, 679.879, 5.37076, -0.5, 268.538, 173.924, 654.883,
    173.464, 629.888, 5.37076, 0.5, 268.538, 173.924, 654.883,
    168.595, 679.983, 5.48604, -0.5, 274.302, 168.161, 654.986,
    167.728, 629.99, 5.48604, 0.5, 274.302, 168.161, 654.986,
    162.82, 680.081, 5.60121, -0.5, 280.06, 162.404, 655.084,
    161.988, 630.087, 5.60121, 0.5, 280.06, 162.404, 655.084,
    157.054, 680.175, 5.71631, -0.5, 285.815, 156.65, 655.178,
    156.246, 630.182, 5.71631, 0.5, 285.815, 156.65, 655.178,
    151.293, 680.267, 5.83135, -0.5, 291.568, 150.898, 655.27,
    150.503, 630.273, 5.83135, 0.5, 291.568, 150.898, 655.27,
    145.537, 680.357, 5.94635, -0.5, 297.318, 145.149, 655.36,
    144.761, 630.363, 5.94635, 0.5, 297.318, 145.149, 655.36,
    139.783, 680.446, 6.06132, -0.5, 303.066, 139.401, 655.449,
    139.019, 630.452, 6.06132, 0.5, 303.066, 139.401, 655.449,
    134.032, 680.533, 6.17626, -0.5, 308.813, 133.655, 655.536,
    133.278, 630.539, 6.17626, 0.5, 308.813, 133.655, 655.536,
    128.283, 680.619, 6.29117, -0.5, 314.558, 127.91, 655.622,
    127.537, 630.625, 6.29117, 0.5, 314.558, 127.91, 655.622,
    122.536, 680.705, 6.40605, -0.5, 320.303, 122.167, 655.708,
    121.797, 630.71, 6.40605, 0.5, 320.303, 122.167, 655.708,
    116.79, 680.789, 6.52091, -0.5, 326.046, 116.424, 655.792,
    116.058, 630.795, 6.52091, 0.5, 326.046, 116.424, 655.792,
    111.046, 680.873, 6.63576, -0.5, 331.788, 110.682, 655.876,
    110.319, 630.878, 6.63576, 0.5, 331.788, 110.682, 655.876,
    105.303, 680.956, 6.75058, -0.5, 337.529, 104.942, 655.959,
    104.581, 630.962, 6.75058, 0.5, 337.529, 104.942, 655.959,
    99.5608, 681.039, 6.86539, -0.5, 343.269, 99.2022, 656.042,
    98.8435, 631.044, 6.86539, 0.5, 343.269, 99.2022, 656.042,
    93.8199, 681.121, 6.98018, -0.5, 349.009, 93.4634, 656.124,
    93.1068, 631.126, 6.98018, 0.5, 349.009, 93.4634, 656.124,
    88.0801, 681.203, 7.09494, -0.5, 354.747, 87.7255, 656.205,
    87.3709, 631.208, 7.09494, 0.5, 354.747, 87.7255, 656.205,
    82.3412, 681.284, 7.2097, -0.5, 360.485, 81.9884, 656.286,
    81.6357, 631.289, 7.2097, 0.5, 360.485, 81.9884, 656.286,
    76.6034, 681.365, 7.32443, -0.5, 366.222, 76.2523, 656.367,
    75.9012, 631.37, 7.32443, 0.5, 366.222, 76.2523, 656.367,
    70.8666, 681.445, 7.43915, -0.5, 371.957, 70.5171, 656.448,
    70.1675, 631.45, 7.43915, 0.5, 371.957, 70.5171, 656.448,
    65.1308, 681.525, 7.55384, -0.5, 377.692, 64.7828, 656.528,
    64.4348, 631.53, 7.55384, 0.5, 377.692, 64.7828, 656.528,
    59.3961, 681.605, 7.66852, -0.5, 383.426, 59.0495, 656.607,
    58.7029, 631.61, 7.66852, 0.5, 383.426, 59.0495, 656.607,
    53.6625, 681.684, 7.78318, -0.5, 389.159, 53.3172, 656.687,
    52.9719, 631.689, 7.78318, 0.5, 389.159, 53.3172, 656.687,
    47.9302, 681.763, 7.89781, -0.5, 394.89, 47.5862, 656.766,
    47.2421, 631.768, 7.89781, 0.5, 394.89, 47.5862, 656.766,
    42.1991, 681.842, 8.01242, -0.5, 400.621, 41.8563, 656.844,
    41.5135, 631.847, 8.01242, 0.5, 400.621, 41.8563, 656.844,
    36.4695, 681.92, 8.127, -0.5, 406.35, 36.1279, 656.923,
    35.7863, 631.925, 8.127, 0.5, 406.35, 36.1279, 656.923,
    30.7416, 681.999, 8.24154, -0.5, 412.077, 30.4011, 657.001,
    30.0607, 632.003, 8.24154, 0.5, 412.077, 30.4011, 657.001,
    25.0157, 682.076, 8.35605, -0.5, 417.802, 24.6763, 657.079,
    24.337, 632.081, 8.35605, 0.5, 417.802, 24.6763, 657.079,
    19.2921, 682.154, 8.47051, -0.5, 423.525, 18.9539, 657.156,
    18.6158, 632.159, 8.47051, 0.5, 423.525, 18.9539, 657.156,
    13.5718, 682.231, 8.5849, -0.5, 429.245, 13.2347, 657.234,
    12.8976, 632.236, 8.5849, 0.5, 429.245, 13.2347, 657.234,
    7.85579, 682.308, 8.69921, -0.5, 434.96, 7.51992, 657.31,
    7.18405, 632.313, 8.69921, 0.5, 434.96, 7.51992, 657.31,
    2.14689, 682.385, 8.81337, -0.5, 440.669, 1.81231, 657.387,
    1.47774, 632.389, 8.81337, 0.5, 440.669, 1.81231, 657.387,
    -3.54673, 682.461, 8.92722, -0.5, 446.361, -3.87976, 657.463,
    -4.21278, 632.465, 8.92722, 0.5, 446.361, -3.87976, 657.463,
    -32.62, 656.414, 9.01152, -0.5, 450.576, -7.63501, 655.549,
    17.35, 654.683, 9.01152, 0.5, 450.576, -7.63501, 655.549,
    -32.8187, 650.679, 9.12629, -0.5, 456.314, -7.83366, 649.814,
    17.1514, 648.948, 9.12629, 0.5, 456.314, -7.83366, 649.814,
    -33.0173, 644.944, 9.24106, -0.5, 462.053, -8.03231, 644.079,
    16.9527, 643.213, 9.24106, 0.5, 462.053, -8.03231, 644.079,
    -33.216, 639.209, 9.35582, -0.5, 467.791, -8.23096, 638.344,
    16.7541, 637.479, 9.35582, 0.5, 467.791, -8.23096, 638.344,
    -33.4146, 633.475, 9.47059, -0.5, 473.529, -8.42961, 632.609,
    16.5554, 631.744, 9.47059, 0.5, 473.529, -8.42961, 632.609,
    -33.6133, 627.74, 9.58536, -0.5, 479.268, -8.62826, 626.874,
    16.3568, 626.009, 9.58536, 0.5, 479.268, -8.62826, 626.874,
    -33.8119, 622.005, 9.70012, -0.5, 485.006, -8.82691, 621.139,
    16.1581, 620.274, 9.70012, 0.5, 485.006, -8.82691, 621.139,
    -34.0106, 616.27, 9.81489, -0.5, 490.744, -9.02556, 615.404,
    15.9595, 614.539, 9.81489, 0.5, 490.744, -9.02556, 615.404,
    -34.2092, 610.535, 9.92965, -0.5, 496.483, -9.22421, 609.67,
    15.7608, 608.804, 9.92965, 0.5, 496.483, -9.22421, 609.67,
    -34.4079, 604.8, 10.0444, -0.5, 502.221, -9.42285, 603.935,
    15.5622, 603.069, 10.0444, 0.5, 502.221, -9.42285, 603.935,
    -34.6065, 599.065, 10.1592, -0.5, 507.959, -9.6215, 598.2,
    15.3635, 597.334, 10.1592, 0.5, 507.959, -9.6215, 598.2,
    -34.8052, 593.33, 10.274, -0.5, 513.698, -9.82015, 592.465,
    15.1649, 591.599, 10.274, 0.5, 513.698, -9.82015, 592.465,
    -35.0038, 587.596, 10.3887, -0.5, 519.436, -10.0188, 586.73,
    14.9662, 585.865, 10.3887, 0.5, 519.436, -10.0188, 586.73,
    -35.2025, 581.861, 10.5035, -0.5, 525.174, -10.2175, 580.995,
    14.7676, 580.13, 10.5035, 0.5, 525.174, -10.2175, 580.995,
    -35.4011, 576.126, 10.6183, -0.5, 530.913, -10.4161, 575.26,
    14.5689, 574.395, 10.6183, 0.5, 530.913, -10.4161, 575.26,
    -35.5998, 570.391, 10.733, -0.5, 536.651, -10.6148, 569.525,
    14.3703, 568.66, 10.733, 0.5, 536.651, -10.6148, 569.525,
    -35.7984, 564.656, 10.8478, -0.5, 542.389, -10.8134, 563.791,
    14.1716, 562.925, 10.8478, 0.5, 542.389, -10.8134, 563.791,
    -35.9971, 558.921, 10.9626, -0.5, 548.128, -11.0121, 558.056,
    13.973, 557.19, 10.9626, 0.5, 548.128, -11.0121, 558.056,
    -36.1957, 553.186, 11.0773, -0.5, 553.866, -11.2107, 552.321,
    13.7743, 551.455, 11.0773, 0.5, 553.866, -11.2107, 552.321,
    -36.3944, 547.451, 11.1921, -0.5, 559.604, -11.4094, 546.586,
    13.5757, 545.72, 11.1921, 0.5, 559.604, -11.4094, 546.586,
    -36.593, 541.716, 11.3069, -0.5, 565.343, -11.608, 540.851,
    13.377, 539.986, 11.3069, 0.5, 565.343, -11.608, 540.851,
    -37.4585, 516.731, 11.8069, -0.5, 565.343, -11.608, 540.851,
    12.5116, 515.001, 11.8069, 0.5, 565.343, -11.608, 540.851,
  ],
  "stroke_range": [0,1],
  "brush_color": [1,0,0,1],
};

function parse_draw_mode(gl, draw_mode)
{
  return gl[draw_mode.toUpperCase()];
}

function Stroke(gl, mesh_data, mesh_program)
{
  this.gl = gl;
  this.mesh_data = mesh_data;
  this.mesh_program = mesh_program;
  this.float_bytes = 4; // 32 bit floats
  this.uv_range = this.calculate_uv_range(this.mesh_data);

  this.view_location = gl.getUniformLocation(this.mesh_program, "view");
  this.stroke_range_location = gl.getUniformLocation(this.mesh_program, "stroke_range");
  this.brush_color_location = gl.getUniformLocation(this.mesh_program, "brush_color");
  this.uv_range_location = gl.getUniformLocation(this.mesh_program, "uv_range");

  this.draw_mode = parse_draw_mode(this.gl, this.mesh_data.draw_mode);

  this.vao = gl.createVertexArray();
  gl.bindVertexArray(this.vao);

  this.data = this.read_attribute_data();
  this.attr = this.read_attributes();

}
Stroke.prototype.calculate_uv_range = function(mesh_data)
{
  // do we hae an 'a_uv' attribute?
  let a_uv = mesh_data.attributes.a_uv;

  if(typeof a_uv != 'Array')
      return [[-.5,0],[.5,1]];

  let total = mesh_data.buffer_data.length;

  let size, stride, offset;
  [size, stride, offset] = a_uv;

  let min_uv = [0,0];
  let max_uv = [0,0];

  for(let i = offset; i < total; i += stride)
  {
    let u = mesh_data.buffer_data[i];
    let v = mesh_data.buffer_data[i+1];

    if(u < min_uv[0]) min_uv[0] = u;
    if(u > max_uv[0]) max_uv[0] = u;
    if(v < min_uv[1]) min_uv[1] = v;
    if(v > max_uv[1]) max_uv[1] = v;
  }

  return [min_uv, max_uv];
};
Stroke.prototype.read_attribute_data = function()
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
Stroke.prototype.read_attributes = function(no_enable)
{
  let ret = [];
  let attribute_names = Object.getOwnPropertyNames(this.mesh_data.attributes);
  for(let i = 0; i < attribute_names.length; i++)
  {
    let k = attribute_names[i];
    let a = this.mesh_data.attributes[k];

    let loc = this.gl.getAttribLocation(this.mesh_program, k);
    let stride = a[1] * this.float_bytes;
    let offset = a[2] * this.float_bytes;
    
    let enabler = function() 
    {
      this.gl.vertexAttribPointer(loc, a[0], this.gl.FLOAT, false, stride, offset);
      this.gl.enableVertexAttribArray(loc);
    }.bind(this);

    if(!no_enable)
      enabler();

    ret.push({
      name: k,
      size: a.size,
      stride: stride,
      offset: offset,
      location: loc,
      enable: enabler,
    });
  }
  return ret;
};

// TODO: use UV coords and add a length parameter here
Stroke.prototype.draw = function(view, range)
{

  // Tell it to use our program (pair of shaders)
  this.gl.useProgram(this.mesh_program);
  this.gl.uniformMatrix3fv(this.view_location, false, view);
  this.gl.uniform4f(this.brush_color_location, this.mesh_data.brush_color[0], 
                                               this.mesh_data.brush_color[1],
                                               this.mesh_data.brush_color[2],
                                               this.mesh_data.brush_color[3]);
  this.gl.uniform2f(this.stroke_range_location, range[0],
                                                range[1]);
  this.gl.uniform2f(this.resolution_location, this.gl.canvas.width, 
                                              this.gl.canvas.height);
  this.gl.uniform4f(this.uv_range_location, this.uv_range[0][0], 
                                            this.uv_range[0][1], 
                                            this.uv_range[1][0], 
                                            this.uv_range[1][1])

  this.gl.bindVertexArray(this.vao);
  this.gl.drawArrays(
    this.draw_mode,
    0,
    this.mesh_data.vertex_count
  );
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

  gl.depthFunc(gl.ALWAYS);
  

  let vs, fs;

  [vs, fs] = await collect_shaders('mesh-test.vert', 'mesh-test.frag');

  // setup GLSL program
  const program = webglUtils.createProgramFromSources(gl, [vs, fs]);

  // let m = new Stroke(gl, mesh_data, program);
  let m = new Stroke(gl, greenland, program);

  let view = new Float32Array([
    0.005, 0.0, -0.35,
    0.0, 0.005, -2.75,
    0.0, 0.0, 1.0
  ]);

  function render(time) {
    time *= 0.001;  // convert to seconds

    webglUtils.resizeCanvasToDisplaySize(gl.canvas);

    // Tell WebGL how to convert from clip space to pixels
    gl.viewport(0, 0, gl.canvas.width, gl.canvas.height);

    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    gl.enable(gl.DEPTH_TEST); 
    gl.disable(gl.CULL_FACE);


    m.draw(view, [0, Math.min(1., time / 3.)]);

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
