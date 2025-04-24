

/**
 * returns the lowest index which val could be inserted into the sorted array arr
 * without affecting the sort
 * @param {Array} arr a sorted array
 * @param {*} val the value to find in the sorted array
 * @param {Number} begin the beginning of the range to check
 * @param {Number} end the end of the (exclusive) of the range to check
 */
function lower_bound(arr, val, less = (a,b) => a < b, begin = 0, end = -1) {
    if (end < 0)
        end = arr.length;

    while (begin < end) 
    {
        let mid = Math.floor((begin + end) / 2);

        // check for <=
        if(less(val, arr[mid]) || !less(arr[mid], val))
        {
            end = mid;
            continue;
        }
        // ASSERT(val > arr[mid])
        // if begin + 1 == end then mid will equal begin
        // in this case we return end
        if (begin == mid)
            break;

        // ASSERT(begin != mid)
        begin = mid;
    }

    return end;
}


/**
 * returns the greatest index which val could be inserted into the sorted array arr
 * without affecting the sort
 * @param {Array} arr a sorted array
 * @param {*} val the value to find in the sorted array
 * @param {Number} begin the beginning of the range to check
 * @param {Number} end the end of the (exclusive) of the range to check
 */
function upper_bound(arr, val, less = (a,b) => a < b, begin = 0, end = -1) {
    if (end < 0)
        end = arr.length;

    while (begin < end) 
    {
        let mid = Math.floor((begin + end) / 2);

        if (less(val, arr[mid])) 
        {
            end = mid;
            continue;
        }
        // ASSERT(val > arr[mid])
        // if begin + 1 == end then mid will equal begin
        // in this case we return end
        if (begin == mid)
            break;

        // ASSERT(begin != mid)
        begin = mid;
    }

    return end;
}


function set_uniform_integer(gl, uniform_name, uniform_location, value)
{
    // TODO: handle invalid uniform_locations witha single console log message

    switch(value.length)
    {
    case 1:
        gl.uniform1iv(uniform_location, value); break;
    case 2:
        gl.uniform2iv(uniform_location, value); break;
    case 3: 
        gl.uniform2iv(uniform_location, value); break;
    case 4:
        gl.uniform4iv(uniform_location, value); break;
    default:
        console.log(`could not deduce length of value "${value}"" for uniform "${uniform_name}".`);
    }
}

function create_uniform_integer_setter(gl, program, uniform_name, v)
{
    var location = gl.getUniformLocation(program, uniform_name);

    return function(value) {
        set_uniform_integer(gl, uniform_name, location, value || v);
    };
}

// TODO: add mechanism for integer uniforms
function set_uniform_value(gl, uniform_name, uniform_location, value)
{
    // TODO: handle invalid uniform_locations witha single console log message

    switch(value.length)
    {
    case 1:
        gl.uniform1fv(uniform_location, value); break;
    case 2:
        gl.uniform2fv(uniform_location, value); break;
    case 3: 
        gl.uniform3fv(uniform_location, value); break;
    case 4:
        gl.uniform4fv(uniform_location, value); break;
    default:
        console.log(`could not deduce length of value "${value}"" for uniform "${uniform_name}".`);
    }
}

function create_uniform_value_setter(gl, program, uniform_name)
{
    var location = gl.getUniformLocation(program, uniform_name);

    return function(value) {
        set_uniform_value(gl, uniform_name, location, value);
    };
}

// TODO: add mechanism for integer uniforms
function set_uniform_matrix(gl, uniform_name, uniform_location, value, normalized)
{
    // TODO: handle invalid uniform_locations witha single console log message
    if(!uniform_location && !gl[`__warned_uniform_${uniform_name}`])
    {
        console.log(`uniform ${uniform_name} not found in program`);
        gl[`__warned_uniform_${uniform_name}`] = true;
        return;
    }

    switch(value.length)
    {
    case 1:
        gl.uniformMatrix1fv(uniform_location, normalized, value); break;
    case 4:
        gl.uniformMatrix2fv(uniform_location, normalized, value); break;
    case 9: 
        gl.uniformMatrix3fv(uniform_location, normalized, value); break;
    case 16:
        gl.uniformMatrix4fv(uniform_location, normalized, value); break;
    default:
        console.error(`could not deduce length of value "${value}" for matrix uniform "${uniform_name}"`);
    }
}

function create_uniform_matrix_setter(gl, program, uniform_name)
{
    var location = gl.getUniformLocation(program, uniform_name);

    return function(value, normalized) {
        set_uniform_matrix(gl, uniform_name, location, value, normalized);
    };
}

function compile_shader(gl, shader_type, shader_source)
{
    // create a new webgl shader of shader_type
    var shader = gl.createShader(shader_type);

    // specify the source of the shader
    gl.shaderSource(shader, shader_source);

    // attempt to compile the shader
    gl.compileShader(shader);

    // if we compiled successfully return the shader
    if(gl.getShaderParameter(shader, gl.COMPILE_STATUS))
        return shader;

    // otherwise display the error and clean up
    const last_error = gl.getShaderInfoLog(shader);
    console.error(`error compiling "${shader_type}" shader: ${last_error}`);
    gl.deleteShader(shader);
    return null;
}

function create_program(gl, shaders)
{
    // create a new webgl program
    var program = gl.createProgram();

    // attach our shaders
    for(const shader of shaders)
        gl.attachShader(program, shader);

    // attempt to link the program
    gl.linkProgram(program);

    // if we linked successfully return the program
    if(gl.getProgramParameter(program, gl.LINK_STATUS))
        return program;

    // otherwise show the error in the console and clean up
    const last_error = gl.getProgramInfoLog(program);
    console.error(`error linking program: ${last_error}`);
    gl.deleteProgram(program);
    return null;
}

function compile_script_shader(gl, script_element)
{
    switch(script_element.type)
    {
    case 'x-shader/x-vertex':
        return compile_shader(gl, gl.VERTEX_SHADER, 
            script_element.textContent.trimStart());

    case 'x-shader/x-fragment':
        return compile_shader(gl, gl.FRAGMENT_SHADER, 
            script_element.textContent.trimStart());

    default:
        console.error(`unknown shader type: ${script_element.type}`);
    }

    return null;
}

async function fetch_text(url)
{
    const response = await fetch(url);

    if(!response.ok)
        throw new Error(`error fetching url ${url}: ${response.status}`);

    return await response.text();
}

async function create_program_from_files(gl, vertex_file, fragment_file)
{   
    const vertex_source = await fetch_text(vertex_file);
    const fragment_source = await fetch_text(fragment_file);

    const vertex_shader = compile_shader(gl, gl.VERTEX_SHADER, vertex_source);
    const fragment_shader = compile_shader(gl, gl.FRAGMENT_SHADER, fragment_source);

    return create_program(gl, [vertex_shader, fragment_shader]);
}

function create_program_from_scripts(gl, script_elements)
{
    // compile each shader from the body of the script_elements
    const shaders = [];
    for(const e of script_elements)
    {
        const shader = compile_script_shader(gl, e);

        // if our shader failed to compile then we return null
        if(!shader)
            return null;

        // otherwise add it to the list of shaders to be linked
        shaders.push(shader);
    }

    // create and link the program
    return create_program(gl, shaders);
}

function enable_attribute(location, stride, size, offset, gl)
{
    // setup the vertex attribute pointer
    gl.vertexAttribPointer(location, size, gl.FLOAT, false, stride, offset);

    // enable this attribute's location
    gl.enableVertexAttribArray(location);
};

/**
 * Layer class
 * */
function Layer(gl, program, name, draw_mode, 
               arclength, strokelength, vertex_range, time_range, section_range,
               brush_size, brush_color, 
               sections, uniforms)
{
    this.name = name || DEFAULT_LAYER_NAME;
    this.draw_mode = draw_mode || gl[DEFAULT_DRAW_MODE.toUpperCase()];
    this.arclength = arclength || 1;
    this.vertex_range = vertex_range || [0,0];
    this.time_range = time_range || [0,1];
    this.section_range = section_range || [0,1];
    this.brush_color = brush_color || [0., 0., 0., 1.];
    this.brush_size = brush_size || 10.;
    this.strokelength = strokelength || this.arclength / this.brush_size;
    this.uniforms = uniforms || {};
    this.sections = sections || [];

    this.render_buffer = null;
    this.width = 0;
    this.height = 0;

    // calculate the uv ranges
    this.uv_range_of = this.sections.map(function(s, i, sections) {
        // if this is the first section, our minimum arclength should be zero
        if(i == 0)
            return [ -0.5, 0., 0.5, s.cumulative_arclength ];

        // otherwise subtract 0.5 from the previous arclength as the minimum
        return [ -0.5, sections[i-1].cumulative_arclength, 0.5, s.cumulative_arclength ];
    });

    // create functions to accept values for the uniforms in this layer
    this.uniform_functions = {};
    for(const [key, u] of Object.entries(this.uniforms))
        this.uniform_functions[key] = create_uniform_value_setter(gl, program, u);

    this.set_uv_range = create_uniform_value_setter(gl, program, UV_RANGE_UNIFORM_NAME);
    this.set_stroke_length = create_uniform_value_setter(gl, program, STROKE_LENGTH_UNIFORM_NAME);
    this.set_brush_size = create_uniform_value_setter(gl, program, BRUSH_SIZE_UNIFORM_NAME);
    this.set_brush_color = create_uniform_value_setter(gl, program, BRUSH_COLOR_UNIFORM_NAME);
}

/*
Layer.prototype.attach_render_buffer = function(gl, framebuffer_target)
{
    if(this.render_buffer)
        gl.framebufferRenderbuffer(framebuffer_target, gl.COLOR_ATTACHMENT0, 
            gl.RENDERBUFFER, this.render_buffer);
}

Layer.prototype.delete_render_buffer = function(gl)
{
    if(!this.render_buffer)
        return;

    gl.deleteRenderbuffer(this.render_buffer);
    this.render_buffer = null;
    this.width = 0;
    this.height = 0;
}

Layer.prototype.initialize_render_buffer = function(gl, width, height, format)
{
    if(width != this.width || height != this.height)
        this.delete_render_buffer(gl);

    // if we have a render buffer, then the width and height must be correct, and we can simply return
    if(this.render_buffer)
        return;

    // otherwise we need to create a new one
    this.render_buffer = gl.createRenderbuffer();
    gl.bindRenderbuffer(gl.RENDERBUFFER, this.render_buffer);
    gl.renderbufferStorage(gl.RENDERBUFFER, format || gl.RGBA8, width, height);

    this.width = width;
    this.height = height;

    gl.bindRenderbuffer(gl.RENDERBUFFER, null);
}
    */

Layer.prototype.vertex_range_of_section = function(section)
{
    // if we are the first section, return the overall range up to the first section end
    if(section == 0)
        return [
            this.vertex_range[0],
            this.sections[0].end_vertex
        ];
  
    // otherwise return the end of the last section up to this section end
    return [
        this.sections[section-1].end_vertex,
        this.sections[section].end_vertex
    ];
};


function less_section_length(a, b)
{
    if(typeof a === "number")
        return a < b.cumulative_arclength;

    if(typeof b === "number")
        return a.cumulative_arclength < b;

    return a.cumulative_arclength < b.cumulative_arclength;
}

/**
 * Layer::section_range method calculates the range of sections to draw
 * based on the percentages passed into the range parameter
 * 
 * returns a range [begin, end) of indices into the sections member array
 * */
Layer.prototype.section_range_of = function(range)
{
    // calculate the arclengths that we should draw
    const s0 = this.strokelength * range[0];
    const s1 = this.strokelength * range[1];

    // begin at the lower bound of the lower arclength
    let begin = lower_bound(this.sections, s0, less_section_length); //, ...this.section_range);

    // end at the upper bound of the higher arclength
    let end = upper_bound(this.sections, s1, less_section_length); //, ...this.section_range);

    if(begin >= this.sections.length || begin > end)
        return [0, 0]; // empty range

    return [begin, Math.min(end+1, this.sections.length)];
};

Layer.prototype.draw_section = function(gl, section, range, t)
{
    // find the range of vertices in this section
    const [begin, end] = this.vertex_range_of_section(section);
  
    // lookup the uv range of this section
    const uv_range = this.uv_range_of[section];

    // setup our uv_range uniform
    this.set_uv_range([
        uv_range[0], uv_range[1], 
        uv_range[2], uv_range[1] + t * (uv_range[3] - uv_range[1])
    ]);

    // set our arclength uniform
    this.set_stroke_length([this.sections[section].cumulative_arclength]);

    // draw the section
    gl.drawArrays(this.draw_mode, 
                  begin, 
                  end - begin); 
};

Layer.prototype.draw = function(gl, range)
{
    // calculate the range of sections we need to draw
    let [begin, end] = this.section_range_of(range);

    // nothing to draw!
    if(begin >= end) 
        return;

    // calculate the arclength up until the last section
    let s0 = 0.;
    if(begin - end >= 2)
        s0 = this.sections[end - 2].cumulative_arclength;

    // calculate the maximum length we should be drawing based on this range
    let s1 = this.strokelength * range[1];

    // arclength of the current section only
    let s = this.sections[end-1].cumulative_arclength - s0;

    // calculate the percentage of the final section we should draw
    // based on the range
    let ds = 1.;
    if(s > 0.001)
        ds = (s1 - s0) / s;



    // set our Layer specific uniforms like u_brush_color
    for(const [name, value] of Object.entries(this.uniforms))
        this.uniform_functions[name](value);

    this.set_brush_color(this.brush_color);
    this.set_brush_size([this.brush_size]);

    // we draw our sections in reverse, from the last to the first
    for(let e = end; begin < e; e--)
    {    
        this.draw_section(gl, e - 1, range, ds);

        // the next section will not be the last, so we should draw the whole thing
        ds = 1.;
    }
};

function view_from(drawing_size)
{
    return new Float32Array([
        2./ drawing_size[0],  0., -1.,
        0., -2./ drawing_size[1],  1,
        0.,                   0.,  1.  
    ]);
}

function Blender(gl, blend_program, clear_color, texture_unit, combined_unit)
{
    this.program = blend_program;
    this.texture_unit = texture_unit || 0;
    this.combined_texture_unit = combined_unit || 1;

    this.position_enable = enable_attribute.bind(this, 
        gl.getAttribLocation(this.program, "a_position"), 16, 2, 0);

    this.uv_enable = enable_attribute.bind(this, 
        gl.getAttribLocation(this.program, "a_uv"),       16, 2, 8);

    const uniform_texture_location = gl.getUniformLocation(this.program, "u_texture");
    this.set_texture = function(texture) {
        gl.activeTexture(gl.TEXTURE0 + this.texture_unit);
        gl.bindTexture(gl.TEXTURE_2D, texture);

        gl.uniform1i(uniform_texture_location, this.texture_unit);
    }.bind(this);

    const uniform_combined_location = gl.getUniformLocation(this.program, "u_combined");
    this.set_combined = function(texture) {
        gl.activeTexture(gl.TEXTURE0 + this.combined_texture_unit);
        gl.bindTexture(gl.TEXTURE_2D, texture);

        gl.uniform1i(uniform_combined_location, this.combined_texture_unit);
    }.bind(this);
}

Blender.prototype.draw = function(gl, texture, combined)
{
    this.position_enable(gl);
    this.uv_enable(gl);
    this.set_texture(texture);
    this.set_combined(combined);

    // gl.enable(gl.BLEND);
    // gl.blendFuncSeparate(gl.DST_ALPHA, gl.SRC_ALPHA, gl.ONE, gl.ONE);
    // gl.blendEquationSeparate(gl.MAX, gl.MAX);

    gl.drawArrays(gl.TRIANGLE_STRIP, 0, 4);
}

function Drawing(gl, program, blend_program,
                 vertex_data, stride, 
                 attributes, uniforms, layers, 
                 drawing_size, draw_time, clear_color)
{
    this.program = program;
    this.blend_program = blend_program;
    this.blender = new Blender(gl, blend_program, clear_color, 0);

    // setup our vertex attribute array
    this.vao = gl.createVertexArray();
    gl.bindVertexArray(this.vao);
    this.buffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, this.buffer);
    gl.bufferData(gl.ARRAY_BUFFER, vertex_data, gl.STATIC_DRAW);

    // setup our vao for quads (blending)
    this.quads = gl.createVertexArray();
    gl.bindVertexArray(this.quads);
    const quad_vertices = new Float32Array([
        -1, -1,  0,  0,
         1, -1,  1,  0,
        -1,  1,  0,  1,
         1,  1,  1,  1
    ]);
    this.quad_buffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, this.quad_buffer);
    gl.bufferData(gl.ARRAY_BUFFER, quad_vertices, gl.STATIC_DRAW);

    // create a framebuffer to render to
    this.__framebuffer = gl.createFramebuffer();
    // gl.bindFramebuffer(gl.DRAW_FRAMEBUFFER, this.__framebuffer);

    // 10 FPS for now
    this.frame_period = 1. / 10.;
    
    // calculate the number of vertices/rows
    this.rows = ( vertex_data.length / stride ) >> 0;

    // create functions to enable each of our attributes
    this.attribute_enablers = attributes.map(a =>
        enable_attribute.bind(this, gl.getAttribLocation(program, a.name), 
            stride, a.size, a.offset)
    );

    // create value setters for our uniforms
    this.uniforms = uniforms;
    this.set_uniform = {};
    for(const [key, u] of Object.entries(this.uniforms))
        this.set_uniform[key] = create_uniform_value_setter(gl, program, u);

    // draw state uniform setters
    this.set_uniform_view = create_uniform_matrix_setter(gl, program, 
        VIEW_UNIFORM_NAME);
    this.set_uniform_range = create_uniform_value_setter(gl, program, 
        RANGE_UNIFORM_NAME);
    this.set_uniform_resolution = create_uniform_value_setter(gl, program, 
        RESOLUTION_UNIFORM_NAME);
    this.set_uniform_mouse = create_uniform_value_setter(gl, program, 
        MOUSE_UNIFORM_NAME);
    this.set_uniform_mouse_state = create_uniform_value_setter(gl, program, 
        MOUSE_STATE_UNIFORM_NAME);
    this.set_uniform_time = create_uniform_value_setter(gl, program, 
        TIME_UNIFORM_NAME);
    this.set_uniform_draw_duration = create_uniform_value_setter(gl, program,
        DRAW_DURATION_UNIFORM_NAME);
    this.set_uniform_mixbox_lut = function() {};

    //// optional parameters
    this.drawing_size = drawing_size || [1,1];
    this.aspect_ratio = this.drawing_size[0] / this.drawing_size[1];
    this.view_matrix = view_from(this.drawing_size);
    this.draw_time = draw_time || 1.;
    this.clear_color = clear_color || [0., 0., 0., 0.];

    // extract our layer information
    this.layers = [];

    for(const [name, l] of Object.entries(layers))
    {
        const layer = new Layer(gl, program, l.name, l.draw_mode, 
            l.arclength, l.strokelength, l.vertex_range, l.time_range, l.section_range, 
            l.brush_size, l.brush_color, 
            l.sections, l.uniforms);

        this.layers.push(layer);
    }

    //// internal members
    this.__prev_time = 0;
    this.__mouse = [0., 0., 0.];
    this.__mouse_state = 0;

    //// mouse event handlers
    let mouse_actions = DEFAULT_MOUSE_ACTIONS;

    for(const [id, action] of mouse_actions.entries())
    {
        gl.canvas.addEventListener(`mouse${action}`, 
            Drawing.prototype.mouse_handler.bind(this, id)
        );
    }
}

function isPowerOf2(n)
{
    if(n === 0)
        return true;

    if(n < 0. || n != n >> 0) 
        return false;

    return ((n - 1) & n) === 0;
}

Drawing.prototype.set_mixbox_lut = function(gl, lut)
{
    this.mixbox_lut = lut;

    if(this.mixbox_texture)
        gl.deleteTexture(this.mixbox_texture);

    this.mixbox_texture = gl.createTexture();
    gl.bindTexture(gl.TEXTURE_2D, this.mixbox_texture);
    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, lut);

    if (isPowerOf2(lut.width) && isPowerOf2(lut.height))
    {
      // Yes, it's a power of 2. Generate mips.
      gl.generateMipmap(gl.TEXTURE_2D);
    }
    else 
    {
      // No, it's not a power of 2. Turn off mips and set
      // wrapping to clamp to edge
      gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
      gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
      gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
    }

    const loc = gl.getUniformLocation(this.program, MIXBOX_LUT_UNIFORM_NAME);

    this.set_uniform_mixbox_lut = function() {
        gl.activeTexture(gl.TEXTURE0 + 2);
        gl.bindTexture(gl.TEXTURE_2D, this.mixbox_texture);

        gl.uniform1i(loc, 2);
    };
};

Drawing.prototype.create_render_texture = function(gl, width, height, level = 0)
{
    // create to render to
    const tex = gl.createTexture();
    gl.bindTexture(gl.TEXTURE_2D, tex);
    
    // define size and format of level 0
    const internal_format = gl.RGBA;
    const border = 0;
    const format = gl.RGBA;
    const type = gl.UNSIGNED_BYTE;
    const data = null;

    gl.texImage2D(gl.TEXTURE_2D, level, internal_format,
                  width, height, border,
                  format, type, data);
    
    // set the filtering so we don't need mips
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);

    gl.bindTexture(gl.TEXTURE_2D, null);

    return tex;
}

Drawing.prototype.mouse_handler = function(mouse_action_id, event)
{
    this.__mouse[0]    = event.clientX;
    this.__mouse[1]    = event.clientY;
    this.__mouse[2]   += (typeof event.deltaY === "number" ? event.deltaY : 0);
    // use Gödel numbering to send both the button and the action in one value
    this.__mouse_state = Math.pow(2, event.button) * Math.pow(3, mouse_action_id);
};

Drawing.prototype.read_vertex_data = function(gl, buffer_data, stride)
{
    // create a GL buffer and specify the buffer data
    let b = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, b);
    gl.bufferData(gl.ARRAY_BUFFER, buffer_data, gl.STATIC_DRAW);
};


Drawing.prototype.get_layer_texture = function(gl, layer)
{
    if(this.viewport_width != gl.canvas.width && this.viewport_height != gl.canvas.height)
    {
        if(layer.__texture)
            gl.deleteTexture(layer.__texture);

        layer.__texture = null;
    }

    if(!layer.__texture)
        layer.__texture = this.create_render_texture(gl, this.viewport_width, this.viewport_height);

    return layer.__texture;
}

Drawing.prototype.get_layer_combine_texture = function(gl)
{
    if(this.viewport_width != gl.canvas.width && this.viewport_height != gl.canvas.height)
    {
        if(this.__combine_texture)
            gl.deleteTexture(this.__combine_texture);
        
        this.__combine_texture = null;
    }

    if(!this.__combine_texture)
        this.__combine_texture = this.create_render_texture(gl, this.viewport_width, this.viewport_height);

    return this.__combine_texture;
}

Drawing.prototype.draw_layers = function(gl, upto)
{
    // turn our time into a fraction of the animation
    let u = Math.min(1., upto / this.draw_time);

    // initialize the canvas
    gl.clearColor(...this.clear_color);
    gl.depthFunc(gl.ALWAYS);
    gl.disable(gl.DEPTH_TEST);
    gl.disable(gl.CULL_FACE);
    // gl.enable(gl.BLEND);
    // gl.blendFuncSeparate(gl.DST_ALPHA, gl.SRC_ALPHA, gl.ONE, gl.ONE);
    // gl.blendEquationSeparate(gl.MAX, gl.MAX);

    // ensure our program is being used
    gl.useProgram(this.program);
    gl.bindVertexArray(this.vao);
    gl.bindBuffer(gl.ARRAY_BUFFER, this.buffer);

    // setup our base uniforms
    this.set_uniform_view(this.view_matrix);
    this.set_uniform_range([0, u]);
    this.set_uniform_resolution([this.viewport_dimensions[2], this.viewport_dimensions[3]]);
    this.set_uniform_mouse(this.__mouse);
    this.set_uniform_mouse_state([this.__mouse_state]);
    this.set_uniform_time([upto]);
    this.set_uniform_draw_duration([this.draw_time]);
    this.set_uniform_mixbox_lut();

    // set any additional uniforms from the wasm instance
    for(const [name, value] of Object.entries(this.uniforms))
        this.set_uniform[name](value);

    // enable our vertex attributes
    for(const enable_attribute of this.attribute_enablers)
        enable_attribute(gl);

    if(ENABLE_BLEND_PROGRAM)
        gl.bindFramebuffer(gl.FRAMEBUFFER, this.__framebuffer);

    // draw our layers
    this.layers.forEach(function(layer) 
    { 
        if(ENABLE_BLEND_PROGRAM)
        {
            // layer.attach_render_buffer(gl, gl.DRAW_FRAMEBUFFER);
            let tex = this.get_layer_texture(gl, layer);
            gl.bindTexture(gl.TEXTURE_2D, tex);

            gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.TEXTURE_2D, tex, 0);
            gl.viewport(0, 0, this.viewport_width, this.viewport_height);
            
            gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
        }
        
        // draw this layer to the layers buffer
        layer.draw(gl, [0., u]);
        
    }.bind(this));
};


Drawing.prototype.resize_canvas = function(gl, clientWidth, clientHeight)
{
    if(typeof this.viewport_dimensions === "undefined")
        this.viewport_dimensions = [0, 0, clientWidth, clientHeight];

    if (gl.canvas.width !== clientWidth || gl.canvas.height !== clientHeight) 
    {
        gl.canvas.width  = clientWidth;
        gl.canvas.height = clientHeight;

        let width_by_height = gl.canvas.height * this.aspect_ratio;
        let height_by_width = gl.canvas.width / this.aspect_ratio;

        if(gl.canvas.height > height_by_width)
        {
            this.viewport_dimensions = [0, (gl.canvas.height - height_by_width) / 2., gl.canvas.width, height_by_width];
        }
        else
        {
            this.viewport_dimensions = [(gl.canvas.width - width_by_height) / 2., 0, width_by_height, gl.canvas.height];
        }
        return true;
    }

    return false;
};


Drawing.prototype.draw = function(gl, upto)
{
    // draw each layer to it's associated texture
    // gl.disable(gl.BLEND);

    // TODO: determine proper blending for single layers
    // gl.enable(gl.BLEND);
    // gl.blendFuncSeparate(gl.SRC_ALPHA, gl.DST_ALPHA, gl.ONE, gl.ONE);
    // gl.blendEquationSeparate(gl.MIN, gl.MAX);
    // gl.blend

    // resize the viewport based on the screen size if we aren't using the blend program
    if(!ENABLE_BLEND_PROGRAM)
    {
        gl.viewport(...this.viewport_dimensions);
    }

    // draw our layers
    this.draw_layers(gl, upto);


    if(ENABLE_BLEND_PROGRAM)
    {
        // create a texture to blend with and render to
        let tex = this.get_layer_combine_texture(gl);

        // blend the layers together

        gl.disable(gl.BLEND);
        // gl.blendFuncSeparate(gl.DST_ALPHA, gl.SRC_ALPHA, gl.ONE, gl.ONE);
        // gl.blendEquationSeparate(gl.MAX, gl.MAX);
        this.combine_layers_to_texture(gl, tex);

        // write the output to the canvas
        this.blit_texture_to_canvas(gl, tex);
    }
};


/**
 * combine_layers_to_texture uses the blend program to stack all drawn layers
 */
Drawing.prototype.combine_layers_to_texture = function(gl, combined_texture)
{
    // bind our framebuffer for drawing
    gl.bindFramebuffer(gl.FRAMEBUFFER, this.__framebuffer);
    gl.viewport(0, 0, this.viewport_width, this.viewport_height);  

    // create a buffer texture to avoid feedback loops
    let buffer_fb = gl.createFramebuffer();
    let buffer_texture = this.create_render_texture(gl, this.viewport_width, this.viewport_height);

    // clear our buffer texture
    gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.TEXTURE_2D, buffer_texture, 0);
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);  

    // clear our combined texture
    gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.TEXTURE_2D, combined_texture, 0);
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);  

    // now start using the blend program
    gl.useProgram(this.blend_program);
    gl.bindVertexArray(this.quads);
    gl.bindBuffer(gl.ARRAY_BUFFER, this.quad_buffer);

    this.set_uniform_mixbox_lut();

    for(let i = 0; i < this.layers.length; i++)
    {
        const layer = this.layers[i];

        // extract the layer texture from this layer
        let layer_texture = this.get_layer_texture(gl, layer);

        // bind the combined texture to the draw framebuffer
        gl.bindFramebuffer(gl.FRAMEBUFFER, this.__framebuffer);
        gl.viewport(0, 0, this.viewport_width, this.viewport_height);  
        gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.TEXTURE_2D, combined_texture, 0);

        // blend the layers together using the buffer_texture to avoid feedback loops
        this.blender.draw(gl, layer_texture, buffer_texture);

        // copy our combined texture to our buffer texture
        gl.bindFramebuffer(gl.READ_FRAMEBUFFER, this.__framebuffer);
        gl.framebufferTexture2D(gl.READ_FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.TEXTURE_2D, combined_texture, 0);
        gl.bindFramebuffer(gl.DRAW_FRAMEBUFFER, buffer_fb);
        gl.framebufferTexture2D(gl.DRAW_FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.TEXTURE_2D, buffer_texture, 0);
        gl.viewport(0, 0, this.viewport_width, this.viewport_height);  

        // blit from the combined texture to the buffer texture
        gl.blitFramebuffer(0, 0, this.viewport_width, this.viewport_height,
                           0, 0, this.viewport_width, this.viewport_height,
                           gl.COLOR_BUFFER_BIT, gl.NEAREST);
    }

    // destroy our buffer texture
    // TODO: maybe make this part of the class?
    gl.deleteFramebuffer(buffer_fb);
    gl.deleteTexture(buffer_texture);
};

Drawing.prototype.blit_texture_to_canvas = function(gl)
{
    // bind our framebuffer to the READ attachment in preparation for blit
    gl.bindFramebuffer(gl.READ_FRAMEBUFFER, this.__framebuffer);

    // bind the canvas framebuffer
    gl.bindFramebuffer(gl.DRAW_FRAMEBUFFER, null);
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    let x = 0, y = 0;
    if(gl.canvas.height > this.viewport_height)
        y = (gl.canvas.height - this.viewport_height) >> 1;

    if(gl.canvas.width > this.viewport_width)
        x = (gl.canvas.width - this.viewport_width) >> 1;
    
    gl.blitFramebuffer(0, 0,     this.viewport_width,     this.viewport_height,
                       x, y, x + this.viewport_width, y + this.viewport_height,
                       gl.COLOR_BUFFER_BIT, gl.NEAREST);
};

Drawing.prototype.render = function(gl, time)
{
    let elapsed = time - this.__prev_time;

    // if we are a bit early just wait for the next frame
    if(this.__prev_time == 0 || !THROTTLE_FRAMES || elapsed > this.frame_period)
    {
        const sec = time * 0.001;

        // TODO: redraw on resize
        // if(sec > this.draw_time)
        //     return; // done drawing

        let resized = this.resize_canvas(gl, 
            document.body.clientWidth, 
            document.body.clientHeight);

        // if(resized || this.__prev_time == 0)
        //     this.layers.forEach(layer =>
        //         layer.initialize_render_buffer(gl, document.body.clientWidth, document.body.clientHeight)
        //     );
        
        this.draw(gl, sec);

        this.__prev_time = time;
    }

    // continue the animation
    this.continue_animate(gl);
};

Drawing.prototype.continue_animate = function(gl)
{
    requestAnimationFrame(Drawing.prototype.render.bind(this, gl));
};



/******************************************************************************
 * Mesh class proceses the wasm instance to ready it for rednering 
 * with the Drawing class
 ** */
function Mesh(instance)
{
    this.__instance = instance;
    this.__exports = instance.exports;
    this.__buf = instance.exports.memory.buffer;

    this.__dec = new TextDecoder();
}
Mesh.prototype.string_from_buffer = function(ptr, len)
{
    let dat = new Uint8Array(this.__buf, ptr, len);
    return this.__dec.decode(dat);
}
Mesh.prototype.float32_array_from_buffer = function(ptr, len)
{
    return new Float32Array(this.__buf, ptr, len);
}
Mesh.prototype.vertex_data = function()
{
    return new Float32Array(this.__buf, 
        this.__exports.buffer(), this.__exports.buffer_size())
};
Mesh.prototype.stride = function()
{
    return this.__exports.stride();
};
Mesh.prototype.attributes = function()
{
    let a = [];

    for(let i = 0; i < this.__exports.attribute_count(); i++)
    {
        a.push({
            name:   this.string_from_buffer(
                        this.__exports.attribute_name_ptr(i),
                        this.__exports.attribute_name_length(i)
                    ),
            size:   this.__exports.attribute_size(i),
            offset: this.__exports.attribute_offset(i),
        })
    }

    return a;
};

// TODO: allow for integer uniforms
Mesh.prototype.uniforms = function()
{
    let u = {};

    for(let i = 0; i < this.__exports.uniform_count(); i++)
    {
        const name = this.string_from_buffer(
            this.__exports.uniform_name_ptr(i),
            this.__exports.uniform_name_length(i)
        );
        const value = this.float32_array_from_buffer(
            this.__exports.uniform_data(i),
            this.__exports.uniform_size(i)
        );

        u[name] = value;
    }

    return u;
};

Mesh.prototype.layers = function()
{
    let lays = [];

    for(let i = 0; i < this.__exports.stroke_count(); i++)
    {
        let l = {};
        l.name = this.string_from_buffer(
            this.__exports.stroke_name_ptr(i),
            this.__exports.stroke_name_length(i)
        );
        l.draw_mode = this.__exports.stroke_draw_mode(i);
        l.arclength = this.__exports.stroke_arclength(i);
        l.strokelength = this.__exports.stroke_strokelength(i);
        l.vertex_range = [
            this.__exports.stroke_vertex_start(i),
            this.__exports.stroke_vertex_end(i)
        ];
        l.time_range = [
            this.__exports.stroke_time_start(i), 
            this.__exports.stroke_time_end(i)
        ];
        l.brush_size = this.__exports.stroke_brush_size(i);
        l.brush_color = this.float32_array_from_buffer(
            this.__exports.stroke_brush_color_ptr(i),
            this.__exports.stroke_brush_color_length(i)
        );
        l.sections = [];
        for(let j = 0; j < this.__exports.stroke_section_count(i); j++)
        {
            const s = {};
            s.end_vertex = this.__exports.stroke_section_end_vertex(i, j);
            s.cumulative_arclength = this.__exports.stroke_section_cumulative_arclength(i, j);
            l.sections.push(s);
        }
        l.section_range = [
            this.__exports.stroke_section_start(i),
            this.__exports.stroke_section_end(i)
        ];
        lays.push(l);
    }

    return lays;
}

Mesh.prototype.drawing_size = function()
{
    return new Float32Array([
        this.__exports.drawing_width(),
        this.__exports.drawing_height()
    ]);
};

Mesh.prototype.draw_time = function()
{
    return this.__exports.draw_time();
}

Mesh.prototype.clear_color = function()
{
    return this.float32_array_from_buffer(
        this.__exports.clear_color_ptr(),
        this.__exports.clear_color_length()
    );
}

async function init()
{
    const canvas = document.querySelector("canvas");
    const gl = canvas.getContext(WEBGL_CONTEXT_NAME, {
        antialias: false,
    });
    
    const imports = {
        log: function(msg) { console.log(msg); }
    };

    // TODO: lookup wasm by time
    const { instance } = await WebAssembly.instantiateStreaming(
        fetch(DEFAULT_DRAWING), imports
    );

    const lut = new Image();
    lut.src = MIXING_LOOKUP_TABLE;
    
    await new Promise(function (resolve) {
        lut.onload = resolve;
    });

    // parse the wasm instance into mesh data
    const mesh = new Mesh(instance);

    // get the elements containing our vertex and fragment shaders
    // const vertex_shader = document.getElementById("vertex_shader");
    // const fragment_shader = document.getElementById("fragment_shader");

    // compile and link our shader program
    const program = await create_program_from_files(gl, DRAWING_VERTEX_SHADER, DRAWING_FRAGMENT_SHADER);
    // const program = create_program_from_scripts(gl, [vertex_shader, fragment_shader]);

    
    // get the elements containing our vertex and fragment shaders
    // const blend_vertex_shader = document.getElementById("blend_vertex_shader");
    // const blend_fragment_shader = document.getElementById("blend_fragment_shader");

    // compile and link our shader program
    const blend_program = await create_program_from_files(gl, BLEND_VERTEX_SHADER, BLEND_FRAGMENT_SHADER);
    // const blend_program = create_program_from_scripts(gl, [blend_vertex_shader, blend_fragment_shader]);

    // setup our drawing class
    const drawing = new Drawing(gl, program, blend_program,
        mesh.vertex_data(), mesh.stride(), 
        mesh.attributes(), mesh.uniforms(), mesh.layers(),
        mesh.drawing_size(), mesh.draw_time(), new Float32Array([.5, 1.0, 1.0, 0.])/* mesh.clear_color() */
    );

    // set our color blending lookup table
    drawing.set_mixbox_lut(gl, lut);

    // begin the animation
    drawing.continue_animate(gl);
}   
