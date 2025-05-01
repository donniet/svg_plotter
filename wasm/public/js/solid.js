
function Solid(solids, index)
{
    this.__solids = solids;
    this.__index = index;

    const stride = this.__solids.__exports.get_solid_stride();

    this.__attributes = [];
    for(let i = 0; i < this.__solids.__exports.attribute_count(); i++)
    {
        const name = this.__solids.get_attribute_name(i);
        const size = this.__solids.__exports.attribute_size(i);
        const offset = this.__solids.__exports.attribute_offset(i);

        this.__attributes.push(new Attribute(name, size, null, stride, offset));
    }

    this.__shader = null;

    this.__vertex_data = new Float32Array(
        solids.__buf,
        solids.__exports.get_solid_buffer(),
        solids.__exports.get_solid_buffer_size(),
    );
    // this.__painter = null;
}
Solid.prototype.set_shader = function(shader)
{
    this.__shader = shader;
};
Solid.prototype.name = function()
{
    return this.__solids.get_solid_name(this.__index);
};
Solid.prototype.shader_name = function()
{
    return this.__solids.get_solid_shader_name(this.__index);
};
Solid.prototype.draw_mode = function()
{
    return this.__solids.__exports.get_solid_draw_mode(this.__index);
};
Solid.prototype.vertex_range = function()
{
    const vertex_start = this.__solids.__exports.get_solid_vertex_start(this.__index);
    const vertex_end = this.__solids.__exports.get_solid_vertex_end(this.__index);

    return [ vertex_start, vertex_end ];
};
Solid.prototype.time_range = function()
{
    const time_start = this.__solids.__exports.get_solid_time_start(this.__index);
    const time_end = this.__solids.__exports.get_solid_time_end(this.__index);

    return [ time_start, time_end ];
};
Solid.prototype.stride = function()
{
    return this.__solids.__exports.get_solid_stride();
};
Solid.prototype.vertex_data = function()
{
    return this.__vertex_data;
};
Solid.prototype.attributes = function()
{
    return this.__attributes;
};

Solid.prototype.draw = function(gl, time)
{
    const time_range = this.time_range();

    let t = (time - time_range[0]) / (time_range[1] - time_range[0]);

    if(t < 0)
        return;

    if(t > 1) 
        t = 1.;

    // if(ENABLE_BLEND_PROGRAM)
    //     gl.clearColor(...this.clear_color);
    //     gl.clear(gl.COLOR_BUFFER_BIT);

    const animation_range = [0, t];

    // add the range uniform based on this layer's time range
    this.__shader.uniform(RANGE_UNIFORM_NAME).float(animation_range);

    // TODO: add additional uniforms from wasm

    // enable attributes
    // NOTE: this will be handled by the vao in the painter object
    //this.enable_attributes();

    const [begin, end] = this.vertex_range(animation_range);

    gl.drawArrays(this.draw_mode(), begin, end);
};


/**
 * Solids class
 */
function Solids(instance)
{
    this.__instance = instance;
    this.__exports = instance.exports;
    this.__buf = instance.exports.memory.buffer;

    this.__dec = new TextDecoder();

    this.stride = this.__exports.get_solid_stride();
    
    this.each = function(visit) {
        for(let i = 0; i < this.__exports.get_solid_count(); i++) 
            visit(new Solid(this, i));
    }.bind(this);
}

Solids.prototype.string_from_buffer = function(ptr, len)
{
    let dat = new Uint8Array(this.__buf, ptr, len);
    return this.__dec.decode(dat);
};
Solids.prototype.float32_array_from_buffer = function(ptr, len)
{
    return new Float32Array(this.__buf, ptr, len);
};
Solids.prototype.vertex_data = function()
{
    return new Float32Array(this.__buf, 
        this.__exports.get_solid_buffer(), this.__exports.get_solid_buffer_size())
};
Solids.prototype.get_solid_name = function(i)
{
    return this.string_from_buffer(
        this.__exports.get_solid_name_ptr(i),
        this.__exports.get_solid_name_length(i)
    );
};
Solids.prototype.get_solid_shader_name = function(i)
{
    return this.string_from_buffer(
        this.__exports.get_solid_shader_ptr(i),
        this.__exports.get_solid_shader_length(i)
    );
};
Solids.prototype.get_attribute_name = function(i)
{
    return this.string_from_buffer(
        this.__exports.attribute_name_ptr(i),
        this.__exports.attribute_name_length(i)
    );
};


