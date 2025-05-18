
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

Shader.prototype.load_shader = function(shader_path)
{
    return this.app.fetch_shader(shader_path);
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
        const includes = [];

        let match_count = 0;
        matches.forEach(m => {
            if(files_included.has(m[1]))
                throw new Error(`circular #include detected in path "${m[1]}"`);

            /* removing this check for now because rel urls don't seem to parse

            const u = new URL(m[1]); // throws exception if url is invalid
            if(u.hostname != window.location.hostname)
                throw new Error(`invalid url requested in #include "${m[1]}"`);
            */


            files_included.add(m[1]);
            includes.push(m[1]);
            match_count++;
        });

        if(match_count == 0)
            break;

        // TODO: handle self-includes for inline shaders in wasm
        const promises = includes.map(inc => this.load_shader(inc));
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

         promises.push(this.load_shader(f.path));
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