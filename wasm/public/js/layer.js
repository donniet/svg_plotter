
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

        this.uniforms.set(uni.name, uni);
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
