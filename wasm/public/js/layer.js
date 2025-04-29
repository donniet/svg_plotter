
/**
 * Layer class
 * */
function Layer(gl, program, name, draw_mode,
    arclength, strokelength, vertex_range, time_range, section_range,
    brush_size, brush_color,
    sections, uniforms) {
    this.name = name || DEFAULT_LAYER_NAME;
    this.draw_mode = draw_mode || gl[DEFAULT_DRAW_MODE.toUpperCase()];
    this.arclength = arclength || 1;
    this.vertex_range = vertex_range || [0, 0];
    this.time_range = time_range || [0, 1];
    this.section_range = section_range || [0, 1];
    this.brush_color = brush_color || [0., 0., 0., 1.];
    this.brush_size = brush_size || 10.;
    this.strokelength = strokelength || this.arclength / this.brush_size;
    this.uniforms = uniforms || {};
    this.sections = sections || [];

    this.render_buffer = null;
    this.width = 0;
    this.height = 0;

    // calculate the uv ranges
    this.uv_range_of = this.sections.map(function (s, i, sections) {
        // if this is the first section, our minimum arclength should be zero
        if (i == 0)
            return [-0.5, 0., 0.5, s.cumulative_arclength];

        // otherwise subtract 0.5 from the previous arclength as the minimum
        return [-0.5, sections[i - 1].cumulative_arclength, 0.5, s.cumulative_arclength];
    });

    // create functions to accept values for the uniforms in this layer
    this.uniform_functions = {};
    for (const [key, u] of Object.entries(this.uniforms))
        this.uniform_functions[key] = create_uniform_value_setter(gl, program, u);

    this.set_uv_range = create_uniform_value_setter(gl, program, UV_RANGE_UNIFORM_NAME);
    this.set_stroke_length = create_uniform_value_setter(gl, program, STROKE_LENGTH_UNIFORM_NAME);
    this.set_brush_size = create_uniform_value_setter(gl, program, BRUSH_SIZE_UNIFORM_NAME);
    this.set_brush_color = create_uniform_value_setter(gl, program, BRUSH_COLOR_UNIFORM_NAME);
}

/*
Layer.prototype.attach_render_buffer = function(gl, framebuffer_target)
{
if(this.render_buffer)
gl.framebufferRenderbuffer(framebuffer_target, gl.COLOR_ATTACHMENT0, 
 gl.RENDERBUFFER, this.render_buffer);
}

Layer.prototype.delete_render_buffer = function(gl)
{
if(!this.render_buffer)
return;

gl.deleteRenderbuffer(this.render_buffer);
this.render_buffer = null;
this.width = 0;
this.height = 0;
}

Layer.prototype.initialize_render_buffer = function(gl, width, height, format)
{
if(width != this.width || height != this.height)
this.delete_render_buffer(gl);

// if we have a render buffer, then the width and height must be correct, and we can simply return
if(this.render_buffer)
return;

// otherwise we need to create a new one
this.render_buffer = gl.createRenderbuffer();
gl.bindRenderbuffer(gl.RENDERBUFFER, this.render_buffer);
gl.renderbufferStorage(gl.RENDERBUFFER, format || gl.RGBA8, width, height);

this.width = width;
this.height = height;

gl.bindRenderbuffer(gl.RENDERBUFFER, null);
}
*/

Layer.prototype.vertex_range_of_section = function (section) {
    // if we are the first section, return the overall range up to the first section end
    if (section == 0)
        return [
            this.vertex_range[0],
            this.sections[0].end_vertex
        ];

    // otherwise return the end of the last section up to this section end
    return [
        this.sections[section - 1].end_vertex,
        this.sections[section].end_vertex
    ];
};


function less_section_length(a, b) {
    if (typeof a === "number")
        return a < b.cumulative_arclength;

    if (typeof b === "number")
        return a.cumulative_arclength < b;

    return a.cumulative_arclength < b.cumulative_arclength;
}

/**
* Layer::section_range method calculates the range of sections to draw
* based on the percentages passed into the range parameter
* 
* returns a range [begin, end) of indices into the sections member array
* */
Layer.prototype.section_range_of = function (range) {
    // calculate the arclengths that we should draw
    const s0 = this.strokelength * range[0];
    const s1 = this.strokelength * range[1];

    // begin at the lower bound of the lower arclength
    let begin = lower_bound(this.sections, s0, less_section_length); //, ...this.section_range);

    // end at the upper bound of the higher arclength
    let end = upper_bound(this.sections, s1, less_section_length); //, ...this.section_range);

    if (begin >= this.sections.length || begin > end)
        return [0, 0]; // empty range

    return [begin, Math.min(end + 1, this.sections.length)];
};

Layer.prototype.draw_section = function (gl, section, range, t) {
    // find the range of vertices in this section
    const [begin, end] = this.vertex_range_of_section(section);

    // lookup the uv range of this section
    const uv_range = this.uv_range_of[section];

    // setup our uv_range uniform
    this.set_uv_range([
        uv_range[0], uv_range[1],
        uv_range[2], uv_range[1] + t * (uv_range[3] - uv_range[1])
    ]);

    // set our arclength uniform
    this.set_stroke_length([this.sections[section].cumulative_arclength]);

    // draw the section
    gl.drawArrays(this.draw_mode,
        begin,
        end - begin);
};

Layer.prototype.draw = function (gl, range) {
    // calculate the range of sections we need to draw
    let [begin, end] = this.section_range_of(range);

    // nothing to draw!
    if (begin >= end)
        return;

    // calculate the arclength up until the last section
    let s0 = 0.;
    if (begin - end >= 2)
        s0 = this.sections[end - 2].cumulative_arclength;

    // calculate the maximum length we should be drawing based on this range
    let s1 = this.strokelength * range[1];

    // arclength of the current section only
    let s = this.sections[end - 1].cumulative_arclength - s0;

    // calculate the percentage of the final section we should draw
    // based on the range
    let ds = 1.;
    if (s > 0.001)
        ds = (s1 - s0) / s;



    // set our Layer specific uniforms like u_brush_color
    for (const [name, value] of Object.entries(this.uniforms))
        this.uniform_functions[name](value);

    this.set_brush_color(this.brush_color);
    this.set_brush_size([this.brush_size]);

    // we draw our sections in reverse, from the last to the first
    for (let e = end; begin < e; e--) {
        this.draw_section(gl, e - 1, range, ds);

        // the next section will not be the last, so we should draw the whole thing
        ds = 1.;
    }
};
