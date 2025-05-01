
function SolidRenderer(gl, program, name, draw_mode, vertex_range, time_range, clear_color)
{
    this.name = name;
    this.draw_mode = draw_mode;
    this.vertex_range = vertex_range;
    this.time_range = time_range;
    this.clear_color = clear_color || [0.5, 0.5, 0.5, 0.5];


    this.set_uniform_range = create_uniform_value_setter(gl, program, 
        RANGE_UNIFORM_NAME);

}
SolidRenderer.prototype.draw_arrays_range = function(animation_range)
{
    return this.vertex_range;
};
SolidRenderer.prototype.draw = function(gl, time)
{
    let t = (time - this.time_range[0]) / (this.time_range[1] - this.time_range[0]);

    if(t < 0)
        return;

    if(t > 1) 
        t = 1.;

    if(ENABLE_BLEND_PROGRAM)
        gl.clearColor(...this.clear_color);
        gl.clear(gl.COLOR_BUFFER_BIT);

    const animation_range = [0, t];

    this.set_uniform_range(animation_range);

    const [begin, end] = this.draw_arrays_range(animation_range);

    gl.drawArrays(this.draw_mode, begin, end);
};

function SolidsRenderer(gl, shaders, solids)
{
    this.solids = [];

    let compiled_shaders = [];
    for(let i = 0; i < shaders.length; i++)
        compiled_shaders.push(compile_shader(gl, gl[SHADER_TYPES[i]], shaders[i]));
    
    this.program = create_program(gl, compiled_shaders);

    for(let i = 0; i < solids.solids.length; i++)
        this.solids[i] = new SolidRenderer(gl, this.program,
            solids.solids[i].name, solids.solids[i].draw_mode, 
            solids.solids[i].vertex_range, solids.solids[i].time_range);

    this.vao = gl.createVertexArray();
    gl.bindVertexArray(this.vao);
    this.buffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, this.buffer);
    gl.bufferData(gl.ARRAY_BUFFER, solids.vertex_data(), gl.STATIC_DRAW);

    const stride = solids.stride;


    const attributes = [ { name: POSITION_ATTRIBUTE_NAME, size: 2, offset: 0 } ];
    // create functions to enable each of our attributes
    this.attribute_enablers = attributes.map(a =>
        enable_attribute.bind(this, gl.getAttribLocation(this.program, a.name), 
            stride, a.size, a.offset)
    );

    // draw state uniform setters
    this.set_uniform_view = create_uniform_matrix_setter(gl, this.program, 
        VIEW_UNIFORM_NAME);
    // this.set_uniform_range = create_uniform_value_setter(gl, this.program, 
    //     RANGE_UNIFORM_NAME);
    this.set_uniform_resolution = create_uniform_value_setter(gl, this.program, 
        RESOLUTION_UNIFORM_NAME);
    this.set_uniform_mouse = create_uniform_value_setter(gl, this.program, 
        MOUSE_UNIFORM_NAME);
    this.set_uniform_mouse_state = create_uniform_value_setter(gl, this.program, 
        MOUSE_STATE_UNIFORM_NAME);
    this.set_uniform_time = create_uniform_value_setter(gl, this.program, 
        TIME_UNIFORM_NAME);
    this.set_uniform_draw_duration = create_uniform_value_setter(gl, this.program,
        DRAW_DURATION_UNIFORM_NAME);

    this.drawing_size = solids.drawing_size || DEFAULT_DRAWING_SIZE;
    this.aspect_ratio = this.drawing_size[0] / this.drawing_size[1];
    this.view_matrix = view_from(this.drawing_size);
    this.draw_time = solids.draw_time || 1.;

    this.__prev_time = 0;
    this.__mouse = [0., 0., 0.];
    this.__mouse_state = 0;
}


SolidsRenderer.prototype.set_uniforms = function(gl)
{

}

SolidsRenderer.prototype.enable_attributes = function(gl)
{
    this.attribute_enablers.forEach(enable => enable(gl));
}

SolidsRenderer.prototype.set_uniforms = function(gl, time)
{
    
};

SolidsRenderer.prototype.draw = function(gl, time)
{
    if(!ENABLE_BLEND_PROGRAM)
        gl.viewport(...this.viewport_dimensions);


    gl.useProgram(this.program);
    gl.bindVertexArray(this.vao);
    gl.bindBuffer(gl.ARRAY_BUFFER, this.buffer);


    this.set_uniforms(gl);
    this.enable_attributes(gl);

    // if(ENABLE_BLEND_PROGRAM)
    //     gl.bindFramebuffer(gl.FRAMEBUFFER, this._framebuffer);

    this.set_uniform_view(this.view_matrix);
    this.set_uniform_resolution([this.viewport_dimensions[2], this.viewport_dimensions[3]]);
    this.set_uniform_mouse(this.__mouse);
    this.set_uniform_mouse_state([this.__mouse_state]);
    this.set_uniform_time([time]);

    this.solids.forEach(function(solid) {
        solid.draw(gl, time);

    });
};

SolidsRenderer.prototype.resize_canvas = function(gl, clientWidth, clientHeight)
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

SolidsRenderer.prototype.render = function(gl, time)
{
    this.resize_canvas(gl, gl.canvas.clientWidth, gl.canvas.clientHeight);
    this.draw(gl, time);

    this.continue_animate(gl);
};

SolidsRenderer.prototype.continue_animate = function(gl)
{       
    requestAnimationFrame(SolidsRenderer.prototype.render.bind(this, gl));
};



/**
 * Drawing class
 */
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
