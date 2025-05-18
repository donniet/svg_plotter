// import Blend from "blend"
// import Uniform from "uniform"


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

    this.viewport_ = [0, 0, this.dimensions[0], this.dimensions[1]];

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

Drawing.prototype.viewport = function(x0, y0, x1, y1)
{
    this.viewport_ = [x0, y0, x1, y1];
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

    gl.viewport(this.viewport_[0], this.viewport_[1], 
        this.viewport_[2], this.viewport_[3]);

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

