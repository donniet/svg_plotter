

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
};
Mesh.prototype.float32_array_from_buffer = function(ptr, len)
{
    return new Float32Array(this.__buf, ptr, len);
};
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
