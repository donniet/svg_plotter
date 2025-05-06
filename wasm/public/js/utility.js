

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

async function create_program_from_shader_name(gl, shader_name)
{
    return create_program_from_files(gl, 
        `shaders/${shader_name}.vertex`, `shaders/${shader_name}.fragment`);
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
    // enable this attribute's location in the vao
    gl.enableVertexAttribArray(location);

    // setup the vertex attribute pointer
    gl.vertexAttribPointer(location, size, gl.FLOAT, false, stride, offset);
};

function view_from(drawing_size)
{
    return new Float32Array([
        2./ drawing_size[0],  0., -1.,
        0., 2./ drawing_size[1],  -1,
        0.,                   0.,  1.  
    ]);
}

function string_from_buffer(buf, ptr, len)
{
    let dat = new Uint8Array(buf, ptr, len);
    return new TextDecoder().decode(dat);
};
function float32_array_from_buffer(buf, ptr, len)
{
    return new Float32Array(buf, ptr, len);
};
function int32_array_from_buffer(buf, ptr, len)
{
    return new Int32Array(buf, ptr, len);
};

async function create_shader(gl, shader_name, uniforms)
{
    const vert = await fetch_text(`shaders/${shader_name}.vertex`);
    const frag = await fetch_text(`shaders/${shader_name}.fragment`);
    
    return new Shader(gl, [vert, frag], uniforms);
}

function uniform_method_for(base_type)
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

function uniform_setter_for(base_type)
{
    const method = uniform_method_for(base_type);

    if(method.includes("Matrix"))
        return  function(gl, location, array) {
                    gl[method](location, false, array);
                };
    else
        return  function(gl, location, array) {
                    gl[method](location, array);
                };

    
}

/**
 * Uniform class stores the meta-data of a uniform variable to a shader
 * @param gl : WebGL2Context
 * @param uniform_name : String
 * @param location : int is the location id of this uniform in the shader
 * @param size : int is the vector size of this uniform (ie: 4 for vec4)
 */
function Uniform(gl, uniform_name, location, base_type = "float")
{   
    this.name = uniform_name;
    this.location = location; 
    this.base_type = base_type;

    this.__uniform_setter = uniform_setter_for(base_type)
        .bind(this, gl, location);
    
}

Uniform.prototype.set = function(v) {
    if(typeof v === "undefined")
        return;

    if(!this.exists())
        return;
    
    if(typeof v === "number")
    {
        this.__uniform_setter([v]);
        return;
    }
    
    if(typeof v === "object" && typeof v.length === "number")
    {
        this.__uniform_setter(v);
        return;
    }

    console.log(`unknown uniform type for '${this.name}' for value '${v}'`)
};

Uniform.prototype.exists = function(v) {
    return !!this.location;
};

function make_setter_getter(this_object, name, default_value)
{
    let value = default_value;

    this_object[name] = function(v) {
        if(typeof v === "undefined")
            return value;

        value = v;
        return this_object;
    };
}

function Attribute(attribute_name, size, type, stride, offset, normalized)
{
    this._name = attribute_name;
    this._size = size;
    this._type = type;
    this._type = type;
    this._stride = stride;
    this._offset = offset;
    this._normalized = !!normalized;
}
Attribute.prototype.name = function() 
{
    return this._name;
};
Attribute.prototype.size = function()
{
    return this._size;
};
Attribute.prototype.type = function()
{
    return this._type;
};
Attribute.prototype.stride = function()
{
    return this._stride;
};
Attribute.prototype.offset = function()
{
    return this._offset;
};
Attribute.prototype.normalized = function()
{
    return this._normalized;
};




function Shader(gl, sources, uniforms)
{ 
    this._sources = sources;
    this._program = null;
    this._defines = {};
    this._uniforms = {};
    this.__gl = gl;
    

    this._attributes = {};
    this._attribute_locations = {};
}
Shader.prototype.define = function(name, value)
{
    this._defines[name] = value;
};
Shader.prototype.uniform = function(uniform_name, base_type = "float")
{
    let u = this._uniforms[uniform_name];

    if(typeof u !== "undefined")
        return u;

    const location = this.__gl.getUniformLocation(this._program, uniform_name);

    if(!location)
        console.log(`uniform ${uniform_name} is not available in program ${this._program}`);

    u = this._uniforms[uniform_name] = new Uniform(this.__gl, uniform_name, location, base_type);

    return u;
};
Shader.prototype.insert_defines = function(source) 
{
    let defs = ""
    for(const [name, value] of Object.entries(this._defines))
        defs += `#define ${name} ${value}\n`;

    const version_regex = /^(#version\s.*)\n/g;

    return source.replace(version_regex, `$1\n\n${defs}\n`);
}
Shader.prototype.compile = function(gl)
{
    const shader_types = [ gl.VERTEX_SHADER, gl.FRAGMENT_SHADER ];

    const compiled_shaders = [];

    for(let i = 0; i < 2; i++)
    {
        const with_defines = this.insert_defines(this._sources[i]);

        compiled_shaders.push(compile_shader(gl, shader_types[i], with_defines));
    }

    this._program = create_program(gl, compiled_shaders);
};
Shader.prototype.use = function(gl)
{
    if(!this._program)
        this.compile(gl);

    gl.useProgram(this._program);
}
Shader.prototype.enable_attribute = function(gl, attribute)
{
    const location = gl.getAttribLocation(this._program, attribute.name());
    
    // enable this attribute's location in the vao
    gl.enableVertexAttribArray(location);

    // setup the vertex attribute pointer
    gl.vertexAttribPointer(location, 
        attribute.size(), attribute.type() || gl.FLOAT, attribute.normalized(), 
        attribute.stride(), attribute.offset());
}