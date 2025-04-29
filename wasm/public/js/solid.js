
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

    this.solids = [];

    for(let i = 0; i < this.__exports.get_solid_count(); i++)
    {
        const name = this.get_solid_name(i);
        const draw_mode = this.__exports.get_solid_draw_mode(i);
        const vertex_start = this.__exports.get_solid_vertex_start(i);
        const vertex_end = this.__exports.get_solid_vertex_end(i);
        const time_start = this.__exports.get_solid_time_start(i);
        const time_end = this.__exports.get_solid_time_end(i);
        const shader = this.get_solid_shader(i);

        this.solids.push({
            name: name, 
            shader: shader,
            draw_mode: draw_mode, 
            vertex_range: [ vertex_start, vertex_end ], 
            time_range: [ time_start, time_end ],
            stride: this.stride, 
            buffer: this.__buf,
        });
    }
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
Solids.prototype.get_solid_shader = function(i)
{
    return this.string_from_buffer(
        this.__exports.get_solid_shader_ptr(i),
        this.__exports.get_solid_shader_length(i)
    );
};

