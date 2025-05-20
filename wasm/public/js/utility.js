

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

function view_from(drawing_size, position, scale = 2.)
{
    if(typeof position == "undefined")
        position = [0,0];
    
    return new Float32Array([
        scale * 2. / drawing_size[0],  0., -scale * (position[0] / drawing_size[0] + 1.),
        0., -scale * 2./ drawing_size[1],   scale * (position[1] / drawing_size[1] + 1.),
        0.,                   0.,   1.  
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
