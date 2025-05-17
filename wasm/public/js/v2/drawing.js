

function Layer(app, layer_ptr)
{
    this.app = app;
    const exports = app.exports;
    this.layer_ptr = layer_ptr;

    this.draw_mode = exports.layer_draw_mode(layer_ptr);
    this.shader_name = app.string(exports.layer_shader_name(layer_ptr));

    this.attributes = new Map();
    const attribute_count = exports.layer_attribute_count(layer_ptr);
    for(let i = 0; i < attribute_count; i++)
    {
        const a = exports.layer_attribute(layer_ptr, i);
        const name = app.string(exports.attribute_name(a));
        const normalized = exports.attribute_normalized(a);
        const stride = exports.attribute_stride(a);
        const offset = exports.attribute_offset(a);
        
        this.attributes.set(name, new Attribute(name, offset, stride, normalized));
    }

    this.uniforms = new Map();
    const uniform_count = exports.layer_uniform_count(layer_ptr);
    for(let i = 0; i < uniform_count; i++)
    {
        const u = exports.layer_uniform(layer_ptr, i);
        const uni = this.app.uniform_at(u);

        this.uniforms.set(u.name, u);
    }

    this.draw_offset = exports.layer_draw_offset(layer_ptr);
    this.draw_count = exports.layer_draw_count(layer_ptr);

    this.vertex_array_name = app.string(exports.layer_vertex_array_name(layer_ptr));
    this.element_array_name = app.string(exports.layer_element_array_name(layer_ptr));

    this.shader = null;
    this.uniform_locations = new Map();
    this.vertex_array = null;
    this.element_array = null;
    this.vao = null;
}

Layer.prototype.uniform = function(name, type, value)
{
    if(!this.uniforms.has(name)) 
    {
        const u = new Uniform(name, type, value);
        if(this.shader != null)
            u.load_from(this.shader);
        this.uniforms.set(name, u);
        return u;
    }

    const u = this.uniforms.get(name);
    u.data = value;
    return u;
}

Layer.prototype.vertex_array_object = function()
{
    return this.vao;
};

Layer.prototype.load = function(gl)
{
    // collect our shader and arrays
    this.shader = this.app.shader(this.shader_name);
    this.vertex_array = this.app.data_array(this.vertex_array_name);
    if(this.element_array_name !== null)
        this.element_array = this.app.data_array(this.element_array_name);

    // lookup our attributes
    this.attributes.forEach(att => att.load_from(this.shader));
    this.uniforms.forEach(uni => uni.load_from(this.shader));

    // create vertex_array_object
    this.vao = gl.createVertexArray();
    gl.bindVertexArray(this.vao);

    this.vertex_array.bind(gl);
    if(this.element_array !== null)
        this.element_array.bind(gl);

    this.attributes.forEach(att => att.enable(gl))
    gl.bindVertexArray(null);
};

Layer.prototype.draw = function(gl, range)
{
    // set our app uniforms
    this.uniforms.forEach(uniform => uniform.set(gl));

    // decide whether we draw arrays or elements
    gl.drawArrays(this.draw_mode, 
        this.draw_offset, this.draw_count);
};

function Drawing(app, drawing_ptr)
{
    this.app = app;
    const exports = app.exports;
    this.drawing_ptr = drawing_ptr;
    this.exports = exports;

    this.name = app.string(exports.drawing_name(drawing_ptr));

    this.dimensions = [
        exports.drawing_dimension_width(drawing_ptr),
        exports.drawing_dimension_height(drawing_ptr)
    ];

    this.layers = []
    const layer_count = exports.drawing_layers_size(drawing_ptr);
    for(let i = 0; i < layer_count; i++)
    {
        const layer_ptr = exports.drawing_layer(drawing_ptr, i);
        const layer = new Layer(app, layer_ptr);

        this.layers.push(layer);
    }

    this.uniforms = new Map();

    const blend_ptr = exports.drawing_blend(drawing_ptr);
    this.blend = new Blend(app, blend_ptr);

    const cc = exports.drawing_clear_color(drawing_ptr);
    this.clear_color = float32array_from_wasm(exports, cc, 4);
    this.clear_bits = exports.drawing_clear_bits(drawing_ptr);
}

Drawing.prototype.uniform = function(name, type, value)
{
    if(!this.uniforms.has(name))
    {
        const u = new Uniform(name, type, value);
        this.uniforms.set(name, u);
        return u;
    }

    const u = this.uniforms.get(name);
    u.data = value;
    return u;
}

Drawing.prototype.clear = function(gl)
{
    if(!this.clear_bits)
        return;

    gl.clearColor(...this.clear_color);
    gl.clear(this.clear_bits);
};

Drawing.prototype.load = function(gl)
{
    this.layers.forEach(layer => layer.load(gl));
};

Drawing.prototype.draw = function(gl, range)
{
    // perform a clear on our canvas
    this.clear(gl);
    this.blend.apply(gl);

    this.layers.forEach((layer, index) => {
        // the drawing controls the use of programs to allow for optimization later
        const vao = layer.vertex_array_object(gl);

        this.uniforms.forEach(u => 
            layer.uniform(u.name, u.type, u.data));

        layer.shader.use(gl);

        gl.bindVertexArray(vao);
    
        layer.draw(gl, range);
    });
};

function Drink(app, drink_ptr)
{
    this.app = app;
    const exports = app.exports;
    this.drink_ptr = drink_ptr;
    this.exports = exports;

    this.name = app.string(exports.drink_name(drink_ptr));
    this.drawing = new Drawing(app, exports.drink_drawing(drink_ptr));
    this.position = [0,0];
}
Drink.prototype.load = function(gl)
{
    this.drawing.load(gl);
};
Drink.prototype.viewport = function(x0, y0, x1, y1)
{
    console.log(`drink.viewport(${x0},${y0},${x1},${y1})`);
};
Drink.prototype.draw = function(gl, range)
{
    this.drawing.uniform(UNIFORM_DRINK_POSITION, "vec2", this.position);

    this.drawing.draw(gl, range);
};
Drink.prototype.set_position = function(pos)
{
    this.position = pos;
};

function App()
{
    this.drinks = [];
    this.locations = [];

    // setup our canvas object
    this.canvas = document.getElementsByTagName('canvas')[0];
    this.gl = this.canvas.getContext(CONTEXT_NAME, {
        antialias: false,
    });

    this.module_imports = {
        log: function(msg) { console.log(msg); }
    };
}


App.prototype.draw = function(range)
{
    let resized = this.resize_canvas(this.gl, 
        window.innerWidth,
        window.innerHeight);

    this.drinks.forEach(drink => {
        if(resized)
            drink.viewport(...this.viewport_dimensions);

        drink.draw(this.gl, range);
    });
}

App.prototype.continue_animate = function(time)
{
    if(typeof time != "undefined")
        this.draw([0, time]);

    requestAnimationFrame(time => this.continue_animate(time));
}

App.prototype.init = async function(mod) 
{
    // load our wasm module
    console.log(`loading module ${mod}...`);
    this.wasm = await WebAssembly.instantiateStreaming(
        fetch(mod), this.module_imports
    );
    const exports = this.wasm.instance.exports;
    this.exports = exports;

    // helper function for extracting strings
    this.string = s => 
        string_from_wasm(exports, s, exports.string_length(s));

    

    this.shaders = new Map();
    const shader_count = exports.shader_count();

    for(let i = 0; i < shader_count; i++)
    {
        const s = exports.shader_by_index(i);
        this.shaders.set(this.string(exports.shader_name(s)), 
            new Shader(this, s));
    }

    console.log(`loading drinks...`);
    const drink_count = exports.drink_count();
    for(let i = 0; i < drink_count; i++)
    {
        const d = exports.drink_by_index(i);
        this.drinks.push(new Drink(this, d));
    }

    console.log(`loading arrays...`);
    this.arrays = new Map();
    const array_count = exports.array_count();
    for(let i = 0; i < array_count; i++)
    {
        const a = exports.array_by_index(i);
        const name = this.string(exports.array_name(a));
        const t = exports.array_type(a);
        const array_type = this.string(exports.array_type_name(t));
        const size = exports.array_type(a);
        const ptr = exports.array_data(a);
        const target = exports.array_target(a);

        // find the constructor for this type of data array
        const con = window[array_type];

        const arr = new DataArray(
            name, new con(exports.memory.buffer, ptr, size), t, target);

        this.arrays.set(name, arr);
    }

    console.log(`setting up canvas...`);
    this.canvas_size = [
        exports.canvas_width(),
        exports.canvas_height(),
    ];
    this.aspect_ratio = this.canvas_size[0] / this.canvas_size[1];

    console.log('fetching shaders...');
    let promises = [];
    this.shaders.forEach(shader => promises.push(shader.load()));

    await Promise.all(promises);

    console.log(`compiling shaders...`);
    this.shaders.forEach(shader => shader.compile(this.gl));

    console.log(`initializing buffers...`);
    this.arrays.forEach(arr => arr.load(this.gl));

    console.log(`loading drawings...`);
    this.drinks.forEach(drink => {
        drink.drawing.uniform(UNIFORM_VIEW, "mat3", view_from(this.canvas_size));
        drink.load(this.gl);
    });
    this.locations.forEach(location => {
        location.drawing.uniform(UNIFORM_VIEW, "mat3", view_from(this.canvas_size));
        location.load(this.gl);
    });
};

App.prototype.uniform_at = function(u)
{
    const exports = this.exports;
    const name = this.string(exports.uniform_name(u));
    const type = this.string(exports.uniform_type(u));
    const data = exports.uniform_data(u);
    const data_size = exports.uniform_data_size(u);

    const array_type = this.string(exports.array_type_name(exports.uniform_gl_type(u)));

    const v = new window[array_type](exports.memory.buffer,
        data, data_size);

    return new Uniform(name, type, v);
}

App.prototype.shader = function(name)
{
    return this.shaders.get(name);
};

App.prototype.data_array = function(name)
{
    return this.arrays.get(name);
};

App.prototype.resize_canvas = function(gl, clientWidth, clientHeight)
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
            this.viewport_dimensions = [
                0, (gl.canvas.height - height_by_width) / 2., gl.canvas.width, height_by_width 
            ];
        else
            this.viewport_dimensions = [
                (gl.canvas.width - width_by_height) / 2., 0, width_by_height, gl.canvas.height
            ];
        
        return true;
    }

    return false;
};

function Blend(app, blend_ptr)
{
    this.app = app;
    this.blend_ptr = blend_ptr;
    const exports = app.exports;

    if(!this.blend_ptr)
        return;

    const rgba = exports.blend_color(blend_ptr);

    this.color = float32array_from_wasm(exports, rgba, 4);

    this.equations = [
        exports.blend_equation_rgb(blend_ptr),
        exports.blend_equation_alpha(blend_ptr),
    ];

    this.funcs = [
        exports.blend_source_rgb(blend_ptr),
        exports.blend_destination_rgb(blend_ptr),
        exports.blend_source_alpha(blend_ptr),
        exports.blend_destination_alpha(blend_ptr),
    ];
}

Blend.prototype.apply = function(gl)
{
    if(!this.blend_ptr) 
    {
        gl.disable(gl.BLEND);
        return;
    }

    gl.enable(gl.BLEND);
    gl.blendColor(...this.color);
    gl.blendEquationSeparate(...this.equations);
    gl.blendFuncSeparate(...this.funcs);
}

function DataArray(name, data, type, target)
{
    this.name = name;
    this.data = data;
    this.type = type;
    this.target = target;
    this.buffer = null;
}
DataArray.prototype.load = function(gl, usage)
{
    if(typeof usage === "undefined")
        usage = gl.STATIC_DRAW;

    this.buffer = gl.createBuffer();
    gl.bindBuffer(this.target, this.buffer);
    gl.bufferData(this.target, this.data, usage);
}
DataArray.prototype.bind = function(gl)
{
    gl.bindBuffer(this.target, this.buffer);
}


function Uniform(name, type, data = null, location = null)
{
    this.name = name;
    this.type = type;
    this.gl_setter = gl_uniform_method_for(type);
    this.data = data;
    this.location = location;
    this.is_matrix = gl_uniform_is_matrix(type);
}

Uniform.prototype.load_from = function(shader)
{
    this.location = shader.uniform_location(this.name);
}

Uniform.prototype.set = function(gl, value, transpose) 
{
    if(this.location === null)
    {
        if(!this.__warned__)
        {
            // this warning keeps printing because this object goes out of scope
            // but I'd like to keep the warning in
            console.log(`uniform ${this.name} not found in program`);
            this.__warned__ = true;
        }
        return;
    }

    if(typeof value === "undefined" && this.data == null)
    {
        console.log(`no value set for uniform "${this.name}"`);
        return;
    }

    if(typeof value === "undefined")
        if(this.is_matrix)
            gl[this.gl_setter](this.location, transpose || false, this.data);
        else
            gl[this.gl_setter](this.location, this.data);
    else if(typeof transpose === "undefined")
        if(this.is_matrix)
            gl[this.gl_setter](this.location, transpose || false, value);
        else
            gl[this.gl_setter](this.location, value);
    else
        gl[this.gl_setter](this.location, transpose, value);
}

function Attribute(name, offset, stride, normalized)
{
    this.name = name;
    this.offset = offset;
    this.stride = stride;
    this.normalized = normalized;
    this.shader = null;
    this.definition = null;
}
Attribute.prototype.load_from = function(shader)
{
    this.shader = shader;
    this.definition = shader.attribute_definition(this.name);
};
Attribute.prototype.enable = function(gl)
{
    if(this.definition === null)
        throw new Error(`attempt to enable attribute ${this.name} without definition`);

    this.definition.enable(gl, this.offset, this.stride, this.normalized);
};

function AttributeDef(name, size, type)
{
    this.name = name;
    this.size = size;
    this.type = type;
    this.location = null;
}
AttributeDef.prototype.enable = function(gl, offset = 0, stride = 0, normalized = false)
{
    if(this.location == null)
    {
        if(!this.__warned__)
        {
            console.log(`attribute "${this.name}" not found in program.`);
            this.__warned__ = true;
        }
        return;
    }

    // assume this is the only attribute if stride is not set
    if(stride === 0)
        stride = this.size * bytes_from_element_type(gl, this.type);

    gl.enableVertexAttribArray(this.location);
    gl.vertexAttribPointer(this.location, this.size, this.type, 
        normalized, stride, offset);
};


function Shader(app, shader_ptr)
{
    this.app = app;
    const exports = app.exports;

    this.shader_ptr = shader_ptr;
    this.name = app.string(exports.shader_name(shader_ptr));
    this.shader_files = new Map();

    let N = exports.shader_file_count(shader_ptr);
    for(let i = 0; i < N; i++)
    {
        const f = exports.shader_file(shader_ptr, i);
        const path = app.string(exports.shader_file_path(f));
        this.shader_files.set(path, {
            path: path,
            text: app.string(exports.shader_file_text(f)),
            type: exports.shader_file_type(f),
        });
    }

    this.defines = new Map();
    N = exports.shader_define_size(shader_ptr);
    for(let i = 0; i < N; i++)
    {
        const d = exports.shader_define(shader_ptr, i);
        this.defines.set(app.string(exports.define_name(d)),
                         app.string(exports.define_value(d)));
    }

    this.uniforms = new Map();
    N = exports.shader_uniform_size(shader_ptr);
    for(let i = 0; i < N; i++)
    {
        const u = exports.shader_uniform(shader_ptr, i);
        const uniform = this.app.uniform_at(u);
            
        this.uniforms.set(uniform.name, uniform);
    }

    this.attributes = new Map();
    N = exports.shader_attribute_size(shader_ptr);
    for(let i = 0; i < N; i++)
    {
        const a = exports.shader_attribute_def(shader_ptr, i);
        const att = new AttributeDef(
            app.string(exports.attribute_def_name(a)),
            exports.attribute_def_size(a),
            exports.attribute_def_type(a));
        this.attributes.set(att.name, att);
    }
    this.program = null;
    this.gl = null;
}

Shader.prototype.attribute_definition = function(name)
{
    return this.attributes.get(name);
};

Shader.prototype.uniform = function(uniform_name, base_type = "float")
{
    let u = this.uniforms.get(uniform_name);

    if(typeof u == "undefined")    
    {
        u = new Uniform(uniform_name, base_type);
        if(this.program != null)
            this.gl.getUniformLocation(this.program, uniform_name);

        this.uniforms.set(uniform_name, u);
    }

    return u;
}

Shader.prototype.uniform_location = function(uniform_name)
{
    return this.gl.getUniformLocation(this.program, uniform_name);
}

Shader.prototype.insert_defines = function(source) 
{
    let defs = ""
    this.defines.forEach((value, name) => 
        defs += `#define ${name} ${value}\n`);

    const version_regex = /^(#version\s.*)\n/g;
    return source.replace(version_regex, `$1\n\n${defs}\n`);
}

Shader.prototype.replace_includes = async function(source)
{
    const files_included = new Set();

    for(;;)
    {
        const include_regex = /#include\s+"([^\"]+)"/g;
        const matches = source.matchAll(include_regex);

        let match_count = 0;
        matches.forEach(m => {
            if(files_included.has(m[1]))
                throw new Error(`circular #include detected in path "${m[1]}"`);

            const u = new URL(m[1]); // throws exception if url is invalid
            if(u.hostname != window.location.hostname)
                throw new Error(`invalid url requested in #include "${m[1]}"`);

            files_included.add(m[1]);
            match_count++;
        });

        if(match_count == 0)
            break;

        // TODO: handle self-includes for inline shaders in wasm
        const promises = matches.map(m => fetch_text(m[1]));
        const files = await Promise.all(promises);

        let i = 0;
        source = source.replaceAll(include_regex, m => files[i++]);
    }

    return source;
}

Shader.prototype.load = async function()
{
    let promises = [], names = [], sources, compiled;

    this.shader_files.forEach(f => {
        if(typeof f.text === "string")
            promises.push(Promise.resolve(f.text));

         promises.push(fetch_text(f.path));
         names.push(f.path);
    });
    sources = await Promise.all(promises);

    promises = sources.map(source => this.replace_includes(source));
    sources = await Promise.all(promises);

    sources.forEach((source, index) => 
        this.shader_files.get(names[index]).text = this.insert_defines(source));
};

Shader.prototype.compile = function(gl)
{
    this.gl = gl;

    let compiled = []
    this.shader_files.forEach(f => 
        compiled.push(compile_shader(gl, f.type, f.text)));

    this.program = create_program(gl, compiled);

    this.uniforms.forEach(u => {
        u.location = gl.getUniformLocation(this.program, u.name);
    });

    this.attributes.forEach(a => {
        a.location = gl.getAttribLocation(this.program, a.name);
    });
};

Shader.prototype.use = function(gl)
{
    if(!this.program)
        this.compile(gl);

    gl.useProgram(this.program);

    this.uniforms.forEach(u => u.set(gl));
};


function bytes_from_element_type(gl, element_type)
{
    switch(element_type)
    {
    case gl.UNSIGNED_BYTE:
        return 1;
    case gl.UNSIGNED_SHORT:
        return 2;
    case gl.UNSIGNED_INT:
        return 4;
    }

    throw new Error(`unknown element type '${element_type}'`);
}

function uint8array_from_wasm(exports, ptr, len)
{
    return new Uint8Array(exports.memory.buffer, ptr, len);
}

function float32array_from_wasm(exports, ptr, len)
{
    return new Float32Array(exports.memory.buffer, ptr, len);
}
function int32array_from_wasm(exports, ptr, len)
{
    return new Int32Array(exports.memory.buffer, ptr, len);
}
function int16array_from_wasm(exports, ptr, len)
{
    return new Int16Array(exports.memory.buffer, ptr, len);
}
function string_from_wasm(exports, ptr, len)
{
    if(ptr === 0)
        return null;

    let dat = new Uint8Array(exports.memory.buffer, ptr, len);

    // TODO: make text decoder global
    return (new TextDecoder()).decode(dat);
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


async function fetch_text(url)
{
    const response = await fetch(url);

    if(!response.ok)
        throw new Error(`error fetching url ${url}: ${response.status}`);

    return await response.text();
}

async function create_program_from_shader_name(gl, shader_name, defines)
{
    return create_program_from_files(gl, 
        `shaders/${shader_name}.vertex`, `shaders/${shader_name}.fragment`,
        defines);
}

function insert_defines(source, defines) 
{
    let defs = ""
    for(const [name, value] of Object.entries(defines))
        defs += `#define ${name} ${value}\n`;

    const version_regex = /^(#version\s.*)\n/g;

    return source.replace(version_regex, `$1\n\n${defs}\n`);
}

async function create_program_from_files(gl, vertex_file, fragment_file, defines)
{   
    let vertex_source = await fetch_text(vertex_file);
    let fragment_source = await fetch_text(fragment_file);

    if(typeof defines == "object") 
    {
        vertex_source = insert_defines(vertex_source, defines);
        fragment_source = insert_defines(fragment_source, defines);
    }

    const vertex_shader = compile_shader(gl, gl.VERTEX_SHADER, vertex_source);
    const fragment_shader = compile_shader(gl, gl.FRAGMENT_SHADER, fragment_source);

    return create_program(gl, [vertex_shader, fragment_shader]);
}

function view_from(drawing_size, position)
{
    if(typeof position == "undefined")
        position = [0,0];
    
    return new Float32Array([
        2. / drawing_size[0],  0., -position[0] / drawing_size[0] - 1.,
        0., 2./ drawing_size[1],   -position[1] / drawing_size[1] - 1,
        0.,                   0.,  1.  
    ]);
}


function gl_uniform_is_matrix(base_type)
{
    return base_type.startsWith("mat");
}

function gl_uniform_method_for(base_type)
{
    const type_parser = /^(?:(bool|int|uint|float|double)|(([biud]?)(vec)([234]))|((mat)([234])(x[234])?))$/

    const r = type_parser.exec(base_type);
    
    if(!r)
        return null;

    if(r[7] == "mat")
        return `uniformMatrix${r[8]}fv`;

    if(r[4] == "vec")
        switch(r[3])
        {
        case 'b':
        case 'i':
        case 'u':
            return `uniform${r[5]}iv`;
        case 'd':
        default:
            return `uniform${r[5]}fv`;
        }

    if(r[1]) 
        switch(r[1])
        {
        case 'bool':
        case 'int':
        case 'uint':
            return `uniform1iv`;
        case 'float':
        case 'double':
            return `uniform1fv`;
        }

    return null;
}

async function load(mod = IMPORT) 
{
    const app = new App();

    await app.init(mod);

    app.continue_animate();
}
