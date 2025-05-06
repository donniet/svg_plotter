
function Buffer(buf, buffer_data, target, usage)
{
    this._buf = buf;
    this._buffer_data = buffer_data;
    this._target = target;
    this._usage = usage;
}
Buffer.prototype.data = function()
{
    return this._buffer_data;
}
Buffer.prototype.bind = function(gl)
{
    gl.bindBuffer(this._target || gl.ARRAY_BUFFER, this._buf);
};
Buffer.prototype.load = function(gl)
{
    gl.bindBuffer(this._target || gl.ARRAY_BUFFER, this._buf);
    gl.bufferData(this._target || gl.ARRAY_BUFFER, this._buffer_data, this._usage || gl.STATIC_DRAW);
};

/**
 * Painter class sets up webgl environment and renders layers
 */
function Painter(gl, drawing_size)
{ 
    this.drawing_size = drawing_size || DEFAULT_DRAWING_SIZE;
    this.aspect_ratio = this.drawing_size[0] / this.drawing_size[1];
    this.view_matrix = view_from(this.drawing_size);

    this.layers = [];
    this.vaos = [];
    this.buffers = [];

    this.filters = [];
    this.shaders = {};


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


/**
 * add_layer method adds a layer to the top of the layer stack
 * the `layer` must parameter must have the following methods
 * 
 * layer.compile_shader(gl) : String
 * layer.shader() : Shader
 * layer.draw(gl : WebGL2RenderingContext, time : Number)
 * layer.vertex_attribute_object() : WebGL2VertexAttriuteObject
 * 
 */
Painter.prototype.add_layer = async function(gl, layer)
{ 
    this.layers.push(layer);

    await layer.compile_shader(gl);

    return this;
}
// Painter.prototype.add_filter = function(gl, filter)
// { }
Painter.prototype.render = function(gl, time)
{ 
    let resized = this.resize_canvas(gl, 
        document.body.clientWidth, 
        document.body.clientHeight);

    // if(resized)
    gl.viewport(...this.viewport_dimensions);

    gl.clearColor(0.8, 0.8, 0.8, 1.);
    gl.clear(gl.COLOR_BUFFER_BIT);

    this.layers.forEach((layer, layer_index) => {
        const shader = layer.shader();

        // use this program
        shader.use(gl);

        // bind the vao
        // DT: moved to the layer objects
        // gl.bindVertexArray(vao);

        // set the values of our basic uniforms
        shader.uniform(VIEW_UNIFORM_NAME, "mat3").set(this.view_matrix);
        shader.uniform(RESOLUTION_UNIFORM_NAME, "vec2").set([this.viewport_dimensions[2], this.viewport_dimensions[3]]);
        shader.uniform(MOUSE_UNIFORM_NAME, "vec3").set(this.__mouse);
        shader.uniform(MOUSE_STATE_UNIFORM_NAME, "uint").set([this.__mouse_state]);
        shader.uniform(TIME_UNIFORM_NAME, "float").set([time]);

        // ask the layer to draw
        layer.draw(gl, time);
    });

    /*
    this.filters.forEach(function(filter) {
        filter.draw(gl, time, uniforms);
    });
    */
   
    requestAnimationFrame(Painter.prototype.render.bind(this, gl));
}
Painter.prototype.continue_animate = function(gl)
{ 
    requestAnimationFrame(Painter.prototype.render.bind(this, gl));
}
Painter.prototype.mouse_handler = function(mouse_action_id, event)
{
    this.__mouse[0]    = event.clientX;
    this.__mouse[1]    = event.clientY;
    this.__mouse[2]   += (typeof event.deltaY === "number" ? event.deltaY : 1);
    // use Gödel numbering to send both the button and the action in one value
    this.__mouse_state = Math.pow(2, event.button) * Math.pow(3, mouse_action_id);
};
Painter.prototype.resize_canvas = function(gl, clientWidth, clientHeight)
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

