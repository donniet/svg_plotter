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
  "vertex_count": 76,
  "attributes": {
    "a_position": [ 2, 7, 0 ],
    "a_uv": [ 2, 7, 2 ],
    "a_brush": [ 2, 7, 4 ],
    "a_section": [ 1, 7, 6 ],
  },
  "buffer_data": [
    -36.608, 565.851, 0.5, -0.5, -11.608, 540.851, 0, 
    -36.608, 515.851, -0.5, -0.5, -11.608, 540.851, 0, 
    -11.608, 565.851, 0.5, 0, -11.608, 540.851, 0, 
    -11.608, 515.851, -0.5, 0, -11.608, 540.851, 0, 
    16.9748, 565.851, 0.5, 0.573832, 17.0836, 540.851, 0, 
    17.1924, 515.851, -0.5, 0.573832, 17.0836, 540.851, 0, 
    19.549, 565.787, 0.5, 0.688429, 22.8115, 541.001, 0, 
    26.074, 516.215, -0.5, 0.688429, 22.8115, 541.001, 0, 
    92.5732, 575.988, 0.5, 2.18039, 96.6912, 551.329, 0, 
    100.809, 526.671, -0.5, 2.18039, 96.6912, 551.329, 0, 
    92.5732, 575.988, 0.5, 2.18039, 96.6912, 551.329, 1, 
    100.809, 526.671, -0.5, 2.18039, 96.6912, 551.329, 1, 
    85.6957, 573.541, 0.5, 2.29097, 101.456, 554.135, 1, 
    117.216, 534.728, -0.5, 2.29097, 101.456, 554.135, 1, 
    101.256, 587.104, 0.5, 2.75004, 118.741, 569.236, 1, 
    136.227, 551.369, -0.5, 2.75004, 118.741, 569.236, 1, 
    99.8283, 585.991, 0.5, 2.86336, 121.777, 574.021, 1, 
    143.725, 562.051, -0.5, 2.86336, 121.777, 574.021, 1, 
    108.974, 603.305, 0.5, 3.20766, 129.689, 589.31, 1, 
    150.404, 575.314, -0.5, 3.20766, 129.689, 589.31, 1, 
    120.437, 613.386, 0.5, 3.3215, 134.331, 592.603, 1, 
    150.404, 575.314, -0.5, 3.20766, 129.689, 589.31, 2, 
    120.437, 613.386, 0.5, 3.3215, 134.331, 592.603, 2, 
    148.226, 571.82, -0.5, 3.3215, 134.331, 592.603, 2, 
    143.863, 628.849, 0.5, 3.89533, 158.271, 608.418, 2, 
    172.679, 587.987, -0.5, 3.89533, 158.271, 608.418, 2, 
    143.863, 628.849, 0.5, 3.89533, 158.271, 608.418, 3, 
    172.679, 587.987, -0.5, 3.89533, 158.271, 608.418, 3, 
    142.377, 627.428, 0.5, 4.00769, 162.303, 612.329, 3, 
    182.229, 597.231, -0.5, 4.00769, 162.303, 612.329, 3, 
    149.101, 637.69, 0.5, 4.23722, 168.536, 621.966, 3, 
    187.972, 606.241, -0.5, 4.23722, 168.536, 621.966, 3, 
    158.909, 646.168, 0.5, 4.34985, 172.94, 625.476, 3, 
    187.972, 606.241, -0.5, 4.23722, 168.536, 621.966, 4, 
    158.909, 646.168, 0.5, 4.34985, 172.94, 625.476, 4, 
    186.97, 604.785, -0.5, 4.34985, 172.94, 625.476, 4, 
    168.608, 652.22, 0.5, 4.57938, 182.675, 631.553, 4, 
    196.743, 610.886, -0.5, 4.57938, 182.675, 631.553, 4, 
    168.608, 652.22, 0.5, 4.57938, 182.675, 631.553, 5, 
    196.743, 610.886, -0.5, 4.57938, 182.675, 631.553, 5, 
    166.374, 649.167, 0.5, 4.69078, 187.009, 635.053, 5, 
    207.643, 620.938, -0.5, 4.69078, 187.009, 635.053, 5, 
    166.374, 649.167, 0.5, 4.69078, 187.009, 635.053, 6, 
    207.643, 620.938, -0.5, 4.69078, 187.009, 635.053, 6, 
    164.69, 647.107, 0.5, 4.8058, 188.806, 640.516, 6, 
    212.922, 633.925, -0.5, 4.8058, 188.806, 640.516, 6, 
    164.69, 647.107, 0.5, 4.8058, 188.806, 640.516, 7, 
    212.922, 633.925, -0.5, 4.8058, 188.806, 640.516, 7, 
    165.154, 648.572, 0.5, 4.9205, 190.033, 646.118, 7, 
    214.912, 643.664, -0.5, 4.9205, 190.033, 646.118, 7, 
    165.154, 648.572, 0.5, 4.9205, 190.033, 646.118, 8, 
    214.912, 643.664, -0.5, 4.9205, 190.033, 646.118, 8, 
    167.857, 640.05, 0.5, 5.03431, 189.92, 651.807, 8, 
    211.983, 663.564, -0.5, 5.03431, 189.92, 651.807, 8, 
    167.857, 640.05, 0.5, 5.03431, 189.92, 651.807, 9, 
    211.983, 663.564, -0.5, 5.03431, 189.92, 651.807, 9, 
    178.534, 630.621, 0.5, 5.13927, 185.496, 654.632, 9, 
    192.459, 678.643, -0.5, 5.13927, 185.496, 654.632, 9, 
    178.534, 630.621, 0.5, 5.13927, 185.496, 654.632, 10, 
    192.459, 678.643, -0.5, 5.13927, 185.496, 654.632, 10, 
    179.323, 629.774, 0.5, 5.2553, 179.697, 654.771, 10, 
    180.07, 679.769, -0.5, 5.2553, 179.697, 654.771, 10, 
    -3.98347, 632.463, 0.5, 8.92722, -3.87976, 657.463, 10, 
    -3.77604, 682.463, -0.5, 8.92722, -3.87976, 657.463, 10, 
    -3.98347, 632.463, 0.5, 8.92722, -3.87976, 657.463, 11, 
    -3.77604, 682.463, -0.5, 8.92722, -3.87976, 657.463, 11, 
    17.3103, 653.895, 0.5, 9.01152, -7.63501, 655.549, 11, 
    -32.5803, 657.202, -0.5, 9.01152, -7.63501, 655.549, 11, 
    17.3103, 653.895, 0.5, 9.01152, -7.63501, 655.549, 12, 
    -32.5803, 657.202, -0.5, 9.01152, -7.63501, 655.549, 12, 
    13.377, 539.986, 0.5, 11.3068, -11.608, 540.851, 12, 
    -32.5803, 657.202, -0.5, 9.01152, -7.63501, 655.549, 13, 
    13.377, 539.986, 0.5, 11.3068, -11.608, 540.851, 13, 
    -36.593, 541.716, -0.5, 11.3068, -11.608, 540.851, 13, 
    12.5116, 515.001, 0.5, 11.8068, -11.608, 540.851, 13, 
    -37.4585, 516.731, -0.5, 11.8068, -11.608, 540.851, 13, 
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

    // gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
    gl.depthFunc(gl.ALWAYS);

    // gl.enable(gl.DEPTH_TEST); 
    gl.disable(gl.DEPTH_TEST);
    gl.disable(gl.CULL_FACE);

    gl.enable(gl.BLEND);
    gl.blendFuncSeparate(gl.SRC_ALPHA, gl.ONE, gl.ONE, gl.ONE);
    gl.blendEquationSeparate(gl.MAX, gl.MAX);


    m.draw(view, [0, 3 * time]);

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
