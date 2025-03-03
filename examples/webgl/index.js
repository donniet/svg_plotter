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

function normalize_stroke(stroke, animation_bounds, scale = 1.)
{
  let s = stroke.map((p) => {
    return { 
      x: p.x * scale / animation_bounds.x, 
      y: p.y * scale / animation_bounds.y, 
      t: p.t,
      is_move: p.t == 0
    };
  });

  let ct = 0.;
  for(let i = 0; i < s.length; i++)
  {
    ct += s[i].t;
    s[i].t = ct;
  }
  for(let i = 0; i < s.length; i++)
    s[i].t = s[i].t / ct;
  
  return s;
}

function scale_stroke(stroke, scale)
{
  return stroke.map((p) => {
    return {
      x: p.x * scale,
      y: p.y * scale,
      t: p.t
    };
  });
}

function render_stroke_position(ctx, stroke, brush_diameter, animation_bounds)
{
  let normalized = normalize_stroke(stroke, animation_bounds);

  let pen = {x: 0, y: 0, t: 0};
  let time = 0.;

  ctx.lineWidth = brush_diameter;
  ctx.lineCap = "round";
  ctx.lineJoin = "round";

  ctx.globalCompositionOperation = "destination-out";
  
  for(let i = 1; i < stroke.length; i++)
  {
    let n = normalized[i];
    let s = stroke[i];

    if(n.is_move)
      continue;

    let n0 = normalized[i-1];
    let s0 = stroke[i-1];

    let ds = { x: s.x - s0.x, y: s.y - s0.y };
    let l = Math.sqrt(ds.x * ds.x + ds.y * ds.y);
    ds.x /= l;
    ds.y /= l;
    ds.x *= brush_diameter/2;
    ds.y *= brush_diameter/2;

    let grad = ctx.createLinearGradient(s0.x - ds.x, s0.y - ds.y, s.x + ds.x, s.y + ds.y);
    grad.addColorStop(0, `rgba(${n0.x * 255}, ${n0.y * 255}, 0, ${n0.t})`);
    grad.addColorStop(1, `rgba(${ n.x * 255}, ${ n.y * 255}, 0, ${ n.t})`);
  
    ctx.strokeStyle = grad;
    // ctx.strokeStyle = `rgb(${n.x * 256}, ${n.y * 256}, 0)`;
    ctx.beginPath();
    ctx.moveTo(s0.x, s0.y);
    ctx.lineTo(s.x, s.y);
    ctx.stroke();
  }
}

// returns two sampler objects, stroke_position and stroke_direction
function render_stroke_samplers(stroke, brush_diameter, animation_bounds)
{
  return with_canvas(animation_bounds, 
  (canvas) => {
    const ctx = canvas.getContext("2d");

    render_stroke_position(ctx, stroke, brush_diameter, animation_bounds);
  });
}

function test_rendering()
{
  const canvas = document.createElement("canvas");
  canvas.width = 100;
  canvas.height = 100;

  const ctx = canvas.getContext("2d");

  let s = scale_stroke(test_stroke, 10);

  render_stroke_position(ctx, s, 7, {x: 100, y: 100, t: 1000 /* ms */ });

  canvas.toBlob((blob) => {
    const newImg = document.createElement("img");
    const url = URL.createObjectURL(blob);
  
    newImg.onload = () => {
      // no longer need to read the blob so it's revoked
      URL.revokeObjectURL(url);
    };
  
    newImg.src = url;
    document.body.appendChild(newImg);
  });
}

async function main(vertex_shader_path, fragment_shader_path) {
  test_rendering();
  return;

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
    gl.uniform4f(colorLocation, sin(time), cos(time), -sin(2.3 * time), 1.);

    gl.drawArrays(
      gl.TRIANGLES,
      0,     // offset
      6,     // num vertices to process
    );

    requestAnimationFrame(render);
  }
  requestAnimationFrame(render);
}

main('brush-strokes.vert', 'brush-strokes.frag');
